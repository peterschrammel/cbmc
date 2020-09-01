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

  *x = (struct data){.memory=42u, .shadow=1};
  
  assert(x);
}
