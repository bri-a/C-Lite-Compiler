/*****************************************************************************
 File name:     rts.h
 Author:				Brianna Alcoran
 Date:          2/20/15
 Class:         CS480
 Assignment:    Compiler Project written in C
 Purpose:       Runtime Stack for the Compiler
 Modifications:
******************************************************************************/

#include "../include/pcc13defines.h"
#include "../include/list.h"

#ifndef RUNTIME_STACK_H
#define RUNTIME_STACK_H

#define MAX_STACK 5000


typedef enum RTS_ERROR {	RTS_NO_ERROR = 0,
													ERROR_NO_RTS_CREATE = 1,
													RTS_ERROR_NULL_PTR,
													RTS_ERROR_FULL} RTS_ERROR;


typedef struct RuntimeStack
{
	int Stack[MAX_STACK];
	int availablePos;
}RuntimeStack;


typedef RuntimeStack * RuntimeStackPtr;

/*******************************************************************************
*										Allocation & Deallocation
*******************************************************************************/
RTS_ERROR rtsCreate (RuntimeStackPtr *);
RTS_ERROR rtsDispose (RuntimeStackPtr *);

/*******************************************************************************
*										Adding
*******************************************************************************/
RTS_ERROR rtsAdd (RuntimeStackPtr, int);

/*******************************************************************************
*										Printing
*******************************************************************************/
void rtsPrint (RuntimeStackPtr );
void rtsPrintHeader ();
void rtsDisplay (RuntimeStackPtr);
void rtsDisplayToFile (RuntimeStackPtr, FILE *);

/*******************************************************************************
*										Getters
*******************************************************************************/
int rtsGetAvailablePos (RuntimeStackPtr );

#endif /* CONSTANT_TABLE_H */
