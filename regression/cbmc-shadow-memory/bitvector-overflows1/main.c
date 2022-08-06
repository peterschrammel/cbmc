#include <assert.h>

#define has_overflow(x, T) (((T)(x)) != (x))

int main()
{
#if 1
  __CPROVER_field_decl_local("abstracted", (_Bool)0);
  long long a, b, c;
  __CPROVER_assume((-4 <= a && a <= 4));
  __CPROVER_assume((-4 <= b && b <= 4));
  long long tmp = a + b;
  c = (signed __CPROVER_bitvector[4])tmp;
  __CPROVER_set_field(&c, "abstracted",
                      __CPROVER_get_field(&a, "abstracted") ||
                      __CPROVER_get_field(&b, "abstracted") ||
                      has_overflow(tmp, signed __CPROVER_bitvector[4]));
  assert((-8 <= c && c <= 7) || __CPROVER_get_field(&c, "abstracted"));
#else
  long long a, b, c;
  __CPROVER_assume((-4 <= a && a <= 4));
  __CPROVER_assume((-4 <= b && b <= 4));
  long long c = a + b;
  assert((-8 <= c && c <= 7) || (c == 8));
#endif

  return 0;
}
