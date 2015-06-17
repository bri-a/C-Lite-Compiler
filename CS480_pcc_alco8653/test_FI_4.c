
/*
 * POINTERS
 */

int gValue=9;
int *gPtr = &gValue;
int k=7;

int foo(ptrToValue, realValue)
int *ptrToValue;
int realValue;
{

	output(*ptrToValue);

	output(*ptrToValue == realValue);
	if (*ptrToValue == realValue )
		return 1;
	return 0;
}


int takePointer(ptr)
int *ptr;
{
	*ptr = k++;
	return *ptr;
}


int swap(left, right)
int *left, *right;
{
	int tmp = *left;
	*left = *right;
	*right = tmp;

	output(*right == *left);

	if (*right == *left)
		return 1;
	return 0;
}

int noswap(left, right)
int left, right;
{
	int tmp = left;
	left = right;
	right = tmp;

	output(right == left);
	if (right == left)
		return 1;
	return 0;
}

int z = takePointer(gPtr);

int bar (x)
int x;
{
  int t[3]={3,2,1};
  int *ptrs[3];

	t[2] = x;
  ptrs[0] = &(t[0]);
  ptrs[1] = &t[1];
  ptrs[2] = &t[2];

  t[0] = 1024;

  output (*ptrs[0], *ptrs[1], *ptrs[2]);

 return 0;
}


int givePtrToPtr(ptrToPtr)
int *ptrToPtr[];
{

	output(0,0,0,0,0,0,0);
	output(*(ptrToPtr[0]));
	*(ptrToPtr[0]) = 1;
	output(*(ptrToPtr[0]));
	output(0,0,0,0,0,0,*ptrToPtr[0]);
	return *ptrToPtr[0];
}

int givePtr(ptrToPtr)
int *ptrToPtr;
{

	output(*ptrToPtr);
	*ptrToPtr = 1;
	output(*ptrToPtr);
	return *ptrToPtr;
}



main()
{
	int *myPtr = gPtr;
	int localInt = 19;
	int *ptrToPtr[10] = { gPtr, &localInt};

	output(z);					/* 7 */

	output(gValue,*gPtr);		/* 7 7 */
	output(takePointer(gPtr));	/* 8 */ 
	output(gValue, *gPtr);		/* 8 8 */

	output(*myPtr, *gPtr);		/* 8 8 */

	myPtr = &localInt;
	output(*myPtr, *gPtr);		/* 19 8 */

	myPtr = gPtr;
	output(*myPtr, *gPtr, localInt); /* 8 8 19 */

	output(foo(myPtr, localInt)); /* 8																	 0
									 0 */


	output(swap(myPtr, &localInt)); /*0
	 	 	 	 	 	 	 	 	  0*/

	output(noswap(*myPtr, localInt)); /*0
	 	 	 	 	 	 	 	 	   0*/

	output(bar(9));  /* 1024	2	9
					    0*/

	output(givePtr(myPtr)); /* 19
	 	 	 	 	 	 	 	1
	 	 	 	 	 	 	 	1*/

	output(givePtrToPtr(ptrToPtr));
/*	output:      0,     0,     0,     0,     0,     0,     0
	output:      1
	output:      1
	output:      0,     0,     0,     0,     0,     0,     1
	output:      1
*/
}
