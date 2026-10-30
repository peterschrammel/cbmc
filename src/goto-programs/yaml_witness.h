/*******************************************************************\

Module: Witnesses for Traces in the SV-COMP YAML Format

Author: Peter Schrammel

\*******************************************************************/

/// \file
/// Violation witnesses in the YAML format (version 2) of
/// https://gitlab.com/sosy-lab/benchmarking/sv-witnesses

#ifndef CPROVER_GOTO_PROGRAMS_YAML_WITNESS_H
#define CPROVER_GOTO_PROGRAMS_YAML_WITNESS_H

#include <util/source_location.h>

#include <iosfwd>
#include <map>
#include <optional>
#include <string>
#include <vector>

class cmdlinet;
class exprt;
class goto_tracet;
class goto_trace_stept;
class namespacet;
class optionst;

#define OPT_YAML_WITNESS                                                       \
  "(yaml-witness):"                                                            \
  "(yaml-witness-input-file):"                                                 \
  "(yaml-witness-specification):"

#define HELP_YAML_WITNESS                                                      \
  " {y--yaml-witness} {ufilename} \t write a violation witness in the "        \
  "SV-COMP YAML format to {ufilename} ({y-} for stdout)\n"                     \
  " {y--yaml-witness-input-file} {ufile} \t source file or directory that "    \
  "witness locations refer to; can be given multiple times (default: the "     \
  "source files given on the command line)\n"                                  \
  " {y--yaml-witness-specification} {uspec} \t specification to state in the " \
  "witness (default: derived from the violated property)\n"

void parse_yaml_witness_options(const cmdlinet &, optionst &);

/// Builds a violation witness from a counterexample trace.
/// The witness consists of a `function_return` waypoint for each
/// nondeterministic value returned by a `__VERIFIER_nondet_*` function (C) or
/// an `org.sosy_lab.sv_benchmarks.Verifier.nondet*` method (Java), followed by
/// a `target` waypoint for the violated property.
class yaml_violation_witnesst
{
public:
  yaml_violation_witnesst(const namespacet &, const optionst &);

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
    std::optional<std::size_t> column;
    std::string function;
  };

  const namespacet &ns;
  const optionst &options;
  bool is_java;
  std::vector<std::string> input_files;
  std::string specification;
  std::vector<waypointt> waypoints;
  std::map<std::size_t, std::vector<std::string>> input_file_lines;

  struct current_linet
  {
    source_locationt location;
    /// number of calls to each function on this line so far
    std::map<std::string, std::size_t> calls;
  };
  /// the line currently executed in each stack frame
  std::vector<current_linet> current_lines;

  /// input file index for each file name and working directory
  mutable std::map<std::pair<irep_idt, irep_idt>, std::optional<std::size_t>>
    input_file_of;

  std::optional<std::size_t>
  find_input_file(const source_locationt &source_location) const;
  std::optional<std::size_t>
  find_input_file_uncached(const source_locationt &source_location) const;
  const std::vector<std::string> &lines_of(std::size_t input_file);
  std::optional<std::size_t> call_column(
    std::size_t input_file,
    std::size_t line,
    const std::string &callee,
    std::size_t occurrence,
    bool at_call_start);

  void add_function_return(
    const source_locationt &call_location,
    const irep_idt &callee,
    const exprt &value,
    const irep_idt &function_id);
  void add_target(
    const goto_trace_stept &step,
    const std::vector<const goto_trace_stept *> &call_stack,
    const source_locationt &last_input_location);

  std::string default_specification(
    const goto_trace_stept &violation,
    const std::string &entry_function) const;
};

#endif // CPROVER_GOTO_PROGRAMS_YAML_WITNESS_H
