#ifndef EXPR2CLEANC_H_
#define EXPR2CLEANC_H_

#include <ansi-c/expr2c_class.h>

class expr2cleanct : public expr2ct
{
public:
  expr2cleanct(const namespacet &ns)
    : expr2ct(ns)
  {}

protected:
  virtual std::string convert_struct(const exprt &src, unsigned &precedence) override;
};

#endif // EXPR2CLEANC_H_
