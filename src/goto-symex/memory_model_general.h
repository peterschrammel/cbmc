/*******************************************************************\

Module: Memory models for C11 memory model

(would be later extended to all memory models)

Author: Zhihang Sun, sunzh20@mails.tsinghua.edu.cn

\*******************************************************************/

#ifndef CPROVER_GOTO_SYMEX_MEMORY_MODEL_C11_H
#define CPROVER_GOTO_SYMEX_MEMORY_MODEL_C11_H

#include "memory_model.h"

class memory_model_generalt:public memory_model_baset
{
public:
  explicit memory_model_generalt(const namespacet &_ns, cat_modulet& _cat, bool _strict_guard, bool _enable_cutting) : 
    memory_model_baset(_ns), cat(_cat), strict_guard(_strict_guard), enable_cutting(_enable_cutting) {}

  virtual void operator()(symex_target_equationt &equation, message_handlert &);

  void add_necessary_oc_edge(symex_target_equationt &equation, event_it e1, event_it e2, std::string kind, exprt guard_expr);
  void add_necessary_oc_edge(symex_target_equationt &equation, std::string e1_str, std::string e2_str, std::string kind, exprt guard_expr);
  void add_necessary_oc_label(symex_target_equationt &equation, event_it e, std::string label, exprt guard_expr);
  void add_necessary_oc_label(symex_target_equationt &equation, std::string e_str, std::string label, exprt guard_expr);
  void add_apo(symex_target_equationt &equation, event_it e1, event_it e2, std::set<event_it>& added_rmw_labels);

  void build_labels(symex_target_equationt &equation);
  void build_lkmm_nesting(symex_target_equationt &equation);
  void build_loc(symex_target_equationt &equation);
  void build_thd(symex_target_equationt &equation);
  void build_id(symex_target_equationt &equation);

  bool is_mm_apo(event_it e1, event_it e2);

  void build_po(symex_target_equationt &equation);
  void build_rf(symex_target_equationt &equation);
  void build_co(symex_target_equationt &equation);

  void build_shared_reads(symex_target_equationt &equation, std::set<std::string>& reads);
  void build_guard_map(symex_target_equationt& equation, std::map<symbol_exprt, exprt>& guard_map);
  void build_data(symex_target_equationt &equation, std::set<std::string>& reads);
  void build_addr(symex_target_equationt &equation, std::set<std::string>& reads);
  void build_ctrl(symex_target_equationt &equation, std::set<std::string>& reads);

  std::vector<event_it> lkmm_locks;
  std::vector<event_it> lkmm_unlocks;
  void build_lkmm_crit(symex_target_equationt &equation);

  // for cutting

  bool really_need_cutting();
  void build_cutting(symex_target_equationt &equation);

  void remove_atomic_typecast(symex_target_equationt &equation);
  
protected:
  virtual exprt before(event_it e1, event_it e2);

private:
  cat_modulet cat;
  bool strict_guard;
  bool enable_cutting;
};
#endif // CPROVER_GOTO_SYMEX_MEMORY_MODEL_C11_H