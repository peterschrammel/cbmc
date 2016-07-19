/*******************************************************************\

Module:

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#include <algorithm>
#include <set>
#include <iostream>

#include <util/prefix.h>
#include <util/std_types.h>
#include <util/std_code.h>
#include <util/std_expr.h>
#include <util/expr_util.h>
#include <util/cprover_prefix.h>
#include <util/message.h>
#include <util/config.h>
#include <util/namespace.h>
#include <util/pointer_offset_size.h>
#include <util/i2string.h>
#include <util/prefix.h>

#include <goto-programs/goto_functions.h>

#include "java_entry_point.h"

//#include "zero_initializer.h"

#define INITIALIZE CPROVER_PREFIX "initialize"

/*******************************************************************\

Function: create_initialize

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

namespace {
void create_initialize(symbol_tablet &symbol_table)
{
  symbolt initialize;
  initialize.name=INITIALIZE;
  initialize.base_name=INITIALIZE;
  initialize.mode=ID_java;

  code_typet type;
  type.return_type()=empty_typet();
  initialize.type=type;
  
  code_blockt init_code;
  
  namespacet ns(symbol_table);
  
  symbol_exprt rounding_mode=
    ns.lookup(CPROVER_PREFIX "rounding_mode").symbol_expr();

  init_code.add(code_assignt(rounding_mode, gen_zero(rounding_mode.type())));
  
  initialize.value=init_code;
  
  if(symbol_table.add(initialize))
    throw "failed to add "+std::string(INITIALIZE);
}
}

/*******************************************************************\

Function: gen_nondet_init

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

namespace {
  symbolt &new_tmp_symbol(symbol_tablet &symbol_table, const std::string& prefix = "tmp_struct_init")
{
  static int temporary_counter=0;

  auxiliary_symbolt new_symbol;
  symbolt *symbol_ptr;

  do
  {
    new_symbol.name=prefix+'$'+i2string(++temporary_counter);
    new_symbol.base_name=new_symbol.name;
    new_symbol.mode=ID_java;
  } while(symbol_table.move(new_symbol, symbol_ptr));

  return *symbol_ptr;
}
}

/*******************************************************************\

Function: gen_nondet_init

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

namespace {
void gen_nondet_init(
  const exprt &expr,
  code_blockt &init_code,
  symbol_tablet &symbol_table,
  std::set<irep_idt> &recursion_set,
  bool is_sub,
  irep_idt class_identifier,
  bool skip_classid = false,
  bool create_dynamic_objects = false)
{
  const namespacet ns(symbol_table);
  const typet &type=ns.follow(expr.type());
  
  if(type.id()==ID_pointer)
  {
    // dereferenced type
    const pointer_typet &pointer_type=to_pointer_type(type);
    const typet &subtype=ns.follow(pointer_type.subtype());

    if(subtype.id()==ID_struct)
    {
      const struct_typet &struct_type=to_struct_type(subtype);
      const irep_idt struct_tag=struct_type.get_tag();

      if(recursion_set.find(struct_tag)!=recursion_set.end())
      {
        // make null
        null_pointer_exprt null_pointer_expr(pointer_type);
        code_assignt code(expr, null_pointer_expr);
        init_code.copy_to_operands(code);

        return;
      }
    }

    if(!create_dynamic_objects)
    {
    
      symbolt &aux_symbol=new_tmp_symbol(symbol_table);
      aux_symbol.type=subtype;
      aux_symbol.is_static_lifetime=true;

      exprt object=aux_symbol.symbol_expr();
      gen_nondet_init(object, init_code, symbol_table, recursion_set, false, "", false, create_dynamic_objects);

      address_of_exprt aoe(object);

      code_assignt code(expr, aoe);
      init_code.copy_to_operands(code);

    }
    else
    {

      // build size expression
      exprt object_size=size_of_expr(subtype, ns);

      if(subtype.id()!=ID_empty && !object_size.is_nil())
	{
	  // malloc expression
	  side_effect_exprt malloc_expr(ID_malloc);
	  malloc_expr.copy_to_operands(object_size);
	  malloc_expr.type()=pointer_type;

	  code_assignt code(expr, malloc_expr);
	  init_code.copy_to_operands(code);

	  // dereference expression
	  dereference_exprt deref_expr(expr, subtype);

	  gen_nondet_init(deref_expr, init_code, symbol_table, recursion_set, false, "", false, create_dynamic_objects);
	}
      else
	{
	  // make null
	  null_pointer_exprt null_pointer_expr(pointer_type);
	  code_assignt code(expr, null_pointer_expr);
	  init_code.copy_to_operands(code);
	}
    }
  }
  else if(type.id()==ID_struct)
  {
    typedef struct_typet::componentst componentst;

    const struct_typet &struct_type=to_struct_type(type);
    const irep_idt struct_tag=struct_type.get_tag();

    const componentst &components=struct_type.components();
    
    if(!is_sub)
      class_identifier=struct_tag;

    recursion_set.insert(struct_tag);
    assert(!recursion_set.empty());
    
    for(const auto &component : components)
    {
      const typet &component_type=component.type();
      irep_idt name=component.get_name();

      member_exprt me(expr, name, component_type);

      if(name=="@class_identifier")
      {

	if(skip_classid)
	  continue;
	
        constant_exprt ci(class_identifier, string_typet());

        code_assignt code(me, ci);
        init_code.copy_to_operands(code);
      }
      else
      {
        assert(!name.empty());

        bool _is_sub=name[0]=='@';
#if 0
        irep_idt _class_identifier=
          _is_sub?(class_identifier.empty()?struct_tag:class_identifier):"";
#endif

        gen_nondet_init(me, init_code, symbol_table, recursion_set, _is_sub,
			class_identifier, false, create_dynamic_objects);
      }
    }

    recursion_set.erase(struct_tag);
  }
  else
  {
    side_effect_expr_nondett se=side_effect_expr_nondett(type);

    code_assignt code(expr, se);
    init_code.copy_to_operands(code);
  }
}
}

/*******************************************************************\

Function: gen_nondet_init

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

namespace {
void gen_nondet_init(
  const exprt &expr,
  code_blockt &init_code,
  symbol_tablet &symbol_table,
  bool skip_classid = false,
  bool create_dynamic_objects = false)
{
  std::set<irep_idt> recursion_set;
  gen_nondet_init(expr, init_code, symbol_table, recursion_set, false, "", skip_classid, create_dynamic_objects);
}
}

/*******************************************************************\

Function: gen_argument

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

namespace {

exprt get_nondet_bool(const typet& type) {
  // We force this to 0 and 1 and won't consider
  // other values.
  return typecast_exprt(side_effect_expr_nondett(bool_typet()), type);
}

exprt gen_argument(
  const typet &type,
  code_blockt &init_code,
  bool is_this,
  bool allow_null,
  symbol_tablet &symbol_table)
{
  if(type.id()==ID_pointer)
  {
    symbolt &aux_symbol=new_tmp_symbol(symbol_table);
    aux_symbol.type=type.subtype();
    aux_symbol.is_static_lifetime=true;

    exprt object=aux_symbol.symbol_expr();
    
    gen_nondet_init(object, init_code, symbol_table);

    // todo: need to pass null, possibly
    return address_of_exprt(object);
  }
  else if(type.id()==ID_c_bool)
    return get_nondet_bool(type);
  else
    return side_effect_expr_nondett(type);
}
}

/*******************************************************************\

Function: java_static_lifetime_init

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

bool java_static_lifetime_init(
  symbol_tablet &symbol_table,
  const source_locationt &source_location,
  message_handlert &message_handler)
{
  symbolt &initialize_symbol=symbol_table.lookup(INITIALIZE);
  code_blockt &code_block=to_code_block(to_code(initialize_symbol.value));
  
  // we need to zero out all static variables
  
  for(symbol_tablet::symbolst::const_iterator
      it=symbol_table.symbols.begin();
      it!=symbol_table.symbols.end();
      it++)
  {
    if(it->second.type.id()!=ID_code &&
       it->second.is_lvalue &&
       it->second.is_state_var &&
       it->second.is_static_lifetime &&
       it->second.value.is_not_nil() &&
       it->second.mode==ID_java)
    {
      code_assignt assignment(it->second.symbol_expr(), it->second.value);
      code_block.add(assignment);
    }
  }
  
  // we now need to run all the <clinit> methods

  for(symbol_tablet::symbolst::const_iterator
      it=symbol_table.symbols.begin();
      it!=symbol_table.symbols.end();
      it++)
  {
    if(it->second.base_name=="<clinit>" &&
       it->second.type.id()==ID_code &&
       it->second.mode==ID_java)
    {
      code_function_callt function_call;
      function_call.lhs()=nil_exprt();
      function_call.function()=it->second.symbol_expr();
      code_block.add(function_call);
    }
  }
  
  return false;
}

/*******************************************************************\

Function: java_entry_point

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

bool java_entry_point(
  symbol_tablet &symbol_table,
  const irep_idt &main_class,
  message_handlert &message_handler)
{
  // check if the entry point is already there
  if(symbol_table.symbols.find(goto_functionst::entry_point())!=
     symbol_table.symbols.end())
    return false; // silently ignore

  messaget message(message_handler);

  symbolt symbol; // main function symbol

  // find main symbol
  if(config.main!="")
  {
    // Add java:: prefix
    std::string main_identifier="java::"+config.main;
    
    symbol_tablet::symbolst::const_iterator s_it;
    
    // Does it have a type signature? (':' suffix)
    if(config.main.rfind(':')==std::string::npos)
    {
      std::string prefix=main_identifier+':';
      std::set<irep_idt> matches;
      
      for(const auto & s : symbol_table.symbols)
        if(has_prefix(id2string(s.first), prefix) &&
           s.second.type.id()==ID_code)
          matches.insert(s.first);

      if(matches.empty())
      {
        message.error() << "main symbol `" << config.main
                        << "' not found" << messaget::eom;
        return true;
      }
      else if(matches.size()==1)
      {
        s_it=symbol_table.symbols.find(*matches.begin());
        assert(s_it!=symbol_table.symbols.end());
      }
      else
      {
        message.error() << "main symbol `" << config.main
                        << "' is ambiguous:\n";

        for(const auto & s : matches)
          message.error() << "  " << s << '\n';
        
        message.error() << messaget::eom;

        return true;
      }
    }
    else
    {
      // just look it up
      s_it=symbol_table.symbols.find(main_identifier);

      if(s_it==symbol_table.symbols.end())
      {
        message.error() << "main symbol `" << config.main
                        << "' not found" << messaget::eom;
        return true;
      }
    }

    // function symbol
    symbol=s_it->second;

    if(symbol.type.id()!=ID_code)
    {
      message.error() << "main symbol `" << config.main
                      << "' not a function" << messaget::eom;
      return true;
    }
    
    // check if it has a body
    if(symbol.value.is_nil())
    {
      message.error() << "main method `" << main_class
                      << "' has no body" << messaget::eom;
      return true;
    }
  }
  else
  {
    // no function given, we look for the main class
    assert(config.main=="");

    // are we given a main class?
    if(main_class.empty())
      return false; // silently ignore

    std::string entry_method=
      id2string(main_class)+".main";

    std::string prefix="java::"+entry_method+":";

    // look it up
    std::set<irep_idt> matches;

    for(symbol_tablet::symbolst::const_iterator
        s_it=symbol_table.symbols.begin();
        s_it!=symbol_table.symbols.end();
        s_it++)
    {
      if(s_it->second.type.id()==ID_code &&
         has_prefix(id2string(s_it->first), prefix))
        matches.insert(s_it->first);
    }

    if(matches.empty())
    {
      // Not found, silently ignore
      return false;
    }

    if(matches.size()>=2)
    {
      message.error() << "main method in `" << main_class
                      << "' is ambiguous" << messaget::eom;
      return true; // give up with error, no main
    }

    // function symbol
    symbol=symbol_table.symbols.find(*matches.begin())->second;
  
    // check if it has a body
    if(symbol.value.is_nil())
    {
      message.error() << "main method `" << main_class
                      << "' has no body" << messaget::eom;
      return true; // give up with error
    }
  }

  assert(!symbol.value.is_nil());
  assert(symbol.type.id()==ID_code);

  const code_typet &code_type=to_code_type(symbol.type);
    
  create_initialize(symbol_table);

  if(java_static_lifetime_init(symbol_table, symbol.location, message_handler))
    return true;

  code_blockt init_code;

  // build call to initialization function
  {
    symbol_tablet::symbolst::iterator init_it=
      symbol_table.symbols.find(INITIALIZE);

    if(init_it==symbol_table.symbols.end())
    {
      message.error() << "failed to find " INITIALIZE " symbol"
                      << messaget::eom;
      return true; // give up with error
    }

    code_function_callt call_init;
    call_init.lhs().make_nil();
    call_init.add_source_location()=symbol.location;
    call_init.function()=init_it->second.symbol_expr();

    init_code.move_to_operands(call_init);
  }

  // build call to the main method

  code_function_callt call_main;
  call_main.add_source_location()=symbol.location;
  call_main.function()=symbol.symbol_expr();

  const code_typet::parameterst &parameters=
    code_type.parameters();

  exprt::operandst main_arguments;
  main_arguments.resize(parameters.size());
  
  for(std::size_t param_number=0;
      param_number<parameters.size();
      param_number++)
  {
    bool is_this=param_number==0 &&
                 parameters[param_number].get_this();
    bool allow_null=config.main!="";
    
    main_arguments[param_number]=
      gen_argument(parameters[param_number].type(), 
                   init_code, is_this, allow_null, symbol_table);
  }

  call_main.arguments()=main_arguments;

  init_code.move_to_operands(call_main);

  // add "main"
  symbolt new_symbol;

  code_typet main_type;
  main_type.return_type()=empty_typet();

  new_symbol.name=goto_functionst::entry_point();
  new_symbol.type.swap(main_type);
  new_symbol.value.swap(init_code);
  new_symbol.mode=ID_java;

  if(symbol_table.move(new_symbol))
  {
    message.error() << "failed to move main symbol" << messaget::eom;
    return true;
  }

  return false;
}

namespace { // Anon namespace for insert-nondet support functions

exprt clean_deref(const exprt ptr) {

  return ptr.id() == ID_address_of ? ptr.op0() : dereference_exprt(ptr, ptr.type().subtype());
  
}

bool find_superclass_with_type(exprt& ptr, const typet& target_type, const namespacet& ns) {

  while(true) {
  
    const typet ptr_base = ns.follow(ptr.type().subtype());
  
    if(ptr_base.id() != ID_struct)
      return false;

    const struct_typet& base_struct = to_struct_type(ptr_base);

    if(base_struct.components().size() == 0)
      return false;

    const typet& first_field_type = ns.follow(base_struct.components()[0].type());
    ptr = clean_deref(ptr);
    ptr = member_exprt(ptr, base_struct.components()[0].get_name(), first_field_type);
    ptr = address_of_exprt(ptr);

    if(first_field_type == target_type)
      return true;

  }

}
  
exprt make_clean_pointer_cast(const exprt ptr, const typet& target_type, const namespacet& ns) {

  assert(target_type.id() == ID_pointer && "Non-pointer target in make_clean_pointer_cast?");
  
  if(ptr.type() == target_type)
    return ptr;

  const typet& target_base = ns.follow(target_type.subtype());

  exprt bare_ptr = ptr;
  while(bare_ptr.id() == ID_typecast) {
    assert(bare_ptr.type().id() == ID_pointer && "Non-pointer in make_clean_pointer_cast?");
    if(bare_ptr.type().subtype() == empty_typet())
      bare_ptr = bare_ptr.op0();
  }

  assert(bare_ptr.type().id() == ID_pointer && "Non-pointer in make_clean_pointer_cast?");

  if(bare_ptr.type() == target_type)
    return bare_ptr;

  exprt superclass_ptr = bare_ptr;
  if(find_superclass_with_type(superclass_ptr, target_base, ns))
    return superclass_ptr;
  
  return typecast_exprt(bare_ptr, target_type);

}

void insert_nondet_opaque_fields_at(const typet& expected_type,
				    const exprt &ptr,
				    symbol_tablet& symbol_table,
				    code_blockt* parent_block,
				    unsigned insert_before_index,
				    bool is_constructor,
				    bool assume_non_null)
{

  // At this point we know 'ptr' points to an opaque-typed object. We should nondet-initialise it
  // and insert the instructions *after* the offending call at (*parent_block)[insert_before_index].

  assert(expected_type.id() == ID_pointer && "Nondet initialiser should have pointer type");
  assert(parent_block && "Must have an existing block to insert nondet-init code");

  namespacet ns(symbol_table);

  const auto& expected_base = ns.follow(expected_type.subtype());
  if(expected_base.id() != ID_struct)
    return;
  
  const exprt cast_ptr = make_clean_pointer_cast(ptr, expected_type, ns);
  code_labelt set_null_label;
  code_labelt init_done_label;

  code_blockt new_instructions;

  if(!is_constructor) {

    // Per default CBMC would suppose this to be any conceivable pointer.
    // For now, insist that it is either fresh or null. In future we will
    // want to consider the possiblity that it aliases other objects.
    
    static unsigned long synthetic_constructor_count = 0;

    if(!assume_non_null) {

      auto returns_null_sym = new_tmp_symbol(symbol_table, "opaque_returns_null");
      returns_null_sym.type = c_bool_typet(1);
      auto returns_null = returns_null_sym.symbol_expr();
      auto assign_returns_null = code_assignt(returns_null, get_nondet_bool(returns_null_sym.type));
      new_instructions.move_to_operands(assign_returns_null);

      auto set_null_inst = code_assignt(cast_ptr, null_pointer_exprt(to_pointer_type(cast_ptr.type())));

      std::ostringstream fresh_label_oss;
      fresh_label_oss << "post_synthetic_malloc_" << (++synthetic_constructor_count);
      std::string fresh_label = fresh_label_oss.str();
      set_null_label = code_labelt(fresh_label, set_null_inst);

      init_done_label = code_labelt(fresh_label + "_init_done", code_skipt());

      code_ifthenelset null_check;
      null_check.cond() = notequal_exprt(returns_null, constant_exprt("0", returns_null_sym.type));
      null_check.then_case() = code_gotot(fresh_label);
      new_instructions.move_to_operands(null_check);

    }

    // Note this allocates memory but doesn't call any constructor.
    side_effect_exprt malloc_expr(ID_malloc);
    malloc_expr.copy_to_operands(size_of_expr(expected_base, ns));
    malloc_expr.type() = expected_type;
    auto alloc_inst = code_assignt(cast_ptr, malloc_expr);
    new_instructions.move_to_operands(alloc_inst);

  }

  exprt derefd = clean_deref(cast_ptr);
  
  gen_nondet_init(derefd, new_instructions, symbol_table, is_constructor, /*create_dynamic=*/true);

  if((!is_constructor) && !assume_non_null) {
    new_instructions.copy_to_operands(code_gotot(init_done_label.get_label()));    
    new_instructions.move_to_operands(set_null_label);
    new_instructions.move_to_operands(init_done_label);
  }
  
  if(new_instructions.operands().size() != 0) {

    auto institer = parent_block->operands().begin();
    std::advance(institer, insert_before_index);
    parent_block->operands().insert(institer, new_instructions.operands().begin(), new_instructions.operands().end());
    
  }

}

void assign_parameter_names(code_typet& ftype, const irep_idt& name_prefix, symbol_tablet& symbol_table) {

  code_typet::parameterst &parameters=ftype.parameters();

  // Mostly borrowed from java_bytecode_convert.cpp; maybe factor this out.
  // assign names to parameters
  for(std::size_t i=0, param_index=0;
      i < parameters.size(); ++i)
  {
    irep_idt base_name, identifier;

    if(i==0 && parameters[i].get_this())
      base_name="this";
    else
      base_name="stub_ignored_arg"+i2string(param_index);

    identifier=id2string(name_prefix)+"::"+id2string(base_name);
    parameters[i].set_base_name(base_name);
    parameters[i].set_identifier(identifier);

    // add to symbol table
    parameter_symbolt parameter_symbol;
    parameter_symbol.base_name=base_name;
    parameter_symbol.mode=ID_java;
    parameter_symbol.name=identifier;
    parameter_symbol.type=parameters[i].type();
    symbol_table.add(parameter_symbol);
  }
  
}
			    
void insert_nondet_opaque_fields(symbolt& sym, symbol_tablet& symbol_table, code_blockt* parent, unsigned parent_index, bool assume_non_null) {

  code_blockt new_instructions;
  code_typet& required_type = to_code_type(sym.type);
  namespacet ns(symbol_table);

  bool is_constructor = sym.type.get_bool(ID_constructor);

  if(!is_constructor) {
    const auto& needed = required_type.return_type();
    if(needed.id() != ID_pointer) {
      // Returning a primitive -- no point generating a stub.
      return;
    }
  }

  assign_parameter_names(required_type, sym.name, symbol_table);
      
  if(is_constructor)
  {
    const auto& thisarg = required_type.parameters()[0];
    const auto& thistype = thisarg.type();
    auto& init_symbol = new_tmp_symbol(symbol_table, "to_construct");
    init_symbol.type = thistype;
    const auto init_symexpr = init_symbol.symbol_expr();
    auto getarg = code_assignt(init_symexpr, symbol_exprt(thisarg.get_identifier()));
    new_instructions.copy_to_operands(getarg);
    insert_nondet_opaque_fields_at(thistype, init_symexpr, symbol_table, &new_instructions,
				   1, true, assume_non_null);
    sym.type.set("opaque_method_capture_symbol", init_symbol.name);    
  }
  else
  {
    auto toreturn_symbol = new_tmp_symbol(symbol_table, "to_return");
    toreturn_symbol.type = required_type.return_type();
    auto toreturn_symexpr = toreturn_symbol.symbol_expr();
    insert_nondet_opaque_fields_at(required_type.return_type(),
				   toreturn_symexpr,
				   symbol_table, &new_instructions, 0, false, assume_non_null);
    new_instructions.copy_to_operands(code_returnt(toreturn_symexpr));
    sym.type.set("opaque_method_capture_symbol", toreturn_symbol.name);        
  }

  sym.value = new_instructions;

}
  
void insert_nondet_opaque_fields(symbolt &sym, symbol_tablet &symbol_table, bool assume_non_null) {

  if(sym.is_type)
    return;
  if(sym.value.id() != ID_nil)
    return;
  if(sym.type.id() != ID_code)
    return;

  insert_nondet_opaque_fields(sym, symbol_table, 0, 0, assume_non_null);

}

} // End anon namespace for insert-nondet support functions

void java_generate_opaque_method_stubs(symbol_tablet &symbol_table, bool assume_non_null) {

  std::vector<irep_idt> identifiers;
  identifiers.reserve(symbol_table.symbols.size());
  forall_symbols(s_it, symbol_table.symbols)
    identifiers.push_back(s_it->first);

  for(auto& id : identifiers)
    insert_nondet_opaque_fields(symbol_table.symbols[id], symbol_table, assume_non_null);

}
