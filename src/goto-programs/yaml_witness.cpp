/*******************************************************************\

Module: Violation Witnesses in Witness Format 2.x

Author: Peter Schrammel

\*******************************************************************/

/// \file
/// Violation witnesses in the YAML format (version 2) of
/// https://gitlab.com/sosy-lab/benchmarking/sv-witnesses

#include "yaml_witness.h"

#include <util/cmdline.h>
#include <util/config.h>
#include <util/ieee_float.h>
#include <util/namespace.h>
#include <util/options.h>
#include <util/prefix.h>
#include <util/std_expr.h>
#include <util/symbol.h>
#include <util/version.h>

#include <ansi-c/expr2c.h>
#include <langapi/language_util.h>
#include <langapi/mode.h>

#include "goto_functions.h"
#include "goto_trace.h"

#include <ctime>
#include <iomanip>
#include <ostream>
#include <random>
#include <sstream>

void parse_yaml_witness_options(const cmdlinet &cmdline, optionst &options)
{
  if(!cmdline.isset("witness"))
    return;

  options.set_option("witness", cmdline.get_value("witness"));
  options.set_option("stop-on-fail", true);
  options.set_option("trace", true);
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

/// \return whether \p function is one of the methods of
///   `org.sosy_lab.sv_benchmarks.Verifier` that return nondeterministic values
static bool is_java_nondet_function(const irep_idt &function)
{
  return has_prefix(
    id2string(function), "java::org.sosy_lab.sv_benchmarks.Verifier.nondet");
}

yaml_violation_witnesst::yaml_violation_witnesst(const namespacet &ns)
  : ns(ns), is_java(false)
{
  const symbolt *entry_point;
  if(!ns.lookup(goto_functionst::entry_point(), entry_point))
    is_java = entry_point->mode == ID_java;
}

/// \return whether \p source_location is in the program under verification
///   rather than in a library, such as CBMC's C library or JBMC's models of
///   the Java library
bool yaml_violation_witnesst::is_program_location(
  const source_locationt &source_location) const
{
  const std::string &file = id2string(source_location.get_file());
  if(
    file.empty() || source_location.get_line().empty() ||
    source_locationt::is_built_in(file))
  {
    return false;
  }

  if(is_java)
  {
    for(const char *library_package :
        {"java/", "javax/", "jdk/", "sun/", "org/cprover/"})
    {
      if(has_prefix(file, library_package))
        return false;
    }
  }

  return true;
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
  const exprt &value,
  const irep_idt &function_id)
{
  if(!is_program_location(call_location))
    return;

  const auto value_string = value_to_string(ns, function_id, value);
  if(!value_string.has_value())
    return;

  waypointt waypoint;
  waypoint.type = "function_return";
  waypoint.constraint_value = "\\result == " + *value_string;
  waypoint.constraint_format =
    is_java ? "ext_java_expression" : "acsl_expression";
  waypoint.file_name = id2string(call_location.get_file());
  waypoint.line = std::stoul(id2string(call_location.get_line()));
  if(!is_java)
    waypoint.function = id2string(call_location.get_function());
  waypoints.push_back(waypoint);
}

void yaml_violation_witnesst::add_target(
  const goto_trace_stept &step,
  const std::vector<const goto_trace_stept *> &call_stack,
  const source_locationt &last_program_location)
{
  source_locationt source_location = step.pc->source_location();
  auto caller = call_stack.rbegin();

  // An assertion located at the declaration of its function, as in generated
  // function bodies, is violated by calling that function.
  const symbolt *function;
  if(
    caller != call_stack.rend() && !ns.lookup(step.function_id, function) &&
    function->location.get_file() == source_location.get_file() &&
    function->location.get_line() == source_location.get_line())
  {
    source_location = (*caller)->pc->source_location();
    ++caller;
  }

  // If the violation occurs in a library function, we point to the call of
  // that function instead.
  while(!is_program_location(source_location) && caller != call_stack.rend())
  {
    source_location = (*caller)->pc->source_location();
    ++caller;
  }

  // Memory leaks and uncaught exceptions are detected in the entry point
  // after the entry function has returned.
  if(
    (!is_program_location(source_location) ||
     step.function_id == goto_functionst::entry_point()) &&
    last_program_location.is_not_nil())
  {
    source_location = last_program_location;
  }

  if(source_location.get_line().empty())
    return;

  waypointt waypoint;
  waypoint.type = "target";
  waypoint.file_name = id2string(source_location.get_file());
  waypoint.line = std::stoul(id2string(source_location.get_line()));
  if(!is_java)
    waypoint.function = id2string(source_location.get_function());
  waypoints.push_back(waypoint);
}

void yaml_violation_witnesst::operator()(const goto_tracet &goto_trace)
{
  std::vector<const goto_trace_stept *> call_stack;
  // a returned call to a nondet method whose result is yet to be assigned
  const goto_trace_stept *returned_nondet_call = nullptr;
  // number of nested calls to nondet methods (Java only)
  std::size_t nondet_depth = 0;
  // the most recent location in the program under verification
  source_locationt last_program_location = source_locationt::nil();

  for(const auto &step : goto_trace.steps)
  {
    const source_locationt &step_location = step.pc->source_location();
    if(
      !step.hidden && !step.pc->is_end_function() && !step.pc->is_dead() &&
      !step.is_function_return() &&
      step.function_id != goto_functionst::entry_point() &&
      is_program_location(step_location))
    {
      last_program_location = step_location;
    }

    switch(step.type)
    {
    case goto_trace_stept::typet::FUNCTION_CALL:
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
            step.pc->source_location(), step.full_lhs_value, step.function_id);
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
        if(
          identifier.find("return_value___VERIFIER_nondet_") ==
          std::string::npos)
          break;
        add_function_return(
          step.pc->source_location(), step.full_lhs_value, step.function_id);
      }
      break;

    case goto_trace_stept::typet::ASSERT:
      if(!step.cond_value)
      {
        add_target(step, call_stack, last_program_location);
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
  // The input files, their hashes and the specification are to be added by
  // the caller, who knows the verification task.
  out << "    task:\n";
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
    if(!waypoint.function.empty())
      out << "          function: " << yaml_string(waypoint.function) << '\n';
  }
}
