/*******************************************************************************
 File name:     bu.c
 Author:				Brianna Alcoran
 Date:          3/9/15
 Class:         CS480
 Assignment:    Compiler Project written in C
 Purpose:       Bottom Up Parser for the Compiler (implementation)
 Modifications: SA2 changes & SA3 changes
*******************************************************************************/

#include "../include/bu.h"

#define NUM_RHS 18

static int glOperatorPrecedenceTable[NUM_CLASSES][NUM_CLASSES] =
       {{1,1,1,1,1,1,1,1,1,1,1,2,0,2,1,1,2,1,2},
        {0,2,1,1,1,1,1,1,1,1,1,2,0,2,1,1,2,1,2},
        {0,2,2,1,1,1,1,1,1,1,1,2,0,2,1,1,2,1,2},
        {0,2,2,2,1,1,1,1,1,1,1,2,0,2,1,1,2,1,2},
        {0,2,2,2,2,1,1,1,1,1,1,2,0,2,1,1,2,1,2},
        {0,2,2,2,2,2,1,1,1,1,1,2,0,2,1,1,2,1,2},
        {0,2,2,2,2,2,2,1,1,1,1,2,0,2,1,1,2,1,2},
        {2,2,2,2,2,2,2,0,0,0,0,2,0,2,0,0,2,0,2},
        {2,2,2,2,2,2,2,1,1,1,1,2,0,2,1,1,2,1,2},
        {2,2,2,2,2,2,2,0,0,0,1,2,0,2,0,1,2,0,2},
        {1,1,1,1,1,1,1,1,1,1,1,3,0,0,1,1,1,1,0},
        {2,2,2,2,2,2,2,2,0,0,0,2,0,2,0,0,2,0,2},
        {1,1,1,1,1,1,1,1,1,1,1,0,0,3,1,1,0,1,0},
        {2,2,2,2,2,2,2,2,0,0,0,2,0,2,0,0,2,0,2},
        {2,2,2,2,2,2,2,0,0,0,0,2,0,2,0,0,2,0,2},
        {2,2,2,2,2,2,2,2,0,0,3,2,3,2,0,0,2,0,2},
        {1,1,1,1,1,1,1,1,1,1,1,2,0,0,1,1,1,1,0},
        {2,2,2,2,2,2,2,1,1,1,1,2,0,2,1,1,2,1,2},
        {1,1,1,1,1,1,1,1,1,1,1,4,0,0,1,1,4,1,4}};

static TokenClass validHandles [NUM_RHS][MAX_HANDLE] =
{
		{Dummy, 				Assignop, 		Dummy, 					NONE}, //0
		{Dummy, 				Orop, 				Dummy, 					NONE}, //1
		{Dummy, 				Andop, 				Dummy, 					NONE}, //2
		{Dummy, 				Equop, 				Dummy, 					NONE}, //3
		{Dummy, 				Relop, 				Dummy, 					NONE}, //4
		{Dummy, 				Addop, 				Dummy, 					NONE}, //5
		{Dummy, 				Mulop, 				Dummy,					NONE}, //6
		{Autoop, 				Dummy, 				NONE, 					NONE}, //7
		{Dummy, 				Negop, 				NONE, 					NONE}, //8
		{Dummy, 				Addressof, 		NONE, 					NONE}, //9
		{Right_Paren, 	Dummy, 				Left_Paren, 		NONE}, //10
		{Right_Paren, 	Dummy, 				Left_Paren, 		Identifier}, //11
		{NONE, 					NONE, 				NONE, 					NONE}, //12
		{Right_Brack, 	Dummy, 				Left_Brack, 		Identifier}, //13
		{NONE, 					NONE, 				NONE, 					NONE}, //to align with TokenClass enum 14
		{Identifier, 		NONE, 				NONE, 					NONE}, //15
		{Dummy, 				Comma, 				Dummy, 					NONE}, //16
		{Dummy, 				Deref, 				NONE, 					NONE}  //17

};

static BU_ERROR shift 	(BUParserPtr, LEX_DATATYPE**);
static BU_ERROR reduce 	(BUParserPtr, LEX_DATATYPE**, int*, BUExprStackElement **);
static int 			compareTerminals (BUParserPtr ,LEX_DATATYPE, LEX_DATATYPE);
static BU_ERROR handleCheck 	(BUParserPtr, BUExprStackElement[], int);
static BU_ERROR addQuad (BUParserPtr , BUExprStackElement [], int, int *,
																	QuadPtr, BUExprStackElement *);
static BU_ERROR createQuad 		(BUParserPtr , BUExprStackElement [],
											int, int *, QuadOpcode, QuadPtr, BUExprStackElement *);

static bool checkExpressionEnd (BUParserPtr, LEX_DATATYPE *, LEX_DATATYPE **);

/*******************************************************************************
*										Stack Operations
*******************************************************************************/
static BU_ERROR buStkCreate			(BUExprStackPtr );
static BU_ERROR buStkTerminate 	(BUExprStackPtr );
static BU_ERROR buStkIsFull 		(const BUExprStackPtr , bool *);
static BU_ERROR buStkIsEmpty 		(const BUExprStackPtr , bool *);
static BU_ERROR buStkPush 	(BUExprStackPtr , BUExprStackElement );
static BU_ERROR buStkPop 		(BUExprStackPtr , BUExprStackElement *);
static BU_ERROR buStkPeek 	(const BUExprStackPtr , BUExprStackElement *);
static BU_ERROR buStkPeekTerminal (const BUExprStackPtr ,
																					BUExprStackElement *);
static BU_ERROR buStkSize 	(const BUExprStackPtr , int *);
static BU_ERROR buStkReset 	(BUExprStackPtr );

/*******************************************************************************
 Function:		buCreate

 Description:	create a bottom up parser

 Parameters:	hsBU	-	handle to the bottum up parser
							psLex	- pointer to the lexer
 	 	 	 	 	 	 	psER	-	pointer to the error handler

 Returned:		error - BU_ERROR
*******************************************************************************/
BU_ERROR buCreate (BUParserPtr * hsBU, LexerPtr psLex, ErrorHandlerPtr psER,
										QuadFilePtr psQD, SymbolTablePtr psST, RuntimeStackPtr psRTS)
{
	BU_ERROR error = BU_NO_ERROR;
	BUExprStackElement sFirstElem;
	lexCreateToken (0, Dollar, 0, "", &(sFirstElem.Token));

	*hsBU = (BUParserPtr) malloc (sizeof(BUParser));

	if (NULL == *hsBU || NULL == psLex || NULL == psER || NULL == psQD ||
			NULL == psST || NULL == psRTS)
	{
		error = BU_ERROR_NO_CREATE;
	}
	else
	{
		buStkCreate(&((*hsBU)->BUStack));
		buStkPush(&((*hsBU)->BUStack), sFirstElem);
		(*hsBU)->bDebugOn = false;
		(*hsBU)->psLex = psLex;
		(*hsBU)->psER = psER;
		(*hsBU)->psQD = psQD;
		(*hsBU)->psST = psST;
		(*hsBU)->psRTS = psRTS;
		(*hsBU)->bFuncParams = (*hsBU)->bInOutput = (*hsBU)->bInReturn = false;
		(*hsBU)->bRHS = (*hsBU)->bHasNoParams = (*hsBU)->bReducingFunction = false;
		(*hsBU)->numParams = 0;

		memset (&((*hsBU)->psQD2Extra), '\0', sizeof ((*hsBU)->psQD2Extra));
	}

	return error;
}

/*******************************************************************************
 Function:		buDispose

 Description:	disposes of a bottom up parser

 Parameters:	hsBU	-	handle to the bottum up parser

 Returned:		error - BU_ERROR
*******************************************************************************/
BU_ERROR buDispose (BUParserPtr * hsBU)
{
	BU_ERROR error = BU_NO_ERROR;

	if (NULL == *hsBU)
	{
		error = BU_ERROR_NULL_PTR;
	}
	else
	{
		buStkTerminate(&(*hsBU)->BUStack);
		free(*hsBU);
		(*hsBU) = NULL;
	}

	return error;
}

/*******************************************************************************
 Function:		setDebug

 Description:	sets the debug of the bottom up parser to the bool passed in

 Parameters:	psBU	- pointer to the bottom up parser
							debug	-	bool to set the debug too

 Returned:		none
*******************************************************************************/
void buSetDebug (BUParserPtr psBU, bool debug)
{
	psBU->bDebugOn = debug;
}

/*******************************************************************************
 Function:		parse

 Description:	begins parsing an expression

 Parameters:	psBU - pointer to the bottom up parser

 Returned:		bu_error - a BU_ERROR
*******************************************************************************/
BU_ERROR buParse (BUParserPtr psBU, LEX_DATATYPE *psInput, LEX_DATATYPE *psReturn,
										int * localDataSize, BUExprStackElement * psElem)
{
	BU_ERROR bu_error = BU_NO_ERROR;
	LEX_ERROR lex_error = LEX_NO_ERROR;
	BUExprStackElement sTopTerminal, sTop;
	bool bEndOfExpression = false;
	(*psElem).bSawDeref = false;

	while (LEX_NO_ERROR == lex_error && !bEndOfExpression)
	{
		bu_error = buStkPeekTerminal(&(psBU->BUStack), &sTopTerminal);
		if (checkExpressionEnd (psBU, psInput, &psReturn)) //change the semi colon to a dollar
		{
			if (Comma == psInput->TokenClass && psBU->bFuncParams)
			{
				/* then we are not at the end of the expression - do not change the
				 * input token into a dollar */
			}
			else if (Right_Paren != psInput->TokenClass &&
								Dollar != sTopTerminal.Token.TokenClass)
				/* will only change the input to a dollar when we are sure
				   we have reached the end of the expression - at this point */
			{
				psInput->TokenClass = Dollar;
			}
		}
		if (BU_NO_ERROR == bu_error)
		{
			bu_error = buStkPeekTerminal(&(psBU->BUStack), &sTopTerminal);
			if (ACCEPT == compareTerminals(psBU, sTopTerminal.Token, *psInput))
			{
				if (psBU->bDebugOn)
				{
					printf("---ACCEPT---\n\n");
				}
				buStkPeek (&(psBU->BUStack), &sTop);

				if ((psBU->bInOutput && psElem->bSawDeref) || (psBU->bInReturn && psElem->bSawDeref) )
				{
					derefOP (psBU, psElem, localDataSize);
					psElem->bSawDeref = false;
				}

				/* since we have accepted the expression, add any quads that should be
				 * added at this point (autoops) */
				quadMerge (psBU->psQD, &(psBU->psQD2Extra));
				quadClear (&(psBU->psQD2Extra));

				bEndOfExpression = true;
				break;
			}
			else
			{
				if (YIELDS == compareTerminals(psBU, sTopTerminal.Token, *psInput) ||
						EQUALS == compareTerminals(psBU, sTopTerminal.Token, *psInput))
				{
					bu_error = shift (psBU, &psInput);
				}
				else if (TAKES == compareTerminals(psBU, sTopTerminal.Token, *psInput))
				{
					bu_error = reduce (psBU, &psInput, localDataSize, &psElem);
					if (psBU->bReducingFunction)
					{
						//checking to see if we have reduced the "largest" function
						buStkPeekTerminal(&(psBU->BUStack), &sTopTerminal);
						buStkPeek(&(psBU->BUStack), &sTop);
						if (Dummy == sTop.Token.TokenClass && psBU->BUStack.size == psBU->funcStackLoc)
						{
							psBU->bReducingFunction = false;
						}
						else
						{
							psBU->bFuncParams = true;
						}
					}
				}
				else /* received error from comparing terminals */
				{
					bu_error = BU_ERROR_BAD_EXPRESSION;
					bEndOfExpression = true;
				}
			}
		}
		if (BU_ERROR_BAD_EXPRESSION == bu_error)
		{
			if (psBU->bDebugOn)
			{
				printf ("---REJECT---\n\n");
			}
			bEndOfExpression = true;
			erAddError (psBU->psER, lexGetLineNum(psBU->psLex), BAD_EXPRESSION, "");

			/* since we have an error in our expression - panic until a semicolon */
			while (psInput->TokenClass != Semicolon && psInput->TokenClass != Dollar)
			{
				lex_error = lexGetToken(psBU->psLex, psInput);
			}
			break;
		}
	}

	if (LEX_NO_MORE_TOKENS == lex_error)
	{
		bu_error = BU_ERROR_NO_MORE_EXPRESSIONS;
	}

	/* we are done parsing - reset the expression stack */
	buStkReset (& (psBU->BUStack));
	psBU->bRHS = false;

	return bu_error;
}
/*******************************************************************************
 Function:		shift

 Description: adds input terminal to the stack and gets a new input

 Parameters:	psBU - pointer to the bottom up parser

 Returned:		bu_error - a BU_ERROR
*******************************************************************************/
static BU_ERROR shift (BUParserPtr psBU, LEX_DATATYPE **hsInput)
{
	LEX_ERROR lex_error = LEX_NO_ERROR;
	BU_ERROR bu_error = BU_NO_ERROR;
	BUExprStackElement sElem;
	bool bCheckNoParams = false;

	initializeStackElem(&sElem);

	/* convert the lex token into a expression stack element */
	sElem.Token = (**hsInput);

	/* this element has certain attributes based on its class */
	if (Constant == sElem.Token.TokenClass)
	{
		sElem.address = sElem.Token.value;
		sElem.sLevel = GLOBAL_LEVEL;
		sElem.sType = INTEGER;
	}
	else if (Identifier == sElem.Token.TokenClass)
	{
		ST_DATATYPE * psEntry = NULL;

		/* search for the ID in the ST */
		stSearch (psBU->psST, sElem.Token.pszIdentifier, &psEntry);
		if (NULL == psEntry)
		{
			/* if the element is not found in the ST, then it has not yet been declared */
			erAddError (psBU->psER, lexGetLineNum (psBU->psLex), UNDECLARED_IDENTIFIER, "");
		}
		else /* element is found in the ST  */
		{
			if (RSRVWRD == psEntry->sType)
				/* error - cannot use Reserved Words in an expression */
			{
				erAddError (psBU->psER, lexGetLineNum (psBU->psLex), UNDECLARED_IDENTIFIER, "");
			}
			else /* the ID has been declared  */
			{
				sElem.sLevel = psEntry->sLevel;
				sElem.address = psEntry->address;
				sElem.sType = psEntry->sType;

				if (FUNCNAME == psEntry->sType)
				{ /* the expression we will reduce may contain function parameters */
					if (!psBU->bReducingFunction)
					{
						bCheckNoParams = true;
						psBU->funcStackLoc = psBU->BUStack.size + 1;
					}
					psBU->bFuncParams = psBU->bReducingFunction = true;
				}
				else
				{
					if (INTEGER == psEntry->sType || INTEGER_ARRAY == psEntry->sType)
					{
						sElem.bCanBeLValue = true;
					}
					else if (POINTER == psEntry->sType || POINTER_ARRAY == psEntry->sType)
					{
						sElem.bIsPointer = true;
					}
					else if (POINTER_TO_POINTER == psEntry->sType)
					{
						sElem.bCanBeLValue = true;
					}

					if (psBU->bInOutput || psBU->bFuncParams || psBU->bRHS || psBU->bInReturn)
					{
						sElem.bCanBeLValue = false;
					}
				}
			}
		}
	}
	else if (Assignop == sElem.Token.TokenClass)
	{
		psBU->bRHS = true;
	}

	/* get the next token */
	lex_error = lexGetToken (psBU->psLex, *hsInput);

	if (psBU->bReducingFunction && bCheckNoParams)
	{
		//saw a function name, and next token is not L_Paren, then 0 params
		if ((*hsInput)->TokenClass != Left_Paren)
		{
			psBU->bHasNoParams = true;
			psBU->bFuncParams = false;
		}
		else
		{
			psBU->bHasNoParams = false;
		}
	}

	/* push the new element onto the stack */
	bu_error = buStkPush (&(psBU->BUStack), sElem);

	if (LEX_NO_ERROR != lex_error)
	{
		bu_error = BU_ERROR_END_OF_EXPRESSION;
	}

	return bu_error;
}
/*******************************************************************************
 Function:		reduce

 Description:	reduces the expression by popping the top necessary symbols of the
 	 	 	 	 	 	 	stack and replacing with a dummy variable

 Parameters:	psBU - pointer to the bottom up parser

 Returned:		bu_error - a BU_ERROR
*******************************************************************************/
static BU_ERROR reduce (BUParserPtr psBU, LEX_DATATYPE **hsInput,
													int * localDataSize,	BUExprStackElement **hsElem)
{
	BU_ERROR bu_error = BU_NO_ERROR;
	BUExprStackElement sTopTerminal, sPopped, sPoppedTerminal, sDummy, sTop;
	BUExprStackElement handle[MAX_HANDLE];
	Quad sQuad;
	int index = 0;
	bool bLoadedLast = false;

	memset (handle, '\0', sizeof (handle));

	lexCreateToken (0, Dummy, lexGetLineNum(psBU->psLex),"", &(sDummy.Token));
	lexCreateToken (0, Dummy, lexGetLineNum(psBU->psLex),"", &(sPoppedTerminal.Token));

	do
	{
		buStkPop (&(psBU->BUStack), &sPopped);
		buStkPeek (&(psBU->BUStack), &sTop);
		buStkPeekTerminal(&(psBU->BUStack), &sTopTerminal);

		/* capturing the function call parameters to be loaded */
		if (psBU->bFuncParams && !bLoadedLast)
		{
			Quad sLoadParams;
			QuadAddressingMode sAM1;

			/* load the popped Dummy in the format foo(E,E,...,E  OR
			 * load the popped Dummy in the format foo(E) */
			if (((Left_Paren == sTop.Token.TokenClass ||
					Comma == sTop.Token.TokenClass)  &&
					Right_Paren == (**hsInput).TokenClass &&
					Dummy == sPopped.Token.TokenClass) ||
					( Right_Paren == sPoppedTerminal.Token.TokenClass &&
							Left_Paren == sTopTerminal.Token.TokenClass &&
							Dummy == sPopped.Token.TokenClass) )
			{
				if (sPopped.bSawDeref)
				{
					derefOP (psBU, &(sPopped), localDataSize);
				}
				++(psBU->numParams);
				calculateAM (&sAM1, (sPopped));
				setQuad (&sLoadParams, psBU->psST->currentLevel, OP_PUSHPARAM,
													sAM1, (sPopped).address, IMMEDIATE, 0, IMMEDIATE, 0);
				quadAdd (psBU->psQD, sLoadParams);
				bLoadedLast = true; /* the last parameter has been loaded */
			}
		}

		if (sPopped.Token.TokenClass != Dummy)
			/* save the most recently popped terminal */
		{
			lexCreateToken (sPopped.Token.value, sPopped.Token.TokenClass, sPopped.Token.lineNum,
											sPopped.Token.pszIdentifier, &(sPoppedTerminal.Token));
		}

		handle[index] = sPopped; /* save the popped terminals and non-terminals */
		++index;

		if (psBU->bDebugOn)
		{
			printf("%d ", (int)sPopped.Token.TokenClass);
		}

	}while ((YIELDS != compareTerminals (psBU,sTopTerminal.Token, sPoppedTerminal.Token)) ||
					(sTop.Token.TokenClass == Dummy) );

	bu_error = handleCheck (psBU, handle, index); /* look for errors in the RHS*/

	/* add quads according to the handle only if the handle passes type 2 checking */
	if (BU_NO_ERROR == bu_error)
	{
		addQuad (psBU, handle, index, localDataSize, &sQuad, &sDummy);
	}

	/* push the dummy representing the reduced handle onto the stack */
	buStkPush (&(psBU->BUStack), sDummy);
	**hsElem = sDummy;

	if (psBU->bDebugOn)
	{
		printf("\n");
	}

	return bu_error;
}
/*******************************************************************************
 Function:		compareTerminals

 Description:	compares two terminals using the precedence table and captures any
 	 	 	 	 	 	 	errors returned from the operator precedence table

 Parameters:	psBU - pointer to the bottom up parser
 	 	 	 	 	 	 	sStack - terminal on the stack
 	 	 	 	 	 	 	sInput - terminal from the input

 Returned:		retVal - return value from precedence table
*******************************************************************************/
static int compareTerminals (BUParserPtr psBU, LEX_DATATYPE sStack, LEX_DATATYPE sInput)
{
	short int retVal = -1;

	if (sStack.TokenClass < NUM_CLASSES && sInput.TokenClass < NUM_CLASSES )
	{
		retVal = glOperatorPrecedenceTable[sStack.TokenClass][sInput.TokenClass];
	}

	if (ERROR == retVal && sInput.TokenClass == Assignop)
	{
		erAddError(psBU->psER, lexGetLineNum(psBU->psLex), EXPECTED_LVALUE, "");
	}
	else if (ERROR == retVal && (sInput.TokenClass == Addressof ||
																		sInput.TokenClass == Deref ||
																		sInput.TokenClass == Negop))
	{
		erAddError(psBU->psER, lexGetLineNum(psBU->psLex), EXPECTED_LVALUE, "");
	}
	else if (ERROR == retVal && sInput.TokenClass == Right_Paren)
	{
		erAddError(psBU->psER, lexGetLineNum(psBU->psLex), MISSING_LPAREN, "");
	}
	else if (ERROR == retVal && sInput.TokenClass == Right_Brack)
	{
		erAddError(psBU->psER, lexGetLineNum(psBU->psLex), MISSING_LBRACKET, "");
	}
	else if (ERROR == retVal && sStack.TokenClass == Left_Paren)
	{
		erAddError(psBU->psER, lexGetLineNum(psBU->psLex), MISSING_RPAREN, "");
	}
	else if (ERROR == retVal && sStack.TokenClass == Left_Brack)
	{
		erAddError(psBU->psER, lexGetLineNum(psBU->psLex), MISSING_RBRACKET, "");
	}
	else if (ERROR == retVal)
	{
		erAddError(psBU->psER, lexGetLineNum(psBU->psLex), BAD_EXPRESSION, "");
	}

	return retVal;
}

/*******************************************************************************
 Function:		initializeStackElem

 Description: initializes a BUExprStackElement

 Parameters:	psElem - pointer to the element

 Returned:		none
*******************************************************************************/
void initializeStackElem (BUExprStackElement *psElem)
{
	memset(&(psElem->Token), '\0', sizeof (LEX_DATATYPE));
	psElem->sLevel = GLOBAL_LEVEL;
	psElem->sType = NONE;
	psElem->address = 0;
	psElem->bCanBeLValue = psElem->bIndirect = psElem->bIsPointer = \
			psElem->bSawDeref = false;
}

/*******************************************************************************
 Function:		calculateAM

 Description: calculates the addressing mode to be used on the operand

 Parameters: 	*psAM - pointer to the addressing mode
 	 	 	 	 	 	 	sOperand - the BUExprStackElement

 Returned:		error - BU_ERROR
*******************************************************************************/
BU_ERROR calculateAM (QuadAddressingMode *psAM, BUExprStackElement sOperand)
{
	BU_ERROR bu_error = BU_NO_ERROR;

	if (GLOBAL_LEVEL == sOperand.sLevel)
	{
		if (sOperand.bIndirect || !sOperand.bCanBeLValue)
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
		if (sOperand.bIndirect || !sOperand.bCanBeLValue)
		{
			*psAM = LOCAL_RVALUE;
		}
		else
		{
			*psAM = LOCAL_LVALUE;
		}
	}
	return bu_error;
}

/*******************************************************************************
 Function:		addTemp

 Description:	adds a temporary variable according to the current level

 Parameters:	psBU - pointer to the bottom up parser
							localDataSize - handle to the size of the local data
							tempLocation	- pointer to the location of the temp

 Returned:		error - BU_ERROR
*******************************************************************************/
BU_ERROR addTemp (BUParserPtr psBU, int * localDataSize, int * tempLocation)
{
	BU_ERROR bu_error = BU_NO_ERROR;
	RTS_ERROR rts_error = RTS_NO_ERROR;

	SymbolTableLevel sLevelOfTemp = psBU->psST->currentLevel;

	if (GLOBAL_LEVEL == sLevelOfTemp)
	{
		(*tempLocation) = rtsGetAvailablePos(psBU->psRTS);
		rts_error = rtsAdd (psBU->psRTS, 0);
		if (RTS_ERROR_FULL == rts_error)
		{
			erAddError(psBU->psER, lexGetLineNum(psBU->psLex), OUT_OF_MEMORY, "");
		}
	}
	else if (LOCAL_LEVEL == sLevelOfTemp)
	{
		++(*localDataSize);
		(*tempLocation) = (*localDataSize);
	}

	return bu_error;
}

/*******************************************************************************
 Function:		derefOP

 Description:	dereferences the operand

 Parameters:	psBU - pointer to the bottom up parser
 	 	 	 	 	 	  psOP - pointer to the operand being dereferenced
							localDataSize - pointer to the size of the local data

 Returned:		error - BU_ERROR
*******************************************************************************/
BU_ERROR derefOP (BUParserPtr psBU, BUExprStackElement *psOP, int * localDataSize)
{
	BU_ERROR bu_error = BU_NO_ERROR;
	QuadAddressingMode sAM3, sAM1;
	BUExprStackElement sTemp;
	Quad sDeref;
	int tempLocation;

	initializeStackElem (&sTemp);

	addTemp (psBU, localDataSize, &tempLocation);
	sTemp.sLevel = psBU->psST->currentLevel;
	sTemp.address = tempLocation;
	sTemp.bCanBeLValue = true;

	calculateAM (&sAM3, sTemp);
	calculateAM (&sAM1, *psOP);


	setQuad (&sDeref, sTemp.sLevel, OP_DEREFERENCE,
			sAM1, psOP->address, IMMEDIATE, 0, sAM3, tempLocation);
	quadAdd (psBU->psQD, sDeref);

	psOP->address = tempLocation;
	psOP->sLevel = LOCAL_LEVEL;

	return bu_error;
}



/*******************************************************************************
 Function:		addQuad

 Description:	creating and adding quads based on the handle passed in. upon
							entering this function we know that we have a valid handle.

 Parameters:	psBU - pointer to the BU
 	 	 	 	 	 	  sHandle - the handle
 	 	 	 	 	 	  size - size of the handle
 	 	 	 	 	 	  localDataSize - local data size of the current function (AP+address)
 	 	 	 	 	 	  psQuad - pointer to the quad
 	 	 	 	 	 	  psDummy - pointer to the dummy we are going to return

 Returned:		error - BU_ERROR
*******************************************************************************/
static BU_ERROR addQuad (BUParserPtr psBU, BUExprStackElement sHandle[],
													int size, int * localDataSize, QuadPtr psQuad,
													BUExprStackElement *psDummy)
{
	BU_ERROR bu_error = BU_NO_ERROR;
	TokenClass sOperator;
	SymbolTableLevel sLevel = stGetLvl (psBU->psST);

	if (1 == size) /* the handle is either an ID or a Constant */
	{
		(*psDummy) = sHandle[0];
		psDummy->Token.TokenClass = Dummy;
		(*psDummy).address = sHandle[0].address;
		if (Constant == sHandle[0].Token.TokenClass)
		{
			psDummy->bIndirect = true;
			psDummy->bSawDeref = psDummy->bCanBeLValue = false;
		}
		else
		{
			ST_DATATYPE * psEntry = NULL;

			/* search for the ID in the ST */
			stSearch (psBU->psST, sHandle[0].Token.pszIdentifier, &psEntry);

			if (NULL == psEntry)
			{
				erAddError (psBU->psER, lexGetLineNum (psBU->psLex), UNDECLARED_IDENTIFIER, "");
			}
			else
			{
				psDummy->sLevel = psEntry->sLevel;
				if (FUNCNAME == psEntry->sType)
				{
					if (psBU->bHasNoParams)
					{
						int tempLocation;
						Quad sFuncCall, sAssign;

						/* function call quad */
						setQuad (&sFuncCall, sLevel, OP_FUNCCALL, IMMEDIATE, psBU->numParams,
																			IMMEDIATE, 0, IMMEDIATE, sHandle[0].address);
						quadAdd (psBU->psQD, sFuncCall);

						/* assign quad - this is for the function return value */
						addTemp (psBU, localDataSize, &tempLocation);
						setQuad (&sAssign, sLevel, OP_ASSIGN, GLOBAL_RVALUE, 0,
								IMMEDIATE, 0, LOCAL_LVALUE, tempLocation);
						quadAdd (psBU->psQD, sAssign);

						psDummy->address = tempLocation;
						psDummy->sLevel = LOCAL_LEVEL;
					}
				}
				else if (INTEGER == psEntry->sType || INTEGER_ARRAY == psEntry->sType)
				{
					psDummy->bCanBeLValue = true;
					psDummy->bIndirect = psDummy->bIsPointer = false;
				}
				else if (POINTER == psEntry->sType || POINTER_ARRAY == psEntry->sType)
				{
					psDummy->bCanBeLValue = psDummy->bIsPointer = true;
					psDummy->bIndirect = false;
				}
				else if (POINTER_TO_POINTER == psEntry->sType)
				{
				}

				if (psBU->bFuncParams || psBU->bInOutput || psBU->bRHS || psBU->bInReturn)
				{
					psDummy->bCanBeLValue = false;
				}
			}
		}
	}
	else if (2 == size)
	{
		sOperator = sHandle[1].Token.TokenClass;

		switch (sOperator)
		{
			case Deref: /*   the expression is of the form   *E   */
				psDummy->bIndirect = psDummy->bSawDeref = true;
				psDummy->address = sHandle[0].address;
				if (sHandle[0].bSawDeref)
				{
					derefOP (psBU, &(sHandle[0]), localDataSize);

					psDummy->address = sHandle[0].address;
					psDummy->sLevel = sHandle[0].sLevel;
					psDummy->bCanBeLValue = false;
					psDummy->bSawDeref = true;
				}
				break;

			case Negop: /*   the expression if of the form   -E   */
				createQuad (psBU, sHandle, size, localDataSize, OP_MINUS, psQuad, psDummy);
				quadAdd (psBU->psQD, *psQuad);
				break;

			case Addressof: /*   the expression if of the form    &E   */
				if (sHandle[0].bSawDeref)
				{
					psDummy->bSawDeref = false;
					psDummy->bIndirect = true;
				}
				else
				{
					sHandle[0].bIndirect = false;
					sHandle[0].bSawDeref = sHandle[0].bCanBeLValue = true;

					createQuad (psBU, sHandle, size, localDataSize, OP_ASSIGN, psQuad, psDummy);
					quadAdd (psBU->psQD, *psQuad);
				}
				break;

			case Dummy: /*   the expression of the form    E++   */
				createQuad (psBU, sHandle, size, localDataSize, OP_ASSIGN, psQuad, psDummy);
				break;
			default:
				break;
		}

	}
	else if (3 == size)
	{
		sOperator = sHandle[1].Token.TokenClass;
		long int whichOne = sHandle[1].Token.value;

		switch (sOperator)
		{
			case Assignop: /*   E = E   */
				createQuad (psBU, sHandle, size, localDataSize, OP_ASSIGN, psQuad, psDummy);
				quadAdd (psBU->psQD, *psQuad);
				break;

			case Orop:		/*   E || E   */
				createQuad (psBU, sHandle, size, localDataSize, OP_BOR, psQuad, psDummy);
				break;

			case Andop:		/*   E && E   */
				createQuad (psBU, sHandle, size, localDataSize, OP_BAND, psQuad, psDummy);
				break;

			case Equop:		/*   E == E   */

				if (REL_EQUAL_EQUAL == whichOne)
				{
					createQuad (psBU, sHandle, size, localDataSize, OP_BEQ, psQuad, psDummy);
				}
				else if (REL_NOT_EQUAL == whichOne)
				{
					createQuad (psBU, sHandle, size, localDataSize, OP_BNE, psQuad, psDummy);
				}
				break;

			case Relop:
				if (REL_LESS_THAN == whichOne) /*   E < E   */
				{
					createQuad (psBU, sHandle, size, localDataSize, OP_BLT, psQuad, psDummy);
				}
				else if (REL_LESS_EQUAL == whichOne) /*   E <= E   */
				{
					createQuad (psBU, sHandle, size, localDataSize, OP_BLE, psQuad, psDummy);
				}
				else if (REL_GREATER_THAN == whichOne) /*   E > E   */
				{
					createQuad (psBU, sHandle, size, localDataSize, OP_BGT, psQuad, psDummy);
				}
				else if (REL_GREATER_EQUAL == whichOne) /*   E >=   */
				{
					createQuad (psBU, sHandle, size, localDataSize, OP_BGE, psQuad, psDummy);
				}
				break;

			case Addop:
				if (ADD_PLUS == whichOne) /*   E + E   */
				{
					createQuad (psBU, sHandle, size, localDataSize, OP_ADD, psQuad, psDummy);
				}
				else			/*   E - E   */
				{
					createQuad (psBU, sHandle, size, localDataSize, OP_SUBTRACT, psQuad, psDummy);
				}
				quadAdd (psBU->psQD, *psQuad); /*   add the quad that was created from above  */
				break;

			case Mulop:
				if (MUL_TIMES == whichOne) /*   E * E   */
				{
					createQuad (psBU, sHandle, size, localDataSize, OP_MULTIPLY, psQuad, psDummy);
				}
				else if (MUL_DIV == whichOne) /*   E / E   */
				{
					createQuad (psBU, sHandle, size, localDataSize, OP_DIVIDE, psQuad, psDummy);
				}
				else	/*   E % E   */
				{
					createQuad (psBU, sHandle, size, localDataSize, OP_MODULUS, psQuad, psDummy);
				}
				quadAdd (psBU->psQD, *psQuad); /*  add the quad that was created from above   */
				break;

			case Dummy: /*  ( E )   */
				(*psDummy) = sHandle[1];
				break;

			case Comma: /*   E , E   */
				(*psDummy) = sHandle[2];
				break;

			default:
				break;
		}

	}
	else if (4 == size)
	{
		if (sHandle[0].Token.TokenClass == validHandles[Right_Brack][0] )
			/*   the expression is of the form id[E]   */
		{
			QuadAddressingMode sAM1, sAM2, sAM3;
			ST_DATATYPE * psEntry = NULL;
			int opd1, opd2, opd3, tempLocation = 0;
			bool bIsArray = false;


			/* search for the ID in the ST */
			stSearch (psBU->psST, sHandle[3].Token.pszIdentifier, &psEntry);

			if (NULL == psEntry)
			{
				erAddError (psBU->psER, lexGetLineNum (psBU->psLex), UNDECLARED_ARRAY, "");
			}
			else
			{
				if (INTEGER_ARRAY == psEntry->sType || POINTER_ARRAY == psEntry->sType)
				{
					bIsArray = true;
				}
				else
				{
					/* we are using something as an array when it is not...! BAD  */
					erAddError (psBU->psER, lexGetLineNum (psBU->psLex), UNDECLARED_ARRAY, "");
				}

				if (bIsArray)
				{
					//deal with the inner id[E] of the array
					//set up operand 2 (what we will be adding by)
					if (sHandle[1].bSawDeref)
					{
						derefOP(psBU, &(sHandle[1]), localDataSize);
					}

					opd2 = sHandle[1].address;
					sHandle[1].bCanBeLValue = false;
					sHandle[1].bIndirect = true; //use as RVAL
					calculateAM (&sAM2, sHandle[1]);

					//set up operand 1 (base ptr of the array)
					sHandle[3].bCanBeLValue = false;
					sHandle[3].bIndirect = true; //use as RVAL
					calculateAM (&sAM1, sHandle[3]);
					opd1 = sHandle[3].address;

					//set up operand 3
					addTemp (psBU, localDataSize, &tempLocation);
					BUExprStackElement sOp3;
					initializeStackElem(&sOp3);
					opd3 = sOp3.address = tempLocation;
					sOp3.sLevel = psBU->psST->currentLevel;
					sOp3.bCanBeLValue = true;
					sOp3.bIndirect = false;
					calculateAM (&sAM3, sOp3);

					psDummy->address = tempLocation;
					psDummy->sLevel = sOp3.sLevel;
					psDummy->bIndirect = psDummy->bSawDeref = true;
					psDummy->bCanBeLValue =	psDummy->bIsPointer = false;

					setQuad (psQuad, psBU->psST->currentLevel, OP_ADD, sAM1, opd1, sAM2, opd2, sAM3, opd3);
					quadAdd (psBU->psQD, *psQuad);
				}
			}
		}
		else if (sHandle[0].Token.TokenClass == validHandles[Right_Paren][0] )
			/*   expression is of the form foo(E)   */
		{
			int tempLocation;
			Quad sFuncCall, sAssign;

			/* function call quad */
			setQuad (&sFuncCall, sLevel, OP_FUNCCALL, IMMEDIATE, psBU->numParams,
																IMMEDIATE, 0, IMMEDIATE, sHandle[3].address);
			quadAdd (psBU->psQD, sFuncCall);



			/* assign quad - this is for the function return value */
			addTemp (psBU, localDataSize, &tempLocation);
			setQuad (&sAssign, sLevel, OP_ASSIGN, GLOBAL_RVALUE, 0,
					IMMEDIATE, 0, LOCAL_LVALUE, tempLocation);
			quadAdd (psBU->psQD, sAssign);

			psDummy->bCanBeLValue = psDummy->bSawDeref = psDummy->bIsPointer = false;
			psDummy->bIndirect = false;
			psDummy->address = tempLocation;

			psBU->bFuncParams = false;
			psBU->numParams = 0;
		}
	}

	return bu_error;
}

/*******************************************************************************
 Function:		createQuad

 Description: creates more detailed quads based on the handle and opCode passed
							in

 Parameters:	psBU - pointer to the BU
							sHandle - current handle we are reducing
							size - size of the handle
							localDataSize - handle to the size of the local data area
							opCode - opCode we are creating the quad for
							psQuad - pointer to the quad
							psDummy - pointer to the dummy we are reducing the handle to

 Returned:		error - BU_ERROR
*******************************************************************************/
static BU_ERROR createQuad (BUParserPtr psBU, BUExprStackElement sHandle[],
							int size, int *localDataSize, QuadOpcode opCode, QuadPtr psQuad,
							BUExprStackElement *psDummy)
{
	BU_ERROR bu_error = BU_NO_ERROR;
	SymbolTableLevel sLevel = stGetLvl (psBU->psST);
	QuadAddressingMode sAM1, sAM2, sAM3;
	int opd1, opd2, opd3, tempLocation;

	sAM1 = sAM2 = sAM3 = opd1 = opd2 = opd3 = tempLocation = 0;

	/* create quads based on the handle and the opcode */
	if (OP_MINUS == opCode)
	{
		addTemp (psBU, localDataSize, &tempLocation);

		BUExprStackElement sTemp;
		initializeStackElem(&sTemp);
		sTemp.sLevel = sLevel;
		sTemp.address = tempLocation;
		sTemp.bIndirect = false;
		sTemp.bCanBeLValue = true;
		calculateAM (&sAM3, sTemp);
		calculateAM (&sAM1, sHandle[0]);

		opd1 = sHandle[0].address;
		opd3 = tempLocation;

		psDummy->sLevel = sLevel;
		psDummy->address = sTemp.address;
		psDummy->bCanBeLValue = false;
		psDummy->bIndirect = psDummy->bIsPointer = psDummy->bSawDeref = false;

		setQuad (psQuad, sLevel, opCode, sAM1, opd1, sAM2, opd2, sAM3, opd3);
	}
	else if (OP_ASSIGN == opCode)
		/* came from a handle of the form : &E, E++, or E = E*/
	{
		if (2 == size)
		{
			if (Addressof == sHandle[1].Token.TokenClass)
			{
				addTemp (psBU, localDataSize, &tempLocation);
				BUExprStackElement sTemp;
				initializeStackElem(&sTemp);
				sTemp.sLevel = sLevel;
				sTemp.address = tempLocation;
				sTemp.bCanBeLValue = true; //temp must be an LValue in this case
				sTemp.bIndirect = false;

				calculateAM (&sAM3, sTemp);
				calculateAM (&sAM1, sHandle[0]); //this has the value that we want to be assigned


				opd1 = sHandle[0].address;
				opd3 = tempLocation;

				sAM2 = opd2 = 0;
				psDummy->address = tempLocation;
				psDummy->bIndirect = psDummy->bCanBeLValue = false;
				psDummy->sLevel = sTemp.sLevel;

				setQuad (psQuad, sLevel, opCode, sAM1, opd1, sAM2, opd2, sAM3, opd3);
			}
			if (Autoop == sHandle[0].Token.TokenClass)
			{
				BUExprStackElement sTemp;
				initializeStackElem(&sTemp);
				QuadOpcode sWhichAuto;
				Quad sAutoop;
				bool bDoDeref = false;

				if (sHandle[0].Token.value == AUTO_INCR)
				{
					sWhichAuto = OP_INCREMENT;
				}
				else
				{
					sWhichAuto = OP_DECREMENT;
				}

				if (sHandle[1].bSawDeref)
				{
					bDoDeref = true;
				}
				else
				{
					psDummy->bSawDeref = false;
				}

				sHandle[1].bCanBeLValue = true;
				calculateAM (&sAM3, sHandle[1]);
				setQuad (&sAutoop, sLevel, sWhichAuto,
												IMMEDIATE, 0, IMMEDIATE, 0,
												sAM3, sHandle[1].address);
				quadAdd (&(psBU->psQD2Extra), sAutoop);

				addTemp (psBU, localDataSize, &tempLocation);
				sTemp.sLevel = sLevel;
				sTemp.address = tempLocation;
				sTemp.bCanBeLValue = true;
				calculateAM (&sAM3, sTemp);

				sHandle[1].bCanBeLValue = false;
				calculateAM (&sAM1, sHandle[1]);
				opd1 = sHandle[1].address;
				opd3 = tempLocation;

				sAM2 = opd2 = 0;
				psDummy->address = tempLocation;
				psDummy->bIndirect = psDummy->bCanBeLValue =false;
				psDummy->sLevel = sTemp.sLevel;


				setQuad (psQuad, sLevel, opCode, sAM1, opd1, sAM2, opd2, sAM3, opd3);
				quadAdd (psBU->psQD, *psQuad);

				if (bDoDeref)
				{
					derefOP (psBU, &(sHandle[1]), localDataSize);
					psDummy->bSawDeref = true;
				}
			}
		}
		else if (3 == size)
		{
			if (sHandle[0].bSawDeref)
			{
				int tempLocation;
				QuadAddressingMode sTempAM3;
				BUExprStackElement sTemp;
				initializeStackElem(&sTemp);
				Quad sDeref;

				addTemp (psBU, localDataSize, &tempLocation);
				sTemp.sLevel = sLevel;
				sTemp.address = tempLocation;
				sTemp.bCanBeLValue = true;
				sTemp.bIndirect = false;
				calculateAM(&sTempAM3, sTemp);
				calculateAM (&sAM1, sHandle[0]);

				psDummy->address = sTemp.address;
				psDummy->sLevel = sTemp.sLevel;

				setQuad (&sDeref, sTemp.sLevel, OP_DEREFERENCE, sAM1, sHandle[0].address,
																				IMMEDIATE, 0, sTempAM3, tempLocation);
				quadAdd (psBU->psQD, sDeref);

				sHandle[2].bCanBeLValue = true;
				calculateAM (&sAM3, sHandle[2]);
				opd3 = sHandle[2].address;
				opd1 = sTemp.address;
				sHandle[0].bCanBeLValue = false;
				calculateAM (&sAM1, sHandle[0]);

				setQuad (psQuad, sLevel, opCode, sAM1, opd1, 0, 0, sAM3, opd3);
			}
			else
			{
				sHandle[0].bCanBeLValue = false;
				sHandle[2].bCanBeLValue = true;
				calculateAM (&sAM3, sHandle[2]);
				calculateAM (&sAM1, sHandle[0]);

				opd1 = sHandle[0].address;
				opd3 = sHandle[2].address;

				sAM2 = opd2 = 0;

				psDummy->address = opd3;
				psDummy->sLevel = sHandle[2].sLevel;

				setQuad (psQuad, sLevel, opCode, sAM1, opd1, 0, 0, sAM3, opd3);
			}

			psDummy->bCanBeLValue = psDummy->bSawDeref = psDummy->bIndirect = false;
		}
	}
	else if (OP_ADD == opCode || OP_SUBTRACT == opCode || OP_MULTIPLY == opCode ||
						OP_DIVIDE == opCode || OP_MODULUS == opCode)
	{

		if (sHandle[2].bSawDeref || sHandle[2].bIsPointer)
		{
			derefOP (psBU, &(sHandle[2]), localDataSize);
		}

		if (sHandle[0].bSawDeref)
		{
			derefOP (psBU, &(sHandle[0]), localDataSize);
		}

		BUExprStackElement sTemp;
		initializeStackElem(&sTemp);
		addTemp (psBU, localDataSize, &tempLocation);

		sHandle[2].bCanBeLValue = sHandle[0].bCanBeLValue = false;
		calculateAM (&sAM1, sHandle[2]);
		calculateAM (&sAM2, sHandle[0]);

		psDummy->sLevel = sLevel;
		psDummy->address = tempLocation;
		psDummy->bIsPointer = false;
		psDummy->bCanBeLValue = psDummy->bIndirect = true;

		sTemp.sLevel = psBU->psST->currentLevel;
		sTemp.address = tempLocation;
		sTemp.bCanBeLValue = true;
		sTemp.bIndirect = false;

		calculateAM (&sAM3, sTemp);

		opd1 = sHandle[2].address;
		opd2 = sHandle[0].address;
		opd3 = tempLocation;

		psDummy->bCanBeLValue = psDummy->bIndirect = psDummy->bSawDeref = false;

		setQuad (psQuad, sLevel, opCode, sAM1, opd1, sAM2, opd2, sAM3, opd3);
	}
	else if (OP_BLT == opCode || OP_BGT == opCode || OP_BLE == opCode ||
						OP_BGE == opCode ||	OP_BNE == opCode || OP_BEQ == opCode ||
						OP_BAND == opCode || OP_BOR == opCode)
	{

		if (sHandle[2].bSawDeref)
		{
			derefOP (psBU, &(sHandle[2]), localDataSize);
		}
		if (sHandle[0].bSawDeref)
		{
			derefOP (psBU, &(sHandle[0]), localDataSize);
		}

		int label = 0;
		addTemp (psBU, localDataSize, &tempLocation);

		sHandle[2].bCanBeLValue = sHandle[0].bCanBeLValue = false;
		calculateAM (&sAM1, sHandle[2]);
		calculateAM (&sAM2, sHandle[0]);

		psDummy->sLevel = sLevel;
		psDummy->address = tempLocation;
		psDummy->bIsPointer =	psDummy->bCanBeLValue =	psDummy->bIndirect = false;

		calculateAM (&sAM3, *psDummy);

		opd1 = sHandle[2].address;
		opd2 = sHandle[0].address;

		opd3 = tempLocation;

		setQuad (psQuad, sLevel, opCode, sAM1, opd1, sAM2, opd2, IMMEDIATE, label);
		quadAdd (psBU->psQD, *psQuad);

		if (GLOBAL_LEVEL == sLevel)
		{
			sAM3 = GLOBAL_LVALUE;
		}
		else
		{
			sAM3 = LOCAL_LVALUE;
		}

		Quad sFallThrough;
		setQuad (&sFallThrough, sLevel, OP_ASSIGN,
																				IMMEDIATE, 0, IMMEDIATE, 0, sAM3, opd3);
		quadAdd (psBU->psQD, sFallThrough);

		Quad sJump;
		int jump = 0;
		setQuad (&sJump, sLevel, OP_BRA, IMMEDIATE, 0, IMMEDIATE, 0, IMMEDIATE, jump);
		quadAdd (psBU->psQD, sJump);

		int numQuads = psBU->psQD->size;
		QuadPtr psBackTrack;
		getLastOPQuad (psBU->psQD, opCode, &psBackTrack);
		setQuadOPD (&psBackTrack, OP3, numQuads);	//backtrack the initial relop label

		Quad sTrue;
		setQuad (&sTrue, sLevel, OP_ASSIGN, IMMEDIATE, 1, IMMEDIATE, 0, sAM3, opd3);
		quadAdd (psBU->psQD, sTrue);

		numQuads = psBU->psQD->size;
		getLastOPQuad (psBU->psQD, OP_BRA, &psBackTrack); //backtrack the branch label
		setQuadOPD (&psBackTrack, OP3, numQuads);
	}

	return bu_error;
}

/*******************************************************************************
 Function:		handleCheck

 Description:	checks the handle against the RHS

 Parameters:	psBU - pointer to the bottom up parser
							sHandle - an array of terminals and nonterminals (handle)
							size - size of the handle

 Returned:		bu_error - a BU_ERROR
*******************************************************************************/
static BU_ERROR handleCheck (BUParserPtr psBU, BUExprStackElement sHandle[],
																																			int size)
{
	BU_ERROR bu_error = BU_NO_ERROR;

	if (size >= 1)
	{
		if (sHandle[0].Token.TokenClass == Dummy && size > 1)
		{
			if ((sHandle[1].Token.TokenClass == Assignop 	||
					sHandle[1].Token.TokenClass == Orop 			||
					sHandle[1].Token.TokenClass == Andop 			||
					sHandle[1].Token.TokenClass == Equop 			||
					sHandle[1].Token.TokenClass == Relop 			||
					sHandle[1].Token.TokenClass == Addop 			||
					sHandle[1].Token.TokenClass == Mulop )) //looking at binary operator
			{
				if ( size != 3 ) //we dont have the correct binary op format
				{
					erAddError (psBU->psER, lexGetLineNum(psBU->psLex), EXPECTED_LVALUE, "");
					bu_error = BU_ERROR_BAD_EXPRESSION;
				} //checked binary operator
				else
				{
					if (sHandle[2].Token.TokenClass == Dummy)
					{
						//we have a valid handle
					}
				}
			}

		}
		else if (sHandle[0].Token.TokenClass == Autoop && size > 1)
		{
			if (size != 2 || sHandle[1].Token.TokenClass != Dummy)
			{
				erAddError (psBU->psER, lexGetLineNum(psBU->psLex), EXPECTED_LVALUE, "");
				bu_error = BU_ERROR_BAD_EXPRESSION;
			}
		}
		else if (sHandle[0].Token.TokenClass == Right_Paren)
		{
			if (size >= 3)
			{
				if (sHandle[2].Token.TokenClass != Left_Paren)
				{
					erAddError (psBU->psER, lexGetLineNum(psBU->psLex), MISSING_LPAREN, "");
					bu_error = BU_ERROR_BAD_EXPRESSION;
				}
				if (size == 4)
				{
					if (sHandle[3].Token.TokenClass != Identifier)
					{
						erAddError (psBU->psER, lexGetLineNum(psBU->psLex), IDENTIFIER_EXPECTED, "");
						bu_error = BU_ERROR_BAD_EXPRESSION;
					}
				}
			}
			else if (size == 2)
			{
				if (sHandle[1].Token.TokenClass != Left_Paren)
				{
					erAddError (psBU->psER, lexGetLineNum(psBU->psLex), MISSING_LPAREN, "");
					bu_error = BU_ERROR_BAD_EXPRESSION;
				}
			}
		}
		else if ((sHandle[0].Token.TokenClass == Addressof || sHandle[0].Token.TokenClass == Deref ||
				sHandle[0].Token.TokenClass == Negop )&& size == 1) //checking unary operators
		{
			erAddError (psBU->psER, lexGetLineNum(psBU->psLex), EXPECTED_LVALUE, "");
			bu_error = BU_ERROR_BAD_EXPRESSION;
		}
	}

	return bu_error;
}

/*******************************************************************************
 Function:		checkExpressionEnd

 Description: checks to see if we are at the end of an expression based on
 	 	 	 	 	 	 	the current lex token

 Parameters:	psInput - pointer to the current lex token
 	 	 	 	 	 	 	hsReturn - handle to the token indicating we may be at the end
 	 	 	 	 	 	 	of the expression

 Returned:		true - if we are at a stopping point in an expression; otherwise,
 	 	 	 	 	 	 	 	 	 	 false
*******************************************************************************/
static bool checkExpressionEnd (BUParserPtr psBU, LEX_DATATYPE *psInput,
																										LEX_DATATYPE ** hsReturn)
{
	bool bIsEndingToken = false;
	if (psInput->TokenClass == Semicolon ||
			psInput->TokenClass == Comma ||
			psInput->TokenClass == Right_Paren ||
			psInput->TokenClass == Right_Brace)
	{
		bIsEndingToken = true;
		(*hsReturn)->value = 0;
		switch (psInput->TokenClass)
		{
			case Semicolon:
				(*hsReturn)->TokenClass = Semicolon;
				break;
			case Comma:
				(*hsReturn)->TokenClass = Comma;
				break;
			case Right_Paren:
				(*hsReturn)->TokenClass = Right_Paren;
				if (psBU->bFuncParams)
				{
					bIsEndingToken = false;
				}
				break;
			case Right_Brace:
				(*hsReturn)->TokenClass = Right_Brace;
				break;
			default:
				break;
		}
	}
	return bIsEndingToken;
}

/*******************************************************************************
*
*										Stack Operations
*
*******************************************************************************/

/****************************************************************************
 Function:  	buStkCreate

 Description:	Creates a stack and initializes both the size and top to 0,
 	 	 	 				unless psBUStack is NULL.

 Parameters:	psBUStack - the pointer to the stack

 Returned:   	ERROR_NO_STACK_CREATE if the stack cannot be created
 	 	 	 				NO_ERROR if no errors occur
 ****************************************************************************/

static BU_ERROR buStkCreate (BUExprStackPtr psBUStack)
{
	BU_ERROR error = NO_ERROR;

	if (NULL == psBUStack)
	{
		error = BU_ERROR_NO_STACK_CREATE;
	}
	else
	{
		psBUStack->size = 0;
		psBUStack->top = 0;
	}

	return error;
}

/****************************************************************************
 Function:  	buStkTerminate

 Description:	Terminates a stack by resetting stack members to 0 unless
 	 	 	 				the psBUStack is NULL.

 Parameters:	psBUStack - the pointer to the stack

 Returned:   	ERROR_NO_STACK_TERMINATE if the stack cannot be terminated
 	 	 	 				NO_ERROR if no errors occur

 ****************************************************************************/
static BU_ERROR buStkTerminate (BUExprStackPtr psBUStack)
{
	BU_ERROR error = NO_ERROR;

	if (NULL == psBUStack)
	{
		error = BU_ERROR_NO_STACK_TERMINATE;
	}
	else
	{
		psBUStack->size = 0;
		psBUStack->top = 0;
	}

	return error;
}

/****************************************************************************
 Function:  	buStkIsFull

 Description:	Checks if the stack is full and requires that psBUStack is not
 	 	 	 				NULL. If the stack is full adjust *pbIsFull to true. If the
 	 	 	 				stack is not full, adjust *pbIsFull to false.

 Parameters:	psBUStack - the pointer to the stack
 	 	 	 				*pbIsFull - pointer to a boolean

 Returned:   	NO_ERROR if no errors occur
	 	 	 				ERROR_NO_STACK_MEMORY if psBUStack is NULL

 ****************************************************************************/
static BU_ERROR buStkIsFull (const BUExprStackPtr psBUStack, bool *pbIsFull)
{
	BU_ERROR error = NO_ERROR;

	if (NULL == psBUStack)
	{
		error = BU_ERROR_NO_STACK_MEMORY;
	}
	else
	{
		if (MAX_TOKEN_STACK == psBUStack->top)
		{
			*pbIsFull = true;
			error = NO_ERROR;
		}
		else
		{
			*pbIsFull = false;
			error = NO_ERROR;
		}
	}

	return error;
}
/****************************************************************************
 Function:  	buStkIsEmpty

 Description:	Checks if the stack is empty and requires that psBUStack is not
 	 	 	 				NULL. If the stack is empty adjust *pbIsEmpty to true. If the
 	 	 	 				stack is not empty, adjust *pbIsEmpty to false.

 Parameters:	psBUStack - the pointer to the stack
 	 	 	 				*pbIsEmpty - pointer to a boolean

 Returned:   	NO_ERROR if no errors occur
 	 	 	 				ERROR_NO_STACK_MEMORY if psBUStack is NULL

 ****************************************************************************/
static BU_ERROR buStkIsEmpty (const BUExprStackPtr psBUStack, bool *pbIsEmpty)
{
	BU_ERROR error = NO_ERROR;
	int size;

	if (NULL == psBUStack)
	{
		error = BU_ERROR_NO_STACK_MEMORY;
	}
	else
	{
		buStkSize (psBUStack, &size);

		if (0 == size)
		{
			*pbIsEmpty = true;
		}
		else
		{
			*pbIsEmpty = false;
		}
	}

	return error;
}

/****************************************************************************
 Function:  	buStkPush

 Description:	Requires that the stack be not full and requires that psBUStack
							is not NULL. If it is not full, push value to the stack, and
 	 	 	 				increment the size and top. If it is full, return the error.

 Parameters:	psBUStack - the pointer to the stack
 	 	 	 				value - value to be added to the stack

 Returned:   	NO_ERROR if no errors occur
 	 	 	 				ERROR_NO_STACK_MEMORY if psBUStack is NULL
							ERROR_STACK_FULL if the stack is full and the value cannot
															 be added
 ****************************************************************************/
static BU_ERROR buStkPush (BUExprStackPtr psBUStack, BUExprStackElement sElem)
{
	BU_ERROR error = NO_ERROR;
	bool bIsFull;

	if (NULL == psBUStack)
	{
		error = BU_ERROR_NO_STACK_MEMORY;
	}
	else
	{
		error = buStkIsFull(psBUStack,&bIsFull);

		if (NO_ERROR == error)
		{
			if (!bIsFull)
			{
				psBUStack->Tokens[psBUStack->top] = sElem;
				++psBUStack->size;
				++psBUStack->top;
			}
			else
			{
				error = BU_ERROR_STACK_FULL;
			}
		}
	}

	return error;
}

/****************************************************************************
 Function:  	buStkPop

 Description:	Requires that the stack be not empty and that psBUStack is not
 	 	 	 				NULL. If the stack is not empty, decrement the top of the
 	 	 	 				stack, assign pValue the most recently added element, and
 	 	 	 				decrement the size. If it is empty, return the error.

 Parameters:	psBUStack - the pointer to the stack
 	 	 	 				pValue - pointer to the value to be popped from the stack

 Returned:   	NO_ERROR if no errors occur
 	 	 	 				ERROR_NO_STACK_MEMORY if psBUStack is NULL
							ERROR_STACK_EMPTY if the stack is full and the value cannot
								  							be added
*****************************************************************************/
static BU_ERROR buStkPop (BUExprStackPtr psBUStack, BUExprStackElement *psValue)
{
	BU_ERROR error = NO_ERROR;
	bool bIsEmpty;

	if (NULL == psBUStack)
	{
		error = BU_ERROR_NO_STACK_MEMORY;
	}
	else
	{
		error = buStkIsEmpty(psBUStack,&bIsEmpty);

		if (NO_ERROR == error)
		{
			if (!bIsEmpty)
			{
				--psBUStack->top;
				*psValue = psBUStack->Tokens[psBUStack->top];
				--psBUStack->size;
			}
			else
			{
				error = BU_ERROR_STACK_EMPTY;
			}
		}
	}

	return error;
}

/****************************************************************************
 Function:  	buStkPeek

 Description:	Requires that the stack be not empty that psBUStack is not
 	 	 	 				NULL. If the stack is not empty, assign the most recently added
 	 	 	 				element to pValue. If the stack is empty, return the error.

 Parameters:	psBUStack - the pointer to the stack
 	 	 	 				pValue - pointer to the value at the top of the stack

 Returned:   	NO_ERROR if no errors occur
 	 	 	  			ERROR_NO_STACK_MEMORY - if psBUStack is NULL
							ERROR_STACK_EMPTY if the stack is full and the value cannot
								  							be added
*****************************************************************************/
static BU_ERROR buStkPeek (const BUExprStackPtr psBUStack,
																							BUExprStackElement *psToken)
{
	BU_ERROR error = BU_NO_ERROR;
	bool bIsEmpty;

	if (NULL == psBUStack)
	{
		error = BU_ERROR_NO_STACK_MEMORY;
	}
	else
	{
		error = buStkIsEmpty(psBUStack,&bIsEmpty);
		if (NO_ERROR == error)
		{
			if (!bIsEmpty)
			{
				*psToken = psBUStack->Tokens[(psBUStack->top)-1];
			}
			else
			{
				error = BU_ERROR_STACK_EMPTY;
			}
		}
	}

	return error;
}

/****************************************************************************
 Function:  	buStkPeekTerminal

 Description:	Requires that the stack be not empty that psBUStack is not
 	 	 	 				NULL. If the stack is not empty, assign the most recently added
 	 	 	 				element to pValue. If the stack is empty, return the error.

 Parameters:	psBUStack - the pointer to the stack
 	 	 	 				pValue - pointer to the value at the top of the stack

 Returned:   	NO_ERROR if no errors occur
 	 	 	  			ERROR_NO_STACK_MEMORY - if psBUStack is NULL
							ERROR_STACK_EMPTY if the stack is full and the value cannot
								  							be added
*****************************************************************************/
static BU_ERROR buStkPeekTerminal (const BUExprStackPtr psBUStack,
																								BUExprStackElement *psToken)
{
	BU_ERROR error = BU_NO_ERROR;
	bool bIsEmpty, bFoundTerminal = false;
	int i;

	if (NULL == psBUStack)
	{
		error = BU_ERROR_NO_STACK_MEMORY;
	}
	else
	{
		error = buStkIsEmpty (psBUStack,&bIsEmpty);
		if (BU_NO_ERROR == error)
		{
			if (!bIsEmpty)
			{
				for (i = (psBUStack->top)-1; i >= 0 && !bFoundTerminal; --i )
				{
					if (psBUStack->Tokens[i].Token.TokenClass != Dummy)
					{
						bFoundTerminal = true;
						*psToken = psBUStack->Tokens[i];
					}
				}
			}
			else
			{
				error = BU_ERROR_STACK_EMPTY;
			}
		}
	}

	return error;

}

/****************************************************************************
 Function:  	buStkSize

 Description:	Adjusts *pSize to the size of the stack

 Parameters:	psBUStack - the pointer to the stack
 	 	 	 				pSize - pointer to an integer of the size of the stack

 Returned:   	NO_ERROR if no errors occur

 ****************************************************************************/
static BU_ERROR buStkSize (const BUExprStackPtr psBUStack, int *pSize)
{
	BU_ERROR error = NO_ERROR;

	if (NULL == psBUStack)
	{
		error = BU_ERROR_NO_STACK_MEMORY;
	}
	else
	{
		*pSize = psBUStack->size;
	}

	return error;
}

/****************************************************************************
 Function:  	buStkReset

 Description:	Resets the BUToken stack

 Parameters:	psBUStack - the pointer to the stack

 Returned:   	error - a BU_ERROR

 ****************************************************************************/
static BU_ERROR buStkReset (BUExprStackPtr psBUStack)
{
	BUExprStackElement sFirstElem;
	lexCreateToken (0, Dollar, 0, "", &(sFirstElem.Token));

	BU_ERROR error = NO_ERROR;

	if (NULL == psBUStack)
	{
		error = BU_ERROR_NO_STACK_TERMINATE;
	}
	else
	{
		psBUStack->size = 0;
		psBUStack->top = 0;

		buStkPush (psBUStack, sFirstElem);
	}

	return error;
}
