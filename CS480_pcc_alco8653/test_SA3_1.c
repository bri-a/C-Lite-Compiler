/* if */


main ()
{

	if (0) /* constant failure */
	{
		output (0, 0, 0, 0, 0, 0, 0, 1);
	}

	if (1)  /* constant success */
	{
		output (1, 1, 1, 1, 1, 1, 1, 1);
	}

	if (1 > 2) /* constant failure with comparison */
	{
		output (0, 0, 0, 0, 0, 0, 0, 2);
	}
	else
	{
		output (1, 1, 1, 1, 1, 1, 1, 2);
	}

}

