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

  handling_active_threads(equation, message_handler);

  collect_reads_and_writes(equation.SSA_steps, message_handler);

  create_write_constraints(equation, message_handler);

  create_read_constraints(equation, message_handler);

  create_cs_constraint(equation, message_handler);

  handling_atomic_sections(equation, message_handler);

  handling_guards(equation, message_handler);

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
    lazy_variable first_lazy_struct = lazy_variable{
      0,
      0,
      this->writes.at(global_variable).front().s_it->ssa_lhs};
    this->lazy_variables[global_variable].emplace_back(first_lazy_struct);
    this->writes.at(global_variable)
      .erase(this->writes.at(global_variable).begin());
    for(std::size_t round = 1; round <= rounds; ++round)
    {
      for(const auto write : this->writes.at(global_variable))
      {
        const symbol_exprt lazy_variable_exprt = create_lazy_symbol(
          write.label, round, write.s_it->ssa_lhs, write.s_it->ssa_lhs.type());
        lazy_variable lazy_struct = lazy_variable{round, write.label, lazy_variable_exprt};
        this->lazy_variables[global_variable].emplace_back(lazy_struct);

        const symbol_exprt exec = create_exec_symbol(write.label, round);

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
        const symbol_exprt exec = create_exec_symbol(read.label, round);

        std::optional<symbol_exprt> previous =
          previous_shared(global_variable, read.label, round);
        if(previous.has_value())
        {
          temp_constraint = if_exprt{exec, previous.value(), temp_constraint};
        }
      }
      equal_exprt final_constraint{read.s_it->ssa_lhs, temp_constraint};
      log.warning() << format(final_constraint) << messaget::eom;
      equation.constraint(
        final_constraint, "read constraint", read.s_it->source);
    }
  }
}

std::optional<symbol_exprt> lazy_c_seqt::previous_shared(
  irep_idt variable,
  unsigned label,
  std::size_t round)
{
  if(lazy_variables.count(variable) == 0)
    return std::nullopt;
  symbol_exprt previous = lazy_variables.at(variable).front().symbol;
  for(const auto &lazy_variable : lazy_variables.at(variable))
  {
    if(round > lazy_variable.round)
    {
      previous = lazy_variable.symbol;
      continue;
    }
    if(round == lazy_variable.round && label > lazy_variable.label)
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
      symbol_exprt cs = create_cs_symbol(thread, round);

      if(round == 1)
      {
        exprt min{from_integer(
          {min_num-1}, unsignedbv_typet{n_bit})};
        less_than_or_equal_exprt constraint{min, cs};
        log.warning() << format(constraint) << messaget::eom;
        equation.constraint(
          constraint,
          "cs constraint",
          equation.SSA_steps.begin()->source);
        previous = cs;
      }
      else {
        less_than_or_equal_exprt constraint{previous, cs};
        log.warning() << format(constraint) << messaget::eom;
        equation.constraint(
          constraint,
          "cs constraint",
          equation.SSA_steps.begin()->source);
        previous = cs;
      }
      if(round == rounds)
      {
        exprt max{from_integer(
          {max_num+1}, unsignedbv_typet{n_bit})};
        less_than_or_equal_exprt last_constraint{cs, max};
        log.warning() << format(last_constraint) << messaget::eom;
        equation.constraint(
          last_constraint,
          "cs constraint",
          equation.SSA_steps.begin()->source);
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
          unsigned label_int = write.label;
          exprt label{from_integer({label_int}, unsignedbv_typet{n_bit})};

          symbol_exprt exec = create_exec_symbol(write.label, round);

          symbol_exprt enabled = create_enabled_symbol(write.label, round);

          symbol_exprt cs_curr =
            create_cs_symbol(write.s_it->source.thread_nr, round);

          symbol_exprt cs_prev =
            create_cs_symbol(write.s_it->source.thread_nr, round - 1);

          std::string active_name =
            "active_thread_T" + std::to_string(write.s_it->source.thread_nr);
          std::optional<symbol_exprt> active_thread =
            previous_shared(active_name, write.label, round);
          exprt active_thread_value = true_exprt{};
          if(active_thread.has_value())
          {
            active_thread_value = active_thread.value();
          }

          greater_than_exprt expr_1{cs_curr, label};
          exprt expr_2;
          if(round == 1)
            expr_2 = true_exprt{};
          else
          {
            expr_2 = less_than_or_equal_exprt{cs_prev, label};
          }
          and_exprt expr_3{expr_1, expr_2};
          equal_exprt enabled_expr{enabled, expr_3};
          simplify(enabled_expr, ns);
          log.warning() << format(enabled_expr) << messaget::eom;
          equation.constraint(
            enabled_expr, "cs constraint", write.s_it->source);

          implies_exprt active_expr{enabled, active_thread_value};
          simplify(active_expr, ns);
          log.warning() << format(active_expr) << messaget::eom;
          equation.constraint(active_expr, "cs constraint", write.s_it->source);

          and_exprt expr_5{enabled, write.s_it->guard};
          equal_exprt constraint{exec, expr_5};
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
          unsigned label_int = read.label;
          exprt label{from_integer({label_int}, unsignedbv_typet{n_bit})};

          symbol_exprt exec = create_exec_symbol(read.label, round);

          symbol_exprt enabled = create_enabled_symbol(read.label, round);

          symbol_exprt cs_curr =
            create_cs_symbol(read.s_it->source.thread_nr, round);

          symbol_exprt cs_prev =
            create_cs_symbol(read.s_it->source.thread_nr, round - 1);

          std::string active_name =
            "active_thread_T" + std::to_string(read.s_it->source.thread_nr);
          std::optional<symbol_exprt> active_thread =
            previous_shared(active_name, read.label, round);
          exprt active_thread_value = true_exprt{};
          if(active_thread.has_value())
          {
            active_thread_value = active_thread.value();
          }

          greater_than_exprt expr_1{cs_curr, label};
          exprt expr_2;
          if(round == 1)
            expr_2 = true_exprt{};
          else
          {
            expr_2 = less_than_or_equal_exprt{cs_prev, label};
          }
          and_exprt expr_3{expr_1, expr_2};
          equal_exprt enabled_expr{enabled, expr_3};
          simplify(enabled_expr, ns);
          log.warning() << format(enabled_expr) << messaget::eom;
          equation.constraint(enabled_expr, "cs constraint", read.s_it->source);

          implies_exprt active_expr{enabled, active_thread_value};
          simplify(active_expr, ns);
          log.warning() << format(active_expr) << messaget::eom;
          equation.constraint(active_expr, "cs constraint", read.s_it->source);

          and_exprt expr_5{enabled, read.s_it->guard};
          equal_exprt constraint{exec, expr_5};
          simplify(constraint, ns);
          log.warning() << format(constraint) << messaget::eom;
          equation.constraint(constraint, "cs constraint", read.s_it->source);
        }
      }
    }
  }
  for(auto blocking_event : blocking_events)
  {
    for(size_t round = 1; round <= rounds; ++round)
    {
      unsigned label_int = blocking_event.label;
      exprt label{from_integer({label_int}, unsignedbv_typet{n_bit})};

      symbol_exprt exec = create_exec_symbol(blocking_event.label, round);

      symbol_exprt enabled = create_enabled_symbol(blocking_event.label, round);

      symbol_exprt cs_curr =
        create_cs_symbol(blocking_event.s_it->source.thread_nr, round);

      symbol_exprt cs_prev =
        create_cs_symbol(blocking_event.s_it->source.thread_nr, round - 1);

      std::string active_name =
        "active_thread_T" +
        std::to_string(blocking_event.s_it->source.thread_nr);
      std::optional<symbol_exprt> active_thread =
        previous_shared(active_name, blocking_event.label, round);
      exprt active_thread_value = true_exprt{};
      if(active_thread.has_value())
      {
        active_thread_value = active_thread.value();
      }

      greater_than_exprt expr_1{cs_curr, label};
      exprt expr_2;
      if(round == 1)
        expr_2 = true_exprt{};
      else
      {
        expr_2 = less_than_or_equal_exprt{cs_prev, label};
      }
      and_exprt expr_3{expr_1, expr_2};
      equal_exprt enabled_expr{enabled, expr_3};
      simplify(enabled_expr, ns);
      log.warning() << format(enabled_expr) << messaget::eom;
      equation.constraint(
        enabled_expr, "cs constraint", blocking_event.s_it->source);

      implies_exprt active_expr{enabled, active_thread_value};
      simplify(active_expr, ns);
      log.warning() << format(active_expr) << messaget::eom;
      equation.constraint(
        active_expr, "cs constraint", blocking_event.s_it->source);

      and_exprt expr_5{enabled, blocking_event.s_it->guard};
      equal_exprt constraint{exec, expr_5};
      simplify(constraint, ns);
      log.warning() << format(constraint) << messaget::eom;
      equation.constraint(
        constraint, "cs constraint", blocking_event.s_it->source);
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

  for(auto global_variable : global_variables)
  {
    if(this->reads.count(global_variable) != 0)
    {
      for(auto &read : this->reads.at(global_variable))
      {
        exprt constraint = false_exprt{};

        for(std::size_t round = 1; round <= rounds; round++)
        {
          symbol_exprt enabled = create_enabled_symbol(read.label, round);

          constraint = or_exprt{constraint, enabled};
        }

        simplify(constraint, ns);

        symbol_exprt reach =
          create_reach_symbol(read.label, read.s_it->source.thread_nr);

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
        exprt constraint = false_exprt{};

        for(std::size_t round = 1; round <= rounds; round++)
        {
          symbol_exprt enabled = create_enabled_symbol(write.label, round);

          constraint = or_exprt{constraint, enabled};
        }

        symbol_exprt reach =
          create_reach_symbol(write.label, write.s_it->source.thread_nr);

        equal_exprt final_constraint{reach, constraint};
        simplify(final_constraint, ns);
        log.warning() << format(final_constraint) << messaget::eom;
        equation.constraint(
          final_constraint, "reach constraint", write.s_it->source);
      }
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

  for(symex_target_equationt::SSA_stepst::const_iterator s_it =
        ssa_steps.begin();
      s_it != ssa_steps.end();
      s_it++)
  {
    exprt guard = s_it->guard;

    if(s_it->is_assert() || s_it->is_assume())
    {
      shared_event blocking_event = blocking_events.front();
      blocking_events.erase(blocking_events.begin());
      SSA_stept step{equation.SSA_steps.front()};
      equation.SSA_steps.pop_front();
      symbol_exprt reach = create_reach_symbol(
        blocking_event.label, blocking_event.s_it->source.thread_nr);

      exprt constraint = false_exprt{};

      for(std::size_t round = 1; round <= rounds; round++)
      {
        symbol_exprt exec = create_exec_symbol(blocking_event.label, round);

        constraint = or_exprt{constraint, exec};
      }

      simplify(constraint, ns);

      equal_exprt final_constraint{reach, constraint};
      simplify(final_constraint, ns);
      log.warning() << format(final_constraint) << messaget::eom;
      temp_equation.constraint(
        final_constraint,
        "blocking statement constraint",
        blocking_event.s_it->source);

      exprt new_guard = reach;
      step.guard = new_guard;
      exprt new_cond = s_it->cond_expr;
      exprt new_expr = implies_exprt{new_guard, new_cond};
      simplify(new_expr, ns);
      step.cond_expr = new_expr;
      log.warning() << format(step.get_ssa_expr()) << messaget::eom;
      log.warning() << "guard: " << format(step.guard) << messaget::eom;
      temp_equation.SSA_steps.emplace_back(step);
    }
    else
    {
      SSA_stept step{equation.SSA_steps.front()};

      equation.SSA_steps.pop_front();
      temp_equation.SSA_steps.emplace_back(step);
    }
  }
  equation = temp_equation;
}

void lazy_c_seqt::handling_atomic_sections(
  symex_target_equationt &equation,
  message_handlert &message_handler)
{
  messaget log{message_handler};

  log.warning()
    << "-------------------ATOMIC SECTIONS--------------------------"
    << messaget::eom;

  for(auto atomic_section : atomic_sections)
  {
    log.warning() << "atomic section: L" << atomic_section.first << " : L"
                  << atomic_section.second << messaget::eom;
    exprt constraint;

    for(std::size_t round = 1; round <= rounds; round++)
    {
      symbol_exprt cs =
        create_cs_symbol(label_to_thread.at(atomic_section.first), round);
      constraint = or_exprt{
        less_than_exprt{
          cs, from_integer(atomic_section.first, unsignedbv_typet{n_bit})},
        greater_than_exprt{
          cs, from_integer(atomic_section.second, unsignedbv_typet{n_bit})}};

      log.warning() << format(constraint) << messaget::eom;
      equation.constraint(
        constraint, "atomic constraint", equation.SSA_steps.begin()->source);
    }
  }
}

void lazy_c_seqt::handling_active_threads(
  symex_target_equationt &equation,
  message_handlert &message_handler)
{
  messaget log{message_handler};

  log.warning() << "-------------------ACTIVE THREAD--------------------------"
                << messaget::eom;

  symex_target_equationt temp_equation{equation};
  temp_equation.clear();

  auto ssa_steps = equation.SSA_steps;

  unsigned thread_current = 999;

  for(symex_target_equationt::SSA_stepst::const_iterator s_it =
        ssa_steps.begin();
      s_it != ssa_steps.end();
      s_it++)
  {
    exprt guard = s_it->guard;

    if(s_it->source.thread_nr != thread_current)
      thread_current = s_it->source.thread_nr;
  }

  exprt guard = true_exprt{};

  for(unsigned thread = 0; thread <= thread_current; thread++)
  {
    create_active_thread_symbol(thread);
    if(thread == 0)
      create_active_thread_statements(
        ssa_steps.begin()->source,
        guard,
        ssa_steps.begin()->atomic_section_id,
        thread,
        temp_equation,
        message_handler,
        true_exprt{});
    else
      create_active_thread_statements(
        ssa_steps.begin()->source,
        guard,
        ssa_steps.begin()->atomic_section_id,
        thread,
        temp_equation,
        message_handler,
        false_exprt{});
  }

  unsigned thread_created = 1;
  thread_current = 999;

  for(symex_target_equationt::SSA_stepst::const_iterator s_it =
        ssa_steps.begin();
      s_it != ssa_steps.end();
      s_it++)
  {
    guard = s_it->guard;

    if(s_it->source.thread_nr != thread_current)
      thread_current = s_it->source.thread_nr;

    if(s_it->is_function_call() && s_it->called_function == "pthread_create")
    {
      create_active_thread_statements(
        s_it->source,
        guard,
        s_it->atomic_section_id,
        thread_created,
        temp_equation,
        message_handler,
        true_exprt{});

      thread_created++;

      SSA_stept step{equation.SSA_steps.front()};

      equation.SSA_steps.pop_front();
      temp_equation.SSA_steps.emplace_back(step);
    }
    else
    {
      if(
        s_it->source.thread_nr != 0 && s_it->is_shared_write() &&
        s_it->ssa_lhs.get_object_name() == "__CPROVER_threads_exited")
      {
        SSA_stept step{equation.SSA_steps.front()};

        equation.SSA_steps.pop_front();
        temp_equation.SSA_steps.emplace_back(step);

        create_active_thread_statements(
          s_it->source,
          guard,
          s_it->atomic_section_id,
          thread_current,
          temp_equation,
          message_handler,
          false_exprt{});
      }
      else
      {
        if(
          s_it->source.thread_nr == 0 && s_it->is_assignment() &&
          s_it->ssa_lhs.get_object_name() == "return'")
        {
          SSA_stept step{equation.SSA_steps.front()};

          equation.SSA_steps.pop_front();
          temp_equation.SSA_steps.emplace_back(step);

          create_active_thread_statements(
            s_it->source,
            guard,
            s_it->atomic_section_id,
            thread_current,
            temp_equation,
            message_handler,
            false_exprt{});
        }
        else
        {
          SSA_stept step{equation.SSA_steps.front()};

          equation.SSA_steps.pop_front();
          temp_equation.SSA_steps.emplace_back(step);
        }
      }
    }
  }
  equation = temp_equation;
}

void lazy_c_seqt::create_active_thread_statements(
  const symex_targett::sourcet &source,
  exprt &guard,
  unsigned int atomic_section_id,
  unsigned &thread,
  symex_target_equationt &equation,
  message_handlert &message_handler,
  const exprt &value)
{
  messaget log{message_handler};

  SSA_stept event_step{source, goto_trace_stept::typet::SHARED_WRITE};
  event_step.guard = guard;
  ssa_exprt event_expr{active_threads_vector.at(thread).symbol};
  event_expr.set_level_2(active_threads_vector.at(thread).l2);
  event_step.ssa_lhs = event_expr;
  event_step.atomic_section_id = atomic_section_id;
  equation.SSA_steps.emplace_back(event_step);
  log.warning() << format(event_step.get_ssa_expr()) << messaget::eom;

  SSA_stept active_step{source, goto_trace_stept::typet::ASSIGNMENT};
  active_step.guard = guard;
  ssa_exprt active_expr{active_threads_vector.at(thread).symbol};
  active_expr.set_level_2(active_threads_vector.at(thread).l2);
  active_threads_vector.at(thread).l2++;
  active_step.ssa_lhs = active_expr;
  active_step.ssa_rhs = value;
  active_step.cond_expr = equal_exprt{active_step.ssa_lhs, active_step.ssa_rhs};
  active_step.assignment_type =
    symex_targett::assignment_typet::VISIBLE_ACTUAL_PARAMETER; //TODO: check
  active_step.atomic_section_id = atomic_section_id;
  equation.SSA_steps.emplace_back(active_step);
  log.warning() << format(active_step.get_ssa_expr()) << messaget::eom;
}

void lazy_c_seqt::collect_reads_and_writes(
  const symex_target_equationt::SSA_stepst &ssa_steps,
  message_handlert &message_handler)
{
  messaget log{message_handler};

  log.warning() << "-------------------COLLECTING--------------------------"
                << messaget::eom;

  unsigned label = 1;


  for(symex_target_equationt::SSA_stepst::const_iterator s_it =
        ssa_steps.begin();
      s_it != ssa_steps.end();
      s_it++)
  {
    if(s_it->source.thread_nr > threads)
      threads = s_it->source.thread_nr;

    if(s_it->is_assert() || s_it->is_assume())
    {
      label_to_thread[label] = s_it->source.thread_nr;
      shared_event shared_event{s_it, label};
      n_bit = 0 ? 0 : 32 - __builtin_clz(label+1);
      label++;

      log.warning() << "Thread: " << shared_event.s_it->source.thread_nr
                    << "\tBlocking statement: " << shared_event.label << "\t"
                    << format(s_it->cond_expr) << messaget::eom;

      this->blocking_events.emplace_back(shared_event);
    }

    if(s_it->is_atomic_begin())
    {
      label++;
      atomic_sections.emplace_back(label, NULL);
    }

    if(s_it->is_atomic_end())
    {
      atomic_sections.back().second = label;
      label++;
      for(auto atomic_write : atomic_writes)
      {
        this->writes[atomic_write.first].emplace_back(atomic_write.second);
      }
      atomic_writes.clear();
    }

    if(s_it->is_shared_write()) {
      // TODO: this may be too restrictive
      if(can_cast_expr<symbol_exprt>(s_it->ssa_lhs))
      {
        label_to_thread[label] = s_it->source.thread_nr;
        shared_event shared_event{s_it, label};
        n_bit = 0 ? 0 : 32 - __builtin_clz(label+1);
        label++;

        log.warning()
          << "Thread: " << shared_event.s_it->source.thread_nr
          << "\tWrite: " << shared_event.label << "   \t"
          << to_symbol_expr(shared_event.s_it->ssa_lhs).get_identifier()
          << "\tL" << shared_event.label << messaget::eom;
        if(s_it->atomic_section_id == 0)
        {
          this->writes[shared_event.s_it->ssa_lhs.get_l1_object_identifier()]
            .emplace_back(shared_event);
          this->global_variables.emplace(
            shared_event.s_it->ssa_lhs.get_l1_object_identifier());
        }
        else
        {
          atomic_writes[shared_event.s_it->ssa_lhs.get_l1_object_identifier()] =
            shared_event;
          this->global_variables.emplace(
            shared_event.s_it->ssa_lhs.get_l1_object_identifier());
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
    if(s_it->is_shared_read())
    {
      // TODO: this may be too restrictive
      if(can_cast_expr<symbol_exprt>(s_it->ssa_lhs))
      {
        label_to_thread[label] = s_it->source.thread_nr;
        shared_event shared_event{s_it, label};
        n_bit = 0 ? 0 : 32 - __builtin_clz(label+1);
        label++;

        log.warning()
          << "Thread: " << shared_event.s_it->source.thread_nr
          << "\tRead: " << shared_event.label << "   \t"
          << to_symbol_expr(shared_event.s_it->ssa_lhs).get_identifier()
          << "\tL" << shared_event.label << messaget::eom;

        this->reads[shared_event.s_it->ssa_lhs.get_l1_object_identifier()].emplace_back(shared_event);
        this->global_variables.insert(shared_event.s_it->ssa_lhs.get_l1_object_identifier());
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

symbol_exprt lazy_c_seqt::create_lazy_symbol(
  unsigned label,
  size_t round,
  ssa_exprt lhs,
  typet type)
{
  std::string suffix =
    "_L" + std::to_string(label) + "_R" + std::to_string(round);
  irep_idt lazy_variable_name =
    id2string(to_symbol_expr(lhs).get_identifier()) + suffix;
  symbol_exprt lazy_variable_exprt{lazy_variable_name, type};

  return lazy_variable_exprt;
}

symbol_exprt lazy_c_seqt::create_exec_symbol(unsigned label, size_t round)
{
  for(const auto &exec : exec_vector)
  {
    if(exec.label == label && exec.round == round)
      return exec.symbol;
  }
  irep_idt exec_name =
    "Ex_L" + std::to_string(label) + "_R" + std::to_string(round);
  symbol_exprt exec_symbol{exec_name, bool_typet{}};

  exec exec_struct{label, round, exec_symbol};
  exec_vector.emplace_back(exec_struct);

  return exec_symbol;
}

symbol_exprt lazy_c_seqt::create_enabled_symbol(unsigned label, size_t round)
{
  for(const auto &enabled : enabled_vector)
  {
    if(enabled.label == label && enabled.round == round)
      return enabled.symbol;
  }
  irep_idt enabled_name =
    "En_L" + std::to_string(label) + "_R" + std::to_string(round);
  symbol_exprt enabled_symbol{enabled_name, bool_typet{}};

  enabled enabled_struct{label, round, enabled_symbol};
  enabled_vector.emplace_back(enabled_struct);

  return enabled_symbol;
}

symbol_exprt lazy_c_seqt::create_cs_symbol(size_t thread, size_t round)
{
  for(const auto &cs : cs_vector)
  {
    if(cs.thread == thread && cs.round == round)
      return cs.symbol;
  }
  irep_idt cs_name =
    "cs_T" + std::to_string(thread) + "_R" + std::to_string(round);
  symbol_exprt cs_symbol{cs_name, unsignedbv_typet{n_bit}};

  cs cs_struct{thread, round, cs_symbol};
  cs_vector.emplace_back(cs_struct);

  return cs_symbol;
}

symbol_exprt lazy_c_seqt::create_reach_symbol(unsigned label, size_t thread)
{
  for(const auto &reach : reach_vector)
  {
    if(reach.label == label && reach.thread == thread)
      return reach.symbol;
  }
  irep_idt reach_name =
    "reach_L" + std::to_string(label) + "_T" + std::to_string(thread);
  symbol_exprt reach_symbol{reach_name, bool_typet{}};

  reach reach_struct{label, thread, reach_symbol};
  reach_vector.emplace_back(reach_struct);

  return reach_symbol;
}

symbol_exprt lazy_c_seqt::create_active_thread_symbol(unsigned thread)
{
  irep_idt active_thread_name = "active_thread_T" + std::to_string(thread);
  symbol_exprt active_thread_expr{active_thread_name, bool_typet{}};

  active_thread active_thread_struct{thread, 1, active_thread_expr};
  active_threads_vector.emplace(thread, active_thread_struct);

  return active_thread_expr;
}