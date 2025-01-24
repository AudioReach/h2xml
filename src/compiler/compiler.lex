%{
/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////////////
//! \brief					 LEX generated file													\file
////////////////////////////////////////////////////////////////////////////////////////////////////
%}

%option never-interactive 

%top{
	#include <stdint.h> 
}     

%{ 
	#include "h2xml_common.h"
	#include "h2xml_global.h"
	#include "global_functions.h"
	#include "compilerYaccyy.hpp" 

	extern void newline_ydfpcc();

	Cvdt* current_vdt;
	static int linecnt=0;
	static int inline_cnt=0;
	int removeBlanks;
	int commentStateSave;
	bool doxygenSingleLine=false;

	long InsertBuffer(Cvdt* &current_vdt, char* prolog=NULL, char* buffer=yycompilertext);
	bool ChangeActiveModule(Cvdt* in);
	void insert_module(char* name, int mode=0, bool is_struct=0);
	void end_module(char* name=NULL);
	void update_mod_names(CStr &name,Cvdt* list);
	char* getname(CStr* module, char* text);
	Cvdt* getvdt(char* name);								// search all levels
	void unputc(const char* in, char* buffer=yycompilertext);
	void endif(int k);
	void lex_begin_comment();
	void check_version(double version,int mode=0);			//mode: no error if ydfp version:  0->greater or equal
	void lex_begin_last_state();
	void lex_begin_initial(int save_state);
	void lex_begin_readline_ydfpcc();
	void lex_begin_cdefine();
	void lex_begin_cdoxygen();
	void lex_end_cdoxygen();
    void lex_begin_include_os();
	void lex_begin_state(int state);
	void checkAddText(CStr* last);
	void lex_begin_add_text(int mode=0);
    bool checkDefined(char* buffer);        /* defined(name) */
    bool checkDefined2(char* buffer);       /* defined name */

	void DoSkip(int k, int from=0);
	int endoffile();
	void process_include(char* name);
	void generateInputFileNames(char* name, int line);
	void selectIdentifier(char* text);
	void copyIdentifier(char* text);
	void nameUnnamedStruct(char* text);
	void lex_WriteToDoxygenParser(char* text);

	
	extern int yyannotlex();
	extern YY_BUFFER_STATE yyannot_scan_string(const char * str);
	extern void yyannot_delete_buffer(YY_BUFFER_STATE buffer);
	extern void startDoxyComment();
	extern void endDoxyComment();

	//! struct for the include processing (generated from lex-file)
	struct incstruct {
		int linecnt;
		CStr filename;
		CStr pathname;
		CStr inputname;
		YY_BUFFER_STATE buf_stack;	//Stack fuer .incl processing
		FILE* infile;
		incstruct* next;
	};
	incstruct* incstr=NULL;
	char stateName[][30]={"INITIAL","IFC1", "__ATTR__","SKIP_IN CCOMMENT","CDOXYGEN","READLINE_YDFPCC","SKIP_INC","CDEFINE","ASM_MID","ADDTEXT","ASM_GETVALUE","ASM_GETLIST","INLINE"};
	bool doResolveDefines=true;

	char moduleSeparatorChar='#';
	char moduleSeparator[]="#";
    int include_os_state;

%}
	// original LEX definition for constants
	//D			[0-9]
	//L			[a-zA-Z_]
	//H			[a-fA-F0-9]
	//E			[Ee][+-]?{D}+
	//FS		(f|F|l|L)
	//IS		(u|U|l|L)*
	//[xX]{H}+{IS}?				{ return(CONSTANT); }
	//0{D}+{IS}?				{ return(CONSTANT); }
	//{D}+{IS}?					{ return(CONSTANT); }
	//L?'(\\.|[^\\'])+'			{ return(CONSTANT); }
	//{D}+{E}{FS}?				{ return(CONSTANT); }
	//{D}*"."{D}+({E})?{FS}?	{ return(CONSTANT); }
	//{D}+"."{D}*({E})?{FS}?	{ return(CONSTANT); }


WSPACE		[ \t]+
LABEL		[a-zA-Z_][a-zA-Z0-9_.]*
LABEL_		[a-zA-Z_]([a-zA-Z0-9_.]|::)*
YCC_IDENTIFIER	[a-zA-Z_][a-zA-Z0-9_]*
RLABEL		[a-zA-Z_(][^}]*
RLABEL2		[^}]*
STRING		[a-zA-Z0-9_\-()][a-zA-Z0-9_.\-()]*  
STRINGPR	[^\x0d\x0a!]* 
STRINGSTA	[^\x0d\x0a!;:]* 
INUM		[0-9]+(u|U|l|L)?
INUMS		-?[0-9]+(u|U|l|L)?
FNUM		(([0-9]+"."[0-9]*)|([0-9]*"."[0-9]+))("."f)? 
HEXNUM2		0x[0-9a-fA-F]+(u|U|l|L)?  
HEXNUM3		h[0-9a-fA-F]+(u|U|l|L)?
BINNUM1		%[01]+
BINNUM2		0b[01]+
NL			\x0d\x0a|\x0a\x0d|\x0d|\x0a
FILENAME	.+
FILENAME_OLD	[a-zA-Z0-9_.\\/:\-]+
DEFINESTR_	([^\x0d\x0a/]+"/"{0,1})*
DEFINESTR	[^\x0d\x0a]*

%s IFC1 __ATTR__

%x SKIP_IN CCOMMENT CDOXYGEN READLINE_YDFPCC SKIP_INC CDEFINE ASM_MID ADDTEXT ASM_GETVALUE ASM_GETLIST INLINE INCLUDE_OS

%%

 /* general Roules */
<READLINE_YDFPCC>.*					{as.ycc_code=_strdup(yycompilertext);yyless(0);yy_set_bol(1);lex_begin_last_state();
									if (as.fpStderr) {
										fflush(as.fpStderr);
									}}	
<READLINE_YDFPCC>{NL}				{yyless(0);yy_set_bol(1);lex_begin_last_state();} 

 /* doxygen comments */
(("///<")|("//#<")|("//!<"))		{lex_begin_cdoxygen(); as.connect_previous = true; doxygenSingleLine=true; if (as.isEnumTypedf) {return DOXYCOMMENT;}}
(("///")|("//#")|("//!"))			{lex_begin_cdoxygen(); as.connect_previous = false; doxygenSingleLine=true; }

(("/**")|("/*!")|("/*#")){WSPACE}*<	{lex_begin_cdoxygen();as.connect_previous=true;if (as.isEnumTypedf) {return DOXYCOMMENT;}}
("/**")|("/*!")|("/*#")				{lex_begin_cdoxygen(); as.connect_previous = false; }
<CDOXYGEN>"*/"						{lex_end_cdoxygen(); }
<CDOXYGEN>.							{as.doxycomment.stradd(yycompilertext);}
<CDOXYGEN>[^\*\x0d\x0a]*			{as.doxycomment.stradd(yycompilertext);}
<CDOXYGEN>{NL}						{as.doxycomment.stradd(yycompilertext);linecnt++;as.linecnt++;gd->linecnt++; 
									 if (doxygenSingleLine) {lex_end_cdoxygen();} doxygenSingleLine=false;}

 /*comment*/
"//"[^/\#!\x0d\x0a].*				/*Ignore Comments*/
"//"/{NL}							/*Ignore Comments*/
"/*"								{commentStateSave=YYSTATE; lex_begin_comment(); }
<CCOMMENT>"*/"						{lex_begin_state(commentStateSave); }
<CCOMMENT>.
<CCOMMENT>{NL}						{linecnt++; as.linecnt++;gd->linecnt++; }

<*>{WSPACE}							/*Ignore Whitespace*/
<IFC1>\\{NL}						{linecnt++;as.linecnt++;gd->linecnt++;} /*multi line*/
<IFC1>{NL}							{linecnt++;newline_ydfpcc();lex_begin_initial();return EOLC;}
<INITIAL,SKIP_IN,SKIP_INC>{NL}		{linecnt++;newline_ydfpcc();}

 /* C preprocessor directives */
#{WSPACE}*{INUM}{WSPACE}*\"{FILENAME}\".*	{char name[1000];int line; int n=sscanf(yycompilertext,"# %d \"%s\"",&line,name);if (n==2) generateInputFileNames(name,line);}
#{WSPACE}*{INUM}{WSPACE}*\"[^\"]*\".*		{}


 /*ignore VSS Versions */
"$"{STRING}+{WSPACE}*:{WSPACE}*{STRING}*{WSPACE}*"$"			{CStr tmp=yycompilertext; tmp.Extract(":","$");unputc(tmp.buf);}							/* ignore $revision */
\""$"{STRING}+{WSPACE}*:{WSPACE}*{STRING}*{WSPACE}*"$"\"		{CStr tmp=yycompilertext; tmp.Extract(":","$");tmp=(CStr)"\""+tmp+"\"";unputc(tmp.buf);}	/* ignore $revision */

 /*ignore typedef struct xx xx , for same names*/
typedef{WSPACE}*struct{WSPACE}*{LABEL}{WSPACE}*{LABEL}{WSPACE}*; { char label1[1000]=""; char label2[1000]="";
                                                                 sscanf(yycompilertext,"typedef struct %s %s", label1, label2);
                                                                 int len = strlen(label2); if (len > 0) {
                                                                     label2[len - 1] = 0;
                                                                 }
                                                                 if (strcmp(label1, label2) != 0){
                                                                     REJECT;
                                                                 }}

  /* processing of Expressions, für pass2 und org processing*/
{INUM}							{yylval.ival=atoll(yycompilertext);return INUM;}
{FNUM}							{yylval.fval=atof(yycompilertext);return FNUM;}
{HEXNUM2}						{yylval.ival=strtoul(&yycompilertext[2], (char **)NULL, 16);
								return UNUM;} /* Hex Koeffizienten sind unsigned*/
{BINNUM2}						{yylval.ival=strtol(&yycompilertext[2], (char **)NULL, 2);return INUM;}
\"[^\"\t]*\"					{as.str=yycompilertext; CStr* tmp=new CStr; *tmp=yycompilertext; yylval.str=tmp;return TSTRING;}

"&""&"							{return YOP_LOGIC_AND;}
"|""|"							{return YOP_LOGIG_OR;}

"-""-"							{return YOP_MINUS_MINUS;}  
"+""+"							{return YOP_PLUS_PLUS;}	   

"=""="							{return YOP_EQUAL;}	     /*logical equal*/
"!""="							{return YOP_NOT_EQUAL;}	 /*logical not equal*/
">""="							{return YOP_GREATER_EQ;} /*logical greater or equal*/
"<""="							{return YOP_LESS_EQ;}	 /*logical less or equal*/

"*""="							{return YOP_MUL_ASSIGN	       ;}
"/""="							{return YOP_DIV_ASSIGN		   ;}
"%""="							{return YOP_MOD_ASSIGN		   ;}
"+""="							{return YOP_ADD_ASSIGN		   ;}
"-""="							{return YOP_SUB_ASSIGN		   ;}
"<<""="							{return YOP_SHIFT_LEFT_ASSIGN  ;}	
">>""="							{return YOP_SHIFT_RIGHT_ASSIGN ;}
"&""="							{return YOP_AND_ASSIGN	       ;}
"^""="							{return YOP_XOR_ASSIGN	       ;}
"|""="							{return YOP_OR_ASSIGN		   ;}    

"<""<"							{return YOP_SHIFT_LEFT;}	/*binary shift left*/
">"">"							{return YOP_SHIFT_RIGHT;}	/*binary shift right*/
 
 /*replace long long by longlong*/
long{WSPACE}+long{WSPACE}+								{unputc("longlong ");}
long{WSPACE}+long{WSPACE}+int{WSPACE}+					{unputc("longlong ");}
long{WSPACE}+long{WSPACE}+unsigned{WSPACE}+int{WSPACE}+	{unputc("unsigned longlong ");} 
short{WSPACE}+int{WSPACE}+								{unputc("short ");}
long{WSPACE}+int{WSPACE}+								{unputc("long ");}
short{WSPACE}+unsigned{WSPACE}+int{WSPACE}+				{unputc("unsigned short ");} 
 
 /*remove inline statement*/
inline							{BEGIN INLINE;}
static{WSPACE}*inline			{BEGIN INLINE;}
<INLINE>"{"						{inline_cnt++;}
<INLINE>"}"						{inline_cnt--;if (inline_cnt==0) BEGIN INITIAL;}
<INLINE>.						{;}
<INLINE>{NL}					{linecnt++;as.linecnt++;gd->linecnt++;}

 /*ignore __attribute__ statement until next ";"*/
 /*__attribute__[^\)]*\)*		{;}*/
"__attribute__"				{BEGIN __ATTR__;}
<__ATTR__>{WSPACE}*"(("{WSPACE}*packed{WSPACE}*"))" {BEGIN 0; yylval.ival=COMPILE_ATTR_PACKED; return ATTRIBUTE;}
<__ATTR__>;						{BEGIN 0; return ';';}
<__ATTR__>.						{;}
<__ATTR__>{NL}					{;}

#pragma.*						{;} 

__builtin_va_list			{unputc("int");}				/* simply define va_list as an type to avoid error messages */

 /* C Keywords */
asm{WSPACE}*"(".*											/*ignore asm directive*/ 
asm{WSPACE}*volatile{WSPACE}*"(".*							/*ignore asm directive*/ 
 
struct 													{return STRUCT    ;}
typedef													{return TYPEDEF   ;}	    
extern                             						{return EXTERN    ;}     
static                              					{return STATIC    ;}     
auto                               						{return AUTO      ;}     
register                            					{return REGISTER  ;}
restrict                            					{}  
__restrict                            					{}          
union													{return UNION     ;}     
"..."	                            					{return ELLIPSIS  ;}     
const                               					{return CONST     ;}     
volatile                            					{return VOLATILE  ;}    
case													{return CASE	  ;}			
default                                                 {return DEFAULT   ;}       
switch                                                  {return SWITCH    ;}       
if								                        {return IF        ;}       
else                                                    {return ELSE      ;}       
while													{return WHILE	  ;}			
do                                                      {return DO        ;}       
for		                                                {return FOR		  ;}       
goto 			 										{return GOTO  	  ;}			
continue                                                {return CONTINUE  ;}       
break                                                   {return BREAK     ;}       
return 													{return RETURN 	  ;}		
sizeof 													{return SIZEOF 	  ;}		
enum 													{as.isEnumTypedf=true; return ENUM ;}		

->														{return YOP_ARROW ;}
namespace												{return NAMESPACE ;}	

 /* Preprocessor */

#{WSPACE}*include{WSPACE}+<{WSPACE}*	                {include_os_state=YYSTATE; BEGIN INCLUDE_OS; return PREPROC_INCLUDE;}
<INCLUDE_OS>[^\"\t\>\<]*{WSPACE}*\>                     {as.str=yycompilertext; CStr* tmp=new CStr; *tmp=yycompilertext;tmp->Replace(">","",true); yylval.str=tmp;BEGIN include_os_state; return TSTRING;}

#{WSPACE}*include{WSPACE}+				                {return PREPROC_INCLUDE;}
#{WSPACE}*define{WSPACE}+				                {doResolveDefines=false; return PREPROC_DEFINE;}
#{WSPACE}*undef{WSPACE}+					            {doResolveDefines=false; return PREPROC_UNDEFINE;}
#{WSPACE}*error.*				                        {gd->dbgPrintf(TERROR, "%s\n",&yycompilertext[sizeof("#error")]);}
<CDEFINE>{DEFINESTR}			                        {as.str=yycompilertext; CStr* tmp=new CStr; *tmp=yycompilertext;tmp->cut("//");tmp->cut("/*"); yylval.str=tmp;return PREPROC_DEFINESTR;}
<CDEFINE>"//".*					                        {return EOLC;}
<CDEFINE>{WSPACE}*\\{NL}		                        {linecnt++; as.linecnt++; gd->linecnt++;}
<CDEFINE>{NL}					                        {linecnt++; as.linecnt++; gd->linecnt++; return EOLC;}
defined{WSPACE}*"("{WSPACE}*{LABEL}{WSPACE}*")"	        {bool tmp=checkDefined(yycompilertext);if (tmp) {unputc("1");} else {unputc("0");}}
defined{WSPACE}*{LABEL}	                                {bool tmp=checkDefined2(&yycompilertext[sizeof("defined")]);if (tmp) {unputc("1");} else {unputc("0");}}
<SKIP_INC,INITIAL>#{WSPACE}*if			                {if (as.skip_input==true) unputc("0 //"); DoSkip(2,10); return PREPROC_IF;}  // argument is forced to false if line is already skipped, force no_skip
<SKIP_INC,INITIAL>#{WSPACE}*ifdef		                {doResolveDefines=false; if (as.skip_input==true) unputc("0 //"); DoSkip(2,11); return PREPROC_IFDEF;}  // argument is forced to false if line is already skipped, force no_skip
<SKIP_INC,INITIAL>#{WSPACE}*ifndef		                {doResolveDefines=false; if (as.skip_input==true) unputc("0 //"); DoSkip(2,12); return PREPROC_IFNDEF;}  // argument is forced to false if line is already skipped, force no_skip
<SKIP_INC,INITIAL>#{WSPACE}*else			            {endif(1);}  
<SKIP_INC,INITIAL>#{WSPACE}*elif                        {endif(1); CvdtRemoveFromBegin(as.iflist); if (as.skip_input == true) unputc("0 //"); DoSkip(2, 13); return PREPROC_IF; }
<SKIP_INC,INITIAL>#{WSPACE}*endif		                {endif(0); }
<SKIP_INC>.						                        {}	/* skip input if state=skip */
_ENDVARIABLE					                        {if (as.varlist!=NULL && as.varlist->list!=NULL ) as.varlist->list->active=0;CvdtRemoveFromBegin(as.varlist);} //end of inserted variable (detect recursive declarations)

                                                                                                 

  /* Processing of Lables and EQU, muss ganz zum Schluss stehen, sonst werden Mnemonics als Labels erkannt! */
  /* Reihenfolge beibehalten ! */
  /* write content of the Label back into the input stream*/
  
  /*check for correct namespace*/
{LABEL}::						{
								yycompilertext[strlen(yycompilertext)-2]=0; 
								Cvdt* tmpvdt=getvdt(yycompilertext);
								if (tmpvdt==NULL ||tmpvdt->token!=TOKEN_NAMESPACE) {
									ycc_erpnt(113,yycompilertext);
								}
								unputc("_");
								unputc(yycompilertext);
								}
								
								
{LABEL}							{char* tmp=strchr(yycompilertext,'.'); 
								if (tmp!=NULL) {
									yyless((int)(tmp-yycompilertext));tmp[0]=0;
								}
								
								Cvdt* tmpvdt;int token=InsertBuffer(tmpvdt); if(token<0) return ERR; 
								if (token>0) {
									yylval.vdt=tmpvdt;
									return token;}
								}/*no return if label is evaluated*/ 

"."								{return YOP_DOT;} 


 /* all thats left */
.					{return yycompilertext[0];}
%%

/* yywrap: Called when EOF is reached on current input.
 * Have it return 1 if processing is finished, or
 * do something to fix the EOF condition (like open
 * another file and point to it) and return 0 to indicate
 * more input is available.
 */
int yywrap ()
{

	return endoffile();

}

void lex_begin_state(int state)
{
	as.current_state=state;
	BEGIN state;

	if (yy_flex_debug!=0)
		fprintf(stderr,"----->Enter State <%s %d> !!\n", stateName[state], state);

}


// dont safe state if Argument !=0
void lex_begin_initial(int save_state)
{
	BEGIN INITIAL;
	if (save_state==0) as.current_state=INITIAL;
	if (yy_flex_debug!=0)
		fprintf(stderr,"----->Enter State <INITIAL %d> !!\n", INITIAL);

}


void lex_begin_readline_ydfpcc()
{
	BEGIN READLINE_YDFPCC;
	if (yy_flex_debug!=0)
		fprintf(stderr,"----->Enter State <READLINE_YDFPCC %d> !!\n",READLINE_YDFPCC);

}


void lex_begin_last_state()
{
		BEGIN as.current_state;
		if (yy_flex_debug!=0)
			fprintf(stderr,"----->Enter State <last_state %d> !!\n",as.current_state);
}


void lex_begin_comment()
{
	BEGIN CCOMMENT;
	as.current_state=CCOMMENT;
	if (yy_flex_debug!=0)
		fprintf(stderr,"----->Enter State <CCOMMENT %d> !!\n",CCOMMENT);
}

void lex_begin_cdoxygen()
{
	BEGIN CDOXYGEN;
	as.current_state=CDOXYGEN;
	as.linecntStartDoxy=as.linecnt;
	as.doxycomment.clear();    
	if (yy_flex_debug!=0)
		fprintf(stderr,"----->Enter State <CDOXYGEN %d> !!\n",CDOXYGEN);
}

void lex_end_cdoxygen()
{
    as.doxyInsertcode.clear();
	lex_WriteToDoxygenParser(as.doxycomment.buf);
	lex_begin_initial();	

    if (!as.doxyInsertcode.isEmpty()) {
        unputc(as.doxyInsertcode.buf);
    }
}

void lex_WriteToDoxygenParser(char* text)
{

	int tmpLinecnt=as.linecnt;
	as.linecnt=as.linecntStartDoxy;
	gd->linecnt=as.linecntStartDoxy;

	/* call comment parser*/
	if (yy_flex_debug != 0) {
		fprintf(stderr, "------------------------------------------------------\n");
		fprintf(stderr, "-- Enter Doxygen Parser\n");
		fprintf(stderr, "------------------------------------------------------\n");
	}
    gd->macroLevel++;
	YY_BUFFER_STATE buffer = yyannot_scan_string(text);
	startDoxyComment();
	yyannotlex();
	endDoxyComment();
	yyannot_delete_buffer(buffer);
	if (yy_flex_debug != 0) {
		fprintf(stderr, "------------------------------------------------------\n");
		fprintf(stderr, "-- Exit Doxygen Parser\n");
		fprintf(stderr, "------------------------------------------------------\n");
	}

	if (yy_flex_debug != 0) {
		if (as.linecnt !=	tmpLinecnt) {
			dbgPrintf(TWARNING,"INTERNAL; wrong linecnt: global=%d, doxy=%d\n",tmpLinecnt,as.linecnt);
		}
	}
	as.linecnt=	tmpLinecnt;	
	gd->linecnt= tmpLinecnt;	
}

void lex_begin_include_os() {
	BEGIN INCLUDE_OS;
	if (yy_flex_debug!=0)
		fprintf(stderr,"----->Enter State <INCLUDE_OS %d> !!\n", INCLUDE_OS);
}

void lex_begin_add_text(int mode)
{
	BEGIN ADDTEXT;
	as.removeBlanks=mode;
	as.current_state=ADDTEXT;
	if (yy_flex_debug!=0)
		fprintf(stderr,"----->Enter State <ADDTEXT %d> !!\n",ADDTEXT);
}


void lex_begin_cdefine() {
	BEGIN CDEFINE;
	if (yy_flex_debug!=0)
		fprintf(stderr,"----->Enter State <CDEFINE %d> !!\n", CDEFINE);
}

void lex_set_bol()
{
		 yy_set_bol(1);			// start of line again
}


void endif(int k)
{
    dbgPrintf(TINFO, "### DO endif: k=%d %s:%d \n", k, as.inputfile.buf, as.linecnt);

	int status=0;

	if (k==0)  	//endif
	{ 
		if (as.iflist==NULL) ycc_erpnt(606);
		CvdtRemoveFromBegin(as.iflist);
		if (as.iflist==NULL) {
			status=0;
		}
		else
		{
			status=(int) as.iflist->data;
		}
	}
	if (k==1)		//else
	{
		if (as.iflist==NULL) ycc_erpnt(607);
		as.skip_input=!as.skip_input;
		if (as.skip_input) status=1;
		if (as.iflist!=NULL) as.iflist->data=status;	
	}

	DoSkip(status,20);
}

// k=1: do dkip input
// k=0: clear skip_input
void DoSkip(int k, int from)
{
    if (yy_flex_debug != 0) {
        char skiplist[1000] = "";
        Cvdt* tmp = as.iflist;
        while (tmp != NULL) {
            if (tmp->data == 1) {
                strcat(skiplist, "1");
            }
            else {
                strcat(skiplist, "0");
            }
            tmp = tmp->next;
        }
        dbgPrintf(TINFO, "### DO Skip: k=%d,from=%d %s %s:%d \n", k,from, skiplist, as.inputfile.buf, as.linecnt);
    }
 



	if (k==1){
		as.skip_input=true;
		BEGIN SKIP_INC;
		as.current_state=SKIP_INC;
	}
	else if (k==0){

		bool skip=false;
		Cvdt* tmp=as.iflist;
		while (tmp!=NULL) {
			if (tmp->data==1) skip=true;	// any skip in the stack
			tmp=tmp->next;
		}

		if (!skip) {
			as.skip_input=false;
			lex_begin_initial();
		}
		else {		// skip further input
			as.skip_input=true;
			lex_begin_state(SKIP_INC);
			as.current_state=SKIP_INC;
		}
	}
	else  //force no_skip
	{
		as.skip_input=false;
		lex_begin_state(IFC1);
		as.current_state=IFC1;
	}
}

bool checkDefined(char* buffer)
{
	bool retval=false; 
	CStr label=buffer;
	label.Extract("(",")");
	label.ReplaceAll(" ","");
	current_vdt=getvdt(label.buf);

	if (current_vdt && current_vdt->token==TOKEN_DEFINE) {
		retval=true;
	}
	return retval;
}

bool checkDefined2(char* buffer)
{
    bool retval = false;
    CStr label = buffer;
    label.RemoveBlanks();
    current_vdt = getvdt(label.buf);

    if (current_vdt && current_vdt->token == TOKEN_DEFINE) {
        retval = true;
    }
    return retval;
}

long InsertBuffer(Cvdt* &current_vdt, char* prolog, char* buffer)
{

            #ifdef _H2XML_DEBUG
				dbgPrintf(TINFO,"InsertBuffer: %s, doResolveDefines=%d\n", buffer,doResolveDefines);
			#endif

    if (strcmp(buffer,"param_1")==0) {
        //int dbg=0;
    }

	char* ptyycompilertext=buffer;
	if (prolog!=NULL) {
		ptyycompilertext+=strlen(prolog);
	}
			

	long token=0;
	// find label for all modules
	
	current_vdt=getvdt(ptyycompilertext);

	
	// check for typdedef
	if (current_vdt!=NULL && current_vdt->token==TOKEN_TYPE) {
		token= TYPSPECIFIER;
	}
	else if (current_vdt!=NULL && (current_vdt->token==TOKEN_IDENTIFIER || current_vdt->token==TOKEN_UNDEFINED)) {
		token= IDENTIFIER;
	}
	else if (current_vdt!=NULL && (current_vdt->token==TOKEN_NAMESPACE )) {
		ycc_erpnt(112,current_vdt->name.buf);
		token= IDENTIFIER;
	}
	else if (current_vdt!=NULL && (current_vdt->token==TOKEN_DEFINE )) {
        if(current_vdt->isEnumElement) {
            as.enumName=buffer;
        }        
		if(current_vdt->valstr.buf !=NULL && doResolveDefines) {
			unputc(current_vdt->valstr.buf,buffer);
		} else {
			token= IDENTIFIER;
			doResolveDefines=true;
		}
	}
	
	// initialize if not available
	else if (current_vdt==NULL) {  
		doResolveDefines=true;
		current_vdt=htab.hashinstall(getname(&as.module_name,ptyycompilertext));
		
		
		#if defined (_H2XML_DEBUG)
		//if (current_vdt!=NULL) fprintf(stderr,"insert name %s\n",current_vdt->name.buf);
		#endif
		current_vdt->shortname=ptyycompilertext;
		current_vdt->token=TOKEN_UNDEFINED;

		if (!as.is_struct) {// if struct the global-value is taken from the struct (insertModule function)
			if (as.module_name.len == 0) 
				as.global_scope=SCOPE_GLOBAL;
			else
				as.global_scope=SCOPE_LOCAL;
		
			if (as.is_static) as.global_scope=SCOPE_GLOBAL;
		}
		
		token= IDENTIFIER;
	}

	// #define: replace content
	else {	
		//check redeclaration of variables
		if (current_vdt!=NULL && current_vdt->token==TOKEN_IDENTIFIER){
		ycc_erpnt(115,current_vdt->name.buf);
		}

	
		if(current_vdt!=NULL && current_vdt->valstr.buf !=NULL )			// Label vorhanden und initialisert ?
		{
			CvdtAddToBegin(as.varlist,current_vdt);							// put variable name on stack, to detect recursive definitions

			if (ChangeActiveModule(current_vdt))							// adopt module name to module of variable, Reset after ) has passed
			{
					unputc("_ENDMN",buffer);											// signal end of changed module
			}
			else 
			{
				unputc("_ENDVARIABLE",buffer);
			}
			if (current_vdt->cases<2) unputc(")",buffer);							// Inhalt in Klammern <()> fassen, deshalb zuerst eine <)>nur falls cases <2 (nicht für opcode etc.)
		
			// check, if variable depends on itself (infinite recursion)
			if (current_vdt->active!=0) {
				unputc("0",buffer);
				ycc_erpnt(5,current_vdt->name.buf);
			}
			else {
				unputc(current_vdt->valstr.buf,buffer);							// push content of variable back, only if no recursion
			}
			if (current_vdt->cases<2) unputc("(",buffer);							// Inhalt in Klammern <()> fassen, deshalb mit <)> beenden

			current_vdt->active=1;

		}
		else { 
			if(current_vdt!=NULL && current_vdt->valstr.buf ==NULL ) ycc_erpnt(6,ptyycompilertext);	// error : label not initialized
			token=-1;														// ->return ERR;			
		} 
	}
    doResolveDefines=true;
	return token;
}

// change active module to module of variable
bool ChangeActiveModule(Cvdt* in)
{
	bool retval=false;
	CStr tmp=in->name;
	char* end=strrchr(tmp.buf,'.');		// without variable name (=last .xxx)
	if (end!=NULL) end[1]=0;
	else tmp="";

	// tmp contains now path name 
	// check if path name changed and variable will be stored with () -> cases <2
	if(strcasecmp(as.module_name.buf,tmp.buf)!=0  && in->cases<2)
	{
		retval=true;		// return true, if active module has changed 
		// create new list entry
		Cvdt* tmpvdt=newCvdt(0);
		Cvdt* tmplist=as.module_list;
		as.module_list=tmpvdt;

		// add values to new list
		char* modname=strtok(tmp.buf,".");
		while (modname!=NULL){

			CvdtAddToBegin(as.module_list,modname);
			modname=strtok(NULL,".");
						//printf("name: %s\n",modname);
		}
		as.module_list->list=tmplist;
		update_mod_names(as.module_name,as.module_list);
		#ifdef _H2XML_DEBUG
			fprintf(stderr,"\t\tline %d: Set Active %s\n",as.linecnt,as.module_name.buf);
		#endif

	}

	return retval;
}

void unputc(const char* in, char* buffer)
{
	const char *p= in;							// Charcter inhalt des Labels in Input Buffer zurück
	const char *q=p+strlen(p);
	while(q>p) {
		yyunput(*--q, buffer);
	}
}


// Include Processing
int endoffile()
{
	// Include File
	as.include_cnt--;		// include depth
	if (incstr!=NULL && as.include_cnt>0) {
		yy_delete_buffer(YY_CURRENT_BUFFER);

		//printf("Lex: close file %s\n",as.inputfile.buf);

		as.inputfile=incstr->filename;
		as.linecnt=incstr->linecnt;
		gd->linecnt=incstr->linecnt;
		as.inputpath=incstr->pathname;
		as.inputname=incstr->inputname;

		// replace actual include directory
		if (as.include_cnt>=0 && as.include_dirs!=0) as.include_dirs=as.include_dirs->next;

		fclose(as.infile);
		as.infile=incstr->infile;
		yy_switch_to_buffer(incstr->buf_stack);
		lex_begin_readline_ydfpcc();

		//printf("Lex: open file %s\n",incstr->filename.buf);
		#ifdef _H2XML_DEBUG
        dbgPrintf(TINFO,"### Include end, open file %s\n",incstr->filename.buf);
		#endif

		incstruct* tmp=incstr->next;
		delete incstr;
		incstr=tmp;
		gd->ptInputFileName=_strdup(as.inputfile.buf);

		return 0; // continuewith next file
	}

	//Done
	else {
		//	yyterminate();
			return 1;  //terminate, pass 0-token to yacc
	}
	return 0;
}


void process_include(char* name)
{
    dbgPrintf(TINFO,"### including file %s\n",name);
	

	as.include_cnt++;
	incstruct* tmp=new incstruct;
	tmp->next=incstr;
	incstr=tmp;

	incstr->buf_stack=YY_CURRENT_BUFFER;
	incstr->filename=as.inputfile;
	incstr->pathname=as.inputpath;
	incstr->inputname=as.inputname;
	incstr->linecnt=as.linecnt;
	incstr->infile=as.infile;

	as.linecnt=1;
	gd->linecnt=1;
	FILE* tmpfile;

	// check all include paths
	Cvdt* incdirs=as.include_dirs;
	while (incdirs!=NULL) {
		as.inputfile=incdirs->name+name;
		as.inputfile.AdjustForUnix();

		dbgPrintf(TINFO,"checking for include %s, path= %s\n",as.inputfile.buf,incdirs->name.buf);

		if ((tmpfile=fopen(as.inputfile.buf,"r"))==NULL) {
			incdirs=incdirs->next;
		}
		else {
			incdirs=NULL;
			fclose(tmpfile);
		}
	}

	// replace "\" by "/" for linux , solaris
	dbgPrintf(TINFO," including %s\n",as.inputfile.buf);

	if ((as.infile=fopen(as.inputfile.buf,"r"))==NULL){
		as.skiperror=false;
		as.inputfile=incstr->filename;		// restore name + line nr. for correct error message
		as.linecnt=incstr->linecnt-1;
		gd->linecnt=incstr->linecnt-1;
		as.infile=incstr->infile;

		incstruct* tmp=incstr->next;
		delete incstr;
		incstr=tmp;
		as.include_cnt--;
        if (as.include_cnt == 0) {
            ycc_erpnt(3, name);             // input file
        }
        else {
            ycc_erpnt(4, name);             // include file
        }
		//exit(1);
	} else {
		yy_switch_to_buffer(yy_create_buffer(as.infile,YY_BUF_SIZE));

		// calculate new include path

		as.inputpath=as.inputfile;
		char *ctmp=strrchr(as.inputpath.buf,*"/");		//Unix
		char *ctmp2=strrchr(as.inputpath.buf,*"\\");		//DOS
		if (ctmp!=NULL) {
			as.inputname=&ctmp[1];
			ctmp[1]=0;									//terminate String
		}
		else if (ctmp2!=NULL) {
			as.inputname=&ctmp2[1];
			ctmp2[1]=0;									//terminate String
		}
		else {
			as.inputpath.buf[0]=0;						//terminate String
			as.inputname=as.inputfile;
		}

		// add new path to include directories
		CvdtAddToBegin(as.include_dirs,as.inputpath.buf);	
	}
	gd->ptInputFileName=_strdup(as.inputfile.buf);
}

void generateInputFileNames(char* name, int line)
{
	//printf("%s %s\n",name,yycompilertext);
	static bool reset=true;
	CStr tmpname=name;
	tmpname.ReplaceAll("\\\\/","/");
	tmpname.ReplaceAll("\\\\","\\");
	tmpname.ReplaceAll("\"","");

	linecnt=line;
	as.linecnt=line;
	gd->linecnt=line;
	as.inputfile=tmpname;
	as.inputpath=as.inputfile;
	char *ctmp=strrchr(as.inputpath.buf,*"/");		//Unix
	char *ctmp2=strrchr(as.inputpath.buf,*"\\");		//DOS
	if (ctmp!=NULL) {
		as.inputname=&ctmp[1];
		ctmp[1]=0;									//terminate String
	}
	else if (ctmp2!=NULL) {
		as.inputname=&ctmp2[1];
		ctmp2[1]=0;									//terminate String
	}
	else {
		as.inputpath.buf[0]=0;						//terminate String
		as.inputname=as.inputfile;
	}

	if (reset) {
		reset=false;
		as.inputname_main=as.inputname;
	}
}


// mode=0: parse name for module name with strtok (regular assembler)
// mode=1: ydfpcc, name = module name
void insert_module(char* name, int mode, bool is_struct)
{

	CStr modname="";
	if (mode==0) {
		strtok(name," \t"); 
		modname=strtok(NULL," \t");
	}
	else {
		modname=name;
	}
		
	as.is_struct = is_struct;
	CvdtAddToBegin(as.module_list,modname.buf);
	update_mod_names(as.module_name,as.module_list);
	
	Cvdt* tmp = newCvdt(0);
    tmp->name = name;  
    tmp->datatype = as.declspec->getDataType(); 
    
    if (as.is_struct && (as.module_name.len==0)) 
			as.global_scope=SCOPE_GLOBAL;
    
    as.actual_module=tmp;
	
	#ifdef _H2XML_DEBUG
		//printf("line %d: module name=%s\n",as.linecnt,as.module_name.buf);
	#endif
}

// mode=0: parse name for module name with strtok (regular assembler)
// mode=1: ydfpcc, name = module name
void end_module(char* name)
{
	if (as.module_list!=NULL)
	{
		if(name!=NULL) {
			char *tmpstr=strrchr(name,'#');
			char *tmpstr1=strrchr(name,' ');
			char *tmpstr2=strrchr(name,'\t');

			if (tmpstr1>tmpstr) tmpstr=tmpstr1;
			if (tmpstr2>tmpstr) tmpstr=tmpstr2;

			// check for matching .endmodule<name>
			if (tmpstr!=NULL && strcmp(&tmpstr[1],as.module_list->name.buf)!=0)
				ycc_erpnt(605,as.module_list->name.buf);
		}
		Cvdt* tmp=as.module_list;
		as.module_list=as.module_list->next;
		tmp->next=NULL;
		delete tmp;
	}

	else {
		ycc_erpnt(602);			// unmached end_modules
	}

	update_mod_names(as.module_name,as.module_list);
}

void update_mod_names(CStr &name, Cvdt* list)
{
	//printf("%s\n",name.buf);
#define NAMEXSIZE 100000

	char namex[NAMEXSIZE]="";
	namex[NAMEXSIZE-1]=0;
	long total_len=1;
	Cvdt* tmp=list;
	while (tmp!=NULL && tmp->name.buf[0]!=0){
		int len=(int)strlen(tmp->name.buf);
		namex[NAMEXSIZE-total_len-1]=moduleSeparatorChar;
		total_len+=len+1;
		memmove(&namex[NAMEXSIZE-total_len],tmp->name.buf,len);
		tmp=tmp->next;
	}
	if (list!=NULL) {
		list->valstr.insert(&namex[NAMEXSIZE-total_len]);
	}
	name.insert(&namex[NAMEXSIZE-total_len]);
}


char* getname(CStr* module, char* text)
{
	char tmpstr[100];
	if (text[0]==':')
		text+=1;
	
	if ((struct_seen == 1) && (typedef_seen) && (module->len == 0)) { //typedef of struct (identifier not know, yet)
		sprintf(tmpstr,"__tmp_struct%d_",struct_typedef_cnt);
		as.str.add(tmpstr,text);
	}
	else if ((struct_seen == 2) && (!typedef_seen) && (module->len == 0)) { //struct with initializer behind
		//create tnp-struct and variable with this new type
		sprintf(tmpstr,"__tmp_struct%d_",struct_typedef_cnt);
		as.str.add(tmpstr,text);
	} else
		as.str.add(module->buf,text);

	return as.str.buf;
}

Cvdt* getvdt(char* name)
{
		Cvdt* list=as.module_list;
		Cvdt* current_vdt=NULL;
		CStr current_name="";
		while (list!=NULL && current_vdt==NULL)
		{
			update_mod_names(current_name,list);
			if (as.ycc_structname!=NULL) {
				current_name.add(current_name.buf,as.ycc_structname);
				current_name.add(current_name.buf,moduleSeparator);
			}
			current_vdt=htab.hashlookup(getname(&current_name,name));			// Label 
			list=list->next;
			#ifdef _H2XML_DEBUG
				dbgPrintf(TINFO,"hashlookup label: %s\n", name);
			#endif
		}
		if (current_vdt==NULL){
			current_name="";
			if (as.ycc_structname!=NULL) {
				current_name=as.ycc_structname;
				current_name.add(current_name.buf,moduleSeparator);
			}
			current_name.add(current_name.buf,name);
			current_vdt=htab.hashlookup(current_name.buf);			// top level
			#ifdef _H2XML_DEBUG
				dbgPrintf(TINFO,"hashlookup label: %s\n", name);
			#endif
		}
		return current_vdt;
}

// Check YDFP version
//mode: no error if ydfp version:  0->greater or equal
void check_version(double version,int mode)	
{
	switch (mode) {
		case 0:
			if (as.ydfp_version<version) {
				//erpnt(10,(int) version);
			}
		break;
		default:
		break;
	}
}		


void selectIdentifier(char* text)
{
    strncpy(tmpBuf, HASH_PREFIX, TMP_BUF_SIZE - 1);
    strncat(tmpBuf, text, TMP_BUF_SIZE-100);	
	Cvdt* tmp=htab.hashlookup(tmpBuf);
	if (tmp!=NULL) {
	as.last_identifier=tmp->identifier;
	} else {
		ycc_erpnt(200,text);
	}
}

void nameUnnamedStruct(char* text)
{
	char tmpc[1000]=HASH_PREFIX;
    sscanf(yycompilertext,"//::renameunnmamedstruct %s",&tmpc[8]);
	Cvdt* tmp=htab.hashinstall(tmpc); 
	if (tmp!=NULL) {
		tmp->identifier=as.last_identifier;
	}
}



void checkAddText(CStr* last)
{
	last->ReplaceAll("//  ","");
	last->ReplaceAll("// ","");
	last->ReplaceAll("//","");
	int len=strlen(last->buf);
	if (len>1 && last->buf[len-1]=='/' && last->buf[len-2]=='+') {  //replace with <space>
		last->Replace("+/"," ",true);					// replace last ovccurence
		lex_begin_add_text(as.removeBlanks);
		as.last_cstr=last;
	} else if (last->buf[len-1]=='/') {					// replace with <enter>
		last->Replace("/","\n",true);					// replace last
		lex_begin_add_text(as.removeBlanks);
		as.last_cstr=last;
	} else {
		if (as.removeBlanks) {
			last->RemoveLeadingBlanks();
			last->RemoveTrailingBlanks();
		}
		lex_begin_initial();
	}
}
