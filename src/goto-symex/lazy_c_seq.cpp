/// \file
/// LazyCSeq context-bounded concurrency SSA transformation

#include "lazy_c_seq.h"

#include <util/cprover_prefix.h>
#include <util/format.h>
#include <util/format_expr.h>
#include <util/pointer_expr.h>
#include <util/prefix.h>
#include <util/simplify_expr.h>
#include <util/source_location.h>

#include "arith_tools.h"

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
  std::vector<symex_target_equationt::SSA_stepst::const_iterator> main_reads;
  std::
    unordered_map<irep_idt, symex_target_equationt::SSA_stepst::const_iterator>
      last_update;
  std::unordered_map<irep_idt, irep_idt> last_update_main;
  std::
    unordered_map<irep_idt, symex_target_equationt::SSA_stepst::const_iterator>
      last_cprover_upadte;
  exprt exited_array;

  collect_reads_and_writes(
    equation.SSA_steps, reads, main_reads, writes, message_handler);

  if(!writes.empty())
    create_write_constraints(
      equation, writes, last_update, last_update_main, message_handler);

  if(!reads.empty())
    create_read_constraints(
      equation, reads, writes, last_update, last_update_main, message_handler);

  create_cprover_constraints(equation, last_cprover_upadte, exited_array, message_handler);

  if(!(writes.empty() && reads.empty()))
  {
    create_cs_constraint(equation,reads,writes,message_handler);
    create_reach_constraint(equation, reads, writes, exited_array, message_handler);
  }

  if(!main_reads.empty())
    create_main_read_constraints(
      equation, last_update, main_reads, message_handler);

  handling_guards(equation, message_handler);

  exprt tmp;
  simplify(tmp, ns);
}

void lazy_c_seqt::create_write_constraints(
  symex_target_equationt &equation,
  const std::unordered_map<
    unsigned,
    std::vector<symex_target_equationt::SSA_stepst::const_iterator>> &writes,
  std::unordered_map<
    irep_idt,
    symex_target_equationt::SSA_stepst::const_iterator> &last_update,
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
      for(unsigned thread_nr = 1; thread_nr < writes.size(); ++thread_nr)
      {
        bool var_contained = false;
        for(const auto &s_it : writes.at(thread_nr)) //TODO: check if writes.at(thread_nr) exist
        {
          if(s_it->ssa_lhs.get_object_name() == variable)
          {
            var_contained = true;
          }
        }
        if(!var_contained)
          continue;
        for(const auto &s_it : writes.at(thread_nr))
        {
          std::string suffix =
            "_L" +
            std::to_string(
              s_it->source.pc->location_number) +
            "_R" + std::to_string(round);
          irep_idt statement_label_name = "E" + suffix;
          symbol_exprt statement_label{statement_label_name, bool_typet{}};

          // We don't need to check that this is a symbol because we have alread done that in collect_reads_and_writes.
          irep_idt end_of_round_name =
            id2string(to_symbol_expr(s_it->ssa_lhs)
                        .get_identifier()) +
            suffix;
          symbol_exprt end_of_round_value{
            end_of_round_name, s_it->ssa_lhs.type()};
          equal_exprt constraint{
            end_of_round_value,
            if_exprt{
              statement_label, s_it->ssa_lhs, last_write_of_previous_round}};
          log.warning() << format(constraint) << messaget::eom;
          equation.constraint(
            constraint, "write constraint", s_it->source);
          last_write_of_previous_round = end_of_round_value;

          last_update[variable] = s_it;
        }
      }
    }
  }
}

void lazy_c_seqt::create_read_constraints(
  symex_target_equationt &equation,
  const std::vector<std::pair<
    symex_target_equationt::SSA_stepst::const_iterator,
    std::optional<symex_target_equationt::SSA_stepst::const_iterator>>> &reads,
  const std::unordered_map<
    unsigned,
    std::vector<symex_target_equationt::SSA_stepst::const_iterator>> &writes,
  std::unordered_map<
    irep_idt,
    symex_target_equationt::SSA_stepst::const_iterator> &last_update,
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
    typet type = read_variable->ssa_lhs.type();

    if(!check_if_write_in_threads(writes, read_variable))
    {
      auto previous_name =
        id2string(last_update_main[read_variable->ssa_lhs.get_original_name()]);
      symbol_exprt previous{previous_name, type};
      equal_exprt constraint{read_variable->ssa_lhs, previous};

      log.warning() << format(constraint) << messaget::eom;
      equation.constraint(constraint, "read constraint", read_variable->source);

      continue;
    }

    for(std::size_t round = rounds; round >= 1; --round)
    {
      std::string condition_suffix =
        "_L" + std::to_string(read_variable->source.pc->location_number) +
        "_R" + std::to_string(round);
      irep_idt statement_label_name = "E" + condition_suffix;
      symbol_exprt statement_label{statement_label_name, bool_typet{}};

      std::string variable_suffix;
      irep_idt variable_round_name;

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
            id2string(
              to_symbol_expr(
                last_update[read.first->ssa_lhs.get_original_name()]->ssa_lhs)
                .get_identifier()) +
            "_L" +
            std::to_string(last_update[read.first->ssa_lhs.get_original_name()]
                             ->source.pc->location_number) +
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

bool lazy_c_seqt::check_if_write_in_threads(
  const std::unordered_map<
    unsigned,
    std::vector<symex_target_equationt::SSA_stepst::const_iterator>> &writes,
  symex_target_equationt::SSA_stepst::const_iterator current_read)
{
  bool var_contained = false;
  for(unsigned thread_nr = 1; thread_nr < writes.size(); ++thread_nr)
  {
    for(const auto &s_it : writes.at(thread_nr))
    {
      if(
        s_it->ssa_lhs.get_object_name() ==
        current_read->ssa_lhs.get_object_name())
      {
        var_contained = true;
        break;
      }
    }
  }
  return var_contained;
}

void lazy_c_seqt::create_main_read_constraints(
  symex_target_equationt &equation,
  std::unordered_map<
    irep_idt,
    symex_target_equationt::SSA_stepst::const_iterator> &last_update,
  std::vector<symex_target_equationt::SSA_stepst::const_iterator> &main_reads,
  message_handlert &message_handler)
{
  messaget log{message_handler};
  log.warning() << "-------------------MAIN READS--------------------------"
                << messaget::eom;

  for(auto read : main_reads)
  {
    std::string variable_name =
      id2string(
        to_symbol_expr(last_update[read->ssa_lhs.get_object_name()]->ssa_lhs)
          .get_identifier()) +
      "_L" +
      std::to_string(last_update[read->ssa_lhs.get_object_name()]
                       ->source.pc->location_number) +
      "_R" + std::to_string(rounds);

    symbol_exprt last_update_expr{
      variable_name,
      last_update[read->ssa_lhs.get_object_name()]->ssa_lhs.type()};

    equal_exprt constraint{read->ssa_lhs, last_update_expr};

    log.warning() << format(constraint) << messaget::eom;
    equation.constraint(constraint, "read constraint", read->source);
  }
}

void lazy_c_seqt::create_cs_constraint(
  symex_target_equationt &equation,
  std::vector<std::pair<
    symex_target_equationt::SSA_stepst::const_iterator,
    std::optional<symex_target_equationt::SSA_stepst::const_iterator>>> &reads,
  std::unordered_map<
    unsigned,
    std::vector<symex_target_equationt::SSA_stepst::const_iterator>> &writes,
  message_handlert &message_handler)
{
  messaget log{message_handler};
  log.warning() << "-------------------CS--------------------------"
                << messaget::eom;

  for(unsigned thread = 1; thread < writes.size(); ++thread)
  {
    exprt previous;
    int max_read = 0;
    for(auto &read : reads)
    {
      if(read.first->source.thread_nr == thread && (int)reinterpret_cast<unsigned>(read.first->source.pc->location_number) > max_read)
      {
        max_read = reinterpret_cast<unsigned>(read.first->source.pc->location_number);
      }
    }
    int max_write = 0;
    for(auto &write : writes.at(thread))
    {
      if(write->source.thread_nr == thread && (int)reinterpret_cast<unsigned>(write->source.pc->location_number) > max_read)
      {
        max_read = reinterpret_cast<unsigned>(write->source.pc->location_number);
      }
    }
    int max_num = max_read > max_write ? max_read + 1 : max_write + 1;
    for(size_t round = 1; round <= rounds; ++round)
    {
      irep_idt cs_name =
        "cs_T" + std::to_string(thread) + "_R" + std::to_string(round);
      symbol_exprt cs{cs_name, unsignedbv_typet{8}}; //TODO: check type

      if(round == 1)
      {
        exprt zero{
          from_integer({0}, unsignedbv_typet{8})}; //TODO: check corretness
        less_than_or_equal_exprt constraint{zero, cs};
        log.warning() << format(constraint) << messaget::eom;
        equation.constraint(
          constraint,
          "cs constraint",
          equation.SSA_steps.begin()->source); //TODO: check source
        previous = cs;
        continue;
      }
      less_than_or_equal_exprt constraint{previous, cs};
      log.warning() << format(constraint) << messaget::eom;
      equation.constraint(
        constraint,
        "cs constraint",
        equation.SSA_steps.begin()->source); //TODO: check source
      previous = cs;
      if(round == rounds)
      {
        exprt max{from_integer(
          {max_num}, unsignedbv_typet{8})}; //TODO: check corretness
        less_than_or_equal_exprt constraint{cs, max};
        log.warning() << format(constraint) << messaget::eom;
        equation.constraint(
          constraint,
          "cs constraint",
          equation.SSA_steps.begin()->source); //TODO: check source
        previous = cs;

        irep_idt thread_exited_name =
          "thread_exited_T" + std::to_string(thread);
        symbol_exprt thread_exited{thread_exited_name, bool_typet{}};
        equal_exprt thread_exited_constraint{
          thread_exited,
          greater_than_or_equal_exprt{
            cs, from_integer({max_num}, unsignedbv_typet{8})}};
        log.warning() << format(thread_exited_constraint) << messaget::eom;
        equation.constraint(
          thread_exited_constraint,
          "cs constraint",
          equation.SSA_steps.begin()->source); //TODO: check source
      }
    }
  }
  for(unsigned thread = 1; thread < writes.size(); ++thread)
  {
    for(const auto &write : writes.at(thread))
    {
      for(size_t round = 1; round <= rounds; ++round)
      {
        std::string label_name =
          "_L" + std::to_string(write->source.pc->location_number);
        std::string round_curr_name = "_R" + std::to_string(round);
        std::string round_prev_name = "_R" + std::to_string(round - 1);
        std::string thread_name = "_T" + std::to_string(thread);

        int label_int =
          reinterpret_cast<unsigned>(write->source.pc->location_number);
        exprt label{from_integer({label_int}, unsignedbv_typet{8})};

        irep_idt statement_label_name = "E" + label_name + round_curr_name;
        symbol_exprt statement_label{statement_label_name, bool_typet{}};

        irep_idt cs_curr_name = "cs" + thread_name + round_curr_name;
        symbol_exprt cs_curr{cs_curr_name, unsignedbv_typet{8}};

        irep_idt cs_prev_name = "cs" + thread_name + round_prev_name;
        symbol_exprt cs_prev{cs_prev_name, unsignedbv_typet{8}};

        irep_idt active_thread_name = "active_thread" + thread_name + round_curr_name;
        symbol_exprt active_thread{active_thread_name, bool_typet{}};

        greater_than_exprt expr_1{cs_curr, label};
        exprt expr_2;
        if(round == 1)
          expr_2 = true_exprt{};
        else
        {
          expr_2 = less_than_or_equal_exprt{cs_prev, label};
        }
        and_exprt expr_3{expr_1, expr_2};
        and_exprt expr_4{true_exprt{}, expr_3};
        and_exprt expr_5{expr_4, write->guard};
        equal_exprt constraint{statement_label, expr_5};
        simplify(constraint, ns);

        log.warning() << format(constraint) << messaget::eom;
        equation.constraint(constraint, "cs constraint", write->source);
      }
    }
  }
  for(auto read : reads)
  {
    for(size_t round = 1; round <= rounds; ++round)
    {
      std::string label_name =
        "_L" + std::to_string(read.first->source.pc->location_number);
      std::string round_curr_name = "_R" + std::to_string(round);
      std::string round_prev_name = "_R" + std::to_string(round - 1);
      std::string thread_name =
        "_T" + std::to_string(read.first->source.thread_nr);

      int label_int =
        reinterpret_cast<unsigned>(read.first->source.pc->location_number);
      exprt label{from_integer({label_int}, unsignedbv_typet{8})};

      irep_idt statement_label_name = "E" + label_name + round_curr_name;
      symbol_exprt statement_label{statement_label_name, bool_typet{}};

      irep_idt cs_curr_name = "cs" + thread_name + round_curr_name;
      symbol_exprt cs_curr{cs_curr_name, unsignedbv_typet{8}};

      irep_idt cs_prev_name = "cs" + thread_name + round_prev_name;
      symbol_exprt cs_prev{cs_prev_name, unsignedbv_typet{8}};

      irep_idt active_thread_name = "active_thread" + thread_name + round_curr_name;
      symbol_exprt active_thread{active_thread_name, bool_typet{}};

      greater_than_exprt expr_1{cs_curr, label};
      exprt expr_2;
      if(round == 1)
        expr_2 = true_exprt{};
      else
      {
        expr_2 = less_than_or_equal_exprt{cs_prev,label};
      }
      and_exprt expr_3{expr_1, expr_2};
      and_exprt expr_4{true_exprt{}, expr_3};
      and_exprt expr_5{expr_4, read.first->guard};
      equal_exprt constraint{statement_label, expr_5};
      simplify(constraint, ns);

      log.warning() << format(constraint) << messaget::eom;
      equation.constraint(constraint, "cs constraint", read.first->source);
    }
  }
}

void lazy_c_seqt::create_reach_constraint(
  symex_target_equationt &equation,
  std::vector<std::pair<
    symex_target_equationt::SSA_stepst::const_iterator,
    std::optional<symex_target_equationt::SSA_stepst::const_iterator>>> &reads,
  std::unordered_map<
    unsigned,
    std::vector<symex_target_equationt::SSA_stepst::const_iterator>> &writes,
  exprt &exited_array,
  message_handlert &message_handler)
{
  messaget log{message_handler};

  log.warning() << "-------------------REACH--------------------------"
                << messaget::eom;

  for(auto &read : reads)
  {
    std::string label_name =
      "_L" + std::to_string(read.first->source.pc->location_number);
    std::string thread_name =
      "_T" + std::to_string(read.first->source.thread_nr);
    std::string round_name = "_R" + std::to_string(writes.size());

    irep_idt statement_label_name = "E" + label_name + round_name;
    symbol_exprt statement_label{statement_label_name, bool_typet{}};

    exprt previous_expr = statement_label;
    exprt constraint;
    for(std::size_t round = rounds - 1; round >= 1; --round)
    {
      std::string round_name = "_R" + std::to_string(round);

      irep_idt statement_label_name = "E" + label_name + round_name;
      symbol_exprt statement_label{statement_label_name, bool_typet{}};

      or_exprt temp_constraint{statement_label, previous_expr};
      constraint = temp_constraint;
      previous_expr = constraint;
    }

    irep_idt reach_name = "reach" + label_name + thread_name;
    symbol_exprt reach{reach_name, bool_typet{}};

    equal_exprt final_constraint{reach, constraint};
    simplify(final_constraint, ns);
    log.warning() << format(final_constraint) << messaget::eom;
    equation.constraint(
      final_constraint, "reach constraint", read.first->source);

    index_exprt exited{
      exited_array,
      from_integer({read.first->source.thread_nr}, unsignedbv_typet{8})};
    equal_exprt reach_constraint{reach, exited};
    simplify(reach_constraint, ns);
    log.warning() << format(reach_constraint) << messaget::eom;
    equation.constraint(
      reach_constraint, "reach constraint", read.first->source);
  }

  for(unsigned thread_nr = 1; thread_nr < writes.size(); ++thread_nr)
  {
    for(auto &write : writes.at(thread_nr))
    {
      std::string label_name =
        "_L" + std::to_string(write->source.pc->location_number);
      std::string thread_name = "_T" + std::to_string(write->source.thread_nr);
      std::string round_name = "_R" + std::to_string(writes.size());

      irep_idt statement_label_name = "E" + label_name + round_name;
      symbol_exprt statement_label{statement_label_name, bool_typet{}};

      exprt previous_expr = statement_label;
      exprt constraint;
      for(std::size_t round = rounds - 1; round >= 1; --round)
      {
        std::string round_name = "_R" + std::to_string(round);

        irep_idt statement_label_name = "E" + label_name + round_name;
        symbol_exprt statement_label{statement_label_name, bool_typet{}};

        or_exprt temp_constraint{statement_label, previous_expr};
        constraint = temp_constraint;
        previous_expr = constraint;
      }
      irep_idt reach_name = "reach" + label_name + thread_name;
      symbol_exprt reach{reach_name, bool_typet{}};

      equal_exprt final_constraint{reach, constraint};
      simplify(final_constraint, ns);
      log.warning() << format(final_constraint) << messaget::eom;
      equation.constraint(final_constraint, "reach constraint", write->source);

      index_exprt exited{
        exited_array, from_integer({thread_nr}, unsignedbv_typet{8})};
      equal_exprt reach_constraint{reach, exited};
      simplify(reach_constraint, ns);
      log.warning() << format(reach_constraint) << messaget::eom;
      equation.constraint(reach_constraint, "reach constraint", write->source);
    }
  }
}

void lazy_c_seqt::handling_guards(
  symex_target_equationt &equation,
  message_handlert &message_handler)
{
  messaget log{message_handler};

  log.warning() << "-------------------GUARDS--------------------------"
                << messaget::eom;

  symex_target_equationt temp_equation{equation};
  temp_equation.clear();

  auto ssa_steps = equation.SSA_steps;

  symex_target_equationt::SSA_stepst::const_iterator previous_shared_event;

  int thread = 1;

  for(symex_target_equationt::SSA_stepst::const_iterator s_it =
        ssa_steps.begin();
      s_it != ssa_steps.end();
      s_it++)
  {
    bool skip = false;
    const std::string &file =
      id2string(s_it->source.pc->source_location().get_file());
    if(
      file.find("builtin-library") != std::string::npos ||
      file.find("built-in-additions") != std::string::npos)
    {
      skip = true;
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
        skip = true;
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
            skip = true;
          }
        }
      }
      if(!can_cast_expr<symbol_exprt>(s_it->ssa_lhs))
      {
        skip = true;
      }
    }

    /*if(s_it->is_assignment() && !s_it->ssa_lhs.get_level_0().empty()) //TODO: skip non global assignment
    {
      for(auto operand : s_it->ssa_rhs.operands())
      {
        if(to_ssa_expr(operand).get_level_0().empty())
        {
          skip = true;
        }
      }
    }*/

    if(
      (s_it->is_assert() || s_it->is_assume() /*|| s_it->is_shared_read() ||
       s_it->is_shared_write() || s_it->is_assignment()*/) &&
      s_it->source.thread_nr > 0 && !skip)
    {
      exprt guard = s_it->guard;
      /*if(s_it->is_shared_read() || s_it->is_shared_write())
      {
        //log.warning() << "s_it->guard: " << format(s_it->guard) << messaget::eom;

        std::string label_name =
          "_L" + std::to_string(s_it->source.pc->location_number);
        std::string thread_name = "_T" + std::to_string(s_it->source.thread_nr);

        irep_idt reach_name = "reach" + label_name + thread_name;
        symbol_exprt reach{
          reach_name,
          bool_typet{}}; //TODO: we already have it, is that ok or we have to take it from the previous functions?
        and_exprt new_guard{reach, guard};
        simplify(new_guard, this->ns);

        SSA_stept step = equation.SSA_steps.front();
        equation.SSA_steps.pop_front();
        step.guard = new_guard;
        temp_equation.SSA_steps.emplace_back(step);

        log.warning() << format(step.get_ssa_expr()) << messaget::eom;
        log.warning() << "guard: " << format(step.guard) << messaget::eom;
        previous_reach = reach;
      }

      if(s_it->is_assignment())
      {
        std::string label_name =
          "_L" + std::to_string(s_it->source.pc->location_number);
        std::string thread_name = "_T" + std::to_string(s_it->source.thread_nr);

        irep_idt reach_name = "reach" + label_name + thread_name;
        symbol_exprt reach{
          reach_name,
          bool_typet{}}; //TODO: we already have it, is that ok or we have to take it from the previous functions?
        and_exprt new_guard{reach, guard};
        simplify(new_guard, this->ns);

        SSA_stept step = equation.SSA_steps.front();
        equation.SSA_steps.pop_front();
        step.guard = new_guard;
        temp_equation.SSA_steps.emplace_back(step);

        log.warning() << format(step.get_ssa_expr()) << messaget::eom;
        log.warning() << "guard: " << format(step.guard) << messaget::eom;
      }*/

      if(s_it->is_assert() || s_it->is_assume())
      {
        std::string label_name =
          "_L" +
          std::to_string(previous_shared_event->source.pc->location_number);
        std::string thread_name =
          "_T" + std::to_string(previous_shared_event->source.thread_nr);

        irep_idt reach_name = "reach" + label_name + thread_name;
        symbol_exprt previous_reach{reach_name, bool_typet{}};

        and_exprt new_guard{previous_reach, guard};
        simplify(new_guard, ns);

        SSA_stept step = equation.SSA_steps.front();
        equation.SSA_steps.pop_front();
        step.guard = new_guard;
        //log.warning() << "s_it->cond_expr: " << format(s_it->cond_expr.operands().at(s_it->cond_expr.operands().size()-1)) << messaget::eom;
        step.cond_expr = implies_exprt{
          new_guard,
          s_it->cond_expr};
        temp_equation.SSA_steps.emplace_back(step);

        log.warning() << format(step.get_ssa_expr()) << messaget::eom;
        log.warning() << "guard: " << format(step.guard) << messaget::eom;
      }
    }
    else
    {
      if((s_it->is_shared_read() || s_it->is_shared_write()) && !skip)
        previous_shared_event = s_it;

      SSA_stept step = equation.SSA_steps.front();

      if(file.find("builtin-library-__spawned_thread") != std::string::npos)
      {
        std::stringstream rhs_stream;
        rhs_stream << format(s_it->ssa_rhs);
        std::string rhs = rhs_stream.str();
        //log.warning() << "rhs: " << rhs << messaget::eom;
        //log.warning() << "step: " << format(s_it->get_ssa_expr()) << messaget::eom;

        if(
          s_it->is_assignment() &&
          s_it->ssa_lhs.get_object_name() == "__CPROVER_threads_exited" &&
          rhs.find("with") != std::string::npos)
        {
          with_exprt old_rhs = to_with_expr(step.ssa_rhs);

          irep_idt thread_exited_name =
            "thread_exited_T" + std::to_string(thread);
          thread++;
          symbol_exprt thread_exited{thread_exited_name, bool_typet{}};

          with_exprt new_rhs{old_rhs.old(), old_rhs.where(), thread_exited};

          step.ssa_rhs = new_rhs;
          step.cond_expr = equal_exprt{step.ssa_lhs, step.ssa_rhs};

          log.warning() << format(step.get_ssa_expr()) << messaget::eom;
          log.warning() << "guard: " << format(step.guard) << messaget::eom;
        }
      }
      equation.SSA_steps.pop_front();
      temp_equation.SSA_steps.emplace_back(step);
    }
  }
  equation = temp_equation;
}

void lazy_c_seqt::create_cprover_constraints(
  symex_target_equationt &equation,
  std::unordered_map<
    irep_idt,
    symex_target_equationt::SSA_stepst::const_iterator> &last_cprover_upadte,
  exprt &exited_array,
  message_handlert &message_handler)
{
  //std::vector<symex_target_equationt::SSA_stepst::const_iterator> atomic_op;
  messaget log{message_handler};
  log.warning() << "-------------------CPROVER--------------------------"
                << messaget::eom;

  auto ssa_steps = equation.SSA_steps;
  //bool atomic_section = false;
  for(symex_target_equationt::SSA_stepst::const_iterator s_it =
        ssa_steps.begin();
      s_it != ssa_steps.end();
      s_it++)
  {
    const std::string &file =
      id2string(s_it->source.pc->source_location().get_file());
    if(!(file.find("builtin-library") != std::string::npos ||
         file.find("built-in-additions") != std::string::npos))
    {
      /*if(s_it->is_atomic_begin()) //TODO: manage atomic sections
      {
        atomic_section = true;
      }
      if(s_it->is_atomic_end())
      {
        exprt or_expr{false_exprt{}};
        for(size_t round = 1; round <= rounds; ++round)
        {
          exprt and_expr{true_exprt{}};
          for(auto op : atomic_op)
          {
            irep_idt statement_name = "E_L" + std::to_string(op->source.pc->location_number) + "_R" + std::to_string(round);
            symbol_exprt statement{statement_name, bool_typet{}};

            and_exprt and_temp_expr{statement, and_expr};
            and_expr = and_temp_expr;
          }
          or_exprt or_temp_expr{and_expr, or_expr};
          or_expr = or_temp_expr;
        }
        simplify(or_expr, ns);
        log.warning() << format(or_expr) << messaget::eom;
        equation.constraint(or_expr, "atomic constraint", s_it->source);
        atomic_op.clear();
        atomic_section = false;
      }
      if(atomic_section)
      {
        atomic_op.emplace_back(s_it);
      }*/
      continue;
    }
    if(s_it->is_shared_write())
    {
      if(s_it->ssa_lhs.get_object_name() == "__CPROVER_threads_exited")
        exited_array = s_it->ssa_lhs;
      last_cprover_upadte[s_it->ssa_lhs.get_object_name()] = s_it;
    }
    if(s_it->is_shared_read())
    {
      irep_idt previous_name =
        id2string(s_it->ssa_lhs.get_object_name()) + "#" +
        id2string(last_cprover_upadte[s_it->ssa_lhs.get_object_name()]
                    ->ssa_lhs.get_level_2());
      symbol_exprt previous{
        previous_name,
        last_cprover_upadte[s_it->ssa_lhs.get_object_name()]->ssa_lhs.type()};

      equal_exprt constraint{previous, s_it->ssa_lhs};
      log.warning() << format(constraint) << messaget::eom;
      equation.constraint(constraint, "cprover constraint", s_it->source);
    }
  }
}

void lazy_c_seqt::collect_reads_and_writes(
  const symex_target_equationt::SSA_stepst &ssa_steps,
  std::vector<std::pair<
    symex_target_equationt::SSA_stepst::const_iterator,
    std::optional<symex_target_equationt::SSA_stepst::const_iterator>>> &reads,
  std::vector<symex_target_equationt::SSA_stepst::const_iterator> &main_reads,
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
        if(
          previous_write.count(s_it->ssa_lhs.get_object_name()) == 0 ||
          s_it->source.thread_nr == 0)
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

        if(s_it->source.thread_nr >= 1)
        {
          if(previous_write.count(s_it->ssa_lhs.get_object_name()) == 0)
            reads.emplace_back(std::pair(s_it, std::nullopt));
          else
          {
            reads.emplace_back(
              std::pair(s_it, previous_write[s_it->ssa_lhs.get_object_name()]));
          }
        }
        else
        {
          if(s_it->ssa_lhs.get_level_0().empty())
          {
            log.warning() << "Main Read " << messaget::eom;
            main_reads.emplace_back(s_it);
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