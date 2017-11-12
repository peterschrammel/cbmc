/*******************************************************************\

Module: 

Author: Areej Alatawi

Date: October 2017

\*******************************************************************/

/// \file
/// 


#include <util/message.h>
#include <util/std_code.h>
#include <util/std_expr.h>
#include <util/expr_util.h>
#include <util/irep.h>
#include <util/source_location.h>
#include <util/base_type.h>

#include <goto-programs/goto_functions.h>
#include <goto-programs/goto_functions_template.h>
#include <goto-programs/goto_program_template.h>
#include <goto-programs/remove_skip.h>


#include "loop_to_recursion.h"

bool has_loop (
   goto_programt &goto_function_body)
{
  
   bool found=false;
 
   //search for loop
   Forall_goto_program_instructions(it, goto_function_body)

   {  
      if(!it->is_backwards_goto() &&
	 it!=goto_function_body.instructions.end())
    	 continue;

      if(it->is_backwards_goto())
         found = true; 
         break;     
   }
  
   if(!found)
   return false;

   else 
   return true;          
}


void define_rec_function(
  goto_functionst &goto_functions,
  symbol_tablet &symbol_table,
  messaget message)
  {
  
  //add REC_function symbol
  symbolt recursive_function;
  const irep_idt rec_f_name = "REC_function";
  const irep_idt rec_f_bname = "REC_function";
  recursive_function.mode = ID_C;
  recursive_function.name = rec_f_name;
  recursive_function.base_name = rec_f_bname;
  recursive_function.pretty_name = rec_f_name; 
  recursive_function.type = code_typet();
  symbol_table.add(recursive_function);

  //modify functions map
  goto_functionst::goto_functiont &rec_f=goto_functions.function_map[recursive_function.name];
  
  goto_functions.update();
  
  //set return type of REC_function
  rec_f.type.return_type() = struct_typet();

  symbolt &recf_symbol = symbol_table.lookup(recursive_function.name);
  recf_symbol.type = rec_f.type;
   
  //no body yet!  
  assert(!rec_f.body_available());
  
  message.status() << "loop found\nREC_function defined"
                    << messaget::eom;      
}

void set_rec_function_return_expr(
    goto_functionst &goto_functions,
    symbol_tablet &symbol_table,
    exprt &lhs)
{
 
   namespacet ns(symbol_table);

   goto_functionst::function_mapt::iterator f_it=
    goto_functions.function_map.find("REC_function");
   
   const symbolt &recf_symbol = ns.lookup(f_it->first);
   
   //add return symbol
   symbolt return_val_symbol;
   const irep_idt rv_name = "return_vals";
   const irep_idt rv_bname = "return_vals";
   return_val_symbol.is_static_lifetime=true; 
   return_val_symbol.module =recf_symbol.module;
   return_val_symbol.mode =recf_symbol.module;
   return_val_symbol.name = rv_name;
   return_val_symbol.base_name = rv_bname;
   return_val_symbol.type = struct_typet();
   symbol_table.add(return_val_symbol);

   
   lhs = return_val_symbol.symbol_expr();

   //set return struct components same type as parameters
   const code_typet::parameterst &parameters = f_it->second.type.parameters();
   
   struct_typet &rv_struct=to_struct_type(lhs.type());

   struct_typet::componentst &components=rv_struct.components();
   
   unsigned nr=1;

   for(code_typet::parameterst::const_iterator 
      p_it = parameters.begin();
      p_it != parameters.end();
      p_it++)
     {

      const code_typet::parametert &parameter = *p_it;
      const typet &par_type = ns.follow(parameter.type());
      const irep_idt &par_name = parameter.get_identifier();
 
      struct_typet::componentt component; 
       
      component.set_name(par_name);
      component.type() = par_type;
      components.push_back(component);
 
      nr++;

     }

   //update
   return_val_symbol.type = rv_struct;
   
   //return variable decl in REC_function 
   goto_programt rv_decl; 

   goto_programt::targett rv_t= rv_decl.add_instruction(); 
   rv_t->make_decl(); 
   rv_t->code = code_declt(return_val_symbol.symbol_expr());

   f_it->second.body.destructive_append(rv_decl);

   f_it->second.body.update();
   
   lhs = return_val_symbol.symbol_expr();
  
}

void get_rec_function_arguments(
   const goto_functionst::goto_functiont &function,
   symbol_tablet &symbol_table,
   exprt::operandst &arguments)
{
  namespacet ns(symbol_table);

  
  std::set<irep_idt> local_vars;
 
  //locals in loop functions
  get_local_identifiers(function, local_vars);

  for(const auto v: local_vars)
     { 
       //exclude return value
       if(v == "return_vals")
        continue;

       const symbolt &arg = ns.lookup(v);

       arguments.push_back(arg.symbol_expr());    
     }
}

void set_rec_function_parameters(
   goto_functionst::goto_functiont &rec_function,
   symbol_tablet &symbol_table,
   const exprt::operandst &arguments,
   std::map<exprt,symbol_exprt> &recf_vars) 
{

   code_typet::parameterst &parameters = rec_function.type.parameters();
  
   assert(parameters.empty());

   symbolt &recf_symbol = symbol_table.lookup("REC_function");
   //code_typet::parameterst::iterator it = parameters.begin();
   
   unsigned nr =1;

   for(exprt::operandst::const_iterator 
      a_it = arguments.begin();
      a_it != arguments.end();
      a_it++)
   {
       
       exprt arg = *a_it;

       //add parameter symbol, name it whatever
       parameter_symbolt rec_param;
       
       rec_param.module =recf_symbol.module;
       const irep_idt &param_id = "p_"+std::to_string(nr);
       rec_param.name = param_id;
       rec_param.base_name = param_id;
       rec_param.mode = recf_symbol.mode;
       rec_param.type = arg.type();
       symbol_table.add(rec_param);
      
       nr++;

       //construct rec_function parameters vector:
       code_typet::parametert parameter;
      
       parameter.set_identifier(rec_param.name);
       parameter.set_base_name(rec_param.base_name);
       parameter.type() = rec_param.type;

       parameters.push_back(parameter);

       //to keep tracking symbols for later use
       recf_vars.insert(std::pair<exprt,symbol_exprt>(arg, rec_param.symbol_expr()));      
   }
     
   //update
   recf_symbol.type = rec_function.type;
}

void assign_rec_function_parameters(
   goto_functionst::goto_functiont &rec_function,
   symbol_tablet &symbol_table,
   goto_programt &dest,
   const exprt::operandst &arguments,
   const irep_idt &recf_name)
{

   namespacet ns(symbol_table); 

   goto_programt par_decls;
   goto_programt par_assignments;

   code_typet::parameterst &parameters = rec_function.type.parameters();

   exprt::operandst::const_iterator a_it =arguments.begin();


   for(code_typet::parameterst::const_iterator 
      p_it = parameters.begin();
      p_it != parameters.end();
      p_it++)
     {
      const code_typet::parametert &parameter = *p_it;

      const typet &par_type = ns.follow(parameter.type());

      const irep_idt &identifier = parameter.get_identifier();
 
      const symbolt &par_symbol = ns.lookup(identifier);

      //declare param:
      goto_programt::targett decl=par_decls.add_instruction();
      decl->make_decl();
      decl->code=code_declt(par_symbol.symbol_expr());
      decl->function=recf_name;

      //treat argument
      exprt arg;
      
      //following copied from "goto_inlined_class.cpp":

      // if you run out of actual arguments there was a mismatch
      arg =*a_it;
      assert(!arg.is_nil());

      // nil means "don't assign"
      if(arg.is_nil())
       {
       }

      else
       {
        // might be no need as params built following args themselves!
        if(!base_type_eq(par_type, arg.type(), ns))
         {
            const typet &f_partype=ns.follow(par_type);
            const typet &f_acttype=ns.follow(arg.type());

            // we are willing to do some conversion
           if((f_partype.id()==ID_pointer &&
              f_acttype.id()==ID_pointer) ||
             (f_partype.id()==ID_pointer &&
              f_acttype.id()==ID_array &&
              f_partype.subtype()==f_acttype.subtype()))
              {
                 arg.make_typecast(par_type);
              }
           else if((f_partype.id()==ID_signedbv ||
                    f_partype.id()==ID_unsignedbv ||
                    f_partype.id()==ID_bool) &&
                  (f_acttype.id()==ID_signedbv ||
                   f_acttype.id()==ID_unsignedbv ||
                   f_acttype.id()==ID_bool))
              {
                  arg.make_typecast(par_type);
              }
           else
              {
                 
              }
        }
      }
      
      
      //add an assignment of the param and arg
      code_assignt assignment(symbol_exprt(identifier, par_type), arg);

      par_assignments.add_instruction(ASSIGN);   
      par_assignments.instructions.back().code.swap(assignment);
      par_assignments.instructions.back().function = recf_name;


      if(a_it!=arguments.end())
         ++a_it;
     
    }
  
   //put them together
   dest.destructive_append(par_decls);
   dest.destructive_append(par_assignments);

}

void destruct_rec_function_parameters(
   goto_functionst::goto_functiont &rec_function,
   goto_programt &dest,
   const irep_idt &recf_name,
   symbol_tablet &symbol_table)  
{
   const code_typet::parameterst &parameters = rec_function.type.parameters();

   goto_programt par_destructions;

   for(code_typet::parameterst::const_iterator 
      p_it = parameters.begin();
      p_it != parameters.end();
      p_it++)
     {
      const code_typet::parametert &parameter = *p_it;

      const irep_idt &identifier = parameter.get_identifier();

      const symbolt &par_symbol = symbol_table.lookup(identifier);
      
      goto_programt::targett dead=par_destructions.add_instruction();

      dead->make_dead();
      dead->code=code_deadt(par_symbol.symbol_expr());
      dead->function=recf_name;

     }

   dest.destructive_append(par_destructions);
}

void replace_vars(
   exprt &code_expr,
   const std::map<exprt,symbol_exprt> &recf_vars,
   symbol_tablet &symbol_table)
{
   
   namespacet ns(symbol_table);
   
   unsigned i =0;
             
   Forall_operands(it, code_expr)		
   {
     //target only symbols
     if(code_expr.operands()[i].id()== ID_symbol)
          {
             const irep_idt &op_identifier = code_expr.operands()[i].get(ID_identifier);
      
             const symbolt &op_symbol = ns.lookup(op_identifier);
             
             //make sure to replace the right symbol using rec_vars         
             for(std::map<exprt,symbol_exprt>::const_iterator
	         rv_it = recf_vars.begin();
		 rv_it != recf_vars.end();
		 rv_it++)
                 {
 	           if(op_symbol.symbol_expr() == rv_it->first) 
                     {
 			*it = rv_it->second;                        
		     }  	   
                 }
            }
 		 
     i++;
   }

}

void rebuild_loop_body(
   goto_functionst::goto_functiont &rec_function,
   goto_programt &function_body,//loop funnction
   symbol_tablet &symbol_table,
   const unsigned loop_begin_location,
   const unsigned loop_end_location,
   goto_programt &new_loop_body,
   exprt &cond,
   const std::map<exprt,symbol_exprt> &recf_vars)
{

  assert(!recf_vars.empty());

  goto_programt inst_line;
  goto_programt::targett inst_t;

  for (auto &inst : function_body.instructions)
    {  
      
       if(inst.location_number==loop_begin_location)
           {
            //main loop condition:
             cond = inst.guard.op0();
             replace_vars(cond, recf_vars,symbol_table);
   	    
             cond = boolean_negate(cond);
           }

       if(inst.location_number > loop_begin_location &&
               inst.location_number < loop_end_location)
           {
	     //instructions betwee loop cond and loop end

             inst_t = inst_line.add_instruction();
             inst_t->type = inst.type;
            
	      if(inst.is_assign())
	       {
		//treat both sides
  	        exprt &lhs = inst.code;
	        replace_vars(lhs, recf_vars,symbol_table);
              
  	        exprt &rhs = inst.code.op1();
	        replace_vars(rhs, recf_vars,symbol_table);
	     
                inst_t->code = to_code(lhs); 
	       }

              if(inst.is_function_call())
               { 
                //only target call arguments
                exprt &new_arguments = inst.code.op2();
                replace_vars(new_arguments, recf_vars,symbol_table);
                
                code_function_callt updated_call;
                updated_call.function() = inst.code.op1();
                updated_call.arguments() =new_arguments.operands();
           
                inst_t->code = updated_call;                
               }

              if(inst.is_assume() || inst.is_assert())
               { 
                  //treat guard
                  exprt &inner_cond = inst.guard;
                  replace_vars(inner_cond, recf_vars,symbol_table);
              
                  if(inst.is_assume())
                    {
                      inst_t->make_assumption(inner_cond);
                    }
                  else
                    {
                      inst_t->make_assertion(inner_cond);  
                    }                  
               }

              if(inst.is_goto())
               { 
                 if(!inst.guard.is_true())
                   {
                    //inner conditions                    
                    exprt &inner_cond = inst.guard.op0();
                    replace_vars(inner_cond, recf_vars,symbol_table);
                    
                    goto_programt::targett f_skip = inst_line.add_instruction(SKIP);
                    
                    inst_t->make_goto(f_skip);
                    inst_t->guard.swap(inner_cond);                   
                   }
               }
              
               if(inst.is_decl() || inst.is_dead())
               {
                 //just ignore, already treated before as local vars
                 inst_t->type = SKIP; 
                 inst_t->code.clear();
               }

               else
               {
                inst_t->code =inst.code;
               }
            
            //insert the new instruction 
            new_loop_body.destructive_append(inst_line);
             
            continue; 
           }

       continue; 
    }
    
   //update
   new_loop_body.update();

}


void remove_loop(
   goto_programt &function_body, //loop function body
   const unsigned loop_begin_location,
   const unsigned loop_end_location)
{
   
   for (auto &inst : function_body.instructions)
    {  
	if(inst.location_number >= loop_begin_location &&
               inst.location_number <= loop_end_location)
           {
              //skip it:
              inst.type = SKIP;
              inst.code.clear();
              
              continue;
           }

        continue;
    }
    
   //remove skipped instructions
   remove_skip(function_body);
   
   //update
   function_body.update(); 
    
}
    
void build_rec_function_body(
   goto_functionst &goto_functions, 
   goto_functionst::goto_functiont &loop_function,
   symbol_tablet &symbol_table,
   const unsigned loop_begin_location,
   const unsigned loop_end_location,
   const exprt::operandst &arguments,
   exprt &lhs,
   const std::map<exprt,symbol_exprt> recf_vars)
{

   goto_functionst::function_mapt::iterator rec_f_info= 
       goto_functions.function_map.find("REC_function");
     
   const irep_idt recf_identifier=rec_f_info->first;

   goto_functionst::goto_functiont &rec_function=rec_f_info->second;
         
   //REC_function body ahould be:
   //-----------------------------
   // w: if(!c) goto z;
   // x: body;
   // y: call rec_function();  
   // z: return values;       
   //    end_function
   //------------------------------
  
   
   //params decls, assignments
   goto_programt param_assignments;

   assign_rec_function_parameters(
              rec_function,  
              symbol_table, 
              param_assignments, 
              arguments,
              recf_identifier);

   rec_function.body.destructive_append(param_assignments);
   rec_function.body.update();
   
   
   //modify loop_body:
   exprt cond;
   goto_programt gp_x;

   rebuild_loop_body(
         rec_function, 
         loop_function.body, 
         symbol_table, 
         loop_begin_location, 
         loop_end_location,
         gp_x, 
         cond, 
         recf_vars);


   //do z
   goto_programt gp_z; 
   goto_programt::targett z = gp_z.add_instruction(SKIP);
   
   //w: if (!c) goto z
   goto_programt gp_w;

   goto_programt::targett w =gp_w.add_instruction();
   w->make_goto(z);
   w->guard.swap(cond);
    
   //prepare REC_function()
   const symbolt &rec_f = symbol_table.lookup(recf_identifier);

   // get rec call arguments: 
   exprt::operandst rec_arguments;
   get_rec_function_arguments(rec_function, symbol_table, rec_arguments);

   // define call code:
   code_function_callt rec_call_code;
   rec_call_code.lhs()=lhs;
   rec_call_code.function()=rec_f.symbol_expr();
   rec_call_code.arguments()= rec_arguments;


   //build: y: call REC_function(params)
   goto_programt gp_y;
   goto_programt::targett y=gp_y.add_instruction();

   y->make_function_call(rec_call_code);
   y->code = rec_call_code;
   y->guard =true_exprt();

   //rturns:
   goto_programt ret_value; 
   goto_programt::targett rv_t = ret_value.add_instruction();
   
   code_returnt return_code(rec_call_code.lhs());
   rv_t->make_return();
   rv_t->code= return_code;

   //destroy ret_vals
   const symbolt &rv_symbol =symbol_table.lookup("return_vals");

   goto_programt::targett rvd_t = rec_function.body.insert_after(rv_t);
   rvd_t->make_dead();
   rvd_t->code= code_deadt(rv_symbol.symbol_expr());
    
   //params destructions
   goto_programt param_destructions;

   destruct_rec_function_parameters(
              rec_function, 
              param_destructions, 
              recf_identifier,
              symbol_table);

   // build END_FUNCTION
   goto_programt gp_end_function;
   goto_programt::targett end_function=gp_end_function.add_instruction();
   end_function->type=END_FUNCTION;
   
   //insert them   
   rec_function.body.destructive_append(gp_w);
   rec_function.body.destructive_append(gp_x);
   rec_function.body.destructive_append(gp_y);
   rec_function.body.destructive_append(gp_z);
   rec_function.body.destructive_append(ret_value);
   rec_function.body.destructive_append(param_destructions);
   rec_function.body.destructive_append(gp_end_function);
   
   //update
   goto_functions.update();
   rec_function.body.update();

   //loop no more needed:
   remove_loop(loop_function.body, loop_begin_location, loop_end_location);
  
}
  
void replace_loop_body(
   goto_functionst::goto_functiont &goto_function,//loop_function
   goto_functionst &goto_functions, 
   symbol_tablet &symbol_table)
  
{
   
   goto_functionst::function_mapt::iterator rec_func=
    goto_functions.function_map.find("REC_function");

   //to specify loop locaion 
   unsigned loop_begin_loc =0;
   unsigned loop_end_loc =0;

   goto_programt::targett loop_head;
  
   symbolt &rec_f = symbol_table.lookup("REC_function"); 
  
   /*before adding replacing loop by REC_function call, 
   need to a)get arguments, b)set parameters, and c)set return*/
   exprt::operandst arguments;
   get_rec_function_arguments(goto_function, symbol_table, arguments);
   
   //should be done now for later use
   std::map<exprt,symbol_exprt> recf_vars;
 
   if(rec_func->second.type.parameters().empty())
     {
	set_rec_function_parameters(
              rec_func->second,  
              symbol_table,  
              arguments, 
              recf_vars);
     }
   
   exprt lhs;
   set_rec_function_return_expr(goto_functions, symbol_table, lhs);


   Forall_goto_program_instructions(it, goto_function.body)
   {

    if(!it->is_backwards_goto() &&
	it!=goto_function.body.instructions.end())
    	continue; 


    loop_head=it->get_target();
    
    //call code:
    code_function_callt rec_function_call;
    rec_function_call.add_source_location()=loop_head->source_location;
    rec_function_call.lhs()=lhs;
    rec_function_call.function()=rec_f.symbol_expr();
    rec_function_call.arguments()= arguments;


    // inser REC_function call:
    goto_programt::targett rec_call_t=goto_function.body.insert_before(loop_head);

    rec_call_t->make_function_call(rec_function_call);
    rec_call_t->source_location=loop_head->source_location;
    rec_call_t->code = rec_function_call;

    //return value decl
    goto_programt::targett rv_decl=goto_function.body.insert_before(rec_call_t);
    rv_decl->make_decl();
    rv_decl-> code =code_declt(lhs);
    
    //might need to re-assign struct components!
 
    goto_function.body.update();

    //loop location info 
    loop_begin_loc = loop_head->location_number;
    loop_end_loc = it->location_number;
    
   }

   //ready to build REC_function body
   build_rec_function_body( 
        goto_functions,
        goto_function,
        symbol_table, 
        loop_begin_loc, 
        loop_end_loc,
        arguments,
        lhs,
        recf_vars);
}

void loop_to_recursion(
  goto_functionst &goto_functions, 
  symbol_tablet &symbol_table,
  message_handlert &message_handler)
{

  messaget message(message_handler);

  bool loop_present = false; 

  message.status() << "Searching for loops"
                     << messaget::eom;
 
  Forall_goto_functions(f_it, goto_functions)
   {

     const irep_idt function_identifier=f_it->first;
     assert(!function_identifier.empty());
     goto_functionst::goto_functiont &goto_function=f_it->second;

     if(!goto_function.body_available())
        continue;

     if(has_loop(goto_function.body))
       { 
       
         loop_present = true;
           
         //check if REC_function already added:
         goto_functionst::function_mapt::iterator rec_func_id=
          goto_functions.function_map.find("REC_function");

         //not added?
         if(rec_func_id==goto_functions.function_map.end())
           {
             define_rec_function(
                  goto_functions,
                  symbol_table,
                  message);   
           }

         else
           {
             //clear for next loop function 
             rec_func_id->second.type.parameters().clear();
             //ignore this for now
             //rec_func_id->second.body.clear();
           }
         
         replace_loop_body(
           goto_function,
           goto_functions,
           symbol_table);              
       }
     
   }
    
    if(!loop_present) 
      {
        message.error() << "No loop found in goto program "
                        << messaget::eom; 
      }    
}
