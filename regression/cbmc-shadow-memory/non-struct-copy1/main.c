union UNIONNAME
{
  int x1;
  struct {
    char y1;
    // char padding;
    short y2;
    short y3;
  } x2;
  char x3[3];
};

struct STRUCTNAME
{
  int x;
  long y;
  char z;
};

int main(){
    __CPROVER_field_decl_local("field1", (_Bool)0);
    struct STRUCTNAME s;
    int ax[2];
    long ay[2];
    char az[2];
    union UNIONNAME u;

    int x1;
    __CPROVER_set_field(&x1, "field1", (_Bool)1);
    assert(__CPROVER_get_field(&x1, "field1") == 1);
    int x2;
    assert(__CPROVER_get_field(&x2, "field1") == 0);
    x2 = x1;
    assert(__CPROVER_get_field(&x2, "field1") == 1);
    assert(__CPROVER_get_field(&s.x, "field1") == 0);
    s.x = x1;
    assert(__CPROVER_get_field(&s.x, "field1") == 1);
    assert(__CPROVER_get_field(&ax[0], "field1") == 0);
    ax[0] = x1;
    assert(__CPROVER_get_field(&ax[0], "field1") == 1);
    assert(__CPROVER_get_field(&u.x1, "field1") == 0);
//    u.x1 = x1;
//    assert(__CPROVER_get_field(&u.x1, "field1") == 1);

    // This is not supported
    // __CPROVER_set_field(&x1, "field1", (_Bool)0);
    // assert(__CPROVER_get_field(&x1, "field1") == 0);
    // x1 = az[0];
    // assert(__CPROVER_get_field(&x1, "field1") == 1);
    // This is not supported, but seems to work
    // __CPROVER_set_field(&x1, "field1", (_Bool)0);
    // assert(__CPROVER_get_field(&x1, "field1") == 0);
    // x1 = s.x;
    // assert(__CPROVER_get_field(&x1, "field1") == 1);
    // This is not supported
    // __CPROVER_set_field(&x1, "field1", (_Bool)0);
    // assert(__CPROVER_get_field(&x1, "field1") == 0);
    // x1 = u.x1;
    // assert(__CPROVER_get_field(&x1, "field1") == 1);

    long y1;
    __CPROVER_set_field(&y1, "field1", (_Bool)1);
    assert(__CPROVER_get_field(&y1, "field1") == 1);
    long y2;
    assert(__CPROVER_get_field(&y2, "field1") == 0);
    y2 = y1;
    assert(__CPROVER_get_field(&y2, "field1") == 1);
    assert(__CPROVER_get_field(&s.y, "field1") == 0);
    s.y = y1;
    assert(__CPROVER_get_field(&s.y, "field1") == 1);
    assert(__CPROVER_get_field(&ay[0], "field1") == 0);
    ay[0] = y1;
    assert(__CPROVER_get_field(&ay[0], "field1") == 1);

    char z1;
    __CPROVER_set_field(&z1, "field1", (_Bool)1);
    assert(__CPROVER_get_field(&z1, "field1") == 1);
    char z2;
    assert(__CPROVER_get_field(&z2, "field1") == 0);
    z2 = z1;
    assert(__CPROVER_get_field(&z2, "field1") == 1);
    assert(__CPROVER_get_field(&s.z, "field1") == 0);
    s.z = z1;
    assert(__CPROVER_get_field(&s.z, "field1") == 1);
    assert(__CPROVER_get_field(&az[0], "field1") == 0);
    az[0] = z1;
    assert(__CPROVER_get_field(&az[0], "field1") == 1);
    __CPROVER_set_field(&u.x3[2], "field1", (_Bool)0);
    assert(__CPROVER_get_field(&u.x3[2], "field1") == 0);
//    u.x3[2] = x1;
//    assert(__CPROVER_get_field(&u.x3[2], "field1") == 1);

/*
    union UNIONNAME u1;
    __CPROVER_set_field(&u1, "field1", (_Bool)1);
    assert(__CPROVER_get_field(&u1, "field1") == 1);
    union UNIONNAME u2;
    u2 = u1;
    assert(__CPROVER_get_field(&u2, "field1") == 1);
*/
    return 0;
}
