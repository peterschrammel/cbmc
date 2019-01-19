/*******************************************************************\

Module: Goto Trace Storage

Author: Daniel Kroening, Peter Schrammel

\*******************************************************************/

/// \file
/// Goto Trace Storage

#ifndef CPROVER_GOTO_CHECKER_GOTO_TRACE_STORAGE_H
#define CPROVER_GOTO_CHECKER_GOTO_TRACE_STORAGE_H

#include <goto-programs/goto_trace.h>

class goto_trace_storaget
{
public:
  goto_trace_storaget(const namespacet &);
  goto_trace_storaget(const goto_trace_storaget &) = delete;

  const goto_tracet &insert(goto_tracet &&);

  const std::vector<goto_tracet> &all() const;
  const goto_tracet &operator[](const irep_idt &property_id) const;

  const namespacet &get_namespace() const;

protected:
  /// the namespace related to the traces
  const namespacet &ns;

  /// stores the traces
  std::vector<goto_tracet> traces;

  // maps property ID to index in error_traces
  std::unordered_map<irep_idt, std::size_t> property_to_trace;
};

#endif // CPROVER_GOTO_CHECKER_GOTO_TRACE_STORAGE_H