/// \file
/// LazyCSeq context-bounded concurrency SSA transformation

#include "lazy_c_seq.h"

#include <util/cprover_prefix.h>
#include <util/format.h>
#include <util/format_expr.h>
#include <util/pointer_expr.h>
#include <util/prefix.h>

#include <optional>

void lazy_c_seqt::operator()(
  symex_target_equationt &equation,
  message_handlert &message_handler)
{
  messaget log{message_handler};
  log.statistics() << "Adding LazyCSeq constraints with " << rounds << " rounds"
                   << messaget::eom;

  std::vector<std::pair<
    symex_target_equationt::SSA_stepst::const_iterator,
    std::optional<symex_target_equationt::SSA_stepst::const_iterator>>>
    reads;
  std::unordered_map<
    unsigned,
    std::vector<symex_target_equationt::SSA_stepst::const_iterator>>
    writes;
  std::unordered_map<irep_idt, irep_idt> last_update;
  std::unordered_map<irep_idt, irep_idt> last_update_main;
  collect_reads_and_writes(equation.SSA_steps, reads, writes, message_handler);

  create_write_constraints(
    equation, writes, last_update, last_update_main, message_handler);

  create_read_constraints(
    equation, reads, last_update, last_update_main, message_handler);

  exprt tmp;
  simplify(tmp, ns);
}

void lazy_c_seqt::create_write_constraints(
  symex_target_equationt &equation,
  const std::unordered_map<
    unsigned,
    std::vector<symex_target_equationt::SSA_stepst::const_iterator>> &writes,
  std::unordered_map<irep_idt, irep_idt> &last_update,
  std::unordered_map<irep_idt, irep_idt> &last_update_main,
  message_handlert &message_handler)
{
  messaget log{message_handler};

  log.warning() << "-------------------WRITES--------------------------"
                << messaget::eom;

  // last write of main thread
  exprt last_write_of_previous_round;
  std::unordered_set<irep_idt> global_variables;
  for(const auto &s_it : writes.at(0))
  {
    global_variables.insert(s_it->ssa_lhs.get_object_name());
  }

  for(irep_idt variable : global_variables)
  {
    for(const auto &s_it : writes.at(0))
    {
      if(s_it->ssa_lhs.get_object_name() == variable)
      {
        last_write_of_previous_round = s_it->ssa_lhs;
        last_update_main[variable] = id2string(
          to_symbol_expr(last_write_of_previous_round).get_identifier());
      }
    }

    for(std::size_t round = 1; round <= rounds; ++round)
    {
      symex_target_equationt::SSA_stepst::const_iterator
        last_write_of_current_round;
      for(unsigned thread_nr = 1; thread_nr < writes.size(); ++thread_nr)
      {
        bool var_contained = false;
        for(const auto &s_it : writes.at(thread_nr))
        {
          if(s_it->ssa_lhs.get_object_name() == variable)
          {
            var_contained = true;
            last_write_of_current_round = s_it;
          }
        }
        if(!var_contained)
          continue;
        std::string suffix =
          "_L" +
          std::to_string(
            last_write_of_current_round->source.pc->location_number) +
          "_R" + std::to_string(round);
        irep_idt statement_label_name = "E" + suffix;
        symbol_exprt statement_label{statement_label_name, bool_typet{}};

        // We don't need to check that this is a symbol because we have alread done that in collect_reads_and_writes.
        irep_idt end_of_round_name =
          id2string(to_symbol_expr(last_write_of_current_round->ssa_lhs)
                      .get_identifier()) +
          suffix;
        symbol_exprt end_of_round_value{
          end_of_round_name, last_write_of_current_round->ssa_lhs.type()};
        equal_exprt constraint{
          end_of_round_value,
          if_exprt{
            statement_label,
            last_write_of_current_round->ssa_lhs,
            last_write_of_previous_round}};
        log.warning() << format(constraint) << messaget::eom;
        equation.constraint(
          constraint, "write constraint", last_write_of_current_round->source);
        last_write_of_previous_round = end_of_round_value;

        last_update[variable] =
          id2string(to_symbol_expr(last_write_of_current_round->ssa_lhs)
                      .get_identifier()) +
          "_L" +
          std::to_string(
            last_write_of_current_round->source.pc->location_number);
      }
    }
  }
}

void lazy_c_seqt::create_read_constraints(
  symex_target_equationt &equation,
  const std::vector<std::pair<
    symex_target_equationt::SSA_stepst::const_iterator,
    std::optional<symex_target_equationt::SSA_stepst::const_iterator>>> &reads,
  std::unordered_map<irep_idt, irep_idt> &last_update,
  std::unordered_map<irep_idt, irep_idt> &last_update_main,
  message_handlert &message_handler)
{
  messaget log{message_handler};
  log.warning() << "-------------------READS--------------------------"
                << messaget::eom;

  for(const auto &read : reads)
  {
    std::vector<std::pair<exprt, exprt>> constraints;
    auto read_variable = read.first;
    for(std::size_t round = rounds; round >= 1; --round)
    {
      std::string condition_suffix =
        "_L" + std::to_string(read_variable->source.pc->location_number) +
        "_R" + std::to_string(round);
      irep_idt statement_label_name = "E" + condition_suffix;
      symbol_exprt statement_label{statement_label_name, bool_typet{}};

      std::string variable_suffix;
      irep_idt variable_round_name;
      typet type = read.first->ssa_lhs.type();

      if(!read.second.has_value())
      {
        if(round == 1)
        {
          //log.warning() << "DEFAULT VALUE:" << messaget::eom;
          variable_round_name = id2string(
            last_update_main[read.first->ssa_lhs.get_original_name()]);
        }
        else
        {
          //log.warning() << "VALUE FROM PREVIOUS ROUND:" << messaget::eom;
          variable_round_name =
            id2string(last_update[read.first->ssa_lhs.get_original_name()]) +
            "_R" + std::to_string(round - 1);
        }
      }
      else
      {
        //log.warning() << "VALUE FROM CURRENT ROUND:" << messaget::eom;
        variable_suffix =
          "_L" +
          std::to_string(read.second.value()->source.pc->location_number) +
          "_R" + std::to_string(round);

        variable_round_name =
          id2string(
            to_symbol_expr(read.second.value()->ssa_lhs).get_identifier()) +
          variable_suffix;

        type = read.second.value()->ssa_lhs.type();
      }

      symbol_exprt round_value{variable_round_name, type};

      constraints.emplace_back(std::pair(statement_label, round_value));
    }

    exprt previous_expr = read_variable->ssa_lhs;
    exprt constraint;
    for(const auto &pair : constraints)
    {
      if_exprt temp_constraint{pair.first, pair.second, previous_expr};
      constraint = temp_constraint;
      previous_expr = constraint;
    }
    equal_exprt final_constraint{read_variable->ssa_lhs, constraint};
    log.warning() << format(final_constraint) << messaget::eom;
    equation.constraint(
      final_constraint, "read constraint", read_variable->source);
  }
}

void lazy_c_seqt::collect_reads_and_writes(
  const symex_target_equationt::SSA_stepst &ssa_steps,
  std::vector<std::pair<
    symex_target_equationt::SSA_stepst::const_iterator,
    std::optional<symex_target_equationt::SSA_stepst::const_iterator>>> &reads,
  std::unordered_map<
    unsigned,
    std::vector<symex_target_equationt::SSA_stepst::const_iterator>> &writes,
  message_handlert &message_handler)
{
  messaget log{message_handler};
  std::unordered_map<
    irep_idt,
    std::optional<symex_target_equationt::SSA_stepst::const_iterator>>
    previous_write;
  previous_write.clear();
  for(symex_target_equationt::SSA_stepst::const_iterator s_it =
        ssa_steps.begin();
      s_it != ssa_steps.end();
      s_it++)
  {
    const std::string &file =
      id2string(s_it->source.pc->source_location().get_file());
    if(
      file.find("builtin-library") != std::string::npos ||
      file.find("built-in-additions") != std::string::npos)
    {
      continue;
    }
    if(s_it->is_shared_read() || s_it->is_shared_write())
    {
      const bool has_cprover_prefix =
        can_cast_expr<symbol_exprt>(s_it->ssa_lhs) &&
        has_prefix(
          id2string(to_symbol_expr(s_it->ssa_lhs).get_identifier()),
          CPROVER_PREFIX);
      if(has_cprover_prefix)
      {
        continue;
      }
    }
    if(s_it->is_shared_read() || s_it->is_shared_write())
    {
      if(can_cast_expr<symbol_exprt>(s_it->ssa_lhs))
      {
        const typet &type = to_symbol_expr(s_it->ssa_lhs).type();
        //log.warning() << "Type of " << to_symbol_expr(s_it->ssa_lhs).get_identifier() << ": " << type.pretty() << messaget::eom;
        if(
          can_cast_type<pointer_typet>(type) &&
          can_cast_type<struct_tag_typet>(to_pointer_type(type).base_type()))
        {
          if(
            id2string(to_struct_tag_type(to_pointer_type(type).base_type())
                        .get_identifier()) == "tag-_opaque_pthread_t")
          {
            //log.warning() << "Skipped" << messaget::eom;
            continue;
          }
        }
      }
    }
    //log.warning() << "Not skipped" << messaget::eom;

    /*
    if(s_it->is_assignment() && s_it->source.pc->is_assign()) {
      const exprt &lhs = to_code_assign(s_it->source.pc->code()).lhs();
      const bool has_cprover_prefix = can_cast_expr<symbol_exprt>(lhs) &&
                                      has_prefix(id2string(to_symbol_expr(lhs).get_identifier()), CPROVER_PREFIX);
      if(has_cprover_prefix) {
        continue;
      }
    }*/

    if(s_it->is_shared_write())
    {
      // TODO: this may be too restrictive
      if(can_cast_expr<symbol_exprt>(s_it->ssa_lhs))
      {
        log.warning() << "Thread: " << s_it->source.thread_nr
                      << "\tWrite: " << s_it->source.pc->location_number
                      << "   \t"
                      << to_symbol_expr(s_it->ssa_lhs).get_identifier() << "\tL"
                      << s_it->source.pc->location_number << messaget::eom;
        writes[s_it->source.thread_nr].emplace_back(s_it);
        if(previous_write.count(s_it->ssa_lhs.get_object_name()) == 0)
          previous_write[s_it->ssa_lhs.get_object_name()] = std::nullopt;
        else
          previous_write[s_it->ssa_lhs.get_object_name()] = s_it;
      }
      else
      {
        log.warning() << "Skipping: "
                      << "Thread: " << s_it->source.thread_nr
                      << "\tWrite: " << s_it->source.pc->location_number
                      << messaget::eom;
      }
    }
    if(s_it->is_shared_read())
    {
      // TODO: this may be too restrictive
      if(can_cast_expr<symbol_exprt>(s_it->ssa_lhs))
      {
        log.warning() << "Thread: " << s_it->source.thread_nr
                      << "\tRead: " << s_it->source.pc->location_number
                      << "     \t"
                      << to_symbol_expr(s_it->ssa_lhs).get_identifier() << "\tL"
                      << s_it->source.pc->location_number << messaget::eom;

        if(s_it->source.thread_nr >= 1) //TODO: maybe we don't need this later
        {
          if(previous_write.count(s_it->ssa_lhs.get_object_name()) == 0)
            reads.emplace_back(std::pair(s_it, std::nullopt));
          else
          {
            reads.emplace_back(
              std::pair(s_it, previous_write[s_it->ssa_lhs.get_object_name()]));
          }
        }
      }
      else
      {
        log.warning() << "Skipping: "
                      << "Thread: " << s_it->source.thread_nr
                      << "\tWrite: " << s_it->source.pc->location_number
                      << messaget::eom;
      }
    }
  }
}