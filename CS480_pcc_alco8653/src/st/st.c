/*******************************************************************************
 File name:     st.c
 Author:				Brianna Alcoran
 Date:          2/1/15
 Class:         CS480
 Assignment:    Compiler Project written in C
 Purpose:       Symbol Table for the Compiler (implementation)
 Modifications: 2/13/15 -
 	 	 	 	 	 	 	 	Edited stAddName() and added a line of code to fix
 	 	 	 	 	 	 	 	stDeleteCurrentLevel()
*******************************************************************************/

#include <stdbool.h>
#include <string.h>
#include "../include/st.h"

/*******************************************************************************
 Function:		stCreate

 Description:	create a Symbol Table

 Parameters:	hsSymbolTable 		- handle to the Symbol Table
							size 							- size of the Symbol Table
							userHashFunction 	- function pointer to the hashing function
							userCompare 			- function pointer used to compare two keys
							userCompareStData - function pointer to compare ST_DATATYPES

 Returned:		error - an ST_ERROR
*******************************************************************************/
ST_ERROR stCreate (SymbolTablePtr* hsSymbolTable, unsigned int size,
		int (*userHashFunction) (char *, unsigned int, unsigned int *, unsigned int),
				int (*userCompare) (char*, unsigned int, char*, unsigned int),
				int (*userCompareStData) (const void*, const void* ))
{
	ST_ERROR error = ST_NO_ERROR;

	*hsSymbolTable = (SymbolTablePtr) malloc (sizeof (SymbolTable));

	if (NULL == *hsSymbolTable)
	{
		error = ERROR_NO_SYMBOLTABLE_CREATE;
	}
	else if (NULL == userHashFunction || NULL == userCompare ||
						NULL == userCompareStData)
	{
		error = ST_ERROR_NULL_PTR;
	}
	else
	{
		(*hsSymbolTable)->psTable = (ListPtr) malloc (sizeof (List)*size);

		if (NULL == (*hsSymbolTable)->psTable)
		{
			error = ERROR_NO_SYMBOLTABLE_CREATE;
		}
		else
		{
			(*hsSymbolTable)->currentLevel = RSRVWRD_LEVEL; //initialize the level

			int i;
			for (i = 0; i < size; ++i)
			{
				lstCreate (&(*hsSymbolTable)->psTable[i]); //array of list*
			}

			(*hsSymbolTable)->tableSize = size;
			(*hsSymbolTable)->numEntries = 0;
			(*hsSymbolTable)->userHashFunction = userHashFunction;
			(*hsSymbolTable)->userCompare = userCompare;
			(*hsSymbolTable)->userCompareStData = userCompareStData;
		}
	}
	return error;
}

/*******************************************************************************
 Function:		stDispose

 Description:	dispose of a Symbol Table

 Parameters:	hsSymbolTable	- handle to the Symbol Table

 Returned:		error - an ST_ERROR
*******************************************************************************/
ST_ERROR stDispose (SymbolTablePtr *hsSymbolTable)
{
	ST_ERROR error = ST_NO_ERROR;

	if (NULL == *hsSymbolTable)
	{
		error = ST_ERROR_INVALID_SYMBOLTABLE;
	}
	else
	{
		int i;
		for (i = 0; i < (*hsSymbolTable)->tableSize; ++i )
		{
			lstDispose(&(*hsSymbolTable)->psTable[i]);
		}

		free ((*hsSymbolTable)->psTable);
		free (*hsSymbolTable);
		*hsSymbolTable = NULL;
	}
	return error;
}

/*******************************************************************************
 Function:		stPrintHeader

 Description:	prints heading of the Symbol Table (warm fuzzy)

 Parameters:	none

 Returned:		none
*******************************************************************************/
void stPrintHeader ()
{
	int numDashes = 79;
	printf ("%52s", "S Y M B O L   T A B L E\n");
	printf("%33s", "Identifier");
	printf("%7s", "Level");
	printf("%11s", "Type");
	printf("%12s", "Address");
	printf("%16s", "Other");
	printf("\n");
	for(;numDashes--;printf("-"));
	printf("\n");
}

/*******************************************************************************
 Function:		stPrint

 Description:	Starts at the beginning of the Symbol Table, printing each entry

 Parameters:	psSymbolTable - pointer to the Symbol Table

 Returned:		error - an ST_ERROR
*******************************************************************************/
ST_ERROR stPrint (SymbolTablePtr psSymbolTable)
{
	ST_ERROR error = ST_NO_ERROR;
	int i, j, sizeList;
	DATATYPE sTemp;

	if (NULL == psSymbolTable)
	{
		error = ST_ERROR_INVALID_SYMBOLTABLE;
	}
	else
	{
		stPrintHeader();

		for (i = 0; i < psSymbolTable->tableSize; ++i)
		{
			lstFirst(&(psSymbolTable->psTable[i]), &sTemp);
			lstSize(&(psSymbolTable->psTable[i]), &sizeList);

			for (j = 0; j < sizeList; ++j)
			{
				lstNext(&(psSymbolTable->psTable[i]), &sTemp);
				stPrintEntry (psSymbolTable, sTemp.st_data);
			}
		}
	printf("\n");
	}
	return error;
}

/*******************************************************************************
 Function:		stPrintSorted

 Description:	organizes the entries in the Symbol Table before printing them

 Parameters:	psSymbolTable - pointer to the Symbol Table

 Returned:		error - an ST_ERROR
*******************************************************************************/
ST_ERROR stPrintSorted (SymbolTablePtr psSymbolTable)
{
	ST_ERROR error = ST_NO_ERROR;
	ST_DATATYPE sEntries[psSymbolTable->numEntries];
	DATATYPE sTemp;
	int i, j, count = 0, sizeList;

	/* put all entries into an array */
	for (i = 0; i < psSymbolTable->tableSize; ++i)
	{
		lstFirst(&(psSymbolTable->psTable[i]), &sTemp);
		lstSize(&(psSymbolTable->psTable[i]), &sizeList);

		for (j = 0; j < sizeList; ++j)
		{
			lstNext(&(psSymbolTable->psTable[i]), &sTemp);
			sEntries[count] = sTemp.st_data;
			++count;
		}
	}

	/* sort array using SymbolTable's userCompareStData function */
	qsort(sEntries, psSymbolTable->numEntries, sizeof(ST_DATATYPE),
					psSymbolTable->userCompareStData);

	/* print the sorted entries */
	stPrintHeader();
	for (i = 0; i < psSymbolTable->numEntries; ++i)
	{
		stPrintEntry (psSymbolTable, sEntries[i]);
	}
	printf("\n");
	return error;
}
/*******************************************************************************
 Function:		stPrintEntry

 Description:	prints a particular entry in the Symbol Table underneath the
							table header

 Parameters:	psSymbolTable - pointer to the Symbol Table
 	 	 	 	 	 	 	sEntry				- the data to be printed

 Returned:		error - an ST_ERROR
*******************************************************************************/

ST_ERROR stPrintEntry (SymbolTablePtr psSymbolTable, ST_DATATYPE sEntry)
{
	ST_ERROR error = ST_NO_ERROR;

	if (NULL == psSymbolTable)
	{
		error = ST_ERROR_INVALID_SYMBOLTABLE;
	}
	else
	{
		printf("%33.*s", sEntry.idSize, sEntry.pszIdentifier);
		printf("%7d", sEntry.sLevel);
		printf("%11d", sEntry.sType);
		printf("%12d", sEntry.address);
		printf("%16d", sEntry.other);
		printf("\n");
	}
	return error;
}
/*******************************************************************************
 Function:		stSearch (NEEDS TO BE FIXED)

 Description:	searches for a given name in the Symbol Table. if found, a
  						pointer to the entry is returned through the argument list.
  						otherwise, NULL is returned

 Parameters:	psSymbolTable - pointer to the Symbol Table
 	 	 	 	 	 	 	pIdentifier 	- the name to be searched for
 	 	 	 	 	 	 	psEntry				- pointer to the Symbol Table entry

 Returned:		error - an ST_ERROR
*******************************************************************************/
ST_ERROR stSearch (SymbolTablePtr psSymbolTable, char *pID,
									 ST_DATATYPE **psEntry)
{
	ST_ERROR error = ST_NO_ERROR;
	//DATATYPE sTemp;
	ST_DATATYPE * psTest = NULL;
	bool bIsEmpty, bIsListEmpty, bFoundEntry = false;

	unsigned int bucket;
	int i, functionSuccess, size, listSize;

	if (NULL == psSymbolTable)
	{
		error = ST_ERROR_INVALID_SYMBOLTABLE;
	}
	else if (NULL == pID)
	{
		error = ST_ERROR_NULL_PTR;
	}
	else if (!isValidID(pID))
	{
		error = ST_ERROR_INVALID_ID;
	}
	else
	{
		//(*psEntry).bInitialized = false;
		error = ST_ERROR_NO_DATA;
		stIsEmpty(*psSymbolTable, &bIsEmpty);

		if (!bIsEmpty)
		{
			size = strlen(pID);
			/* get a table index using hash function and name we are searching for */
			functionSuccess = psSymbolTable->userHashFunction
												(pID, size, &bucket, psSymbolTable->tableSize);

			if (functionSuccess)
			{	/* if the list at this index is empty, then the name we are
			 	 	 	searching is not in the table */
				lstIsEmpty (&(psSymbolTable->psTable[bucket]), &bIsListEmpty);
				if (!bIsListEmpty)
				{
					//lstFirst (&(psSymbolTable->psTable[bucket]), &sTemp);
					lstFirstST (&(psSymbolTable->psTable[bucket]), &psTest);
					lstSize (&(psSymbolTable->psTable[bucket]), &listSize);

					/*iterate through the elements in the bucket checking for a duplicate */
					for (i = 0; i < listSize; ++i)
					{
						//lstNext(&(psSymbolTable->psTable[bucket]), &sTemp);

						lstNextST (&(psSymbolTable->psTable[bucket]), &psTest);
						while (0 == strncmp(pID, psTest->pszIdentifier, size) &&
										i < listSize)
						{
							if (bFoundEntry)
							{
								if ((*psEntry)->sLevel < psTest->sLevel)
								{
									*psEntry = psTest;
								}
							}
							else
							{
								*psEntry = psTest;
								bFoundEntry = true;
								error = ST_NO_ERROR;
							}
							//lstNext(&(psSymbolTable->psTable[bucket]), &sTemp);
							lstNextST (&(psSymbolTable->psTable[bucket]), &psTest);
							++i;
						}
					}
				}
			}
		}
	}

	return error;
}

/*******************************************************************************
*										Adding and Deleting Entries
*******************************************************************************/
/*******************************************************************************
 Function:		stAddEntry

 Description:	add an entry to the Symbol Table

 Parameters:	psSymbolTable - pointer to the SymbolTable
 	 	 	 	 	 	 	pIdentifer 		- identifier name of the entry
 	 	 	 	 	 	 	sType					- SymbolTableType of the entry
 	 	 	 	 	 	 	fieldMisc			- misc field of the entry
 	 	 	 	 	 	 	fieldOther		- extra field of the entry

 Returned:		error - an ST_ERROR
*******************************************************************************/
ST_ERROR stAddEntry (SymbolTablePtr psSymbolTable, char *pID,
												SymbolTableType sType, int fieldMisc, int fieldOther)
{
	ST_ERROR error = ST_NO_ERROR;
	int functionSuccess;
	unsigned int bucket;
	bool pbResult, bHasCurrent;
	DATATYPE sTemp, sNewData;
	int sizeList, i;

	if (NULL == psSymbolTable)
	{
		error = ST_ERROR_INVALID_SYMBOLTABLE;
	}
	else if (NULL == pID)
	{
		error = ST_ERROR_NULL_PTR;
	}
	else if (!isValidID(pID))
	{
		error = ST_ERROR_INVALID_ID;
	}
	else
	{
		/* initializing the new entry */
		sNewData.st_data.idSize = strlen(pID);
		memset(sNewData.st_data.pszIdentifier, '\0', sizeof(sNewData.st_data.pszIdentifier));

		strncpy(sNewData.st_data.pszIdentifier, pID, sNewData.st_data.idSize);
		sNewData.st_data.sLevel = psSymbolTable->currentLevel;
		sNewData.st_data.sType = sType;
		sNewData.st_data.address = fieldMisc;
		sNewData.st_data.other = fieldOther;
		sNewData.st_data.bInitialized = true;
		strncpy(sNewData.st_key, pID, sNewData.st_data.idSize);
		sNewData.st_size = sNewData.st_data.idSize;

		functionSuccess =	psSymbolTable->userHashFunction
			(pID, sNewData.st_data.idSize, &bucket, psSymbolTable->tableSize);

		if (functionSuccess == true)
		{
			lstIsEmpty(&(psSymbolTable->psTable[bucket]), &pbResult);

			if (true == pbResult) /* bucket was empty */
			{
				lstInsertAfter(&(psSymbolTable->psTable[bucket]), sNewData);
				++(psSymbolTable->numEntries);
			}
			else			/* bucket was not empty */
			{
				lstFirst(&(psSymbolTable->psTable[bucket]), &sTemp);
				lstSize(&(psSymbolTable->psTable[bucket]), &sizeList);

				/* iterate through the elements in the bucket checking for a duplicate */
				for (i = 0; i < sizeList; ++i)
				{ /* check for same name */
					if (0 == psSymbolTable->userCompare(sTemp.st_data.pszIdentifier,
							sTemp.st_data.idSize, pID, sNewData.st_data.idSize))
					{ /* then check for same level */
						if (sTemp.st_data.sLevel == sNewData.st_data.sLevel)
						{
							/* found a duplicate */
							error = ST_ERROR_DUPLICATE;
							break;
						}
					}

					lstNext(&(psSymbolTable->psTable[bucket]), &sTemp);
					lstPeek(&(psSymbolTable->psTable[bucket]), &sTemp);
					lstHasCurrent(&(psSymbolTable->psTable[bucket]), &bHasCurrent);
					if (!bHasCurrent) /* place new entry as last element in the list */
					{
						lstLast(&(psSymbolTable->psTable[bucket]), &sTemp);
						lstInsertAfter(&(psSymbolTable->psTable[bucket]), sNewData);
						++(psSymbolTable->numEntries);
					}
				}
			}
		}
		else
		{
			error = ST_ERROR_HASH_FAILED;
		}
	}

	return error;
}

/*******************************************************************************
 Function:		stAddName

 Description:	add an entry to the Symbol Table

 Parameters:	psSymbolTable - pointer to the Symbol Table
 	 	 	 	 	 		pIdentifer		- identifier name of the entry

 Returned:		error - an ST_ERROR
*******************************************************************************/
ST_ERROR stAddName (SymbolTablePtr psSymbolTable, char *pIdentifier)
{
	ST_ERROR error = ST_NO_ERROR;

	if (NULL == psSymbolTable)
	{
		error = ST_ERROR_INVALID_SYMBOLTABLE;
	}
	else if (NULL == pIdentifier)
	{
		error = ST_ERROR_NULL_PTR;
	}
	else if (!isValidID (pIdentifier))
	{
		error = ST_ERROR_INVALID_ID;
	}
	else
	{
		error = stAddEntry(psSymbolTable, pIdentifier, -1,-1,-1);
	}

	return error;
}

/*******************************************************************************
 Function:		stDeleteCurrentLevel

 Description:	deletes all entries in the ST that are of the current	level

 Parameters:	psSymbolTable	- pointer to the Symbol Table

 Returned:		error - an ST_ERROR
*******************************************************************************/
ST_ERROR stDeleteCurrentLevel (SymbolTablePtr psSymbolTable)
{
	ST_ERROR error = ST_NO_ERROR;
	bool bIsEmpty;
	DATATYPE sTemp;
	int i, j, listSize;

	if (NULL == psSymbolTable)
	{
		error = ST_ERROR_INVALID_SYMBOLTABLE;
	}
	else if (RSRVWRD_LEVEL > psSymbolTable->currentLevel)
	{
		error = ST_ERROR_INVALID_LEVEL;
	}
	else
	{
		stIsEmpty(*psSymbolTable, &bIsEmpty);

		if (!bIsEmpty)
		{
			/* iterate through each entry in the ST */
			for (i = 0; i < psSymbolTable->tableSize; ++i)
			{
				lstLast(&(psSymbolTable->psTable[i]), &sTemp);
				lstSize(&(psSymbolTable->psTable[i]), &listSize);

				for (j = 0; j < listSize; ++j)
				{
					/* delete entry whose level matches the current level of the ST */
					if (psSymbolTable->currentLevel == sTemp.st_data.sLevel)
					{
						lstDeleteCurrent(&(psSymbolTable->psTable[i]), &sTemp);
						lstCurrent(&(psSymbolTable->psTable[i]), &sTemp); /* added this */
						--(psSymbolTable->numEntries);
					}
					else
					{
						lstPrev(&(psSymbolTable->psTable[i]), &sTemp);
					}
				}
			}
		}
	}

	return error;
}

/*******************************************************************************
 Function:		stIncrementLevel

 Description:	increments the current level of the Symbol Table
 	 	 	 	 	 	 	(cannot go beyond the local level)

 Parameters:	psSymbolTable	- pointer to the Symbol Table

 Returned:		error - an ST_ERROR
*******************************************************************************/
ST_ERROR stIncrementLevel (SymbolTablePtr psSymbolTable)
{
	ST_ERROR error = ST_NO_ERROR;

	if (NULL == psSymbolTable)
	{
		error = ST_ERROR_INVALID_SYMBOLTABLE;
	}
	else
	{
		if (LOCAL_LEVEL == psSymbolTable->currentLevel)
		{
			error = ST_ERROR_MAX_LEVEL;
		}
		else
		{
			++(psSymbolTable->currentLevel);
		}
	}

	return error;
}

/*******************************************************************************
 Function:		stDecrementLevel

 Description:	decrements the current level of the Symbol Table
 	 	 	 	 	 	 	(cannot go past -1)

 Parameters:	psSymbolTable	- pointer to the Symbol Table

 Returned:		error - an ST_ERROR
*******************************************************************************/
ST_ERROR stDecrementLevel (SymbolTablePtr psSymbolTable)
{
	ST_ERROR error = ST_NO_ERROR;

	if (NULL == psSymbolTable)
	{
		error = ST_ERROR_INVALID_SYMBOLTABLE;
	}
	else
	{
		if (RSRVWRD_LEVEL > psSymbolTable->currentLevel)
		{
			error = ST_ERROR_MAX_LEVEL;
		}
		else
		{
			--(psSymbolTable->currentLevel);
		}
	}

	return error;
}

/*******************************************************************************
 Function:		stIsEmpty

 Description:	returns true (through argument list) if the Symbol Table has no
 	 	 	 	 	 	 	entries; otherwise, false.

 Parameters:	sSymbolTable	- the Symbol Table
							pbResult			- boolean indicating if the Symbol Table is empty

 Returned:		error - an ST_ERROR
*******************************************************************************/
ST_ERROR stIsEmpty (SymbolTable sSymbolTable, bool *pbResult)
{
	ST_ERROR error = ST_NO_ERROR;
	*pbResult = true;

	int i;
	for (i = 0; i < sSymbolTable.tableSize && *pbResult; i++)
	{
		error = lstIsEmpty (&(sSymbolTable.psTable[i]), pbResult);
	}

	return error;
}

/*******************************************************************************
 Function:		stVisit

 Description: has the function visitor visit each entry in the Symbol Table

 Parameters:	psSymbolTable - pointer to the Symbol Table
							visitor				- function pointer that will visit each entry

 Returned:		error - an ST_ERROR
*******************************************************************************/
ST_ERROR stVisit (SymbolTablePtr psSymbolTable,
			 void(*visitor) (char* pData, unsigned int keyLength, ST_DATATYPE data))
{
	ST_ERROR error = ST_NO_ERROR;
	DATATYPE sTemp;
	int i, j, sizeList;

	if (NULL == psSymbolTable)
	{
		error = ST_ERROR_INVALID_SYMBOLTABLE;
	}
	else if (NULL == visitor)
	{
		error = ST_ERROR_NULL_PTR;
	}
	else
	{
		for (i = 0; i < psSymbolTable->tableSize; ++i)
		{
			lstFirst(&(psSymbolTable->psTable[i]), &sTemp);
			lstSize(&(psSymbolTable->psTable[i]), &sizeList);

			for (j = 0; j < sizeList; ++j)
			{
				lstNext(&(psSymbolTable->psTable[i]), &sTemp);
				visitor(sTemp.st_key,sTemp.st_size, sTemp.st_data);
			}
		}
	}

	return error;
}

/*******************************************************************************
 Function:		stAddRsrvdWords

 Description: adds reserved words to the Symbol Table and increments the level
							of the Symbol Table once completed without errors

 Parameters:	psSymbolTable - pointer to the Symbol Table
							pzWords				- array of words to be added
							numWords			- the number of words to be added

 Returned:		error - an ST_ERROR
*******************************************************************************/
ST_ERROR stAddRsrvdWords (SymbolTablePtr psSymbolTable, char **pszWords,
												 	 int numWords)
{
	ST_ERROR error = ST_NO_ERROR;
	SymbolTableType sType = RSRVWRD;
	int i, other = -1;

	if (NULL == psSymbolTable)
	{
		error = ST_ERROR_INVALID_SYMBOLTABLE;
	}
	else if ( NULL == *pszWords )
	{
		error = ST_ERROR_NULL_PTR;
	}
	else
	{
		for (i = 0; i < numWords && error == ST_NO_ERROR; ++i)
		{
			error = stAddEntry(psSymbolTable, pszWords[i], sType, RSRVWRD_MAIN + i, other);
		}

		if (ST_NO_ERROR == error)
		{
			error = stIncrementLevel(psSymbolTable);
		}
	}
	return error;
}

/*******************************************************************************
 Function:		isValidId

 Description: checks the length of an identifier before it is added to the ST

 Parameters:	pID		- the name that is checked

 Returned:		bool 	- true if identifier is within the accepted range;
 	 	 	 	 	 	 	 	 	 	 	otherwise, false.
*******************************************************************************/
bool isValidID (char *pID)
{
	bool isValid = false;
	int length = strlen(pID);

	if (0 <= length && MAX_ID_LENGTH > length)
	{
		isValid = true;
	}

	return isValid;
}

/*******************************************************************************
 Function:		stGetLvl

 Description: returns current level of the ST

 Parameters:	psST - pointer to the ST

 Returned:		sLevel - current level of ST
*******************************************************************************/
SymbolTableLevel stGetLvl (SymbolTablePtr psST)
{
	SymbolTableLevel sLevel = RSRVWRD_LEVEL;
	if (NULL !=psST)
	{
		sLevel = psST->currentLevel;
	}
	return sLevel;
}

/*******************************************************************************
 Function:    	userHashFunctionDivision

 Description:		Uses mid-square hash method

 Parameters:  	key        - the data over which to calculate the hash
 	 	 	 	 	 	 	 	size       - the size of the data
 	 	 	 	 	 	 	  result     - the hash value produced
 	 	 	 	 	 	 	 	tableSize  - the size of the hash table
 	 	 	 	 	 	 	 	             (for % size calculations)

 Returned:    	TRUE success 	FALSE failure of any kind
 ******************************************************************************/
int userHashFunctionMidSquare (char *pKey, unsigned int size,
															 unsigned int *pResult, unsigned int tableSize)
{
	unsigned int theData = 0;
	int i;
	unsigned int middle;

	for (i = 0; i < size; i++)
	{
		theData += pKey[i];
	}

 // compare results taking various bits from the key
	//middle = (theData & 0x000ff000) >> 12;
	 middle = (theData & 0x0ffffff0) >> 4;
  //	middle = (theData & 0x00ffff00) >> 8;

	*pResult = middle % tableSize;

	return true;
}
/*******************************************************************************
 Function:    	userHashFunctionDJB2

 Description:		Uses hash function from http://www.cse.yorku.ca/~oz/hash.html

 Parameters:  	key        - the data over which to calculate the hash
 	 	 	 	 	 	 	 	size       - the size of the data
 	 	 	 	 	 	 	  result     - the hash value produced
 	 	 	 	 	 	 	 	tableSize  - the size of the hash table
 	 	 	 	 	 	 	 	             (for % size calculations)

 Returned:    	TRUE success 	FALSE failure of any kind
 ******************************************************************************/
int userHashFunctionDJB2 (char *pKey, unsigned int size,
		 unsigned int *pResult, unsigned int tableSize)
{
	unsigned long hash = 5381;
	int c;

	while ((c = *pKey++))
	{
		hash = ((hash << 5) + hash) + c;
	}

	*pResult = hash % tableSize;

	return true;
}
/******************************************************************************
 Function:    	userCompare

 Description:		Compare two keys

 Parameters:  	pLeft     - the left key
 	 	 	 	 	 	 	  leftSize  - the size of the left key
 	 	 	 	 	 	 	 	pRight    - the right key
 	 	 	 	 	 	 	  rightSize - the size of the right key

 Returned:    	return < 0 if left <  right
 	 	 	 	 	 	 	  return   0 if left == right
 	 	 	 	 	 	 	 	return > 0 if left >  right
 *****************************************************************************/
int userCompare (char *pLeft, unsigned int leftSize, char *pRight,
								 unsigned int rightSize)
{
	int i;
	int retVal = 0;

	if (NULL != pLeft && NULL != pRight)
	{
		if (leftSize == rightSize)
		{
			for (i = 0; i < leftSize && !retVal; i++)
			{
				if (pLeft[i] > pRight[i])
				{
					retVal = 1;
				}
				else if (pLeft[i] < pRight[i])
				{
					retVal = -1;
				}
			}
		}
		else
		{
			return (1 - ((leftSize < rightSize) * 2));
		}
	}
	return retVal;
}

/******************************************************************************
 Function:    	userCompareStData

 Description:		Compare two ST_DATATYPEs

 Parameters:  	pLeft     - the left ST_DATATYPE
 	 	 	 	 	 	 	 	pRight    - the right ST_DATATYPEs

 Returned:    	return < 0 if left <  right
 	 	 	 	 	 	 	  return   0 if left == right
 	 	 	 	 	 	 	 	return > 0 if left >  right
 *****************************************************************************/
int userCompareStData (const void *pLeft, const void *pRight)
{
	int retVal = 0;

	ST_DATATYPE *pL = (ST_DATATYPE *) pLeft;
	ST_DATATYPE *pR = (ST_DATATYPE *) pRight;


	if (0 == strcmp(pL->pszIdentifier, pR->pszIdentifier))
	{
		if (pL->sLevel < pR->sLevel)
		{
			retVal = -1;
		}
		else if (pL->sLevel > pR->sLevel)
		{
			retVal = 1;
		}
	}
	else if (0 > strcmp(pL->pszIdentifier, pR->pszIdentifier))
	{
		retVal = -1;
	}
	else
	{
		retVal = 1;
	}

	return retVal;
}

