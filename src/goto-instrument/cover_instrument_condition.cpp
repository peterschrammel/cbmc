/*******************************************************************\

Module: Coverage Instrumentation

Author: Daniel Kroening

\*******************************************************************/

/// \file
/// Coverage Instrumentation for Conditions

#include "cover_instrument_condition.h"

#include "cover_util.h"

void cover_instrument_condition(
  const namespacet &ns,
  goto_programt &goto_program,
  goto_programt::targett &i_it)
{
  const irep_idt coverage_criterion="location";
  const irep_idt property_class="coverage";

  if(i_it->is_assert())
    i_it->make_skip();

  // Conditions are all atomic predicates in the programs.
  if(!i_it->source_location.is_built_in())
  {
    const std::set<exprt> conditions=collect_conditions(i_it);

    const source_locationt source_location=i_it->source_location;

    for(const auto &c : conditions)
    {
      const std::string c_string=from_expr(ns, "", c);

      const std::string comment_t="condition `"+c_string+"' true";
      const irep_idt function=i_it->function;
      goto_program.insert_before_swap(i_it);
      i_it->make_assertion(c);
      i_it->source_location=source_location;
      i_it->source_location.set_comment(comment_t);
      i_it->source_location.set(ID_coverage_criterion, coverage_criterion);
      i_it->source_location.set_property_class(property_class);
      i_it->source_location.set_function(function);
      i_it->function=function;

      const std::string comment_f="condition `"+c_string+"' false";
      goto_program.insert_before_swap(i_it);
      i_it->make_assertion(not_exprt(c));
      i_it->source_location=source_location;
      i_it->source_location.set_comment(comment_f);
      i_it->source_location.set(ID_coverage_criterion, coverage_criterion);
      i_it->source_location.set_property_class(property_class);
      i_it->source_location.set_function(function);
      i_it->function=function;
    }

    for(std::size_t i=0; i<conditions.size()*2; i++)
      i_it++;
  }
}
