/*****************************************************************************
 File name:     bu_driver.c
 Author:				Brianna Alcoran
 Date:          3/7/15
 Class:         CS480
 Assignment:    Compiler Project written in C
 Purpose:       Driver to test the Top Down Parser
 Modifications:
 Hours:					18
******************************************************************************/

#include "../include/bu.h"
#include "../include/td.h"
#include <getopt.h>
#include <unistd.h>

#define LEX_DEBUG 1

#define SYMBOLTABLE_SIZE 1109
#define CONSTANTTABLE_SIZE 1109
#define NUM_RSRVD_WORDS 8
#define MAX_STRING 100

#define SUCCESS 1

char *ReservedWords[] = {
		"main",
		"int",
		"if",
		"else",
		"return",
		"for",
		"input",
		"output" };

bool initAll (SymbolTablePtr*, ErrorHandlerPtr*, ConstantTablePtr*,
							RuntimeStackPtr*, LexerPtr*, TopDownParserPtr*,
							BUParserPtr*,QuadFilePtr*, FILE*, char *);
void disposeAll (SymbolTablePtr*, ErrorHandlerPtr*, ConstantTablePtr*,
									RuntimeStackPtr*,LexerPtr*, TopDownParserPtr*,
									BUParserPtr*, QuadFilePtr*, FILE *);
bool getArgs (int argc, char **argv, bool *bLexDebug);
void printExpression (bool, FILE*);

/******************************************************************************
 Function:    	main

 Description:		used for testing top down parser

 Parameters:  	argc - number of command line arguments
 	 	 	 	 	 	 	  argv - pointers to the command line arguments

 Returned:    	EXIT_SUCCESS
 *****************************************************************************/
int main (int argc, char **argv)
{
	BU_ERROR bu_error = BU_NO_ERROR;
	SymbolTable 	* psST 	= NULL;
	ConstantTable * psCT 	= NULL;
	RuntimeStack 	* psRTS = NULL;
	ErrorHandler 	* psER 	= NULL;
	Lexer 				* psLex = NULL;
	TopDownParser * psTD 	= NULL;
	BUParser 			* psBU 	= NULL;
	QuadFile 			* psQD = NULL;

	FILE 	*pFile 			= NULL,
				*pFile2 		= NULL;
	char *pszFileName = NULL;
	char *pszQuadName = "quad.q";
	bool bBUDebug 	= true,
			 bLexDebug 	= false;


	if (!(SUCCESS == getArgs(argc, argv, &bLexDebug)))
	{
		return EXIT_FAILURE;
	}

	pszFileName = argv[optind++];
	pFile = fopen (pszFileName, "r");
	pFile2 = fopen (pszFileName, "r");

  erCreate(&psER);
  if (NULL == pFile) /* if file cannot be opened, exit the program  */
  {
		erAddError (psER, -1, CANNOT_OPEN_FILE, pszFileName);
		erPrintErrors (psER);
		erClear (psER);
		erDispose (&psER);
		return EXIT_FAILURE;
  }


  /* initializes everything  */
  if ( !(SUCCESS == initAll (&psST, &psER, &psCT, &psRTS,
  														&psLex, &psTD, &psBU, &psQD, pFile, pszQuadName)))
  {
  	return EXIT_FAILURE;
  }

	setDebug(psLex, bLexDebug);
	buSetDebug (psBU, bBUDebug);

	/* parse the file fo expressions!! */

	LEX_DATATYPE sData, sEndingToken;
	LEX_ERROR lex_error = LEX_NO_ERROR;
	int localSize = 0;
	BUExprStackElement sTest;

	while (BU_ERROR_NO_MORE_EXPRESSIONS != bu_error && LEX_NO_ERROR == lex_error)
	{
		printExpression(bBUDebug, pFile2);
		lex_error = lexGetToken (psLex, &sData);
		if (LEX_NO_ERROR == lex_error)
		{
			bu_error = buParse (psBU, &sData, &sEndingToken, &localSize, &sTest);
		}
	}


	ctPrint (psCT);
	rtsPrint (psRTS);
	erPrintErrorPerLine (psER);

	disposeAll (&psST, &psER, &psCT, &psRTS, &psLex, &psTD, &psBU, &psQD, pFile);
	fclose(pFile2);

	return EXIT_SUCCESS;
}

/******************************************************************************
 Function:    	initAll

 Description:		initializes symbol table, error handler, constant table,
								runtime stack, lexer, and the top down parser

 Parameters:  	psST	- pointer to the symbol table
								psER	- pointer to the error handler
								psCT	-	pointer to the constant table
								psRTS	- pointer to the runtime stack
								psLex	-	pointer to the lexer
								psTD	-	pointer to the top down parser
								pFile	-	pointer to the file

 Returned:    	bSuccess - true if initialized successfully; otherwise, false
 *****************************************************************************/
bool initAll (SymbolTablePtr* psST, ErrorHandlerPtr *psER, ConstantTablePtr *psCT,
							RuntimeStackPtr *psRTS, LexerPtr *psLex, TopDownParserPtr *psTD,
							BUParserPtr* psBU, QuadFilePtr *psQD, FILE *pFile, char *pQuad)
{
	bool bSuccess = true;

	if (ST_NO_ERROR != stCreate(psST, SYMBOLTABLE_SIZE,
			&userHashFunctionDJB2, &userCompare, &userCompareStData)
			|| CT_NO_ERROR != ctCreate (psCT, CONSTANTTABLE_SIZE,
					&userHashFunctionDJB2, &userCompareCtData)
			|| RTS_NO_ERROR != rtsCreate (psRTS))
	{
		bSuccess = false;
	}

	stAddRsrvdWords(*psST, ReservedWords, NUM_RSRVD_WORDS); /* Add reserved words */

	if (LEX_NO_ERROR != lexCreate (psLex, *psST, *psER, *psCT, *psRTS, pFile))
	{
		stDispose(psST);
		erDispose(psER);
		ctDispose(psCT);
		rtsDispose(psRTS);

		bSuccess = false;
	}

	if (QD_NO_ERROR != quadCreate(psQD, *psER, *psRTS, pQuad))
	{
		bSuccess = false;
	}

	if (BU_NO_ERROR != buCreate (psBU, *psLex, *psER, *psQD, *psST, *psRTS))
	{
		bSuccess = false;
	}

	if (TD_NO_ERROR != tdCreate(psTD, *psLex, *psER, *psST, *psRTS, *psQD, *psBU))
	{
		bSuccess = false;
	}

	return bSuccess;
}

/******************************************************************************
 Function:    	disposeAll

 Description:		disposes symbol table, error handler, constant table,
								runtime stack, lexer, and the top down parser

 Parameters:  	psST	- pointer to the symbol table
								psER	- pointer to the error handler
								psCT	-	pointer to the constant table
								psRTS	- pointer to the runtime stack
								psLex	-	pointer to the lexer
								psTD	-	pointer to the top down parser
								pFile	-	pointer to the file

 Returned:    	none
 *****************************************************************************/
void disposeAll (SymbolTablePtr* psST, ErrorHandlerPtr* psER,
									ConstantTablePtr* psCT, RuntimeStackPtr *psRTS,
									LexerPtr *psLex, TopDownParserPtr *psTD,
									BUParserPtr*psBU, QuadFilePtr*psQD, FILE *pFile)
{
	stDispose (psST);
	erDispose (psER);
	ctDispose (psCT);
	rtsDispose (psRTS);
	lexDispose (psLex);
	tdDispose (psTD);
	buDispose (psBU);
	quadDispose (psQD);

	fclose (pFile);
}

/******************************************************************************
 Function:    	getArgs

 Description:		get command line options and set debugging on or off

 Parameters:  	argc - number of command line arguments
 	 	 	 	 	 	 	  argv - pointers to the command line arguments
 	 	 	 	 	 	 	  bLexDebug	- bool to turn on the lex debug

 Returned:    	bSuccess - true if completed successfully; otherwise, false
 *****************************************************************************/
bool getArgs (int argc, char** argv, bool *bLexDebug)
{
	bool bSuccess = true;
	int c;

	if (2 > argc || 3 < argc) //if less than 2 arguments.. and more than 4
	{
		printf("USAGE: ./bin/bu_driver testFile [--lexdebug] \n");
		return bSuccess = false;
	}

  while (1)
	{
		static struct option long_options[] =
			{
				/* These options set a flag. */
				{"lexdebug", no_argument,  0, LEX_DEBUG},
				{0, 0, 0, 0}
			};
		int option_index = 0;

		c = getopt_long (argc, argv, "", long_options, &option_index);

		if (c == -1)
			break;

		switch (c)
			{
			case LEX_DEBUG:
				*bLexDebug = true;
				break;
			default:
				abort ();
			}
	}

	return bSuccess;
}

/******************************************************************************
 Function:    	printExpression

 Description:		prints an expression from the file if the boolean passed in is
								true

 Parameters:  	bBUDebug - bottom up debug
								inputFile - file to read expressions from

 Returned:    	none
 *****************************************************************************/
void printExpression (bool bBUDebug, FILE* inputFile)
{
	char * stringBuff = NULL;
	size_t len;

	if (bBUDebug && !feof(inputFile))
	{
		getdelim(&stringBuff, &len, ';', inputFile );
		if (!feof(inputFile))
		{
			printf("%s\n", stringBuff);
		}
	}
	free(stringBuff);
}
