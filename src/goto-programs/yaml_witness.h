/*******************************************************************\

Module: Violation Witnesses in Witness Format 2.x

Author: Peter Schrammel

\*******************************************************************/

/// \file
/// Violation witnesses in the YAML format (version 2) of
/// https://gitlab.com/sosy-lab/benchmarking/sv-witnesses

#ifndef CPROVER_GOTO_PROGRAMS_YAML_WITNESS_H
#define CPROVER_GOTO_PROGRAMS_YAML_WITNESS_H

#include <util/irep.h>

#include <iosfwd>
#include <string>
#include <vector>

class cmdlinet;
class exprt;
class goto_tracet;
class goto_trace_stept;
class namespacet;
class optionst;
class source_locationt;

#define OPT_YAML_WITNESS "(witness):"

#define HELP_YAML_WITNESS                                                      \
  " {y--witness} {ufilename} \t write a violation witness in witness format "  \
  "2.x to {ufilename} ({y-} for stdout)\n"

void parse_yaml_witness_options(const cmdlinet &, optionst &);

/// Builds a violation witness from a counterexample trace.
/// The witness consists of a `function_return` waypoint for each
/// nondeterministic value returned by a `__VERIFIER_nondet_*` function (C) or
/// an `org.sosy_lab.sv_benchmarks.Verifier.nondet*` method (Java), followed by
/// a `target` waypoint for the violated property.
///
/// Locations are given by file and line as recorded by the front end; the
/// front ends do not record columns. The metadata of the verification task,
/// i.e., the input files, their hashes and the specification, are not known
/// here and need to be added by the caller to obtain a complete witness.
class yaml_violation_witnesst
{
public:
  explicit yaml_violation_witnesst(const namespacet &);

  void operator()(const goto_tracet &);

  void output(std::ostream &) const;

protected:
  struct waypointt
  {
    std::string type;
    std::string constraint_value;
    std::string constraint_format;
    std::string file_name;
    std::size_t line;
    std::string function;
  };

  const namespacet &ns;
  bool is_java;
  std::vector<waypointt> waypoints;

  bool is_program_location(const source_locationt &) const;

  void add_function_return(
    const source_locationt &call_location,
    const exprt &value,
    const irep_idt &function_id);
  void add_target(
    const goto_trace_stept &step,
    const std::vector<const goto_trace_stept *> &call_stack,
    const source_locationt &last_program_location);
};

#endif // CPROVER_GOTO_PROGRAMS_YAML_WITNESS_H
