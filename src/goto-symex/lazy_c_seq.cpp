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
  exprt exited_array = nil_exprt{};

  collect_reads_and_writes(equation.SSA_steps, message_handler);

  create_write_constraints(equation, message_handler);

  create_read_constraints(equation, message_handler);

  create_cs_constraint(equation, message_handler);

  create_reach_constraint(equation, exited_array, message_handler);

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
    exprt previous = this->writes.at(global_variable).front()->ssa_lhs;
    lazy_variable lazy_struct = lazy_variable{
      global_variable, 0, 0, this->writes.at(global_variable).front()->ssa_lhs};
    this->lazy_variables[global_variable].emplace_back(lazy_struct);
    this->writes.at(global_variable)
      .erase(this->writes.at(global_variable).begin());
    for(std::size_t round = 1; round <= rounds; ++round)
    {
      for(const auto write : this->writes.at(global_variable))
      {
        std::string suffix = "_L" +
                             std::to_string(write->source.pc->location_number) +
                             "_R" + std::to_string(round);
        irep_idt lazy_variable_name =
          id2string(to_symbol_expr(write->ssa_lhs).get_identifier()) + suffix;
        const symbol_exprt lazy_variable_exprt{
          lazy_variable_name, write->ssa_lhs.type()};
        lazy_variable lazy_struct = lazy_variable{
          global_variable,
          round,
          write->source.pc->location_number,
          lazy_variable_exprt};
        this->lazy_variables[global_variable].emplace_back(lazy_struct);

        irep_idt exec_name = "E" + suffix;
        const symbol_exprt exec{exec_name, bool_typet{}};

        equal_exprt constraint{
          lazy_variable_exprt, if_exprt{exec, write->ssa_lhs, previous}};

        log.warning() << format(constraint) << messaget::eom;
        equation.constraint(constraint, "write constraint", write->source);

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
      exprt temp_constraint = read->ssa_lhs;
      for(std::size_t round = rounds; round >= 1; --round)
      {
        std::string suffix = "_L" +
                             std::to_string(read->source.pc->location_number) +
                             "_R" + std::to_string(round);
        irep_idt exec_name = "E" + suffix;
        const symbol_exprt exec{exec_name, bool_typet{}};

        temp_constraint = if_exprt{
          exec,
          previous(global_variable, read->source.pc->location_number, round),
          temp_constraint};
      }
      equal_exprt final_constraint{read->ssa_lhs, temp_constraint};
      log.warning() << format(final_constraint) << messaget::eom;
      equation.constraint(final_constraint, "read constraint", read->source);
    }
  }
}

symbol_exprt
lazy_c_seqt::previous(irep_idt variable, unsigned location, std::size_t round)
{
  symbol_exprt previous = lazy_variables.at(variable).front().symbol;
  for(const auto &lazy_variable : lazy_variables.at(variable))
  {
    if(round > lazy_variable.round)
    {
      previous = lazy_variable.symbol;
      continue;
    }
    if(location > lazy_variable.location)
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
    int max_read = 0;
    int min_read = std::numeric_limits<int>::max();
    int max_write = 0;
    int min_write = std::numeric_limits<int>::max();
    for(auto global_variable : global_variables)
    {
      if(this->reads.count(global_variable) != 0)
      {
        for(auto &read : this->reads.at(global_variable))
        {
          if(
            read->source.thread_nr == thread &&
            (int)reinterpret_cast<unsigned>(read->source.pc->location_number) >
              max_read)
          {
            max_read =
              reinterpret_cast<unsigned>(read->source.pc->location_number);
          }
          if(
            read->source.thread_nr == thread &&
            (int)reinterpret_cast<unsigned>(read->source.pc->location_number) <
              min_read)
          {
            min_read =
              reinterpret_cast<unsigned>(read->source.pc->location_number);
          }
        }
      }
      if(this->writes.count(global_variable) != 0)
      {
        for(auto &write : this->writes.at(global_variable))
        {
          if(
            write->source.thread_nr == thread &&
            (int)reinterpret_cast<unsigned>(write->source.pc->location_number) >
              max_write)
          {
            max_write =
              reinterpret_cast<unsigned>(write->source.pc->location_number);
          }
          if(
            write->source.thread_nr == thread &&
            (int)reinterpret_cast<unsigned>(write->source.pc->location_number) <
              min_write)
          {
            min_write =
              reinterpret_cast<unsigned>(write->source.pc->location_number);
          }
        }
      }
    }
    int max_num = max_read > max_write ? max_read + 1 : max_write + 1;
    int min_num = min_read < min_write ? min_read : min_write;
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
          std::string label_name =
            "_L" + std::to_string(write->source.pc->location_number);
          std::string round_curr_name = "_R" + std::to_string(round);
          std::string round_prev_name = "_R" + std::to_string(round - 1);
          std::string thread_name =
            "_T" + std::to_string(write->source.thread_nr);

          int label_int =
            reinterpret_cast<unsigned>(write->source.pc->location_number);
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
          and_exprt expr_5{expr_4, write->guard};
          equal_exprt constraint{statement_label, expr_5};
          simplify(constraint, ns);

          log.warning() << format(constraint) << messaget::eom;
          equation.constraint(constraint, "cs constraint", write->source);
        }
      }
    }
    if(this->reads.count(global_variable) != 0)
    {
      for(auto read : this->reads.at(global_variable))
      {
        for(size_t round = 1; round <= rounds; ++round)
        {
          std::string label_name =
            "_L" + std::to_string(read->source.pc->location_number);
          std::string round_curr_name = "_R" + std::to_string(round);
          std::string round_prev_name = "_R" + std::to_string(round - 1);
          std::string thread_name =
            "_T" + std::to_string(read->source.thread_nr);

          int label_int =
            reinterpret_cast<unsigned>(read->source.pc->location_number);
          exprt label{from_integer({label_int}, unsignedbv_typet{8})};

          irep_idt statement_label_name = "E" + label_name + round_curr_name;
          symbol_exprt statement_label{statement_label_name, bool_typet{}};

          irep_idt cs_curr_name = "cs" + thread_name + round_curr_name;
          symbol_exprt cs_curr{cs_curr_name, unsignedbv_typet{8}};

          irep_idt cs_prev_name = "cs" + thread_name + round_prev_name;
          symbol_exprt cs_prev{cs_prev_name, unsignedbv_typet{8}};

          irep_idt active_thread_name =
            "active_thread" + thread_name +
            round_curr_name; //TODO: active_thread = !cprover_thread_exited
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
          and_exprt expr_5{expr_4, read->guard};
          equal_exprt constraint{statement_label, expr_5};
          simplify(constraint, ns);

          log.warning() << format(constraint) << messaget::eom;
          equation.constraint(constraint, "cs constraint", read->source);
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
        std::string label_name =
          "_L" + std::to_string(read->source.pc->location_number);
        std::string thread_name = "_T" + std::to_string(read->source.thread_nr);
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
        events[read->source.thread_nr].emplace_back(reach);

        equal_exprt final_constraint{reach, constraint};
        simplify(final_constraint, ns);
        log.warning() << format(final_constraint) << messaget::eom;
        equation.constraint(final_constraint, "reach constraint", read->source);
      }
    }

    if(this->writes.count(global_variable) != 0)
    {
      for(auto &write : this->writes.at(global_variable))
      {
        std::string label_name =
          "_L" + std::to_string(write->source.pc->location_number);
        std::string thread_name =
          "_T" + std::to_string(write->source.thread_nr);
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
        events[write->source.thread_nr].emplace_back(reach);

        equal_exprt final_constraint{reach, constraint};
        simplify(final_constraint, ns);
        log.warning() << format(final_constraint) << messaget::eom;
        equation.constraint(
          final_constraint, "reach constraint", write->source);
      }
    }
  }

  /*if(exited_array != nil_exprt{}) //TODO: for each thread (less the main) we need to see which Crover_thread_exited version is in the guard ad use it in the code below
  {
    for(unsigned thread_nr = 1; thread_nr < writes.size(); ++thread_nr)
    {
      exprt and_constraint{true_exprt{}};
      for(auto &event : events.at(thread_nr))
      {
        and_constraint = and_exprt{and_constraint, event};
      }
      index_exprt exited{
        exited_array, from_integer({thread_nr}, unsignedbv_typet{8})};
      equal_exprt reach_constraint{exited, and_constraint};
      simplify(reach_constraint, ns);
      log.warning() << format(reach_constraint) << messaget::eom;
      equation.constraint(
        reach_constraint,
        "reach constraint",
        equation.SSA_steps.begin()->source);
    }
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

  symex_target_equationt::SSA_stepst::const_iterator previous_shared_event;

  for(symex_target_equationt::SSA_stepst::const_iterator s_it =
        ssa_steps.begin();
      s_it != ssa_steps.end();
      s_it++)
  {
    exprt guard = s_it->guard;

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
      step.cond_expr = implies_exprt{new_guard, s_it->cond_expr};
      temp_equation.SSA_steps.emplace_back(step);

      log.warning() << format(step.get_ssa_expr()) << messaget::eom;
      log.warning() << "guard: " << format(step.guard) << messaget::eom;
      }
      else
      {
        if(s_it->is_shared_read() || s_it->is_shared_write())
          previous_shared_event = s_it;

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
    if(s_it->source.thread_nr > threads)
      threads = s_it->source.thread_nr;

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
        this->writes[s_it->ssa_lhs.get_object_name()].emplace_back(s_it);
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
        log.warning() << "Thread: " << s_it->source.thread_nr
                      << "\tRead: " << s_it->source.pc->location_number
                      << "     \t"
                      << to_symbol_expr(s_it->ssa_lhs).get_identifier() << "\tL"
                      << s_it->source.pc->location_number << messaget::eom;

        this->reads[s_it->ssa_lhs.get_object_name()].emplace_back(s_it);
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
  }
}