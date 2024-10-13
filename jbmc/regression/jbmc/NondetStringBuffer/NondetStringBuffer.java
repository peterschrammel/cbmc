import org.cprover.CProver;

class NondetStringBuffer
{
  static void main()
  {
    StringBuffer x = CProver.nondetWithNull(new StringBuffer());
    assert x == null || x instanceof StringBuffer;
  }
}
