/*******************************************************************************
 File name:     lex.c
 Author:				Brianna Alcoran
 Date:          2/13/15
 Class:         CS480
 Assignment:    Compiler Project written in C
 Purpose:       Lexical Analyzer for the Compiler (implementation)
 Modifications:
*******************************************************************************/

#include "../include/lex.h"
#include <limits.h>

static bool comparePrev(LexerPtr psLex, int value, TokenClass sTokenClass);

/*******************************************************************************
 Function:		lexCreate

 Description:	create a Lexer

 Parameters:	hsLexer	-	handle to the Lexer
							psST	- pointer to the symbol table
 	 	 	 	 	 	 	psER	-	pointer to the error handler
 	 	 	 	 	 	 	psCT	-	pointer to the constant table
 	 	 	 	 	 	 	psRTS	-	pointer to the runtime stack

 Returned:		error - LEX_ERROR
*******************************************************************************/
LEX_ERROR lexCreate (LexerPtr *hsLex, SymbolTablePtr psST, ErrorHandlerPtr psER,
		ConstantTablePtr psCT, RuntimeStackPtr psRTS, FILE *pFile)
{
	LEX_ERROR error = LEX_NO_ERROR;

	*hsLex = (LexerPtr) malloc (sizeof(Lexer));

	if (NULL == *hsLex || NULL == psST || NULL == psER || NULL == psCT ||
			NULL == psRTS || NULL == pFile)
	{
		error = LEX_ERROR_NO_CREATE;
	}
	else
	{
		(*hsLex)->bufferIndex = 0;
		(*hsLex)->currentLine = -1;
		(*hsLex)->debugOn = false;
		(*hsLex)->psST = psST;
		(*hsLex)->psER = psER;
		(*hsLex)->psCT = psCT;
		(*hsLex)->psRTS = psRTS;
		(*hsLex)->pFile = pFile;
		(*hsLex)->currentFileLine = 1;
		lexCreateToken(-1,-1,-1,"",&((*hsLex)->sPrevToken));
	}

	return error;
}

/*******************************************************************************
 Function:		lexDispose

 Description:	dispose of an Error Handler

 Parameters:	hsER	- handle to the Error Handler

 Returned:		error - a LEX_ERROR
*******************************************************************************/
LEX_ERROR lexDispose(LexerPtr *hsLex)
{
	LEX_ERROR error = LEX_NO_ERROR;
	if (NULL == *hsLex)
	{
		error = LEX_ERROR_NULL_PTR;
	}
	else
	{
		free(*hsLex);
		*hsLex = NULL;
	}

	return error;
}

/*******************************************************************************
 Function:		lexGetToken

 Description:	reads a file, finds the next token, and returns it through the
 	 	 	 	 	 	 	argument list

 Parameters:	psLex	- pointer to the lexer
							pFile	- pointer to the file
 	 	 	 	 	 	 	sLexData - token to be added
 	 	 	 	 	 	 	lineNum	-	current line number in the file
 	 	 	 	 	 	 	psST	- pointer to the symbol table
 	 	 	 	 	 	 	psER	-	pointer to the error handler
 	 	 	 	 	 	 	psCT	-	pointer to the constant table
 	 	 	 	 	 	 	psRTS	-	pointer to the runtime stack

 Returned:		an error if one is found
*******************************************************************************/
LEX_ERROR lexGetToken (LexerPtr psLexer, LEX_DATATYPE *psLexData)
{
	LEX_ERROR error = LEX_NO_ERROR;
	LEX_DATATYPE sTheNewData;
	char ch;
	char lexeme[BUFFER_SIZE];
	bool bTokenFound = false, bDoneWithBuffer = false, bDoneTokenize = false;
	int index = 0, state = 0;

	memset(lexeme, '\0', sizeof(lexeme));
	memset(sTheNewData.pszIdentifier, '\0', sizeof (sTheNewData.pszIdentifier));

	if (NULL == psLexer)
	{
		return error = LEX_ERROR_NULL_PTR;
	}

	while (!bTokenFound && !bDoneTokenize)
	{
		switch (state)
		{
			case START: /* deals with getting new lines from file & white space */
				if ((psLexer->currentLine) != (psLexer->currentFileLine))
				{
					psLexer->currentLine = psLexer->currentFileLine;
					memset(psLexer->buffer, '\0', sizeof(psLexer->buffer));
					fgets (psLexer->buffer, sizeof(psLexer->buffer), psLexer->pFile);
					psLexer->bufferIndex = 0;
					if (psLexer->debugOn)		/* print line if debug on */
					{
						printLine(psLexer);
					}
				} /* got new line from file */

				ch = psLexer->buffer[psLexer->bufferIndex]; /*  get lookahead  */
				lexeme[index] = ch; /* start lexeme */

				if (bDoneWithBuffer && feof(psLexer->pFile)) /* check if we are done tokenizing */
				{
					bDoneTokenize = true;
					break;
				}
				if (ch == '\0' || (int)(strlen(psLexer->buffer) == 0))
				{
					bDoneWithBuffer = true;
					break;
				}
				if (isWhiteSpace(ch) && ch != '\n')
				{
					++(psLexer->bufferIndex);
					state = START;
				}
				else if (ch == '\n')
				{
					++(psLexer->currentFileLine);
					state = START;
				}
				else if (ISDIGIT(ch))	{	state = CONSTANT;	}
				else if (ch == '=') 	{	state = EQUAL; }
				else if (ch == '!') 	{ state = NOT; }
				else if (ch == '<') 	{ state = LESS_THAN; }
				else if (ch == '>') 	{ state = GREATER_THAN; }
				else if (ch == '*') 	{ state = MULTIPLY; }
				else if (ch == '/') 	{ state = DIVIDE; }
				else if (ch == '%') 	{ state = MOD; }
				else if (ch == '+') 	{ state = ADD; }
				else if (ch == '-') 	{ state = SUBTRACT; }
				else if (ch == '&') 	{ state = AMPERSAND; }
				else if (ch == '|') 	{ state = PIPE; }
				else if (ch == ';') 	{ state = SEMICOLON; }
				else if (ch == ',') 	{ state = COMMA; }
				else if (ch == '(') 	{ state = PAREN_L; }
				else if (ch == ')') 	{ state = PAREN_R; }
				else if (ch == '[') 	{ state = BRACK_L; }
				else if (ch == ']') 	{ state = BRACK_R; }
				else if (ch == '{') 	{ state = BRACE_L; }
				else if (ch == '}') 	{ state = BRACE_R; }
				else if (isBadChar(ch)) { state = BAD_CHARACTER; }
				else if (ISLETTER (ch)) { state = 	ID; }
				break;

			case CONSTANT: 				/*************      constants      *************/
				while (ISDIGIT(ch))
				{
					lexeme[index] = ch;
					++(psLexer->bufferIndex);
					++index;
					ch = psLexer->buffer[psLexer->bufferIndex];
				}
				int pos = -1;										/*initialize position */
				int constant = atoi(lexeme);		/*convert the lexeme into an integer*/
				bool bFoundConstant = false;
				CT_DATATYPE sTheConstant;
				RTS_ERROR rtsError = RTS_NO_ERROR;

				if (false) //no longer have to make sure that an invalid id is [num][letter]+
				{
					while (ISLETTER(ch) && index < MAX_ID_LENGTH)
					{
						lexeme[index] = ch;
						++(psLexer->bufferIndex);
						++index;
						ch = psLexer->buffer[psLexer->bufferIndex];
					}
					erAddError (psLexer->psER, psLexer->currentLine, INVALID_ID, lexeme);
					memset (lexeme, '\0', sizeof(lexeme));
					index = 0;
					state = START;
				}
				else
				{
					if (constant < INT_MAX && index < 10 )
					{
						ctSearch (psLexer->psCT, constant, &bFoundConstant, &sTheConstant);
						if (!bFoundConstant)
						{
							rtsError = rtsAdd (psLexer->psRTS, constant);
							if (rtsError == RTS_NO_ERROR)
							{
								pos = rtsGetAvailablePos(psLexer->psRTS) - 1;
								ctAddConstant (psLexer->psCT, constant, pos);
								//value of the constant token is its position in the RTS
								lexCreateToken (pos, Constant, psLexer->currentFileLine, lexeme, &sTheNewData);
								--(psLexer->bufferIndex);
								bTokenFound = true; //dont want to print tho if full (not a valid token anymore
							}
							else if (rtsError == RTS_ERROR_FULL)
							{
								erAddError (psLexer->psER, psLexer->currentLine, OUT_OF_MEMORY, "");
								memset(lexeme, '\0', sizeof(lexeme));
								index = 0;
								state = START;
							}
						}
						else
						{
							pos = sTheConstant.stackPosition;
							lexCreateToken (pos, Constant, psLexer->currentFileLine, lexeme,
									&sTheNewData);
							--(psLexer->bufferIndex);
							bTokenFound = true;
						}
					}
					else
					{
						erAddError (psLexer->psER, psLexer->currentLine, CONST_TOO_LONG, lexeme);
						memset(lexeme, '\0', sizeof(lexeme));
						index = 0;
						state = START;
					}
				}
				break;
			case EQUAL: 		/*      = or ==      */
				++index;
				++(psLexer->bufferIndex);
				ch = psLexer->buffer[psLexer->bufferIndex];
				if (ch == '=')
				{
					lexeme[index] = ch;
					lexCreateToken (REL_EQUAL_EQUAL, Equop, psLexer->currentFileLine, lexeme,
							&sTheNewData);
				}
				else
				{
					lexCreateToken (0, Assignop, psLexer->currentFileLine, lexeme,
							&sTheNewData);
					--(psLexer->bufferIndex);
				}
				bTokenFound = true;
				break;
			case NOT: 	/*    != (if not, bad char)    */
				++index;
				++(psLexer->bufferIndex);
				ch = psLexer->buffer[psLexer->bufferIndex];
				if (ch == '=')
				{
					lexeme[index] = ch;
					lexCreateToken (REL_NOT_EQUAL, Equop, psLexer->currentFileLine, lexeme,
							&sTheNewData);
					bTokenFound = true;
				}
				else
				{
					erAddError (psLexer->psER, psLexer->currentLine, INVALID_TOKEN, lexeme);
					memset(lexeme, '\0', sizeof(lexeme));
					index = 0;
					state = START; /*	return to the START to try again */
				}
				break;
			case LESS_THAN:		/*		< or <=		**/
				++index;
				++(psLexer->bufferIndex);
				ch = psLexer->buffer[psLexer->bufferIndex];
				if (ch == '=')
				{
					lexeme[index] = ch;
					lexCreateToken (REL_LESS_EQUAL, Relop, psLexer->currentFileLine, lexeme,
							&sTheNewData);
				}
				else
				{
					lexCreateToken (REL_LESS_THAN, Relop, psLexer->currentFileLine, lexeme,
							&sTheNewData);
					--(psLexer->bufferIndex);
				}
				bTokenFound = true;
				break;
			case GREATER_THAN:		/*		> or >=		*/
				++index;
				++(psLexer->bufferIndex);
				ch = psLexer->buffer[psLexer->bufferIndex];
				if (ch == '=')
				{
					lexeme[index] = ch;
					lexCreateToken (REL_GREATER_EQUAL, Relop, psLexer->currentFileLine,
							lexeme, &sTheNewData);
				}
				else
				{
					lexCreateToken (REL_GREATER_THAN, Relop, psLexer->currentFileLine,
							lexeme, &sTheNewData);
					--(psLexer->bufferIndex);
				}
				bTokenFound = true;
				break;
			case MULTIPLY:		/*		'*'		*/

				if (comparePrev (psLexer, 0, Identifier) ||
						comparePrev (psLexer, 0, Constant) ||
						comparePrev (psLexer, 0, Right_Paren) ||
						comparePrev (psLexer, 0, Right_Brack) ||
						comparePrev (psLexer, AUTO_DECR, Autoop) ||
						comparePrev (psLexer, AUTO_INCR, Autoop) )
				{
					lexCreateToken (MUL_TIMES, Mulop, psLexer->currentFileLine, lexeme,
							&sTheNewData);
				}
				else
				{
					lexCreateToken (0, Deref, psLexer->currentFileLine, lexeme,
							&sTheNewData);
				}

				bTokenFound = true;
				break;
			case DIVIDE:		/*		/		(or eat comment)		*/
				++(psLexer->bufferIndex);
				ch = psLexer->buffer[psLexer->bufferIndex];
				if (ch == '*') //got a comment
				{
					++(psLexer->bufferIndex);
					if (NO_ERROR == eatComment(psLexer, psLexer->psER, psLexer->pFile,
							&(psLexer->currentFileLine)))
					{
						index = 0;
						state = START;
					}
					else
					{
						erAddError (psLexer->psER, psLexer->currentLine, READ_PAST_EOF, "");
						bDoneTokenize = true;
						break;
					}
				}
				else
				{
					lexCreateToken (MUL_DIV, Mulop, psLexer->currentFileLine, lexeme,
							&sTheNewData);
					--(psLexer->bufferIndex);
					bTokenFound = true;
				}
				break;
			case MOD:			/*		%		*/
				lexCreateToken (MUL_MOD, Mulop, psLexer->currentFileLine, lexeme,
						&sTheNewData);
				bTokenFound = true;
				break;
			case ADD:		/*		+  or  ++		*/
				++index;
				++(psLexer->bufferIndex);
				ch = psLexer->buffer[psLexer->bufferIndex];
				if (ch == '+')
				{
					lexeme[index] = ch;
					lexCreateToken (AUTO_INCR, Autoop, psLexer->currentFileLine, lexeme,
							&sTheNewData);
				}
				else
				{
					lexCreateToken (ADD_PLUS, Addop, psLexer->currentFileLine, lexeme,
							&sTheNewData);
					--(psLexer->bufferIndex);
				}
				bTokenFound = true;
				break;
			case SUBTRACT:		/*		-  or  --		*/
				++index;
				++(psLexer->bufferIndex);
				ch = psLexer->buffer[psLexer->bufferIndex];

				if (ch == '-' && (comparePrev (psLexer, 0, Identifier) ||
													comparePrev (psLexer, 0, Right_Paren) ||
													comparePrev (psLexer, 0, Right_Brack)))
				{
					lexeme[index] = ch;
					lexCreateToken (AUTO_DECR, Autoop, psLexer->currentFileLine, lexeme,
							&sTheNewData);
				}
				else //very next char was not a -, so we either have a subtract of negop
				{
					--(psLexer->bufferIndex);

					if (comparePrev (psLexer, 0, Identifier) ||
							comparePrev (psLexer, 0, Constant) ||
							comparePrev (psLexer, 0, Right_Paren) ||
							comparePrev (psLexer, 0, Right_Brack) ||
							comparePrev (psLexer, AUTO_DECR, Autoop) ||
							comparePrev (psLexer, AUTO_INCR, Autoop) )
					{
						lexCreateToken (ADD_SUB, Addop, psLexer->currentFileLine, lexeme,
								&sTheNewData);
					}
					else //otherwise we just have a subtraction
					{
						lexCreateToken (0, Negop, psLexer->currentFileLine, lexeme,
								&sTheNewData);
					}
				}

				bTokenFound = true;
				break;
			case AMPERSAND: 	/*		&  or  &&		*/
				lexeme[index] = ch;
				++index;
				++(psLexer->bufferIndex);
				ch = psLexer->buffer[psLexer->bufferIndex];
				if (ch == '&')
				{
					lexeme[index] = ch;
					lexCreateToken (0, Andop, psLexer->currentFileLine, lexeme, &sTheNewData);
				}
				else
				{
					lexCreateToken (0, Addressof, psLexer->currentFileLine, lexeme, &sTheNewData);
					--(psLexer->bufferIndex);
				}
				bTokenFound = true;
				break;
			case PIPE:		/*		||  or  BAD_CHAR		*/
				lexeme[index] = ch;
				++index;
				++(psLexer->bufferIndex);
				ch = psLexer->buffer[psLexer->bufferIndex];
				if (ch == '|')
				{
					lexeme[index] = ch;
					lexCreateToken (0, Orop, psLexer->currentFileLine, lexeme, &sTheNewData);
					bTokenFound = true;
				}
				else
				{
					erAddError (psLexer->psER, psLexer->currentLine, BAD_CHAR, lexeme);
					memset(lexeme, '\0', sizeof(lexeme));
					index = 0;
					state = START;
				}
				break;
			case SEMICOLON:	/*		;		*/
				lexCreateToken (0, Semicolon, psLexer->currentFileLine, lexeme, &sTheNewData);
				bTokenFound = true;
				break;
			case COMMA:  		/*		,		*/
				lexCreateToken (0, Comma, psLexer->currentFileLine, lexeme, &sTheNewData);
				bTokenFound = true;
				break;
			case PAREN_L:		/*		(		*/
				lexCreateToken (0, Left_Paren, psLexer->currentFileLine, lexeme, &sTheNewData);
				bTokenFound = true;
				break;
			case PAREN_R:		/*		)		*/
				lexCreateToken (0, Right_Paren, psLexer->currentFileLine, lexeme, &sTheNewData);
				bTokenFound = true;
				break;
			case BRACK_L:		/*		[		*/
				lexCreateToken (0, Left_Brack, psLexer->currentFileLine, lexeme, &sTheNewData);
				bTokenFound = true;
				break;
			case BRACK_R:		/*		]		*/
				lexCreateToken (0, Right_Brack, psLexer->currentFileLine, lexeme, &sTheNewData);
				bTokenFound = true;
				break;
			case BRACE_L:		/*		{		*/
				lexCreateToken (0, Left_Brace, psLexer->currentFileLine, lexeme, &sTheNewData);
				bTokenFound = true;
				break;
			case BRACE_R:		/*		}		*/
				lexCreateToken (0, Right_Brace, psLexer->currentFileLine, lexeme, &sTheNewData);
				bTokenFound = true;
				break;
			case BAD_CHARACTER:	/*		invalid character		*/
				++(psLexer->bufferIndex);
				erAddError (psLexer->psER, psLexer->currentLine, BAD_CHAR, lexeme);
				memset(lexeme, '\0', sizeof(lexeme));
				index = 0;
				state = START;
				break;
			case ID:			/*		identifier		*/
				while ( (ISLETTER(ch) || ISDIGIT(ch)))
				{
					lexeme[index] = ch;
					++(psLexer->bufferIndex);
					++index;
					ch = psLexer->buffer[psLexer->bufferIndex];
				}

				if (index >= MAX_ID_LENGTH)
				{
					erAddError (psLexer->psER, psLexer->currentLine, INVALID_ID, lexeme);
					memset (lexeme, '\0', sizeof(lexeme));
					index = 0;
					state = START;
				}
				else
				{
					ST_DATATYPE * stData = NULL;
					ST_ERROR stError;

					stError = stSearch (psLexer->psST, lexeme, &stData); /*  search ST for ID */

					if (stError == ST_NO_ERROR && stData->sLevel == RSRVWRD_LEVEL)
					{
						lexCreateToken (stData->address, Reserved_Word, psLexer->currentFileLine,
																	stData->pszIdentifier, &sTheNewData);
						--(psLexer->bufferIndex);
						bTokenFound = true;
					}
					else
					{
						lexCreateToken (0, Identifier, psLexer->currentFileLine, lexeme, &sTheNewData);
						if (NULL != stData)
						{
							sTheNewData.value = (long int) stData;
						}
						--(psLexer->bufferIndex);
						bTokenFound = true;
					}
				}
				break;
		}
	}

	if (bTokenFound) 		/*      return token      */
	{
		++(psLexer->bufferIndex);
		*psLexData = sTheNewData;
		//set prev token.. so it will work on the first go round...
		lexCreateToken(sTheNewData.value, sTheNewData.TokenClass, sTheNewData.lineNum,
										lexeme, &(psLexer->sPrevToken));

		if (psLexer->debugOn)
		{
			printToken(*psLexData);
		}
	}

	if (bDoneTokenize) 	/*     finished      */
	{
		error = LEX_NO_MORE_TOKENS;
	}

	return error;
}

/*******************************************************************************
 Function:		lexCreateToken

 Description:	initializes the LEX_DATATYPE

 Parameters:	value - value of the token (which one)
							sToken - the token class (e.g. constant, autoop, etc.)
							lineNum - current line number
							lexeme	- string representing the token

 Returned:		none
*******************************************************************************/
void lexCreateToken(int value, TokenClass sToken, int lineNum, char* lexeme,
											LEX_DATATYPE *sTheData)
{
	memset(sTheData->pszIdentifier, '\0', sizeof(sTheData->pszIdentifier));
	sTheData->value = value;
	sTheData->TokenClass = sToken;
	sTheData->lineNum = lineNum ;
	strncpy(sTheData->pszIdentifier, lexeme, strlen(lexeme));
}

/*******************************************************************************
 Function:		isWhiteSpace

 Description:	returns true if the char passed in is a white space (space, tab,
							or new line)

 Parameters:	ch	- char to be checked

 Returned:		isWhiteSpace -	true if whitespace; otherwise, false
*******************************************************************************/
bool isWhiteSpace (char ch)
{
	bool isWhiteSpace = false;

	if (ch == ' ' || ch == '\t' || ch == '\n')
	{
		isWhiteSpace = true;
	}

	return isWhiteSpace;
}

/*******************************************************************************
 Function:		isBadChar

 Description:	returns true if the char passed in is invalid

 Parameters:	ch	- char to be checked

 Returned:		isBadChar -	true if isBadChar; otherwise, false
*******************************************************************************/
bool isBadChar (char ch)
{
	bool isBadChar = false;

	if (ch == '@' || ch == '#' || ch == '$' || ch == '^' || ch == '_'
			|| ch == '.' || ch == '`' || ch == '~' || ch == '"' || ch == '?')
	{
		isBadChar = true;
	}

	return isBadChar;
}

/*******************************************************************************
 Function:		eatComment

 Description:	upon seeing the beginning of a comment this function is called to
 	 	 	 	 	 	 	treat all characters in the file after this sequence of chars as
 	 	 	 	 	 	 	white space. it will stop eating comments when the end of the
 	 	 	 	 	 	 	comment is found or the end of the file is reached

 Parameters:	psLexer - pointer to a lexer
							psER - pointer to error handler
							pFile - pointer to the file we are reading
							lineNum - pointer to the current line we are on in the file

 Returned:		error - an ErrorCode
*******************************************************************************/
ErrorCode eatComment (LexerPtr psLexer, ErrorHandlerPtr psER, FILE * pFile,
												int * lineNum)
{
	ErrorCode error = NO_ERROR;
	bool bEndOfComment = false, bDoneWithBuffer = false;

	char ch = psLexer->buffer[psLexer->bufferIndex];

	while (!bEndOfComment)
	{
		if (ch == '*')
		{
			++(psLexer->bufferIndex);
			ch = psLexer->buffer[psLexer->bufferIndex];
			if (ch == '/')
			{
				bEndOfComment = true;
				++(psLexer->bufferIndex);
				break;
			}
		}
		else if ((ch == '\n' || ch =='\0') && !feof(pFile))
		{
			++(*lineNum);
			psLexer->currentLine = (*lineNum);
			memset(psLexer->buffer, '\0', sizeof(psLexer->buffer));
			fgets (psLexer->buffer, sizeof(psLexer->buffer), pFile);

			if (psLexer->debugOn)
			{
				printLine(psLexer);
			}
			psLexer->bufferIndex = 0;
			ch = psLexer->buffer[psLexer->bufferIndex];
		}
		else
		{
			++(psLexer->bufferIndex);
			ch = psLexer->buffer[psLexer->bufferIndex];
		}

		if (ch == '\0' || (int)(strlen(psLexer->buffer) == 0)
				|| (psLexer->bufferIndex) > (int)(strlen(psLexer->buffer)) )
					{ bDoneWithBuffer = true;}

		if (!bEndOfComment && bDoneWithBuffer && feof(pFile))
		{
			error = READ_PAST_EOF;
			break;
		}
	}
	return error;
}

/*******************************************************************************
 Function:		printLine

 Description:	prints line number, the buffer of this line, and the headings for
							the token class, lexeme and value

 Parameters:	psLex	- pointer to the lexer

 Returned:		none
*******************************************************************************/
void printLine (LexerPtr psLex)
{
	printf ("\nLine  %5d: %.*s\n", psLex->currentLine,
						(int)strlen(psLex->buffer), psLex->buffer);

	if (NULL != psLex->buffer)
	{
	/* print extra line for blank buffer or if there is not \n at and of buffer */
		if (0 == (int)strlen(psLex->buffer) ||
				psLex->buffer[(int)strlen(psLex->buffer)-1] != '\n')
		{
			printf("\n");
		}
	}

	printf ("CLASS     LEXEME                             VALUE\n");
	printf ("-----     ------                             -----\n");
}

/*******************************************************************************
 Function:		printToken

 Description:	prints token data (printed in a way to be lined up under the
							printed heading

 Parameters:	sLexData	- the token to be printed

 Returned:		none
*******************************************************************************/
void printToken (LEX_DATATYPE sLexData)
{
	printf("%5d     ", sLexData.TokenClass);
	if (Constant == sLexData.TokenClass)
	{
		printf("%-32d", atoi(sLexData.pszIdentifier));
	}
	else
	{
		printf("%-32s", sLexData.pszIdentifier);
	}
	printf("%8lu\n", sLexData.value);
}

/*******************************************************************************
 Function:		lexGetLineNum

 Description:	get the line of the file we are at

 Parameters:	psLex - pointer to the lexer

 Returned:		int - current line num of the file
*******************************************************************************/
int lexGetLineNum (LexerPtr psLex)
{
	int lineNum = -1;

	if (NULL != psLex)
	{
		lineNum = psLex->currentFileLine;
	}
	return lineNum;
}

/*******************************************************************************
 Function:		setDebug

 Description:	sets the debug of the lexer to the bool passed in

 Parameters:	psLex	- pointer to the lexer
							debug	-	bool to set the debug too

 Returned:		none
*******************************************************************************/
void setDebug (LexerPtr psLex, bool debug)
{
	psLex->debugOn = debug;
}

/*******************************************************************************
 Function:		comparePrev

 Description:	compares a token to the previous token of the lex

 Parameters:	psLexer	-	pointer to the Lexer
							value		- value of the token
							sTokenClass - the token class

 Returned:		bMatch - returns true if matched. otherwise, false.
*******************************************************************************/
static bool comparePrev(LexerPtr psLex, int value, TokenClass sTokenClass)
{
	bool bMatch = false;

	if (Constant == sTokenClass && (psLex->sPrevToken).TokenClass == Constant)
	{
		bMatch = true;
	}
	else if (Identifier == sTokenClass && (psLex->sPrevToken).TokenClass == Identifier)
	{
		bMatch = true;
	}
	else if ((psLex->sPrevToken).TokenClass == sTokenClass &&
			(psLex->sPrevToken).value == value)
	{
		bMatch = true;
	}

	return bMatch;
}
