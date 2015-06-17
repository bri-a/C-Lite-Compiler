/*****************************************************************************
 File name:     st_driver.c
 Date:          2/1/15
 Class:         CS480
 Assignment:    Compiler Project written in C
 Purpose:       Driver to test the symbol table for the compiler
 Hours:					20.
								Most difficult/time consuming tasks: (1) sorting alphabetically
								(2) fixing valgrind errors with conditional jumps
 Modifications: 2/13/15 -
 	 	 	 	 	 	 	 	Removed the bad division hash function. Modified the
 	 	 	 	 	 	 	 	handleError() method
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "../include/pcc13defines.h"
#include "../include/st.h"
#include "../include/er.h"

#define SYMBOLTABLE_SIZE 1109
#define MAX_STRING 100

char *ReservedWords[] = {
		"main",
		"int",
		"if",
		"else",
		"return",
		"for",
		"input",
		"output" };

#define NUM_RSRVD_WORDS 8

void readFile (FILE *, SymbolTablePtr , ErrorHandlerPtr , int *);
void handleError (ST_ERROR, ErrorHandlerPtr, int, char *);

/******************************************************************************
 Function:    	main

 Description:		used for testing SymbolTable and ErrorHandler

 Parameters:  	argc - number of command line arguments
 	 	 	 	 	 	 	  argv - pointers to the command line arguments

 Returned:    	EXIT_SUCCESS
 *****************************************************************************/
int main (int argc, char **argv)
{
	SymbolTable *psST = NULL;
	FILE *pFile;
	char *pszFileName;
	int lineNum = -1;

	/* initialize the error handler */
	ErrorHandler *psER = NULL;
	erCreate(&psER);

	if (2 != argc)
	{
		printf("USAGE: ./bin/st_driver testFile");
		return EXIT_FAILURE;
	}
	else
	{
		pszFileName = argv[1];
		pFile = fopen (pszFileName, "r");

		if (NULL == pFile)
		{
			erAddError (psER, lineNum, CANNOT_OPEN_FILE, pszFileName);
			erPrintErrors (psER);
			erClear (psER);
			erDispose (&psER);
			return EXIT_FAILURE;
		}
		lineNum = 1;
	}

	/* initialize symbol table */
	if (ST_NO_ERROR != stCreate(&psST, SYMBOLTABLE_SIZE,
			&userHashFunctionDJB2, &userCompare, &userCompareStData))
	{
		return EXIT_FAILURE;
	}

	stAddRsrvdWords(psST,ReservedWords, NUM_RSRVD_WORDS); /* Add reserved words */

	readFile (pFile, psST, psER, &lineNum); /* Loop through file */

	fclose (pFile);
	stDispose(&psST);
	erDispose(&psER);
	return 0;
}

/*******************************************************************************
 Function:    	readFile

 Description:		reads the file (assumed opened correctly), where each line is in
 	 	 	 	 	 	 	 	the form "commandChar [optionalString]"), executes
 	 	 	 	 	 	 	 	the necessary commands to manipulate the Symbol Table and handle
 	 	 	 	 	 	 	 	errors that may be raised

 Parameters:  	pFile		- pointer to the file that will be read
								psST		- pointer to the Symbol Table
								psER		- pointer to the Error Handler

 Returned:    	TRUE success 	FALSE failure of any kind
 ******************************************************************************/
void readFile (FILE *pFile, SymbolTablePtr psST, ErrorHandlerPtr psER,
							 int *lineNum)
{
	char commandCode, wordData[MAX_STRING];
	ST_DATATYPE * sData;
	ST_ERROR error;

	fscanf (pFile, "%c", &commandCode);

	while (!feof(pFile))
	{
		switch (commandCode)
		{
			case 'A':
				fscanf (pFile, "%s", wordData);
				handleError(stAddName(psST, wordData), psER, *lineNum, wordData);
				++(*lineNum);
				break;
			case 'L':
				stIncrementLevel(psST);
				++(*lineNum);
				break;
			case 'D':
				stDeleteCurrentLevel(psST);
				stDecrementLevel (psST);
				++(*lineNum);
				break;
			case 'S':
				fscanf (pFile, "%s", wordData);
				error = stSearch(psST, wordData, &sData);
				if (ST_NO_ERROR == error)
				{
					stPrintHeader();
					stPrintEntry (psST, *sData);
					printf("\n");
				}
				else
				{
					handleError(error, psER, *lineNum, wordData);
				}
				++(*lineNum);
				break;
			case 'P':
				stPrintSorted(psST);
				++(*lineNum);
				break;
		}
		fscanf (pFile, "%c", &commandCode);
	}
}

/*******************************************************************************
 Function:    	handleError

 Description:		handles the error returned from the Symbol Table

 Parameters:  	error		- the Symbol Table error
 	 	 	 	 	 	 	 	psER		- pointer to the Error Handler
 	 	 	 	 	 	 	 	lineNum	- the line number in the file where we got an error
 	 	 	 	 	 	 	 	pExtra	- extra information about the error

 Returned:    	none
 ******************************************************************************/
void handleError (ST_ERROR error, ErrorHandlerPtr psER, int lineNum,
									char *pszExtra)
{
	switch (error)
	{
		case ST_ERROR_NO_DATA:
			erAddError (psER, lineNum, UNDECLARED_IDENTIFIER, pszExtra);
			erPrintErrors (psER);
			erClear (psER);
			break;
		case ST_ERROR_DUPLICATE:
			erAddError (psER, lineNum, DUPLICATE_ID, pszExtra);
			erPrintErrors (psER);
			erClear (psER);
			break;
		case ST_ERROR_INVALID_ID:
			erAddError (psER, lineNum, INVALID_ID, pszExtra);
			erPrintErrors (psER);
			erClear (psER);
			break;
		default:
			/* do something */
			break;
	}
}
