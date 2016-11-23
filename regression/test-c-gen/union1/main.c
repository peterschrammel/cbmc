union u_type
{
  int i;
  char ch;
};

int fun(union u_type u)
{
	if(u.i > 0)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}