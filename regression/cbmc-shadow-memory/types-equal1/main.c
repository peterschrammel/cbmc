#include <assert.h>

struct STRUCTNAME
{
  int x1;
  int B1[3];
};

int main()
{
  {
    unsigned char x;
    assert(__CPROVER_types_equal(x, (unsigned char)0));
    assert(!__CPROVER_types_equal(x, (signed char)0));
  }
  {
    short x;
    assert(__CPROVER_types_equal(x, (short)0));
    assert(!__CPROVER_types_equal(x, (int)0));
  }
  {
    int x;
    assert(__CPROVER_types_equal(x, (int)0));
    assert(!__CPROVER_types_equal(x, (char)0));
  }
  {
    long x;
    assert(__CPROVER_types_equal(x, (long)0));
    assert(!__CPROVER_types_equal(x, (char)0));
  }
  {
    int *x;
    assert(__CPROVER_types_equal(x, (int *)0));
    assert(!__CPROVER_types_equal(x, (int)0));
  }
  {
    _Bool x;
    assert(__CPROVER_types_equal(x, (_Bool)0));
    assert(!__CPROVER_types_equal(x, (int)0));
  }
  {
    char *x;
    assert(__CPROVER_types_equal(x, (char *)0));
    assert(!__CPROVER_types_equal(x, (char)0));
  }
  {
    char x[5];
    char y[5];
    assert(__CPROVER_types_equal(x, y));
    assert(!__CPROVER_types_equal(x, (char *)0));
  }
  {
    struct STRUCTNAME x;
    struct STRUCTNAME y;
    assert(__CPROVER_types_equal(x, y));
    assert(!__CPROVER_types_equal(x, (int)0));
  }
  {
    struct STRUCTNAME *x;
    struct STRUCTNAME *y;
    assert(__CPROVER_types_equal(x, y));
    assert(!__CPROVER_types_equal(x, (char *)0));
  }

  return 0;
}
