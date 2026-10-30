void __assert_fail(const char *, const char *, unsigned int, const char *);
void reach_error()
{
  __assert_fail("0", "main.c", 4, "reach_error");
}
unsigned char __VERIFIER_nondet_uchar(void);
int __VERIFIER_nondet_int(void);

int main()
{
  unsigned char n = __VERIFIER_nondet_uchar();
  int a = __VERIFIER_nondet_int(), b = __VERIFIER_nondet_int();
  if(n == 42 && a == 1 && b == 2)
    reach_error();
  return 0;
}
