%{
/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////////////
//! \brief					 LEX generated file													\file
////////////////////////////////////////////////////////////////////////////////////////////////////
%}

%top{
	#include <stdint.h> 
}  

%option never-interactive 
%option case-insensitive
     
%{ 
	#include "stdio.h"
	#include "h2xml_global.h"
	#include "global_functions.h"
	#include "scanAnnotations.h"
	#include "scanConfig.h"
	#include "CAnnotationData.h"
	#include "CValue.h"
	#include <string>


	#define YY_USER_INIT lexInitDoxy()
	#define MAX_LEX_STATES 20
	#define STRADDLENGTH 500

	long statesDoxy[MAX_LEX_STATES];
	long lexStateIndexDoxy = 0;
	long lexPushStateDoxy(long state);
	long lexPopStateDoxy();
	void lexChangeStateDoxy(long state);
	void endStateParamBody();					// will be called at end of statement
	void pushXmlState(eAnArgTypes arg);
	long InsertBufferDoxy(Cvdt* &current_vdt, char* prolog = NULL);
	extern Cvdt* getvdt(char* name);
	void scanStringbuffer(char* string);
	int endOffile();
	long64 getValueFromString(char* string, int base);
	double getfloatValueFromString(char* string);


	int stop();
	void lexInitDoxy();
	char stateStrDoxy[][20] = { "INITIAL", "HTMLCOMMENT","MODULE_DEF","MODULE_BODY", "ARG_BRACKET", "ARG_LABEL", "ARG_STRING","ARG_INTEGER", "ARG_BOOL", "ARG_RANGE", "ARG_RANGE_STRING1", "ARG_RANGE_STRING2", "ARG_RANGE_LIST"};
	bool popStateAtExit=false;
	CConfigAttributes* dscattr;
	void pushXmlStateBracket();
	int doxyStateTable[30];
	bool insertSpace=false;
	CStr enumStr="";
    CStr identName = "";
    bool doNotResoveEnums = false;

	struct incstructAn {
		YY_BUFFER_STATE buf_stack;	//Stack fuer .incl processing
		incstructAn* next;
	};
	incstructAn* incstrAn=NULL;

%}
	


WSPACE		[ \t\x0d]+
LABEL_		[a-zA-Z_][a-zA-Z0-9_.]*
LABEL		[a-zA-Z_]([a-zA-Z0-9_.]|::)*
LABELQ		[a-zA-Z_]([a-zA-Z0-9_.]|::|\")*
LABEL2		[^\x0d\x0a"]*
LABEL3		[^\x0d\x0a']*
STRING		[a-zA-Z0-9_\-()][a-zA-Z0-9_.\-()]*  
INUM		[0-9]+(u|U|l|L)?
INUMS		-[ \t]*[0-9]+(l|L)?
D			[0-9]
E			[Ee][+-]?[0-9]+
FS			(f|F|l|L)
FNUM1		[+-]?{D}+{E}{FS}?
FNUM2		[+-]?{D}+"."{D}+({E})?{FS}?
FNUM		(({FNUM1})|({FNUM2}))
HEXNUM		0x[0-9a-fA-F]+(u|U|l|L)? 
HEXNUMS		-[ \t]*0x[0-9a-fA-F]+(l|L)?  
BINNUM1		%[01]+
BINNUM2		0b[01]+
NL			\x0d\x0a|\x0a\x0d|\x0a
DEFINESTR_	([^\x0d\x0a/]+"/"{0,1})*
DEFINESTR	[^\x0d\x0a]*


%x HTMLCOMMENT MODULE_DEF MODULE_BODY ARG_BRACKET ARG_LABEL ARG_STRING ARG_INTEGER ARG_BOOL ARG_RANGE ARG_RANGE_STRING1 ARG_RANGE_STRING2 ARG_RANGE_LIST ARG_RANGE_KEY

/*
		State Machine:

		INITIAL->MODULE_DEF->MODULE_BODY->DO_NOTHING ->		end_comment=pop state
		MODULE_BODY->PARAM_DEF ->							end_comment=pop state
		MODULE_BODY->ELEMENT_DEF ->							end_comment=pop state
		MODULE_BODY->@}										end_comment=pop state
*/

%%

 /* HTML comment */
<*>"<!--"									{lexPushStateDoxy(HTMLCOMMENT);}
<HTMLCOMMENT>"-->"							{lexPopStateDoxy();}
<HTMLCOMMENT>[^->\x0d\x0a]*
<HTMLCOMMENT>{NL}							{as.linecnt++;gd->linecnt++;}
<HTMLCOMMENT>.


 /* fixed keyword processing*/
 "@{"										{lexBeginModule();}
 "@}"										{
											lexEndModule();
											;} 
  
 /* variable keyword processing */
@{LABELQ}/{WSPACE}*"{"						{evaluateKeyword(&yyannottext[1],true);}
@{LABELQ}									{evaluateKeyword(&yyannottext[1],false);}
@{LABELQ}/{WSPACE}*"{"{WSPACE}*"}"			{evaluateKeyword(&yyannottext[1],false);}


 
<ARG_BRACKET>{WSPACE}*"{"{WSPACE}*			{lexPopStateDoxy();}
<ARG_BRACKET>[^{]*							{gd->dbgPrintf(TERROR,"Expecxting '{', found %s\n",yyannottext);lexPopStateDoxy();}

 /* Argument processing String, can be multi-Line. Leading, trailing blanks are removed per line*/
<ARG_STRING>[^\x0d\x0a}< \t\\]*				{gd->xD->m_assignmentList->m_string.stradd(yyannottext, STRADDLENGTH); insertSpace=true;}
<ARG_STRING>^{WSPACE}*						{insertSpace=false;}																											/*ignore space at begin of line*/
<ARG_STRING>{WSPACE}*{NL}					{as.linecnt++;gd->linecnt++; if(insertSpace) {gd->xD->m_assignmentList->m_string.stradd(" ", STRADDLENGTH);} insertSpace=false; }				/* ignore space at end of line*/
<ARG_STRING>"\\n"							{gd->xD->m_assignmentList->m_string.stradd(yyannottext, STRADDLENGTH);insertSpace=false;}											/* ignore space at end of line, don't insert blank*/
<ARG_STRING>{WSPACE}*"}"					{gd->xD->m_assignmentList->m_argType=ANARG_STRING;lexPopStateDoxy();insertSpace=false; gd->xD->m_knArgumentsIn++; endStateArgument();}
<ARG_STRING>{WSPACE}*/"<!--"				{}																																/* ignore blanks before commment */
<ARG_STRING>{WSPACE}*						{gd->xD->m_assignmentList->m_string.stradd(yyannottext, STRADDLENGTH);insertSpace=false;}
<ARG_STRING>.								{gd->xD->m_assignmentList->m_string.stradd(yyannottext, STRADDLENGTH);insertSpace=true;}											


 /*Range Argument progessing*/
<ARG_RANGE>{INUM}						{gd->xD->insertValues(ANARG_UNSIGNED_INTEGER, yyannottext, getValueFromString(yyannottext,10),0);lexChangeStateDoxy(ARG_RANGE_LIST);}
<ARG_RANGE>{INUMS}						{gd->xD->insertValues(ANARG_INTEGER, yyannottext, getValueFromString(yyannottext,10),0);lexChangeStateDoxy(ARG_RANGE_LIST);}
<ARG_RANGE>{HEXNUM}						{gd->xD->insertValues(ANARG_UNSIGNED_INTEGER, yyannottext, getValueFromString(yyannottext,16),0,ANARG_DPLY_HEX);lexChangeStateDoxy(ARG_RANGE_LIST);}
<ARG_RANGE>{HEXNUMS}					{gd->xD->insertValues(ANARG_INTEGER , yyannottext, getValueFromString(yyannottext,16),0,ANARG_DPLY_HEX);lexChangeStateDoxy(ARG_RANGE_LIST);}
<ARG_RANGE>{FNUM}						{gd->xD->insertValues(ANARG_DOUBLE, yyannottext, 0, getfloatValueFromString(yyannottext));lexChangeStateDoxy(ARG_RANGE_LIST);}
<ARG_RANGE>true							{gd->xD->insertValues(ANARG_BOOL, yyannottext, 1,0);lexChangeStateDoxy(ARG_RANGE_LIST);}
<ARG_RANGE>false						{gd->xD->insertValues(ANARG_BOOL, yyannottext, 0,0);lexChangeStateDoxy(ARG_RANGE_LIST);}
<ARG_RANGE>({LABEL}@{LABEL})|(\"{LABEL}\"@{LABEL}) {gd->xD->insertValues(ANARG_REFERENCE, yyannottext, 0, 0);
                                        lexChangeStateDoxy(ARG_RANGE_LIST); }
<ARG_RANGE>@{LABEL}                     {gd->xD->insertValues(ANARG_LABEL, yyannottext, 0,0);}
<ARG_RANGE>(\"{LABEL2})					{if (enumStr.isEmpty()) {enumStr=yyannottext; enumStr.RemoveQuotes();} gd->xD->insertValues(ANARG_LABEL, yyannottext, 0,0);lexChangeStateDoxy(ARG_RANGE_STRING1);}
<ARG_RANGE_STRING1>{NL}					{as.linecnt++;gd->linecnt++;}
<ARG_RANGE_STRING1>{LABEL2}				{if (enumStr.isEmpty()) {enumStr.stradd(yyannottext); enumStr.RemoveQuotes();} gd->xD->insertValues(ANARG_LABEL, yyannottext, 0,0,ANARG_DPLY_NORMAL,true);lexChangeStateDoxy(ARG_RANGE_STRING1);}
<ARG_RANGE_STRING1>\"					{lexChangeStateDoxy(ARG_RANGE_LIST);}
<ARG_RANGE>(\'{LABEL3})					{if (enumStr.isEmpty()) {enumStr=yyannottext; enumStr.RemoveQuotes();} gd->xD->insertValues(ANARG_LABEL, yyannottext, 0,0);lexChangeStateDoxy(ARG_RANGE_STRING2);}
<ARG_RANGE_STRING2>{NL}					{as.linecnt++;gd->linecnt++;}
<ARG_RANGE_STRING2>{LABEL3}				{if (enumStr.isEmpty()) {enumStr.stradd(yyannottext); enumStr.RemoveQuotes();} gd->xD->insertValues(ANARG_LABEL, yyannottext, 0,0,ANARG_DPLY_NORMAL,true);lexChangeStateDoxy(ARG_RANGE_STRING1);}
<ARG_RANGE_STRING2>\'					{lexChangeStateDoxy(ARG_RANGE_LIST);}


<ARG_RANGE>{LABEL}						{Cvdt* tmpvdt; if (enumStr.isEmpty()) {enumStr=yyannottext;}
										int token=InsertBufferDoxy(tmpvdt);
										if (token<0) {
											gd->xD->insertValues(ANARG_LABEL, yyannottext, 0,0);
                                            lexChangeStateDoxy(ARG_RANGE_LIST);}}


<ARG_RANGE>"}"							{lexPopStateDoxy(); endStateArgument();}
<ARG_RANGE_LIST>";"|".."|"="|","		{lexChangeStateDoxy(ARG_RANGE); gd->xD->addValueToEnd();}
<ARG_RANGE_LIST>[^;.=, \t\x0d\x0a}]*		{gd->dbgPrintf(TERROR,"Argument list: <%s>: Multiple arguments without proper delimiter (';' or ',' or'=' or '..')\n",yyannottext);}
<ARG_RANGE_LIST>"}"						{lexPopStateDoxy(); endStateArgument();}


 /* all thats left */
<*>{WSPACE}									/*Ignore Whitespace*/
<*>{NL}										{as.linecnt++;gd->linecnt++; }
<*>.										{ }
%%

void lexInitDoxy()
{
	memset(doxyStateTable,0,sizeof(doxyStateTable));
	doxyStateTable[ANARG_STRING]=ARG_STRING;
	doxyStateTable[ANARG_INTEGER]=ARG_INTEGER;
}

/* yywrap: Called when EOF is reached on current input.
* Have it return 1 if processing is finished, or
* do something to fix the EOF condition (like open
* another file and point to it) and return 0 to indicate
* more input is available.
*/
int yywrap()
{
	return endOffile();
}

int stop()
{
	lexChangeStateDoxy(INITIAL);
	lexStateIndexDoxy = 0;
	return 0;
}

void pushXmlStateBracket()
{
	lexPushStateDoxy(ARG_BRACKET);
}

long lexPushStateDoxy(long state)
{
	statesDoxy[lexStateIndexDoxy] = YYSTATE;
	lexStateIndexDoxy++;
	if (lexStateIndexDoxy >= MAX_LEX_STATES) {
		printf("LEX maximum number of lex states reached!\n");
		if (yyannot_flex_debug) {
			fprintf(stderr, "LEX maximum number of lex states reached!\n");
		}
		lexStateIndexDoxy--;
	}
	else {
		BEGIN state;
	}
	if (yyannot_flex_debug) {
		fprintf(stderr, "Flex new state (push): %s %d %ld\n", stateStrDoxy[YYSTATE], YYSTATE, lexStateIndexDoxy - 1);
		//printf("Flex new state (push): %s %d %ld\n", stateStrDoxy[YYSTATE], YYSTATE, lexStateIndexDoxy - 1);
	}
	return lexStateIndexDoxy;
}

void lexChangeStateDoxy(long state)
{
	lexPopStateDoxy();
	lexPushStateDoxy(state);
}

long lexPopStateDoxy()
{
	lexStateIndexDoxy--;
	BEGIN statesDoxy[lexStateIndexDoxy];
	if (lexStateIndexDoxy<0) {
		printf("LeX state stack <0!\n");
		if (yyannot_flex_debug) {
			fprintf(stderr, "LEX state stack <0!\n");
		}
		lexStateIndexDoxy = 0;
	}
	if (yyannot_flex_debug) {
		fprintf(stderr, "Flex new state (pop): %s %d %ld\n", stateStrDoxy[YYSTATE], YYSTATE, lexStateIndexDoxy);
		//printf("Flex new state (pop): %s %d %ld\n", stateStrDoxy[YYSTATE], YYSTATE, lexStateIndexDoxy);
	}
	return lexStateIndexDoxy;
}

void pushXmlState(eAnArgTypes arg) {
    doNotResoveEnums = false;
	switch (arg) {
		case ANARG_NONE:                 
		case ANARG_IGNORE:
			break;              
		case ANARG_STRING:   
			lexPushStateDoxy(ARG_STRING);
			lexPushStateDoxy(ARG_BRACKET);    
			break;       
		case ANARG_LABEL:
        case ANARG_ENUM_ELEMENT:
		case ANARG_ENUM_TYPE:
		case ANARG_CONSTLABEL:
		case ANARG_BOOL:                
		case ANARG_INTEGER:             
		case ANARG_UNSIGNED_INTEGER:    
		case ANARG_DOUBLE:    
		case ANARG_VARIANT:                
			lexPushStateDoxy(ARG_RANGE);
			lexPushStateDoxy(ARG_BRACKET);   
			break; 
		default:
		break;
	}         
}

void checkStateAtEndOfDoxyComment() {
	switch (YYSTATE) {
		case ARG_BRACKET:
			gd->dbgPrintf(TERROR, "Argument missing\n");
			break;
		case ARG_STRING:
			gd->dbgPrintf(TERROR, "Unterminated String, '}' missing\n");
			break;
		case ARG_RANGE:
			gd->dbgPrintf(TERROR, "Argument '}' missing\n");
			break;
		case ARG_RANGE_LIST:
			gd->dbgPrintf(TERROR, "Argument '}' missing\n");
			break;
		case INITIAL:
			break;
	default:
			gd->dbgPrintf(TERROR, "Doxy comment wrong state at end of comment\n");
			break;
	}

	while (YYSTATE!=0) {
		lexPopStateDoxy();
	}
}

void endStateParamBody() {
	if (yyannot_flex_debug) {
		fprintf(stderr, "Flex End Parameter Body\n");
	}
}

long InsertBufferDoxy(Cvdt* &current_vdt, char* prolog )
{

	char* ptyytext=yyannottext;
	if (prolog!=NULL) {
		ptyytext+=strlen(prolog);
	}
			

	long token=-1;
	
	current_vdt=getvdt(ptyytext);

	if (current_vdt!=NULL && (current_vdt->token==TOKEN_DEFINE )) {
        if (current_vdt->isEnumElement ){  
            gd->xD->insertValues(ANARG_LABEL, yyannottext, 0, 0, ANARG_DPLY_NORMAL,true);
        }
        if (current_vdt->valstr.buf != NULL) {
            scanStringbuffer(current_vdt->valstr.buf);
        }
        token = 0;
	}
	return token;
}

void scanStringbuffer(char* string)
{
	// push carrent state on stack
	incstructAn* tmp=new incstructAn;
	tmp->next=incstrAn;
	incstrAn=tmp;
	incstrAn->buf_stack=YY_CURRENT_BUFFER;
    gd->macroLevel++;
	yyannot_scan_string(string);
}

int endOffile()
{
    //gd->dbgPrintf(TINFO, "End of inserted string\n");
    gd->macroLevel--;
    if (gd->macroLevel < 0) {
        gd->dbgPrintf(TERROR, "Internal error, macro level (%d)  <0\n", gd->macroLevel);
    }
	int retval=1;
	if (incstrAn!=NULL) {
		yy_delete_buffer(YY_CURRENT_BUFFER);
		yy_switch_to_buffer(incstrAn->buf_stack);
		incstructAn* tmp=incstrAn->next;
		delete incstrAn;
		incstrAn=tmp;
		retval=0;
	}
	return retval;
}

long64 getValueFromString(char* string, int base)
{
	errno=0;
	long64 value=0;
	try {
		value=strtoull(string,NULL,base);
	}
	catch (...) { 
	}
	if (errno==ERANGE) {
		gd->dbgPrintf(TERROR,"Value <%s> out of range\n",string);
	}
	return value;
}

double getfloatValueFromString(char* string)
{
	errno=0;
	double value = 0;
	try {
		value=std::stod(string);
	}
	catch (...) { 
	}
	if (errno==ERANGE) {
		gd->dbgPrintf(TERROR,"Value <%s> out of range\n",string);
	}
	return value;
}