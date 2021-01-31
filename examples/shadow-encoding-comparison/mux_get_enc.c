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

  unsigned temp = x->memory;
  unsigned value = temp & 0xefffffff;
  _Bool shadow = temp >> 31;
  
  assert(x);
}
