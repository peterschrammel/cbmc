public class Uncaught {
  public static int foo(int[] args) {
    assert args.length > 0;
    return args[0];
  }
}
