/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

//!	Global include file for the YDFP compiler
/*! \file 
*/


////////////////////////////////////////////////////////////////////////////////////
//	Assembler für YDFP und XDFP
//
//
//	1.0		02.07.2009	MV	first compiler version extracted from ydfpasm v 2.3
//						
///////////////////////////////////////////////////////////////////////////////////

#ifndef _H2XML_GLOBAL__INCLUDED_
#define _H2XML_GLOBAL__INCLUDED_

#include "version.h"

#ifdef _DEBUG
	#define _H2XML_DEBUG		// Debug output
#endif

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <time.h>
#include "CStr.h"
#include "hashtable.h"
#include "h2xml_definitions.h"
#include "CPointer.h"
#include "CList.h"
#include <limits>


// definiert den typ 64bit integer
#ifdef WIN32
	#define long64 __int64
    #define ulong64 unsigned __int64
	#define	strcasecmp	_stricmp
	double pow(int a,int b);
	double fmod(int a,int b);
	double log(int a);
#else
	#define long64 long long
    #define ulong64 unsigned long long
    #define _strdup strdup
    #define _itoa(val,str,base) sprintf(str, "%d", val);
	#define vsprintf_s  vsnprintf
	#define vsnprintf_s  vsnprintf
	#define _snprintf snprintf
	#define _snprintf_s snprintf
	#define sprintf_s  snprintf
	#define INT_MAX    2147483647
#endif

#ifdef __SOLARIS__
	double pow(int a,int b);
	double fmod(int a,int b);
#endif


// global definitions
enum scope {NOT_DEFINED,SCOPE_GLOBAL,SCOPE_LOCAL};

// extern function definitions for lexx and yacc
extern	FILE* yycompilerin;
extern	int yyparse(void);
extern	int yydebug;
extern	int yycompiler_flex_debug;
extern	int yyannot_flex_debug;
extern	char* yycompilertext;
extern	void lex_begin_last_state();
extern  void lex_begin_cdefine();
extern	void lex_begin_initial(int save_state=0);
extern	void insert_module(char* name, int mode, bool is_struct);
extern	void end_module(char* name);
extern	void lex_begin_readline_ydfpcc();
extern	void DoSkip(int k, int from);
extern 	void process_include(char* name);
extern 	char* getname(CStr* module, char* text);
extern	int kn_global_mem;
extern	bool emit_pointer;

// YDFPCC definitions
class  CIdentifier;
class  CDataType;
struct initmem_struct;
struct initializerlist;
struct decl_specifiers;
class  CExpression;
class  CStatement;
struct CStatementBlock;
struct CIdentifierBlock;
class  Cvdt;
struct mem_usage_struct;
struct cur_mem_assignment;

extern CPointer ydfpPointer[NR_POINTERS];
//! struct for CStatement block
struct CStatementBlock {
	CStatement* first;
	CStatement* last;
};
//! struct for CIdentifier block
struct CIdentifierBlock {
	CIdentifier* first;
	CIdentifier* last;
};
//! struct for CExpression block
struct CExpressionBlock {
	CExpression* first;
	CExpression* last;
};
//! struct for typeQualifier block
struct typeQualifier {
	int type_qualifier;
	typeQualifier* next;
};
//! struct for typeQualifier block
struct typeQualifierBlock {
	typeQualifier* first;
	typeQualifier* last;
};



//Global Enumerations
enum IdentifierToken {
	IDENTIFIER_UNDEFINED,
	IDENTIFIER_VARIABLE,
	IDENTIFIER_FUNCTION,
    IDENTIFIER_FUNCTION_NOARGS,
	IDENTIFIER_pFUNCTION,
    IDENTIFIER_pFUNCTION_NOARGS,
    IDENTIFIER_PARAMETER,
	IDENTIFIER_TYPEDEF,
	IDENTIFIER_EXTERN,
    IDENTIFIER_LABEL
};
enum operators {
	OP_NOT_DEFINED, OP_CONST_INT, OP_CONST_FLOAT, OP_IDENTIFIER, OP_IDENTIFIER_YDFPCC, OP_FUNCTION,
	OP_ASSIGN, OP_MUL_ASSIGN,OP_DIV_ASSIGN,OP_MOD_ASSIGN,OP_ADD_ASSIGN,OP_SUB_ASSIGN,OP_SHIFT_LEFT_ASSIGN,OP_SHIFT_RIGHT_ASSIGN,OP_XOR_ASSIGN,OP_OR_ASSIGN,
	OP_AND_ASSIGN,OP_CONDITIONAL,OP_COMMATA, OP_LOGIC_OR,OP_LOGIC_AND,OP_INCLUSIVE_OR,OP_EXCLUSIVE_OR, OP_ANDX,OP_EQUAL,OP_NOT_EQUAL,OP_LESS,OP_GREATER,OP_LESS_EQ,OP_GREATER_EQ,
	OP_SHIFT_LEFT,OP_SHIFT_RIGHT,OP_PLUS,OP_MINUS,OP_MULT,OP_DIV,OP_MODULO,
	OP_TYPECAST, OP_UNARY_PRE_PLUS_PLUS,OP_UNARY_PRE_MINUS_MINUS,OP_UNARY_AND,OP_UNARY_STAR,OP_UNARY_PLUS,OP_UNARY_MINUS,
	OP_UNARY_NEGATE,OP_UNARY_INVERT,OP_ARRAY,OP_DOT,OP_ARROW,OP_POST_PLUS_PLUS,OP_POST_MINUS_MINUS,
	OPYCC_LDA,OPYCC_ADD,OPYCC_MUL_CONST,OPYCC_MUL_PRC,OPYCC_MAC_CONST,OPYCC_MAC_PRC,OPYCC_LDM,OPYCC_STM, 
	OP_TYPECASTP, OP_CONST_STRING, kn_EXP_OPERATORS
};
enum statements {
	STATEMENT_UNDEFINED,
	STATEMENT_DEKLARATION,
	STATEMENT_DEKLARATION_TYPE,
	STATEMENT_FUNCTION_DEKLARATION,
	STATEMENT_FUNCTION_DEFINITION,
	STATEMENT_LABEL,             
	STATEMENT_CASE,              
	STATEMENT_DEFAULT,           
	STATEMENT_EXPRESSION,        
	STATEMENT_COMPOUND_STATEMENT,
	STATEMENT_IF,                
	STATEMENT_IFELSE,            
	STATEMENT_SWITCH,            
	STATEMENT_WHILE,
	STATEMENT_DO,
	STATEMENT_DOWHILE,           
	STATEMENT_FOR,               
	STATEMENT_GOTO,              
	STATEMENT_CONTINUE,          
	STATEMENT_BREAK,             
	STATEMENT_RETURN,
	STATEMENT_PREPROC_IFDEF,
	STATEMENT_PREPROC_DEFINE,
    STATEMENT_EMPTY
};

enum memtype {
	TYPE_UNKNOWN,
	TYPE_CONST_INT,		// constatnt int
	TYPE_CONST_FLOAT,	// constant float
    TYPE_FIXP24,		// fixp24
	TYPE_FIXP24S,		// fixp24 stereo
	TYPE_PFIXP24,		// pointer to fixp24
	TYPE_PFIXP24S,		// pointer to fixp24 stereo
	TYPE_AFIXP24,		// array fixp24
	TYPE_AFIXP24S,		// array fixp24
	TYPE_TYPEDEF,		// i.e. typedef, struct etc
	TYPE_STRUCT,
	TYPE_UNION,
    TYPE_INT,
    TYPE_PINT,
    TYPE_FLOAT,
    TYPE_PFLOAT,
	TYPE_CHAR,
    TYPE_PCHAR,
    TYPE_SHORT,
    TYPE_PSHORT,
    TYPE_LONG,
	TYPE_LONG_LONG,
    TYPE_PLONG,
    TYPE_SIGNED,
    TYPE_UNSIGNED,
    TYPE_DOUBLE,
    TYPE_PDOUBLE,
    TYPE_VOID,
    // Register Types
	TYPE_BOOL,
	TYPE_REG_ACCU,
	TYPE_REG_M,
	TYPE_REG_C,
	TYPE_REG_L,
	TYPE_FLAG_F,
	TYPE_FLAG_T,
    TYPE_ENUM,
	kn_TYPES
};


//! Struct to keep the main data information
struct as_elements {
	// general
	int linecnt;				// line nr of actual file
	int linecntStartDoxy;		// line nr at start of doxygen comment
	int linecntEndDoxy;			// line nr at end of doxygen comment

	int total_linecnt;			// count all lines, also include files
	double ydfp_version;		// version of ydfp core. can be set by .ydfp_version xx
	CStr str;					// string argument to pass to yacc
	long64 enum_cnt;
    CStr enumName;
	bool verbose;
    bool printVersion;

	// error handling
    FILE* fpStderr;
	int error_code;
	int warningcount;
	int errorcount;				// number of errors
	bool skiperror;				// set this flag to supress error message for missing labels in pass1
	CStr error_text;

	// i/o handling
	CStr inputfile;				// actual input file + pfad
	CStr inputpath;				// aktueller input pfad
	CStr inputname;				// nur filename ohne pfad
    CStr outputname;
	CStr outputpath;
	CStr outputpathRtf;
	CStr outputfile;
	CStr doxycomment;			//
    CStr doxyInsertcode;
	bool connect_previous;

	
	CStr inputname_main;
	int output_level;

	CStr* last_cstr;			// points to last string read, needed for extended string over seveeral lines

	CStr file_extention;		// file extention for different processors e.g. _1, _2...
	FILE* infile;
	bool do_include;			// true if include at next new_line
	Cvdt* include_dirs;			// list of include directories, initialized in command lin
	CStr include;
	int include_cnt;			// include depth

	// conditional input
	Cvdt* iflist;				// list for nested .ifs
	bool skip_input;			// skip input, for .if .else .endif 
	int current_state;
	int removeBlanks;			//

	// module handling
	CStr module_name;			// name of actual module
	Cvdt* actual_module;	    // actual module
	Cvdt* module_list;			// list of module names
	Cvdt* last_var_name;		// name of last variable used
	Cvdt* varlist;				// list of variables used, to detect nested declarations


	// C-Compiler specific
	decl_specifiers* declspec;
	CDataType *actual_type;
	
    //Cvdt* actual_struct;
	long ycc_pointer_level;
	int is_struct;
    bool is_static;
    int global_scope;							// true if actual module = global
	initmem_struct*	initmem;
	long initcnt;								// counts number of elements for initialization		
	Cvdt default_cvdt;							// e.g. for structs without name defined
	CStatement* ycc_start;						// points to first statement
	char* ycc_code;								// pointer to actual code line
	CExpression* ydfpcc_reg_accu;				// points to actual identifier for tmp-registers
    CExpression* ydfpcc_preg_accu;				// points to actual identifier for tmp-registers
    char* ycc_structname;						// if struct is evaluated: points to name of struct for hashlookup
	CDataType* CDataType_save;
	int pointerSize;							// szize of Pointers in bytes (default=4)
	int enumSize;								// size of enum
    bool isEnumTypedf;                          // currently processing a n enum typedef

	// predefined type
	CDataType* type_fixp24;						//predefined type                                                      

    CDataType* type_int;						//predefined type integer                                                      
    CDataType* type_float;						//predefined type float                                              
    CDataType* type_double;						//predefined type float                                              
    CDataType* type_char;						//predefined type char                                                     
    CDataType* type_short;						//predefined type short 
    CDataType* type_long;						//predefined type long 
    CDataType* type_long_long;					//predefined type long 
    CDataType* type_signed;						//predefined type signed 
    CDataType* type_unsigned;					//predefined type unsigned
    CDataType* type_void;					    //predefined type void


	// predefined Identifiers
	CIdentifier* ident_undef  ;					//predefined identifer, inserted in expression if Identifier not defined                                                
	CIdentifier* ident_const_int;				//predefined identifer
	CIdentifier* ident_const_float;				//predefined identifer
	CIdentifier* ident_const_string;	    	//predefined identifer

    CIdentifier* ident_int;						//predefined identifier integer                                                      
    CIdentifier* ident_float;				    //predefined identifier float                                              
    CIdentifier* last_ident;				    //last used identifier in do_break function;

	//Cvdt* filelist;
	int nameless_cnt;							// craeate names for nameless structs
	CIdentifier* last_identifier;
	CStatement* last_statement;
} ;
extern as_elements as;

//! Struct for thr error code information
struct yerrcode{
	int ernum;
	char message[200];
}; 

extern yerrcode errmessage[];
extern CHashTable htab;

extern bool typedef_seen;
extern int  struct_seen;
extern int  struct_typedef_cnt;

extern char  StatementString[][100];
extern char  OperatorString[][10];
extern char  Typenames[][100]; 

class CValue;
void PrintCIdentifierBlock (CIdentifierBlock* tmp,const char* tmp2) ;
char *strtok_esc(char* buf,const char* lim);  // strtok, but ignores \escape characters

#endif // _H2XML_GLOBAL__INCLUDED_

