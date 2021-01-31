void main()
{
  unsigned x;
  __CPROVER_assume(0 <= x && x <= 7);
#ifdef ABSTRACTION
  x = x & 0xF;
#endif
  unsigned y;
  __CPROVER_assume(0 <= y && y <= 7);
#ifdef ABSTRACTION
  y = y & 0xF;
#endif
  unsigned z = x + y;
#ifdef ABSTRACTION
  z = z & 0xF;
#endif
  for(int i = 0; i < 1000; ++i)
  {
    z -= x + y;
#ifdef ABSTRACTION
    z = z & 0xF;
#endif
    z += x + y;
#ifdef ABSTRACTION
    z = z & 0xF;
#endif
  }
  assert(0 <= z && z <= 15);
}
