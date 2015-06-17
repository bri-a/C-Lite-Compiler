/*****************************************************************************
 File name:     ct.h
 Author:				Brianna Alcoran
 Date:          2/20/15
 Class:         CS480
 Assignment:    Compiler Project written in C
 Purpose:       Constant Table for the Compiler
 Modifications:
******************************************************************************/

#include "er.h"
#include "pcc13defines.h"
#include "list.h"

#ifndef CONSTANT_TABLE_H
#define CONSTANT_TABLE_H

#define MAX_CONSTANTS 5000
#define CONSTANTTABLE_SIZE 1109

typedef enum CT_ERROR {	CT_NO_ERROR = 0,
												ERROR_NO_CONSTANTTABLE_CREATE,
												CT_ERROR_FULL_CONSTANTTABLE,
												CT_ERROR_INVALID_CONSTANTTABLE,
												CT_ERROR_NULL_PTR,
												CT_ERROR_HASH_FAILED,
												CT_ERROR_NO_DATA,
												CT_ERROR_DUPLICATE,
												CT_ERROR_FULL} CT_ERROR;


typedef struct ConstantTable
{
	unsigned int tableSize;
	unsigned int numEntries;
	List* psTable;
	int (*userHashFunction) (char * , unsigned int, unsigned int*, unsigned int);
	int (*userCompareConstants) (const void*, const void*);
}ConstantTable;

typedef ConstantTable * ConstantTablePtr;

/*******************************************************************************
*										Allocation & Deallocation
*******************************************************************************/
CT_ERROR ctCreate (ConstantTablePtr *, unsigned int,
		int (*userHashFunction) (char * , unsigned int, unsigned int *, unsigned int),
		int (*userCompareConstants) (const void*, const void*));
CT_ERROR ctDispose (ConstantTablePtr *);


/*******************************************************************************
*										Searching & Adding
*******************************************************************************/
CT_ERROR ctSearch (ConstantTablePtr, int, bool*, CT_DATATYPE *);
CT_ERROR ctAddConstant (ConstantTablePtr, int, int);


/*******************************************************************************
*										Printing
*******************************************************************************/
void ctPrint (ConstantTablePtr);
void ctPrintEntry (ConstantTablePtr, CT_DATATYPE sEntry);
void ctPrintHeader ();


/*******************************************************************************
*										Hash & Compare Functions
*******************************************************************************/
int userHashFunctionConstantTable (int num, unsigned int size,
		 	 	 	 	 	 	 	 	 	 	 	 	 	 	 unsigned int *pResult, unsigned int tableSize);
int userCompareCtData (const void *, const void *);


#endif /* CONSTANT_TABLE_H */
