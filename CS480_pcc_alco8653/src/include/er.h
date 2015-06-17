/*****************************************************************************
 File name:     er.h
 Date:          2/1/15
 Class:         CS480
 Assignment:    Compiler Project written in C
 Purpose:       Error Handler for the Compiler
 Modifications: 2/22/15 - added erPrintErrorPerLine
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "pcc13defines.h"
#include "list.h"

#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

typedef struct ErrorHandler
{
	List sErrors; /* to store errors as they are generated */
	int numErrors;
	bool bPrintDragon;
}ErrorHandler;

typedef ErrorHandler *ErrorHandlerPtr;

/*******************************************************************************
*										Allocation and Deallocation
*******************************************************************************/
void erCreate (ErrorHandlerPtr *hsER);
void erDispose (ErrorHandlerPtr *hsER);


/*******************************************************************************
*										Adding and Deleting
*******************************************************************************/
void erAddError (ErrorHandlerPtr psER, int lineNum, ErrorCode error, char *pOptional);
void erClear (ErrorHandlerPtr psER);

int erGetNumErrors (ErrorHandlerPtr psER);
void erSetPrintDragon (ErrorHandlerPtr psER, bool);


/*******************************************************************************
*										Printing
*******************************************************************************/
void erPrintCurrentError (ErrorHandlerPtr psER);
void erPrintErrors (ErrorHandlerPtr psER);
void erPrintDragon ();
void erPrintErrorPerLine (ErrorHandlerPtr psER);


#endif /* ERRORHANDLER_H */
