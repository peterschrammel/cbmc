int main(){
    __CPROVER_field_decl_global("abstr", (_Bool)0); 
    __CPROVER_field_decl_local("abstr", (_Bool)0);
    _Bool z = 1;
    int x = 0;
    __CPROVER_set_field(&x, "abstr", z);
    assert(__CPROVER_get_field(&x, "abstr"));
    return 0;
}
