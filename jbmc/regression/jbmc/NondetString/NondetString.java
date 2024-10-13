import org.cprover.CProver;

class NondetString
{
  static void main()
  {
    String x = CProver.nondetWithNull(new String());
    assert x == null || x instanceof String;
  }
}
