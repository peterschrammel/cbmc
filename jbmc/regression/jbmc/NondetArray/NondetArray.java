import org.cprover.CProver;

class NondetArray
{
  void main()
  {
    Object[] obj = CProver.nondetWithoutNull(new Object[0]);
    assert obj != null;
  }
}
