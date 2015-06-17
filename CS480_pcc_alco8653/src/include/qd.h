/*****************************************************************************
 File name:     quad.h
 Author:				Brianna Alcoran
 Date:          3/20/15
 Class:         CS480
 Assignment:    Compiler Project written in C
 Purpose:       Quads for the Compiler -
 	 	 	 	 	 	 	 	 will be maintained by the parser
 Modifications:
******************************************************************************/

#include "st.h"
#include "rts.h"

#ifndef QUAD_H
#define QUAD_H

#define MAX_QUAD_FILE 1500

#define AM1 1
#define AM2 2
#define AM3 3

#define OP1 1
#define OP2 2
#define OP3 3


typedef enum QD_ERROR {QD_NO_ERROR = 0,
	QD_ERROR_NO_CREATE,
	QD_ERROR_NO_TERMINATE,
	QD_ERROR_NULL_PTR,
	QD_ERROR_FULL,
	QD_ERROR_EMPTY,
	QD_ERROR_FILE_OPEN
} QD_ERROR;

typedef struct Quad
{
	SymbolTableLevel level;

	QuadOpcode opcode;

	QuadAddressingMode am1;
	int opd1;										//operand 1

	QuadAddressingMode am2;
	int opd2; 									//operand 2

	QuadAddressingMode am3;
	int opd3; 									//operand 3

	int QuadFilePos;
}Quad;

typedef Quad * QuadPtr;


typedef struct QuadFile
{
	Quad quads[MAX_QUAD_FILE];	//max of 1500 quads
	int size;
	ErrorHandlerPtr psER;
	RuntimeStackPtr psRTS;
	FILE * pFile;							//pointer to the file (the quad file .q by default)
	bool bPrintToScreen;
}QuadFile;

typedef QuadFile * QuadFilePtr;


/*******************************************************************************
*										Allocation & Deallocation
*******************************************************************************/
QD_ERROR quadCreate (QuadFilePtr*, ErrorHandlerPtr, RuntimeStackPtr, char*);
QD_ERROR quadDispose (QuadFilePtr*);
QD_ERROR quadClear (QuadFilePtr);

QD_ERROR quadAdd (QuadFilePtr, Quad);
QD_ERROR quadPop (QuadFilePtr, QuadPtr);
QD_ERROR quadMerge (QuadFilePtr, QuadFilePtr);
QD_ERROR setQuadPrintToScreen (QuadFilePtr, bool);
QD_ERROR quadIncrementLabels (QuadFilePtr, int);

/*******************************************************************************
*										Checking
*******************************************************************************/
QD_ERROR quadIsFull (QuadFilePtr, bool *);
QD_ERROR quadIsEmpty (QuadFilePtr, bool *);


/*******************************************************************************
*										Getters & Setters
*******************************************************************************/

void setQuad (QuadPtr, SymbolTableLevel, QuadOpcode, QuadAddressingMode,
							int, QuadAddressingMode, int, QuadAddressingMode, int);

QD_ERROR getQuad (QuadFilePtr, int, QuadPtr *);
QD_ERROR getLastQuad (QuadFilePtr, QuadPtr *);
QD_ERROR getLastFuncQuad (QuadFilePtr, QuadPtr *);
QD_ERROR getLastOPQuad (QuadFilePtr, QuadOpcode, QuadPtr * );
QD_ERROR getQuadFileSize (QuadFilePtr, int *);

SymbolTableLevel quadGetSTlevel (QuadPtr);
QuadOpcode quadGetOpcode (QuadPtr);

void setQuadSTLevel (QuadPtr *, SymbolTableLevel);
void setQuadOpcode (QuadPtr *, QuadOpcode);
void setQuadAM (QuadPtr *, int, QuadAddressingMode);
void setQuadOPD (QuadPtr *, int, int);

/*******************************************************************************
*										Printing
*******************************************************************************/
void printQuadToScreen (Quad sQ);
void printQuad (Quad sQ, FILE *);
void printQuadFile (QuadFilePtr);


#endif
