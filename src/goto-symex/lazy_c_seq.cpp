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

  collect_reads_and_writes(equation.SSA_steps, message_handler);

  create_write_constraints(equation, message_handler);

  create_read_constraints(equation, message_handler);

  create_cs_constraint(equation, message_handler);

  create_reach_constraint(equation, message_handler);

  handling_guards(equation, message_handler);

  exprt tmp;
  simplify(tmp, ns);
}

void lazy_c_seqt::create_write_constraints(
  symex_target_equationt &equation,
  message_handlert &message_handler)
{
  messaget log{message_handler};

  log.warning() << "-------------------WRITES--------------------------"
                << messaget::eom;

  for(auto global_variable : global_variables)
  {
    if(this->writes.count(global_variable) == 0)
      continue;
    exprt previous = this->writes.at(global_variable).front().s_it->ssa_lhs;
    lazy_variable lazy_struct = lazy_variable{
      global_variable,
      0,
      0,
      this->writes.at(global_variable).front().s_it->ssa_lhs};
    this->lazy_variables[global_variable].emplace_back(lazy_struct);
    this->writes.at(global_variable)
      .erase(this->writes.at(global_variable).begin());
    for(std::size_t round = 1; round <= rounds; ++round)
    {
      for(const auto write : this->writes.at(global_variable))
      {
        std::string suffix =
          "_L" + std::to_string(write.label) + "_R" + std::to_string(round);
        irep_idt lazy_variable_name =
          id2string(to_symbol_expr(write.s_it->ssa_lhs).get_identifier()) +
          suffix;
        const symbol_exprt lazy_variable_exprt{
          lazy_variable_name, write.s_it->ssa_lhs.type()};
        lazy_variable lazy_struct = lazy_variable{
          global_variable, round, write.label, lazy_variable_exprt};
        this->lazy_variables[global_variable].emplace_back(lazy_struct);

        irep_idt exec_name = "E" + suffix;
        const symbol_exprt exec{exec_name, bool_typet{}};

        equal_exprt constraint{
          lazy_variable_exprt, if_exprt{exec, write.s_it->ssa_lhs, previous}};

        log.warning() << format(constraint) << messaget::eom;
        equation.constraint(constraint, "write constraint", write.s_it->source);

        previous = lazy_variable_exprt;
      }
    }
  }
}

void lazy_c_seqt::create_read_constraints(
  symex_target_equationt &equation,
  message_handlert &message_handler)
{
  messaget log{message_handler};
  log.warning() << "-------------------READS--------------------------"
                << messaget::eom;

  for(auto global_variable : global_variables)
  {
    if(this->reads.count(global_variable) == 0)
      continue;
    for(const auto read : this->reads.at(global_variable))
    {
      exprt temp_constraint = read.s_it->ssa_lhs;
      for(std::size_t round = rounds; round >= 1; --round)
      {
        std::string suffix =
          "_L" + std::to_string(read.label) + "_R" + std::to_string(round);
        irep_idt exec_name = "E" + suffix;
        const symbol_exprt exec{exec_name, bool_typet{}};

        temp_constraint = if_exprt{
          exec, previous(global_variable, read.label, round), temp_constraint};
      }
      equal_exprt final_constraint{read.s_it->ssa_lhs, temp_constraint};
      log.warning() << format(final_constraint) << messaget::eom;
      equation.constraint(
        final_constraint, "read constraint", read.s_it->source);
    }
  }
}

symbol_exprt
lazy_c_seqt::previous(irep_idt variable, unsigned label, std::size_t round)
{
  symbol_exprt previous = lazy_variables.at(variable).front().symbol;
  for(const auto &lazy_variable : lazy_variables.at(variable))
  {
    if(round > lazy_variable.round)
    {
      previous = lazy_variable.symbol;
      continue;
    }
    if(label > lazy_variable.label)
    {
      previous = lazy_variable.symbol;
      continue;
    }
    return previous;
  }
  return previous;
}

void lazy_c_seqt::create_cs_constraint(
  symex_target_equationt &equation,
  message_handlert &message_handler)
{
  messaget log{message_handler};
  log.warning() << "-------------------CS--------------------------"
                << messaget::eom;

  for(unsigned thread = 0; thread <= threads; ++thread)
  {
    exprt previous;
    unsigned max_read = 0;
    unsigned min_read = std::numeric_limits<int>::max();
    unsigned max_write = 0;
    unsigned min_write = std::numeric_limits<int>::max();
    for(auto global_variable : global_variables)
    {
      if(this->reads.count(global_variable) != 0)
      {
        for(auto &read : this->reads.at(global_variable))
        {
          if(read.s_it->source.thread_nr == thread && read.label > max_read)
          {
            max_read = read.label;
          }
          if(read.s_it->source.thread_nr == thread && read.label < min_read)
          {
            min_read = read.label;
          }
        }
      }
      if(this->writes.count(global_variable) != 0)
      {
        for(auto &write : this->writes.at(global_variable))
        {
          if(write.s_it->source.thread_nr == thread && write.label > max_write)
          {
            max_write = write.label;
          }
          if(write.s_it->source.thread_nr == thread && write.label < min_write)
          {
            min_write = write.label;
          }
        }
      }
    }
    unsigned max_num = max_read > max_write ? max_read + 1 : max_write + 1;
    unsigned min_num = min_read < min_write ? min_read : min_write;
    for(size_t round = 1; round <= rounds; ++round)
    {
      irep_idt cs_name =
        "cs_T" + std::to_string(thread) + "_R" + std::to_string(round);
      symbol_exprt cs{cs_name, unsignedbv_typet{8}}; //TODO: check type

      if(round == 1)
      {
        exprt min{from_integer(
          {min_num}, unsignedbv_typet{8})}; //TODO: check corretness
        less_than_or_equal_exprt constraint{min, cs};
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
      }
    }
  }
  for(auto global_variable : global_variables)
  {
    if(this->writes.count(global_variable) != 0)
    {
      for(const auto &write : this->writes.at(global_variable))
      {
        for(size_t round = 1; round <= rounds; ++round)
        {
          std::string label_name = "_L" + std::to_string(write.label);
          std::string round_curr_name = "_R" + std::to_string(round);
          std::string round_prev_name = "_R" + std::to_string(round - 1);
          std::string thread_name =
            "_T" + std::to_string(write.s_it->source.thread_nr);

          unsigned label_int = write.label;
          exprt label{from_integer({label_int}, unsignedbv_typet{8})};

          irep_idt statement_label_name = "E" + label_name + round_curr_name;
          symbol_exprt statement_label{statement_label_name, bool_typet{}};

          irep_idt cs_curr_name = "cs" + thread_name + round_curr_name;
          symbol_exprt cs_curr{cs_curr_name, unsignedbv_typet{8}};

          irep_idt cs_prev_name = "cs" + thread_name + round_prev_name;
          symbol_exprt cs_prev{cs_prev_name, unsignedbv_typet{8}};

          irep_idt active_thread_name =
            "active_thread" + thread_name + round_curr_name;
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
          and_exprt expr_5{expr_4, write.s_it->guard};
          equal_exprt constraint{statement_label, expr_5};
          simplify(constraint, ns);

          log.warning() << format(constraint) << messaget::eom;
          equation.constraint(constraint, "cs constraint", write.s_it->source);
        }
      }
    }
    if(this->reads.count(global_variable) != 0)
    {
      for(auto read : this->reads.at(global_variable))
      {
        for(size_t round = 1; round <= rounds; ++round)
        {
          std::string label_name = "_L" + std::to_string(read.label);
          std::string round_curr_name = "_R" + std::to_string(round);
          std::string round_prev_name = "_R" + std::to_string(round - 1);
          std::string thread_name =
            "_T" + std::to_string(read.s_it->source.thread_nr);

          unsigned label_int = read.label;
          exprt label{from_integer({label_int}, unsignedbv_typet{8})};

          irep_idt statement_label_name = "E" + label_name + round_curr_name;
          symbol_exprt statement_label{statement_label_name, bool_typet{}};

          irep_idt cs_curr_name = "cs" + thread_name + round_curr_name;
          symbol_exprt cs_curr{cs_curr_name, unsignedbv_typet{8}};

          irep_idt cs_prev_name = "cs" + thread_name + round_prev_name;
          symbol_exprt cs_prev{cs_prev_name, unsignedbv_typet{8}};

          irep_idt active_thread_name =
            "active_thread" + thread_name +
            round_curr_name; //TODO: implement active thread variables
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
          and_exprt expr_5{expr_4, read.s_it->guard};
          equal_exprt constraint{statement_label, expr_5};
          simplify(constraint, ns);

          log.warning() << format(constraint) << messaget::eom;
          equation.constraint(constraint, "cs constraint", read.s_it->source);
        }
      }
    }
  }
}

void lazy_c_seqt::create_reach_constraint(
  symex_target_equationt &equation,
  message_handlert &message_handler)
{
  messaget log{message_handler};

  log.warning() << "-------------------REACH--------------------------"
                << messaget::eom;

  std::unordered_map<
    unsigned,
    std::vector<exprt>> events;

  for(auto global_variable : global_variables)
  {
    if(this->reads.count(global_variable) != 0)
    {
      for(auto &read : this->reads.at(global_variable))
      {
        std::string label_name = "_L" + std::to_string(read.label);
        std::string thread_name =
          "_T" + std::to_string(read.s_it->source.thread_nr);
        std::string round_name = "_R" + std::to_string(rounds);

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
        events[read.s_it->source.thread_nr].emplace_back(reach);

        equal_exprt final_constraint{reach, constraint};
        simplify(final_constraint, ns);
        log.warning() << format(final_constraint) << messaget::eom;
        equation.constraint(
          final_constraint, "reach constraint", read.s_it->source);
      }
    }

    if(this->writes.count(global_variable) != 0)
    {
      for(auto &write : this->writes.at(global_variable))
      {
        std::string label_name = "_L" + std::to_string(write.label);
        std::string thread_name =
          "_T" + std::to_string(write.s_it->source.thread_nr);
        std::string round_name = "_R" + std::to_string(rounds);

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
        events[write.s_it->source.thread_nr].emplace_back(reach);

        equal_exprt final_constraint{reach, constraint};
        simplify(final_constraint, ns);
        log.warning() << format(final_constraint) << messaget::eom;
        equation.constraint(
          final_constraint, "reach constraint", write.s_it->source);
      }
    }
  }

  //TODO: for each thread we need to see which Crover_thread_exited version is in the guard ad use it in the code below
  /*for(unsigned thread_nr = 0; thread_nr <= threads; ++thread_nr)
  {
    exprt and_constraint{true_exprt{}};

    for(auto &event : events.at(thread_nr))
    {
      and_constraint = and_exprt{and_constraint, event};
    }
    index_exprt exited{exited_array, from_integer({thread_nr}, unsignedbv_typet{8})};
    equal_exprt reach_constraint{true_exprt{}, and_constraint};
    simplify(reach_constraint, ns);
    log.warning() << format(reach_constraint) << messaget::eom;
    equation.constraint(
      reach_constraint,
      "reach constraint",
      equation.SSA_steps.begin()->source);
  }*/
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

  for(symex_target_equationt::SSA_stepst::const_iterator s_it =
        ssa_steps.begin();
      s_it != ssa_steps.end();
      s_it++)
  {
    exprt guard = s_it->guard;

    if(s_it->is_assert() || s_it->is_assume())
    {
      shared_event previous_event = previous_events.front();
      previous_events.erase(previous_events.begin());

      SSA_stept step = equation.SSA_steps.front();
      equation.SSA_steps.pop_front();

      if(previous_event.s_it != ssa_steps.begin())
      {
        std::string label_name = "_L" + std::to_string(previous_event.label);
        std::string thread_name =
          "_T" + std::to_string(previous_event.s_it->source.thread_nr);

        irep_idt reach_name = "reach" + label_name + thread_name;
        symbol_exprt previous_reach{reach_name, bool_typet{}};

        and_exprt new_guard{previous_reach, guard};
        simplify(new_guard, ns);
        step.guard = new_guard;
        step.cond_expr = implies_exprt{new_guard, s_it->cond_expr};

        log.warning() << format(step.get_ssa_expr()) << messaget::eom;
        log.warning() << "guard: " << format(step.guard) << messaget::eom;
      }
      temp_equation.SSA_steps.emplace_back(step);
    }
    else
    {
      SSA_stept step = equation.SSA_steps.front();

      equation.SSA_steps.pop_front();
      temp_equation.SSA_steps.emplace_back(step);
    }
  }
  equation = temp_equation;
}

void lazy_c_seqt::collect_reads_and_writes(
  const symex_target_equationt::SSA_stepst &ssa_steps,
  message_handlert &message_handler)
{
  messaget log{message_handler};

  unsigned label = 1;

  shared_event previous_event{ssa_steps.begin()};

  for(symex_target_equationt::SSA_stepst::const_iterator s_it =
        ssa_steps.begin();
      s_it != ssa_steps.end();
      s_it++)
  {
    if(s_it->source.thread_nr > threads)
      threads = s_it->source.thread_nr;

    if(s_it->is_assert() || s_it->is_assume())
    {
      previous_events.emplace_back(previous_event);
    }

    if(s_it->is_shared_write())
    {
      // TODO: this may be too restrictive
      if(can_cast_expr<symbol_exprt>(s_it->ssa_lhs))
      {
        shared_event shared_event{s_it, label};
        label++;
        previous_event = shared_event;

        log.warning()
          << "Thread: " << shared_event.s_it->source.thread_nr
          << "\tWrite: " << shared_event.label << "   \t"
          << to_symbol_expr(shared_event.s_it->ssa_lhs).get_identifier()
          << "\tL" << shared_event.label << messaget::eom;
        this->writes[s_it->ssa_lhs.get_object_name()].emplace_back(
          shared_event);
        this->global_variables.insert(s_it->ssa_lhs.get_object_name());
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
        shared_event shared_event{s_it, label};
        label++;
        previous_event = shared_event;

        log.warning()
          << "Thread: " << shared_event.s_it->source.thread_nr
          << "\tRead: " << shared_event.label << "   \t"
          << to_symbol_expr(shared_event.s_it->ssa_lhs).get_identifier()
          << "\tL" << shared_event.label << messaget::eom;

        this->reads[s_it->ssa_lhs.get_object_name()].emplace_back(shared_event);
        this->global_variables.insert(s_it->ssa_lhs.get_object_name());
      }
      else
      {
        log.warning() << "Skipping: " << "Thread: " << s_it->source.thread_nr
                      << "\tRead: " << s_it->source.pc->location_number
                      << messaget::eom;
      }
    }
  }
}