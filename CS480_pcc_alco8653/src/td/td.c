/*******************************************************************************
 File name:     td.c
 Author:				Brianna Alcoran
 Date:          2/13/15
 Class:         CS480
 Assignment:    Compiler Project written in C
 Purpose:       Top Down Parser for the Compiler (implementation)
 Modifications:
*******************************************************************************/

#define MAX_PARAMS 256

#include "../include/td.h"

#define ARGUMENT_OFFSET -4
#define LOCAL_OFFSET 0

/* ************			ADDED FOR SA			************** */
typedef union
{
	int stackAddress;
	ReservedWord sRsrvdWrd;
	int quadLocation;
}Misc;

typedef union
{
	int arrayDim;
	int numParams;
}Other;

typedef struct
{
	char pszID [MAX_ID_LENGTH];
	SymbolTableType sType;
	SymbolTableLevel sLevelDecl;
	Other otherField;
	Misc addressField;
	bool bIsArray;
	bool bIsPointer;
	bool bIsFunc;
	bool bIndirect;
	bool bHasReturn;
	int localDataAreaSize;
}ATTRIBUTES;

/* globals for SA  */
char gszFuncName [MAX_ID_LENGTH];
char gszParamsList [MAX_PARAMS][MAX_ID_LENGTH];
int gNumParamsList = 0;
int gNumParamsDeclared = 0;

int gARoffset = 0;
bool gbLocals = false;
bool gbComparingParams = false;
/* ********************************************** */


static void program 			(TopDownParserPtr psTD);
static void externaldefs 	(TopDownParserPtr psTD, ATTRIBUTES *);
static void externaldef 	(TopDownParserPtr psTD, ATTRIBUTES *);
static void typepart 			(TopDownParserPtr psTD, ATTRIBUTES *);
static void vardecl 			(TopDownParserPtr psTD, ATTRIBUTES *);
static void optparamlist 	(TopDownParserPtr psTD, ATTRIBUTES *, ATTRIBUTES *);
static void moreparams 		(TopDownParserPtr psTD, ATTRIBUTES *, ATTRIBUTES *);
static void functionbody 	(TopDownParserPtr psTD, ATTRIBUTES *, ATTRIBUTES *);
static void typedecllist 	(TopDownParserPtr psTD, ATTRIBUTES *, ATTRIBUTES *);
static void idorptr 			(TopDownParserPtr psTD, ATTRIBUTES *);
static void optarraydecl 	(TopDownParserPtr psTD, ATTRIBUTES *, ATTRIBUTES *);
static void moredecls 		(TopDownParserPtr psTD, ATTRIBUTES *, ATTRIBUTES *);
static void functionstmt 	(TopDownParserPtr psTD, ATTRIBUTES *, ATTRIBUTES *);
static void optdecllist 	(TopDownParserPtr psTD, ATTRIBUTES *, ATTRIBUTES *);
static void optinit 			(TopDownParserPtr psTD, ATTRIBUTES *, BUExprStackElement *);
static void initializer 	(TopDownParserPtr psTD, ATTRIBUTES *, BUExprStackElement *);
static void initlist 			(TopDownParserPtr psTD, ATTRIBUTES *, int *);
static void moreinit 			(TopDownParserPtr psTD, ATTRIBUTES *, int *);
static void moreinitdecls (TopDownParserPtr psTD, ATTRIBUTES *);
static void stmtlist 			(TopDownParserPtr psTD, ATTRIBUTES *, ATTRIBUTES *);
static void statement 		(TopDownParserPtr psTD, ATTRIBUTES *, ATTRIBUTES *);
static void optexp 				(TopDownParserPtr psTD, ATTRIBUTES *, ATTRIBUTES *);
static void optelse 			(TopDownParserPtr psTD, ATTRIBUTES *, ATTRIBUTES *);
static void mainprogram 	(TopDownParserPtr psTD, ATTRIBUTES *);
static void arglist 			(TopDownParserPtr psTD, ATTRIBUTES *, ATTRIBUTES *,
														QuadFilePtr, QuadFilePtr);
static void morearglist 	(TopDownParserPtr psTD, ATTRIBUTES *, ATTRIBUTES *,
														QuadFilePtr, QuadFilePtr);
static void EXPRESSION 		(TopDownParserPtr psTD, int *, BUExprStackElement * );
static void PANIC 				(TopDownParserPtr psTD, TokenClass, int );
static void printSTLeaving 		(TopDownParserPtr psTD, bool );
static bool firstOfStatement 	(TopDownParserPtr psTD);
static bool firstOfExpression (TopDownParserPtr psTD);
static bool compare 					(TopDownParserPtr psTD, TokenClass , long int );
static bool checkExtraTokens 	(TopDownParserPtr psTD);
static void calculateAMAttr 	(QuadAddressingMode *, ATTRIBUTES );

/*******************************************************************************
 Function:		tdCreate

 Description:	creates the top down parser

 Parameters:	hsTD	-	handle to the top down parser
							psLex	-	pointer to a lexer
							psER	-	pointer to an error handler

 Returned:		error - a TD error
*******************************************************************************/
TD_ERROR tdCreate (TopDownParserPtr *hsTD, LexerPtr psLex, ErrorHandlerPtr psER,
		SymbolTablePtr psST, RuntimeStackPtr psRTS,  QuadFilePtr psQD,
		BUParserPtr psBU)
{
	TD_ERROR error = TD_NO_ERROR;

	(*hsTD) = (TopDownParserPtr) malloc (sizeof (TopDownParser));

	if (NULL == *hsTD || NULL == psLex || NULL == psER || NULL == psST ||
			NULL == psRTS || NULL == psQD	 || NULL == psBU)
	{
		error = TD_ERROR_NO_CREATE;
	}
	else
	{
		(*hsTD)->psLex 	= psLex;
		(*hsTD)->psER 	= psER;
		(*hsTD)->psST 	= psST;
		(*hsTD)->psRTS 	= psRTS;
		(*hsTD)->psQD 	= psQD;
		(*hsTD)->psBU 	= psBU;
		(*hsTD)->bDebugEnter 	= false;
		(*hsTD)->bDebugLeave 	= false;
		(*hsTD)->bDebugST 		= false;
	}
	return error;
}

/*******************************************************************************
 Function:		tdDispose

 Description:	disposes of a top down parser

 Parameters:	hsTD - handle to the top down parser

 Returned:		error - a TD_ERROR
*******************************************************************************/
TD_ERROR tdDispose (TopDownParserPtr *hsTD)
{
	TD_ERROR error = TD_NO_ERROR;

	if (NULL == *hsTD)
	{
		error = TD_ERROR_NULL_PTR;
	}
	else
	{
		free (*hsTD);
		*hsTD = NULL;
	}
	return error;
}

/*******************************************************************************
 Function:		setDebugEnter

 Description:	sets the parser's bool (bDebugEnter) to the bool passed in. This
							will enable or disable displaying the name of the function just
							entered

 Parameters:	psTD	- pointer to the top down parser
							bDebugEnter	- the bool to set the debugger too to

 Returned:		error - a TD_ERROR
*******************************************************************************/
TD_ERROR tdSetDebugEnter (TopDownParserPtr psTD, bool bDebugEnter)
{
	TD_ERROR error = TD_NO_ERROR;

	if (NULL == psTD)
	{
		error = TD_ERROR_NULL_PTR;
	}
	else
	{
		psTD->bDebugEnter = bDebugEnter;
	}
	return error;
}

/*******************************************************************************
 Function:		setDebugLeave

 Description:	sets the parser's bool (bDebugLeave) to the bool passed in. This
							will enable or disable displaying the name of the function just
							entered

 Parameters:	psTD	- pointer to the top down parser
							bDebugLeave	- the bool to set the debugger too to

 Returned:		error - a TD_ERROR
*******************************************************************************/
TD_ERROR tdSetDebugLeave (TopDownParserPtr psTD, bool bDebugLeave)
{
	TD_ERROR error = TD_NO_ERROR;

	if (NULL == psTD)
	{
		error = TD_ERROR_NULL_PTR;
	}
	else
	{
		psTD->bDebugLeave = bDebugLeave;
	}
	return error;
}
/*******************************************************************************
 Function:		setDebugST

 Description:	sets the parser's bool (bDebugST) to the bool passed in. This
							will enable or disable displaying the symbol table before leaving
							a function

 Parameters:	psTD	- pointer to the top down parser
							bDebugST	- the bool to set the debugger too to

 Returned:		error - a TD_ERROR
*******************************************************************************/
TD_ERROR tdSetDebugST (TopDownParserPtr psTD, bool bDebugST)
{
	TD_ERROR error = TD_NO_ERROR;

	if (NULL == psTD)
	{
		error = TD_ERROR_NULL_PTR;
	}
	else
	{
		psTD->bDebugST = bDebugST;
	}
	return error;
}

/*******************************************************************************
 Function:		printEnter

 Description: if debug enabled, prints function name

 Parameters:	funcName - name of the function
 	 	 	 	 	 	 	bDebugEnter - bool to tell to print or not

 Returned:		none
*******************************************************************************/
void printEnter (const char* funcName, bool bDebugEnter)
{
	if (bDebugEnter)
	{
		printf ("%s\n", funcName);
	}
}

/*******************************************************************************
 Function:		printLeaving

 Description:	if debug enabled, prints function name

 Parameters:	psTD - pointer to the top down parser
 	 	 	 	 	 	  bDebugEnter - bool to tell to print or not

 Returned:		none
*******************************************************************************/
void printLeaving (const char* funcName, bool bDebugLeaving)
{
	if (bDebugLeaving)
	{
		printf ("leaving %s\n", funcName);
	}
}

/*******************************************************************************
 Function:		printSTLeaving

 Description:	prints the ST in sorted order (will be called just before deleting
							a level of the ST)

 Parameters:	psTD - pointer to the top down parser
 	 	 	 	 	 	  bDebugST - bool to tell to print ST or not

 Returned:		none
*******************************************************************************/
static void printSTLeaving (TopDownParserPtr psTD, bool bDebugST)
{
	if (bDebugST)
	{
		printf ("Deleting a level - ST before leaving function %s: \n\n", gszFuncName);

		stPrintSorted (psTD->psST);
	}
}

/*******************************************************************************
 Function:		match

 Description:	checks if the current token of the parser matches the token passed
							in. If so, get the next token. Else, return an error

 Parameters:	psTD - pointer to the top down parser

 Returned:		error - a TD_ERROR
*******************************************************************************/
TD_ERROR match (TopDownParserPtr psTD, TokenClass sTokenClass, int value)
{
	TD_ERROR error = TD_NO_ERROR;
	LEX_ERROR lexError = LEX_NO_ERROR;

	if (compare (psTD, Constant, 0) || compare (psTD, sTokenClass, value) )
	{
		lexError = lexGetToken (psTD->psLex, &(psTD->currentToken));
		if (LEX_NO_MORE_TOKENS == lexError)
		{
			error = TD_ERROR_NO_MORE_TOKENS;
			psTD->currentToken.TokenClass = -1; /* setting current token invalid */
			psTD->currentToken.value = -1;
		}
	}
	else
	{
		error = TD_ERROR_NO_MATCH;
	}

	return error;
}

/*******************************************************************************
 Function:		attrReset

 Description:	sets all member variables of the ATTRIBUTES stuct

 Parameters:	psAttr - pointer to the ATTRIBUTES struct

 Returned:		none
*******************************************************************************/
void attrReset (ATTRIBUTES * psAttr)
{
	memset (psAttr->pszID, '\0', sizeof (psAttr->pszID));
	psAttr->sType = INTEGER;
	psAttr->sLevelDecl = GLOBAL_LEVEL;
	psAttr->bIsArray = false;
	psAttr->bIsPointer = false;
	psAttr->bIsFunc = false;
	psAttr->otherField.arrayDim = NONE;
	psAttr->addressField.stackAddress = NONE;
	psAttr->localDataAreaSize = 0;

}	/* Created for SA */

/*******************************************************************************
 Function:		parse

 Description:	begins parsing (calls first production, program)

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
void parse (TopDownParserPtr psTD)
{
	program (psTD);
}

/*******************************************************************************
 Function:		program

 Description: program -> externaldefs mainprogram

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static void program (TopDownParserPtr psTD)
{
	printEnter (__func__, psTD->bDebugEnter);

	LEX_ERROR sLexError = LEX_NO_ERROR;
	ATTRIBUTES sAttribute;
	attrReset (&sAttribute);

	Quad sQuad;

	/* Create and add quad - PROG_BEGIN */
	setQuad (&sQuad, GLOBAL_LEVEL, OP_PROGBEGIN, IMMEDIATE, 0, IMMEDIATE, 0, IMMEDIATE, 0);
	quadAdd (psTD->psQD, sQuad);

	/* get the first token and begin!! */
	sLexError = lexGetToken (psTD->psLex, &(psTD->currentToken));

	if (LEX_NO_ERROR == sLexError)
	{
			externaldefs (psTD, &sAttribute);
			mainprogram (psTD, &sAttribute);
	}
	else
	{
		erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MAIN_EXPECTED, "");
	}


	stDecrementLevel (psTD->psST);

	/* Create and add quad - PROG_END */
	setQuad (&sQuad, GLOBAL_LEVEL, OP_PROGEND, IMMEDIATE, 0, IMMEDIATE, 0, IMMEDIATE, 0);
	quadAdd (psTD->psQD, sQuad);

	printLeaving (__func__, psTD->bDebugLeave);
}
/*******************************************************************************
 Function:		externaldefs

 Description: externaldefs -> int externaldef | ε

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static void externaldefs (TopDownParserPtr psTD, ATTRIBUTES * psAttr)
{
	printEnter (__func__, psTD->bDebugEnter);

	if (compare (psTD, Reserved_Word, RSRVWRD_INT))
	{
		if (TD_NO_ERROR == match (psTD, Reserved_Word, RSRVWRD_INT))
		{
			externaldef (psTD, psAttr);
		}
	}
	else if (compare (psTD, Identifier, 0) || compare (psTD, Deref, 0))
	{
		erAddError (psTD->psER, lexGetLineNum (psTD->psLex), INVALID_DECLARATION, "");
		externaldef (psTD, psAttr);
	}
	else
	{
		//erAddError (psTD->psER, lexGetLineNum(psTD->psLex), INVALID_DECLARATION, "");
		//PANIC (psTD, -1, -1);
	}

	printLeaving (__func__, psTD->bDebugLeave);
}
/*******************************************************************************
 Function:		externaldef

 Description:	externaldef -> id typepart externaldefs |
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 * id vardecl ; externaldefs

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static void externaldef (TopDownParserPtr psTD, ATTRIBUTES * psAttr)
{
	printEnter (__func__, psTD->bDebugEnter);


	if (compare (psTD, Identifier, 0))
	{
		/* adjust attributes */
		memset (psAttr->pszID, '\0', sizeof (psAttr->pszID));
		strncpy (psAttr->pszID, psTD->currentToken.pszIdentifier,
										strlen (psTD->currentToken.pszIdentifier));
		psAttr->sType = INTEGER;
		psAttr->bIsPointer = false;
		psAttr->bIsFunc = false;
		psAttr->otherField.arrayDim = NONE;
		/**********************************************************/

		if (TD_NO_ERROR == match (psTD, Identifier, 0))
		{
			typepart (psTD, psAttr);
			externaldefs (psTD, psAttr);
		}
	}
	else if ( compare (psTD, Deref, 0) )
	{
		if (TD_NO_ERROR == match (psTD, Deref, 0))
		{
			if ( compare (psTD, Identifier, 0))
			{
				/* adjust attributes */
				memset (psAttr->pszID, '\0', sizeof (psAttr->pszID));
				strncpy (psAttr->pszID, psTD->currentToken.pszIdentifier,
												strlen (psTD->currentToken.pszIdentifier));
				psAttr->sType = POINTER;
				psAttr->bIsPointer = true;
				psAttr->bIsFunc = false;
				psAttr->otherField.arrayDim = NONE;
				/**********************************************************/

				if (TD_NO_ERROR == match (psTD, Identifier, 0))
				{
					vardecl (psTD, psAttr);
					if (TD_NO_ERROR == match (psTD, Semicolon, 0))
					{
						externaldefs (psTD, psAttr);
					}
				}
			}
			else
			{
				erAddError (psTD->psER, lexGetLineNum (psTD->psLex), IDENTIFIER_EXPECTED, "");

				vardecl (psTD, psAttr);
				if ( compare (psTD, Semicolon, 0) )
				{
					if (TD_NO_ERROR == match (psTD, Semicolon, 0))
					{
						externaldefs (psTD, psAttr);
					}
				}
				else
				{
					erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_SEMICOLON, "");
					externaldefs (psTD, psAttr);
				}
			}
		}
	}
	else //if there is no id or * id
	{
		erAddError (psTD->psER, lexGetLineNum (psTD->psLex), IDENTIFIER_EXPECTED, "");
		PANIC (psTD, Dummy, 0);
	}

	printLeaving (__func__, psTD->bDebugLeave);
}
/*******************************************************************************
 Function:		typepart

 Description: typepart -> ( optparamlist ) functionbody | vardecl ;

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static void typepart (TopDownParserPtr psTD, ATTRIBUTES * psAttr)
{
	printEnter (__func__, psTD->bDebugEnter);

	ATTRIBUTES sParam;
	attrReset (&sParam);

	if (compare (psTD, Left_Paren, 0) )
	{
		/* left paren at this point, indicates that the current item is a
		 * function name - set Attr accordingly  */

		/**************						 	 Attr adjust 					*******************/
		psAttr->sType = FUNCNAME;
		psAttr->bIsPointer = false;
		psAttr->bIsArray = false;
		psAttr->bIsFunc = true;

		/* set global variable function name */
		memset (gszFuncName, '\0', sizeof (gszFuncName));
		strncpy (gszFuncName, psAttr->pszID, strlen (psAttr->pszID));
		/*********************************************************************/

		if (TD_NO_ERROR == match (psTD, Left_Paren, 0))
		{
			optparamlist (psTD, &sParam, psAttr);

			if ( compare (psTD, Right_Paren, 0))
			{
				if (TD_NO_ERROR == match (psTD, Right_Paren, 0))
				{
					functionbody (psTD, &sParam, psAttr);
				}
			}
			else
			{
				erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_RPAREN, "");
				functionbody (psTD, &sParam, psAttr);
			}
		}
	}
	//no left paren but check first of optparamlist()
	else if ( compare (psTD, Identifier, 0))
	{
		/* we are pretty sure we have a function name, but the L_PAREN is missing*/

		/**************						 	 Attr adjust 					*******************/
		psAttr->sType = FUNCNAME;
		psAttr->bIsPointer = false;
		psAttr->bIsArray = false;
		psAttr->bIsFunc = true;

		/* set global variable function name */
		memset (gszFuncName, '\0', sizeof (gszFuncName));
		strncpy (gszFuncName, psAttr->pszID, strlen (psAttr->pszID));

		/*********************************************************************/

		erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_LPAREN, "");
		optparamlist (psTD, &sParam, psAttr);
	}
	/* no left paren so check first of vardecl() */
	else if ( compare (psTD, Left_Brack, 0))
	{
		vardecl (psTD, psAttr);
		if (compare (psTD, Semicolon, 0))
		{
			match (psTD, Semicolon, 0);
		}
		else
		{
			erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_SEMICOLON, "");
		}
	}
	else /* just go into vardecl if all else fails  */
	{
		vardecl (psTD, psAttr);
		if (compare (psTD, Semicolon, 0))
		{
			match (psTD, Semicolon, 0);
		}
		else
		{
			erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_SEMICOLON, "");
		}
	}

	printLeaving (__func__, psTD->bDebugLeave);
}
/*******************************************************************************
 Function:		vardecl

 Description:	vardecl -> [ constant ] optinit moreinitdecls |
 	 	 	 	 	 	 	 	 	 	 	 	 optinit moreinitdecls

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static void vardecl (TopDownParserPtr psTD, ATTRIBUTES * psAttr)
{
	printEnter (__func__, psTD->bDebugEnter);

	BUExprStackElement sElem;

	if ( compare (psTD, Left_Brack, 0))
	{
		/* L_BRACK - indicates that we have an array */
		psAttr->bIsArray = true;
		psAttr->bIsFunc = false;

		/* Determine what kind of array it is */
		if (psAttr->bIsPointer && psAttr->bIsArray)
		{
			psAttr->sType = POINTER_ARRAY;
			psAttr->bIndirect = true;
		}
		else if (!(psAttr->bIsPointer) && psAttr->bIsArray)
		{
			psAttr->sType = INTEGER_ARRAY;
		}
		/* **************************************** */


		if (TD_NO_ERROR == match (psTD, Left_Brack, 0))
		{
			if ( compare (psTD, Constant, 0))
			{
				/* we know that we are an array - the constant refers to the array dim */
				psAttr->otherField.arrayDim = atoi (psTD->currentToken.pszIdentifier);

				if (GLOBAL_LEVEL == psAttr->sLevelDecl)
				{
					psAttr->addressField.stackAddress = rtsGetAvailablePos (psTD->psRTS);

					if (((psAttr->otherField.arrayDim + psAttr->addressField.stackAddress) < MAX_STACK))
					{
						int i;
						for (i = 0; i <= psAttr->otherField.arrayDim; ++i)
						{
							rtsAdd (psTD->psRTS, 0);
						}
					}
				}
				else if ( LOCAL_LEVEL == psAttr->sLevelDecl)
				{
					if ( (psAttr->otherField.arrayDim + rtsGetAvailablePos (psTD->psRTS)) >= MAX_STACK )
					{
						erAddError (psTD->psER, lexGetLineNum (psTD->psLex), OUT_OF_MEMORY, "");
					}

					gARoffset += psAttr->otherField.arrayDim;
					if (gARoffset >= MAX_STACK)
					{
						erAddError (psTD->psER, lexGetLineNum (psTD->psLex), OUT_OF_MEMORY, "");
					}
				}

				Quad sArrayQuad;
				SymbolTableLevel sLevel = psAttr->sLevelDecl;
				QuadAddressingMode sAM1, sAM3;

				if (GLOBAL_LEVEL == sLevel)
				{
					sAM1 = sAM3 = GLOBAL_LVALUE;
				}
				else
				{
					sAM1 = sAM3 = LOCAL_LVALUE;
				}

				/* assigns the base ptr of the array to the address of the first
				 * array element  */
				setQuad (&sArrayQuad, psAttr->sLevelDecl, OP_ASSIGN,
														sAM1, psAttr->addressField.stackAddress + 1,
														IMMEDIATE, 0,
														sAM3, psAttr->addressField.stackAddress);

				quadAdd (psTD->psQD, sArrayQuad);


				/* does this array fit onto the stack */

				/*********************************************************************/


				if (TD_NO_ERROR == match (psTD, Constant, (psTD->currentToken).value))
				{
					if ( compare (psTD, Right_Brack, 0))
					{
						if (TD_NO_ERROR == match (psTD, Right_Brack, 0))
						{
							optinit (psTD, psAttr, &sElem);
							moreinitdecls (psTD, psAttr);
						}
					}
					else
					{
						erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_RBRACKET, "");
						optinit (psTD, psAttr, &sElem);
						moreinitdecls (psTD, psAttr);
					}
				}
			}
			/* there was no constant but there was a right bracket */
			else if ( compare (psTD, Right_Brack, 0))
			{
				erAddError (psTD->psER, lexGetLineNum (psTD->psLex), CONSTANT_EXPECTED, "");
				if (TD_NO_ERROR == match (psTD, Right_Brack, 0))
				{
					optinit (psTD, psAttr, &sElem);
					moreinitdecls (psTD, psAttr);
				}
			}
			else
			{
				optinit (psTD, psAttr, &sElem);
				moreinitdecls (psTD, psAttr);
			}
		}
	}/* if no bracket, see if have a constant. if not go to second option */
	else if ( compare (psTD, Constant, 0))
	{
		erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_LBRACKET, "");

		/* we have an array still, but L_BRACK was missing */
		psAttr->bIsArray = true;
		psAttr->bIsFunc = false;

		/* constant refers to the array dim */
		psAttr->otherField.arrayDim = atoi (psTD->currentToken.pszIdentifier);

		/* does this array fit onto the stack */
		if ((psAttr->otherField.arrayDim + rtsGetAvailablePos (psTD->psRTS)) >= MAX_STACK )
		{
			erAddError (psTD->psER, lexGetLineNum (psTD->psLex), OUT_OF_MEMORY, "");
		}

		/* if a local array - adjust activation record offset */
		if (LOCAL_LEVEL == psAttr->sLevelDecl)
		{
			gARoffset += psAttr->otherField.arrayDim;
			if (gARoffset >= MAX_STACK)
			{
				erAddError (psTD->psER, lexGetLineNum (psTD->psLex), OUT_OF_MEMORY, "");
			}
		}
		/*********************************************************************/

		if (TD_NO_ERROR == match (psTD, Constant, 0))
		{
			if ( compare (psTD, Right_Brack, 0) )
			{
				if (TD_NO_ERROR == match (psTD, Right_Brack, 0))
				{
					optinit (psTD, psAttr, &sElem);
					moreinitdecls (psTD, psAttr);
				}
			}
			else
			{
				erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_RBRACKET, "");
				optinit (psTD, psAttr, &sElem);
				moreinitdecls (psTD, psAttr);
			}
		}
	}
	else //was not an array it seems
	{
		if (rtsGetAvailablePos (psTD->psRTS) >= MAX_STACK )
		{
			erAddError (psTD->psER, lexGetLineNum (psTD->psLex), OUT_OF_MEMORY, "");
		}
		else
		{
			if (GLOBAL_LEVEL == psAttr->sLevelDecl)
			{
				rtsAdd (psTD->psRTS, 0);
				psAttr->addressField.stackAddress = rtsGetAvailablePos(psTD->psRTS) - 1;
			}
		}

		optinit (psTD, psAttr, &sElem);
		moreinitdecls (psTD, psAttr);
	}

	printLeaving (__func__, psTD->bDebugLeave);
}
/*******************************************************************************
 Function:		optparamlist

 Description:	optparamlist -> id moreparams | ε

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static void optparamlist (TopDownParserPtr psTD, ATTRIBUTES * psAttr, ATTRIBUTES * psFunc)
{
	printEnter (__func__, psTD->bDebugEnter);

	if ( compare (psTD, Identifier, 0) )
	{

		/* keep track of the params we are seeing */
		strcpy (gszParamsList[gNumParamsList], psTD->currentToken.pszIdentifier);

		memset (psAttr->pszID, '\0', sizeof (psAttr->pszID));
		strncpy (psAttr->pszID, psTD->currentToken.pszIdentifier,
												strlen (psTD->currentToken.pszIdentifier));

		++gNumParamsList;
		if (TD_NO_ERROR == match (psTD, Identifier, 0))
		{
			moreparams (psTD, psAttr, psFunc);
		}
	}
	/* we are just missing the ID and see first of moreparams */
	else if (compare (psTD, Comma, 0))
	{
		erAddError (psTD->psER, lexGetLineNum (psTD->psLex), IDENTIFIER_EXPECTED, "");
		moreparams (psTD, psAttr, psFunc);
	}
	else
	{
		//empty
	}

	printLeaving (__func__, psTD->bDebugLeave);
}
/*******************************************************************************
 Function:		moreparams

 Description:	moreparams -> , id moreparams | ε

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static void moreparams (TopDownParserPtr psTD, ATTRIBUTES * psAttr, ATTRIBUTES * psFunc)
{
	printEnter (__func__, psTD->bDebugEnter);

	/* look if we have the comma */
	if ( compare (psTD, Comma, 0))
	{
		if (TD_NO_ERROR == match (psTD, Comma, 0))
		{
			/* if we have the comma but dont have an identifier */
			if ( compare (psTD, Identifier, 0))
			{
				/* continue to keep track of params */
				strcpy (gszParamsList[gNumParamsList],
									psTD->currentToken.pszIdentifier);
				memset (psAttr->pszID, '\0', sizeof (psAttr->pszID));
				strncpy (psAttr->pszID, psTD->currentToken.pszIdentifier,
																strlen (psTD->currentToken.pszIdentifier));
				++gNumParamsList;
				/*********************************************************************/

				if (TD_NO_ERROR == match (psTD, Identifier, 0))
				{
					moreparams (psTD, psAttr, psFunc);
				}
			}
			else
			{
				erAddError (psTD->psER, lexGetLineNum (psTD->psLex), IDENTIFIER_EXPECTED, "");
				moreparams (psTD, psAttr, psFunc);
			}
		}
	}
	/* if we dont have the comma but have an identifier */
	else if ( compare (psTD, Identifier, 0) )
	{
		/* continue to keep track of params */
		strcpy (gszParamsList[gNumParamsList],
							psTD->currentToken.pszIdentifier);
		memset (psAttr->pszID, '\0', sizeof (psAttr->pszID));
		strncpy (psAttr->pszID, psTD->currentToken.pszIdentifier,
																		strlen (psTD->currentToken.pszIdentifier));
		++gNumParamsList;
		/*********************************************************************/

		erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_COMMA, "");

		if (TD_NO_ERROR == match (psTD, Identifier, 0))
		{
			moreparams (psTD, psAttr, psFunc);
		}
	}
	else
	{
		//empty
	}

	printLeaving (__func__, psTD->bDebugLeave);
}
/*******************************************************************************
 Function:		functionbody

 Description: functionbody -> typedecllist functionstmt

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static void functionbody (TopDownParserPtr psTD, ATTRIBUTES * psAttr, ATTRIBUTES * psFunc)
{
	printEnter (__func__, psTD->bDebugEnter);

	psFunc->sLevelDecl = psTD->psST->currentLevel;
	stIncrementLevel (psTD->psST);
	psAttr->sLevelDecl = psTD->psST->currentLevel;

	/* upon entering function body, know that we are going to begin comparing the
	 * parameters (if any) - adjust globals */
	gbComparingParams = true;
	gARoffset = ARGUMENT_OFFSET;

	typedecllist (psTD, psAttr, psFunc);

	/*********************************************************************/
	/* after returning from typedecllist, we have declared the params, if any
	 * so we double check the number of these params */
	if (gNumParamsDeclared != gNumParamsList)
	{
		erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISMATCHED_PARAMS, gszFuncName);
	}
	gbComparingParams = false;

	/* adjust function attributes for params */
	psFunc->otherField.numParams = gNumParamsList;
	int i;
	for (i = 0; i < gNumParamsList; ++i)
	{
		memset (gszParamsList[i], '\0', sizeof (gszParamsList[i]));
	}
	gNumParamsDeclared = gNumParamsList = 0; /* reset params */
	/*********************************************************************/

	functionstmt (psTD, psAttr, psFunc);
	psAttr->sLevelDecl = psTD->psST->currentLevel;
	psAttr->otherField.arrayDim = NONE;
	psAttr->bIsFunc = false;

	printLeaving (__func__, psTD->bDebugLeave);
}
/*******************************************************************************
 Function:		typedecllist

 Description: typedecllist -> int idorptr optarraydecl moredecls ; typedecclist | ε

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static void typedecllist (TopDownParserPtr psTD, ATTRIBUTES *psAttr, ATTRIBUTES *psFunc)
{
	printEnter (__func__, psTD->bDebugEnter);

	if ( compare (psTD, Reserved_Word, RSRVWRD_INT))
	{
		if (TD_NO_ERROR == match (psTD, Reserved_Word, RSRVWRD_INT))
		{
			psAttr->sType = INTEGER;

			idorptr (psTD, psAttr);
			optarraydecl (psTD, psAttr, psFunc);
			moredecls (psTD, psAttr, psFunc);
			if ( compare (psTD, Semicolon, 0) )
			{
				if (TD_NO_ERROR == match (psTD, Semicolon, 0))
				{
					typedecllist (psTD, psAttr, psFunc);
				}
			}
			else
			{
				erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_SEMICOLON, "");
				typedecllist (psTD, psAttr, psFunc);
			}
		}
	}
	else
	{
		/* catch type declaration errors */
		if (gbComparingParams && gNumParamsList != 0 && gNumParamsDeclared != gNumParamsList)
		{
			erAddError (psTD->psER, lexGetLineNum (psTD->psLex), UNDECLARED_IDENTIFIER, "");
		}
	}

	printLeaving (__func__, psTD->bDebugLeave);
}
/*******************************************************************************
 Function:		idorptr

 Description: idorptr -> id | * id

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static void idorptr (TopDownParserPtr psTD, ATTRIBUTES *psAttr)
{
	printEnter (__func__, psTD->bDebugEnter);

	/*    checking to see if id     */
	if ( compare (psTD, Identifier, 0) )
	{
		if (gbComparingParams)
		{
			/* checking the order of param list with their declarations */
			if ((0 == strcmp (psTD->currentToken.pszIdentifier,
					gszParamsList[gNumParamsDeclared])) )
			{
				memset (psAttr->pszID, '\0', sizeof (psAttr->pszID));
				strncpy (psAttr->pszID, psTD->currentToken.pszIdentifier,
														strlen (psTD->currentToken.pszIdentifier));
				psAttr->addressField.stackAddress = gARoffset+1;
			}
			else if (0 == gNumParamsList)
			{
				erAddError (psTD->psER, lexGetLineNum (psTD->psLex), INVALID_DECLARATION, "");
			}
			else
			{
				erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISMATCHED_PARAMS,
						gszFuncName);
			}

			if (gNumParamsList != 0)
			{
				++gNumParamsDeclared;
			}
		}
		else /* we are not checking params */
		{
			memset (psAttr->pszID, '\0', sizeof (psAttr->pszID));
			strncpy (psAttr->pszID, psTD->currentToken.pszIdentifier,
													strlen (psTD->currentToken.pszIdentifier));
			psAttr->addressField.stackAddress = gARoffset+1;
		}

		if (gbLocals) /* adjust AR offset */
		{
			++gARoffset;
		}
		else
		{
			--gARoffset;
		}


		match (psTD, Identifier, 0);
	}

	else if ( compare (psTD, Deref, 0) )/*    checking to see if *id   */
	{
		psAttr->sType = POINTER;
		psAttr->bIsPointer = true;
		psAttr->bIsFunc = false;

		if (TD_NO_ERROR == match (psTD, Deref, 0 ))
		{
			if ( compare (psTD, Identifier, 0))
			{
				/* checking the order of param list with their declarations */
				if (gbComparingParams)
				{
					if (0 == strcmp(psTD->currentToken.pszIdentifier,
							gszParamsList[gNumParamsDeclared]))
					{
						memset (psAttr->pszID, '\0', sizeof (psAttr->pszID));
						strncpy (psAttr->pszID, psTD->currentToken.pszIdentifier,
																strlen (psTD->currentToken.pszIdentifier));
						psAttr->addressField.stackAddress = gARoffset+1;
					}
					else
					{
						erAddError (psTD->psER, lexGetLineNum (psTD->psLex),
								MISMATCHED_PARAMS, gszFuncName);
					}

					++gNumParamsDeclared;
				}
				else
				{
					memset (psAttr->pszID, '\0', sizeof (psAttr->pszID));
					strncpy (psAttr->pszID, psTD->currentToken.pszIdentifier,
															strlen (psTD->currentToken.pszIdentifier));
					psAttr->addressField.stackAddress = gARoffset+1;
				}

				if (gbLocals) /* make offset adjustments */
				{
					++gARoffset;
				}
				else
				{
					--gARoffset;
				}


				match (psTD, Identifier, 0);
			}
			else
			{
				erAddError (psTD->psER, lexGetLineNum (psTD->psLex),
						IDENTIFIER_EXPECTED, "");
			}
		}
	}
	else /*   there was no id or *id (which sucks)    */
	{
		erAddError (psTD->psER, lexGetLineNum (psTD->psLex),
				IDENTIFIER_EXPECTED, "");
		PANIC (psTD, Dummy, 0);
	}

	printLeaving (__func__, psTD->bDebugLeave);
}
/*******************************************************************************
 Function:		optarraydecl

 Description: optarraydecl -> [] | ε

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static void optarraydecl (TopDownParserPtr psTD, ATTRIBUTES *psAttr,
		ATTRIBUTES * psFunc)
{
	printEnter (__func__, psTD->bDebugEnter);

	/*    checking to see if [     */
	if ( compare (psTD, Left_Brack, 0) )
	{
		/* determine what type of array */
		if (psAttr->bIsPointer)
		{
			psAttr->sType = POINTER_ARRAY;
			psAttr->bIndirect = true;
		}
		else
		{
			psAttr->sType = INTEGER_ARRAY;
		}
		/* **************************************** */


		if (TD_NO_ERROR == match (psTD, Left_Brack, 0 ))
		{
			/*    checking to see if ]     */
			if ( compare (psTD, Right_Brack, 0) )
			{
				match (psTD, Right_Brack, 0);
			}
			else /*    if there was no ]    */
			{
				erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_RBRACKET, "");
			}
		}
	}
	/*    checking to see if ] (which means [ was missing)     */
	else if ( compare (psTD, Right_Brack, 0) )
	{
		/* determine what type of array */
		if (psAttr->bIsPointer)
		{
			psAttr->sType = POINTER_ARRAY;
			psAttr->bIndirect = true;
		}
		else
		{
			psAttr->sType = INTEGER_ARRAY;
		}
		/* **************************************** */


		if (TD_NO_ERROR == match (psTD, Right_Brack, 0))
		{
			erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_LBRACKET, "");
		}
	}
	else /*    there were no brackets at all   */
	{
		//empty
	}

	printLeaving (__func__, psTD->bDebugLeave);
}
/*******************************************************************************
 Function:		moredecls

 Description: moredecls -> , idorptr optarraydecl moredecls | ε

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static void moredecls (TopDownParserPtr psTD, ATTRIBUTES * psAttr,
		ATTRIBUTES * psFunc)
{
	printEnter (__func__, psTD->bDebugEnter);

	/* there is enough we know about the declaration to add it the to ST */
	stAddEntry (psTD->psST, psAttr->pszID, psAttr->sType,
								psAttr->addressField.stackAddress, psAttr->otherField.arrayDim);
	psAttr->sType = INTEGER;
	psAttr->bIsPointer = false;
	psAttr->bIsArray = false;
	psAttr->otherField.arrayDim = NONE;
	psAttr->bIsFunc = false;
	psAttr->addressField.stackAddress = NONE;


	/*    checking to see if ,     */
	if ( compare (psTD, Comma, 0) )
	{
		if (TD_NO_ERROR == match (psTD, Comma, 0 ))
		{
			idorptr (psTD, psAttr);
			optarraydecl (psTD, psAttr, psFunc);
			moredecls (psTD, psAttr, psFunc);
		}
	}
	else if ( compare (psTD, Identifier, 0) || compare (psTD, Deref, 0) )
	{
		erAddError (psTD->psER, lexGetLineNum(psTD->psLex), MISSING_COMMA, "");

		idorptr (psTD, psAttr);
		optarraydecl (psTD, psAttr, psFunc);
		moredecls (psTD, psAttr, psFunc);
	}
	else
	{
		//empty
	}

	printLeaving (__func__, psTD->bDebugLeave);
}
/*******************************************************************************
 Function:		functionstmt

 Description: functionstmt -> { optdecllist stmtlist }

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static void functionstmt (TopDownParserPtr psTD, ATTRIBUTES * psAttr,
		ATTRIBUTES *psFunc)
{
	printEnter (__func__, psTD->bDebugEnter);

	/* in a function, so adjust globals accordingly */
	gbLocals = true;
	gARoffset = LOCAL_OFFSET;
	psFunc->localDataAreaSize = 0;
	psFunc->bIsFunc = true;
	psFunc->bIsArray = psFunc->bIsPointer = psFunc->bHasReturn = false;

	/*  make the quad for the function  */
	Quad sFuncQuad;
	//QuadPtr psUpdateQuad;

	/*  set and add quad - function begin  */
	setQuad (&sFuncQuad, LOCAL_LEVEL, OP_FUNCBEGIN,
											IMMEDIATE, psFunc->localDataAreaSize,
											IMMEDIATE, 0,
											IMMEDIATE, 0);
	quadAdd (psTD->psQD, sFuncQuad);

	int funcQuadPos = 0;
	getQuadFileSize (psTD->psQD, &funcQuadPos);
	psFunc->addressField.quadLocation = funcQuadPos - 1;

	stDecrementLevel (psTD->psST);
	stAddEntry (psTD->psST, psFunc->pszID, psFunc->sType,
		psFunc->addressField.quadLocation, psFunc->otherField.numParams);
	stIncrementLevel (psTD->psST);


	if ( compare (psTD, Left_Brace, 0) )
	{
		if (TD_NO_ERROR == match (psTD, Left_Brace, 0))
		{
			optdecllist (psTD, psAttr, psFunc);
			psFunc->localDataAreaSize = gARoffset;

			//update the quad for this function
			QuadPtr psUpdateFuncQuad;
			getLastFuncQuad (psTD->psQD, &psUpdateFuncQuad);
			setQuadOPD (&psUpdateFuncQuad, OP1,  psFunc->localDataAreaSize);

			stmtlist (psTD, psAttr, psFunc);

			if ( compare (psTD, Right_Brace, 0) )
			{
				match (psTD, Right_Brace, 0);
			}
			else
			{
				erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_RBRACE, "");
			}
		}
		else
		{
			erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_RBRACE, "");
		}
	}
	/*    if we did not see { but we saw the first of optdecllist  */
	else if ( compare (psTD, Reserved_Word, RSRVWRD_INT) )
	{
		erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_LBRACE, "");
		optdecllist (psTD, psAttr, psFunc);


		QuadPtr psUpdateFuncQuad;
		getLastFuncQuad (psTD->psQD, &psUpdateFuncQuad);
		setQuadOPD (&psUpdateFuncQuad, OP1,  psFunc->localDataAreaSize);

		stmtlist(psTD, psAttr, psFunc);
	}
	else
	{
		erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_LBRACE, "");
		stmtlist (psTD, psAttr, psFunc);
	}

	/* have finished dealing with the function statement - so adjust globals & quad*/
	gbLocals = false;

	stDecrementLevel (psTD->psST);

	if (0 == strcmp (psFunc->pszID, "main"))
	{
		ST_DATATYPE * psUpdate = NULL;
		stSearch (psTD->psST, psFunc->pszID, &psUpdate);
		if (NULL == psUpdate)
		{
			erAddError (psTD->psER, lexGetLineNum (psTD->psLex), UNDECLARED_IDENTIFIER, "");
		}
		else
		{
			psUpdate->address = psFunc->addressField.quadLocation;
		}
	}
	else
	{
//		stAddEntry (psTD->psST, psFunc->pszID, psFunc->sType,
//			psFunc->addressField.quadLocation, psFunc->otherField.numParams);
	}

	stIncrementLevel (psTD->psST);

	if (!(psFunc->bHasReturn))
	{
		Quad sResetReturn;
		setQuad (&sResetReturn, LOCAL_LEVEL, OP_ASSIGN,
																IMMEDIATE, 0,	IMMEDIATE, 0, GLOBAL_LVALUE,0);
		quadAdd (psTD->psQD, sResetReturn);

		setQuad (&sFuncQuad, LOCAL_LEVEL, OP_FUNCRETURN,
																IMMEDIATE, 0,	IMMEDIATE, 0, IMMEDIATE, 0);
		quadAdd (psTD->psQD, sFuncQuad);
	}


	printSTLeaving (psTD, psTD->bDebugST);

	stDeleteCurrentLevel (psTD->psST);
	stDecrementLevel (psTD->psST);

	printLeaving (__func__, psTD->bDebugLeave);
}

/*******************************************************************************
 Function:		optdecllist

 Description: optdecllist -> int idorptr vardecl ; optdecllist | ε

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static void optdecllist (TopDownParserPtr psTD, ATTRIBUTES *psAttr,
		ATTRIBUTES *psFunc)
{
	printEnter (__func__, psTD->bDebugEnter);

	if (compare (psTD, Reserved_Word, RSRVWRD_INT))
	{
		if (TD_NO_ERROR == match (psTD, Reserved_Word, RSRVWRD_INT))
		{
			idorptr (psTD, psAttr);
			vardecl (psTD, psAttr);

			if (compare (psTD, Semicolon, 0))
			{
				if (TD_NO_ERROR == match (psTD, Semicolon, 0))
				{
					optdecllist (psTD, psAttr, psFunc);
				}
			}
			else
			{
				erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_SEMICOLON, "");
				optdecllist (psTD, psAttr, psFunc);
			}
		}
	}
	else
	{
		//empty
	}

	psFunc->localDataAreaSize = gARoffset;

	printLeaving (__func__, psTD->bDebugLeave);
}
/*******************************************************************************
 Function:		optinit

 Description: optinit -> = initializer | ε

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static void optinit (TopDownParserPtr psTD, ATTRIBUTES *psAttr,
		BUExprStackElement * psElem )
{
	printEnter (__func__, psTD->bDebugEnter);

	if ( compare (psTD, Assignop, 0) )
	{
		if (TD_NO_ERROR == match (psTD, Assignop, 0 ))
		{
			initializer (psTD, psAttr, psElem);
		}
	}
	else if (compare (psTD, Identifier, 0)  )
	{
		erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_COMMA, "");
	}
	else if ( firstOfExpression (psTD) )
	{
		erAddError (psTD->psER, lexGetLineNum (psTD->psLex), INVALID_DECLARATION, "");
		initializer (psTD, psAttr, psElem);
	}
	else
	{
		//empty
	}

	printLeaving (__func__, psTD->bDebugLeave);
}
/*******************************************************************************
 Function:		initializer

 Description: initializer -> EXPRESSION | { initlist }

 Parameters:	psTD - pointer to the top down parser

 Returned:
*******************************************************************************/
static void initializer (TopDownParserPtr psTD, ATTRIBUTES * psAttr,
		BUExprStackElement * psElem)
{
	printEnter (__func__, psTD->bDebugEnter);

	if ( compare (psTD, Left_Brace, 0) )
	{
		if (TD_NO_ERROR == match (psTD, Left_Brace, 0 ))
		{
			int numInit = 0;
			initlist (psTD, psAttr, &numInit);

			if ( compare (psTD, Right_Brace, 0) )
			{
				match (psTD, Right_Brace, 0);
			}
			else
			{
				erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_RBRACE, "");
			}
		}
	}
	else
	{
		psTD->psBU->bRHS = true;
		EXPRESSION (psTD, &(psAttr->localDataAreaSize), psElem );
		psTD->psBU->bRHS = false;
		Quad sInitQuad;
		QuadAddressingMode sAM1, sAM3;

		if (psElem->bSawDeref)
		{
			int x = psAttr->addressField.stackAddress;
			derefOP(psTD->psBU, psElem, &(x));
		}
		calculateAM (&sAM1, *psElem);

		calculateAMAttr (&sAM3, *psAttr);
		setQuad (&sInitQuad, psAttr->sLevelDecl, OP_ASSIGN,
												sAM1, psElem->address,
												IMMEDIATE, 0,
												sAM3, psAttr->addressField.stackAddress);

		quadAdd (psTD->psQD, sInitQuad);
	}

	printLeaving (__func__, psTD->bDebugLeave);
}
/*******************************************************************************
 Function:		initlist

 Description: initlist -> EXPRESSION moreinit

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static void initlist (TopDownParserPtr psTD, ATTRIBUTES *psAttr, int * numInit)
{
	printEnter (__func__, psTD->bDebugEnter); //todo

	BUExprStackElement sTest;

	int stackPos = psAttr->localDataAreaSize + *numInit;
	EXPRESSION (psTD, &stackPos, &sTest);
	psAttr->localDataAreaSize = stackPos;

	if (psAttr->otherField.arrayDim > *numInit)
	{
		Quad sInitQuad;
		QuadAddressingMode sAM1, sAM3;

		++(*numInit);

		calculateAM (&sAM1, sTest);

		calculateAMAttr (&sAM3, *psAttr);

		setQuad (&sInitQuad, psAttr->sLevelDecl, OP_ASSIGN,
												sAM1, sTest.address,
												IMMEDIATE, 0,
												sAM3, psAttr->addressField.stackAddress + *numInit);
		quadAdd (psTD->psQD, sInitQuad);

	}


	moreinit (psTD, psAttr, numInit);

	printLeaving (__func__, psTD->bDebugLeave);
}
/*******************************************************************************
 Function:		moreinit

 Description:	moreinit -> , EXPRESSION moreinit | ε

 Parameters:	psTD - pointer to the top down parser

 Returned:
*******************************************************************************/
static void moreinit (TopDownParserPtr psTD, ATTRIBUTES *psAttr, int * numInit)
{
	printEnter (__func__, psTD->bDebugEnter);

	BUExprStackElement sTest;

	if ( compare (psTD, Comma, 0) )
	{
		if (TD_NO_ERROR == match (psTD, Comma, 0))
		{
			int stackPos = psAttr->localDataAreaSize + *numInit;
			EXPRESSION (psTD, &stackPos, &sTest);
			psAttr->localDataAreaSize = stackPos;

			if (psAttr->otherField.arrayDim > *numInit)
			{
				Quad sInitQuad;
				QuadAddressingMode sAM1, sAM3;

				++(*numInit);

				calculateAM (&sAM1, sTest);
				calculateAMAttr (&sAM3, *psAttr);

				setQuad (&sInitQuad, psAttr->sLevelDecl, OP_ASSIGN,
														sAM1, sTest.address,
														IMMEDIATE, 0,
														sAM3, psAttr->addressField.stackAddress + *numInit);
				quadAdd (psTD->psQD, sInitQuad);
			}
			moreinit (psTD, psAttr, numInit);
		}
	}
	else if ( firstOfExpression (psTD) )
	{
		erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_COMMA, "");
		EXPRESSION (psTD, &(psAttr->localDataAreaSize), &sTest);
		moreinit (psTD, psAttr, numInit);
	}
	else
	{
		//empty
	}

	printLeaving (__func__, psTD->bDebugLeave);
}
/*******************************************************************************
 Function:		moreintdecls

 Description:	moreinitdecls-> , idorptr vardecl | ε

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static void moreinitdecls (TopDownParserPtr psTD, ATTRIBUTES *psAttr)
{
	printEnter (__func__, psTD->bDebugEnter);


	/* there is enough we know about the declaration to add it the to ST */
	stAddEntry (psTD->psST, psAttr->pszID, psAttr->sType,
								psAttr->addressField.stackAddress, psAttr->otherField.arrayDim);

	//attrReset (psAttr);
	/* reset certain aspects */
	psAttr->sType = INTEGER;
	psAttr->otherField.numParams = NONE;
	psAttr->bIsPointer = false;
	psAttr->bIsArray = false;
	psAttr->bIsFunc = false;
	psAttr->otherField.arrayDim = NONE;
	psAttr->addressField.stackAddress = NONE;
	/*********************************************************************/

	if ( compare (psTD, Comma, 0) )
	{
		if (TD_NO_ERROR == match (psTD, Comma, 0))
		{
			idorptr (psTD, psAttr);
			vardecl (psTD, psAttr);
		}
	}
	else if (compare (psTD, Identifier, 0) ||  compare (psTD, Deref, 0) )
	{
		erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_COMMA, "");
		idorptr (psTD, psAttr);
	}
	else
	{
		//empty
	}

	printLeaving (__func__, psTD->bDebugLeave);
}

/*******************************************************************************
 Function:		stmtlist

 Description:	stmtlist -> statement stmtlist | ε

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static void stmtlist (TopDownParserPtr psTD, ATTRIBUTES *psAttr, ATTRIBUTES *psFunc)
{
	printEnter (__func__, psTD->bDebugEnter);

	/* checking for beginning of statement to decide to go into statement at all */
	if ( firstOfStatement (psTD) )
	{
		statement (psTD, psAttr, psFunc);

		QuadPtr psUpdateFuncQuad;
		getLastFuncQuad (psTD->psQD, &psUpdateFuncQuad);
		setQuadOPD (&psUpdateFuncQuad, OP1,  psFunc->localDataAreaSize);

		stmtlist (psTD, psAttr, psFunc);
	}
	else
	{
		//empty
	}

	printLeaving (__func__, psTD->bDebugLeave);
}
/*******************************************************************************
 Function:		statement

 Description:	statement ->
 	 	 	 	 	 	 	if ( EXPRESSION ) statement optelse |
							for ( EXPRESSION ; EXPRESSION ; EXPRESSION) statement |
							return optexp; |
							{ stmtlist } |
							EXPRESSION; |
							input ( arglist ); |
							output ( arglist );

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static void statement (TopDownParserPtr psTD, ATTRIBUTES *psAttr, ATTRIBUTES *psFunc)
{
	printEnter (__func__, psTD->bDebugEnter);

	BUExprStackElement sTest;

	if ( firstOfStatement (psTD) )
	{
		if ( compare (psTD, Reserved_Word, RSRVWRD_IF) )
		{
			if (TD_NO_ERROR == match (psTD, Reserved_Word, RSRVWRD_IF))
			{
				if ( compare (psTD, Left_Paren, 0))
				{
					if (TD_NO_ERROR == match (psTD, Left_Paren, 0))
					{
						EXPRESSION (psTD, &(psFunc->localDataAreaSize), &sTest);
						Quad sQuad;
						QuadPtr psBeginIf;
						QuadAddressingMode sAM1;

						calculateAM (&sAM1, sTest);
						setQuad (&sQuad, LOCAL_LEVEL, OP_BNOT, sAM1, sTest.address, 0, 0,
																										IMMEDIATE, 0);
						quadAdd (psTD->psQD, sQuad);
						getLastQuad (psTD->psQD, &psBeginIf); //to keep track of the if quad

						if ( compare (psTD, Right_Paren, 0) )
						{
							if (TD_NO_ERROR == match (psTD,  Right_Paren, 0))/* if(EXPRESSION) */
							{
								statement (psTD, psAttr, psFunc);

								int quadSize;
								getQuadFileSize (psTD->psQD, &quadSize);//backpatch original if
								setQuadOPD(&psBeginIf, OP3, quadSize+1);

								setQuad (&sQuad, LOCAL_LEVEL, OP_BRA, 0,0,0,0,0, quadSize+1);
								quadAdd (psTD->psQD, sQuad);
								QuadPtr psEndIf;
								getLastQuad (psTD->psQD, &psEndIf);

								optelse (psTD, psAttr, psFunc);

								getQuadFileSize (psTD->psQD, &quadSize);
								setQuadOPD(&psEndIf, OP3, quadSize);
							}
						}
						else /* if(EXPRESSION */
						{
							erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_RPAREN, "");
							statement (psTD, psAttr, psFunc);
							optelse (psTD, psAttr, psFunc);
						}
					}
				}
				else if (firstOfExpression (psTD)) //did not get the left paren but see first of expression
				{
					erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_LPAREN, "");
					EXPRESSION (psTD, &(psFunc->localDataAreaSize), &sTest);
					if ( compare (psTD, Right_Paren, 0) ) /* if EXPRSSION) */
					{
						if (TD_NO_ERROR == match (psTD, Right_Paren, 0))
						{
							statement (psTD, psAttr, psFunc);
							optelse (psTD, psAttr, psFunc);
						}
					}
					else /* if EXPRSSION */
					{
						erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_RPAREN, "");
						statement (psTD, psAttr, psFunc);
						optelse (psTD, psAttr, psFunc);
					}
				}
				else
				{
					erAddError (psTD->psER, lexGetLineNum (psTD->psLex), BAD_STMT, "");
					PANIC (psTD, Dummy, 0);
				}
			}
		}
		else if ( compare (psTD, Reserved_Word, RSRVWRD_FOR) )
		{
			if (TD_NO_ERROR == match (psTD, Reserved_Word, RSRVWRD_FOR))
			{
				Quad sQuad;
				QuadPtr psSecondExpr, psStatementBegin/*, psQuad*/;
				QuadAddressingMode sAM1;
				int locationForCheck, locationSecondExpr, locationThirdExpr, statementLocation;

				if ( compare (psTD, Left_Paren, 0))
				{
					if (TD_NO_ERROR == match (psTD, Left_Paren, 0))
					{
						//first expression
						if (firstOfExpression(psTD))
						{
							EXPRESSION (psTD, &(psFunc->localDataAreaSize), &sTest);
						}

						getQuadFileSize (psTD->psQD, &locationForCheck);

						if ( compare (psTD, Semicolon, 0) )
						{
							if (TD_NO_ERROR == match (psTD, Semicolon, 0))
							{
								//second expression
								getQuadFileSize (psTD->psQD, &locationSecondExpr);
								EXPRESSION (psTD, &(psFunc->localDataAreaSize), &sTest);

								calculateAM (&sAM1, sTest);
								setQuad (&sQuad, LOCAL_LEVEL, OP_BNOT, sAM1, sTest.address, 0, 0,
																												IMMEDIATE, 0);
								quadAdd (psTD->psQD, sQuad);
								getLastQuad (psTD->psQD, &psSecondExpr);
								//to keep track of the second expression quad

								//branch always to statement
								setQuad (&sQuad, LOCAL_LEVEL, OP_BRA, 0, 0, 0, 0,
																												IMMEDIATE, 0); //needs backpatching
								quadAdd (psTD->psQD, sQuad);
								getLastQuad (psTD->psQD, &psStatementBegin);
							}
						}
						else if ( compare (psTD, Right_Paren, 0) )
						{
							erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_SEMICOLON, "");
							if (TD_NO_ERROR == match (psTD, Right_Paren, 0))
							{
								statement (psTD, psAttr, psFunc);
							}
						}

						if ( compare (psTD, Semicolon, 0) )
						{
							if (TD_NO_ERROR == match (psTD, Semicolon, 0))
							{
								getQuadFileSize (psTD->psQD, &locationThirdExpr);

								//third expression
								EXPRESSION (psTD, &(psFunc->localDataAreaSize), &sTest);

								setQuad (&sQuad, LOCAL_LEVEL, OP_BRA, 0, 0, 0, 0,
																								IMMEDIATE, locationSecondExpr); //needs backpatching
								quadAdd (psTD->psQD, sQuad);

							}
						}
						else if ( compare (psTD, Right_Paren, 0) )
						{
							erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_SEMICOLON, "");
							if (TD_NO_ERROR == match (psTD, Right_Paren, 0))
							{
								statement (psTD, psAttr, psFunc);
							}
						}

						if ( compare (psTD, Right_Paren, 0) )
						{
							if (TD_NO_ERROR == match (psTD, Right_Paren, 0))
							{
								//body of for loop
								getQuadFileSize (psTD->psQD, &statementLocation);
								setQuadOPD(&psStatementBegin, OP3, statementLocation);

								statement (psTD, psAttr, psFunc);

								//back patching!
								setQuad (&sQuad, LOCAL_LEVEL, OP_BRA, 0,0,0,0,0, locationThirdExpr);
								quadAdd (psTD->psQD, sQuad);

								//back patch
								int quadSize;
								getQuadFileSize (psTD->psQD, &quadSize);//back patch original if
								setQuadOPD(&psSecondExpr, OP3, quadSize);

							}
						}
						else if ( compare (psTD, Right_Brace, 0) )
						{
							erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_RPAREN, "");
							statement (psTD, psAttr, psFunc);
						}
					}
				}
				else if (firstOfExpression (psTD))
				{
					erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_LPAREN, "");
					EXPRESSION (psTD, &(psFunc->localDataAreaSize), &sTest);

					if ( compare (psTD, Semicolon, 0) )
					{
						if (TD_NO_ERROR == match (psTD, Semicolon, 0))
						{
							EXPRESSION (psTD, &(psFunc->localDataAreaSize), &sTest);
						}
					}

					if ( compare (psTD, Semicolon, 0) )
					{
						if (TD_NO_ERROR == match (psTD, Semicolon, 0))
						{
							EXPRESSION (psTD, &(psFunc->localDataAreaSize), &sTest);
						}
					}

					if ( compare (psTD, Right_Paren, 0) )
					{
						if (TD_NO_ERROR == match (psTD, Right_Paren, 0))
						{
							statement (psTD, psAttr, psFunc);
						}

					}
					else if (firstOfStatement (psTD))
					{
						erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_RPAREN, "");
						statement (psTD, psAttr, psFunc);
					}
				}
			}

		}
		else if ( compare (psTD, Reserved_Word, RSRVWRD_RETURN) )
		{
			if (TD_NO_ERROR == match (psTD, Reserved_Word, RSRVWRD_RETURN))
			{
				optexp (psTD, psAttr, psFunc);
				if ( compare (psTD, Semicolon, 0) )
				{
					match (psTD, Semicolon, 0);
				}
				else
				{
					erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_SEMICOLON, "");
				}
			}
		}
		else if ( compare (psTD, Semicolon, 0) )
		{
			match (psTD, Semicolon, 0);
		}
		else if ( compare (psTD, Left_Brace, 0) )
		{
			if (TD_NO_ERROR == match (psTD, Left_Brace, 0))
			{
				stmtlist (psTD, psAttr, psFunc);
				if ( compare (psTD, Right_Brace, 0) )
				{
					match (psTD, Right_Brace, 0);
				}
			}
		}
		else if ( compare (psTD, Reserved_Word, RSRVWRD_INPUT)  )
		{
			if (TD_NO_ERROR == match (psTD, Reserved_Word, RSRVWRD_INPUT))
			{
				if ( compare (psTD, Left_Paren, 0))
				{
					if (TD_NO_ERROR == match (psTD, Left_Paren, 0))
					{
						int numInputArgs = 0;
						QuadFile sLoadArgs;
						QuadFile sDerefs;

						memset (&sDerefs, '\0', sizeof(sDerefs));
						memset (&sLoadArgs, '\0', sizeof(sLoadArgs));

						sDerefs.size = sLoadArgs.size = 0;

						arglist (psTD, psAttr, psFunc, &sLoadArgs, &sDerefs);

						///////////////
						numInputArgs = sLoadArgs.size;
						int i;
						for (i = 0; i < sDerefs.size; ++i)
						{
							quadAdd (psTD->psQD, sDerefs.quads[i]);
						}
						for (i = sLoadArgs.size - 1; i >= 0; --i)
						{
							quadAdd (psTD->psQD, sLoadArgs.quads[i]);
						}
						////////////////
						Quad sInputQuad;

						setQuad (&sInputQuad, LOCAL_LEVEL, OP_INPUT,
																IMMEDIATE, numInputArgs,
																IMMEDIATE, 0,
																IMMEDIATE, 0);

						quadAdd (psTD->psQD, sInputQuad);



						if ( compare (psTD, Right_Paren, 0) )
						{
							if (TD_NO_ERROR == match (psTD, Right_Paren, 0))
							{
								if ( compare (psTD, Semicolon, 0) )
								{
									match (psTD, Semicolon, 0);
								}
								else
								{
									erAddError (psTD->psER, lexGetLineNum (psTD->psLex),
																MISSING_SEMICOLON, "");
								}
							}
						}
						else /* if input arglist */
						{
							erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_RPAREN, "");
						}
						if ( compare (psTD, Semicolon, 0) )
						{
							match (psTD, Semicolon, 0);
						}
					}
				}
				else if (firstOfExpression (psTD)) //no left paren but see first expression
				{
					erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_LPAREN, "");
					QuadFile sArgs;
					QuadFile sDerefs;

					arglist (psTD, psAttr, psFunc, &sArgs, &sDerefs);
					if ( compare (psTD, Right_Paren, 0) ) /* if input arglist) */
					{
						match (psTD, Right_Paren, 0);
					}
					else /* if input arglist */
					{
						erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_RPAREN, "");
					}
					if ( compare (psTD, Semicolon, 0) )
					{
						match (psTD, Semicolon, 0);
					}
					else
					{
						erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_SEMICOLON, "");
					}
				}
			}
		}
		else if (compare (psTD, Reserved_Word, RSRVWRD_OUTPUT) )
		{
			if (TD_NO_ERROR == match (psTD, Reserved_Word, RSRVWRD_OUTPUT))
			{
				if ( compare (psTD, Left_Paren, 0))
				{
					if (TD_NO_ERROR == match (psTD, Left_Paren, 0))
					{
						int numOutputArgs = 0;
						QuadFile sLoadArgs;
						QuadFile sDerefs;

						memset (&sDerefs, '\0', sizeof(sDerefs));
						memset (&sLoadArgs, '\0', sizeof(sLoadArgs));

						sDerefs.size = sLoadArgs.size = 0;

						//set the BU
						psTD->psBU->bInOutput = true;
						//

						arglist (psTD, psAttr, psFunc, &sLoadArgs, &sDerefs);

						numOutputArgs = sLoadArgs.size;
						int i;
						for (i = 0; i < sDerefs.size; ++i)
						{
							quadAdd (psTD->psQD, sDerefs.quads[i]);
						}
						for (i = sLoadArgs.size - 1; i >= 0; --i)
						{
							quadAdd (psTD->psQD, sLoadArgs.quads[i]);
						}

						Quad sOutputQuad;
						setQuad (&sOutputQuad, LOCAL_LEVEL, OP_OUTPUT,
																IMMEDIATE, numOutputArgs,
																IMMEDIATE, 0,
																IMMEDIATE, 0);

						quadAdd (psTD->psQD, sOutputQuad);

						//set the BU
						psTD->psBU->bInOutput = false;
						//

						if ( compare (psTD, Right_Paren, 0) )/* if output (arglist) */
						{
							if (TD_NO_ERROR == match (psTD, Right_Paren, 0))
							{
								if ( compare (psTD, Semicolon, 0) )
								{
									match (psTD, Semicolon, 0);/* if output (arglist); */
								}
								else /* if output (arglist) */
								{
									erAddError (psTD->psER, lexGetLineNum (psTD->psLex),
																MISSING_SEMICOLON, "");
								}
							}
						}
						else /* if output (arglist */
						{
							erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_RPAREN, "");
						}
						if ( compare (psTD, Semicolon, 0) )
						{
							match (psTD, Semicolon, 0);
						}
					}
				}
				else if (firstOfExpression (psTD)) /* no left paren but see first expression*/
				{
					erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_LPAREN, "");
					QuadFile sArgs;
					QuadFile sDerefs;

					arglist (psTD, psAttr, psFunc, &sArgs, &sDerefs);
					if ( compare (psTD, Right_Paren, 0) ) /* if output arglist) */
					{
						match (psTD, Right_Paren, 0);
					}
					else /* if output arglist */
					{
						erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_RPAREN, "");
					}
					if ( compare (psTD, Semicolon, 0) )
					{
						match (psTD, Semicolon, 0);
					}
					else
					{
						erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_SEMICOLON, "");
					}
				}
			}
		}
		else if (firstOfExpression (psTD))
		{
			EXPRESSION (psTD, &(psFunc->localDataAreaSize), &sTest);
			if ( compare (psTD, Semicolon, 0) )
			{
				match (psTD, Semicolon, 0);
			}
			else
			{
				erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_SEMICOLON, "");
			}
		}
	}
	else
	{
		erAddError (psTD->psER, lexGetLineNum (psTD->psLex), BAD_STMT, "");
		PANIC (psTD, Dummy, 0);
	}

	printLeaving (__func__, psTD->bDebugLeave);
}
/*******************************************************************************
 Function:		optexp

 Description:	optexp -> EXPRESSION | ε

 Parameters:	psTD - pointer to the top down parser

 Returned:
*******************************************************************************/
static void optexp (TopDownParserPtr psTD, ATTRIBUTES *psAttr, ATTRIBUTES *psFunc)
{
	printEnter (__func__, psTD->bDebugEnter);

	BUExprStackElement sTest;
	int returnLocation, afterReturn;

	if (firstOfExpression (psTD))
	{
		psTD->psBU->bInReturn = psFunc->bHasReturn = true;
		getQuadFileSize (psTD->psQD, &returnLocation);

		EXPRESSION (psTD, &(psFunc->localDataAreaSize), &sTest);

		psTD->psBU->bInReturn = false;

		getQuadFileSize (psTD->psQD, &afterReturn);

		QuadFile sTestFile;
		memset (&sTestFile, '\0', sizeof (QuadFile));
		Quad sTheQuad;

		int i;
		for (i = afterReturn-1; i >= returnLocation; --i)
		{
			if (OP_INCREMENT == psTD->psQD->quads[i].opcode ||
					OP_DECREMENT == psTD->psQD->quads[i].opcode)
			{
				quadPop(psTD->psQD, &sTheQuad);
				quadAdd (&sTestFile, sTheQuad);
				++(psFunc->localDataAreaSize);
			}
		}


		Quad sReturnQuad, sFuncReturnQuad;
		QuadAddressingMode sAM1;

		calculateAM(&sAM1, sTest);
		setQuad (&sReturnQuad, psTD->psST->currentLevel, OP_ASSIGN, sAM1, sTest.address,
															0, 0, GLOBAL_LVALUE, 0);
		quadAdd (psTD->psQD, sReturnQuad);

		quadMerge(psTD->psQD, &sTestFile);
		quadClear (&sTestFile);


		setQuad (&sFuncReturnQuad, LOCAL_LEVEL, OP_FUNCRETURN,
																										IMMEDIATE, 0,
																										IMMEDIATE, 0,
																										IMMEDIATE, 0);
		quadAdd (psTD->psQD, sFuncReturnQuad);
	}

	printLeaving (__func__, psTD->bDebugLeave);
}
/*******************************************************************************
 Function:		optelse

 Description:	optelse -> else statement | ε

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static void optelse (TopDownParserPtr psTD, ATTRIBUTES *psAttr, ATTRIBUTES *psFunc)
{
	printEnter (__func__, psTD->bDebugEnter);

	if ( compare (psTD, Reserved_Word, RSRVWRD_ELSE) )
	{
		if (TD_NO_ERROR == match (psTD, Reserved_Word, RSRVWRD_ELSE) )
		{
			statement (psTD, psAttr, psFunc);
		}
	}
	else if ( compare (psTD, Left_Brace, 0) )
	{
		statement (psTD, psAttr, psFunc);
	}
	else
	{
		//empty
	}

	printLeaving (__func__, psTD->bDebugLeave);
}
/*******************************************************************************
 Function:		mainprogram

 Description:	mainprogram -> main () functionstmt

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static void mainprogram (TopDownParserPtr psTD, ATTRIBUTES * psAttr)
{
	printEnter (__func__, psTD->bDebugEnter);

	ATTRIBUTES sParam;
	attrReset (&sParam);
	sParam.sLevelDecl = LOCAL_LEVEL;

	if ( compare (psTD, Reserved_Word, RSRVWRD_MAIN) )
	{
		/***** we are now in main - change attr and set the global funcName *******/
		memset (gszFuncName, '\0', sizeof (gszFuncName));
		strncpy (gszFuncName, "main", strlen ("main"));

		memset (psAttr->pszID, '\0', sizeof (psAttr->pszID));
		strncpy (psAttr->pszID, "main", strlen ("main"));


		if (TD_NO_ERROR == match (psTD, Reserved_Word, RSRVWRD_MAIN))
		{
			if (compare (psTD, Left_Paren, 0))
			{
				if (TD_NO_ERROR == match (psTD, Left_Paren, 0))
				{
					if ( compare (psTD, Right_Paren, 0)) /* got: main () */
					{
						if (TD_NO_ERROR == match (psTD, Right_Paren, 0))
						{

							stIncrementLevel (psTD->psST);
							functionstmt (psTD, &sParam, psAttr);

							Quad sInvokeMain;
							ST_DATATYPE * psMain;
							stSearch (psTD->psST, "main", &psMain);
							if (NULL == psMain)
							{
								erAddError (psTD->psER, lexGetLineNum (psTD->psLex), UNDECLARED_IDENTIFIER, "");
							}
							else
							{
								setQuad (&sInvokeMain, GLOBAL_LEVEL, OP_FUNCCALL,
																		IMMEDIATE, 0,
																		IMMEDIATE, 0,
																		IMMEDIATE, psMain->address);

								quadAdd (psTD->psQD, sInvokeMain);
							}
						}
					}
					else /* got: main ( */
					{
						erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_RPAREN, "");
						stIncrementLevel (psTD->psST);
						functionstmt (psTD, &sParam, psAttr);
					}
				}
			}
			else if ( compare (psTD, Right_Paren, 0)) /* got: main ) */
			{
				erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_LPAREN, "");
				if (TD_NO_ERROR == match (psTD, Right_Paren, 0))
				{
					stIncrementLevel (psTD->psST);
					functionstmt (psTD, &sParam, psAttr);
				}
			}
			else /* no parens at all panic until functionstmt */
			{
				erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_LPAREN, "");
				PANIC (psTD, Left_Brace, 0);
				stIncrementLevel (psTD->psST);
				functionstmt (psTD, &sParam, psAttr);
			}
		}
	}
	else if (compare (psTD, Left_Paren, 0))
	{
		if (TD_NO_ERROR == match (psTD, Left_Paren, 0))
		{
			erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MAIN_EXPECTED, "");
			if ( compare (psTD, Right_Paren, 0))  /* got: () */
			{
				if (TD_NO_ERROR == match (psTD, Right_Paren, 0))
				{
					stIncrementLevel (psTD->psST);
					functionstmt (psTD, &sParam, psAttr);
				}
			}
		}
	}
	else if (compare (psTD, Right_Paren, 0))
	{
		erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MAIN_EXPECTED, "");
		if (TD_NO_ERROR == match (psTD, Right_Paren, 0))
		{
			stIncrementLevel (psTD->psST);
			functionstmt (psTD, &sParam, psAttr);
		}
	}
	else
	{
		erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MAIN_EXPECTED, "");
		PANIC (psTD, Left_Brace, 0);
		stIncrementLevel (psTD->psST);
		functionstmt (psTD, &sParam, psAttr);
	}

	if (checkExtraTokens (psTD) )
	{
		erAddError (psTD->psER, lexGetLineNum (psTD->psLex), EXTRA_TOKENS, "");
	}

	printLeaving (__func__, psTD->bDebugLeave);
}
/*******************************************************************************
 Function:		arglist

 Description:	EXPRESSION morearglist

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static void arglist (TopDownParserPtr psTD, ATTRIBUTES *psAttr,
									ATTRIBUTES *psFunc, QuadFilePtr psArgs, QuadFilePtr psDerefs)
{
	printEnter (__func__, psTD->bDebugEnter);

	BUExprStackElement sTest;
	Quad sArg;
	/*Quad sDeref;*/
	int tempLocation;
	EXPRESSION (psTD, &(psFunc->localDataAreaSize), &sTest);

	if (psTD->psBU->bInOutput)
	{
		sTest.bCanBeLValue = false;
		sTest.bIndirect = false;
	}
	else
	{
		sTest.bCanBeLValue = true;
	}
	tempLocation = sTest.address;

	QuadAddressingMode sAM1;
	calculateAM(&sAM1, sTest);

	setQuad (&sArg, LOCAL_LEVEL, OP_PUSHPARAM, sAM1, tempLocation,
																	IMMEDIATE, 0, IMMEDIATE, 0);
	quadAdd (psArgs, sArg);


	if ( compare (psTD, Right_Paren, 0))
	{
	}
	else
	{
		morearglist (psTD, psAttr, psFunc, psArgs, psDerefs);
	}

	printLeaving (__func__, psTD->bDebugLeave);
}
/*******************************************************************************
 Function:		morearglist

 Description:	morearglist -> , arglist | ε

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static void morearglist (TopDownParserPtr psTD, ATTRIBUTES *psAttr,
		ATTRIBUTES *psFunc, QuadFilePtr psArgs, QuadFilePtr psDerefs)
{
	printEnter (__func__, psTD->bDebugEnter);

	if ( compare (psTD, Comma, 0) )
	{
		if (TD_NO_ERROR == match (psTD, Comma, 0))
		{
			arglist (psTD, psAttr, psFunc, psArgs, psDerefs);
		}
	}
	else if ( firstOfExpression (psTD) )
	{
		erAddError (psTD->psER, lexGetLineNum (psTD->psLex), MISSING_COMMA, "");
		arglist (psTD, psAttr, psFunc, psArgs, psDerefs);
	}
	else
	{
		//empty
	}

	printLeaving (__func__, psTD->bDebugLeave);
}
/*******************************************************************************
 Function:		EXPRESSION

 Description:	eats tokens until any of the following are found:
 	 	 	 	 	 	 	;
 	 	 	 	 	 	 	,
 	 	 	 	 	 	 	)
 	 	 	 	 	 	 	}

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static void EXPRESSION (TopDownParserPtr psTD, int * localDataSize,
													BUExprStackElement *psElem)
{
	printEnter (__func__, psTD->bDebugEnter);

	LEX_DATATYPE sLastTokenRead;


	buParse (psTD->psBU, &(psTD->currentToken), &sLastTokenRead, localDataSize,
						psElem);


	psTD->currentToken = sLastTokenRead;

	printLeaving (__func__, psTD->bDebugLeave);
}

/*******************************************************************************
 Function:		PANIC

 Description:	PANIC & EAT TOKENS - either stop at the specified stopping token,
 	 	 	 	 	 	 	or stop at a default stopping point: ; or ) or }

 Parameters:	psTD				-	pointer to the top down parser
							sTokenClass - the token class of the token to stop at
							value 			- value(whichOne) of the token to stop at

 Returned:		none
*******************************************************************************/
void PANIC (TopDownParserPtr psTD, TokenClass sTokenClass, int value )
{
	LEX_ERROR error = LEX_NO_ERROR;

	if (Dummy == sTokenClass) //use default
	{
		while ( ( !(compare (psTD, Semicolon, 0)) &&
						!(compare (psTD, Reserved_Word, RSRVWRD_MAIN)) &&
						!(compare (psTD, Right_Paren, 0)) &&
						!(compare (psTD, Right_Brace, 0)) ) &&
						(error == LEX_NO_ERROR)  )
		{
			error = lexGetToken (psTD->psLex, &(psTD->currentToken));
		}
	}
	else //use specified stopping point
	{
		while ( (!(compare (psTD, sTokenClass, value))) && (error == LEX_NO_ERROR) )
		{
			error = lexGetToken (psTD->psLex, &(psTD->currentToken));
		}
	}
}

/*******************************************************************************
 Function:		firstOfStatement

 Description:	returns true if the current token is the first of Statement

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static bool firstOfStatement (TopDownParserPtr psTD)
{
	bool bAtFirstOfStatement = false;

	if (( compare (psTD, Reserved_Word, RSRVWRD_IF))  			||
			( compare (psTD, Reserved_Word, RSRVWRD_FOR)) 			||
			( compare (psTD, Reserved_Word, RSRVWRD_RETURN)) 		||
			( compare (psTD, Semicolon, 0)) 										||
			( compare (psTD, Left_Brace, 0)) 										||
			( firstOfExpression (psTD)) 												||
			( compare (psTD, Reserved_Word, RSRVWRD_INPUT)) 		||
			( compare (psTD, Reserved_Word, RSRVWRD_OUTPUT))
			)
	{
		bAtFirstOfStatement = true;
	}
	return bAtFirstOfStatement;
}

/*******************************************************************************
 Function:		firstOfExpression

 Description:	returns true if the current token is the first of EXPRESSION

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static bool firstOfExpression (TopDownParserPtr psTD)
{
	bool bAtFirstOfExpression = false;

	if ( (compare (psTD, Identifier, 0) ) 	||
			(compare (psTD, Constant, 0) ) 			||
			(compare (psTD, Addressof, 0) ) 		||
			(compare (psTD, Mulop, MUL_TIMES) ) ||
			(compare (psTD, Addop, ADD_SUB)) 		||
			(compare (psTD, Deref, 0)) 					||
			(compare (psTD, Negop, 0))					||
			(compare (psTD, Left_Paren, 0)))
	{
		bAtFirstOfExpression = true;
	}
	return bAtFirstOfExpression;
}

/*******************************************************************************
 Function:		compare

 Description:	returns true if the current token matches the parameters

 Parameters:	psTD - pointer to the top down parser

 Returned:		none
*******************************************************************************/
static bool compare (TopDownParserPtr psTD, TokenClass sTokenClass, long int value)
{
	bool bMatch = false;

	if ( (Constant == sTokenClass && Constant == (psTD->currentToken).TokenClass ) ||
			(Identifier == sTokenClass && Identifier == (psTD->currentToken).TokenClass ) )
	{
		bMatch = true;
	}
	else if ( (psTD->currentToken).TokenClass == sTokenClass &&
			(psTD->currentToken).value == value)
	{
		bMatch = true;
	}
	return bMatch;
}

/*******************************************************************************
 Function:		checkExtraTokens

 Description:	checks for any more tokens in the file

 Parameters:	psTD - pointer to the top down parser

 Returned:		true - if there are more tokens in the file; otherwise, false
*******************************************************************************/
static bool checkExtraTokens (TopDownParserPtr psTD)
{
	bool bExtraTokens = true;

	if (LEX_NO_MORE_TOKENS == lexGetToken (psTD->psLex, &(psTD->currentToken)))
	{
		bExtraTokens = false;
	}
	return bExtraTokens;
}

/*******************************************************************************
 Function:		CalculateAMAttr

 Description: calculate addressing mode of the ATTRIBUTES struct passed in

 Parameters:	*psAM - pointer to the addressing mode
 	 	 	 	 	 	 	sOperand - what we are calculating the AM for

 Returned:		none
*******************************************************************************/
static void calculateAMAttr (QuadAddressingMode *psAM, ATTRIBUTES sOperand)
{
	if (GLOBAL_LEVEL == sOperand.sLevelDecl)
	{
		if (sOperand.bIsPointer && sOperand.bIndirect)
		{
			*psAM = GLOBAL_RVALUE;
		}
		else
		{
			*psAM = GLOBAL_LVALUE;
		}
	}
	else
	{
		if (sOperand.bIsPointer && sOperand.bIndirect)
		{
			*psAM = LOCAL_RVALUE;
		}
		else
		{
			*psAM = LOCAL_LVALUE;
		}
	}
}
