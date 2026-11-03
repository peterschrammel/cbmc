// Calling an abstract method that declares `throws` from another method used
// to make --validate-goto-model fail (in the default lazy-methods mode) with a
// symbol type inconsistency: converting the method more than once added its
// `throws` list to the symbol's type again, so the call target already in a
// GOTO program no longer matched the symbol table.
class MyException extends Exception
{
}

abstract class Base
{
  abstract void f(int b) throws MyException;

  void g(int n) throws MyException
  {
    for(int i = 0; i < n; i++)
      f(i);
  }
}

class Derived extends Base
{
  void f(int b)
  {
  }
}

public class abstract_method_throws
{
  public static void main(String[] args) throws Exception
  {
    new Derived().g(2);
    assert true;
  }
}
