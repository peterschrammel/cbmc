#include <assert.h>
#include <string.h>

int a[10];

int main(){
  __CPROVER_field_decl_local("dr_write", (_Bool)0);
  __CPROVER_field_decl_global("dr_write", (_Bool)0);

  assert(a[9] == 0);
  assert(__CPROVER_get_field(&(a[9]), "dr_write") == 0);

  int b[10];
  b[9] = 1;
  assert(__CPROVER_get_field(&(b[9]), "dr_write") == 0);
  __CPROVER_set_field(&(b[9]), "dr_write", 1);
  assert(__CPROVER_get_field(&(b[9]), "dr_write") == 1);

  memcpy(b, a, 10 * sizeof(int));
  assert(b[9] == 0);
  assert(__CPROVER_get_field(&(a[9]), "dr_write") == 0);
  assert(__CPROVER_get_field(&(b[9]), "dr_write") == 1);
}
