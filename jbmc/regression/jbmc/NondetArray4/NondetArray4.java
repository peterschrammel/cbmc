import org.cprover.CProver;

class NondetArray4
{
  void main()
  {
    int a = 1;
    int b = 2;
    int c = 3;

    Integer[] ints = CProver.nondetWithoutNull(new Integer[0]);
    assert ints != null;
  }
}
