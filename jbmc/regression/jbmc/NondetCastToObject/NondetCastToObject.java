import org.cprover.CProver;

class NondetCastToObject
{
  void main()
  {
    Object o = CProver.nondetWithNull(new Object());
    CProver.assume(o != null);
    assert o != null;
  }
}
