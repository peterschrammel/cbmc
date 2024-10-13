import org.cprover.CProver;

class A
{
  int a;
}

class NondetDirectFromMethod
{
  A methodReturningA()
  {
    return CProver.nondetWithoutNull(new A());
  }

  void main()
  {
    assert methodReturningA() != null;
  }
}
