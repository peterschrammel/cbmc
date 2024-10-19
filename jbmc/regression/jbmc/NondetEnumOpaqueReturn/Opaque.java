import org.cprover.CProver;

public class Opaque {

  Color c = CProver.nondetWithNull(Color.RED);

  public Color getC() {
    return c;
  }

}
