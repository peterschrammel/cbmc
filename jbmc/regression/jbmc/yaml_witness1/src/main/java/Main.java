import org.sosy_lab.sv_benchmarks.Verifier;

public class Main {
  static int add(int a, int b) {
    return a + b;
  }

  public static void main(String[] args) {
    long l = Verifier.nondetLong();
    int x = add(Verifier.nondetInt(), Verifier.nondetInt());
    if (l == -3L && x == 7) {
      assert false;
    }
  }
}
