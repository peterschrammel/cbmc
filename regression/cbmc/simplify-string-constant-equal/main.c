#include <assert.h>

int main()
{
  assert("string" == "string");
  assert("string" == &"string"[0]);
  assert(&"string"[0] == "string");
  assert(&"string"[0] == &"string"[0]);
  assert(&"string"[2] == &"string"[2]);

  int i;
  __CPROVER_assume(0 <= i && i < 5);
  assert(&"string"[i] == &"string"[i]);
}
