void *malloc(unsigned long int);
int nondet_int();

void *__cs_safe_malloc(unsigned int __cs_size)
{
  void *__cs_ptr = (malloc(__cs_size));
  return __cs_ptr;
}

void __cs_init_scalar(void *__cs_var, unsigned int __cs_size)
{
  if (__cs_size == (sizeof(int)))
    *((int *) __cs_var) = nondet_int();
  else
    __cs_var = malloc(__cs_size);
}

void main()
{
  __CPROVER_field_decl_global("dr_write", (_Bool) 0);

  static char *local_Lkeys_thread_ver;
  local_Lkeys_thread_ver = (char *) __cs_safe_malloc((sizeof(char)) * 5l);

  static unsigned long int local_main_Lkeys;
  __cs_init_scalar(&local_main_Lkeys, sizeof(unsigned long int));

  assert(__CPROVER_get_field(local_Lkeys_thread_ver, "dr_write") == 0);
  assert(__CPROVER_get_field(local_Lkeys_thread_ver + 1, "dr_write") == 0);
  assert(__CPROVER_get_field(&local_main_Lkeys, "dr_write") == 0);

  __CPROVER_set_field(local_Lkeys_thread_ver + 1, "dr_write", 1);
  __CPROVER_set_field(&local_main_Lkeys, "dr_write", 2);

  assert(__CPROVER_get_field(local_Lkeys_thread_ver, "dr_write") == 0);
  assert(__CPROVER_get_field(local_Lkeys_thread_ver + 1, "dr_write") == 1);
  assert(__CPROVER_get_field(&local_main_Lkeys, "dr_write") == 2);
}
