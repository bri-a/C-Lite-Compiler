/*******************************************************************************
 File name:		  list.c
 Author:        Brianna Alcoran
 Date:          10/1/14
 Class:         CS300
 Assignment:    List Implementation
 Purpose:				This is the implementation file for each of the list functions
 	 	 	 	 	 	 	 	associated with the dynamic list module.
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "../include/list.h"

/*******************************************************************************
 Function:  	lstCreate

 Description: Initializes the list by setting size to 0 and setting
							psCurrent, psHead, and psLast to NULL.

 Parameters:	psList - the pointer to the list

 Returned:   	ERROR_NO_LIST_CREATE if the list cannot be created
 	 	 	 				NO_ERROR if no errors occur
 ******************************************************************************/
 ERRORCODE lstCreate (ListPtr psList)
 {
	 ERRORCODE error = NO_ERROR;

	 if (NULL != psList)
	 {
		 psList->numElements = 0;
		 psList->psCurrent = NULL;
		 psList->psHead = NULL;
		 psList->psLast = NULL;
	 }
	 else
	 {
		 error = ERROR_NO_LIST_CREATE;
	 }
	 return error;
 }

/*******************************************************************************
 Function:  	lstDispose

 Description: Disposes the list by freeing each ListElementPtr and
 	 	 	 	 	 	 	decrementing the number of elements until psHead is NULL.

 Parameters:	psList - the pointer to the list

 Returned:   	ERROR_INVALID_LIST if psList is NULL
 	 	 	 				NO_ERROR if no errors occur
*******************************************************************************/
 ERRORCODE lstDispose (ListPtr psList)
 {
	 ERRORCODE error = NO_ERROR;

	 if (NULL == psList)
	 {
		 error = ERROR_INVALID_LIST;
	 }
	 else
	 {
		 while (psList->psHead != NULL)
		 {
			 psList->psCurrent = psList->psLast->psPrev;

			 if (psList->psCurrent == NULL) /* freeing the only element in the list */
			 {
				 free((ListElementPtr)psList->psLast);

				 psList->psHead = NULL;
				 psList->psCurrent = NULL;
				 psList->psLast= NULL;
			 }
			 else
			 {
				 free((ListElementPtr)psList->psLast);
				 psList->psLast = psList->psCurrent;
			 }
			 --psList->numElements;
		 }
	 }
	 return error;
 }

/*******************************************************************************
*									Checking number of elements in list
*******************************************************************************/

/*******************************************************************************
 Function:  	lstSize

 Description: Adjusts *pSize to the number of elements in the list

 Parameters:	psList - the pointer to the list
 	 	 	 	 	 	 	pSize - pointer to an integer of the size of the list

 Returned:   	ERROR_INVALID_LIST if psList is NULL
 	 	 	 				NO_ERROR if no errors occur
*******************************************************************************/
 ERRORCODE lstSize (ListPtr psList, int *pSize)
 {
	 ERRORCODE error = NO_ERROR;

	 if (NULL == psList)
	 {
		 error = ERROR_INVALID_LIST;
	 }
	 else
	 {
		 if (NULL == pSize)
		 {
			 error = ERROR_NULL_PTR;
		 }
		 else
		 {
			 *pSize = psList->numElements;
		 }
	 }

	 return error;
 }

/*******************************************************************************
 Function:  	lstFull

 Description: Checks if the list is full and requires that psList is not NULL.
 	 	 	 	 	 	 	Assume the list is full; adjust *bIsFull to true. If the list
 	 	 	 	 	 	 	is not NULL, adjust *bIsFull to false.

 Parameters:	psList - the pointer to the list
 	 	 	 	 	 	 	bIsFull - pointer to a boolean

 Returned:   	ERROR_INVALID_LIST if psList is NULL
 	 	 	 				NO_ERROR if no errors occur
*******************************************************************************/
 ERRORCODE lstIsFull (ListPtr psList, bool *bIsFull)
 {
	 ERRORCODE error = NO_ERROR;

		if (NULL == psList)
		{
			error = ERROR_INVALID_LIST;
		}
		else
		{
			if (NULL == bIsFull)
			{
				error = ERROR_NULL_PTR;
			}
			else
			{
				*bIsFull = false;
			}
		}

	 return error;
 }

/*******************************************************************************
 Function:  	lstEmpty

 Description: Checks if the list is empty and requires that psList is not NULL.
 	 	 	 	 	 	 	If the list is empty, adjust *bIsEmpty to true. If the list is
 	 	 	 	 	 	 	not empty, adjust *bIsEmpty to false.

 Parameters:	psList - the pointer to the list
 	 	 	 	 	 	 	bIsEmpty - pointer to a boolean

 Returned:   	ERROR_INVALID_LIST if psList is NULL
 	 	 	 				NO_ERROR if no errors occur
*******************************************************************************/
 ERRORCODE lstIsEmpty (ListPtr psList, bool *bIsEmpty)
 {
	 ERRORCODE error = NO_ERROR;

	 if (NULL == psList)
	 {
		 error = ERROR_INVALID_LIST;
	 }
	 else
	 {

		 if (NULL == bIsEmpty)
		 {
			 error = ERROR_NULL_PTR;
		 }
		 else
		 {
			 if (0 == (psList->numElements))
			 {
				 *bIsEmpty = true;
			 }
			 else if (0 < (psList->numElements))
			 {
				 *bIsEmpty = false;
			 }
		 }

	 }

	 return error;
 }

/*******************************************************************************
*													Peek Operations
*******************************************************************************/

/*******************************************************************************
 Function:  	lstPeek

 Description: Requires that psList is not NULL, that list is not empty, and
 	 	 	 	 	 	 	and that psCurrent isn't NULL. If psCurrent is NULL, return error.
 	 	 	 	 	 	 	If the list is empty, return the error. If the list is not empty,
 	 	 	 	 	 	 	the value of the current element is returned to *pValue.

 Parameters:	psList - the pointer to the list
 	 	 	 	 	 	 	pValue - the pointer to the value of the current element

 Returned:   	ERROR_INVALID_LIST if psList is NULL
							ERROR_NULL_PTR if pValue is NULL
 	 	 	 	 	 	 	ERROR_EMPTY_LIST if the list is empty
 	 	 	 	 	 	 	ERROR_NO_CURRENT if the current is NULL
 	 	 	 				NO_ERROR if no errors occur
*******************************************************************************/
 ERRORCODE lstPeek (ListPtr psList, DATATYPE *pValue)
 {
	 ERRORCODE error = NO_ERROR;
	 bool bIsEmpty;

	 if (NULL == psList)
	 {
		 error = ERROR_INVALID_LIST;
	 }
	 else
	 {
		 if (NULL == pValue)
		 {
			 error = ERROR_NULL_PTR;
		 }
		 else
		 {
			 error = lstIsEmpty(psList, &bIsEmpty);
			 if (NO_ERROR == error)
			 {
				 if(bIsEmpty)
				 {
					 error = ERROR_EMPTY_LIST;
				 }
				 else
				 {
					 if (NULL == psList->psCurrent)
					 {
						 error = ERROR_NO_CURRENT;
					 }
					 else
					 {
						 *pValue = psList->psCurrent->data;
					 }
				 }
			 }
		 }

	 }
	 return error;
 }

/*******************************************************************************
 Function:  	lstPeekPrev

 Description: Requires that psList is not NULL, that list contains two or more
							elements, and that current is not the first element. The datavalue
							of current's predecessor is returned to *pValue.

 Parameters:	psList - the pointer to the list
 	 	 	 	 	 	 	pValue - the pointer to the value of the element previous to
 	 	 	 	 	 	 					 current

 Returned:   	ERROR_INVALID_LIST if psList is NULL
							ERROR_NULL_PTR if pValue is NULL
							ERROR_EMPTY_LIST if the list is empty
							ERROR_NO_CURRENT if current is NULL
							ERROR_NO_PREV if current is the first element
 	 	 	 				NO_ERROR if no errors occur
*******************************************************************************/
 ERRORCODE lstPeekPrev (ListPtr psList, DATATYPE *pValue)
 {
	 ERRORCODE error = NO_ERROR;
	 bool bIsEmpty;

	 if (NULL == psList)
	 {
		 error = ERROR_INVALID_LIST;
	 }
	 else
	 {
		 if (NULL == pValue)
		 {
			 error = ERROR_NULL_PTR;
		 }
		 else
		 {
			 error = lstIsEmpty(psList, &bIsEmpty);
			 if (NO_ERROR == error)
			 {
				 if (bIsEmpty)
				 {
					 error = ERROR_EMPTY_LIST;
				 }
				 else
				 {
					 if (NULL == psList->psCurrent)
					 {
					 	 error = ERROR_NO_CURRENT;
					 }
					 else
					 {
					 	 if (NULL == psList->psCurrent->psPrev) /*current is first*/
					 	 {
					 		 error = ERROR_NO_PREV;
					 	 }
					 	 else
					 	 {
					 		 *pValue = psList->psCurrent->psPrev->data;
					 	 }
					 }
				 }
			 }
		 }
	 }

	 return error;
 }

/*******************************************************************************
 Function:  	lstPeekNext

 Description: Requires that psList is not NULL, that list contains two or more
							elements, and that current is not the last element. The datavalue
							of current's predecessor is returned to *pValue.

 Parameters:	psList - the pointer to the list
 	 	 	 	 	 	 	pValue - the pointer to the value of the element previous to
 	 	 	 	 	 	 					 current

 Returned:   	ERROR_INVALID_LIST if psList is NULL
 	 	 	 	 	 	 	ERROR_NO_NEXT if current is the last element
 	 	 	 				NO_ERROR if no errors occur
*******************************************************************************/
 ERRORCODE lstPeekNext (ListPtr psList, DATATYPE *pValue)
 {
	 ERRORCODE error = NO_ERROR;
	 bool bIsEmpty;

	 if (NULL == psList)
	 {
		 error = ERROR_INVALID_LIST;
	 }
	 else
	 {
		 if (NULL == pValue)
		 {
			 error = ERROR_NULL_PTR;
		 }
		 else
		 {
			 error = lstIsEmpty(psList, &bIsEmpty);
			 if (NO_ERROR == error)
			 {
				 if (bIsEmpty)
				 {
					 error = ERROR_EMPTY_LIST;
				 }
				 else
				 {
					 if (NULL == psList->psCurrent)
					 {
					 	 error = ERROR_NO_CURRENT;
					 }
					 else
					 {
					 	 if (NULL == psList->psCurrent->psNext) /*current is not first*/
					 	 {
					 		 error = ERROR_NO_NEXT;
					 	 }
					 	 else
					 	 {
					 		 *pValue = psList->psCurrent->psNext->data;
					 	 }
					 }
				 }
			 }
		 }
	 }

	 return error;
 }

/*******************************************************************************
*							Retrieving values and updating current
*******************************************************************************/

/*******************************************************************************
 Function:  	lstFirst

 Description: Requires that psList is not NULL and that the list is not empty.
 	 	 	 	 	 	 	The datavalue of the first element is returned to *pValue. Then,
 	 	 	 	 	 	 	current is changed to the first element.

 Parameters:	psList - the pointer to the list
 	 	 	 	 	 	 	pValue - the pointer to the value of the first element

 Returned:   	ERROR_INVALID_LIST if psList is NULL
 	 	 	 	 	 	 	ERROR_EMPTY_LIST if the list is empty
 	 	 	 	 	 	 	ERROR_NULL_PTR if the first element does not exist
 	 	 	 				NO_ERROR if no errors occur
*******************************************************************************/
 ERRORCODE lstFirst(ListPtr psList, DATATYPE *pValue)
 {
	 ERRORCODE error = NO_ERROR;
	 bool bIsEmpty;

	 if (NULL == psList)
	 {
		 error = ERROR_INVALID_LIST;
	 }
	 else
	 {
		 if (NULL == pValue)
		 {
			 error = ERROR_NULL_PTR;
		 }
		 else
		 {
			 error = lstIsEmpty(psList, &bIsEmpty);
			 if (NO_ERROR == error)
			 {
				 if (bIsEmpty)
				 {
					 error = ERROR_EMPTY_LIST;
				 }
				 else
				 {
					 *pValue = psList->psHead->data;
					 psList->psCurrent = psList->psHead;
				 }
			 }
		 }
	 }

	 return error;
 }

/*******************************************************************************
 Function:  	lstLast

 Description: Requires that psList is not NULL and that the list is not empty.
 	 	 	 	 	 	 	The datavalue of the last element is returned to *pValue. Then,
 	 	 	 	 	 	 	current is changed to the last element.

 Parameters:	psList - the pointer to the list
 	 	 	 	 	 	 	pValue - the pointer to the value of the last element

 Returned:   	ERROR_INVALID_LIST if psList is NULL
 	 	 	 	 	 	 	ERROR_EMPTY_LIST if the list is empty
 	 	 	 	 	 	 	ERROR_NULL_PTR if the last element does not exist
 	 	 	 				NO_ERROR if no errors occur
*******************************************************************************/
 ERRORCODE lstLast(ListPtr psList, DATATYPE *pValue)
 {
	 ERRORCODE error = NO_ERROR;
	 bool bIsEmpty;

	 if (NULL == psList)
	 {
		 error = ERROR_INVALID_LIST;
	 }
	 else
	 {
		 if (NULL == psList->psLast || NULL == pValue)
		 {
			 error = ERROR_NULL_PTR;
		 }
		 else
		 {
			 error = lstIsEmpty(psList, &bIsEmpty);
			 if (NO_ERROR == error)
			 {
				 if (bIsEmpty)
				 {
					 error = ERROR_EMPTY_LIST;
				 }
				 else
				 {
 					 *pValue = psList->psLast->data;
 					 psList->psCurrent = psList->psLast;
				 }
			 }
		 }
	 }

	 return error;
 }

/*******************************************************************************
 Function:  	lstNext

 Description: Requires that psList is not NULL, that the list is not empty,
 	 	 	 	 	 	 	and that current is not past the end of the list.
 	 	 	 	 	 	 	The datavalue of the current element is returned to *pValue.
 	 	 	 	 	 	 	Then, current is changed to the successor of the current element.

 Parameters:	psList - the pointer to the list
 	 	 	 	 	 	 	pValue - the pointer to the value of the current element

 Returned:   	ERROR_INVALID_LIST if psList is NULL
 	 	 	 	 	 	 	ERROR_EMPTY_LIST if the list is empty
 	 	 	 	 	 	 	ERROR_NULL_PTR if the current element does not exist
 	 	 	 				NO_ERROR if no errors occur
*******************************************************************************/
 ERRORCODE lstNext (ListPtr psList, DATATYPE *pValue)
 {
	 ERRORCODE error = NO_ERROR;
	 bool bIsEmpty;

	 if (NULL == psList)
	 {
		 error = ERROR_INVALID_LIST;
	 }
	 else
	 {
		 if (NULL == pValue)
		 {
			 error = ERROR_NULL_PTR;
		 }
		 else
		 {
			 error = lstIsEmpty(psList, &bIsEmpty);
			 if (NO_ERROR == error)
			 {
				 if (bIsEmpty)
				 {
					 error = ERROR_EMPTY_LIST;
				 }
				 else
				 {
					 if (NULL == psList->psCurrent)
					 {
						 error = ERROR_NO_CURRENT;
					 }
					 else
					 {
						 *pValue = psList->psCurrent->data;
						// pValue = &(psList->psCurrent->data);
						 psList->psCurrent = psList->psCurrent->psNext;
					 }
				 }
			 }
		 }
	 }

	 return error;
 }

/*******************************************************************************
 Function:  	lstPrev

 Description: Requires that psList is not NULL, that the list is not empty,
 	 	 	 	 	 	 	and that current is not past the first of the list.
 	 	 	 	 	 	 	The datavalue of the current element is returned to *pValue.
 	 	 	 	 	 	 	Then, current is changed to the predecessor of the current element.

 Parameters:	psList - the pointer to the list
 	 	 	 	 	 	 	pValue - the pointer to the value of the current element

 Returned:   	ERROR_INVALID_LIST if psList is NULL
 	 	 	 	 	 	 	ERROR_EMPTY_LIST if the list is empty
 	 	 	 	 	 	 	ERROR_NULL_PTR if the current element does not exist
 	 	 	 				NO_ERROR if no errors occur
*******************************************************************************/
 ERRORCODE lstPrev(ListPtr psList, DATATYPE *pValue)
 {
	 ERRORCODE error = NO_ERROR;
	 bool bIsEmpty;

	 if (NULL == psList)
	 {
		 error = ERROR_INVALID_LIST;
	 }
	 else
	 {
		 if (NULL == pValue)
		 {
			 error = ERROR_NULL_PTR;
		 }
		 else
		 {
			 error = lstIsEmpty(psList, &bIsEmpty);
			 if (NO_ERROR == error)
			 {
				 if (bIsEmpty)
				 {
					 error = ERROR_EMPTY_LIST;
				 }
				 else
				 {
					 if (NULL == psList->psCurrent)
					 {
						 error = ERROR_NO_CURRENT;
					 }
					 else
					 {
			 			 *pValue = psList->psCurrent->data;
			 			 psList->psCurrent = psList->psCurrent->psPrev;
					 }

				 }
			 }
		 }
	 }

	 return error;
 }

 /*******************************************************************************
  Function:  	lstCurrent

  Description: Requires that psList is not NULL, that the list is not empty,
  	 	 	 	 	 	 	and that current is not NULL.
  	 	 	 	 	 	 	The datavalue of the current element is returned to *pValue.

  Parameters:	psList - the pointer to the list
  	 	 	 	 	 	 	pValue - the pointer to the value of the current element

  Returned:   	ERROR_INVALID_LIST if psList is NULL
  	 	 	 	 	 	 	ERROR_EMPTY_LIST if the list is empty
  	 	 	 	 	 	 	ERROR_NULL_PTR if the current element does not exist
  	 	 	 				NO_ERROR if no errors occur
 *******************************************************************************/
  ERRORCODE lstCurrent(ListPtr psList, DATATYPE *pValue)
  {
 	 ERRORCODE error = NO_ERROR;
 	 bool bIsEmpty;

 	 if (NULL == psList)
 	 {
 		 error = ERROR_INVALID_LIST;
 	 }
 	 else
 	 {
 		 if (NULL == pValue)
 		 {
 			 error = ERROR_NULL_PTR;
 		 }
 		 else
 		 {
 			 error = lstIsEmpty(psList, &bIsEmpty);
 			 if (NO_ERROR == error)
 			 {
 				 if (bIsEmpty)
 				 {
 					 error = ERROR_EMPTY_LIST;
 				 }
 				 else
 				 {
 					 if (NULL == psList->psCurrent)
 					 {
 						 error = ERROR_NO_CURRENT;
 					 }
 					 else
 					 {
 			 			 *pValue = psList->psCurrent->data;
 					 }

 				 }
 			 }
 		 }
 	 }

 	 return error;
  }

/*******************************************************************************
*									Insertion, Deletion, and Updating
*******************************************************************************/

/*******************************************************************************
 Function:  	lstDeleteCurrent

 Description: Requires that psList is not NULL, that the list is not empty,
  						and that current is not NULL. If these requirements are satisfied,
  						the current element is deleted and its successor and predecessor
  						become each others successor and predecessor. If the deleted
  						element had a predecessor, then make it the current element;
  						otherwise make the first element current if it exists.

 Parameters:	psList - the pointer to the list
 	 	 	 	 	 	 	pValue - the pointer to a boolean

 Returned:   	ERROR_INVALID_LIST if psList is NULL
 	 	 	 	 	 	 	ERROR_EMPTY_LIST if the list is empty
 	 	 	 	 	 	 	ERROR_NO_CURRENT if psCurrent is NULL
 	 	 	 				NO_ERROR if no errors occur
*******************************************************************************/
 ERRORCODE lstDeleteCurrent (ListPtr psList, DATATYPE *pValue)
 {
	 ERRORCODE error = NO_ERROR;
	 bool bIsEmpty;

	 if (NULL == psList)
	 {
		 error = ERROR_INVALID_LIST;
	 }
	 else
	 {
		 if (NULL == pValue)
		 {
			 error = ERROR_NULL_PTR;
		 }
		 else
		 {
			 error = lstIsEmpty(psList, &bIsEmpty);
			 if (NO_ERROR == error)
			 {
				 if (bIsEmpty)
				 {
					 error = ERROR_EMPTY_LIST;
				 }
				 else
				 {
		 			 if (NULL == psList->psCurrent)
		 			 {
		 				 error = ERROR_NO_CURRENT;
		 			 }
		 			 else
		 			 {
		 				 if ((psList->psCurrent == psList->psHead) &&
		 						 	(psList->psCurrent == psList->psLast))
		 				 {
		 					 /* freeing the only element in the list */

		 					 *pValue = psList->psCurrent->data;
		 					 free((ListElementPtr)psList->psCurrent);

		 					 psList->psHead = NULL;
		 					 psList->psCurrent = NULL;
		 					 psList->psLast= NULL;

		 					 --psList->numElements;

		 				 }
		 				 else if (psList->psCurrent == psList->psHead)
		 				 {
		 					 psList->psHead = psList->psCurrent->psNext;
		 					 psList->psHead->psPrev = NULL;

		 					 *pValue = psList->psCurrent->data;
		 					 free((ListElementPtr)psList->psCurrent);
		 					 psList->psCurrent = psList->psHead;

		 					 --psList->numElements;

		 				 }
		 				 else if(psList->psCurrent == psList->psLast)
		 				 {
		 					 psList->psLast = psList->psCurrent->psPrev;
		 					 psList->psLast->psNext = NULL;

		 					 *pValue = psList->psCurrent->data;
		 					 free((ListElementPtr)psList->psCurrent);
		 					 psList->psCurrent = psList->psLast;

		 					 --psList->numElements;
		 				 }
		 				 else
		 				 {
		 					 ListElementPtr psPrevOfCurrent;
		 					 psPrevOfCurrent = psList->psCurrent->psPrev;
		 					 psList->psCurrent->psPrev->psNext = psList->psCurrent->psNext;
		 					 psList->psCurrent->psNext->psPrev = psList->psCurrent->psPrev;

		 					 *pValue = psList->psCurrent->data;
		 					 free((ListElementPtr)psList->psCurrent);

		 					 psList->psCurrent = psPrevOfCurrent;

		 					 --psList->numElements;
		 				 }
		 			 }
				 }
			 }
		 }
	 }

	 return error;
 }

/*******************************************************************************
 Function:  	lstInsertAfter

 Description: Requires that psList is not NULL and that list is not full.
 	 	 	 	 	 	 	If the list is empty, the element being inserted is the first
 	 	 	 	 	 	 	element in the list. Set psHead, psCurrent, and psLast to this
 	 	 	 	 	 	 	element. If the list is not empty, the element being inserted
 	 	 	 	 	 	 	is not the first. Set psCurrent to this new element.

 Parameters:	psList - the pointer to the list
 	 	 	 	 	 	 	value - the value of the element being inserted into the list

 Returned:   	ERROR_INVALID_LIST if psList is NULL
 	 	 	 	 	 	 	ERROR_FULL_LIST if the list is full
 	 	 	 	 	 	 	ERROR_NO_CURRENT if psCurrent is NULL
 	 	 	 				NO_ERROR if no errors occur
*******************************************************************************/
 ERRORCODE lstInsertAfter (ListPtr psList, DATATYPE value)
 {
	 ERRORCODE error = NO_ERROR;

	 bool bIsEmpty;
	 bool bIsFull;

	 if (NULL == psList)
	 {
		 error = ERROR_INVALID_LIST;
	 }
	 else
	 {
		 error = lstIsFull(psList, &bIsFull);
		 if (NO_ERROR == error)
		 {
			 if(!bIsFull)
			 {
				 error = lstIsEmpty(psList, &bIsEmpty);
				 if (NO_ERROR == error)
				 {
					 if (bIsEmpty)
					 {
						 /* adding the first element to the list */

						 ListElementPtr psNew;
						 psNew = (ListElementPtr)malloc(sizeof(ListElement));
						 psNew->data = value;

						 /* set head, current, and last to the new element */
						 psList->psHead = psNew;
						 psList->psCurrent = psNew;
						 psList->psLast = psNew;

						 /* since this is the first element, there is no next and prev */
						 psNew->psNext = NULL;
						 psNew->psPrev = NULL;
						 ++psList->numElements;
					 }
					 else
					 {
						 if (NULL == psList->psCurrent)
						 {
							 error = ERROR_NO_CURRENT;
						 }
						 else
						 {
							 ListElementPtr psNew;
							 psNew = (ListElementPtr)malloc(sizeof(ListElement));
							 psNew->data = value;

							 if (psList->psCurrent == psList->psLast)
							 {
								 /* if current is pointing to the last of the list, the
								  * element being added will be the new last of the list */

								 psList->psLast = psNew;
								 psNew->psPrev = psList->psCurrent;
								 psNew->psNext = NULL;
								 psList->psCurrent->psNext = psNew;
								 psList->psCurrent = psNew;
							 }
							 else
							 {
								 /* the element being added is not going to be the new head of
								  * the list */

								 psNew->psPrev = psList->psCurrent;
								 psNew->psNext = psList->psCurrent->psNext;
								 psList->psCurrent->psNext->psPrev = psNew;
								 psList->psCurrent->psNext = psNew;

								 psList->psCurrent = psNew;
							 }

							 ++psList->numElements;
						 }
					 }
				 }
			 }
			 else
			 {
				 error = ERROR_FULL_LIST;
			 }
		 }
	 }

	 return error;
 }

/*******************************************************************************
 Function:  	lstInsertBefore

 Description: Requires that psList is not NULL and that list is not full.
 	 	 	 	 	 	 	If the list is empty, the element being inserted is the first
 	 	 	 	 	 	 	element in the list. Set psHead, psCurrent, and psLast to this
 	 	 	 	 	 	 	element. If the list is not empty, the element being inserted
 	 	 	 	 	 	 	is not the first. Set psCurrent to this new element.

 Parameters:	psList - the pointer to the list
 	 	 	 	 	 	 	value - the value of the element being inserted into the list

 Returned:   	ERROR_INVALID_LIST if psList is NULL
 	 	 	 	 	 	 	ERROR_NO_CURRENT if psCurrent is NULL
 	 	 	 	 	 	 	ERROR_FULL_LIST if the list is full
 	 	 	 				NO_ERROR if no errors occur
*******************************************************************************/
 ERRORCODE lstInsertBefore (ListPtr psList, DATATYPE value)
 {
	 ERRORCODE error = NO_ERROR;
	 bool bIsEmpty;
	 bool bIsFull;

	 if (NULL == psList)
	 {
		 error = ERROR_INVALID_LIST;
	 }
	 else
	 {
		 error = lstIsFull(psList, &bIsFull);
		 if (NO_ERROR == error)
		 {
			 if(!bIsFull)
			 {
				 error = lstIsEmpty(psList, &bIsEmpty);
				 if (NO_ERROR == error)
				 {
					 if (bIsEmpty)
					 {
						 /* adding the first element to the list */

						 ListElementPtr psNew;
						 psNew = (ListElementPtr)malloc(sizeof(ListElement));

						 /* set head, current, and last to the new element */
						 psList->psHead = psNew;
						 psList->psCurrent = psNew;
						 psList->psLast = psNew;

						 /* since this is the first element, there is no next and prev */
						 psNew->data = value;
						 psNew->psNext = NULL;
						 psNew->psPrev = NULL;
						 ++psList->numElements;
					 }
					 else
					 {
						 if (NULL == psList->psCurrent)
						 {
							 error = ERROR_NO_CURRENT;
						 }
						 else
						 {
							 ListElementPtr psNew;
							 psNew = (ListElementPtr)malloc(sizeof(ListElement));
							 psNew->data = value;

							 if (psList->psCurrent == psList->psHead)
							 {
								 /* if current is pointing to the head of the list, the
								  * element being added will be the new head of the list */

								 psList->psHead = psNew;
								 psNew->psPrev = NULL;
								 psNew->psNext = psList->psCurrent;
								 psList->psCurrent->psPrev = psNew;
								 psList->psCurrent = psNew;
							 }
							 else
							 {
								 /* the element being added is not going to be the new head of
								  * the list */

								 psNew->psPrev = psList->psCurrent->psPrev;
								 psNew->psNext = psList->psCurrent;
								 psList->psCurrent->psPrev->psNext = psNew;
								 psList->psCurrent->psPrev = psNew;

								 psList->psCurrent = psNew;
							 }

							 ++psList->numElements;
						 }
					 }
				 }
			 }
			 else
			 {
				 error = ERROR_FULL_LIST;
			 }
		 }
	 }
	 return error;
 }

/*******************************************************************************
 Function:  	lstUpdateCurrent

 Description: Requires that psList is not NULL, that the list is not empty, and
 	 	 	 	 	 	 	that current is not NULL. Changes the data value of the current
 	 	 	 	 	 	 	element to the value passed into the function.

 Parameters:	psList - the pointer to the list
 	 	 	 	 	 	 	value - the value passed to the data of the current element

 Returned:   	ERROR_INVALID_LIST if psList is NULL
 	 	 	 	 	 	 	ERROR_EMPTY_LIST if the list is empty
 	 	 	 	 	 	 	ERROR_NO_CURRENT if psCurrent is NULL
 	 	 	 				NO_ERROR if no errors occur
*******************************************************************************/
 ERRORCODE lstUpdateCurrent (ListPtr psList, DATATYPE value)
 {
	 ERRORCODE error = NO_ERROR;
	 bool bIsEmpty;

	 if(NULL == psList)
	 {
		 error = ERROR_INVALID_LIST;
	 }
	 else
	 {
		 error = lstIsEmpty(psList, &bIsEmpty);
		 if (NO_ERROR == error)
		 {
			 if (bIsEmpty)
			 {
				 error = ERROR_EMPTY_LIST;
			 }
			 else
			 {
				 if (NULL == psList->psCurrent)
				 {
					 error = ERROR_NO_CURRENT;
				 }
				 else
				 {
					 psList->psCurrent->data = value;
				 }
			 }
		 }
	 }

	 return error;
 }

/*******************************************************************************
*												List Testing
*******************************************************************************/

/*******************************************************************************
 Function:  	lstHasNext

 Description: Requires that psList is not NULL, that psHasNext is not NULL, that
 	 	 	 	 	 	 	the list is not empty, and that current is not NULL. If these
 	 	 	 	 	 	 	requirements are satisfied, pbHasNext is changed to false if
 	 	 	 	 	 	 	psCurrent has psNext that is NULL. Otherwise, pbHasNext is true.

 Parameters:	psList - the pointer to the list
 	 	 	 	 	 	 	pbHasNext - the pointer to a boolean

 Returned:   	ERROR_INVALID_LIST if psList is NULL
 	 	 	 	 	 	 	ERROR_NULL_PTR if pbHasNext is NULL
 	 	 	 	 	 	 	ERROR_EMPTY_LIST if the list is empty
 	 	 	 	 	 	 	ERROR_NO_CURRENT if psCurrent is NULL
 	 	 	 				NO_ERROR if no errors occur
*******************************************************************************/
 ERRORCODE lstHasNext (ListPtr psList, bool *pbHasNext)
 {
	 ERRORCODE error = NO_ERROR;
	 bool bIsEmpty;

	 if (NULL == psList)
	 {
		 error = ERROR_INVALID_LIST;
	 }
	 else
	 {
		 if (NULL == pbHasNext)
		 {
			 error = ERROR_NULL_PTR;
		 }
		 else
		 {
			 error = lstIsEmpty(psList, &bIsEmpty);
			 if (NO_ERROR == error)
			 {
				 if (bIsEmpty)
				 {
					 error = ERROR_EMPTY_LIST;
				 }
				 else
				 {
					 if (NULL == psList->psCurrent)
					 {
						 error = ERROR_NO_CURRENT;
					 }
					 else
					 {
						 if (NULL == psList->psCurrent->psNext)
						 {
							 *pbHasNext = false;
						 }
						 else
						 {
							 *pbHasNext = true;
						 }
					 }
				 }
			 }
		 }
	 }

	 return error;
 }

/*******************************************************************************
 Function:  	lstHasPrev

 Description: Requires that psList is not NULL, that psHasPrev is not NULL, that
 	 	 	 	 	 	 	the list is not empty, and that current is not NULL. If these
 	 	 	 	 	 	 	requirements are satisfied, pbHasPrev is changed to false if
 	 	 	 	 	 	 	psCurrent has psPrev that is NULL. Otherwise, pbHasPrev is true.

 Parameters:	psList - the pointer to the list
 	 	 	 	 	 	 	pbHasPrev - the pointer to a boolean

 Returned:   	ERROR_INVALID_LIST if psList is NULL
 	 	 	 	 	 	 	ERROR_NULL_PTR if pbHasPrev is NULL
 	 	 	 	 	 	 	ERROR_EMPTY_LIST if the list is empty
 	 	 	 	 	 	 	ERROR_NO_CURRENT if psCurrent is NULL
 	 	 	 				NO_ERROR if no errors occur
*******************************************************************************/
 ERRORCODE lstHasPrev(ListPtr psList, bool *pbHasPrev)
 {
	 ERRORCODE error = NO_ERROR;
	 bool bIsEmpty;

	 if (NULL == psList)
	 {
		 error = ERROR_INVALID_LIST;
	 }
	 else
	 {
		 if (NULL == pbHasPrev)
		 {
			 error = ERROR_NULL_PTR;
		 }
		 else
		 {
			 error = lstIsEmpty(psList, &bIsEmpty);
			 if (NO_ERROR == error)
			 {
				 if (bIsEmpty)
				 {
					 error = ERROR_EMPTY_LIST;
				 }
				 else
				 {
					 if (NULL == psList->psCurrent)
					 {
						 error = ERROR_NO_CURRENT;
					 }
					 else
					 {
						 if (NULL == psList->psCurrent->psPrev)
						 {
							 *pbHasPrev = false;
						 }
						 else
						 {
							 *pbHasPrev = true;
						 }
					 }
				 }
			 }
		 }
	 }

	 return error;
 }

/*******************************************************************************
 Function:  	lstHasCurrent

 Description: Requires that psList is not NULL, that pbHasCurrent is not NULL,
 	 	 	 	 	 	 	and that the list is not empty. If these requirements are
 	 	 	 	 	 	 	satisfied, pbHasCurrent is changed to false if psCurrent is NULL.
 	 	 	 	 	 	 	Otherwise, pbHasCurrent is true.

 Parameters:	psList - the pointer to the list
 	 	 	 	 	 	 	pbHasCurrent - the pointer to a boolean

 Returned:   	ERROR_INVALID_LIST if psList is NULL
 	 	 	 	 	 	 	ERROR_NULL_PTR if pbHasCurrent is NULL
 	 	 	 	 	 	 	ERROR_EMPTY_LIST if the list is empty
 	 	 	 				NO_ERROR if no errors occur
*******************************************************************************/
 ERRORCODE lstHasCurrent(ListPtr psList, bool *pbHasCurrent)
 {
	 ERRORCODE error = NO_ERROR;
	 bool bIsEmpty;

	 if (NULL == psList)
	 {
		 error = ERROR_INVALID_LIST;
	 }
	 else
	 {
		 if (NULL == pbHasCurrent)
		 {
			 error = ERROR_NULL_PTR;
		 }
		 else
		 {
			 error = lstIsEmpty(psList, &bIsEmpty);
			 if (NO_ERROR == error)
			 {
				 if (bIsEmpty)
				 {
					 error = ERROR_EMPTY_LIST;
				 }
				 else
				 {
					 if (NULL == psList->psCurrent)
					 {
						 *pbHasCurrent = false;
					 }
					 else
					 {
						 *pbHasCurrent = true;
					 }
				 }
			 }
		 }
	 }

	 return error;
 }

 ERRORCODE lstFirstST (ListPtr psList, ST_DATATYPE ** hsSTData)
 {
	 ERRORCODE error = NO_ERROR;
	 bool bIsEmpty;

	 if (NULL == psList)
	 {
		 error = ERROR_INVALID_LIST;
	 }
	 else
	 {
		 error = lstIsEmpty(psList, &bIsEmpty);
		 if (NO_ERROR == error)
		 {
			 if (bIsEmpty)
			 {
				 error = ERROR_EMPTY_LIST;
			 }
			 else
			 {
				 *hsSTData = &(psList->psHead->data.st_data);
				 psList->psCurrent = psList->psHead;
			 }
		 }
	 }

	 return error;
 }
 ERRORCODE lstLastST (ListPtr psList, ST_DATATYPE ** hsSTData)
 {
	 ERRORCODE error = NO_ERROR;
	 bool bIsEmpty;

	 if (NULL == psList)
	 {
		 error = ERROR_INVALID_LIST;
	 }
	 else
	 {
		 if (NULL == psList->psLast)
		 {
			 error = ERROR_NULL_PTR;
		 }
		 else
		 {
			 error = lstIsEmpty(psList, &bIsEmpty);
			 if (NO_ERROR == error)
			 {
				 if (bIsEmpty)
				 {
					 error = ERROR_EMPTY_LIST;
				 }
				 else
				 {
 					 *hsSTData = &(psList->psLast->data.st_data);
 					 psList->psCurrent = psList->psLast;
				 }
			 }
		 }
	 }

	 return error;
 }
 ERRORCODE lstNextST (ListPtr psList, ST_DATATYPE ** hsSTData)
 {
	 ERRORCODE error = NO_ERROR;
	 bool bIsEmpty;

	 if (NULL == psList)
	 {
		 error = ERROR_INVALID_LIST;
	 }
	 else
	 {

		 error = lstIsEmpty(psList, &bIsEmpty);
		 if (NO_ERROR == error)
		 {
			 if (bIsEmpty)
			 {
				 error = ERROR_EMPTY_LIST;
			 }
			 else
			 {
				 if (NULL == psList->psCurrent)
				 {
					 error = ERROR_NO_CURRENT;
				 }
				 else
				 {
					 //*pValue = psList->psCurrent->data;
					 *hsSTData = &(psList->psCurrent->data.st_data);
					 psList->psCurrent = psList->psCurrent->psNext;
				 }
			 }
		 }
	 }
	 return error;
 }
