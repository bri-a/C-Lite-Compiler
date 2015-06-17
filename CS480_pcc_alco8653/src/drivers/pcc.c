/*****************************************************************************
 File name:     pcc.c
 Author:				Brianna Alcoran
 Date:          5/04/15
 Class:         CS480
 Assignment:    Compiler Project written in C
 Purpose:       Compile a program written in C-Lite!
 Modifications: 4/25/15 - changes made for SA2
 	 	 	 	 	 	 	 	5/04/45 - completed changes for SA3
 Hours:					45 - SA2
								25 - SA3
 	 	 	 	 	 	 	 	200 (Total)
******************************************************************************/

#include "../include/td.h"

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
							BUParserPtr* , FILE*, char*);

void disposeAll (SymbolTablePtr*, ErrorHandlerPtr*, ConstantTablePtr*,
									RuntimeStackPtr*,LexerPtr*, TopDownParserPtr*, QuadFilePtr*,
									BUParserPtr* ,FILE*);

bool getArgs (int argc, char** argv,
							bool *bLexDebug, bool *bTDDebugEnter, bool *bTDDebugLeave,
							bool *bBUdumpHandle, bool *bSymDump, bool *, bool *, bool *, bool *,
							char * outputfileName);

/******************************************************************************
 Function:    	main

 Description:		used for testing the compiler! AHH

 Parameters:  	argc - number of command line arguments
 	 	 	 	 	 	 	  argv - pointers to the command line arguments

 Returned:    	EXIT_SUCCESS
 *****************************************************************************/
int main (int argc, char **argv)
{
	SymbolTable 	* psST 		= NULL;
	ConstantTable * psCT 		= NULL;
	RuntimeStack 	* psRTS 	= NULL;
	ErrorHandler 	* psER 		= NULL;
	Lexer 				* psLex 	= NULL;
	TopDownParser * psTD 		= NULL;
	QuadFile 			* psQD 		= NULL;
	BUParser			*	psBU		= NULL;
	FILE 					* pFile 	= NULL;

	char *pszFileName = NULL;
	char *pszQuadName = "quad.q"; //default quad file

	int lineNum = -1;
	bool bLexDebug 			= false,
			 bTDDebugEnter 	= false,
			 bTDDebugLeave 	= false,
			 bBUdumpHandle 	= false,
			 bSymDump 			= false,
			 bTabDump  			= false,
			 bQuadDump 			= false,
			 bTypeCheck 		= false,
			 bPrintDragon 	= false;

	if (!(SUCCESS == getArgs(argc, argv, &bLexDebug, &bTDDebugEnter, &bTDDebugLeave,
										&bBUdumpHandle, &bSymDump, &bTabDump, &bQuadDump, &bTypeCheck,
										&bPrintDragon, pszQuadName)))
	{
		return EXIT_FAILURE;
	}

	/* first argument is filename */
	pszFileName = argv[1];
	pFile = fopen (pszFileName, "r");

	if (!pFile)
	{
		printf("USAGE: ./bin/pcc testFile [--lexdebug]  [--tddebugenter]"
					"  [--tddebugleave] [--budumphandle] [--sa1symdump] [-o quadfile.q]\n");
		return EXIT_FAILURE;
	}

  erCreate(&psER);
  if (NULL == pFile) /* if file not opened, exit the program and display error */
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

  /* set debugging */
	setDebug(psLex, bLexDebug);
	tdSetDebugEnter (psTD, bTDDebugEnter);
	tdSetDebugLeave (psTD, bTDDebugLeave);
	buSetDebug (psBU, bBUdumpHandle);
	tdSetDebugST (psTD, bSymDump);
	setQuadPrintToScreen (psQD, bQuadDump);
	erSetPrintDragon (psER, bPrintDragon);

	parse (psTD); /* parse the file */

	/* dump constant table, runtime stack, and the error table */
	if (bTabDump)
	{
		ctPrint (psCT);
		rtsPrint (psRTS);
	}

	erPrintErrorPerLine (psER);

	if (0 == erGetNumErrors (psER))
	{
		printQuadFile (psQD);
	}

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
							QuadFilePtr *psQD, BUParserPtr *psBU , FILE *pFile, char *pQuad)
{
	bool bSuccess = true;

	if (ST_NO_ERROR != stCreate (psST, SYMBOLTABLE_SIZE,
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
		stDispose (psST);
		erDispose (psER);
		ctDispose (psCT);
		rtsDispose (psRTS);

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
									LexerPtr *psLex, TopDownParserPtr *psTD, QuadFilePtr *psQD,
									BUParserPtr* psBU, FILE *pFile)
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
bool getArgs (int argc, char** argv, bool *bLexDebug, bool *bTDDebugEnter,
							bool *bTDDebugLeave, bool *bBUdumpHandle, bool *bSymDump,
							bool *bTabDump, bool *bQuadDump, bool *bTypeCheck, bool *bPrintDragon,
							char * outputFileName)
{
	bool bSuccess = true;

	if (2 > argc || 7 < argc) //if less than 2 arguments.. and more than 7
	{
		printf("USAGE: ./bin/pcc testFile [--lexdebug]  [--tddebugenter]"
						"  [--tddebugleave] [--budumphandle] [--sa1symdump][-o quadfile.q]\n");
		return bSuccess = false;
	}

	int i;
	char * possibleName = NULL;
	bool bFoundFileName = false;
	bool bNeedFileName = false;

	for (i = 2; i < argc; ++i)
	{

		if (0 == strcmp(argv[i], "--lexdebug"))
		{
			*bLexDebug = true;
		}
		else if (0 == strcmp(argv[i], "--tddebugenter"))
		{
			*bTDDebugEnter = true;
		}
		else if (0 == strcmp(argv[i], "--tddebugleave"))
		{
			*bTDDebugLeave = true;
		}
		else if (0 == strcmp(argv[i], "--budumphandle"))
		{
			*bBUdumpHandle = true;
		}
		else if (0 == strcmp(argv[i], "--sa1symdump"))
		{
			*bSymDump = true;
		}
		else if (0 == strcmp(argv[i], "--sa2tabdump"))
		{
			*bTabDump = true;
		}
		else if (0 == strcmp(argv[i], "--sa3quaddump"))
		{
			*bQuadDump = true;
		}
		else if (0 == strcmp(argv[i], "--sa3typecheck"))
		{
			*bTypeCheck = true;
		}
		else if (0 == strcmp(argv[i], "--dragon"))
		{
			*bPrintDragon = true;
		}
		else if (0 == strcmp(argv[i], "-o"))
		{
			bNeedFileName = true;
		}
		else
		{
			possibleName = argv[i];
			bFoundFileName = true;
		}
	}

	if (bNeedFileName && bFoundFileName)
	{
		outputFileName = possibleName;
	}


	return bSuccess;
}
