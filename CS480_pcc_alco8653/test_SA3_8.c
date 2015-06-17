/* error */
int gValue=foo(1,1);


int foo(a, b)
int a, b;
{
	return a+b;
}

main()
{

	int i=1, k,a;

	foo(1); /* mismatched params*/

	++i; /* bad expression */
	2++; /* bad lvalue*/
	i++ ++; /* bad expression / bad lvalue */
	input(); /* missing expression */

	for(int i=0;i<10;i++) /* bad expression */
	{
		i++;
		output(i);
	}


	if ( 1 /* missing right paren */
			{
				;
			}

	for(); /* missing ;, bad expression */


	else
	{
			a++;
	}



}

return
