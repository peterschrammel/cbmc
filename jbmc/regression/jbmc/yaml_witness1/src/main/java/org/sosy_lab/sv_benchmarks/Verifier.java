package org.sosy_lab.sv_benchmarks;

import org.cprover.CProver;

public final class Verifier {
  public static int nondetInt() {
    return CProver.nondetInt();
  }

  public static long nondetLong() {
    return CProver.nondetLong();
  }
}
