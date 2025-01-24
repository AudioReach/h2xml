/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////////////
//! \brief	compiler global function implementation										\file
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "global_functions.h"
#include "CList.h"
#include "scanConfig.h"
#include <stdarg.h>

char StatementString[][100] = { "STATEMENT_UNDEFINED",
"STATEMENT_DEKLARATION",
"STATEMENT_DEKLARATION_TYPE",
"STATEMENT_FUNCTION_DEKLARATION",
"STATEMENT_FUNCTION_DEFINITION",
"STATEMENT_LABEL",
"STATEMENT_CASE",
"STATEMENT_DEFAULT",
"STATEMENT_EXPRESSION",
"STATEMENT_COMPOUND_STATEMENT",
"STATEMENT_IF",
"STATEMENT_IFELSE",
"STATEMENT_SWITCH",
"STATEMENT_WHILE",
"STATEMENT_DO",
"STATEMENT_DOWHILE",
"STATEMENT_FOR",
"STATEMENT_GOTO",
"STATEMENT_CONTINUE",
"STATEMENT_BREAK",
"STATEMENT_RETURN",
"STATEMENT_EMPTY"
};

char OperatorString[][10] = {
	"UDEF", "CINT", "CFLOAT", "IDENT", "IDENT_YCC", "FUNCTION", "=", "*=", "/=", "%=", "+=",
	"-=", "<<=", ">>=", "^=", "|=", "&=", "if",
	",", "||", "&&", "|", "^", "&", "==", "!=", "<", ">", "<=", ">=", "<<", ">>", "+", "-", "*", "/", "%", "x", "++", "--", "&",
	"*", "+", "-",
	"~", "!",
	"[", ".", "->", "++", "--"
};


char Typenames[][100] = {
	"TYPE_UNKNOWN",
	"TYPE_CONST_INT",	// constant int                     
	"TYPE_CONST_FLOAT",	// constant float                        
	"TYPE_FIXP24",		// fixp24                                
	"TYPE_FIXP24S",		// fixp24 stereo                         
	"TYPE_PFIXP24",		// pointer to fixp24                     
	"TYPE_PFIXP24S",	// pointer to fixp24 stereo          
	"TYPE_AFIXP24",		// array fixp24                          
	"TYPE_AFIXP24S",	// array fixp24                      
	"TYPE_TYPEDEF",		// i.e. typedef", struct etc             
	"TYPE_STRUCT",
	"TYPE_UNION",
	"TYPE_INT",
	"TYPE_PINT",
	"TYPE_FLOAT",
	"TYPE_PFLOAT",
	"TYPE_CHAR",
	"TYPE_PCHAR",
	"TYPE_SHORT",
	"TYPE_PSHORT",
	"TYPE_LONG",
	"TYPE_PLONG",
	"TYPE_SIGNED",
	"TYPE_UNSIGNED",
	"TYPE_VOID",
	"TYPE_BOOL",
	"TYPE_REG_ACCU",
	"TYPE_REG_M",
	"TYPE_REG_C",
	"TYPE_REG_L",
	"TYPE_FLAG_F",
	"TYPE_FLAG_T"
};

char anGroupNames[kn_ANGROUP][30] = {
	"ANGROUP_NONE",
	"ANGROUP_GLOBAL",
	"ANGROUP_MODULE",
	"ANGROUP_STRUCTURE",
	"ANGROUP_ELEMENT",
    "ANGROUP_BITFIELD",
	"ANGROUP_XMLGEN",
};


char anArgNames[kn_ANARG][30] = {
	"NONE",
	"IGNORE",
	"STRING",
	"LABEL",
	"CONSTLABEL",
    "ENUM_ELEMENT",
    "ENUM_TYPE",
	"BOOL",
	"INT",
	"UINT",
    "FLOAT32",
	"DOUBLE",
    "VARIANT",
    "REFERENCE"
};

char tmpBuf[TMP_BUF_SIZE];
char tmpBuf2[TMP_BUF_SIZE];


extern 			void endStateParamBody();					// will be called at end of statement
extern			int yyannot_flex_debug;

#define DBGFILE "h2xml_verbose.txt"

type_struct TypeConvTable[kn_EXP_OPERATORS][kn_TYPES][kn_TYPES] = {  };

initializerlist* addInitializer(initializerlist* list, initializerlist* next, double data)
{
	initializerlist* tmp = new initializerlist;
	tmp->data = data;
	tmp->next = next;
	tmp->list = list;
	return tmp;
}

typeQualifier* addTypeQualifier(typeQualifier* list, int type)
{
	typeQualifier* tmp = new typeQualifier;
	tmp->type_qualifier = type;
	tmp->next = list;
	return tmp;
}

CStatement* new_CStatement(int token, CIdentifier* ident, int linenumber)
{
	CStatement* tmp = new (CStatement);
	tmp->m_token = token;
	tmp->m_codeline = as.ycc_code;
	if (linenumber != 0)
		tmp->m_codeline_cnt = linenumber;
	else
		tmp->m_codeline_cnt = as.linecnt;
	typedef_seen = false;
	struct_seen = false;
	if (ident != NULL && (token == STATEMENT_DEKLARATION || token == STATEMENT_DEKLARATION_TYPE)){
		tmp->m_identifier = ident;
		checkTypedef(ident);
		if (ident->m_token == IDENTIFIER_FUNCTION || ident->m_token == IDENTIFIER_FUNCTION_NOARGS) {
			tmp->m_token = STATEMENT_FUNCTION_DEKLARATION;
		}
		endStateParamBody();

		as.last_statement = tmp;
	}
	return tmp;
}




///////////////////////////////////////////////////////////////////
//Errorhandling
///////////////////////////////////////////////////////////////////

yerrcode ycc_errmessage[] = {
	{ 0, { "Syntax Error: %s" } },
	{ 1, { "Expression <%s> must be constant" } },
    { 3, { "cannot open input file <%s>\n\n Compilation aborted" } },
	{ 4, { "cannot open include file <%s>\n\n Compilation aborted" } },
	{ 5, { "Infinite recursion in variable <%s>" } },
	{ 6, { "Label <%s> not initialized" } },

	{ 100, { "'%s' : undeclared identifier" } },
	{ 101, { "'%s' : redefinition" } },
	{ 102, { "'%s' : redefinition with different parameters" } },
	{ 103, { "term does not evaluate to a function" } },
	{ 104, { "'%s' :wrong number of parameters" } },
	{ 105, { "'%s' :no typeconversion for parameters" } },
	{ 106, { " left of '.%s' must have struct or union type" } },
	{ 107, { " left of '->%s' must point to struct or union" } },
	{ 108, { " identifier is not a meber of '%s'" } },
	{ 109, { " more than one storage class specified" } },
	{ 110, { " subscript requires array or pointer type" } },
	{ 111, { " illegal indirection" } },
	{ 112, { " '%s': illegal use of namespace identifier" } },
	{ 113, { " '%s' must be a namespace when followed by '::'" } },
	{ 114, { "The variable '%s' is being used without being initialized." } },
	{ 115, { "Redeclaration of variable '%s'." } },
	{ 116, { "Value redeclaration in ENUM '%s'." } },
	{ 117, { "Parameter passing by Reference used: C++ only (not supported) '%s'." } },
	{ 118, { "Size of variable '%s' is Zero." } },
	{ 119, { "'STRUCT' %s type redefininition." } },


	{ 200, { "::select Identifier <%s> not defined" } },
	{ 201, { "more than 500 elemts of struct/union parsed, syntax error expected !" } },
	{ 202, { "redefinition of struct: '%s' !" } },

	{ 602, { "unmatched <.EndModule> statement in line <%s>" } },
	{ 603, { "unmatched <.Module> statement" } },
	{ 604, { "cannot restore old module name" } },
	{ 605, { "unmatched <.EndModule> statement, expecting <.endmodule %s>" } },
	{ 606, { "unmatched <#endif> statement" } },
	{ 607, { "unmatched <#else> statement" } },
	{ 608, { "unmatched <#if> statement" } },

	{ 700, { "unknown keyword <%s>" } },

	{ 901, { "syntax error: missing ';' before '%s'" } },
	{ 902, { "syntax error in variable declaration" } },
	{ 903, { "syntax error in variable initialization" } },
	{ 904, { "syntax error: missing ')' before '%s'" } },
	{ 905, { "syntax error: missing '}' before '%s'" } },

	{ 1000, { "Operation <%s> not defined" } },

	{ 9999, { "YDFP Compiler Error, unknown error code <%s>" } },



	{ -1, { "end of messages" } }

};



//print errormessage
void ycc_erpnt(int en, int value)
{
	char tmp[10000];
	sprintf(tmp, "%d", value);
	ycc_erpnt(en, tmp);
}

//print errormessage
void ycc_erpnt(int en, const char* message, int value, int warning, int linecnt, char* inputfile)
{
	if (linecnt<0) linecnt = as.linecnt;
	if (inputfile == NULL) inputfile = as.inputfile.buf;

	as.skiperror = false;
	if (as.skiperror == false) {
		// find index
		int i = 0;
		int index = -1;
		while (ycc_errmessage[i].ernum >= 0 && index<0) {
			if (ycc_errmessage[i].ernum == en) index = i;
			i++;
		}

		if (index >= 0) {

			char tmp[4000];
			char* tmp2 = tmp;

			if (warning == 0) {
				sprintf(tmp2, """""%s""""(%d): ERROR %d: ", inputfile, linecnt, en);
				as.errorcount++;
			}
			else {
				sprintf(tmp2, """""%s""""(%d): WARNING %d: ", inputfile, linecnt, en);
				as.warningcount++;
			}

			tmp2 = strcat(tmp2, ycc_errmessage[index].message);
			tmp2 = strcat(tmp2, "\n");


			char outmessage[10000];
			if (message == NULL)
				sprintf(outmessage, tmp2, " ", 0);
			else
				sprintf(outmessage, tmp2, message, value);

			// output
			printf("%s", outmessage);
			if (yycompiler_flex_debug != 0){
				fprintf(stderr, "%s", outmessage);
				fflush(stderr);
			}
		}
		else
		{
			ycc_erpnt(9999, en);
		}
	}
}

void initYdfpcc()
{
	// clear hash table
	htab.hashClear();

	insertint("ydfpcc", 0, 2);				//flag YDFPCC
	Cvdt* tmp = htab.hashinstall("__H2XML__");
	tmp->token = TOKEN_DEFINE;
	tmp->valstr = "1";

	// Datatypes	// sizes in Bytes
	CDataType* type_const_int = CDataType::newCDataType("__type_const_int__", TYPE_CONST_INT, SIZE_INT);
	CDataType* type_const_float = CDataType::newCDataType("__type_const_float__", TYPE_CONST_FLOAT, SIZE_FLOAT);
	CDataType* type_const_string = CDataType::newCDataType("__type_const_string__", TYPE_CONST_FLOAT, SIZE_CHAR);
	CDataType* type_unknown = CDataType::newCDataType("TYPE_UNKNOWN", TYPE_UNKNOWN, 0);

	as.type_int = CDataType::newCDataType("int", TYPE_INT, SIZE_INT);
	as.type_float = CDataType::newCDataType("float", TYPE_FLOAT, SIZE_FLOAT);
	as.type_double = CDataType::newCDataType("double", TYPE_DOUBLE, SIZE_DOUBLE);
	as.type_char = CDataType::newCDataType("char", TYPE_CHAR, SIZE_CHAR);
	as.type_short = CDataType::newCDataType("short", TYPE_SHORT, SIZE_SHORT);
	as.type_long = CDataType::newCDataType("long", TYPE_LONG, SIZE_LONG);
	as.type_long_long = CDataType::newCDataType("longlong", TYPE_LONG_LONG, SIZE_LONG_LONG);
	as.type_signed = CDataType::newCDataType("signed", TYPE_SIGNED, SIZE_SIGNED);
	as.type_unsigned = CDataType::newCDataType("unsigned", TYPE_UNSIGNED, SIZE_UNSIGNED);
	as.type_void = CDataType::newCDataType("void", TYPE_VOID, SIZE_VOID);

	// predefined variables
	// NULL-pointer for C
	Cvdt* tmp_cvdt = InsertCIdentifier("NULL", "void", 0);
	tmp_cvdt->pointer_data = 0;
	tmp_cvdt->data = 0;
	tmp_cvdt->identifier->m_pointer_level = 1;

	as.ident_const_int = newCIdentifier(type_const_int, "__IDENTIFIER_CONST_INT__");
	as.ident_const_float = newCIdentifier(type_const_float, "__IDENTIFIER_CONST_FLOAT__");
	as.ident_const_string = newCIdentifier(type_const_string, "__IDENTIFIER_CONST_STRING__");
	as.ident_undef = newCIdentifier(type_unknown, "__IDENTIFIER_UNKNOWN__");
	as.ident_int = newCIdentifier(as.type_int, "__IDENTIFIER_INT__");
	as.ident_float = newCIdentifier(as.type_float, "__IDENTIFIER_FLOAT__");


	// global as struct initialization
	as.doxycomment = "";

	as.inputfile = "";
    as.outputfile = "";
	as.file_extention = "";
	as.outputpath = ".";
	as.outputpathRtf = "";

    as.verbose = false;
    as.printVersion = false;
	as.output_level = 0;
	as.pointerSize = 4;
	as.enumSize = 4;
    as.isEnumTypedf = false;

	// must be after initializing of predefined variables!
	initTypeConvTable();

	//clearAsmHeaderStatment(&as.ah);
	as.last_identifier = NULL;
	as.last_statement = NULL;

    as.fpStderr = NULL;
}


void yyerror(char const* s)
{
	//printf("Error: %s\n",s);
	fprintf(stderr, "ERROR %s", s);
	ycc_erpnt(as.error_code, yycompilertext);
}

void SetDefaults()
{
	as.last_var_name = NULL;
}

void newline_ydfpcc()
{
	SetDefaults();
	lex_begin_readline_ydfpcc();
	as.linecnt++;
	if (as.do_include) {
		as.include.Extract("\"", "\"", 0);
		process_include(as.include.buf);
	}
	as.do_include = false;
}

void er(int ercode)
{
	as.error_code = ercode;
	as.error_text = yycompilertext;
}

void init_values()
{
	as.linecnt = 0;
	as.total_linecnt = 0;
	as.skip_input = false;
	as.module_list = NULL;
	as.module_name = "";
	as.include_cnt = 0;
	as.iflist = NULL;
	as.ydfp_version = 0;
	as.do_include = false;
}


void checkTypedef(CIdentifier* id){
	if (id->m_declspec->isTypedef()) {
		if (id->m_cvdt != NULL) {
			//typedef happened: insert new type (only when no struct (already done in this case)
			CDataType* tmp = CDataType::newCDataType(id->getName(),
				id->GetTypeId(),
				id->GetTypeSize(),
				id->getDataType()->GetElements());
			tmp->m_align = id->GetTypeAlign();
		}
		else { // (id->m_cvdt == NULL)
			id->m_declspec->setStorageSpecifier(0);
		}
	}
	return;
}

void insertShortNameStruct(CStatement* stat)		// for struct xyz {};
{
	if (stat != NULL && stat->m_identifier != NULL && stat->m_identifier->m_declspec != NULL) {
        if (stat->m_identifier && stat->m_identifier->getDataType() && stat->m_identifier->getDataType()->GetTypeName()){
            char* name = stat->m_identifier->getDataType()->GetTypeName();
            if (name){
                strncpy(tmpBuf, HASH_PREFIX, TMP_BUF_SIZE - 1);
                strncat(tmpBuf, name, TMP_BUF_SIZE-100);
                Cvdt* tmp = htab.hashinstall(tmpBuf);
                tmp->identifier = stat->m_identifier;
            }
        }
	}
}


void insertShortName(CStatement* stat)
{
	if (stat != NULL && stat->m_identifier != NULL && stat->m_identifier->m_cvdt != NULL) {
		// generate name entry in hash table for parser
        strncpy(tmpBuf, HASH_PREFIX, TMP_BUF_SIZE - 1);
		strncat(tmpBuf, stat->m_identifier->m_cvdt->shortname.buf, TMP_BUF_SIZE - 100);
		//printf("%s\n",stat->m_identifier->m_cvdt->shortname.buf);
		if (strcmp(stat->m_identifier->m_cvdt->shortname.buf, "sMAsRoutingUi_t") == 0) {
			//int dbg=0;
		}
		Cvdt* tmp = htab.hashinstall(tmpBuf);
		tmp->identifier = stat->m_identifier;
		if (stat != NULL && stat->m_identifier != NULL && stat->m_identifier->m_declspec != NULL &&
			stat->m_identifier->m_declspec->datatype != NULL && stat->m_identifier->m_declspec->datatype->m_elements != NULL) {
			CIdentifier* id = stat->m_identifier->m_declspec->datatype->m_elements;
			stat->m_identifier->m_declspec->datatype->generateName(id, tmpBuf);

		}
		else {
			//Cvdt* tmp=htab.hashinstall(buf);
			//tmp->identifier=stat->m_identifier;
		}
	}
}

void fix_var_names(CDataType* type){

	char tmpstr[1000];
	CStr tmpstr2;
	CIdentifier* tmpID;
	CIdentifier* idlist = type->GetElements();

	//remove tmp_struct prefix and insert new ident with correct struct-prefix
	for (CIdListIterator it = idlist->GetIdList_Begin(); it != idlist->GetIdList_End(); it++) {
		sprintf(tmpstr, "__tmp_struct%d_", struct_typedef_cnt);
		tmpstr2 = it->m_cvdt->name;
		tmpstr2.Replace(tmpstr, "");
		sprintf(tmpstr, "%s_%s", type->GetTypeName(), tmpstr2.buf);
		tmpID = it->m_cvdt->identifier;
		Cvdt* lastcvdt = it->m_cvdt;
		it->m_cvdt = htab.hashinstall(tmpstr);
		it->m_cvdt->token = TOKEN_IDENTIFIER;
		it->m_cvdt->identifier = tmpID;
		it->m_cvdt->shortname = lastcvdt->shortname;
	}

	struct_typedef_cnt++;
	return;
}



void fix_id_after_struct(CDataType* type, Cvdt* cvdt) {
	char tmpstr[1000];
	CStr tmpstr2;

	if (typedef_seen) return;

	//remove tmp_struct prefix and insert new ident with correct type
	sprintf(tmpstr, "__tmp_struct%d_", struct_typedef_cnt - 1);
	tmpstr2 = cvdt->name;
	tmpstr2.Replace(tmpstr, "");

	//insert new variable
	CIdentifier* id = newCIdentifier(type, tmpstr2.buf, 0);
	id->m_cvdt->token = TOKEN_IDENTIFIER;

	return;
}


void insert_struct_bitslice(CIdentifier* id, int value){
	id->m_bitslice = value;
	return;
}

CIdentifier* fix_noVar_in_struct(int bitslice) {

	CIdentifier* tmp;

	tmp = newCIdentifier(as.type_char, "tmp_reserved", 0);
	tmp->m_cvdt->shortname = "__Reserved__";
	tmp->m_bitslice = bitslice;
	return tmp;

	//return newCIdentifier((Cvdt*)NULL);
}

decl_specifiers::decl_specifiers(CDataType* dtype, int storage, int tqualifier)
: storage_specifier(storage), type_qualifier(tqualifier), datatype(dtype) {
	datatype2 = 0;
	if (dtype && dtype->m_cvdt && dtype->m_cvdt->identifier && dtype->m_cvdt->identifier->m_declspec) {
		datatype2 = dtype->m_cvdt->identifier->m_declspec->datatype2;
	}
}

void decl_specifiers::setStorageSpecifier(int sspecifier) {

	//	if (storage_specifier != 0 && sspecifier != 0) {
	//		ycc_erpnt(109);
	//	}

	//storage_specifier = sspecifier;
	// use this instead of above, should be more safe
	if (as.declspec != NULL)
		as.declspec->storage_specifier = sspecifier;

};

bool decl_specifiers::isTypedef() const {
	return storage_specifier == TYPEDEF;
};

bool decl_specifiers::isExtern() const {
	return storage_specifier == EXTERN;
};

char *decl_specifiers::getTypeQualifierString() const {
	switch (type_qualifier) {
	case CONST: return "const";
	case VOLATILE: return "volatile";
	default: return NULL;
	}
};
char *decl_specifiers::getStorageSpecifierString() const {
	switch (storage_specifier) {
	case TYPEDEF: return "typedef";
	case EXTERN: return "extern";
	case STATIC: return "static";
	case AUTO: return "auto";
	case REGISTER: return "register";
	default: return NULL;
	}
};

void decl_specifiers::setDataType(CDataType* dtype)  {
	/*datatype2=datatype;
	datatype = dtype; */
	datatype2 = dtype;
	if (typedef_seen)
		setStorageSpecifier(TYPEDEF);
}

void PrintCIdentifierBlock(CIdentifierBlock* tmp, const char* tmp2) {
	/*printf("%s",tmp2);
	if (tmp->first!=NULL) printf(" %s ",tmp->first->GetTypeName());
	if (tmp->last!=NULL) printf(" %s ",tmp->last->GetTypeName());
	printf("\n");*/
};

void insertEnum(Cvdt* cvdt, long64 data)
{
	cvdt->data = (double) data;
	cvdt->token = TOKEN_DEFINE;
    cvdt->isEnumElement = true;
    cvdt->enumName = as.enumName;
    char tmp2[100];
    sprintf(tmp2, "%lld", data);
	cvdt->valstr = _strdup(tmp2);
}

Cvdt* findIdentifier(char* identName) {

    CStr name = "_parser_";
    name = name + identName;
    name.ReplaceAll("::", ".");
    Cvdt* tmp = htab.hashlookup(name.buf);      // structure
    if (tmp == NULL) {
        tmp = htab.hashlookup(identName);       // enum, or referenced by enumName::element
    }
    return tmp;
}


