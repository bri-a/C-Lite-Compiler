/*****************************************************************************
 File name:     st.h
 Date:          2/1/15
 Class:         CS480
 Assignment:    Compiler Project written in C
 Purpose:       Symbol Table for the Compiler
 Modifications: moved hash and compare functions to here from the driver
******************************************************************************/
#include "er.h"
#include "list.h"

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#define MAX_ID_LENGTH 32
#define SYMBOLTABLE_SIZE 1109

typedef enum ST_ERROR {ST_NO_ERROR = 0,
												ERROR_NO_SYMBOLTABLE_CREATE,
												ST_ERROR_FULL_SYMBOLTABLE,
												ST_ERROR_INVALID_SYMBOLTABLE,
												ST_ERROR_NULL_PTR,
												ST_ERROR_HASH_FAILED,
												ST_ERROR_NO_DATA,
												ST_ERROR_DUPLICATE,
												ST_ERROR_INVALID_KEY,
												ST_ERROR_MAX_LEVEL,
												ST_ERROR_INVALID_LEVEL,
												ST_ERROR_INVALID_ID } ST_ERROR;


typedef struct SymbolTable
{
	unsigned int tableSize;
	unsigned int numEntries;
	SymbolTableLevel currentLevel;
	List *psTable;
	int (*userHashFunction) (char*, unsigned int, unsigned int*, unsigned int);
	int (*userCompare) (char*, unsigned int, char*, unsigned int);
	int (*userCompareStData) (const void*, const void*);
}SymbolTable;


typedef SymbolTable *SymbolTablePtr;

/*******************************************************************************
*										Allocation and Deallocation
*******************************************************************************/
ST_ERROR stCreate (SymbolTablePtr* hsSymbolTable, unsigned int size,
		int (*userHashFunction) 	(char *, unsigned int, unsigned int *, unsigned int),
		int (*userCompare) 				(char*, unsigned int, char*, unsigned int),
		int (*userCompareStData) 	(const void*, const void*));
ST_ERROR stDispose (SymbolTablePtr* hsSymbolTable);

/*******************************************************************************
*										Searching and Printing
*******************************************************************************/
void stPrintHeader ();
ST_ERROR stPrint(SymbolTablePtr psSymbolTable);
ST_ERROR stPrintSorted (SymbolTablePtr psSymbolTable);
ST_ERROR stPrintEntry (SymbolTablePtr psSymbolTable, ST_DATATYPE sEntry);
ST_ERROR stSearch (SymbolTablePtr psSymbolTable, char *pIdentifer,
										ST_DATATYPE **psTheData);

/*******************************************************************************
*										Adding and Deleting Entries
*******************************************************************************/
ST_ERROR stAddEntry (SymbolTablePtr psSymbolTable, char *pIdentifier,
											SymbolTableType sType, int fieldMisc, int fieldOther);
ST_ERROR stAddName (SymbolTablePtr psSymbolTable, char *pIdentifier);
ST_ERROR stAddRsrvdWords (SymbolTablePtr psSymbolTable, char **pzWords,
														int numWords);
ST_ERROR stDeleteCurrentLevel (SymbolTablePtr psSymbolTable);
ST_ERROR stIncrementLevel (SymbolTablePtr psSymbolTable);
ST_ERROR stDecrementLevel (SymbolTablePtr psSymbolTable);

/*******************************************************************************
*										Other
*******************************************************************************/
ST_ERROR stIsEmpty (SymbolTable sSymbolTable, bool *pbResult);
ST_ERROR stVisit (SymbolTablePtr psSymbolTable,
				void(*visitor) (char* pData, unsigned int keyLength, ST_DATATYPE data));
bool isValidID (char *pID);
SymbolTableLevel stGetLvl (SymbolTablePtr);

/*******************************************************************************
*										Hash & Compare Functions
*******************************************************************************/
int userHashFunctionMidSquare (char *, unsigned int, unsigned int *,
																																unsigned int);
int userHashFunctionDJB2 (char *, unsigned int, unsigned int *,
																																unsigned int);
int userCompare (char *, unsigned int , char *, unsigned int );
int userCompareStData (const void *, const void *);

#endif /* SYMBOLTABLE_H */
