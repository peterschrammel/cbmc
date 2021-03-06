/*******************************************************************\

Module:

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#include "merge_irep.h"

#include "irep_hash.h"

std::size_t to_be_merged_irept::hash() const
{
  std::size_t result=hash_string(id());

  const irept::subt &sub=get_sub();
  const irept::named_subt &named_sub=get_named_sub();

  for(const auto &irep : sub)
  {
    result =
      hash_combine(result, static_cast<const merged_irept &>(irep).hash());
  }

  for(const auto &irep_entry : named_sub)
  {
    result = hash_combine(result, hash_string(irep_entry.first));
    result = hash_combine(
      result, static_cast<const merged_irept &>(irep_entry.second).hash());
  }

  const std::size_t named_sub_size = named_sub.size();
  result = hash_finalize(result, named_sub_size + sub.size());

  return result;
}

bool to_be_merged_irept::operator == (const to_be_merged_irept &other) const
{
  if(id()!=other.id())
    return false;

  const irept::subt &sub=get_sub();
  const irept::subt &o_sub=other.get_sub();
  const irept::named_subt &named_sub=get_named_sub();
  const irept::named_subt &o_named_sub=other.get_named_sub();

  if(sub.size()!=o_sub.size())
    return false;
#if NAMED_SUB_IS_FORWARD_LIST
  if(
    std::distance(named_sub.begin(), named_sub.end()) !=
    std::distance(o_named_sub.begin(), o_named_sub.end()))
  {
    return false;
  }
#else
  if(named_sub.size()!=o_named_sub.size())
    return false;
#endif

  {
    irept::subt::const_iterator s_it=sub.begin();
    irept::subt::const_iterator os_it=o_sub.begin();

    for(; s_it!=sub.end(); s_it++, os_it++)
      if(static_cast<const merged_irept &>(*s_it)!=
         static_cast<const merged_irept &>(*os_it))
        return false;
  }

  {
    irept::named_subt::const_iterator s_it=named_sub.begin();
    irept::named_subt::const_iterator os_it=o_named_sub.begin();

    for(; s_it!=named_sub.end(); s_it++, os_it++)
      if(s_it->first!=os_it->first ||
         static_cast<const merged_irept &>(s_it->second)!=
         static_cast<const merged_irept &>(os_it->second))
        return false;
  }

  return true;
}

const merged_irept &merged_irepst::merged(const irept &irep)
{
  // first see if it's already a merged_irep

  merged_irep_storet::const_iterator entry=
    merged_irep_store.find(merged_irept(irep));

  if(entry!=merged_irep_store.end())
    return *entry; // nothing to do

  // need to build new irep that will be in the container
  irept new_irep(irep.id());

  const irept::subt &src_sub=irep.get_sub();
  irept::subt &dest_sub=new_irep.get_sub();
  dest_sub.reserve(src_sub.size());

  for(const auto &sub_irep : src_sub)
    dest_sub.push_back(merged(sub_irep)); // recursive call

  const irept::named_subt &src_named_sub=irep.get_named_sub();
  irept::named_subt &dest_named_sub=new_irep.get_named_sub();

#if NAMED_SUB_IS_FORWARD_LIST
  irept::named_subt::iterator before = dest_named_sub.before_begin();
#endif
  for(const auto &irep_entry : src_named_sub)
  {
#if NAMED_SUB_IS_FORWARD_LIST
    dest_named_sub.emplace_after(
      before, irep_entry.first, merged(irep_entry.second)); // recursive call
    ++before;
#else
    dest_named_sub[irep_entry.first] =
      merged(irep_entry.second); // recursive call
#endif
  }

  std::pair<to_be_merged_irep_storet::const_iterator, bool> result=
    to_be_merged_irep_store.insert(to_be_merged_irept(new_irep));

  if(result.second) // really new, record
    merged_irep_store.insert(merged_irept(new_irep));

  return
    static_cast<const merged_irept &>(
      static_cast<const irept &>(*result.first));
}

void merge_irept::operator()(irept &irep)
{
  // only useful if there is sharing
  #ifdef SHARING
  irep=merged(irep);
  #endif
}

const irept &merge_irept::merged(const irept &irep)
{
  auto entry = irep_store.insert(irep);
  if(!entry.second)
    return *entry.first;

  const irept::subt &src_sub=irep.get_sub();
  irept::subt *dest_sub_ptr = nullptr;

  std::size_t index = 0;
  for(const auto &sub_irep : src_sub)
  {
    const irept &op = merged(sub_irep); // recursive call
    if(&op.read() != &(sub_irep.read()))
    {
      if(!dest_sub_ptr)
        dest_sub_ptr = &(const_cast<irept &>(*entry.first)).get_sub();
      (*dest_sub_ptr)[index] = op;
    }
    ++index;
  }

  const irept::named_subt &src_named_sub=irep.get_named_sub();
  irept::named_subt *dest_named_sub_ptr = nullptr;

  std::ptrdiff_t advance_by = 0;
  for(const auto &irep_entry : src_named_sub)
  {
    if(!irept::is_comment(irep_entry.first))
    {
      const irept &op = merged(irep_entry.second); // recursive call
      if(&op.read() != &(irep_entry.second.read()))
      {
        if(!dest_named_sub_ptr)
          dest_named_sub_ptr =
            &(const_cast<irept &>(*entry.first)).get_named_sub();
        std::next(dest_named_sub_ptr->begin(), advance_by)->second = op;
      }
    }
    ++advance_by;
  }

  return *entry.first;
}

void merge_full_irept::operator()(irept &irep)
{
  // only useful if there is sharing
  #ifdef SHARING
  irep=merged(irep);
  #endif
}

const irept &merge_full_irept::merged(const irept &irep)
{
  irep_storet::const_iterator entry=irep_store.find(irep);
  if(entry!=irep_store.end())
    return *entry;

  irept new_irep(irep.id());

  const irept::subt &src_sub=irep.get_sub();
  irept::subt &dest_sub=new_irep.get_sub();
  dest_sub.reserve(src_sub.size());

  for(const auto &sub_irep : src_sub)
    dest_sub.push_back(merged(sub_irep)); // recursive call

  const irept::named_subt &src_named_sub=irep.get_named_sub();
  irept::named_subt &dest_named_sub=new_irep.get_named_sub();

#if NAMED_SUB_IS_FORWARD_LIST
  irept::named_subt::iterator before = dest_named_sub.before_begin();
#endif
  for(const auto &irep_entry : src_named_sub)
  {
#if NAMED_SUB_IS_FORWARD_LIST
    dest_named_sub.emplace_after(
      before, irep_entry.first, merged(irep_entry.second)); // recursive call
    ++before;
#else
    dest_named_sub[irep_entry.first] =
      merged(irep_entry.second); // recursive call
#endif
  }

  return *irep_store.insert(std::move(new_irep)).first;
}
