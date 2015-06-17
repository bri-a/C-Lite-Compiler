/* prime sieve */

/* adapted from http://www.dreamincode.net/code/snippet3315.htm */
main ()
{
	int i=-1;
	/*create prime list*/
	int prime[1000] = {2,2,2};
	int c1, c2, c3;

	for(i=-1; i<1 || i>999;1)
		input(i);

	/*fill list with 0 - prime*/
	for (c1 = 2; c1 <= i; c1++)
	{
		prime[c1] = 0;
	}

	/*set 0 and 1 as not prime*/
	prime[0] = 1;
	prime[1] = 1;

	/*find primes then eliminate their multiples (0 = prime, 1 = composite)*/
	/* we don't have a sqrt function!*/
	for (c2 = 2; c2 <= i/2 + 1; c2++)
	{
		if (prime[c2] == 0)
		{
			c1 = c2;
			for (c3 = 2* c1 ; c3 <= i + 1; c3 = c3 + c1)
			{
				prime[c3] = 1;
			}
		}
	}

	/*print primes*/
	for (c1 = 0; c1 < i + 1; c1++)
	{
		if (prime[c1] == 0)
			output(c1);
	}
}
