struct data
{
  unsigned memory;
};

void main()
{
  struct data *x;
  _Bool *y;
  for(int i=0; i<300; ++i)
  {
    if(nondet())
    {
      x = malloc(sizeof(struct data));
      y = malloc(sizeof(_Bool));
    }
  }

  x->memory = 42u;
  *y = 1;
  
  assert(x);
}
