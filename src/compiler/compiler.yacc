/* All commands must be terminated by <estd> =EndOfStatement  */
%{
/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////////////
//! \brief					 YACC generated file												\file
////////////////////////////////////////////////////////////////////////////////////////////////////
%}
%{
	#include <stdio.h>
	#include <math.h>
	#define	YYERROR_VERBOSE
	#define YYDEBUG 1
	#undef __GNUC_MINOR__			// workaround: otherwise compiler error will be generated

	/*void newline();
	void yyerror (char const *);
	*/

	#include "global_functions.h"
	#include "h2xml_global.h"
	#include "CIdentifier.h"
	#include "CExpression.h"
	#include "CStatement.h"
	#include "CDataType.h"
	#include "compilerYaccyy.hpp"

	#define __builtin_memcpy __builtin_memmove    // avoid memcopy because of gcc library incompatibilities


	#define yylex() yycompilerlex()
	extern int yycompilerlex();

	as_elements as;
	int current_pass;
    bool typedef_seen;
    int struct_seen;
    int  struct_typedef_cnt;

%}

%expect 2	/* expect 4 shift - reduce conflicts, solved by Bison itself*/

%union{
	unsigned long64 uval;
	long64 ival;
	double fval;
	Cvdt* vdt;
	CStr* str;
	CDataType* dtype;
	CIdentifier* ident;
	initializerlist* init_list;
	decl_specifiers* decl_specs;
	CExpression* cexpr;
	CStatement* cstat;
	CStatementBlock cstatblock; 
	CIdentifierBlock identblock;
	CExpressionBlock expblock;
	typeQualifier* typequal;
}


/* Expressions */
%token <ival> UNUM
%token <ival> INUM ERR
%token <fval> FNUM
%token <vdt>  LABEL HLABEL ADDRESS DOTFOR 
%token <str>  TSTRING PREPROC_DEFINESTR

/* PreProc*/
%token <ival> ENUM DOXYCOMMENT ELN PREPROC_IF PREPROC_IFDEF PREPROC_IFNDEF EOLC PREPROC_INCLUDE PREPROC_DEFINE PREPROC_UNDEFINE

/* Symbol-Table */
%token <vdt>	IDENTIFIER TYPSPECIFIER 

/* C - Keywords */
%token <ival>	TYPEDEF EXTERN STATIC AUTO REGISTER RESTRICT SIGNED UNSIGNED STRUCT UNION ELLIPSIS CONST VOLATILE 
%token <ival>	CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN NAMESPACE
/* C - Operators */  
%token <ival>	YOP_ASSIGN YOP_MUL_ASSIGN YOP_DIV_ASSIGN YOP_MOD_ASSIGN YOP_ADD_ASSIGN YOP_SUB_ASSIGN YOP_SHIFT_LEFT_ASSIGN YOP_SHIFT_RIGHT_ASSIGN YOP_AND_ASSIGN 
%token <ival>	YOP_XOR_ASSIGN YOP_OR_ASSIGN YOP_CONDITIONAL YOP_LOGIG_OR YOP_LOGIC_AND YOP_INCLUSIVE_OR YOP_EXCLUSIVE_OR YOP_ANDX YOP_EQUAL YOP_NOT_EQUAL 
%token <ival>	YOP_LESS YOP_GREATER YOP_LESS_EQ YOP_GREATER_EQ YOP_SHIFT_LEFT YOP_SHIFT_RIGHT YOP_PLUS YOP_MINUS YOP_MULT YOP_DIV YOP_MODULO 
%token <ival>	YOP_UNARY_TYPECAST YOP_UNARY_PRE_PLUS_PLUS YOP_UNARY_PRE_MINUS_MINUS YOP_UNARY_AND YOP_UNARY_STAR YOP_UNARY_PLUS YOP_UNARY_MINUS 
%token <ival>	YOP_UNARY_NEGATE YOP_UNARY_INVERT YOP_ARRAY YOP_DOT YOP_ARROW YOP_POST_PLUS_PLUS YOP_POST_MINUS_MINUS  YOP_PLUS_PLUS  YOP_MINUS_MINUS   
%token <ival>	SIZEOF ATTRIBUTE 
/* C-Grammar Tokens */
%type  <dtype>			type_specifier enum_specifier struct_or_union_specifier struct_or_union_specifier_no_body
%type  <ident>			enumerator enumerator_list direct_declarator direct_declarator_bracket declarator struct_declarator init_declarator func_1 func_2
%type  <ident>			parameter_type_list parameter_declaration abstract_declarator
%type  <ival>			struct_or_union storage_class_specifier type_qualifier assignment_operator unary_operator type_qualifier_list estd attributes
%type  <init_list>		initializer_list initializer
%type  <decl_specs>		declaration_specifiers specifier_qualifier_list 
			
%type  <cexpr>			cexpression assignment_expression conditional_expression constant_expression logical_or_expression 
%type  <cexpr>			logical_and_expression inclusive_or_expression exclusive_or_expression and_expression equality_expression relational_expression
%type  <cexpr>			shift_expression additive_expression multiplicative_expression cast_expression unary_expression constant
%type  <cexpr>			primary_expression postfix_expression function_call

%type  <cstat>			labeled_statement compound_statement statement function_definition expression_statement selection_statement iteration_statement jump_statement preprocessor
%type  <cstatblock>		init_declarator_list  declaration external_declaration statement_list declaration_list translation_unit

%type  <identblock>		parameter_list struct_declaration_list struct_declaration  struct_declarator_list struct_body 
%type  <expblock>		argument_expression_list
%type  <typequal>		cpointer
%type  <str>			defineArg

/* LAST TOKEN */
%token MAXTOKEN

/* %destructor { ; } <vdt> */ 
 
/* Associations */
%left '|'
%left '^'
%left '&'
%left '-' '+' '*' '/'
%%


/************************************************************************************************/
/* Main Programm																				*/
/************************************************************************************************/
prog:	code				{;}
//	|	prog code			{;}
//	|	prog error ELN		{newline();}	/*syntax error*/
	|	error				{;} ELN			{newline_ydfpcc(); lex_begin_initial();}	/*syntax error*/

	;

code:	
	|	translation_unit			{as.ycc_start=$1.first; newline_ydfpcc();}
	;



/************************************************************************************************/
/* ANSI C Parser																				*/
/************************************************************************************************/
//pragmas:	EXTERNAL_TABLE		
//	;


/****************************/
/* 	Variable declaration 	*/
/****************************/



translation_unit
	: external_declaration												{er(0);$$=$1;}
	| translation_unit external_declaration								{er(0);if ($1.last!=NULL){$1.last->set_next(&$2);$$.first=$1.first;$$.last=$2.last;}}
	| NAMESPACE  IDENTIFIER												{$2->token=TOKEN_NAMESPACE;insert_module($2->name.buf,1,0);}
		'{' translation_unit '}'										{er(0);$$=$5;end_module(NULL);}
	| translation_unit NAMESPACE  IDENTIFIER							{$3->token=TOKEN_NAMESPACE;insert_module($3->name.buf,1,0);}
		'{' translation_unit '}'										{er(0);if ($1.last!=NULL){$1.last->set_next(&$6);$$.first=$1.first;$$.last=$6.last;} end_module(NULL);}
	| translation_unit error external_declaration						{er(0);}
	;

external_declaration
	: declaration														{$$=$1;}
	| function_definition 												{$$.first=$1;$$.last=$1;}
	| preprocessor														{$$.first=$1;$$.last=$1;}
	;

declaration
	: declaration_specifiers estd										{CIdentifier* tmp=newCIdentifier((Cvdt*)NULL);$$.first=new_CStatement(STATEMENT_DEKLARATION_TYPE,tmp);
																		 $$.last=$$.first; tmp->m_declspec=as.declspec; tmp->m_token=IDENTIFIER_TYPEDEF;insertShortNameStruct($$.first);tmp->m_attribute=(int)$2;}								
	| declaration_specifiers init_declarator_list  estd					{$$=$2; insertShortName($$.first); $$.first->m_identifier->m_attribute=(int)$3;}
	;


declaration_specifiers
	: storage_class_specifier											{$$=new decl_specifiers(NULL, (int)$1, 0);as.declspec=$$;}		
	| storage_class_specifier declaration_specifiers					{$2->setStorageSpecifier((int)$1); as.is_static = false;}	// only one storage class allowed
	| type_specifier													{$$=new decl_specifiers($1, 0, 0);as.declspec=$$;}				
	| type_specifier declaration_specifiers								{$2->setDataType($1);}	
	| type_qualifier													{$$=new decl_specifiers(NULL, 0, (int)$1); as.declspec=$$; }	

	| type_qualifier declaration_specifiers								{$2->setTypeQualifier((int)$1);}
	;

init_declarator_list
	: init_declarator													{$$.first=new_CStatement(STATEMENT_DEKLARATION,$1);$$.last=$$.first;}
	| init_declarator_list  ','  init_declarator						{CStatement* tmp=new_CStatement(STATEMENT_DEKLARATION,$3);tmp->m_identifier=$3;if ($1.last!=NULL) $$.last=$1.last->set_next(tmp);}					
	;

init_declarator
    : declarator														{$$=$1; $1->clearFunctionParameter(); as.is_struct=false;} // for function deklaration xy(fix24 c); -> variable c is not declared!
	| declarator '=' initializer										{$$=$1; $1->adaptInitializerList($3);}
	;

storage_class_specifier
	: TYPEDEF															{$$=TYPEDEF; typedef_seen = true;}
	| EXTERN															{$$=EXTERN;}
	| STATIC															{$$=STATIC; as.is_static = true;}														
	| AUTO																{$$=AUTO;}
	| REGISTER															{$$=REGISTER;}
	| RESTRICT															{$$=RESTRICT;}
	;

type_qualifier
	: CONST																{$$=CONST;}									
	| VOLATILE															{$$=VOLATILE;}
	;
	
/* FIXP24, FIXP24s, ... SIGNED, UNSIGNED */																	
type_specifier
	: TYPSPECIFIER														{$$=$1->datatype; }																					
	| struct_or_union_specifier											{$$=$1; as.is_struct=false;}
	| enum_specifier													{$$=$1;}
    //	| struct_or_union_specifier error									{end_module(NULL);}
	;


/*struct_or_union_specifier

     : struct_or_union IDENTIFIER '{' struct_declaration_list '}'

     | struct_or_union '{' struct_declaration_list '}'

     | struct_or_union IDENTIFIER

     ;
*/



struct_or_union_specifier
	: struct_or_union IDENTIFIER										{startStructCompound(0); insert_module($2->name.buf,1,1); } 
	  struct_body														{as.CDataType_save=CDataType::newCDatatype($2,(int)$1);
																		 as.CDataType_save->insertElements($4.first);
																		 end_module(NULL);$$=as.CDataType_save;endStructCompound(0);}
	
    | struct_or_union TYPSPECIFIER  									{startStructCompound(1); if ($2->datatype && $2->datatype->m_elements) { ycc_erpnt(119, $2->name.buf);}insert_module($2->name.buf,1,1); } 
	  struct_body														{as.CDataType_save=CDataType::newCDatatype($2,(int)$1);
																		 as.CDataType_save->insertElements($4.first);
																		 end_module(NULL);$$=as.CDataType_save;endStructCompound(1);}
   
	| struct_or_union													{startStructCompound(2); char name[100];sprintf(name,"__nameless_struct_%d",as.nameless_cnt);as.nameless_cnt++;insert_module(name,1,1); }
	  struct_body														{Cvdt* tmp=htab.hashinstall(as.module_name.buf);
																			as.CDataType_save=CDataType::newCDatatype(tmp,(int)$1); as.CDataType_save->insertElements($3.first); end_module(NULL);$$=as.CDataType_save;endStructCompound(2);}
	| struct_or_union_specifier_no_body									{$$=$1;}
    ;
    
struct_or_union_specifier_no_body
	: struct_or_union IDENTIFIER										{as.CDataType_save=CDataType::newCDatatype($2,(int)$1);$$=as.CDataType_save;}
	| struct_or_union TYPSPECIFIER          							{as.CDataType_save=CDataType::newCDatatype($2,(int)$1);$$=as.CDataType_save;}
	;

struct_body
	: '{' struct_declaration_list '}'									{$$=$2; /*PrintCIdentifierBlock(&$$,"struct_body")*/;} //struct_seen=2;
	| '{'   error '}'													{$$.first=NULL;$$.last=NULL;}
;


struct_or_union
	: STRUCT															{$$=TYPE_STRUCT;}			//struct_seen=true;							
    | UNION																{$$=TYPE_UNION;}														
	;

struct_declaration_list
	: struct_declaration												{$$.first=$1.first;$$.last=$1.last;}
	| struct_declaration_list struct_declaration						{if ($1.last!=NULL) $$.last=$1.last->set_next(&$2);
																		 if ($$.first==NULL) $$.first=$2.first;
																		 if ($$.last==NULL) $$.last=$2.last;}
	;

struct_declaration
	: specifier_qualifier_list struct_declarator_list estd				{$$=$2;}
	//added: for nameless structs. check if additional information must be provided!
	| specifier_qualifier_list  estd									{CIdentifier* tmpid=newCIdentifier((Cvdt*)NULL);$$.first=tmpid;$$.last=tmpid;}
	| preprocessor														{$$.first=NULL;$$.last=NULL;}

	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list							{$2->setDataType($1);}
	| type_specifier													{$$=new decl_specifiers($1, 0, 0);as.declspec=$$;}			
	| type_qualifier specifier_qualifier_list							{$2->setTypeQualifier((int)$1);}
	| type_qualifier													{$$=new decl_specifiers(NULL, 0, (int)$1);as.declspec=$$;}
	;


struct_declarator_list
	: struct_declarator													{$$.first=$1;$$.last=$1;}				
	| struct_declarator_list ',' struct_declarator						{if ($1.last!=NULL) $$.last=$1.last->set_next($3);
																		 if ($$.first==NULL) $$.first=$3;
																		 if ($$.last==NULL) $$.last=$3;}			
	;


struct_declarator										
	: declarator														{$$=$1;}		
	| ':' constant_expression											{$$=fix_noVar_in_struct( int ($2->GetData()) ); }
	| declarator ':' constant_expression								{$$=$1; insert_struct_bitslice($1, int ($3->GetData()));}
	;



// original with : pointer direct_declarator
declarator
    : cpointer direct_declarator										{$$=$2; $2->m_pt_type_qual=$1;$2->m_pointer_level+=as.ycc_pointer_level;as.ycc_pointer_level=0;}	
    | '&' direct_declarator								        		{$$=$2; ycc_erpnt(117);}	
    | direct_declarator													{$$=$1;}
	;

direct_declarator
	: IDENTIFIER														{$$=newCIdentifier($1);}
	| '(' declarator ')'												{$$=$2;}																																
    | direct_declarator '[' constant_expression ']'						{$$=$1; $1->insertArrayDimension((long)$3->GetData());}
	| direct_declarator '[' ']'											{$$=$1; $1->insertArrayDimension();}
    
    | direct_declarator_bracket parameter_type_list ')'					{if ($1->m_pointer_level>1) {$1->m_token=IDENTIFIER_pFUNCTION; $1->m_pointer_level--;}
                                                                        else $1->m_token=IDENTIFIER_FUNCTION;
                                                                        $$=$1; $1->checkParameter($2);;
                                                                        end_module(NULL); $1->SetFunArgIds( $1->GetIdList_Next() );  }	//check if correct parameter type if already defined															
    
    | direct_declarator_bracket identifier_list ')'						{$$=$1;end_module(NULL);}
    | direct_declarator_bracket ')'										{if ($1->m_pointer_level!=0) $1->m_token=IDENTIFIER_pFUNCTION_NOARGS; //check pointer to functions
                                                                         else $1->m_token=IDENTIFIER_FUNCTION_NOARGS;  
                                                                         $$=$1;  end_module(NULL);}
	| direct_declarator_bracket error 	')'								{$$=$1;end_module(NULL);}
	;

direct_declarator_bracket
	: direct_declarator '('												{$$=$1; insert_module($1->m_cvdt->name.buf,1,0);}								
	;

cpointer
	: '*'																{$$=addTypeQualifier(NULL,0);as.ycc_pointer_level++;}
	| '*' type_qualifier_list											{$$=addTypeQualifier(NULL,(int)$2);as.ycc_pointer_level=1;}
    | '*' cpointer														{$$=addTypeQualifier($2,0);as.ycc_pointer_level++;}
	| '*' type_qualifier_list cpointer									{$$=addTypeQualifier($3,(int)$2);as.ycc_pointer_level++;}
	;

// i.e. int const* y;
type_qualifier_list
	: type_qualifier													{if ($1==CONST) $$=1; if ($1==VOLATILE) $$=2;}					// CONST=1, VOLATILE=2
	| type_qualifier_list type_qualifier								{if ($2==CONST) $$=$1 | 1; if ($2==VOLATILE) $$=$1 | 2;}		// CONST=1, VOLATILE=2		
	;

initializer
	: '{' initializer_list '}'											{$$=addInitializer($2,NULL,0);}
	| '{' initializer_list ',' '}'										{$$=addInitializer($2,NULL,888); }
	|  assignment_expression											{$$=addInitializer(NULL,NULL,$1->GetData()); $$->exp=$1;}
	;

initializer_list
	: initializer														{$$=$1;}
	| initializer_list ',' initializer									{$3->next=$1; $$=$3;}
	;

//////////////////////////////////////
// function definitions				//
//////////////////////////////////////

function_definition
	: func_1 declaration_list compound_statement						{$$=new_CStatement(STATEMENT_FUNCTION_DEFINITION);$$->insertCompound($1, $3);end_module(NULL);}
	| func_1 compound_statement											{$$=new_CStatement(STATEMENT_FUNCTION_DEFINITION);$$->insertCompound($1, $2);end_module(NULL);}
	| func_2 declaration_list compound_statement						{$$=new_CStatement(STATEMENT_FUNCTION_DEFINITION);$$->insertCompound($1, $3);end_module(NULL);}
	| func_2 compound_statement											{$$=new_CStatement(STATEMENT_FUNCTION_DEFINITION);$$->insertCompound($1, $2);end_module(NULL);}
	;

func_1
	:  declaration_specifiers declarator								{$$=$2; insert_module($2->m_cvdt->name.buf,1,0); }	//caused problems in parser: for some rease syntax in source code caused interpretation of functions with endless insert module calls!				
	;

func_2
	: declarator														{$$=$1; insert_module($1->m_cvdt->name.buf,1,0); }																						
	;

//////////////////////////////////////
// function Parameters				//
//////////////////////////////////////
parameter_type_list
	: parameter_list													{$$=$1.first;}							
	| parameter_list ',' ELLIPSIS										{$$=$1.first;}
	;

parameter_list
	: parameter_declaration												{$$.first=$1;$$.last=$1;}													
	| parameter_list ',' parameter_declaration							{if ($1.first!=NULL) if ($1.last!=NULL) $$.last=$1.last->set_next($3);}	
	;

parameter_declaration
	: declaration_specifiers declarator									{$$=$2;if ($2!=NULL) $2->m_token=IDENTIFIER_PARAMETER;}																		
	| declaration_specifiers abstract_declarator						{$$=$2;if ($2!=NULL) $2->m_token=IDENTIFIER_PARAMETER;}
	| declaration_specifiers											{$$=NULL;}
	;

identifier_list
	: IDENTIFIER													
	| identifier_list ',' IDENTIFIER								
	;

/*type_name
	: specifier_qualifier_list										
	| specifier_qualifier_list abstract_declarator					
	;glo
*/

abstract_declarator
	: cpointer															{$$=NULL; as.ycc_pointer_level=0;}						
    | direct_abstract_declarator									    {$$=NULL;}
	| cpointer direct_abstract_declarator								{$$=NULL; as.ycc_pointer_level=0;}
	;

// only type in function declaration i.e. int a(int, void*)
direct_abstract_declarator
	: '(' abstract_declarator ')'									
	| '[' ']'														
	| '[' constant_expression ']'									
	| direct_abstract_declarator '[' ']'							
	| direct_abstract_declarator '[' constant_expression ']'		
	| '(' ')'														
	| '(' parameter_type_list ')'									
	| direct_abstract_declarator '(' ')'							
	| direct_abstract_declarator '(' parameter_type_list ')'		
	;


//////////////////////////////////////
// ENUM								//
//////////////////////////////////////
doxy_comment_list
	: DOXYCOMMENT
	| doxy_comment_list DOXYCOMMENT
	;

enum_specifier
	: ENUM                                                                      {as.enum_cnt=0;startStructCompound(20);} 
      '{' enumerator_list brOrCol                                                 {$$=CDataType::newCDatatype(TYPE_ENUM); $$->insertElements($4); endStructCompound(20); /*$$->initEnum()*/}
	| ENUM IDENTIFIER                                                           {as.enum_cnt=0;startStructCompound(20); }
      '{' enumerator_list brOrCol                                               {$$=CDataType::newCDatatype($2,TYPE_ENUM); $$->insertElements($5); endStructCompound(20); /*$$->initEnum();*/}
	| ENUM IDENTIFIER															{$$=CDataType::newCDatatype($2,TYPE_ENUM);as.isEnumTypedf=false;dbgPrintf(TINFO,"isEnum=false\n");} 
	| ENUM TYPSPECIFIER															{$$=CDataType::newCDatatype($2,TYPE_ENUM);as.isEnumTypedf=false;dbgPrintf(TINFO,"isEnum=false\n");} 
	| ENUM IDENTIFIER doxy_comment_list												{$$=CDataType::newCDatatype($2,TYPE_ENUM);as.isEnumTypedf=false;dbgPrintf(TINFO,"isEnum=false\n");} 
	| ENUM TYPSPECIFIER	doxy_comment_list	 											{$$=CDataType::newCDatatype($2,TYPE_ENUM);as.isEnumTypedf=false;dbgPrintf(TINFO,"isEnum=false\n");} 
	; 
	
brOrCol
	: '}'																		{as.isEnumTypedf=false;dbgPrintf(TINFO,"isEnum=false\n");}
	| enum_separator '}'														{as.isEnumTypedf=false;dbgPrintf(TINFO,"isEnum=false\n");}
	| doxy_comment_list '}'															{as.isEnumTypedf=false;dbgPrintf(TINFO,"isEnum=false\n");}
	;

enumerator_list
	: enumerator																{$$=$1;} 
	| preprocessor_list enumerator													{$$=$2;}
    | enumerator_list enum_separator enumerator                                 {$$=$1; $1->set_last($3);}
	| enumerator_list preprocessor												{$$=$1; }

	;

enum_separator
	: enum_separator_prep
	| enum_separator_prep preprocessor_list
	;

enum_separator_prep
	: ','
	| ',' doxy_comment_list 
	;

enumerator
: IDENTIFIER																	{$$ = newCIdentifier($1); insertEnum($$->m_cvdt, as.enum_cnt); 
																				 as.enum_cnt++; $$->m_declspec = new decl_specifiers(as.type_long, 0, 0); }  //default as.declspec is overwritten
| IDENTIFIER { as.enumName = ""; }  '=' constant_expression						{$$ = newCIdentifier($1); insertEnum($$->m_cvdt, (long64)$4->GetData()); 
																				 as.enum_cnt = (long64)($4->GetData() + 1); $$->m_declspec = new decl_specifiers(as.type_long, 0, 0); }
																		 
;


//////////////////////////////////////
// STATEMENTS						//
//////////////////////////////////////


statement
: labeled_statement                                                         {$$=$1;er(0); $$->m_module_name = as.actual_module;}
	| compound_statement													{$$=$1;er(0); $$->m_module_name = as.actual_module;}
	| expression_statement													{$$=$1;er(0); $$->m_module_name = as.actual_module;}																																						
	| selection_statement													{$$=$1;er(0); $$->m_module_name = as.actual_module;}
	| iteration_statement													{$$=$1;er(0); $$->m_module_name = as.actual_module;}
	| jump_statement														{$$=$1;er(0); $$->m_module_name = as.actual_module;}
	| preprocessor															{$$=$1;er(0);}
	| error																	{$$=0; er(0);}											
	;

labeled_statement
    : IDENTIFIER ':' statement												{$$=new_CStatement(STATEMENT_LABEL); $$->m_identifier=newCIdentifier($1); $$->list=$3; $1->labeled_statement=$$->list; $$->m_identifier->m_token=IDENTIFIER_LABEL;}
    | CASE constant_expression ':' statement                                {$$=new_CStatement(STATEMENT_CASE);  $$->m_exp1=$2; $$->list=$4;}		             
	| DEFAULT ':' statement                                                 {$$=new_CStatement(STATEMENT_DEFAULT);              $$->list=$3;}
	;

compound_statement
	: '{' close_br2															{$$=new_CStatement(STATEMENT_COMPOUND_STATEMENT); $$->list=NULL;}									
	| '{' statement_list close_br2											{$$=new_CStatement(STATEMENT_COMPOUND_STATEMENT); $$->list=$2.first;if ($2.first!=NULL) $2.first->previous=$$;}								
//	| '{' declaration_list '}'												{$$=new_CStatement(STATEMENT_COMPOUND_STATEMENT); $$->list=$2.first;if ($2.first!=NULL) $2.first->previous=$$;}					
//	| '{' declaration_list statement_list '}'								{$$=new_CStatement(STATEMENT_COMPOUND_STATEMENT); $$->list=$2.first;if ($2.first!=NULL) $2.first->previous=$$; if ($2.last!=NULL)$2.last->set_next($3.first);}					
	;

declaration_list
	: declaration															{$$=$1;}
	| declaration_list declaration											{if ($1.last!=NULL) $$.last=$1.last->set_next(&($2));}
	;

/* changed from original: allow declaration within compound statement*/
statement_list
	: statement																{$$.first=$1;$$.last=$1;}
	| statement_list statement												{if ($1.last!=NULL) $$.last=$1.last->set_next($2);}
	| declaration															{$$=$1;}
	| statement_list declaration											{if ($1.last!=NULL) $$.last=$1.last->set_next(&($2));}
	;

expression_statement
    : ';'                                                                   {$$=new_CStatement(STATEMENT_EMPTY);}
	| cexpression estd														{$$=new_CStatement(STATEMENT_EXPRESSION);$$->m_exp1=$1;}													
	;

selection_statement
    : IF '(' cexpression ')' statement									    {$$=new_CStatement(STATEMENT_IF);     $$->m_exp1=$3; $$->list=$5;}
    | IF '(' cexpression ')' statement ELSE	statement                       {$$=new_CStatement(STATEMENT_IFELSE); $$->m_exp1=$3; $$->list=$5; $$->branch_to=$7;}
	| SWITCH '(' cexpression ')' statement							        {$$=new_CStatement(STATEMENT_SWITCH); $$->m_exp1=$3; $$->list=$5;}
	;

iteration_statement
	: WHILE '('  cexpression ')'  statement						            {$$=new_CStatement(STATEMENT_WHILE);   $$->m_exp1=$3; $$->list=$5;}
	| DO statement WHILE '(' cexpression ')' estd	                        {$$=new_CStatement(STATEMENT_DOWHILE); $$->m_exp1=$5; $$->list=$2;}
    | FOR '(' cexpression estd cexpression estd
                          cexpression')'  statement			                {$$=new_CStatement(STATEMENT_FOR); $$->m_exp1=$3; $$->m_exp2=$5; $$->m_exp3=$7; $$->list=$9;}
    | FOR '('estd estd')'  statement			                            {$$=new_CStatement(STATEMENT_FOR); $$->m_exp1=NULL; $$->m_exp2=NULL; $$->m_exp3=NULL; $$->list=$6;}
    
    ;

jump_statement
	: GOTO IDENTIFIER estd													{$$=new_CStatement(STATEMENT_GOTO); $$->m_identifier=newCIdentifier($2); $$->m_identifier->m_token=IDENTIFIER_LABEL;}      
	| CONTINUE estd															{$$=new_CStatement(STATEMENT_CONTINUE) 	;}
	| BREAK estd															{$$=new_CStatement(STATEMENT_BREAK)    	;}
	| RETURN estd															{$$=new_CStatement(STATEMENT_RETURN)   	;}
    | RETURN cexpression estd												{$$=new_CStatement(STATEMENT_RETURN);$$->m_exp1=$2;}
                                                                             //$$->m_module_name = as.actual_module   ;}
	;

//////////////////////////////////////
// Expressions						//
//////////////////////////////////////
cexpression
	: assignment_expression												{$$=$1;}
	| cexpression ',' assignment_expression								{$$=CExpression::makeCommata($1, $3);}
	;

assignment_expression
	: conditional_expression											{$$=$1;}
	| unary_expression assignment_operator assignment_expression		{$$=CExpression::makeAssignmentOp($1,(long)$2,$3);} // mixed form, like +=
	;

assignment_operator
	: '='																{$$=OP_ASSIGN			  ;}				
	| YOP_MUL_ASSIGN													{$$=OP_MUL_ASSIGN		  ;}			  
	| YOP_DIV_ASSIGN													{$$=OP_DIV_ASSIGN	      ;}              
	| YOP_MOD_ASSIGN													{$$=OP_MOD_ASSIGN	      ;}              
	| YOP_ADD_ASSIGN													{$$=OP_ADD_ASSIGN	      ;}              
	| YOP_SUB_ASSIGN													{$$=OP_SUB_ASSIGN	      ;}              
	| YOP_SHIFT_LEFT_ASSIGN												{$$=OP_SHIFT_LEFT_ASSIGN  ;}              
	| YOP_SHIFT_RIGHT_ASSIGN	                                   		{$$=OP_SHIFT_RIGHT_ASSIGN ;}              
	| YOP_AND_ASSIGN	                                           		{$$=OP_AND_ASSIGN	      ;}              
	| YOP_XOR_ASSIGN	                                           		{$$=OP_XOR_ASSIGN	      ;}              
	| YOP_OR_ASSIGN		                                           		{$$=OP_OR_ASSIGN		  ;}              								
	;	
	
conditional_expression
	: logical_or_expression												{$$=$1;}
	| logical_or_expression '?' cexpression ':' conditional_expression	{$$=CExpression::makeTernaryOp($1,$3,$5);}	
	;

constant_expression
	: conditional_expression											{$$=$1;}
	;

logical_or_expression
	: logical_and_expression											{$$=$1;}
	| logical_or_expression YOP_LOGIG_OR logical_and_expression			{$$=CExpression::makeLogicalOr($1, $3);}	
	;

logical_and_expression
	: inclusive_or_expression											{$$=$1;}
	| logical_and_expression YOP_LOGIC_AND inclusive_or_expression		{$$=CExpression::makeLogicalAnd($1, $3);}	
	;

inclusive_or_expression
	: exclusive_or_expression											{$$=$1;}
	| inclusive_or_expression '|' exclusive_or_expression				{$$=CExpression::makeArithmeticOr($1, $3);}	
	;

exclusive_or_expression
	: and_expression													{$$=$1;}
	| exclusive_or_expression '^' and_expression						{$$=CExpression::makeArithmeticXor($1, $3);}				
	;
	
and_expression
	: equality_expression												{$$=$1;}														
	| and_expression '&' equality_expression							{$$=CExpression::makeArithmeticAnd($1, $3);}		
	;	
	 
equality_expression
	: relational_expression												{$$=$1;}
	| equality_expression  YOP_EQUAL     relational_expression		    {$$=CExpression::makeCompEqual($1,$3);}		
	| equality_expression  YOP_NOT_EQUAL relational_expression			{$$=CExpression::makeCompNotEqual($1,$3);}		
	;
	
relational_expression
	: shift_expression													{$$=$1;}
	| relational_expression '<' shift_expression						{$$=CExpression::makeCompLess($1,$3);}						
	| relational_expression '>' shift_expression						{$$=CExpression::makeCompGreater($1,$3);}							
	| relational_expression YOP_LESS_EQ shift_expression				{$$=CExpression::makeCompLessEq($1,$3);}						
	| relational_expression YOP_GREATER_EQ shift_expression				{$$=CExpression::makeCompGreaterEq($1,$3);}
	;	

shift_expression
	: additive_expression												{$$=$1;}
	| shift_expression YOP_SHIFT_LEFT additive_expression				{$$=CExpression::makeShiftLeft($1,$3); }					
	| shift_expression YOP_SHIFT_RIGHT additive_expression				{$$=CExpression::makeShiftRight($1,$3);}					
	;

additive_expression
	: multiplicative_expression											{$$=$1; }
	| additive_expression '+' multiplicative_expression					{$$=CExpression::makePlus($1,$3);}
	| additive_expression '-' multiplicative_expression					{$$=CExpression::makeMinus($1,$3);}
	;

multiplicative_expression
	: cast_expression													{$$=$1;}
	| multiplicative_expression '*' cast_expression						{$$=CExpression::makeMult($1,$3);} 
	| multiplicative_expression '/' cast_expression						{$$=CExpression::makeDiv($1,$3);} 		
	| multiplicative_expression '%' cast_expression						{$$=CExpression::makeModulo($1,$3);}		
	;
	
cast_expression
	: unary_expression													{$$=$1;}
	| '(' TYPSPECIFIER ')' cast_expression								{$$=CExpression::makeTypecast($2,OP_TYPECAST,$4);}						    
	| '(' TYPSPECIFIER '*' ')' cast_expression							{$$=CExpression::makeTypecast($2,OP_TYPECASTP,$5);} // FIXME: and what about (typspec **)? add different typespec which includes pointers						    
    ;

unary_expression
	: postfix_expression												{$$=$1;}					
	| YOP_PLUS_PLUS unary_expression									{$$=CExpression::makePreIncrement($2);}
	| YOP_MINUS_MINUS unary_expression									{$$=CExpression::makePreDecrement($2);}
	| unary_operator cast_expression									{$$=CExpression::makeUnaryOp((long)$1,$2);}														
	| SIZEOF unary_expression											{$$=NULL;}
	| SIZEOF '(' TYPSPECIFIER ')'										{$$=CExpression::makeSizeof($3);}	
	;

unary_operator
    : '&'																{$$=OP_UNARY_AND    ;}		
	| '*'															    {$$=OP_UNARY_STAR   ;}    
	| '+'															    {$$=OP_UNARY_PLUS   ;}    
	| '-'															    {$$=OP_UNARY_MINUS  ;}    
	| '~'															    {$$=OP_UNARY_NEGATE ;}    
    | '!'															    {$$=OP_UNARY_INVERT ;}    
    ;                                                                                             


/****************************/
/* 	Variable access 		*/
/****************************/

primary_expression													
    : IDENTIFIER													    {if (($1->identifier!=NULL) && ($1->identifier->GetTypeId() == TYPE_ENUM))  $$=CExpression::makeConstant($1->identifier->m_cvdt->data,OP_CONST_INT);
                                                                         else $$=CExpression::makeIdentifier($1,true);}
	| constant															{$$=$1;}																							
	| '(' cexpression close_br											{$$=$2;}					
	;


function_call
	: postfix_expression '('											{$$=$1;}
	;

postfix_expression
	: primary_expression												{$$=$1;}						
	| postfix_expression '[' cexpression ']'							{$$=CExpression::makeArrayAccess($1, $3);}
	| postfix_expression '[' cexpression ']' '[' cexpression ']'		{$$=CExpression::makeArrayAccess2($1, $3, $6);}
    | function_call ')'													{$$=CExpression::makeFunCall($1,NULL);}																
	| function_call argument_expression_list ')'						{$$=CExpression::makeFunCall($1,$2.first);}

    | postfix_expression YOP_DOT {$1->startStruct();} IDENTIFIER		{$$=CExpression::makeDot($1,$4);$1->endStruct(); }
                                    




    | postfix_expression YOP_ARROW {$1->startStruct();} IDENTIFIER		{$$=CExpression::makeArrow($1,$4);$1->endStruct();}
	| postfix_expression YOP_PLUS_PLUS									{$$=CExpression::makePostIncrement($1);}																
	| postfix_expression YOP_MINUS_MINUS								{$$=CExpression::makePostDecrement($1);}		
	;
						
						
argument_expression_list
	: assignment_expression												{$$.first=$1;$$.last=$$.first;}															
	| argument_expression_list ',' assignment_expression				{$$.last=$1.last->set_next($3);}
	;

constant
	: INUM																{$$=CExpression::makeConstant((long64)$1,OP_CONST_INT);}
	| UNUM																{$$=CExpression::makeConstant((unsigned long64)$1,OP_CONST_INT);}
	| FNUM																{$$=CExpression::makeConstant($1,OP_CONST_FLOAT);}
	| TSTRING														    {$$=CExpression::makeStringLiteral(*$1);}

    ;
 

/* end statement*/
estd
	:	{er(901);} attributes											{$$=$2;}							
	|	{er(901);} error  attributes									{$$=$3;}
	|	{er(901);} preprocessor_list {er(901);} attributes 				{$$=$4;}						
	;

/* end statement*/
attributes
	: ';'								{$$=0;}
	| ATTRIBUTE ';'						{$$=1;}
	;

/* close brackets*/
close_br
	:	{er(904);} ')'
	|	{er(904);} error  ')'											
	;

close_br2
	:	{er(905);} '}'
	//|	{er(905);} error  '}'											
	;


/****************************/
/* 	Preprocessor 			*/
/****************************/
preprocessor_list
	: preprocessor																										
	| preprocessor_list preprocessor				
	;

preprocessor
	:		PREPROC_IF cexpression 	EOLC								{$$=new_CStatement(STATEMENT_PREPROC_IFDEF)    	;int n=0;if ($2->getConstant()==0) n=1;CvdtAddToBegin(as.iflist,n); DoSkip(n,1);}
	|		PREPROC_IFDEF IDENTIFIER EOLC								{$$=new_CStatement(STATEMENT_PREPROC_IFDEF)    	;int n=1;if ($2->token==TOKEN_DEFINE) n=0;CvdtAddToBegin(as.iflist,n); DoSkip(n,2);}
	|		PREPROC_IFNDEF IDENTIFIER EOLC								{$$=new_CStatement(STATEMENT_PREPROC_IFDEF)    	;int n=1;if ($2->token!=TOKEN_DEFINE) n=0;CvdtAddToBegin(as.iflist,n); DoSkip(n,3);}
	|		PREPROC_IFDEF constant EOLC									{$$=new_CStatement(STATEMENT_PREPROC_IFDEF)    	;int n=0;CvdtAddToBegin(as.iflist,n); DoSkip(n,4);}
	|		PREPROC_IFNDEF constant EOLC								{$$=new_CStatement(STATEMENT_PREPROC_IFDEF)    	;int n=1;CvdtAddToBegin(as.iflist,n); DoSkip(n,5);}
	|		PREPROC_INCLUDE TSTRING										{$$=new_CStatement(STATEMENT_PREPROC_IFDEF)    	;as.include=$2->buf;as.do_include=true;}
	|		PREPROC_DEFINE IDENTIFIER									{lex_begin_cdefine();}
			defineArg													{$2->token=TOKEN_DEFINE;$$=new_CStatement(STATEMENT_PREPROC_DEFINE); if ($4) {$2->valstr=$4->buf;} else {$2->valstr="";} } 
	|		PREPROC_UNDEFINE IDENTIFIER									{$$=new_CStatement(STATEMENT_PREPROC_DEFINE); 
																		 if ($2->token==TOKEN_DEFINE || $2->token==TOKEN_UNDEFINED) {$2->name=$2->name+"__deleted__";}}
	;

	defineArg
	:		PREPROC_DEFINESTR											{ $$=$1; lex_begin_last_state();}
	|		EOLC														{ $$=NULL;lex_begin_last_state();}
%%

/************************************************************************************************/
/* Start MAIN and Subroutines									*/
/************************************************************************************************/



