/*****************************************************************************
 File name:     pcc13defines.h Version 1.0 $Rev: 33 $
 Date:          2/1/13
 Class:         CS480
 Assignment:    Compiler Project written in C
 Purpose:       These are the manifest constants used by the various
                parts of the compiler. Do not modify any part of this
                file.  I will always let you know when a new version
                is created and where you can find it.
 Modifications:
******************************************************************************/

#ifndef PCC13DEFINES_H
#define PCC13DEFINES_H


//*****************************************************************************
// symbol table entry levels
//*****************************************************************************
typedef enum  SymbolTableLevel { RSRVWRD_LEVEL = 0,
                         GLOBAL_LEVEL,
                         LOCAL_LEVEL } SymbolTableLevel;


//*****************************************************************************
//  symbol table entry types
//*****************************************************************************
typedef enum SymbolTableType { NONE = -1,
			RSRVWRD,
                        INTEGER,
                        INTEGER_ARRAY,
                        POINTER_ARRAY,
                        POINTER,
                        FUNCNAME,
                        POINTER_TO_POINTER } SymbolTableType ;


//*****************************************************************************
// manifest const for reserved words
//*****************************************************************************
typedef enum ReservedWord { RSRVWRD_MAIN = 1,
                     RSRVWRD_INT,
                     RSRVWRD_IF,
                     RSRVWRD_ELSE,
                     RSRVWRD_RETURN,
                     RSRVWRD_FOR,
                     RSRVWRD_INPUT,
                     RSRVWRD_OUTPUT } ReservedWord;

//*****************************************************************************
// error codes
//*****************************************************************************
typedef enum ErrorCode {NO_ERROR = 0,
		 UNKNOWN_ERROR = 1,
		 BAD_CHAR,
                 MISSING_SEMICOLON,
                 MISSING_RPAREN,
                 MISSING_LPAREN,
                 MISSING_RBRACE,
                 MISSING_LBRACE,
                 MISSING_RBRACKET,
                 MISSING_LBRACKET,
                 IDENTIFIER_EXPECTED,
                 CONSTANT_EXPECTED,
                 MAIN_EXPECTED,
                 INVALID_DECLARATION,
                 READ_PAST_EOF,
                 BAD_EXPRESSION,
                 DUPLICATE_ID,
                 UNDECLARED_IDENTIFIER,
                 UNDECLARED_FUNCTION,
                 ID_NOT_RIGHT_TYPE,
                 UNDECLARED_ARRAY,
                 MISMATCHED_PARAMS,
                 UNARY_TYPE_MISMATCH,
                 ADDOP_TYPE_MISMATCH,
                 MULOP_TYPE_MISMATCH,
                 DEREF_TYPE_MISMATCH,
                 ASSIGN_TYPE_MISMATCH,
                 INVALID_ID,
                 CONST_TOO_LONG,
                 BAD_STMT,
                 EXTRA_TOKENS,
                 NO_MORE_TOKENS,
                 CANNOT_OPEN_FILE,
                 OUT_OF_MEMORY,
                 MISSING_COMMA,
                 INVALID_TOKEN,
                 EXPECTED_LVALUE,
		 RETURN_TYPE_MISMATCH} ErrorCode;


#define ISDIGIT(c) ((c >= '0') && (c<= '9'))
#define ISLETTER(c) (((c >= 'a') && (c <= 'z')) ||\
                         ((c >= 'A') && (c <= 'Z')))
#define IS_VALID_IDENT_CHAR(c) (ISDIGIT(c) || ISLETTER(c))

/* You must declare gszErrors in your code */

#define LOAD_ERRORS strcpy(gszErrors[NO_ERROR], "No Error.");\
strcpy(gszErrors[UNKNOWN_ERROR], "Error: Unknown Error.");\
strcpy(gszErrors[BAD_CHAR], "Error: Character Not In Grammar.");\
strcpy(gszErrors[MISSING_SEMICOLON], "Error: Missing Semicolon.");\
strcpy(gszErrors[MISSING_RPAREN], "Error: Missing Right Parenthesis.");\
strcpy(gszErrors[MISSING_LPAREN], "Error: Missing Left Parenthesis.");\
strcpy(gszErrors[MISSING_RBRACE], "Error: Missing Right Brace.");\
strcpy(gszErrors[MISSING_LBRACE], "Error: Missing Left Brace.");\
strcpy(gszErrors[MISSING_RBRACKET], "Error: Missing Right Bracket.");\
strcpy(gszErrors[MISSING_LBRACKET], "Error: Missing Left Bracket.");\
strcpy(gszErrors[IDENTIFIER_EXPECTED], "Error: Identifier Expected.");\
strcpy(gszErrors[CONSTANT_EXPECTED], "Error: Constant Expected.");\
strcpy(gszErrors[MAIN_EXPECTED], "Error: Main Declaration Expected.");\
strcpy(gszErrors[INVALID_DECLARATION], "Error: Invalid Declaration.");\
strcpy(gszErrors[READ_PAST_EOF], "Error: Read Past EOF.");\
strcpy(gszErrors[BAD_EXPRESSION], "Error: Bad Expression.");\
strcpy(gszErrors[DUPLICATE_ID], "Error: Duplicate Identifier.");\
strcpy(gszErrors[UNDECLARED_IDENTIFIER], "Error: Undeclared Identifier.");\
strcpy(gszErrors[UNDECLARED_FUNCTION], "Error: Undeclared Function.");\
strcpy(gszErrors[ID_NOT_RIGHT_TYPE], "Error: Identifier Not Right Type.");\
strcpy(gszErrors[UNDECLARED_ARRAY], "Error: Undeclared Array.");\
strcpy(gszErrors[MISMATCHED_PARAMS], "Error: Mismatched Parameters.");\
strcpy(gszErrors[UNARY_TYPE_MISMATCH], "Error: Unary Type Mismatch.");\
strcpy(gszErrors[ADDOP_TYPE_MISMATCH], "Error: Addop Type Mismatch.");\
strcpy(gszErrors[MULOP_TYPE_MISMATCH], "Error: Mulop Type Mismatch.");\
strcpy(gszErrors[DEREF_TYPE_MISMATCH], "Error: Dereference Type Mismatch.");\
strcpy(gszErrors[ASSIGN_TYPE_MISMATCH], "Error: Assign Type Mismatch.");\
strcpy(gszErrors[INVALID_ID], "Error: Invalid Identifier.");\
strcpy(gszErrors[CONST_TOO_LONG], "Error: Constant Too Long.");\
strcpy(gszErrors[BAD_STMT], "Error: Bad Statement.");\
strcpy(gszErrors[EXTRA_TOKENS], "Error: Extra Tokens.");\
strcpy(gszErrors[NO_MORE_TOKENS], "Error: No More Tokens.");\
strcpy(gszErrors[CANNOT_OPEN_FILE], "Error: Cannot Open File.");\
strcpy(gszErrors[OUT_OF_MEMORY], "Error: Out Of Memory.");\
strcpy(gszErrors[MISSING_COMMA], "Error: Missing Comma.");\
strcpy(gszErrors[INVALID_TOKEN], "Error: Invalid Token.");\
strcpy(gszErrors[EXPECTED_LVALUE], "Error: Expected Lvalue.");\
strcpy(gszErrors[RETURN_TYPE_MISMATCH], "Error: Return Type Mismatch.");




//*****************************************************************************
// miscellaneous constants
//*****************************************************************************
#define FALSE  0
#define TRUE  1
#define RVALUE  0
#define LVALUE  1
#define MAX_ERRORS  100
#define MAX_QUADS  1500
#define MAX_RUNTIME  5000
#define ERROR_TBL_SIZE  38
#define MAX_ERROR_LINE  50
#define EXIT_NOERROR  0
#define EXIT_ERROR  1




//*****************************************************************************
// quad opcodes
//*****************************************************************************

typedef enum QuadOpcode { OP_NOP = 0,
                   OP_ADD,
                   OP_SUBTRACT,
                   OP_MULTIPLY,
                   OP_DIVIDE,
                   OP_MODULUS,
                   OP_MINUS,
                   OP_INCREMENT,
                   OP_DECREMENT,
                   OP_DEREFERENCE,
                   OP_BLT,
                   OP_BGT,
                   OP_BLE,
                   OP_BGE,
                   OP_BNE,
                   OP_BEQ,
                   OP_BAND,
                   OP_BOR,
                   OP_BNOT,
                   OP_BRA,
                   OP_PUSHPARAM,
                   OP_FUNCCALL,
                   OP_FUNCBEGIN,
                   OP_FUNCRETURN,
                   OP_INPUT,
                   OP_OUTPUT,
                   OP_ASSIGN,
                   OP_PROGBEGIN,
                   OP_PROGEND } QuadOpcode;


//*****************************************************************************
// quadruple address modes
//*****************************************************************************

typedef enum QuadAddressingMode { IMMEDIATE = 0,
                           GLOBAL_LVALUE,
                           GLOBAL_RVALUE,
                           LOCAL_LVALUE,
                           LOCAL_RVALUE } QuadAddressingMode;

//*****************************************************************************
// operator precedence relations
//*****************************************************************************
#define ERROR  0
#define YIELDS  1
#define TAKES  2
#define EQUALS  3
#define ACCEPT  4

/**********************************/
/* operator precedence table      */
/**********************************/
#define NUM_CLASSES  19

#endif
