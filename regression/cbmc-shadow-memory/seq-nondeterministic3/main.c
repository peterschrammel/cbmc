int x;

int foo()
{

  int y=10;

  __CPROVER_set_field( &x, "one", 1);    
  __CPROVER_set_field( &x, "two", 2);    
  __CPROVER_set_field( &x, "field1", 3);    
  __CPROVER_set_field( &x, "field2", 4);    
  __CPROVER_set_field( &x, "field3", 5);    
 assert(__CPROVER_get_field( &x, "field3") == 5); 
 assert(__CPROVER_get_field( &x, "field2") == 4); 
 assert(__CPROVER_get_field( &x, "field1") == 3); 
  assert(__CPROVER_get_field( &x, "one") == 1);
  assert(__CPROVER_get_field( &x, "two") == 2);

  __CPROVER_set_field( &y, "field1", y);    
  assert(__CPROVER_get_field( &y, "field1") == 10); 
  assert(__CPROVER_get_field( &y, "field2") == 0); 
  assert(__CPROVER_get_field( &y, "field3") == 0); 
  assert(__CPROVER_get_field( &y, "one") == 0);   
  assert(__CPROVER_get_field( &y, "one") == 10);  
  assert(__CPROVER_get_field( &y, "two") == 0);  


}


int main ()
{
    // field declarations
  __CPROVER_field_decl_local( "field1", (int)0 );
  __CPROVER_field_decl_local( "field2", (int)0 );
  __CPROVER_field_decl_local( "field3", (__CPROVER_bitvector[6])0 );
  __CPROVER_field_decl_global( "one", (int)0 );
  __CPROVER_field_decl_global( "two", (__CPROVER_bitvector[6])0 );
  foo();
}
