/* basic recursion - greatest common divisor */

int gcd(x,y)
int x,y;
{
	if ( y == 0)
	{
		return x;
	}
	return gcd(y, x % y );
}

main()
{
	int x=0, y=0;

	input(x, y);

	for(1; x != -1 && y != -1;1 )
	{
		output(x,y, gcd(x, y));

		if ( gcd(x,y) == gcd(y,x) )
		{
			output(1,1,1,1,1,1,1,1,1);
		}
		else
		{
			output(0,0,0,0,0,0,0,0,0);
		}
		input(x, y);
	}
}
