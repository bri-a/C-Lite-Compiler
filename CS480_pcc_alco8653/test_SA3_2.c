/* advanced if */


int gValue = 1;

main ()
{
	int value = 9;
	int secondValue = 7;

	if (1 > 2 || -1 < 9)   /* constant success with comparison and OR */
	{
		output (1, 1, 1, 1, 1, 1, 1, 1);
	}
	else
	{
		output (0, 0, 0, 0, 0, 0, 0, 1);
	}

	if( 1 < 2 ) /* nesting */
	{
		if( value > 1 )
		{
			output (1, 1, 1, 1, 1, 1, 1, 2);
		}
		else
		{
			output (0, 0, 0, 0, 0, 0, 0, 2);
		}
	}
	else
	{
		output (0, 0, 0, 0, 0, 0, 0, 3);
	}

	if ( value > secondValue ) /* compare two locals */
	{
		if( value > gValue ) /* compare local and global, also nested */
		{
			if ( gValue == value)
			{
				output (0, 0, 0, 0, 0, 0, 0, 4);
			}
			else
			{
				output (1, 1, 1, 1, 1, 1, 1, 4);
			}
		}
		else
		{
			output (0, 0, 0, 0, 0, 0, 0, 5);
		}
	}
	else
	{
		output (0, 0, 0, 0, 0, 0, 0, 6);
	}

	if( value * 2 > 9 ) /* test expression */
	{
		output (1, 1, 1, 1, 1, 1, 1, 7);
	}
	else
	{
		output (0, 0, 0, 0, 0, 0, 0, 7);
	}


	if( gValue + value == value + gValue ) /* test another expression using globals and locals */
	{
		output (1, 1, 1, 1, 1, 1, 1, 8);
	}
	else
	{
		output (0, 0, 0, 0, 0, 0, 0, 8);
	}

	if( ( (1 <= 2 ) ) ) /* parens */
	{
		output (1, 1, 1, 1, 1, 1, 1, 9);
	}
	else
	{
		output (0, 0, 0, 0, 0, 0, 0, 9);
	}

	if(  -value <= 2  ) /* negation */
	{
		output (1, 1, 1, 1, 1, 1, 1, 10);
	}
	else
	{
		output (0, 0, 0, 0, 0, 0, 0, 10);
	}

	/* long expression */
	if ( value > 1000 || value < -1000 || value + 1 == 0 || value * value == 6 || value != 4242)
	{
		output (1, 1, 1, 1, 1, 1, 1, 11);
	}
	else
	{
		output (0, 0, 0, 0, 0, 0, 0, 11);
	}

	value = 0;
	if( value ++ ) /* auto op */
	{
		output (0, 0, 0, 0, 0, 0, 0, 12);
	}
	else
	{
		output (1, 1, 1, 1, 1, 1, 1, 12);
	}

	value = 0;
	if( value = 1  ) /* assignment */
	{
		output (1, 1, 1, 1, 1, 1, 1, 13);
	}
	else
	{
		output (0, 0, 0, 0, 0, 0, 0, 13);
	}

  value = 0;
  if( 1 && value++ ) /* tricky auto op */
  {
  	output (0, 0, 0, 0, 0, 0, 0, 14);
  }
  else
  {
  	output (1, 1, 1, 1, 1, 1, 1, 14);
  }

	if( -1 + 2 - 3 * 4 / 6 % 7 ) /* many operators! */
	{
		output (1, 1, 1, 1, 1, 1, 1, 15);
	}
	else
	{
		output (0, 0, 0, 0, 0, 0, 0, 15);
	}

	value = 3;

	if(0 == value) /* else if chain */
	{
		output (0, 0, 0, 0, 0, 0, 0, 16);
	}
	else if( 1 == value )
	{
		output (0, 0, 0, 0, 0, 0, 1, 16);
	}
	else if( 2 == value )
	{
		output (0, 0, 0, 0, 0, 0, 2, 16);
	}
	else if( -0 == value )
	{
		output (0, 0, 0, 0, 0, 0, 3, 16);
	}
	else if( -1 == value )
	{
		output (0, 0, 0, 0, 0, 0, 4, 16);
	}
	else if( -2 == value )
	{
		output (0, 0, 0, 0, 0, 0, 5, 16);
	}
	else if( 3 == value)
	{
		output (1, 1, 1, 1, 1, 1, 1, 16);
	}
	else
	{
		output (0, 0, 0, 0, 0, 0, 6, 16);
	}

	/* OR */
	if(0 == value || 1 == value || 2 == value || -0 == value ||
			-1 == value || -2 == value || 3 == value)
	{
		output (1, 1, 1, 1, 1, 1, 1, 17);
	}
	else
	{
		output (0, 0, 0, 0, 0, 0, 0, 17);
	}

	/* precedence! */
	if(0 == value || 1 == value || 2 == value || -0 == value ||
			-1 == value || -2 == value && 3 == value)
	{
		output (0, 0, 0, 0, 0, 0, 0, 18);
	}
	else
	{
		output (1, 1, 1, 1, 1, 1, 1, 18);
	}
}

