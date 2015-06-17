/*******************************************************************************
 File name:		  list.h
 Author:        CS, Pacific University
 Date:          10/10/14
 Class:         CS300
 Assignment:    List Implementation
 Purpose:       This file defines the constants, data structures, and function
                prototypes for implementing a list data structure. In essence,
                the list API is defined for other modules. This list has been
                modified for the Priority Queue assignment.
 ******************************************************************************/

#ifndef LIST_H_
#define LIST_H_

#include <stdbool.h>
#include <string.h>
#include "pcc13defines.h"

typedef enum TokenClass {Assignop = 0,
	Orop = 1,
	Andop = 2,
	Equop = 3,
	Relop = 4,
	Addop = 5,
	Mulop = 6,
	Autoop = 7,
	Negop = 8,
	Addressof = 9,
	Left_Paren = 10,
	Right_Paren = 11,
	Left_Brack = 12,
	Right_Brack = 13,
	Constant = 14,
	Identifier = 15,
	Comma = 16,
	Deref = 17,
	Dollar = 18,
	Reserved_Word = 19,
	Left_Brace = 20,
	Right_Brace = 21,
	Semicolon = 22,
	Dummy = 99
}TokenClass;


// List error codes for each function to use

#define NO_ERROR        				0

// list create failed
#define ERROR_NO_LIST_CREATE 	 -1

// user tried to operate on an empty list
#define ERROR_EMPTY_LIST     	 -2

// user tried to add data to a full list
#define ERROR_FULL_LIST        -3

// user tried to peekNext when no next existed
#define ERROR_NO_NEXT          -6

// user tried to peekPrev when no prev existed
#define ERROR_NO_PREV          -7

// user tried to use current when current is NULL
#define ERROR_NO_CURRENT			 -8

// user tried to operate on an invalid list. An invalid
// list may be a NULL ListPtr or contain an invalid value for numElements
#define ERROR_INVALID_LIST		 -9

// user provided a NULL pointer to the function (other than the ListPtr)
#define ERROR_NULL_PTR				 -10


#define NO_CURRENT -100
#define EMPTY_LIST 0
// Possible datatypes for later use

#define CHARACTER_VALUE 0
#define INTEGER_VALUE   1
#define FLOAT_VALUE     2

#define NO_CURRENT -100
#define EMPTY_LIST 0

#define MAX_ID_LENGTH 32

#define ST_MAX_IDENTIFIER_LENGTH 32
#define ST_MAX_KEY_SIZE 100
#define TRUE 1
#define FALSE 0

#define MAX_ERROR_MESSAGE 1024

// User-defined datatypes for easier reading


typedef short int ERRORCODE;

// The user of this data structure is only concerned with
// two data types: List and DATATYPE.  ListElement is an internal
// data structure not to be directly used by the user.
// If the List implementation changes (to static memory, a tree, etc)
// ListElement will change.

// Added for priority queue module

/*********************************************************************
 * 				DATATYPES
 *********************************************************************/
typedef struct Q_DATATYPE					/******    FOR Q    ********/
{
	int qValue;
	int priority;
}Q_DATATYPE;

typedef struct HT_DATATYPE				/******    FOR HT    ********/
{
	int count;
} HT_DATATYPE;

typedef struct ST_DATATYPE				/******    FOR ST    ********/
{
	char pszIdentifier [ST_MAX_IDENTIFIER_LENGTH];
	unsigned int idSize;
	SymbolTableLevel sLevel;
	SymbolTableType sType;
	int address;
	int other;
	bool bInitialized;
}ST_DATATYPE;

typedef struct ER_DATATYPE					/******    FOR ER    ********/
{
	char pszErrorMessage [MAX_ERROR_MESSAGE];
	ErrorCode errorID;
	int lineNum;

}ER_DATATYPE;

typedef struct LEX_DATATYPE 				/******    FOR LEX    ********/
{
	long int value;
	TokenClass TokenClass;
	int lineNum;
	char pszIdentifier [MAX_ID_LENGTH];
}LEX_DATATYPE;


typedef struct CT_DATATYPE					/******    FOR CT    ********/
{
	int constant;
	int stackPosition;
}CT_DATATYPE;

/*********************************************************************
 * 				THE MAIN DATATYPE
 *********************************************************************/
typedef struct DATATYPE
{
	char st_key[ST_MAX_KEY_SIZE];
	unsigned int st_size;
	ST_DATATYPE st_data;
	ER_DATATYPE er_data;
	LEX_DATATYPE lex_data;
	CT_DATATYPE ct_data;
	//Q_DATATYPE data;
	//HT_DATATYPE ht_data;

	// maintained so that the list driver still works with list
	union
  {
  	char charValue;
  	unsigned int intValue;
    float floatValue;
  };
  unsigned short whichOne;
} DATATYPE;

/*********************************************************************
 * 				ListElement
 *********************************************************************/

typedef struct ListElement* ListElementPtr;
typedef struct ListElement
{
	DATATYPE data;
	ListElementPtr psNext;
	ListElementPtr psPrev;
} ListElement;


// A list is a dynamic data structure where the current pointer and number
// of elements are maintained at all times

typedef struct List* ListPtr;
typedef struct List
{
	ListElementPtr psHead;
	ListElementPtr psLast;
	ListElementPtr psCurrent;
	int numElements;
} List;

/******************************************************************************
*										Allocation and Deallocation
*****************************************************************************/
ERRORCODE lstCreate (ListPtr );
// results: If list L can be created, then L exists and is empty returning
//					NO_ERROR; otherwise, ERROR_NO_LIST_CREATE is returned

ERRORCODE lstDispose (ListPtr );
// results: If list L can be disposed, then L no longer exists and is empty
//				  returning NO_ERROR; otherwise, ERROR_INVALID_LIST is returned

/******************************************************************************
*									Checking number of elements in list
*****************************************************************************/
ERRORCODE lstSize (ListPtr, int *);
// results: Returns the number of elements in the list
// error code priority: ERROR_INVALID_LIST, ERROR_NULL_PTR

ERRORCODE lstIsFull (ListPtr, bool *);
// results: If list is full, return true; otherwise, return false
// error code priority: ERROR_INVALID_LIST, ERROR_NULL_PTR

ERRORCODE lstIsEmpty (ListPtr, bool *);
// results: If list is empty, return true; otherwise, return false
// error code priority: ERROR_INVALID_LIST, ERROR_NULL_PTR

/******************************************************************************
*													Peek Operations
*****************************************************************************/
ERRORCODE lstPeek (ListPtr, DATATYPE *);
// requires: List is not empty
// results: The value of the current element is returned
// IMPORTANT: Do not change current
// error code priority: ERROR_INVALID_LIST, ERROR_NULL_PTR, ERROR_EMPTY_LIST,
//											ERROR_NO_CURRENT

ERRORCODE lstPeekPrev (ListPtr, DATATYPE *);
// requires: List contains two or more elements and current is not the first
// results: The data value of current's predecessor is returned
// IMPORTANT: Do not change current
// error code priority: ERROR_INVALID_LIST, ERROR_NULL_PTR, ERROR_EMPTY_LIST,
//											ERROR_NO_CURRENT, ERROR_NO_PREV

ERRORCODE lstPeekNext (ListPtr, DATATYPE *);
// requires: List contains two or more elements and current is not the last
// results: The data value of current's successor is returned
// IMPORTANT: Do not change current
// error code priority: ERROR_INVALID_LIST, ERROR_NULL_PTR, ERROR_EMPTY_LIST,
//											ERROR_NO_CURRENT, ERROR_NO_NEXT

/******************************************************************************
*							Retrieving values and updating current
*****************************************************************************/
ERRORCODE lstFirst(ListPtr, DATATYPE *);
// requires: List is not empty
// results: The value of the first element is returned
// IMPORTANT: Current is changed to first if it exists
// error code priority: ERROR_INVALID_LIST, ERROR_NULL_PTR, ERROR_EMPTY_LIST

 ERRORCODE lstLast(ListPtr, DATATYPE *);
// requires: List is not empty
// results: The value of the last element is returned
// IMPORTANT: Current is changed to last if it exists
// error code priority: ERROR_INVALID_LIST, ERROR_NULL_PTR, ERROR_EMPTY_LIST

 ERRORCODE lstNext(ListPtr, DATATYPE *);
// requires: List is not empty, and current is not NULL
// results: The value of the current element is returned
// IMPORTANT: Current is changed to the successor of the current element
// error code priority: ERROR_INVALID_LIST, ERROR_NULL_PTR, ERROR_EMPTY_LIST,
//											ERROR_NO_CURRENT

ERRORCODE lstPrev(ListPtr, DATATYPE *);
// requires: List is not empty, and current is not NULL
// results: The value of the current element is returned
// IMPORTANT: Current is changed to previous if it exists
// error code priority: ERROR_INVALID_LIST, ERROR_NULL_PTR, ERROR_EMPTY_LIST
//											ERROR_NO_CURRENT

ERRORCODE lstCurrent(ListPtr, DATATYPE *);
// requires: List is not empty, and current is not NULL
// results: The value of the current element is returned
// IMPORTANT: Current is not changed
// error code priority: ERROR_INVALID_LIST, ERROR_NULL_PTR, ERROR_EMPTY_LIST
//											ERROR_NO_CURRENT

/******************************************************************************
*									Insertion, Deletion, and Updating
*****************************************************************************/
ERRORCODE lstDeleteCurrent (ListPtr, DATATYPE *);
// requires: List is not empty
// results: The current element is deleted and its
// successor and predecessor become each
// others successor and predecessor. If the
// deleted element had a predecessor, then
// make it the new current element; otherwise,
// make the first element current if it exists.
// The deleted element is returned through the argument list.
// error code priority: ERROR_INVALID_LIST, ERROR_NULL_PTR, ERROR_EMPTY_LIST
//											ERROR_NO_CURRENT

 ERRORCODE lstInsertAfter (ListPtr, DATATYPE);
// requires: List is not full
// results: if the list is not empty, insert the new
// element as the successor of the current
// element and make the inserted element the
// current element; otherwise, insert element
// and make it current.
// error code priority: ERROR_INVALID_LIST, ERROR_NO_CURRENT

 ERRORCODE lstInsertBefore (ListPtr, DATATYPE);
// requires: List is not full
// results: If the list is not empty, insert the new
// element as the predecessor of the current
// element and make the inserted element the
// current element; otherwise, insert element
// and make it current.
// error code priority: ERROR_INVALID_LIST, ERROR_NO_CURRENT

 ERRORCODE lstUpdateCurrent (ListPtr, DATATYPE);
// requires: List is not empty
// results: The value of ListElement is copied into the current element
// error code priority: ERROR_INVALID_LIST, ERROR_EMPTY_LIST, ERROR_NO_CURRENT

/******************************************************************************
*												List Testing
*****************************************************************************/

 ERRORCODE lstHasCurrent (ListPtr, bool *);
 // results: Returns true if the current node is not NULL; otherwise, false
 //					 is returned
 // error code priority: ERROR_INVALID_LIST, ERROR_NULL_PTR, ERROR_EMPTY_LIST
 //											ERROR_NO_CURRENT

ERRORCODE lstHasNext (ListPtr, bool *);
 // results: Returns true if the current node has a successor; otherwise, false
 //					 is returned
 // error code priority: ERROR_INVALID_LIST, ERROR_NULL_PTR, ERROR_EMPTY_LIST
 //											ERROR_NO_CURRENT

 ERRORCODE lstHasPrev(ListPtr, bool *);
 // results: Returns true if the current node has a predecessor; otherwise,
 //					 false is returned
 // error code priority: ERROR_INVALID_LIST, ERROR_NULL_PTR, ERROR_EMPTY_LIST
 //											ERROR_NO_CURRENT


 //FOR ST
 ERRORCODE lstFirstST (ListPtr, ST_DATATYPE **);
 ERRORCODE lstLastST (ListPtr, ST_DATATYPE **);
 ERRORCODE lstNextST (ListPtr, ST_DATATYPE **);


#endif /* LIST_H_ */

