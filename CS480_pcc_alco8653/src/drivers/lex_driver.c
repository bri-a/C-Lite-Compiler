/*****************************************************************************
 File name:     lex_driver.c
 Author:				Brianna Alcoran
 Date:          2/13/15
 Class:         CS480
 Assignment:    Compiler Project written in C
 Purpose:       Driver to test the lexical analyzer for the compiler
 Modifications:
 Hours:					30
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "../include/lex.h"

#define SYMBOLTABLE_SIZE 1109
#define CONSTANTTABLE_SIZE 1109
#define NUM_RSRVD_WORDS 8
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

/******************************************************************************
 Function:    	main

 Description:		used for testing LEXICAL ANALYZER

 Parameters:  	argc - number of command line arguments
 	 	 	 	 	 	 	  argv - pointers to the command line arguments

 Returned:    	EXIT_SUCCESS
 *****************************************************************************/
int main(int argc, char **argv)
{
	SymbolTable * psST = NULL;
	ConstantTable *psCT = NULL;
	RuntimeStack *psRTS = NULL;
	Lexer * psLex = NULL;
	LEX_DATATYPE sLexData;
	FILE *pFile;
	LEX_ERROR error;
	char *pszFileName;
	int lineNum = -1;

	ErrorHandler *psER = NULL;   /* initialize the error handler */
	erCreate(&psER);

	if (2 != argc)
	{
		printf("USAGE: ./bin/lex_driver testFile");
		erDispose (&psER);
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

	/* initialize symbol table, lexer, constant table, and runtime stack */
	if (ST_NO_ERROR != stCreate(&psST, SYMBOLTABLE_SIZE,
			&userHashFunctionDJB2, &userCompare, &userCompareStData)
			|| CT_NO_ERROR != ctCreate (&psCT, CONSTANTTABLE_SIZE,
					&userHashFunctionDJB2, &userCompareCtData)
			|| RTS_NO_ERROR != rtsCreate (&psRTS))
	{
		return EXIT_FAILURE;
	}

	stAddRsrvdWords(psST, ReservedWords, NUM_RSRVD_WORDS); /* Add reserved words */

	if (LEX_NO_ERROR != lexCreate (&psLex, psST, psER, psCT, psRTS, pFile))
	{
		stDispose(&psST);
		erDispose(&psER);
		ctDispose(&psCT);
		rtsDispose(&psRTS);

		return EXIT_FAILURE;
	}

	setDebug (psLex, true);
	error = lexGetToken (psLex,&sLexData);

	while (error == LEX_NO_ERROR) /* Get tokens until an error is returned */
	{
		error = lexGetToken (psLex, &sLexData);
	}

	/* Printing */
	ctPrint (psCT);
	rtsPrint (psRTS);
	erPrintErrorPerLine (psER);

	/* Dispose */
	erDispose (&psER);
	stDispose (&psST);
	ctDispose (&psCT);
	rtsDispose (&psRTS);
	lexDispose (&psLex);

	/* Close the file!!! */
	fclose (pFile);

	return EXIT_SUCCESS;
}
