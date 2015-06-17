/*****************************************************************************
 File name:     rts.c
 Author:				Brianna Alcoran
 Date:          2/20/15
 Class:         CS480
 Assignment:    Compiler Project written in C
 Purpose:       Runtime Stack for the Compiler (implementation)
 Modifications:
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "../include/rts.h"

/*******************************************************************************
 Function:		rtsCreate

 Description:	creates a runtime stack of size MAX_STACK

 Parameters:	*hsRTS	- handle to the runtime stack

 Returned:		error	- an RTS_ERROR
*******************************************************************************/
RTS_ERROR rtsCreate (RuntimeStackPtr *hsRTS)
{
	RTS_ERROR error = RTS_NO_ERROR;

	(*hsRTS) = (RuntimeStackPtr) malloc (sizeof(RuntimeStack));

	if (NULL == (*hsRTS))
	{
		error = ERROR_NO_RTS_CREATE;
	}
	else
	{
		int unavail = -1;
		memset ((*hsRTS)->Stack, unavail, sizeof((*hsRTS)->Stack));
		(*hsRTS)->availablePos = 1;
	}

	return error;
}

/*******************************************************************************
 Function:		rtsDispose

 Description:	dispose of a runtime stack

 Parameters:	*hsRTS	- handle to the runtime stack

 Returned:		error - an RTS_ERROR
*******************************************************************************/
RTS_ERROR rtsDispose (RuntimeStackPtr *hsRTS)
{
	RTS_ERROR error = RTS_NO_ERROR;
	if (NULL == *hsRTS)
	{
		error = RTS_ERROR_NULL_PTR;
	}
	else
	{
		free (*hsRTS);
		*hsRTS = NULL;
	}

	return error;
}


/*******************************************************************************
 Function:		rtsPrintHeader

 Description:	prints the header for the runtime stack print

 Parameters:	none

 Returned:		none
*******************************************************************************/
void rtsPrintHeader ()
{
	printf("\n");
	printf ("%26s", "RUNTIME STACK\n\n");
	printf ("%25s", "ADDRESS     VALUE\n");
	printf ("%25s", "-------     -----\n");
}

/*******************************************************************************
 Function:		rtsPrint

 Description:	prints the runtime stack from 0 to next available position

 Parameters:	psRTS

 Returned:		none
*******************************************************************************/
void rtsPrint (RuntimeStackPtr psRTS)
{
	int i;
	int pos = psRTS->availablePos;

	rtsPrintHeader();
	if (pos == MAX_STACK)
	{
		pos = MAX_STACK-1;
	}

	if (NULL != psRTS)
	{
		for (i = 0; i <= pos; ++i)
		{
			printf("%14d %9d\n", i, psRTS->Stack[i]);
		}
	}
}

/*******************************************************************************
 Function:		rtsDisplay

 Description:	displays contents of the runtime stack

 Parameters:	psRTS

 Returned:		none
*******************************************************************************/
void rtsDisplay (RuntimeStackPtr psRTS)
{
	printf ("%d \n", psRTS->availablePos);

	int i;
	for (i = 0; i < psRTS->availablePos; ++i)
	{
		if (psRTS->Stack[i] == -1)
		{
			printf ("%d \n", 0);
		}
		else
		{
			printf ("%d \n", psRTS->Stack[i]);
		}
	}
}

/*******************************************************************************
 Function:		rtsDisplayToFile

 Description:	displays contents of the runtime stack

 Parameters:	psRTS

 Returned:		none
*******************************************************************************/
void rtsDisplayToFile (RuntimeStackPtr psRTS, FILE * theFile)
{
	fprintf (theFile, "%d \n", psRTS->availablePos);

	int i;
	for (i = 0; i < psRTS->availablePos; ++i)
	{
		if (psRTS->Stack[i] == -1)
		{
			fprintf (theFile, "%d \n", 0);
		}
		else
		{
			fprintf (theFile, "%d \n", psRTS->Stack[i]);
		}
	}
}



/*******************************************************************************
 Function:		rtsAdd

 Description:	adds an int to the runtime stack

 Parameters:	psRTS	- pointer to the runtime stack
							newConstant	-  the constant to be added onto the stack

 Returned:		error - an RTS error
*******************************************************************************/
RTS_ERROR rtsAdd (RuntimeStackPtr psRTS, int newConstant)
{
	RTS_ERROR error = RTS_NO_ERROR;

	if (NULL == psRTS)
	{
		error = RTS_ERROR_NULL_PTR;
	}
	else if (psRTS->availablePos >= MAX_STACK)
	{
		error = RTS_ERROR_FULL;
	}
	else
	{
			psRTS->Stack[psRTS->availablePos] = newConstant;
			++(psRTS->availablePos);
	}

	return error;
}

/*******************************************************************************
 Function:		rtsGetAvailablePos

 Description:	returns the next available position on the runtime stack

 Parameters:	psRTS	-	pointer to the runtime stack

 Returned:		pos	-	the next available position on the runtime stack
*******************************************************************************/
int rtsGetAvailablePos (RuntimeStackPtr psRTS)
{
	int pos = -1;

	if (NULL != psRTS)
	{
		if (psRTS->availablePos <= MAX_STACK)
		{
			pos = psRTS->availablePos;
		}
	}

	return pos;
}
