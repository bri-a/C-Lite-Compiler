/*****************************************************************************
 File name:     bu.h
 Author:				Brianna Alcoran
 Date:          3/8/15
 Class:         CS480
 Assignment:    Compiler Project written in C
 Purpose:       Bottom Up Parser for the Compiler
 Modifications: Created a new expression stack element (previously qas tokens)
******************************************************************************/

#include "lex.h"
#include "qd.h"

#ifndef BOTTOMUP_H
#define BOTTOMUP_H

#define MAX_TOKEN_STACK 2048
#define MAX_HANDLE 4

typedef enum BU_ERROR {BU_NO_ERROR = 0,
	BU_ERROR_NO_CREATE,
	BU_ERROR_INVALID_LEXER,
	BU_ERROR_NULL_PTR,
	BU_ERROR_NO_STACK_CREATE,
	BU_ERROR_NO_STACK_TERMINATE,
	BU_ERROR_NO_STACK_MEMORY,
	BU_ERROR_STACK_FULL,
	BU_ERROR_STACK_EMPTY,
	BU_ERROR_END_OF_EXPRESSION,
	BU_ERROR_BAD_EXPRESSION,
	BU_ERROR_NO_MORE_EXPRESSIONS,
	BU_ERROR_INVALID_HANDLE
} BU_ERROR;


typedef struct BUExprStackElement
{
	LEX_DATATYPE Token;
	SymbolTableLevel sLevel;
	SymbolTableType sType;
	int address;
	bool bCanBeLValue;
	bool bIsPointer;
	bool bIndirect;
	bool bSawDeref;
} BUExprStackElement;

typedef struct BUExprStack
{
	BUExprStackElement Tokens [MAX_TOKEN_STACK];
	int top,
			size;
}BUExprStack;

typedef struct BUExprStack *BUExprStackPtr;

typedef struct BUParser
{
	BUExprStack BUStack;

	LexerPtr psLex;
	ErrorHandlerPtr psER;
	QuadFilePtr psQD;
	SymbolTablePtr psST;
	RuntimeStackPtr psRTS;
	QuadFile psQD2Extra; //going to be used for autoops

	bool bDebugOn, bFuncParams, bInOutput, bInReturn, bRHS;
	bool bReducingFunction;
	bool bHasNoParams;

	int numParams;
	int funcStackLoc;

}BUParser;

typedef BUParser * BUParserPtr;

/*******************************************************************************
*										Allocation & Deallocation
*******************************************************************************/
BU_ERROR buCreate (BUParserPtr *, LexerPtr, ErrorHandlerPtr, QuadFilePtr,
											SymbolTablePtr,	RuntimeStackPtr);
BU_ERROR buDispose (BUParserPtr *);


/*******************************************************************************
*										Setters & Getters
*******************************************************************************/
void buSetDebug (BUParserPtr, bool);


/*******************************************************************************
*										Parse!
*******************************************************************************/
BU_ERROR buParse (BUParserPtr, LEX_DATATYPE *, LEX_DATATYPE *, int *,
										BUExprStackElement *);

void initializeStackElem (BUExprStackElement *);

BU_ERROR calculateAM (QuadAddressingMode *, BUExprStackElement);
BU_ERROR addTemp (BUParserPtr psBU, int * localDataSize, int * tempLocation);
BU_ERROR derefOP (BUParserPtr psBU, BUExprStackElement *psOP, int * localDataSize);


#endif /* BOTTOMUP_H */
