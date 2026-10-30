/*******************************************************************\

Module: Witnesses for Traces in the SV-COMP YAML Format

Author: Peter Schrammel

\*******************************************************************/

/// \file
/// Violation witnesses in the YAML format (version 2) of
/// https://gitlab.com/sosy-lab/benchmarking/sv-witnesses

#include "yaml_witness.h"

#include <util/cmdline.h>
#include <util/config.h>
#include <util/cprover_prefix.h>
#include <util/ieee_float.h>
#include <util/namespace.h>
#include <util/options.h>
#include <util/prefix.h>
#include <util/sha256.h>
#include <util/std_expr.h>
#include <util/suffix.h>
#include <util/symbol.h>
#include <util/version.h>

#include <ansi-c/expr2c.h>
#include <langapi/language_util.h>
#include <langapi/mode.h>

#include "goto_functions.h"
#include "goto_trace.h"

#include <algorithm>
#include <cctype>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <ostream>
#include <random>
#include <sstream>

/// File extensions of source files that witness locations may refer to
static bool is_source_file(const std::filesystem::path &path)
{
  const std::string extension = path.extension().string();
  return extension == ".c" || extension == ".i" || extension == ".h" ||
         extension == ".java";
}

void parse_yaml_witness_options(const cmdlinet &cmdline, optionst &options)
{
  if(!cmdline.isset("yaml-witness"))
    return;

  options.set_option("yaml-witness", cmdline.get_value("yaml-witness"));
  options.set_option("stop-on-fail", true);
  options.set_option("trace", true);

  optionst::value_listt input_files;
  if(cmdline.isset("yaml-witness-input-file"))
  {
    const auto &values = cmdline.get_values("yaml-witness-input-file");
    input_files.assign(values.begin(), values.end());
  }
  else
  {
    // The positional arguments are the input files for CBMC; for JBMC they
    // are class names or jar files, which are not source files.
    for(const auto &arg : cmdline.args)
    {
      std::error_code error;
      if(std::filesystem::is_regular_file(arg, error) && is_source_file(arg))
        input_files.push_back(arg);
    }
  }
  options.set_option("yaml-witness-input-files", input_files);

  if(cmdline.isset("yaml-witness-specification"))
  {
    options.set_option(
      "yaml-witness-specification",
      cmdline.get_value("yaml-witness-specification"));
  }
}

/// \return \p s as a double-quoted YAML scalar
static std::string yaml_string(const std::string &s)
{
  std::ostringstream result;
  result << '"';
  for(const char c : s)
  {
    switch(c)
    {
    case '"':
      result << "\\\"";
      break;
    case '\\':
      result << "\\\\";
      break;
    case '\n':
      result << "\\n";
      break;
    case '\t':
      result << "\\t";
      break;
    default:
      if(static_cast<unsigned char>(c) < 0x20)
      {
        result << "\\x" << std::hex << std::setw(2) << std::setfill('0')
               << static_cast<unsigned>(static_cast<unsigned char>(c))
               << std::dec;
      }
      else
        result << c;
    }
  }
  result << '"';
  return result.str();
}

/// \return a random (version 4) UUID as defined in RFC 4122
static std::string random_uuid()
{
  std::random_device random_device;
  std::mt19937_64 generator(
    (uint64_t(random_device()) << 32) ^ uint64_t(random_device()));
  uint64_t high = generator();
  uint64_t low = generator();
  high = (high & ~uint64_t(0xf000)) | 0x4000;
  low = (low & ~(uint64_t(0x3) << 62)) | (uint64_t(0x2) << 62);

  std::ostringstream result;
  result << std::hex << std::setfill('0') << std::setw(8) << (high >> 32) << '-'
         << std::setw(4) << ((high >> 16) & 0xffff) << '-' << std::setw(4)
         << (high & 0xffff) << '-' << std::setw(4) << (low >> 48) << '-'
         << std::setw(12) << (low & uint64_t(0xffffffffffff));
  return result.str();
}

/// \return the current time in ISO 8601 format
static std::string creation_time()
{
  const std::time_t now = std::time(nullptr);
  char buffer[32];
  std::strftime(
    buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", std::gmtime(&now));
  return buffer;
}

static bool is_identifier_char(char c)
{
  return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '$';
}

/// \return whether \p function is one of the methods of
///   `org.sosy_lab.sv_benchmarks.Verifier` that return nondeterministic values
static bool is_java_nondet_function(const irep_idt &function)
{
  return has_prefix(
    id2string(function), "java::org.sosy_lab.sv_benchmarks.Verifier.nondet");
}

/// \return the method name of a Java function identifier
///   such as `java::org.sosy_lab.sv_benchmarks.Verifier.nondetInt:()I`
static std::string java_method_name(const irep_idt &function)
{
  std::string name = id2string(function);
  if(has_prefix(name, "java::"))
    name = name.substr(6);
  name = name.substr(0, name.find(':'));
  const auto dot = name.rfind('.');
  return dot == std::string::npos ? name : name.substr(dot + 1);
}

yaml_violation_witnesst::yaml_violation_witnesst(
  const namespacet &ns,
  const optionst &options)
  : ns(ns), options(options), is_java(false)
{
  const symbolt *entry_point;
  if(!ns.lookup(goto_functionst::entry_point(), entry_point))
    is_java = entry_point->mode == ID_java;

  // collect the input files, expanding directories
  for(const auto &input : options.get_list_option("yaml-witness-input-files"))
  {
    std::error_code error;
    if(std::filesystem::is_directory(input, error))
    {
      std::vector<std::string> files;
      for(const auto &entry :
          std::filesystem::recursive_directory_iterator(input, error))
      {
        if(entry.is_regular_file() && is_source_file(entry.path()))
          files.push_back(entry.path().string());
      }
      std::sort(files.begin(), files.end());
      input_files.insert(input_files.end(), files.begin(), files.end());
    }
    else
      input_files.push_back(input);
  }
}

/// \return the index of the input file that \p source_location refers to
std::optional<std::size_t> yaml_violation_witnesst::find_input_file(
  const source_locationt &source_location) const
{
  const irep_idt &file = source_location.get_file();
  if(file.empty() || source_location.is_built_in())
    return {};

  const auto key =
    std::make_pair(file, source_location.get_working_directory());
  const auto cached = input_file_of.find(key);
  if(cached != input_file_of.end())
    return cached->second;
  return input_file_of[key] = find_input_file_uncached(source_location);
}

std::optional<std::size_t> yaml_violation_witnesst::find_input_file_uncached(
  const source_locationt &source_location) const
{
  const irep_idt &file = source_location.get_file();
  std::filesystem::path path(id2string(file));
  if(path.is_relative() && !source_location.get_working_directory().empty())
    path = id2string(source_location.get_working_directory()) / path;

  for(std::size_t i = 0; i < input_files.size(); ++i)
  {
    std::error_code error;
    if(std::filesystem::equivalent(path, input_files[i], error))
      return i;
  }

  // Java source locations only provide the path relative to the class path,
  // e.g., org/sosy_lab/sv_benchmarks/Verifier.java, so we match the trailing
  // path components.
  const std::filesystem::path relative =
    std::filesystem::path(id2string(file)).lexically_normal();
  for(std::size_t i = 0; i < input_files.size(); ++i)
  {
    const std::filesystem::path input =
      std::filesystem::path(input_files[i]).lexically_normal();
    auto input_it = input.end();
    auto relative_it = relative.end();
    while(input_it != input.begin() && relative_it != relative.begin() &&
          *std::prev(input_it) == *std::prev(relative_it))
    {
      --input_it;
      --relative_it;
    }
    if(relative_it == relative.begin())
      return i;
  }

  return {};
}

const std::vector<std::string> &
yaml_violation_witnesst::lines_of(std::size_t input_file)
{
  auto entry = input_file_lines.emplace(input_file, std::vector<std::string>{});
  if(entry.second)
  {
    std::ifstream in(input_files[input_file], std::ios::binary);
    std::string line;
    while(std::getline(in, line))
    {
      if(!line.empty() && line.back() == '\r')
        line.pop_back();
      entry.first->second.push_back(line);
    }
  }
  return entry.first->second;
}

/// Determines the column of a call to \p callee on \p line from the source
/// text, as the front ends do not record columns.
/// For C, `function_return` waypoints point to the closing parenthesis of
/// the call; for Java, to the opening parenthesis.
/// \param input_file: index into input_files
/// \param line: line number (starting at 1)
/// \param callee: name of the called function
/// \param occurrence: which of the calls to \p callee on \p line (starting
///   at 0); if there is a single call only, that one is taken
/// \param at_call_start: return the column of the start of the callee name
///   instead of the parenthesis
/// \return 1-based column, or an empty optional if the call is not found
std::optional<std::size_t> yaml_violation_witnesst::call_column(
  std::size_t input_file,
  std::size_t line,
  const std::string &callee,
  std::size_t occurrence,
  bool at_call_start)
{
  const auto &lines = lines_of(input_file);
  if(line == 0 || line > lines.size() || callee.empty())
    return {};
  const std::string &text = lines[line - 1];

  std::vector<std::size_t> columns;
  for(std::size_t start = text.find(callee); start != std::string::npos;
      start = text.find(callee, start + 1))
  {
    if(start > 0 && is_identifier_char(text[start - 1]))
      continue;
    std::size_t pos = start + callee.size();
    while(pos < text.size() &&
          std::isspace(static_cast<unsigned char>(text[pos])))
    {
      ++pos;
    }
    if(pos >= text.size() || text[pos] != '(')
      continue;

    if(at_call_start)
      columns.push_back(start + 1);
    else if(is_java)
      columns.push_back(pos + 1);
    else
    {
      std::size_t depth = 0;
      for(; pos < text.size(); ++pos)
      {
        if(text[pos] == '(')
          ++depth;
        else if(text[pos] == ')' && --depth == 0)
          break;
      }
      if(pos >= text.size())
        return {};
      columns.push_back(pos + 1);
    }
  }

  if(columns.size() == 1)
    return columns.front();
  else if(occurrence < columns.size())
    return columns[occurrence];
  else
    return {};
}

/// \return \p value as an expression in the source language, if it is a
///   constant that can be stated in a witness
static std::optional<std::string> value_to_string(
  const namespacet &ns,
  const irep_idt &function_id,
  const exprt &value)
{
  if(!value.is_constant())
    return {};

  const irep_idt &type_id = value.type().id();
  if(type_id == ID_floatbv)
  {
    // print with enough digits to recover the exact value
    const ieee_float_valuet f(to_constant_expr(value));
    if(f.is_NaN() || f.is_infinity() || (!f.is_float() && !f.is_double()))
      return {};
    std::ostringstream result;
    if(f.is_float())
      result << std::setprecision(9) << f.to_float();
    else
      result << std::setprecision(17) << f.to_double();
    std::string number = result.str();
    if(number.find_first_of(".e") == std::string::npos)
      number += ".0";
    return f.is_float() ? number + "f" : number;
  }
  else if(
    type_id != ID_signedbv && type_id != ID_unsignedbv &&
    type_id != ID_c_bool && type_id != ID_bool)
  {
    return {};
  }

  if(get_mode_from_identifier(ns, function_id) == ID_C)
    return expr2c(value, ns, expr2c_configurationt::clean_configuration);
  else
    return from_expr(ns, function_id, value);
}

void yaml_violation_witnesst::add_function_return(
  const source_locationt &call_location,
  const irep_idt &callee,
  const exprt &value,
  const irep_idt &function_id)
{
  const auto input_file = find_input_file(call_location);
  if(!input_file.has_value() || call_location.get_line().empty())
    return;

  const auto value_string = value_to_string(ns, function_id, value);
  if(!value_string.has_value())
    return;

  waypointt waypoint;
  waypoint.type = "function_return";
  waypoint.constraint_value = "\\result == " + *value_string;
  waypoint.constraint_format =
    is_java ? "ext_java_expression" : "acsl_expression";
  waypoint.file_name = input_files[*input_file];
  waypoint.line = std::stoul(id2string(call_location.get_line()));
  waypoint.column = call_column(
    *input_file,
    waypoint.line,
    id2string(callee),
    current_lines.back().calls[id2string(callee)]++,
    false);
  if(!is_java)
    waypoint.function = id2string(call_location.get_function());
  waypoints.push_back(waypoint);
}

void yaml_violation_witnesst::add_target(
  const goto_trace_stept &step,
  const std::vector<const goto_trace_stept *> &call_stack,
  const source_locationt &last_input_location)
{
  source_locationt source_location = step.pc->source_location();
  std::string callee;
  auto caller = call_stack.rbegin();

  // For unreach-call the violation is the call to reach_error.
  if(
    !is_java && step.function_id == "reach_error" &&
    caller != call_stack.rend())
  {
    callee = id2string((*caller)->called_function);
    source_location = (*caller)->pc->source_location();
    ++caller;
  }

  // The target location must be in an input file: if the violation occurs
  // in a library function, we point to the call of that function instead.
  auto input_file = find_input_file(source_location);
  while(!input_file.has_value() && caller != call_stack.rend())
  {
    callee.clear();
    source_location = (*caller)->pc->source_location();
    input_file = find_input_file(source_location);
    ++caller;
  }

  // Memory leaks and uncaught exceptions are detected in the entry point
  // after the entry function has returned.
  if(
    (!input_file.has_value() ||
     step.function_id == goto_functionst::entry_point()) &&
    last_input_location.is_not_nil())
  {
    callee.clear();
    source_location = last_input_location;
    input_file = find_input_file(source_location);
  }

  if(source_location.get_line().empty())
    return;

  waypointt waypoint;
  waypoint.type = "target";
  waypoint.line = std::stoul(id2string(source_location.get_line()));
  if(input_file.has_value())
  {
    waypoint.file_name = input_files[*input_file];
    waypoint.column = call_column(*input_file, waypoint.line, callee, 0, true);
  }
  else
    waypoint.file_name = id2string(source_location.get_file());
  if(!is_java)
    waypoint.function = id2string(source_location.get_function());
  waypoints.push_back(waypoint);
}

std::string yaml_violation_witnesst::default_specification(
  const goto_trace_stept &violation,
  const std::string &entry_function) const
{
  const std::string init = "CHECK( init(" + entry_function + "()), LTL(";

  if(is_java)
    return init + "G assert) )";

  const irep_idt &property_class =
    violation.pc->source_location().get_property_class();
  if(property_class == "overflow")
    return init + "G ! overflow) )";
  else if(property_class == "memory-leak")
    return init + "G valid-memtrack) )";
  else if(
    property_class == "pointer dereference" || property_class == "array bounds")
  {
    return init + "G valid-deref) )";
  }
  else if(property_class == "precondition_instance")
  {
    // the checks of library functions such as free and memcpy
    if(violation.comment.find("free") != std::string::npos)
      return init + "G valid-free) )";
    else
      return init + "G valid-deref) )";
  }
  else
    return init + "G ! call(reach_error())) )";
}

void yaml_violation_witnesst::operator()(const goto_tracet &goto_trace)
{
  std::vector<const goto_trace_stept *> call_stack;
  // a returned call to a nondet method whose result is yet to be assigned
  const goto_trace_stept *returned_nondet_call = nullptr;
  std::string entry_function;
  // number of nested calls to nondet methods (Java only)
  std::size_t nondet_depth = 0;
  // the most recent location in an input file
  source_locationt last_input_location = source_locationt::nil();

  for(const auto &step : goto_trace.steps)
  {
    // Track the calls on the line currently executed in each stack frame,
    // to tell apart multiple calls to the same function on a line.
    const source_locationt &step_location = step.pc->source_location();
    current_lines.resize(call_stack.size() + 1);
    if(!step_location.get_line().empty())
    {
      auto &current_line = current_lines.back();
      if(
        step_location.get_line() != current_line.location.get_line() ||
        step_location.get_file() != current_line.location.get_file())
      {
        current_line.location = step_location;
        current_line.calls.clear();
      }

      if(
        !step.hidden && !step.pc->is_end_function() && !step.pc->is_dead() &&
        !step.is_function_return() &&
        step.function_id != goto_functionst::entry_point() &&
        find_input_file(step_location).has_value())
      {
        last_input_location = step_location;
      }
    }

    switch(step.type)
    {
    case goto_trace_stept::typet::FUNCTION_CALL:
      if(
        entry_function.empty() &&
        step.function_id == goto_functionst::entry_point() &&
        !has_prefix(id2string(step.called_function), CPROVER_PREFIX) &&
        !has_suffix(id2string(step.called_function), "::clinit_wrapper") &&
        id2string(step.called_function).find("<clinit>") == std::string::npos)
      {
        const symbolt *symbol;
        if(!ns.lookup(step.called_function, symbol))
        {
          entry_function = id2string(symbol->pretty_name);
          entry_function = entry_function.substr(0, entry_function.find('('));
        }
      }
      call_stack.push_back(&step);
      if(is_java && is_java_nondet_function(step.called_function))
        ++nondet_depth;
      break;

    case goto_trace_stept::typet::FUNCTION_RETURN:
      if(!call_stack.empty())
      {
        const goto_trace_stept *call = call_stack.back();
        call_stack.pop_back();
        if(is_java && is_java_nondet_function(call->called_function))
        {
          --nondet_depth;
          if(nondet_depth == 0)
            returned_nondet_call = call;
        }
      }
      break;

    case goto_trace_stept::typet::ASSIGNMENT:
      if(step.thread_nr != 0)
        break;

      if(is_java)
      {
        // The result of the call is assigned at the call site, either by the
        // call instruction or, if returns have been removed, by the next
        // instruction.
        const auto is_result_assignment = [&step](const goto_trace_stept &call)
        {
          if(step.pc == call.pc)
            return true;
          if(!step.pc->is_assign())
            return false;
          const exprt &rhs = step.pc->assign_rhs();
          return rhs.id() == ID_symbol &&
                 to_symbol_expr(rhs).get_identifier() ==
                   id2string(call.called_function) + "#return_value";
        };
        if(
          returned_nondet_call != nullptr &&
          is_result_assignment(*returned_nondet_call))
        {
          add_function_return(
            step.pc->source_location(),
            java_method_name(returned_nondet_call->called_function),
            step.full_lhs_value,
            step.function_id);
        }
        returned_nondet_call = nullptr;
      }
      else if(!step.hidden)
      {
        // Functions without body, such as __VERIFIER_nondet_int, are not
        // called on the trace: their return value is assigned nondet to a
        // temporary return_value___VERIFIER_nondet_int.
        const auto lhs_object = step.get_lhs_object();
        if(!lhs_object.has_value() || step.full_lhs != *lhs_object)
          break;
        const std::string &identifier = id2string(lhs_object->get_identifier());
        const std::string prefix = "return_value___VERIFIER_nondet_";
        const auto prefix_pos = identifier.find(prefix);
        if(prefix_pos == std::string::npos)
          break;
        std::string callee =
          identifier.substr(prefix_pos + std::string("return_value_").size());
        callee = callee.substr(0, callee.find_first_of("$!@#"));
        add_function_return(
          step.pc->source_location(),
          callee,
          step.full_lhs_value,
          step.function_id);
      }
      break;

    case goto_trace_stept::typet::ASSERT:
      if(!step.cond_value)
      {
        add_target(step, call_stack, last_input_location);
        if(options.is_set("yaml-witness-specification"))
          specification = options.get_option("yaml-witness-specification");
        else
        {
          specification = default_specification(
            step, entry_function.empty() ? "main" : entry_function);
        }
        return;
      }
      break;

    case goto_trace_stept::typet::NONE:
    case goto_trace_stept::typet::ASSUME:
    case goto_trace_stept::typet::GOTO:
    case goto_trace_stept::typet::LOCATION:
    case goto_trace_stept::typet::INPUT:
    case goto_trace_stept::typet::OUTPUT:
    case goto_trace_stept::typet::DECL:
    case goto_trace_stept::typet::DEAD:
    case goto_trace_stept::typet::CONSTRAINT:
    case goto_trace_stept::typet::SHARED_READ:
    case goto_trace_stept::typet::SHARED_WRITE:
    case goto_trace_stept::typet::SPAWN:
    case goto_trace_stept::typet::MEMORY_BARRIER:
    case goto_trace_stept::typet::ATOMIC_BEGIN:
    case goto_trace_stept::typet::ATOMIC_END:
      break;
    }
  }
}

void yaml_violation_witnesst::output(std::ostream &out) const
{
  out << "- entry_type: \"violation_sequence\"\n";
  out << "  metadata:\n";
  // Version 2.0 is the most widely supported one for C; Java requires 2.3+.
  out << "    format_version: " << yaml_string(is_java ? "2.4" : "2.0") << '\n';
  out << "    uuid: " << yaml_string(random_uuid()) << '\n';
  out << "    creation_time: " << yaml_string(creation_time()) << '\n';
  out << "    producer:\n";
  out << "      name: " << yaml_string(is_java ? "JBMC" : "CBMC") << '\n';
  out << "      version: " << yaml_string(CBMC_VERSION) << '\n';
  out << "    task:\n";
  out << "      input_files:";
  if(input_files.empty())
    out << " []";
  out << '\n';
  for(const auto &file : input_files)
    out << "      - " << yaml_string(file) << '\n';
  out << "      input_file_hashes:";
  if(input_files.empty())
    out << " {}";
  out << '\n';
  for(const auto &file : input_files)
  {
    std::ifstream in(file, std::ios::binary);
    out << "        " << yaml_string(file) << ": "
        << yaml_string(sha256_hex(in)) << '\n';
  }
  out << "      specification: " << yaml_string(specification) << '\n';
  if(!is_java)
  {
    out << "      data_model: "
        << yaml_string(config.ansi_c.pointer_width == 32 ? "ILP32" : "LP64")
        << '\n';
  }
  out << "      language: " << yaml_string(is_java ? "Java" : "C") << '\n';

  out << "  content:";
  if(waypoints.empty())
    out << " []";
  out << '\n';
  for(const auto &waypoint : waypoints)
  {
    out << "  - segment:\n";
    out << "    - waypoint:\n";
    out << "        type: " << yaml_string(waypoint.type) << '\n';
    out << "        action: \"follow\"\n";
    if(!waypoint.constraint_value.empty())
    {
      out << "        constraint:\n";
      out << "          value: " << yaml_string(waypoint.constraint_value)
          << '\n';
      if(!waypoint.constraint_format.empty())
      {
        out << "          format: " << yaml_string(waypoint.constraint_format)
            << '\n';
      }
    }
    out << "        location:\n";
    out << "          file_name: " << yaml_string(waypoint.file_name) << '\n';
    out << "          line: " << waypoint.line << '\n';
    if(waypoint.column.has_value())
      out << "          column: " << *waypoint.column << '\n';
    if(!waypoint.function.empty())
      out << "          function: " << yaml_string(waypoint.function) << '\n';
  }
}
