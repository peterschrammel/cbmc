struct data
{
  unsigned memory;
  _Bool shadow;
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

  unsigned value = x->memory;
  _Bool shadow = x->shadow;
  
  assert(x);
}
