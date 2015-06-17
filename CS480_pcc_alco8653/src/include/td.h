/*****************************************************************************
 File name:     td.h
 Author:				Brianna Alcoran
 Date:          2/24/15
 Class:         CS480
 Assignment:    Compiler Project written in C
 Purpose:       Top Down Parser for the Compiler
 Modifications:
******************************************************************************/

#include "lex.h"
#include "qd.h"
#include "bu.h"

#ifndef TOPDOWN_H
#define TOPDOWN_H

typedef enum TD_ERROR {TD_NO_ERROR = 0,
	TD_ERROR_NO_CREATE,
	TD_ERROR_INVALID_LEXER,
	TD_ERROR_NULL_PTR,
	TD_ERROR_NO_MORE_TOKENS,
	TD_ERROR_NO_MATCH
} TD_ERROR;

typedef struct TopDownParser
{
	LEX_DATATYPE currentToken;
	LexerPtr psLex;
	ErrorHandlerPtr psER;
	SymbolTablePtr psST;
	RuntimeStackPtr psRTS;
	QuadFilePtr psQD;
	BUParserPtr psBU;
	bool bDebugEnter, bDebugLeave, bDebugST;
}TopDownParser;

typedef TopDownParser * TopDownParserPtr;

/*******************************************************************************
*										Allocation & Deallocation
*******************************************************************************/
TD_ERROR tdCreate (TopDownParserPtr *, LexerPtr, ErrorHandlerPtr, SymbolTablePtr,
										RuntimeStackPtr, QuadFilePtr, BUParserPtr);
TD_ERROR tdDispose (TopDownParserPtr *);

/*******************************************************************************
*										Debug Setters
*******************************************************************************/
TD_ERROR tdSetDebugEnter (TopDownParserPtr, bool);
TD_ERROR tdSetDebugLeave (TopDownParserPtr, bool);
TD_ERROR tdSetDebugST (TopDownParserPtr, bool);

/*******************************************************************************
*										Printing
*******************************************************************************/
void printEnter (const char*, bool);
void printLeaving (const char*, bool);

/*******************************************************************************
*										Parse
*******************************************************************************/
TD_ERROR match (TopDownParserPtr, TokenClass, int value);
void parse (TopDownParserPtr);


#endif /* TOPDOWN_H */
