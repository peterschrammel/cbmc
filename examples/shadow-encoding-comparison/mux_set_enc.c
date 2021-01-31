struct data
{
  unsigned memory;
};

void main()
{
  struct data *x;
  for(int i=0; i<300; ++i)
  {
    if(nondet())
    {
      x = malloc(sizeof(struct data));
    }
  }

  unsigned value = 42u;
  _Bool shadow = 1;
  x->memory = (value & 0xefffffff) | (((unsigned)shadow) << 31);
  
  assert(x);
}
