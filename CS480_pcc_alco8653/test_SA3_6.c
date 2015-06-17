/* return */
int gValue;

int produceReturnValue(a)
int a;
{
	return 42;
}

int produceReturnValueTwo(a)
int a;
{
	return a;
}


int produceReturnValueThree(a, b)
int a;
int b;
{


	return a+b;
}

int produceReturnValueFour(a)
int a;
{
	int b = a + 1;

	return b;
}

int getReturnValue(a)
int a;
{
	return (a);
}

int simple()
{
	return 9;
}

int gRetValueInit = produceReturnValue(0); /* call function in initialization */

main()
{
	int retValue;
	int retValueInit = produceReturnValue(0); /* call function in initialization */

	output(retValueInit, gRetValueInit);		/* 42 42*/

	retValue = produceReturnValue(0);
	output(retValue);						/* 42 */

	retValue = produceReturnValueTwo(0);
	output(retValue);						/* 0 */

	retValue = produceReturnValueThree(1, 16);
	output(retValue);						/* 17 */

	gValue = produceReturnValueFour(9) + 9;
	output(gValue);							/* 19 */

	output ( simple ); /* 9 */


	output( produceReturnValue(0) + produceReturnValue(0)); /* 84 */

															/* 5 */
	output( produceReturnValueFour( produceReturnValueFour ( produceReturnValueFour (produceReturnValueFour(1)))));
															/* 90 */
	output( produceReturnValueThree(
						produceReturnValueFour(
									produceReturnValueFour (
												produceReturnValueFour (produceReturnValueFour(2)))),

						produceReturnValue(
									produceReturnValueThree(1,2))
						+ produceReturnValue(produceReturnValueThree(3,4))));

	output(getReturnValue(9)); 								/* 9 */

	output( produceReturnValueFour( produceReturnValueFour ( produceReturnValueFour
			(produceReturnValueFour(produceReturnValueFour( produceReturnValueFour (
					produceReturnValueFour (produceReturnValueFour(produceReturnValueFour(
							produceReturnValueFour ( produceReturnValueFour (produceReturnValueFour(
									produceReturnValueFour( produceReturnValueFour ( produceReturnValueFour
											(produceReturnValueFour(0))))))))))))))))); /* 16 */

	output(produceReturnValueThree(
						produceReturnValue(1)+produceReturnValue(1),1)); /* 85 */
	output(produceReturnValueThree(1,
						produceReturnValue(1) +produceReturnValue(1)) );/* 85 */
}
