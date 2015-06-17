/* advanced for */

int gValue = 9;

main()
{
	int i=0;
	int k, x, y, z;
	int *ptr;

	/* complicated increment */
	for( i = 0; i< 10 ; i=i+1)
	{
		output(i,1);
	}

	/* fake increment in for, increment in body */
	for( i = 0; i<5; 1)
	{
		output(i++,2);
	}

	/* marker to track output */
	output(0,0,0,0,0,0,0);

	i=5; /* make sure i is 5 */

	/* fake initialization and increment. */
	for(1;i>-2;1)
	{
		output(i--,3);
	}

	/* marker to track output */
	output(0,0,0,0,0,0,0);

	/* pointers */
	ptr= &i;
	for( *ptr = 0; *ptr<5; i++)
	{
		output(i, *ptr,4);
	}

	/* marker to track output */
	output(0,0,0,0,0,0,0);


	/* complicated condition in test multiple increment statements */
	k = 100;
	for( i = 0; i< 10 && k > 30 ; i++)
	{
		output(i, k,5);
		k = k -10;
	}

	/* marker to track output */
	output(0,0,0,0,0,0,0);


	/* nested */

	for (i=0;i< 10;i++)
	{
		for( k=0; k<=i; k++)
		{
			output(i,k,6);
		}
	}

	for (i=0;i< 10;i++)
	{
		for( k=0; k<=i; k++)
		{
			output(i,k,7);
			for( x = 0;x < k ; x++)
			{
				{
					output(x,8);
				}
				for( y=1;y<x;y++)
					output(y,9);
				for(z=2;z<x;z++)
					output(z,10);
			}
		}
	}

}
