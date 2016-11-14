#ifndef FUNCTION_PARAMETER_BUILDERT_H
#define FUNCTION_PARAMETER_BUILDERT_H

#include <util/irep.h>
#include <util/expr.h>

typedef std::pair<irep_idt, exprt> input_entryt;

class function_parameter_buildert
{
public:
  function_parameter_buildert(const input_entryt &entry,
                              class expr2cleanct &e2c);

  std::string get_parameter_decleration() const;
  std::string get_parameter_variable_name() const;

private:
  std::string variable_name;
  std::string variable_decleration_line;

};

#endif // FUNCTION_PARAMETER_BUILDERT_H
