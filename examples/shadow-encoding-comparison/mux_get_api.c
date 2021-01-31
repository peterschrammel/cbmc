struct data
{
  unsigned memory;
};

void main()
{
  __CPROVER_field_decl_global("field", (_Bool)0);
  struct data *x;
  for(int i=0; i<3; ++i)
  {
    if(nondet())
    {
      x = malloc(sizeof(struct data));
    }
  }

  unsigned value = x->memory;
  _Bool shadow = __CPROVER_get_field(x, "field");
  
  assert(x);
}
