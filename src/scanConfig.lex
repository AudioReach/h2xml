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
    #include "h2xml_global.h"
    #include "global_functions.h"
    #include "scanConfig.h"


    #define MAX_LEX_STATES 30
    #define YY_USER_INIT lexInitConf()
    #define STRADDLENGTH 256

    long statesConf[MAX_LEX_STATES];
    long lexStateIndexConf = 0;
    long lexPushStateConf(long state);
    long lexPopStateConf();
    void lexChangeStateConf(long state);
    int stopConf();
    void confErPrintf(char* format, ...);
    void lexInitConf();
    void addXMLGenConfig(char* name, eAnArgTypes argType,void* address) ;
    void addArgType(eAnArgTypes argType);
    void insertAttributes();
    void lexStateGetString(CStr* string);
    void checkVersion();
    CConfigAttributes* newConfigAttribute(eAnType type);
	void setPrefix();

    char stateStrConf[][30] = { "INITIAL", "DECLAREINFO", "COMMENT", "H2XML_CONFIG", "H2XML_CONFIG_ATTRIBUTES", "TYPE_CONFIG", "TYPE_ATTRIBUTES", "TYPE_SKIP", "GLOBAL_CONFIG", "GLOBAL_ATTRIBUTES",
        "XML_GENERATOR", "XML_GENERATOR_CONFIG", "XML_GENERATOR_ATTRIBUTES", "MODULE_CONFIG", "MODULE_ATTRIBUTES", "STRUCTURE_CONFIG", "STRUCTURE_ATTRIBUTES",
        "ELEMENT_CONFIG", "ELEMENT_ATTRIBUTES", "SET_ATTRIBUTES", "GET_STRING", "IGNORE_STRING", "GET_ARGUMENTS", "GET_INTVALUE", "GET_STRINGLIST", "REMOVE_STRINGLIST", "GET_BOOL", "ACCESS_SPEC" };
    
    eAnGroups state2Enum[50];
    int confLineCnt = 1;
    int unmachedCnt=0;
    CConfigAttributes* scatr;
    CStr* actualString;
    int* actualInt;
    CArgIntRef* actualIntRef;
    CStringList** actualStringList;
    bool* actualBool;
    bool configTypeInList=false;
    CXMLConfigData* confData=NULL;
    CStr actualPrefix="";
    CStr actualTypeName="";
    bool accessSpecPrivate = false;
    eAnGroups actualGroup;
    bool is_argTpeList;
    bool skipKeywords=false;
    CStringList *addTypeList = NULL;
	void (*cb_arglist)(void)=NULL;;		// callback for arglist
	
    sDefaultFunction_t defaultFunctions[]={
        {ANFUNCTION_TAG_ELEMENT, "TAG_ELEMENT"},
        {ANFUNCTION_TAG_BITFIELD, "TAG_BITFIELD"},
        {ANFUNCTION_ALIAS_BITFIELD, "Bitfield"},
        {ANFUNCTION_ALIAS_BITFIELD_END, "BitfieldEnd"},
        {0,""}
    };
%}
    


WSPACE		[ \t\x0d]+
LABEL		[a-zA-Z_][a-zA-Z0-9_.]*
STRING		[a-zA-Z0-9_\-()][a-zA-Z0-9_.\-()]*
INUM		[0-9]+(u|U|l|L)?
INUMS		-?[0-9]+(u|U|l|L)?
FNUM		(([0-9]+"."[0-9]*)|([0-9]*"."[0-9]+))("."f)?
HEXNUM		0x[0-9a-f]+(u|U|l|L)?
BINNUM1		%[01]+
BINNUM2		0b[01]+
NL			\x0d\x0a|\x0a\x0d|\x0a

TYPEAN      CONFIG_TYPE
GLOBALAN    GLOBAL_ATTRIBUTES|GLOBAL_ANNOTATIONS
XMLAN       XML_GENERATOR|XML_GENERATOR_ANNOTATIONS
MODAN       MODULE_ATTRIBUTES|MODULE_ANNOTATIONS
STRUCTAN    STRUCTURE_ATTRIBUTES|STRUCTURE_ANNOTATIONS
ELMAN       ELEMENT_ATTRIBUTES|ELEMENT_ANNOTATIONS



%x DECLAREINFO CCOMMENT H2XML_CONFIG H2XML_CONFIG_ATTRIBUTES TYPE_CONFIG TYPE_ATTRIBUTES TYPE_SKIP GLOBAL_CONFIG GLOBAL_ATTRIBUTES XML_GENERATOR
%x XML_GENERATOR_CONFIG XML_GENERATOR_ATTRIBUTES MODULE_CONFIG MODULE_ATTRIBUTES STRUCTURE_CONFIG STRUCTURE_ATTRIBUTES ELEMENT_CONFIG ELEMENT_ATTRIBUTES
%x SET_ATTRIBUTES GET_STRING IGNORE_STRING GET_ARGUMENTS GET_INTVALUE GET_STRINGLIST REMOVE_STRINGLIST GET_BOOL ACCESS_SPEC

%%

 /* general Roules */
<*>{WSPACE}										            /*Ignore Whitespace*/

 /* skip comments*/
<*>"<!--"										            {lexPushStateConf(CCOMMENT); }
<CCOMMENT>"-->"									            {lexPopStateConf(); }
<CCOMMENT>.									
<CCOMMENT>[^\x0d\x0a-]*						
<CCOMMENT>{NL}									            {confLineCnt++; }

 /* skip info declaration*/
<INITIAL>"<?"									            {lexPushStateConf(DECLAREINFO); }
<DECLAREINFO>"?>"								            {lexPopStateConf(); }
<DECLAREINFO>.
<DECLAREINFO>[^\x0d\x0a\?]*				
<DECLAREINFO>{NL}								            {confLineCnt++; }

 /* Aliases */


 /* HTXML Config Attributes */
"<"H2XML_CONFIG									            {lexPushStateConf(H2XML_CONFIG_ATTRIBUTES);actualGroup = ANGROUP_GLOBAL;}
<H2XML_CONFIG_ATTRIBUTES>{
"/>"											            {lexPopStateConf(); }
">"												            {lexPopStateConf(); lexPushStateConf(H2XML_CONFIG); actualPrefix = gd->config->m_globalPrefix; checkVersion();}
prefix{WSPACE}*={WSPACE}*\"						            {lexPushStateConf(GET_STRING); actualString= &gd->config->m_globalPrefix;}
version{WSPACE}*={WSPACE}*\"					            {lexPushStateConf(GET_STRING); actualString= &gd->config->m_version;}
h2xmlversion{WSPACE}*={WSPACE}*\"			                {lexPushStateConf(GET_STRING); actualString= &gd->config->m_h2xmlVersion;}
action{WSPACE}*={WSPACE}*\"						            {lexPushStateConf(GET_STRING); actualString= &gd->config->m_globalAction;}
}
"<"H2XML_CONFIG{WSPACE}*>						            {lexPushStateConf(H2XML_CONFIG); actualGroup = ANGROUP_GLOBAL; }
<H2XML_CONFIG>"</H2XML_CONFIG"{WSPACE}*>		            {lexPopStateConf(); }

 /* HTXML Type Attributes */
<H2XML_CONFIG>"<"{TYPEAN}{WSPACE}*>			                {lexPushStateConf(TYPE_ATTRIBUTES); }
<H2XML_CONFIG>"<"{TYPEAN}						            {lexPushStateConf(TYPE_CONFIG); actualGroup = ANGROUP_GLOBAL; }
<TYPE_CONFIG>{
"/>"											            {lexPopStateConf(); }
">"												            {lexPopStateConf();
                                                                CStringList* list=addToEnd(lNEXT,gd->configTypeList); list->m_string=actualTypeName.buf;
                                                                if (CStringList::findInList(gd->cmdTypeList,actualTypeName.buf)) {
                                                                    gd->configTypeFound=true;
                                                                    skipKeywords = false;
                                                                    lexPushStateConf(TYPE_ATTRIBUTES); 
                                                                    CStringList::appendStringList(gd->cmdTypeList, addTypeList);
                                                                    CStringList::deleteStringList(addTypeList);
                                                                } else {
                                                                    //lexPushStateConf(TYPE_SKIP); 
                                                                    lexPushStateConf(TYPE_ATTRIBUTES);
                                                                    skipKeywords = true;
                                                                    CStringList::deleteStringList(addTypeList);
                                                                }
                                                            }
name{WSPACE}*={TYPEAN}*\"						            {lexPushStateConf(GET_STRING); actualTypeName=""; actualString= &actualTypeName;}
addConfigType{WSPACE}*={WSPACE}*\"						    {lexPushStateConf(GET_STRINGLIST); actualStringList= &addTypeList;}
removeConfigType{WSPACE}*={WSPACE}*\"                       {lexPushStateConf(REMOVE_STRINGLIST); actualStringList= &gd->cmdTypeList;}

}
<TYPE_ATTRIBUTES>"</"{TYPEAN}{WSPACE}*>		                {actualTypeName=""; lexPopStateConf();  }
<TYPE_SKIP>"</"{TYPEAN}{WSPACE}*>		                    {actualTypeName=""; lexPopStateConf();  }
<TYPE_SKIP>.                                                {}

 /* Global attributes */
<H2XML_CONFIG,TYPE_ATTRIBUTES>"<"{GLOBALAN}{WSPACE}*>	    {gd->config->m_AnGroupAvailable[ANGROUP_GLOBAL] = true; lexPushStateConf(GLOBAL_ATTRIBUTES); actualGroup = ANGROUP_GLOBAL; }
<H2XML_CONFIG,TYPE_ATTRIBUTES>"<"{GLOBALAN}				    {gd->config->m_AnGroupAvailable[ANGROUP_GLOBAL]=true; lexPushStateConf(GLOBAL_CONFIG); actualGroup = ANGROUP_GLOBAL;}
<GLOBAL_CONFIG>{
">"												            {lexPopStateConf(); lexPushStateConf(GLOBAL_ATTRIBUTES); actualPrefix = gd->config->m_globalPrefix + gd->config->m_actualPrefix[actualGroup]; }
}
<GLOBAL_ATTRIBUTES>"</"{GLOBALAN}{WSPACE}*>		            {lexPopStateConf(); accessSpecPrivate = false; }

 /* XML Generator Attributes */
<H2XML_CONFIG,TYPE_ATTRIBUTES>"<"{XMLAN}{WSPACE}*>			{gd->config->m_AnGroupAvailable[ANGROUP_XMLGEN] = true; lexPushStateConf(XML_GENERATOR_ATTRIBUTES); actualGroup = ANGROUP_XMLGEN; }
<H2XML_CONFIG,TYPE_ATTRIBUTES>"<"{XMLAN}					{gd->config->m_AnGroupAvailable[ANGROUP_XMLGEN] = true; lexPushStateConf(XML_GENERATOR_CONFIG); actualGroup = ANGROUP_XMLGEN; }
<XML_GENERATOR_CONFIG>{
">"															{lexPopStateConf(); lexPushStateConf(XML_GENERATOR_ATTRIBUTES); actualPrefix = gd->config->m_globalPrefix  +gd->config->m_actualPrefix[actualGroup]; }
}
<XML_GENERATOR_ATTRIBUTES>"</"{XMLAN}{WSPACE}*>		        {lexPopStateConf(); accessSpecPrivate = false; }


 /* Module Attributes */
<H2XML_CONFIG,TYPE_ATTRIBUTES>"<"{MODAN}{WSPACE}*>			{gd->config->m_AnGroupAvailable[ANGROUP_MODULE] = true; lexPushStateConf(MODULE_ATTRIBUTES); actualGroup = ANGROUP_MODULE; }
<H2XML_CONFIG,TYPE_ATTRIBUTES>"<"{MODAN}					{gd->config->m_AnGroupAvailable[ANGROUP_MODULE] = true; lexPushStateConf(MODULE_CONFIG); actualGroup = ANGROUP_MODULE; }
<MODULE_CONFIG>{
">"															{lexPopStateConf(); lexPushStateConf(MODULE_ATTRIBUTES); actualPrefix = gd->config->m_globalPrefix  + gd->config->m_actualPrefix[actualGroup]; }
}
<MODULE_ATTRIBUTES>"</"{MODAN}{WSPACE}*>		            {lexPopStateConf(); accessSpecPrivate = false; }

 /* Parameter Attributes */
<H2XML_CONFIG,TYPE_ATTRIBUTES>"<"{STRUCTAN}{WSPACE}*>		{gd->config->m_AnGroupAvailable[ANGROUP_STRUCTURE] = true; lexPushStateConf(STRUCTURE_ATTRIBUTES); actualGroup = ANGROUP_STRUCTURE; }
<H2XML_CONFIG,TYPE_ATTRIBUTES>"<"{STRUCTAN}					{gd->config->m_AnGroupAvailable[ANGROUP_STRUCTURE]=true;  lexPushStateConf(STRUCTURE_CONFIG); actualGroup = ANGROUP_STRUCTURE;}
<STRUCTURE_CONFIG>{
">"															{lexPopStateConf(); lexPushStateConf(STRUCTURE_ATTRIBUTES); actualPrefix =  gd->config->m_globalPrefix + gd->config->m_actualPrefix[actualGroup]; }
}
<STRUCTURE_ATTRIBUTES>"</"{STRUCTAN}{WSPACE}*>		        {lexPopStateConf(); accessSpecPrivate = false; }

 /* Element Attributes */
<H2XML_CONFIG,TYPE_ATTRIBUTES>"<"{ELMAN}{WSPACE}*>			{gd->config->m_AnGroupAvailable[ANGROUP_ELEMENT] = true; lexPushStateConf(ELEMENT_ATTRIBUTES); actualGroup = ANGROUP_ELEMENT; }
<H2XML_CONFIG,TYPE_ATTRIBUTES>"<"{ELMAN}					{gd->config->m_AnGroupAvailable[ANGROUP_ELEMENT] = true; lexPushStateConf(ELEMENT_CONFIG); actualGroup = ANGROUP_ELEMENT; }
<ELEMENT_CONFIG>{
">"															{lexPopStateConf(); lexPushStateConf(ELEMENT_ATTRIBUTES); actualPrefix = gd->config->m_globalPrefix + gd->config->m_actualPrefix[actualGroup]; }
}
<ELEMENT_ATTRIBUTES>"</"{ELMAN}{WSPACE}*>		            {lexPopStateConf(); accessSpecPrivate = false; }

<GLOBAL_CONFIG,XML_GENERATOR_CONFIG,MODULE_CONFIG,STRUCTURE_CONFIG,ELEMENT_CONFIG>{
prefix{WSPACE}*={WSPACE}*\"								    {lexStateGetString(&gd->config->m_actualPrefix[actualGroup]); cb_arglist=setPrefix;}
action{WSPACE}*={WSPACE}*\"								    {if(skipKeywords){lexPushStateConf(IGNORE_STRING);} else {lexPushStateConf(GET_STRING);} actualString= &gd->config->m_action[actualGroup];}
outputGlobal{WSPACE}*={WSPACE}*\"					        {lexPushStateConf(GET_BOOL); actualBool= &gd->config->m_outputGlobal[actualGroup];}
accessSpec{WSPACE}*={WSPACE}*\"{WSPACE}*private{WSPACE}*\"  {accessSpecPrivate=true;} 
accessSpec{WSPACE}*={WSPACE}*\"{WSPACE}*public{WSPACE}*\"   {accessSpecPrivate=false;} 
"/>"														{lexPopStateConf(); }
}

 /* access specifier */
<GLOBAL_ATTRIBUTES,MODULE_ATTRIBUTES,STRUCTURE_ATTRIBUTES,ELEMENT_ATTRIBUTES,H2XML_CONFIG,TYPE_ATTRIBUTES,XML_GENERATOR_ATTRIBUTES>{
"<"private{WSPACE}*>                                        {accessSpecPrivate = true; lexPushStateConf(ACCESS_SPEC); }
"<"public{WSPACE}*>                                         {accessSpecPrivate = false; lexPushStateConf(ACCESS_SPEC); }
}
<ACCESS_SPEC>"</"private{WSPACE}*>		                    {lexPopStateConf(); accessSpecPrivate = false; }
<ACCESS_SPEC>"</"public{WSPACE}*>		                    {lexPopStateConf(); accessSpecPrivate = false; }

 /* Attributes */
<GLOBAL_ATTRIBUTES,MODULE_ATTRIBUTES,STRUCTURE_ATTRIBUTES,ELEMENT_ATTRIBUTES,H2XML_CONFIG,TYPE_ATTRIBUTES,XML_GENERATOR_ATTRIBUTES,ACCESS_SPEC>{
"<ALIAS"												    {scatr = newConfigAttribute(ANTYPE_ALIAS); }
"<TAG"													    {scatr = newConfigAttribute(ANTYPE_TAG); }
"<ATTRIBUTE"											    {scatr = newConfigAttribute(ANTYPE_ATTRIBUTE); }
}
<SET_ATTRIBUTES>{
">"
"/>"													    {lexPopStateConf();insertAttributes();}
keyword{WSPACE}*={WSPACE}*\"							    {lexPushStateConf(GET_STRING); actualString= &scatr->m_keyword; }
name{WSPACE}*={WSPACE}*\"								    {lexPushStateConf(GET_STRING); actualString= &scatr->m_anName;}
arguments{WSPACE}*={WSPACE}*\"							    {lexPushStateConf(GET_ARGUMENTS); is_argTpeList=false;}
argumentsList{WSPACE}*={WSPACE}*\"							{lexPushStateConf(GET_ARGUMENTS);  is_argTpeList=true;}
mandatory{WSPACE}*={WSPACE}*\"						        {lexPushStateConf(GET_BOOL); actualBool=&scatr->m_mandatory;}
min{WSPACE}*={WSPACE}*\"								    {lexPushStateConf(GET_INTVALUE); actualIntRef=&scatr->m_min;scatr->m_min_set=true;}
max{WSPACE}*={WSPACE}*\"								    {lexPushStateConf(GET_INTVALUE); actualIntRef=&scatr->m_max;scatr->m_max_set=true;}
odd{WSPACE}*={WSPACE}*\"true\"                              {scatr->m_argFormat=ANARGFORAMT_ODD;}
even{WSPACE}*={WSPACE}*\"true\"                             {scatr->m_argFormat=ANARGFORAMT_EVEN;}
odd{WSPACE}*={WSPACE}*\"false\"                             {scatr->m_argFormat=ANARGFORAMT_NORMAL;}
even{WSPACE}*={WSPACE}*\"false\"                            {scatr->m_argFormat=ANARGFORAMT_NORMAL;}
maxoccurs{WSPACE}*={WSPACE}*\"unbounded\"				    {scatr->m_tagMaxOccurs=0;}
maxoccurs{WSPACE}*={WSPACE}*\"							    {lexPushStateConf(GET_INTVALUE); actualIntRef=NULL; actualInt=&scatr->m_tagMaxOccurs;}
selectExisting{WSPACE}*={WSPACE}*\"						    {lexPushStateConf(GET_BOOL); actualBool=&scatr->m_tagSelectExisting;}
removeRedundantTags{WSPACE}*={WSPACE}*\"				    {lexPushStateConf(GET_BOOL); actualBool=&scatr->m_removeRedundantTags;}
labelList{WSPACE}*={WSPACE}*\"							    {lexPushStateConf(GET_STRINGLIST); actualStringList=&scatr->m_labelList;}
showInTag{WSPACE}*={WSPACE}*\"							    {lexPushStateConf(GET_STRINGLIST); actualStringList=&scatr->m_showInTag;}
default{WSPACE}*={WSPACE}*\"							    {lexPushStateConf(GET_STRING); actualString= &scatr->m_default;scatr->m_default_set=true;}
action{WSPACE}*={WSPACE}*\"								    {lexPushStateConf(GET_STRING); actualString= &scatr->m_action;}
actionList{WSPACE}*={WSPACE}*\"							    {lexPushStateConf(GET_STRING); actualString= &scatr->m_actionList;}
actionPost{WSPACE}*={WSPACE}*\"							    {lexPushStateConf(GET_STRING); actualString= &scatr->m_actionPost;}
show{WSPACE}*={WSPACE}*\"								    {lexPushStateConf(GET_BOOL); actualBool=&scatr->m_showInOutput;}
removeQuotes{WSPACE}*={WSPACE}*\"						    {lexPushStateConf(GET_BOOL); actualBool=&scatr->m_removeQuotes;}
addToFront{WSPACE}*={WSPACE}*\"							    {lexPushStateConf(GET_BOOL); actualBool=&scatr->m_addToFront;}
globalAnnotation{WSPACE}*={WSPACE}*\"					    {lexPushStateConf(GET_BOOL); actualBool=&scatr->m_globalAnnotation;}
private{WSPACE}*={WSPACE}*\"					            {lexPushStateConf(GET_BOOL); actualBool=&scatr->m_accessSpecPrivate;}
attributetype{WSPACE}*={WSPACE}*\"GENERAL\"				    {scatr->m_anFunction=ANFUNCTION_GENERAL;}
attributetype{WSPACE}*={WSPACE}*\"GLOBAL_ANNOTATIONS\"	    {scatr->m_anFunction=ANFUNCTION_GLOBALANNOTATIONS;}
attributetype{WSPACE}*={WSPACE}*\"NAME\"				    {scatr->m_anFunction=ANFUNCTION_NAME;}
attributetype{WSPACE}*={WSPACE}*\"GENERATE_OUTPUT\"		    {scatr->m_anFunction=ANFUNCTION_GENERATE_OUTPUT;}
attributetype{WSPACE}*={WSPACE}*\"DEFAULT\"				    {scatr->m_anFunction=ANFUNCTION_DEFAULT;}
attributetype{WSPACE}*={WSPACE}*\"DEFAULTLIST\"			    {scatr->m_anFunction=ANFUNCTION_DEFAULTLIST;}
attributetype{WSPACE}*={WSPACE}*\"DEFAULTFILE\"			    {scatr->m_anFunction=ANFUNCTION_DEFAULTFILE;}
attributetype{WSPACE}*={WSPACE}*\"DEFAULTLIST_CONDITIONAL\"	{scatr->m_anFunction=ANFUNCTION_DEFAULTLIST_CONDITIONAL;}
attributetype{WSPACE}*={WSPACE}*\"DEFAULTFILE_CONDITIONAL\"	{scatr->m_anFunction=ANFUNCTION_DEFAULTFILE_CONDITIONAL;}
attributetype{WSPACE}*={WSPACE}*\"DEFAULT_CONDITIONAL\"		{scatr->m_anFunction=ANFUNCTION_DEFAULT_CONDITIONAL;}
attributetype{WSPACE}*={WSPACE}*\"ARRAY_DEFAULT\"		    {scatr->m_anFunction=ANFUNCTION_ARRAY_DEFAULT;}

attributetype{WSPACE}*={WSPACE}*\"BITMASK\"				    {scatr->m_anFunction=ANFUNCTION_BITMASK;}
attributetype{WSPACE}*={WSPACE}*\"BYTESIZE\"                {scatr->m_anFunction=ANFUNCTION_BYTESIZE     ;}             
attributetype{WSPACE}*={WSPACE}*\"BITESIZE\"                {scatr->m_anFunction=ANFUNCTION_BITESIZE     ;}             
attributetype{WSPACE}*={WSPACE}*\"BYTEOFFSET\"			    {scatr->m_anFunction=ANFUNCTION_BYTEOFFSET   ;}             
attributetype{WSPACE}*={WSPACE}*\"BITOFFSET\"			    {scatr->m_anFunction=ANFUNCTION_BITOFFSET    ;}  
attributetype{WSPACE}*={WSPACE}*\"BITALIGN\"			    {scatr->m_anFunction=ANFUNCTION_BITALIGN    ;}             
attributetype{WSPACE}*={WSPACE}*\"PACKING\"				    {scatr->m_anFunction=ANFUNCTION_PACKING    ;}   
attributetype{WSPACE}*={WSPACE}*\"STRUCTNAME\"				{scatr->m_anFunction=ANFUNCTION_STRUCT_NAME    ;}             
attributetype{WSPACE}*={WSPACE}*\"ELEMENTNAME\"				{scatr->m_anFunction=ANFUNCTION_ELEMENT_NAME    ;}             
           
attributetype{WSPACE}*={WSPACE}*\"ELEMENTTYPE\"			    {scatr->m_anFunction=ANFUNCTION_ELEMENTTYPE  ;}  
attributetype{WSPACE}*={WSPACE}*\"ELEMENTTYPE_UNSIGNED\"	{scatr->m_anFunction=ANFUNCTION_ELEMENTTYPE_UNSIGNED  ;}            
attributetype{WSPACE}*={WSPACE}*\"ARRAYELEMENTS\"		    {scatr->m_anFunction=ANFUNCTION_ARRAYELEMENTS;}      
attributetype{WSPACE}*={WSPACE}*\"RANGE_MIN\"			    {scatr->m_anFunction=ANFUNCTION_RANGE_MIN  ;}             
attributetype{WSPACE}*={WSPACE}*\"RANGE_MAX\"			    {scatr->m_anFunction=ANFUNCTION_RANGE_MAX;} 
attributetype{WSPACE}*={WSPACE}*\"INITVALUE\"			    {scatr->m_anFunction=ANFUNCTION_INITVALUE;} 
attributetype{WSPACE}*={WSPACE}*\"INITREFERENCE\"		    {scatr->m_anFunction=ANFUNCTION_INITREFERENCE;} 
attributetype{WSPACE}*={WSPACE}*\"RANGE_LIST\"			    {scatr->m_anFunction=ANFUNCTION_RANGE_LIST;}
attributetype{WSPACE}*={WSPACE}*\"RANGE_VALUE\"			    {scatr->m_anFunction=ANFUNCTION_RANGE_VALUE;} 
attributetype{WSPACE}*={WSPACE}*\"DEFAULT_STRING\"		    {scatr->m_anFunction=ANFUNCTION_DEFAULT_STRING;}
attributetype{WSPACE}*={WSPACE}*\"TAG_ELEMENT\"			    {scatr->m_anFunction=ANFUNCTION_TAG_ELEMENT;}
attributetype{WSPACE}*={WSPACE}*\"TAG_BITFIELD\"		    {scatr->m_anFunction=ANFUNCTION_TAG_BITFIELD;}

attributetype{WSPACE}*={WSPACE}*\"EXPAND_STRUCTS\"		    {scatr->m_anFunction=ANFUNCTION_EXPAND_STRUCTS;}
attributetype{WSPACE}*={WSPACE}*\"EXPAND_TYPEDEFS\"		    {scatr->m_anFunction=ANFUNCTION_EXPAND_TYPEDEFS;}
attributetype{WSPACE}*={WSPACE}*\"EXPAND_ENUMS\"		    {scatr->m_anFunction=ANFUNCTION_EXPAND_ENUMS;}
attributetype{WSPACE}*={WSPACE}*\"EXPAND_ARRAY\"		    {scatr->m_anFunction=ANFUNCTION_EXPAND_ARRAY;}
attributetype{WSPACE}*={WSPACE}*\"EXPAND_ARRAY_OFFSET\"	    {scatr->m_anFunction=ANFUNCTION_EXPAND_ARRAY_OFFSET;}
attributetype{WSPACE}*={WSPACE}*\"XML_LINE_LENGTH\"		    {scatr->m_anFunction=ANFUNCTION_XML_LINE_LENGTH;}
attributetype{WSPACE}*={WSPACE}*\"XML_TAB_SIZE\"		    {scatr->m_anFunction=ANFUNCTION_XML_TAB_SIZE;}
attributetype{WSPACE}*={WSPACE}*\"XML_ATTRIBUTE_LENGTH\"	{scatr->m_anFunction=ANFUNCTION_XML_ATTRIBUTE_LENGTH;}
attributetype{WSPACE}*={WSPACE}*\"XML_NUMBER_FORMAT\"	    {scatr->m_anFunction=ANFUNCTION_XML_NUMBER_FORMAT;}
attributetype{WSPACE}*={WSPACE}*\"XML_FLOAT_PRECISION\"	    {scatr->m_anFunction=ANFUNCTION_XML_FLOAT_PRECISION;}

attributetype{WSPACE}*={WSPACE}*\"INSERT_PARAM\"		    {scatr->m_anFunction=ANFUNCTION_INSERT_PARAM;}
attributetype{WSPACE}*={WSPACE}*\"SELECT_COPY\"	            {scatr->m_anFunction=ANFUNCTION_SELECT_COPY;}
attributetype{WSPACE}*={WSPACE}*\"SELECT\"				    {scatr->m_anFunction=ANFUNCTION_SELECT;}
attributetype{WSPACE}*={WSPACE}*\"CREATE_STRUCT\"		    {scatr->m_anFunction=ANFUNCTION_CREATE_STRUCT;}

attributetype{WSPACE}*={WSPACE}*\"WARN_ON_SKIPPED_ANNOTATIONS\" {scatr->m_anFunction=ANFUCNTION_WARN_ON_SKIPPED_ANNOTATIONS;}
       
isList{WSPACE}*={WSPACE}*\"								    {lexPushStateConf(GET_BOOL); actualBool=&scatr->m_isList;}
newParent{WSPACE}*={WSPACE}*\"							    {lexPushStateConf(GET_BOOL); actualBool=&scatr->m_newParent;}
Parent{WSPACE}*={WSPACE}*\"								    {lexPushStateConf(GET_STRING); actualString=&scatr->m_parent;}
}

 /*STRING Argument progessing*/
<GET_STRING>{
\"												            {lexPopStateConf(); if (cb_arglist) {cb_arglist();}}
\\\"											            {actualString->stradd("\"", STRADDLENGTH);}
[^\x0d\x0a\"]*									            {actualString->stradd(yyconftext, STRADDLENGTH);}
{NL}											            {actualString->stradd(" ", STRADDLENGTH); confLineCnt++; }
}

 /*ignore STRING Argument*/
<IGNORE_STRING>{
\"												            {lexPopStateConf();}
\\\"											            { }
[^\x0d\x0a\"]*									            {}
{NL}											            {confLineCnt++; }
}

 /*Int Argument processing*/
<GET_INTVALUE>{
\"												            {lexPopStateConf();}
@{LABEL}                                                   {if (actualIntRef != NULL){ actualIntRef->m_reference = yyconftext; actualIntRef->m_type = ARG_INTREF_TYPE_REF; }
                                                             else { confErPrintf("Reference not allowed '%s'", yyconftext); }}
{INUMS}											            {if (actualIntRef == NULL) { *actualInt = atoi(yyconftext); }
                                                             else { actualIntRef->m_value = atoi(yyconftext); actualIntRef->m_type = ARG_INTREF_TYPE_INT; } }
{HEXNUM}										            {if (actualIntRef == NULL) { *actualInt = 0; sscanf(yyconftext, "0x%x", actualInt); }
                                                             else { sscanf(yyconftext, "0x%x", &actualIntRef->m_value); actualIntRef->m_type = ARG_INTREF_TYPE_INT; } }
{NL}											            {confLineCnt++;}
}

 /*STRING_LIST Argument progessing*/
<GET_STRINGLIST>{
\"												            {lexPopStateConf();}
{WSPACE}|,|;									            {}
[^\x0d\x0a\";\t ]*								            {CStringList* list=addToEnd(lNEXT,*actualStringList); list->m_string=yyconftext;};
{NL}											            {confLineCnt++; }
}

<REMOVE_STRINGLIST>{
\"												            {lexPopStateConf();}
{WSPACE}|,|;									            {}
[^\x0d\x0a\";\t]*								            {CStringList::removeFromList(*actualStringList,yyconftext);}
{NL}											            {confLineCnt++; }
}


 /*Bool Argument progessing*/
<GET_BOOL>{
\"												            {lexPopStateConf();}
"true"|1										            {if (!skipKeywords) *actualBool = true; }
"false"|0										            {if (!skipKeywords)*actualBool = false; }
{NL}											            {confLineCnt++;}
}

 /*Argument processing*/
<GET_ARGUMENTS>{
\"												            {lexPopStateConf();}
;												            {} /* delimiter */
NONE                     						            {addArgType(ANARG_NONE            );}     
IGNORE											            {addArgType(ANARG_IGNORE          );}     
STRING											            {addArgType(ANARG_STRING          );}     
LABEL											            {addArgType(ANARG_LABEL           );} 
ENUM_ELEMENT											    {addArgType(ANARG_ENUM_ELEMENT    );}  
ENUM_TYPE													{addArgType(ANARG_ENUM_TYPE		  );}         
BOOL											            {addArgType(ANARG_BOOL            );}     
INTEGER											            {addArgType(ANARG_INTEGER         );}     
UNSIGNED_INTEGER								            {addArgType(ANARG_UNSIGNED_INTEGER);}   
UNSIGNED_INTEGER_INF                                        {addArgType(ANARG_INTEGER); scatr->m_m1EqualsInfinity = true; } /* range = -1..max_int, -1 = Infinity*/
FLOAT											            {addArgType(ANARG_DOUBLE           );}     
VARIANT											            {addArgType(ANARG_VARIANT         );}     
}											

 /* all thats left */
<*>{NL}											            {confLineCnt++; }
<*>.											            {unmachedCnt++; 
                                                                if (unmachedCnt==1) {
                                                                    confErPrintf("Unmached character '%s'",yyconftext);
                                                                }
                                                                if (unmachedCnt==2) {
                                                                    confErPrintf("Unmached character '%s'. More follow.",yyconftext);
                                                                }
                                                            }

%%

void lexInitConf()
{
    memset(state2Enum,0,sizeof(state2Enum));
    state2Enum[TYPE_CONFIG]=ANGROUP_GLOBAL;
    state2Enum[H2XML_CONFIG]=ANGROUP_GLOBAL;
    state2Enum[GLOBAL_ATTRIBUTES] = ANGROUP_GLOBAL;
    state2Enum[XML_GENERATOR_ATTRIBUTES]=ANGROUP_XMLGEN;
    state2Enum[MODULE_ATTRIBUTES]=ANGROUP_MODULE;
    state2Enum[STRUCTURE_ATTRIBUTES]=ANGROUP_STRUCTURE;
    state2Enum[ELEMENT_ATTRIBUTES]=ANGROUP_ELEMENT;
}

/* yywrap: Called when EOF is reached on current input.
* Have it return 1 if processing is finished, or
* do something to fix the EOF condition (like open
* another file and point to it) and return 0 to indicate
* more input is available.
*/
int yywrap()
{
    return 1;
}

int stopConf()
{
    lexChangeStateConf(INITIAL);
    lexStateIndexConf = 0;
    return 0;
}

long lexPushStateConf(long state)
{
    statesConf[lexStateIndexConf] = YYSTATE;
    lexStateIndexConf++;
    if (lexStateIndexConf >= MAX_LEX_STATES) {
        printf("LEX maximum number of lex states reached!\n");
        if (yy_flex_debug) {
            fprintf(stderr, "LEX maximum number of lex states reached!\n");
        }
        lexStateIndexConf--;
    }
    else {
        BEGIN state;
    }
    if (yy_flex_debug) {
        fprintf(stderr, "Flex new state (push): %s %d %ld\n", stateStrConf[YYSTATE], YYSTATE, lexStateIndexConf - 1);
        //printf("Flex new state (push): %s %d %ld\n", stateStrConf[YYSTATE], YYSTATE, lexStateIndexConf - 1);
    }
    return lexStateIndexConf;
}

void lexChangeStateConf(long state)
{
    lexPopStateConf();
    lexPushStateConf(state);
}

long lexPopStateConf()
{
    lexStateIndexConf--;
    BEGIN statesConf[lexStateIndexConf];
    if (lexStateIndexConf<0) {
        printf("LeX state stack <0!\n");
        if (yy_flex_debug) {
            fprintf(stderr, "LEX state stack <0!\n");
        }
        lexStateIndexConf = 0;
    }
    if (yy_flex_debug) {
        fprintf(stderr, "Flex new state (pop): %s %d %ld\n", stateStrConf[YYSTATE], YYSTATE, lexStateIndexConf);
        //printf("Flex new state (pop): %s %d %ld\n", stateStrConf[YYSTATE], YYSTATE, lexStateIndexConf);
    }
    return lexStateIndexConf;
}

void confErPrintf(char* format, ...)
{
    char str1[10000];
    va_list ap;
    va_start(ap, format);
    vsprintf_s(str1,9999, format, ap);
    dbgPrintf(TERROR, "%s(%d): %s\n",gd->config->m_filename.buf,confLineCnt,str1);
    if (yy_flex_debug) {
        fprintf(stderr, "Error: %s", str1);
    }
}



void addArgType(eAnArgTypes argType) { 
    CAnArgTypeList** ptAnArgTypeList = &scatr->m_anArgTypeList;
    if (is_argTpeList) {
        ptAnArgTypeList = &scatr->m_anArgTypeListList;
    }

    if (argType == ANARG_STRING && *ptAnArgTypeList) {
        confErPrintf("Type 'String' cannot be combined with any other types");
    }
    else if (*ptAnArgTypeList && (*ptAnArgTypeList)->m_argType == ANARG_STRING) {
        confErPrintf("Type 'String' cannot be combined with any other types");
    }
    else {
        CAnArgTypeList *list = addToEnd(lNEXT, *ptAnArgTypeList);
        list->m_argType = argType;
    }
    if (is_argTpeList) {
        scatr->m_knArgumentsList++;
    }
    else {
        scatr->m_knArguments++;
    }
}

void insertAttributes() 
{
    CStr tmp=scatr->m_prefix+scatr->m_keyword;
    if (skipKeywords) {
        tmp = tmp + KEYWORD_SKIPPED_PREFIX;
    }
    gd->config->m_htab.hashinstallInsertEnd(tmp.buf, scatr,&gd->config->m_htabList); 
    dbgPrintf(TINFO, "Insert Keyword <%s>\n", tmp.buf);

    // check  if keyword is in default list
    int i=0;
    while (defaultFunctions[i].fn!=0) {
        if(scatr->m_anFunction>kn_ANFUNCTION && strcasecmp(defaultFunctions[i].keyWord,scatr->m_keyword.buf)==0 ) {
            scatr->m_anFunction=defaultFunctions[i].fn;
            break;
        }
        i++;
    }
}

void lexStateGetString(CStr* string)
{
    lexPushStateConf(GET_STRING); 
    *string="";
    actualString= string;
}

CConfigAttributes* newConfigAttribute(eAnType type)
{
    CConfigAttributes* scatr;
    scatr = new(CConfigAttributes); 
    scatr->m_anGroup = actualGroup;
    scatr->m_prefix = actualPrefix; 
    scatr->m_anFunction = (unsigned long)scatr;
    scatr->hash.cases = KEYWORDS_NOT_CASESENSITIVE; 
    scatr->m_accessSpecPrivate = accessSpecPrivate;
    if (type == ANTYPE_ALIAS){
        scatr->m_showInOutput = false;
    }
    scatr->m_anType = type; 
    lexPushStateConf(SET_ATTRIBUTES);
    return scatr;
}

void setPrefix() 
{
	CStringList* list=addToEnd(lNEXT, gd->config->m_prefixList[actualGroup]);
	if (!skipKeywords) {
		gd->config->m_prefix[actualGroup]=gd->config->m_actualPrefix[actualGroup];
	}
	cb_arglist=NULL;
}