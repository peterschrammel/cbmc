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

  struct data temp = *x;
  unsigned value = temp.memory;
  _Bool shadow = temp.shadow;
  
  assert(x);
}
