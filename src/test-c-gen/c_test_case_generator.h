/*******************************************************************

 Module: C Test Case Generator

 Author: Thomas Kiley, thomas@diffblue.com

\*******************************************************************/

#ifndef C_TEST_CASE_GENERATOR_H_
#define C_TEST_CASE_GENERATOR_H_

#include <util/message.h>

class c_test_case_generatort : public messaget
{
public:
  c_test_case_generatort(message_handlert & _message_handler):
    messaget(_message_handler)
{
}
  void generate_tests();

};

#endif
