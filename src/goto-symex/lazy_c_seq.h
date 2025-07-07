/// \file
/// LazyCSeq context-bounded concurrency SSA transformation

#ifndef CPROVER_GOTO_SYMEX_LAZY_C_SEQ_H
#define CPROVER_GOTO_SYMEX_LAZY_C_SEQ_H

#include "symex_target_equation.h"

class lazy_c_seqt
{
public:
  explicit lazy_c_seqt(const namespacet &ns, const std::size_t rounds, const bool check_rounds)
    : ns(ns), rounds(rounds), check_rounds(check_rounds)
  {
  }

  void operator()(symex_target_equationt &, message_handlert &);

private:
  const namespacet &ns;
  const std::size_t rounds;
  const bool check_rounds;
  struct shared_event
  {
    symex_target_equationt::SSA_stepst::const_iterator s_it;
    unsigned label;
  };
  struct lazy_variable
  {
    std::size_t round;
    unsigned label;
    symbol_exprt symbol;
  };
  struct active_thread
  {
    unsigned thread;
    std::size_t l2;
    symbol_exprt symbol;
  };

  std::size_t threads = 0;
  std::unordered_set<irep_idt> global_variables;
  std::unordered_map<irep_idt, std::vector<shared_event>> writes;
  std::unordered_map<irep_idt, std::vector<shared_event>> reads;
  std::unordered_map<irep_idt, std::vector<lazy_variable>> lazy_variables;
  std::unordered_map<unsigned, active_thread> active_threads_vector;
  std::vector<shared_event> previous_events;
  std::vector<std::pair<std::size_t, std::size_t>> atomic_sections;
  std::unordered_map<std::size_t, std::size_t> label_to_thread;
  std::vector<symbol_exprt> reach_events;

  unsigned n_bit;

  void handling_active_threads(
    symex_target_equationt &equation,
    message_handlert &message_handler);

  void handling_atomic_sections(
    symex_target_equationt &equation,
    message_handlert &message_handler);

  void collect_reads_and_writes(
    const symex_target_equationt::SSA_stepst &ssa_steps,
    message_handlert &message_handler);

  void create_write_constraints(
    symex_target_equationt &equation,
    message_handlert &message_handler);

  void create_read_constraints(
    symex_target_equationt &equation,
    message_handlert &message_handler);

  std::optional<symbol_exprt>
  previous_shared(irep_idt variable, unsigned label, std::size_t round);

  void create_cs_constraint(
    symex_target_equationt &equation,
    message_handlert &message_handler);

  void create_reach_constraint(
    symex_target_equationt &equation,
    message_handlert &message_handler);

  void handling_guards(
    symex_target_equationt &equation,
    message_handlert &message_handler);

  symbol_exprt
  create_lazy_symbol(unsigned label, std::size_t round, ssa_exprt lhs, typet type);

  symbol_exprt create_exec_symbol(unsigned label, std::size_t round);

  symbol_exprt create_enabled_symbol(unsigned label, std::size_t round);

  symbol_exprt create_cs_symbol(std::size_t thread, std::size_t round);

  symbol_exprt create_reach_symbol(unsigned label, std::size_t thread);

  symbol_exprt create_active_thread_symbol(unsigned thread);

  void create_active_thread_statements(
    const symex_targett::sourcet &source,
    exprt &guard,
    unsigned int atomic_section_id,
    unsigned &thread,
    symex_target_equationt &equation,
    message_handlert &message_handler,
    const exprt &value);
};

#endif //CPROVER_GOTO_SYMEX_LAZY_C_SEQ_H
