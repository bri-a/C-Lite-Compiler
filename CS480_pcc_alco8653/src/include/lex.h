/*****************************************************************************
 File name:     lex.h
 Author:				Brianna Alcoran
 Date:          2/13/15
 Class:         CS480
 Assignment:    Compiler Project written in C
 Purpose:       Lexical Analyzer for the Compiler
 Modifications:
******************************************************************************/

#include "er.h"
#include "list.h"
#include "st.h"
#include "ct.h"
#include "rts.h"

#ifndef LEXER_H
#define LEXER_H

#define BUFFER_SIZE 1024

typedef enum lexState { START = 0,
	CONSTANT = 1,
	EQUAL,	NOT,
	LESS_THAN,	GREATER_THAN,
	MULTIPLY,	DIVIDE,	MOD,
	ADD,	SUBTRACT,
	AMPERSAND,
	PIPE,
	SEMICOLON,
	COMMA,
	PAREN_L,	PAREN_R,
	BRACK_L,	BRACK_R,
	BRACE_L,	BRACE_R,
	BAD_CHARACTER,
	ID }lexState;


typedef enum LEX_ERROR {LEX_NO_ERROR = 0,
													LEX_ERROR_NO_CREATE,
													LEX_ERROR_INVALID_LEXER,
													LEX_ERROR_NULL_PTR,
													LEX_NO_MORE_TOKENS} LEX_ERROR;

typedef enum whichRelop {REL_EQUAL_EQUAL = 1,
														REL_NOT_EQUAL = 2,
														REL_LESS_EQUAL,
														REL_GREATER_EQUAL,
														REL_LESS_THAN,
														REL_GREATER_THAN} whichRelop;

typedef enum whichAddop 	{ADD_PLUS = 1, ADD_SUB = 2}	whichAddop;
typedef enum whichMulop 	{MUL_TIMES = 1, MUL_DIV = 2, MUL_MOD = 3} whichMulop;
typedef enum whichAutoop 	{AUTO_INCR = 1, AUTO_DECR = 2} whichAutoop;

typedef struct Lexer
{
	char buffer[BUFFER_SIZE];
	int bufferIndex;
	int currentLine;
	bool debugOn;

	LEX_DATATYPE sPrevToken;

	SymbolTablePtr psST;
	ErrorHandlerPtr psER;
	ConstantTablePtr psCT;
	RuntimeStackPtr psRTS;

	FILE * pFile;
	int currentFileLine;
}Lexer;

typedef Lexer * LexerPtr;

/*******************************************************************************
*										Allocation & Deallocation
*******************************************************************************/
LEX_ERROR lexCreate (LexerPtr *, SymbolTablePtr psST, ErrorHandlerPtr psER,
										ConstantTablePtr psCT, RuntimeStackPtr psRTS, FILE *);
LEX_ERROR lexDispose (LexerPtr *);


/*******************************************************************************
*										GetToken & Creating a Token
*******************************************************************************/
LEX_ERROR lexGetToken (LexerPtr, LEX_DATATYPE *);

void lexCreateToken(int, TokenClass, int, char *, LEX_DATATYPE *);


/*******************************************************************************
*										Buffer Analysis
*******************************************************************************/
bool isWhiteSpace (char );
bool isBadChar (char );
ErrorCode eatComment (LexerPtr, ErrorHandlerPtr, FILE *, int * lineNum);


/*******************************************************************************
*										Printing
*******************************************************************************/
void printLine (LexerPtr);
void printToken (LEX_DATATYPE);


/*******************************************************************************
*										Getters & Setters
*******************************************************************************/
int lexGetLineNum (LexerPtr);
void setDebug (LexerPtr, bool);


#endif /* LEXER_H */
