/*****************************************************************************
 File name:     ct.c
 Author:				Brianna Alcoran
 Date:          2/1/15
 Class:         CS480
 Assignment:    Compiler Project written in C
 Purpose:       Constant for the Compiler (implementation)
 Modifications:
******************************************************************************/

#include "../include/ct.h"

/*******************************************************************************
 Function:		ctCreate

 Description:	creates a constant table

 Parameters:	*hsCT	-	handle to the constant table
							size	-	number of entries in the constant table
							userHashFunction	-	pointer to a hash function
							userCompareConstants	- pointer to a compare function for the
																			constants in the table

 Returned:		error - a CT_ERROR
*******************************************************************************/
CT_ERROR ctCreate (ConstantTablePtr *hsCT, unsigned int size,
		int (*userHashFunction) (char* , unsigned int, unsigned int*, unsigned int),
		int (*userCompareConstants) (const void*, const void*))
{
	CT_ERROR error = CT_NO_ERROR;
	*hsCT = (ConstantTablePtr) malloc (sizeof(ConstantTable));

	if ( NULL == (*hsCT))
	{
		error = ERROR_NO_CONSTANTTABLE_CREATE;
	}
	else if ( NULL == userHashFunction || NULL == userCompareConstants)
	{
		error = CT_ERROR_NULL_PTR;
	}
	else
	{
		(*hsCT)->psTable = (ListPtr) malloc (sizeof(List)*size);

		if (NULL == (*hsCT)->psTable)
		{
			error = CT_ERROR_NULL_PTR;
		}
		else
		{
			int i;
			for (i = 0; i < size; ++i)
			{
				lstCreate(&(*hsCT)->psTable[i]);
			}

			(*hsCT)->tableSize = size;
			(*hsCT)->numEntries = 0;
			(*hsCT)->userHashFunction = userHashFunction;
			(*hsCT)->userCompareConstants = userCompareConstants;
		}
	}
	return error;
}

/*******************************************************************************
 Function:		ctDispose

 Description:	disposes of the constant table

 Parameters:	*hsCT	- handle to a constant table

 Returned:		error - a CT_ERROR
*******************************************************************************/
CT_ERROR ctDispose (ConstantTablePtr *hsCT)
{
	CT_ERROR error = NO_ERROR;
	if (NULL == *hsCT)
	{
		error = CT_ERROR_INVALID_CONSTANTTABLE;
	}
	else
	{
		int i;
		for (i = 0; i < (*hsCT)->tableSize; ++i )
		{
			lstDispose(&(*hsCT)->psTable[i]);
		}

		free ((*hsCT)->psTable);
		free (*hsCT);
		*hsCT = NULL;
	}

	return error;
}
/*******************************************************************************
 Function:		ctPrintHeader

 Description:	prints the header for the constant table print

 Parameters:	none

 Returned:		none
*******************************************************************************/
void ctPrintHeader ()
{
	printf("\n");
	printf ("%27s", "CONSTANT TABLE\n\n");
	printf ("%40s", "CONSTANT VALUE     RUNTIME STACK ADDRESS\n");
	printf ("%40s", "--------------     ---------------------\n");
}

/*******************************************************************************
 Function:		ctPrintEntry

 Description:	prints an entry of the constant table

 Parameters:	psCT	- pointer to the constant table
							sEntry	-	constant table entry to be printed

 Returned:
*******************************************************************************/
void ctPrintEntry (ConstantTablePtr psCT, CT_DATATYPE sEntry)
{
	printf ("%14d", sEntry.constant);
	printf ("%26d\n", sEntry.stackPosition);
}

/*******************************************************************************
 Function:		ctPrint

 Description:	prints the entire constant table

 Parameters:	psCT	-	pointer to the constant table

 Returned:		none
*******************************************************************************/
void ctPrint (ConstantTablePtr psCT)
{
	if (NULL != psCT)
	{
		CT_DATATYPE sEntries[psCT->numEntries];
		DATATYPE sTemp;
		int i, j, count = 0, sizeList;

		/* put all entries into an array */
		for (i = 0; i < psCT->tableSize; ++i)
		{
			lstFirst(&(psCT->psTable[i]), &sTemp);
			lstSize(&(psCT->psTable[i]), &sizeList);

			for (j = 0; j < sizeList; ++j)
			{
				lstNext(&(psCT->psTable[i]), &sTemp);
				sEntries[count] = sTemp.ct_data;
				++count;
			}
		}

		/* sort array using ConstantTable's userCompareCtData function */
		qsort(sEntries, psCT->numEntries, sizeof(CT_DATATYPE),
				psCT->userCompareConstants);

		/* print the sorted entries */
		ctPrintHeader();
		for (i = 0; i < psCT->numEntries; ++i)
		{
			ctPrintEntry (psCT, sEntries[i]);
		}
		printf("\n");

	}
	else
	{
		printf("ERROR INVALID CONSTANT TABLE\n");
	}

}

/*******************************************************************************
 Function:		ctSearch

 Description:	searches for a constant in the constant table. if it is found,
 	 	 	 	 	 	 	true and the constant table entry is passed back through the
 	 	 	 	 	 	 	argument list. otherwise. false and NULL is returned.

 Parameters:	psCT	-	pointer to the constant table
							constant	- the constant we are searching for
							*bFoundConstant	-	pointer to a bool indicating if constant found
							*psTheData	-	pointer to the constant table entry

 Returned:		error	- a CT_ERROR
*******************************************************************************/
CT_ERROR ctSearch (ConstantTablePtr psCT, int constant, bool *bFoundConstant,
										CT_DATATYPE *psTheData)
{
	CT_ERROR error = CT_NO_ERROR;
	DATATYPE sTemp;
	bool bIsListEmpty;
	*bFoundConstant = false;

	unsigned int bucket;
	int i, functionSuccess, listSize;

	if (NULL == psCT)
	{
		error = CT_ERROR_INVALID_CONSTANTTABLE;
	}
	else
	{
		if (0 != (psCT->numEntries))
		{
			char pKey[MAX_ID_LENGTH];
			memset (pKey, '\0', sizeof(pKey));
			sprintf(pKey, "%d", constant);

			/* get a table index using hash function and name we are searching for */
			functionSuccess = psCT->userHashFunction
												(pKey, strlen(pKey), &bucket, psCT->tableSize);

			if (functionSuccess)
			{	/* if the list at this index is empty, then the name we are
						searching is not in the table */
				lstIsEmpty(&(psCT->psTable[bucket]), &bIsListEmpty);
				if (!bIsListEmpty)
				{
					lstFirst(&(psCT->psTable[bucket]), &sTemp);
					lstSize(&(psCT->psTable[bucket]), &listSize);

					/*iterate through the elements in the bucket checking for a duplicate */
					for (i = 0; i < listSize && !(*bFoundConstant); ++i)
					{
						lstNext(&(psCT->psTable[bucket]), &sTemp);
						if (constant == sTemp.ct_data.constant)
						{
							*psTheData = sTemp.ct_data;
							*bFoundConstant = true;
						}
					}

					if (!(*bFoundConstant))
					{
						error = CT_ERROR_NO_DATA;
						psTheData = NULL;
					}
				}
			}
		}
	}
	return error;
}

/*******************************************************************************
 Function:		ctAddConstant

 Description:	adds a constant to the constant table (does not add if already in
							the constant table

 Parameters:	psCT	-	pointer to the constant table
							constant	- the constant we want to add
							stackPos	-	the position on the stack of the constant

 Returned:		error - a CT_ERROR
*******************************************************************************/
CT_ERROR ctAddConstant (ConstantTablePtr psCT, int constant, int stackPos)
{
	CT_ERROR error = CT_NO_ERROR;
	int functionSuccess;
	unsigned int bucket;
	bool bResult, bHasCurrent;
	DATATYPE sTemp, sNewData;
	int sizeList, i;

	if (NULL == psCT)
	{
		error = CT_ERROR_INVALID_CONSTANTTABLE;
	}
	else
	{
		/* initializing the new entry */
		sNewData.ct_data.constant = constant;
		sNewData.ct_data.stackPosition = stackPos;

		char pKey[MAX_ID_LENGTH];
		memset (pKey, '\0', sizeof(pKey));
		sprintf(pKey, "%d", constant);

		if (psCT->numEntries >= MAX_CONSTANTS) //full table dont add
		{
			error = CT_ERROR_FULL;
		}
		else
		{
			functionSuccess =	psCT->userHashFunction
				(pKey, strlen(pKey), &bucket, psCT->tableSize);

			if (functionSuccess == true)
			{
				lstIsEmpty(&(psCT->psTable[bucket]), &bResult);

				if (true == bResult) /* bucket was empty */
				{
					lstInsertAfter(&(psCT->psTable[bucket]), sNewData);
					++(psCT->numEntries);
				}
				else			/* bucket was not empty */
				{
					lstFirst(&(psCT->psTable[bucket]), &sTemp);
					lstSize(&(psCT->psTable[bucket]), &sizeList);

					/* iterate through the elements in the bucket checking for a duplicate */
					for (i = 0; i < sizeList; ++i)
					{ /* check for same constant */
						if ( constant == sTemp.ct_data.constant)
						{
							error = CT_ERROR_DUPLICATE;
							break;
						}

						lstNext(&(psCT->psTable[bucket]), &sTemp);
						lstPeek(&(psCT->psTable[bucket]), &sTemp);
						lstHasCurrent(&(psCT->psTable[bucket]), &bHasCurrent);
						if (!bHasCurrent) /* place new entry as last element in the list */
						{
							lstLast(&(psCT->psTable[bucket]), &sTemp);
							lstInsertAfter(&(psCT->psTable[bucket]), sNewData);
							++(psCT->numEntries);
						}
					}
				}
			}
			else
			{
				error = CT_ERROR_HASH_FAILED;
			}
		}
	}
	return error;
}


/*******************************************************************************
 Function:    	userHashFunctionConstantTable

 Description:		hash function that calculates a hash over an int using the
								mid square method

 Parameters:  	constant   - the int over which to calculate the hash
 	 	 	 	 	 	 	 	size       - the size of the data
 	 	 	 	 	 	 	  result     - the hash value produced
 	 	 	 	 	 	 	 	tableSize  - the size of the hash table
 	 	 	 	 	 	 	 	             (for % size calculations)

 Returned:    	TRUE success 	FALSE failure of any kind
 ******************************************************************************/
int userHashFunctionConstantTable (int constant, unsigned int size,
		 unsigned int *pResult, unsigned int tableSize)
{
	unsigned int theData = 0;
	int i = 0;
	char pKey[MAX_ID_LENGTH];
	unsigned int middle;
	memset (pKey, '\0', sizeof(pKey));
	sprintf(pKey, "%d", constant);

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


/******************************************************************************
 Function:    	userCompareCtData

 Description:		Compare two CT_DATATYPEs

 Parameters:  	pLeft     - the left ST_DATATYPE
 	 	 	 	 	 	 	 	pRight    - the right ST_DATATYPEs

 Returned:    	return < 0 if left <  right
 	 	 	 	 	 	 	  return   0 if left == right
 	 	 	 	 	 	 	 	return > 0 if left >  right
 *****************************************************************************/
int userCompareCtData (const void *pLeft, const void *pRight)
{
	int retVal = 0;

	CT_DATATYPE *pL = (CT_DATATYPE *) pLeft;
	CT_DATATYPE *pR = (CT_DATATYPE *) pRight;

	if (pL->constant < pR->constant)
	{
		return -1;
	}
	else if (pL->constant == pR->constant)
	{
		return 0;
	}
	else
	{
		return 1;
	}

	return retVal;
}
