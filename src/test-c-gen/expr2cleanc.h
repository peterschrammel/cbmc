/*******************************************************************

 Module: C Test Case Generator

 Author: Thomas Kiley, thomas@diffblue.com

\*******************************************************************/

#ifndef EXPR2CLEANC_H
#define EXPR2CLEANC_H

#include <ansi-c/expr2c_class.h>

class expr2cleanct : public expr2ct
{
public:
  expr2cleanct(const namespacet &ns)
    : expr2ct(ns)
  {}

  std::string convert_with_identifier(const typet &src,
                                      const std::string &identifier);

protected:
  virtual std::string convert_struct(const exprt &src, unsigned &precedence) override;

  virtual std::string convert_struct_type(const typet &src,
                                          const std::string &qualifer_str,
                                          const std::string &declarator_str) override;

  virtual std::string convert_array_type(const typet &src,
                                         const c_qualifierst &qualifiers,
                                         const std::string &declarator_str) override;

  virtual std::string convert_constant_bool(bool boolean_value) override;
};

#endif // EXPR2CLEANC_H_
