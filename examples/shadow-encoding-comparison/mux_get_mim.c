struct data
{
  unsigned memory;
};

void main()
{
  struct data *x;
  _Bool *y;
  for(int i=0; i<3; ++i)
  {
    if(nondet())
    {
      x = malloc(sizeof(struct data));
      y = malloc(sizeof(_Bool));
    }
  }

  unsigned value = x->memory;
  _Bool shadow = *y;
  
  assert(x);
}
