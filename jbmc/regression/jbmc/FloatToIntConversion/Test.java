public class Test
{
  public static void floatToInt(int c, float a, float b)
  {
    switch(c)
    {
    case 0: 
      if (a == 3.0f && b == 2.0f) {
        int result = (int)(a / b);
        assert result == 1;
      }
      break;
    case 1: 
      if (a == 3.0f && b == -2.0f) {
        int result = (int)(a / b);
        assert result == -1;
      }
      break;
    case 2: 
      if (a == 1.0f) {
        // use constant here, -0.0f reverses cases 2 and 3.
        int result = (int)(a / 0.0f);
        // a/b = +inf
        assert result == 2147483647;
      }
      break;
    case 3: 
      if (a == -1.0f) {
        int result = (int)(a / 0.0f);
        // a/b = -inf
        assert result == -2147483648;
      }
      break;
    case 4: 
      if (a == 2147483646.0f && b == 1.0f) {
        int result = (int)(a / b);
        // a/b = 2147483648.0f
        assert result == 2147483647;
      }
      break;
    case 5: 
      if (a == -2147483647.0f && b == 1.0f) {
        int result = (int)(a / b);
        // a/b = -2147483648.0f
        assert result == -2147483648;
      }
      break;
    default: 
      if (a == 0.0f && b == 0.0f) {
        int result = (int)(a / b);
        // a/b = nan
        assert result == 0; 
      }
      break;
    }
  }

  public static void floatToLong(int c, float a, float b)
  {
    switch(c)
    {
    case 0: 
      if (a == 3.0f && b == 2.0f) {
        long result = (long)(a / b);
        assert result == 1L;
      }
      break;
    case 1: 
      if (a == 3.0f && b == -2.0f) {
        long result = (long)(a / b);
        assert result == -1L;
      }
      break;
    case 2: 
      if (a == 1.0f) {
        long result = (long)(a / 0.0f);
        // a/b = +inf
        assert result == 9223372036854775807L;
      }
      break;
    case 3: 
      if (a == -1.0f) {
        long result = (long)(a / 0.0f);
        // a/b = -inf
        assert result == -9223372036854775808L;
      }
      break;
    case 4: 
      if (a == 2147483649.0f && b == 1.0f) {
        long result = (long)(a / b);
        // a/b = 2147483648.0f
        assert result == 2147483648L;
      }
      break;
    case 5: 
      if (a == -2147483649.0f && b == 1.0f) {
        long result = (long)(a / b);
        // a/b = -2147483648.0f
        assert result == -2147483648L;
      }
      break;
    default: 
      if (a == 0.0f && b == 0.0f) {
        long result = (long)(a / b);
        // a/b = nan
        assert result == 0L;
      }
      break;
    }
  }

  public static void doubleToInt(int c, double a, double b)
  {
    switch(c)
    {
    case 0: 
      if (a == 3.0 && b == 2.0) {
        int result = (int)(a / b);
        assert result == 1;
      }
      break;
    case 1: 
      if (a == 3.0 && b == -2.0) {
        int result = (int)(a / b);
        assert result == -1;
      }
      break;
    case 2: 
      if (a == 1.0) {
        int result = (int)(a / 0.0);
        // a/b = +inf
        assert result == 2147483647;
      }
      break;
    case 3: 
      if (a == -1.0) {
        int result = (int)(a / 0.0);
        // a/b = -inf
        assert result == -2147483648;
      }
      break;
    case 4: 
      if (a == 4294967296.0 && b == 1.0) {
        int result = (int)(a / b);
        // a/b = not representable in int
        assert result == 2147483647;
      }
      break;
    case 5: 
      if (a == -4294967296.0 && b == 1.0) {
        int result = (int)(a / b);
        // a/b = not representable in int
        assert result == -2147483648;
      }
      break;
    default: 
      if (a == 0.0 && b == 0.0) {
        int result = (int)(a / b);
        // a/b = nan
        assert result == 0;
      }
      break;
    }
  }
  
  public static void doubleToLong(int c, double a, double b)
  {
    switch(c)
    {
    case 0: 
      if (a == 3.0 && b == 2.0) {
        long result = (long)(a / b);
        assert result == 1L;
      }
      break;
    case 1: 
      if (a == 3.0 && b == -2.0) {
        long result = (long)(a / b);
        assert result == -1L;
      }
      break;
    case 2: 
      if (a == 1.0) {
        long result = (long)(a / 0.0);
        // a/b = +inf
        assert result == 9223372036854775807L;
      }
      break;
    case 3: 
      if (a == -1.0) {
        long result = (long)(a / 0.0);
        // a/b = -inf
        assert result == -9223372036854775808L;
      }
      break;
    case 4: 
      if (a == 18446744073709551616.0 && b == 1.0) {
        long result = (long)(a / b);
        // a/b = not representable in long
        assert result == 9223372036854775807L;
      }
      break;
    case 5: 
      if (a == -18446744073709551616.0 && b == 1.0) {
        long result = (long)(a / b);
        // a/b = not representable in long
        assert result == -9223372036854775808L;
      }
      break;
    default: 
      if (a == 0.0 && b == 0.0) {
        long result = (long)(a / b);
        // a/b = nan
        assert result == 0L;
      }
      break;
    }
  }

}
