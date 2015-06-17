/*****************************************************************************
 File name:     er.c
 Author:				Brianna Alcoran
 Date:          2/1/15
 Class:         CS480
 Assignment:    Compiler Project written in C
 Purpose:       Error Handler for the Compiler (implementation)
 Modifications:
******************************************************************************/

#include "../include/er.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

char gszErrors[MAX_ERRORS][MAX_ERROR_LINE];

/*******************************************************************************
 Function:		erCreate

 Description:	create an Error Handler

 Parameters:	hsER	-	handle to the Error Handler

 Returned:		none
*******************************************************************************/
void erCreate(ErrorHandlerPtr *hsER)
{
	*hsER = (ErrorHandlerPtr) malloc (sizeof (ErrorHandler));

	lstCreate(&((*hsER)->sErrors));
	(*hsER)->numErrors = 0;
	(*hsER)->bPrintDragon = false;
	LOAD_ERRORS
}

/*******************************************************************************
 Function:		erDispose

 Description:	dispose of an Error Handler

 Parameters:	hsER	- handle to the Error Handler

 Returned:		none
*******************************************************************************/
void erDispose(ErrorHandlerPtr *hsER)
{
	lstDispose(&((*hsER)->sErrors));
	free(*hsER);
	*hsER = NULL;
}

/*******************************************************************************
 Function:		erAddError

 Description:	adds an error to the Error Handler

 Parameters:	psER			- pointer to the Error Handler
 	 	 	 	 	 	 	lineNum		- the line number where the error occurred
 	 	 	 	 	 	 	pError 		- the error message
 	 	 	 	 	 	 	pOptional	- additional information about the error

 Returned:
*******************************************************************************/
void erAddError (ErrorHandlerPtr psER, int lineNum, ErrorCode error, char *pOptional)
{
	DATATYPE sType;
	DATATYPE sNewData;


	if (0 == strlen (pOptional))
	{
		sprintf(sNewData.er_data.pszErrorMessage, "Line %d: %s \n", lineNum,
																															gszErrors[error]);
	}
	else
	{
		sprintf(sNewData.er_data.pszErrorMessage, "Line %d: %s %s.\n", lineNum,
				gszErrors[error], pOptional);
	}
	sNewData.er_data.errorID = error;
	sNewData.er_data.lineNum = lineNum;
	++(psER->numErrors);
	lstLast (&((psER)->sErrors), &sType);
	lstInsertAfter (&((psER)->sErrors), sNewData);
}

/*******************************************************************************
 Function:		erClear

 Description:	clears all errors stored in the Error Handler

 Parameters:	psER	- pointer to the Error Handler

 Returned:		none
*******************************************************************************/
void erClear(ErrorHandlerPtr psER)
{
	lstDispose (&(psER->sErrors));
	lstCreate (&(psER->sErrors));
	psER->numErrors = 0;
}

/*******************************************************************************
 Function:		erGetNumErrors

 Description:	returns the number of errors in the handler

 Parameters:	psER	- pointer to the Error Handler

 Returned:		int
*******************************************************************************/
int erGetNumErrors (ErrorHandlerPtr psER)
{
	return psER->numErrors;
}

/*******************************************************************************
 Function:		erSetPrintDragon

 Description:	sets the boolean to print the dragon with the errors or not

 Parameters:	psER	- pointer to the Error Handler
 	 	 	 	 	 	 	bPrint - boolean indicating to print dragon or not

 Returned:		none
*******************************************************************************/
void erSetPrintDragon (ErrorHandlerPtr psER, bool bPrint)
{
	if (NULL != psER)
	{
		psER->bPrintDragon = bPrint;
	}
}


/*******************************************************************************
 Function:		erPrintErrors

 Description:	prints all errors stored in the Error Handler

 Parameters:	psER	- pointer to the Error Handler

 Returned:		none
*******************************************************************************/
void erPrintErrors(ErrorHandlerPtr psER)
{
	int i;
	DATATYPE sType;

	lstFirst(&(psER->sErrors), &sType);

	printf("\n");

	if (psER->numErrors == 0)
	{
		printf ("0 Errors. \n\n\n");
	}
	else
	{
		for (i = 0; i < psER->numErrors; ++i)
		{
			lstNext(&(psER->sErrors), &sType);
			printf ("%s", sType.er_data.pszErrorMessage);
		}
	}
	if (psER->bPrintDragon)
	{
		erPrintDragon ();
	}
}

/*******************************************************************************
 Function:		erPrintDragon

 Description:	prints a dragon

 Parameters:	psER	- pointer to the Error Handler

 Returned:		none
*******************************************************************************/
void erPrintDragon()
{
	printf (ANSI_COLOR_GREEN"     ____ __" ANSI_COLOR_RESET "\n");
  printf (ANSI_COLOR_GREEN"    { --.\\  |          "\
  				ANSI_COLOR_RED".)%%%%%%)%%%%"			ANSI_COLOR_RESET "\n");
  printf (ANSI_COLOR_GREEN"     '-._\\\\ | (\\___   "\
  				ANSI_COLOR_RED"%%)%%%%(%%%%(%%%%%%"		ANSI_COLOR_RESET "\n");
  printf (ANSI_COLOR_GREEN"         `\\\\|{/ ^ _)"\
  				ANSI_COLOR_RED"-%%(%%%%%%%%)%%%%;%%%%%%"	ANSI_COLOR_RESET "\n");
  printf (ANSI_COLOR_GREEN"      .'^^^^^^^  /`    "\
  				ANSI_COLOR_RED"%%%%)%%%%%%%%)%%%%%%'"	ANSI_COLOR_RESET "\n");
  printf (ANSI_COLOR_GREEN"      //\\   ) ,  /       "\
  				ANSI_COLOR_RED"'%%%%%%%%(%%%%'"	ANSI_COLOR_RESET "\n");
  printf (ANSI_COLOR_GREEN",  _.'/  `\\<-- \\<"	ANSI_COLOR_RESET "\n");
  printf (ANSI_COLOR_GREEN" `^^^`     ^^   ^^);"ANSI_COLOR_RESET "\n");
}


/*******************************************************************************
 Function:		erPrintErrorPerLine

 Description:	prints 1st error on a line

 Parameters:	psER	- pointer to the Error Handler

 Returned:		none
*******************************************************************************/
void erPrintErrorPerLine (ErrorHandlerPtr psER)
{
	int i, currentLine;
	DATATYPE sType;

	lstFirst(&(psER->sErrors), &sType);
	currentLine = -1;
	printf("\n");

	if (psER->numErrors == 0)
	{
		printf("0 Errors. \n");
	}
	else
	{
		for (i = 0; i < psER->numErrors; ++i)
		{
			lstNext(&(psER->sErrors), &sType);
			if (currentLine != sType.er_data.lineNum)
			{
				currentLine = sType.er_data.lineNum;
				printf ("%s", sType.er_data.pszErrorMessage);
			}
		}
		if (psER->bPrintDragon)
		{
			erPrintDragon ();
		}
	}

}
