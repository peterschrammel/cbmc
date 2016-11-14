#ifndef FUNCTION_RETURN_BUILDERT_H
#define FUNCTION_RETURN_BUILDERT_H

#include <test-c-gen/input_entry.h>

class function_return_buildert
{
public:
  function_return_buildert(const inputst &all_inputs,
                           const irep_idt &function_id,
                           class expr2cleanct &e2c_converter);

  bool get_function_has_return() const;
  std::string get_return_decleration() const;
  std::string get_return_variable_name() const;
  std::vector<std::string> get_assertion_lines() const;

private:
  void set_return_variable_name(const irep_idt &function_id);

  class expr2cleanct &e2c;
  bool has_return;
  input_entryt return_entry;
  std::string return_var_name;
};

#endif // FUNCTION_RETURN_BUILDERT_H
