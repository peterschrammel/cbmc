struct data
{
  unsigned memory;
};

void main()
{
  __CPROVER_field_decl_global("field", (_Bool)0);
  struct data *x;
  for(int i=0; i<300; ++i)
  {
    if(nondet())
    {
      x = malloc(sizeof(struct data));
    }
  }

  x->memory = 42u;
  __CPROVER_set_field(x, "field", 1);
  
  assert(x);
}
