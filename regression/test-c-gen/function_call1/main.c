int bar()
{
  return 3;
}

int fun()
{
  int a = bar();
  if(a > 0)
  {
    a++;
  }
  else
  {
    a--;
  }

  return a;
}
