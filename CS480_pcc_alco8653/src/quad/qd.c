/*******************************************************************************
 File name:     qd.c
 Author:				Brianna Alcoran
 Date:          3/20/15
 Class:         CS480
 Assignment:    Compiler Project written in C
 Purpose:       QuadFile for the Compiler (implementation)
 Modifications:
*******************************************************************************/

#include "../include/qd.h"
#include "stdio.h"

/*******************************************************************************
 Function:		quadCreate

 Description:	Creates the stack of quads

 Parameters:	hsQD - handle to the quads
							psER - pointer to the error handler
							psRTS - pointer to the runtime stack
							fileName - name of the file to output the quads

 Returned:		error - a QD_ERROR
*******************************************************************************/
QD_ERROR quadCreate (QuadFilePtr* hsQD, ErrorHandlerPtr psER,
											RuntimeStackPtr psRTS, char* fileName)
{
	QD_ERROR error = QD_NO_ERROR;

	(*hsQD) = (QuadFilePtr) malloc (sizeof (QuadFile));

	if (NULL == *hsQD || NULL == psER || NULL == psRTS || NULL == fileName)
	{
		error = QD_ERROR_NO_CREATE;
	}
	else
	{
		memset((*hsQD)->quads, '\0', sizeof((*hsQD)->quads)); //set array
		(*hsQD)->psER = psER;
		(*hsQD)->psRTS = psRTS;
		(*hsQD)->size = 0;
		(*hsQD)->pFile = fopen(fileName, "w");
		(*hsQD)->bPrintToScreen = false;
		if (!(*hsQD)->pFile)
		{
			free(*hsQD);
			*hsQD = NULL;
			error = QD_ERROR_FILE_OPEN;
			//possibly add an ER error
		}
	}


	return error;
}

/*******************************************************************************
 Function:		quadDispose

 Description:	Disposes of the quads by closing the quad file and freeing
 	 	 	 	 	 	  allocated space

 Parameters:	hsQD - handle to the quads

 Returned:		error - a QD_ERROR
*******************************************************************************/
QD_ERROR quadDispose (QuadFilePtr* hsQD)
{
	QD_ERROR error = QD_NO_ERROR;

	if (NULL == *hsQD)
	{
		error = QD_ERROR_NULL_PTR;
	}
	else
	{
		fclose((*hsQD)->pFile);		// close the file
		free(*hsQD);							// free allocated space
		*hsQD = NULL;
	}

	return error;
}

/*******************************************************************************
 Function:		quadClear

 Description:	clears the stack of quads

 Parameters:	psQ - pointer to the quads

 Returned:		error - a QD_ERROR
*******************************************************************************/
QD_ERROR quadClear (QuadFilePtr psQD)
{
	QD_ERROR error = QD_NO_ERROR;

	if (NULL == psQD)
	{
		error = QD_ERROR_NULL_PTR;
	}
	else
	{
		memset((psQD)->quads, '\0', sizeof((psQD)->quads));
		psQD->size = 0;
	}

	return error;
}

/*******************************************************************************
 Function:		quadAdd

 Description:	adds a quad to the stack of quads

 Parameters:	psQD - pointer to the stack of quads
							theQuad - the quad to be added to the stack if it is not full

 Returned:		error - a QD_ERROR
*******************************************************************************/
QD_ERROR quadAdd (QuadFilePtr psQD, Quad theQuad)
{
	QD_ERROR error = QD_NO_ERROR;
	bool bQuadFull = false;

	if (NULL == psQD)
	{
		error = QD_ERROR_NULL_PTR;
	}
	else
	{
		quadIsFull (psQD, &bQuadFull);
		if (bQuadFull)
		{
			error = QD_ERROR_FULL;
		}
		else
		{
			theQuad.QuadFilePos = psQD->size;
			psQD->quads[psQD->size] = theQuad;
			++(psQD->size);
		}
	}

	return error;
}

/*******************************************************************************
 Function:		quadPop

 Description:	pops the last quad off of the quad stack

 Parameters:	psQD - pointer to the stack of quads
							theQuad	- the quad that was popped

 Returned:		error - a QD_ERROR
*******************************************************************************/

QD_ERROR quadPop (QuadFilePtr psQD, QuadPtr theQuad)
{
	QD_ERROR error = QD_NO_ERROR;
	bool bQuadIsEmpty = false;

	if (NULL == psQD)
	{
		error = QD_ERROR_NULL_PTR;
	}
	else
	{
		quadIsEmpty (psQD, &bQuadIsEmpty);
		if (bQuadIsEmpty)
		{
			error = QD_ERROR_EMPTY;
		}
		else
		{
			*theQuad = psQD->quads[psQD->size - 1];
			memset (&(psQD->quads[psQD->size]), '\0', sizeof(Quad));
			--(psQD->size);
		}
	}
	return error;
}

/*******************************************************************************
 Function:		quadMerge

 Description:	merges the 2nd quad file with the 1st

 Parameters:	psMainQuadFile - quad file that the quads will be added to
							psQuadToAdd - quad file that contains the quads to merge

 Returned:		error - a QD_ERROR
*******************************************************************************/
QD_ERROR quadMerge (QuadFilePtr psMainQuadFile, QuadFilePtr psQuadToAdd)
{
	QD_ERROR error = QD_NO_ERROR;
	int i, sizeOfQuad;
	getQuadFileSize (psQuadToAdd, &sizeOfQuad);

	for (i = sizeOfQuad - 1; i >= 0; --i)
	{
		quadAdd (psMainQuadFile, psQuadToAdd->quads[i]);
	}

	return error;
}

/*******************************************************************************
 Function:		setQuadPrintToScreen

 Description:	sets boolean to print to the screen or not

 Parameters:	psQD - pointer to the quad
							bPrint - boolean indicating to print to screen or not

 Returned:		error - a QD_ERROR
*******************************************************************************/
QD_ERROR setQuadPrintToScreen (QuadFilePtr psQD, bool bPrint)
{
	QD_ERROR error = QD_NO_ERROR;

	psQD->bPrintToScreen = bPrint;

	return error;
}

/*******************************************************************************
 Function:		quadIsFull

 Description:	checks to see if the stack of quads is filled

 Parameters:	psQD - pointer to the stack of quads
							pbIsFull	- boolean indicating if the stack is full or not

 Returned:		error - a QD_ERROR
*******************************************************************************/
QD_ERROR quadIsFull (QuadFilePtr psQD, bool *pbIsFull)
{
	QD_ERROR error = QD_NO_ERROR;

	if (NULL == psQD)
	{
		error = QD_ERROR_NULL_PTR;
	}
	else
	{
		if (MAX_QUAD_FILE <= psQD->size)
		{
			*pbIsFull = true;
		}
		else
		{
			*pbIsFull = false;
		}
	}

	return error;
}

/*******************************************************************************
 Function:		quadIsEmpty

 Description:	checks to see if the stack of quads is empty

 Parameters:	psQD - pointer to the stack of quads
							pbIsFull	- boolean indicating if the stack is empty or not

 Returned:		error - a QD_ERROR
*******************************************************************************/
QD_ERROR quadIsEmpty (QuadFilePtr psQD, bool *pbIsEmpty)
{
	QD_ERROR error = QD_NO_ERROR;

	if (NULL == psQD)
	{
		error = QD_ERROR_NULL_PTR;
	}
	else
	{
		if (MAX_QUAD_FILE <= psQD->size)
		{
			*pbIsEmpty = true;
		}
		else
		{
			*pbIsEmpty = false;
		}
	}

	return error;
}


/*******************************************************************************
*										Getters & Setters
*******************************************************************************/

/*******************************************************************************
 Function:		setQuad

 Description:	initializes elements of a quad

 Parameters:	psQ - pointer to the quad
 	 	 	 	 	 	 	level - level of the quad (reserved, global, or local)
 	 	 	 	 	 	 	opcode - opcode of the quad (see pcc13defines.h)
 	 	 	 	 	 	 	am1,2,3 - addressing modes (see pcc13defines.h)
 	 	 	 	 	 	 	opd1,2,3 - operands

 Returned:		error - a QD_ERROR
*******************************************************************************/
void setQuad (QuadPtr psQ, SymbolTableLevel level, QuadOpcode opcode,
							QuadAddressingMode am1,	int opd1,
							QuadAddressingMode am2, int opd2,
							QuadAddressingMode am3, int opd3)
{
	psQ->level = level;
	psQ->opcode = opcode;
	psQ->am1 = am1;
	psQ->opd1 = opd1;
	psQ->am2 = am2;
	psQ->opd2 = opd2;
	psQ->am3 = am3;
	psQ->opd3 = opd3;
}
/*******************************************************************************
 Function:		getQuad

 Description:	returns a quad in the quad stack

 Parameters:	psQD - pointer to the stack of quads
							whichQuad - index of the quad in the stack to return
							psQuad - the quad to be returned

 Returned:		error - a QD_ERROR
*******************************************************************************/
QD_ERROR getQuad (QuadFilePtr psQD, int whichQuad, QuadPtr * hsQuad)
{
	QD_ERROR error = QD_NO_ERROR;

	if (NULL == psQD || psQD->size <= whichQuad)
	{
		error = QD_ERROR_NULL_PTR;
	}
	else
	{
		*hsQuad = &(psQD->quads[whichQuad]);
	}

	return error;
}

/*******************************************************************************
 Function:		getLastQuad

 Description:	returns a quad in the quad stack

 Parameters:	psQD - pointer to the stack of quads
							whichQuad - index of the quad in the stack to return
							psQuad - the quad to be returned

 Returned:		error - a QD_ERROR
*******************************************************************************/
QD_ERROR getLastQuad (QuadFilePtr psQD, QuadPtr * hsQuad)
{
	QD_ERROR error = QD_NO_ERROR;

	if (NULL == psQD)
	{
		error = QD_ERROR_NULL_PTR;
	}
	else
	{
		if (0 == psQD->size)
		{
			error = QD_ERROR_EMPTY;
			*hsQuad = NULL;
		}
		else
		{
			*hsQuad = &(psQD->quads[(psQD->size)-1]);
		}

	}

	return error;
}

/*******************************************************************************
 Function:		getLastFuncQuad

 Description:	returns the most recent quad with prog begin opcode

 Parameters:	psQD - pointer to the stack of quads
							hsQuad - handle to the quad to be returned

 Returned:		error - a QD_ERROR
*******************************************************************************/
QD_ERROR getLastFuncQuad (QuadFilePtr psQD, QuadPtr * hsQuad)
{
	QD_ERROR error = QD_NO_ERROR;

	*hsQuad = NULL;

	if (NULL == psQD)
	{
		error = QD_ERROR_NULL_PTR;
	}
	else
	{
		if (0 == psQD->size)
		{
			error = QD_ERROR_EMPTY;
		}
		else
		{
			int i;
			for (i = (psQD->size - 1); i >= 0; --i)
			{
				if (psQD->quads[i].opcode == OP_FUNCBEGIN)
				{
					*hsQuad = &(psQD->quads[i]);
					break;
				}
			}
		}

		if (NULL == *hsQuad)
		{
			error = QD_ERROR_NULL_PTR;
		}

	}

	return error;
}

/*******************************************************************************
 Function:		getLastOPQuad

 Description:	returns the most recent quad with prog begin opcode

 Parameters:	psQD - pointer to the stack of quads
							hsQuad - handle to the quad to be returned

 Returned:		error - a QD_ERROR
*******************************************************************************/
QD_ERROR getLastOPQuad (QuadFilePtr psQD, QuadOpcode theOP, QuadPtr *hsQuad)
{
	QD_ERROR error = QD_NO_ERROR;

	*hsQuad = NULL;

	if (NULL == psQD)
	{
		error = QD_ERROR_NULL_PTR;
	}
	else
	{
		if (0 == psQD->size)
		{
			error = QD_ERROR_EMPTY;
		}
		else
		{
			int i;
			for (i = (psQD->size - 1); i >= 0; --i)
			{
				if (psQD->quads[i].opcode == theOP)
				{
					*hsQuad = &(psQD->quads[i]);
					break;
				}
			}
		}

		if (NULL == *hsQuad)
		{
			error = QD_ERROR_NULL_PTR;
		}

	}

	return error;
}
/*******************************************************************************
 Function:		getSize

 Description:	returns the size of the quad stack

 Parameters:	psQD - pointer to the quad stack
							pSize - int * to the size of the quad stack

 Returned:		error - a QD_ERROR
*******************************************************************************/
QD_ERROR getQuadFileSize (QuadFilePtr psQD, int * pSize)
{
	QD_ERROR error = QD_NO_ERROR;

	if (NULL == psQD || NULL == pSize)
	{
		error = QD_ERROR_NULL_PTR;
	}
	else
	{
		*pSize = psQD->size;
	}

	return error;
}

/*******************************************************************************
 Function:		quadGetSTLevel

 Description:	returns the ST Level of the quad

 Parameters:	psQ - pointer to the quad

 Returned:		level
*******************************************************************************/
SymbolTableLevel quadGetSTlevel (QuadPtr psQ)
{
	return psQ->level;
}

/*******************************************************************************
 Function:		quadGetOpcode

 Description:	returns the opcode of the quad

 Parameters:	psQ - pointer to the quad

 Returned:		opcode
*******************************************************************************/
QuadOpcode quadGetOpcode (QuadPtr psQ)
{
	return psQ->opcode;
}

/*******************************************************************************
 Function:		setQuadSTLevel

 Description:	sets the level of the quad to the Symbol Table level passed in

 Parameters:	psQ - pointer to the quad
							STLevel - the symbol table level of the quad

 Returned:		none
*******************************************************************************/
void setQuadSTLevel (QuadPtr *hsQ, SymbolTableLevel STLevel)
{
	(*hsQ)->level = STLevel;
}

/*******************************************************************************
 Function:		setQuadOpcode

 Description:	sets the opcode of the quad to the QuadOpcode passed in

 Parameters:	psQ - pointer to the quad
							opcode - the opcode of the quad

 Returned:		none
*******************************************************************************/
void setQuadOpcode (QuadPtr *hsQ, QuadOpcode opcode)
{
	(*hsQ)->opcode = opcode;
}

/*******************************************************************************
 Function:		setQuadAM

 Description:	sets the addressing mode of the quad (am1, am2, or am3)

 Parameters:	psQ - pointer to the quad
							whichAM - which addressing mode to set
							am - the new addressing mode of the quad

 Returned:		none
*******************************************************************************/
void setQuadAM (QuadPtr *hsQ, int whichAM, QuadAddressingMode am)
{
	switch (whichAM)
	{
	case AM1: (*hsQ)->am1 = am;
		break;
	case AM2: (*hsQ)->am2 = am;
		break;
	case AM3: (*hsQ)->am3 = am;
		break;
	}
}

/*******************************************************************************
 Function:		setQuadOPD

 Description:	sets the operand of the quad (opd1, opd2, or opd3)

 Parameters:	psQ - pointer to the quad
							whichOPD - which operand to set
							opd - the new operand of the quad

 Returned:		error - a QD_ERROR
*******************************************************************************/
void setQuadOPD (QuadPtr *hsQ, int whichOPD, int opd)
{
	switch (whichOPD)
	{
	case OP1: (*hsQ)->opd1 = opd;
		break;
	case OP2: (*hsQ)->opd2 = opd;
		break;
	case OP3: (*hsQ)->opd3 = opd;
		break;
	}
}

/*******************************************************************************
*										Printing
*******************************************************************************/

/*******************************************************************************
 Function:		printQuadToScreen

 Description:	prints a quad to the screen

 Parameters:	sQ - the quad to be printed

 Returned:		none
*******************************************************************************/
void printQuadToScreen (Quad sQ)
{
	printf ("%d %2d %d %6d %d %6d %d %6d \n", sQ.level, sQ.opcode, sQ.am1, sQ.opd1,
																			sQ.am2, sQ.opd2, sQ.am3, sQ.opd3);
}

/*******************************************************************************
 Function:		printQuad

 Description:	prints a quad to the screen and to a file

 Parameters:	sQ - the quad to be printed
 	 	 	 	 	 	 	theFile - pointer to the file where the quad will also be printed

 Returned:		none
*******************************************************************************/
void printQuad (Quad sQ, FILE * theFile)
{
	if (NULL != theFile)
	{
		fprintf (theFile, "%d %2d %6d %2d %6d %2d %6d %2d \n", sQ.level, sQ.opcode,
														sQ.am1, sQ.opd1, sQ.am2, sQ.opd2, sQ.am3, sQ.opd3);
	}
}

/*******************************************************************************
 Function:		printQuadFile

 Description: prints the entire quad stack to the screen and to a file

 Parameters:	psQD - pointer to the stack of quads

 Returned:		none
*******************************************************************************/
void printQuadFile (QuadFilePtr psQD)
{
	int i = 0;

	if (NULL != psQD)
	{
		for (i = 0; i < psQD->size; ++i)
		{
			if (psQD->bPrintToScreen)
			{
				printQuadToScreen (psQD->quads[i]);
			}
			printQuad (psQD->quads[i], psQD->pFile); //prints to the screen and file
		}
		if (psQD->bPrintToScreen)
		{
			rtsDisplay (psQD->psRTS);
		}
		rtsDisplayToFile (psQD->psRTS, psQD->pFile);
	}
}
