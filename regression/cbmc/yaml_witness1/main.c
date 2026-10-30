unsigned char __VERIFIER_nondet_uchar(void);
int __VERIFIER_nondet_int(void);

int main()
{
  unsigned char n = __VERIFIER_nondet_uchar();
  int a = __VERIFIER_nondet_int(), b = __VERIFIER_nondet_int();
  __CPROVER_assert(!(n == 42 && a == 1 && b == 2), "reachable");
  return 0;
}
