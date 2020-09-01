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

  x->memory = 42u;
  x->shadow = 1;
  
  assert(x);
}
