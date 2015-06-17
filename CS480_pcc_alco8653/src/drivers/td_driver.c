/*****************************************************************************
 File name:     td_driver.c
 Author:				Brianna Alcoran
 Date:          2/24/15
 Class:         CS480
 Assignment:    Compiler Project written in C
 Purpose:       Driver to test the Top Down Parser
 Modifications:
 Hours:					20
******************************************************************************/

#include "../include/bu.h"
#include "../include/td.h"

#include <getopt.h>
#include <unistd.h>

#define LEX_DEBUG 1
#define TD_DEBUG_LEAVE 2

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
							RuntimeStackPtr*, LexerPtr*, TopDownParserPtr*, QuadFilePtr*,
							BUParserPtr*, FILE*, char*);

void disposeAll (SymbolTablePtr*, ErrorHandlerPtr*, ConstantTablePtr*,
									RuntimeStackPtr*,LexerPtr*, TopDownParserPtr*, QuadFilePtr*,
									BUParserPtr*, FILE*);

bool getArgs (int argc, char** argv, bool *bLexDebug, bool *bTDDebug);

/******************************************************************************
 Function:    	main

 Description:		used for testing top down parser

 Parameters:  	argc - number of command line arguments
 	 	 	 	 	 	 	  argv - pointers to the command line arguments

 Returned:    	EXIT_SUCCESS
 *****************************************************************************/
int main(int argc, char **argv)
{
	SymbolTable 	* psST = NULL;
	ConstantTable * psCT = NULL;
	RuntimeStack 	* psRTS = NULL;
	ErrorHandler 	* psER = NULL;
	Lexer 				* psLex = NULL;
	TopDownParser * psTD = NULL;
	QuadFile 			* psQD = NULL;
	BUParser			* psBU = NULL;

	FILE *pFile = NULL;

	char *pszFileName;
	char *pszQuadName = "quad.q";
	int lineNum = -1;
	bool bLexDebug = false, bTDDebugLeave = false;

	if (!(SUCCESS == getArgs(argc, argv, &bLexDebug, &bTDDebugLeave)))
	{
		return EXIT_FAILURE;
	}

	pszFileName = argv[optind++];
	pFile = fopen (pszFileName, "r");

  erCreate(&psER);
  if (NULL == pFile) /* if file cannot be opened, exit the program  */
  {
		erAddError (psER, lineNum, CANNOT_OPEN_FILE, pszFileName);
		erPrintErrors (psER);
		erClear (psER);
		erDispose (&psER);
		return EXIT_FAILURE;
  }

  lineNum = 1;

  /* initializes everything  */
  if ( !(SUCCESS == initAll (&psST, &psER, &psCT, &psRTS, &psLex, &psTD, &psQD,
  														&psBU, pFile, pszQuadName)))
  {
  	return EXIT_FAILURE;
  }

//	setDebug(psLex, bLexDebug);
//	tdSetDebugLeave (psTD, bTDDebugLeave);
//	tdSetDebugST (psTD, false);
  setDebug(psLex, false);
  tdSetDebugEnter (psTD, true);
  tdSetDebugLeave (psTD, false);
  tdSetDebugST (psTD, false);

	parse (psTD); /* parse the file */

	/* dump constant table, runtime stack, and the error table */
	ctPrint (psCT);
	rtsPrint (psRTS);
	erPrintErrorPerLine (psER);

	disposeAll (&psST, &psER, &psCT, &psRTS, &psLex, &psTD, &psQD, &psBU, pFile);

	return 0;
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

 Returned:    	bSuccess - true if intialized successfully; otherwise, false
 *****************************************************************************/
bool initAll (SymbolTablePtr* psST, ErrorHandlerPtr *psER, ConstantTablePtr *psCT,
							RuntimeStackPtr *psRTS, LexerPtr *psLex, TopDownParserPtr *psTD,
							QuadFilePtr *psQD, BUParserPtr *psBU ,FILE *pFile, char* pszQuadName)
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

	if (QD_NO_ERROR != quadCreate(psQD, *psER, *psRTS, pszQuadName))
	{
		bSuccess = false;
	}

	if (BU_NO_ERROR != buCreate (psBU, *psLex, *psER, *psQD, *psST, *psRTS))
	{
		bSuccess = false;
	}

	if (TD_NO_ERROR != tdCreate (psTD, *psLex, *psER, *psST, *psRTS, *psQD, *psBU))
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
									LexerPtr *psLex, TopDownParserPtr *psTD, QuadFilePtr* psQD,
									BUParserPtr * psBU, FILE *pFile)
{
	stDispose (psST);
	erDispose (psER);
	ctDispose (psCT);
	rtsDispose (psRTS);
	lexDispose (psLex);
	buDispose (psBU);
	tdDispose (psTD);
	quadDispose (psQD);

	fclose (pFile);
}

/******************************************************************************
 Function:    	getArgs

 Description:		get command line options and set debugging on or off

 Parameters:  	argc - number of command line arguments
 	 	 	 	 	 	 	  argv - pointers to the command line arguments
 	 	 	 	 	 	 	  bLexDebug	- bool to turn on the lex debug
 	 	 	 	 	 	 	  bTDDebug	- bool to turn on the td debug

 Returned:    	bSuccess - true if completed successfully; otherwise, false
 *****************************************************************************/
bool getArgs (int argc, char** argv, bool *bLexDebug, bool *bTDDebug)
{
	bool bSuccess = true;
	int c;

	if (2 > argc || 4 < argc) //if less than 2 arguments.. and more than 4
	{
		printf("USAGE: ./bin/td_driver testFile [--lexdebug] [--tddebugleave] \n");
		return bSuccess = false;
	}

  while (1)
	{
		static struct option long_options[] =
			{
				/* These options set a flag. */
				{"lexdebug", no_argument,  0, LEX_DEBUG},
				{"tddebugleave",   no_argument, 0, TD_DEBUG_LEAVE},
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
			case TD_DEBUG_LEAVE:
				*bTDDebug = true;
				break;
			default:
				abort ();
			}
	}

	return bSuccess;
}
