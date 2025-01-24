/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

/**
*	\file
*/

#ifndef _H2XML_COMMON_H__INCLUDED_
#define _H2XML_COMMON_H__INCLUDED_

#include <stdarg.h>
#include <stdio.h>
#include "h2xml_global.h"
#include "CStr.h"
#include "CList.h"

#define CONST_LABELS_TO_CAPITAL


#define RETVAL_OK 0
#define RETVAL_ERROR 1
#define RETVAL_MESSAGE 2

#define DEFAULT_FLOAT_PRECISION 7

typedef enum {
	COMPILE_ATTR_NONE = 0,
	COMPILE_ATTR_PACKED = 1,
	COMPILE_ATTR_ALIGNED = 1,
	knCompileAttributes
}eCompileAttributes;

// enums
typedef enum  {
	TMSGALL,
	TMSG,
	TINFO,
	TWARNING,
	TERROR,
	knErTypes
} eMsgTypes;

////////////////////////////////////
// XML states
////////////////////////////////////

typedef enum {
	ANGROUP_NONE,
	ANGROUP_GLOBAL,
	ANGROUP_MODULE,
	ANGROUP_STRUCTURE,
	ANGROUP_ELEMENT,
    ANGROUP_BITFIELD,
	ANGROUP_XMLGEN,
	kn_ANGROUP
} eAnGroups;

typedef enum {
    TAGLEVEL_GLOBAL=1,

}eTagLevel;

extern char anGroupNames[kn_ANGROUP][30];

#define MAXPREFIXNAMESIZE 40

/** Value display type */
typedef enum {
	ANARG_DPLY_NORMAL,
	ANARG_DPLY_HEX,
	ANARG_DPLY_INTEGER,
	knANARG_DPLY
}eAnArgDPLYType;


/** Annotation argument types */
typedef enum  {
	ANARG_NONE,
	ANARG_IGNORE,
	ANARG_STRING,
	ANARG_LABEL,
	ANARG_CONSTLABEL,				// constant value label, i.e. only fix values allowed as defined in config file
    ANARG_ENUM_ELEMENT,             // argument is an enumerated element (name / value pair)
    ANARG_ENUM_TYPE,                // argument is an enum type
	ANARG_BOOL,
	ANARG_INTEGER,
	ANARG_UNSIGNED_INTEGER,
    ANARG_FLOAT32,
	ANARG_DOUBLE,
    ANARG_VARIANT,                 // integer or float
    ANARG_REFERENCE,               // references another identifier@keyword, will be evaluated at XML generation
	kn_ANARG
} eAnArgTypes;
extern char anArgNames[kn_ANARG][30];

/* special restrictions for numerical arguments*/
typedef enum {
    ANARGFORAMT_NORMAL,
    ANARGFORAMT_ODD,
    ANARGFORAMT_EVEN,
    knAnArgFormats
}eAnArgFormat;

/** Special annotation Function */
typedef enum {
	ANFUNCTION_GENERAL,								// default value, no special function
	ANFUNCTION_NAME,
	ANFUNCTION_GENERATE_OUTPUT,
	ANFUNCTION_DEFAULT,
	ANFUNCTION_DEFAULTLIST,
	ANFUNCTION_DEFAULTFILE,
	ANFUNCTION_DEFAULTLIST_CONDITIONAL,
	ANFUNCTION_DEFAULTFILE_CONDITIONAL,
	ANFUNCTION_DEFAULT_CONDITIONAL,
	ANFUNCTION_ARRAY_DEFAULT,
	ANFUNCTION_RANGE_LIST,
	ANFUNCTION_BITMASK,
	ANFUNCTION_RANGE_VALUE,
	ANFUNCTION_GLOBALANNOTATIONS,					// following annotations are global
	ANFUNCTION_INSERT_PARAM,						// insert external parameter to Module
    ANFUNCTION_SELECT_COPY,				            // select and copy identifier. All annotations will move to this copy and not alter the original identifier
	ANFUNCTION_SELECT,								// select an identifier
	ANFUNCTION_CREATE_STRUCT,						// create an identifier

    //Tags
    ANFUNCTION_TAG_ELEMENT,
    ANFUNCTION_TAG_BITFIELD,

    //Aliases
    ANFUNCTION_ALIAS_BITFIELD,
    ANFUNCTION_ALIAS_BITFIELD_END,

	//internally generated
	ANFUNCTION_BYTESIZE,
	ANFUNCTION_BITESIZE,
	ANFUNCTION_BITALIGN,
	ANFUNCTION_BYTEOFFSET,
	ANFUNCTION_BITOFFSET,
	ANFUNCTION_ELEMENTTYPE,
	ANFUNCTION_ELEMENTTYPE_UNSIGNED,
	ANFUNCTION_ARRAYELEMENTS,
	ANFUNCTION_RANGE_MIN,
	ANFUNCTION_RANGE_MAX,
    ANFUNCTION_INITVALUE,
    ANFUNCTION_INITREFERENCE,                       // reference to annotation of an initializer enum
	ANFUNCTION_DEFAULT_STRING,
	ANFUNCTION_PACKING,								// compiled with packed attribute?
	ANFUNCTION_STRUCT_NAME,							// origiginal structure name 
	ANFUNCTION_ELEMENT_NAME,						// origiginal element name

	// Generation attributes
	ANFUNCTION_EXPAND_STRUCTS,
	ANFUNCTION_EXPAND_TYPEDEFS,
	ANFUNCTION_EXPAND_ENUMS,
	ANFUNCTION_EXPAND_ARRAY,
	ANFUNCTION_EXPAND_ARRAY_OFFSET,
	ANFUNCTION_XML_LINE_LENGTH,
	ANFUNCTION_XML_ATTRIBUTE_LENGTH,
	ANFUNCTION_XML_TAB_SIZE,
	ANFUNCTION_XML_NUMBER_FORMAT,
    ANFUNCTION_XML_FLOAT_PRECISION,

	// control functions
	ANFUCNTION_CTRL_SAVEACTIVETAG,
	ANFUCNTION_CTRL_RESTOREACTIVETAG,
	ANFUCNTION_CTRL_SETACTIVETAG,
    ANFUCNTION_CTRL_INTERNAL_BITFIELD_END,              // signals end of bitfield definition
    ANFUCNTION_CTRL_BREAK,                              // stop evaluation at this function
    ANFUCNTION_CTRL_INTERNAL_RESTORE_TAGLEVEL,          // pop taglevel after previous push
    ANFUNCTION_CTRL_INSERT_COMMANDLINE,                 // insert string from command line, only annotations of the respective type (module, param, element etc.)
	ANFUNCTION_CTRL_INC_HIRACHY_LEVEL,
	ANFUNCTION_CTRL_DEC_HIRACHY_LEVEL,

    // WARNING and Error Messages
    ANFUCNTION_WARN_ON_SKIPPED_ANNOTATIONS,
	kn_ANFUNCTION

}eAnFunction;


/** Associate some keywords with special functions 
* this is mainly supposed to be used for backward compatibility
* in General, it is preferred to associate a function to a keyword in
* the config file */
typedef struct {
    long fn;
    char keyWord[100];
}sDefaultFunction_t;

/** Special annotation Types */
typedef enum {
	ANTYPE_UNDEFIEND,
	ANTYPE_ALIAS,
	ANTYPE_TAG,
	ANTYPE_ATTRIBUTE,
	ANTYPE_CONTROL,
	kn_ANTYPE
}eAnType;

// classes
class CStringList : public CList<CStringList>
{
public:
	CStringList();
	CStr m_string;
	static bool findInList(CStringList* list, char* findString);		                    // return true, if findString in list
    static bool removeFromList(CStringList* list, char* findString);                        // remove from list, return true, if findString in list
    static void appendStringList(CStringList* &list1, CStringList* list2);     // append list2 to list1
    static void deleteStringList(CStringList* &list1);                                      // remove list from memory
    void printList(CStr* outList);
};

// classes
class CIntList : public CList<CIntList>
{
public:
    CIntList();
    int  m_value;
    void push(int value);
    int pop();
};

// classes
class CAnnotationData;

typedef enum {
    DEFAULT_TYPE_UNDEFINED,
    DEFAULT_TYPE_VALUE,
    DEFAULT_TYPE_LIST,
    knDefaultListTypes
}eDefaultListType_t;

class CDefaultList: public CList<CDefaultList>
{
public:
	CDefaultList();
	~CDefaultList();
	unsigned char* m_defaultBuffer;
	long m_defaultBufferBitSize;
	long m_defaultBufferSize;
	long m_startAddress;
    eAnArgTypes m_format;

    eDefaultListType_t m_type;
    CAnnotationData* m_anData;
	void copy(CDefaultList* source);
};


#define MAX_TAG_LEVEL 100				// maximum hirarchical level for nested structs
#define MAX_IDENT_LIST_LEVEL 100		// maximum hirarchical level for nested Modules


class CModuleList;
class CIdentList;
class CXMLTag;
class CConfig;
class CValue;

class CGlobalData {
	
public:
	CGlobalData();
	~CGlobalData();
	void init();
	int warningcount;
	int errorcount;				// number of errors
	int verbose;				//
	int linecnt;				//
    int macroLevel;             // 

	CConfig *config;			// configuration settings from config file
	CConfig *actualConfig;		// configuration settings, modified 
	char* ptInputFileName;		// pointer to input file name, persistant

	CAnnotationData* xD;										// pointer for actual arguments
	CAnnotationData* actualTag[MAX_TAG_LEVEL][kn_ANGROUP];	// actual tag, where attributes are added

	int tagLevel;											// points to actual tagLevel (global or pre or post defined doxygen comment)
	int tagLevelPre;										// 0=post defined, 1= global, >=2= pre-defined for hirarchical attributes (nested structs)
	int identLevel;											// for hirarchical Modules
	// global start points
	CModuleList* moduleList;								// List of all modules
	CIdentList* identList[MAX_IDENT_LIST_LEVEL];			// List of all parameters not within modules
	CModuleList** moduleScope;
	CModuleList** moduleScopeNULL;
	CModuleList* actualModule;


	CAnnotationData* mergedTags[MAX_TAG_LEVEL][kn_ANGROUP];	    // actual tag, where attributes are added
	CAnnotationData* configAction[kn_ANGROUP][kn_ANGROUP];		// action tags from config file (each group can contain actions of each group)
    CAnnotationData* cmdlineAction[kn_ANGROUP];                 // global action from command line
    bool stateIsBitfield;                                       // actual annotation is a bitfield and goes to separate annotation group

	int mergedTagCnt;

	bool insertInitialze;
	bool warnPackedData;
    bool checkEnumArgsElement;                                         // check if arguments of type ANARG_ENUM_ELEMENT points to a defined enum value
    bool checkEnumArgsType;                                         // check if arguments of type ANARG_ENUM_ELEMENT points to a defined enum value
	CStr insertInitialzeStr;
    CStringList* cmdTypeList;                                   // all config types, from command line
    CStringList* configTypeList;                                // all config types from config file
    bool configTypeFound;                                       // any config from cmd line matches type from config file

	CXMLTag *xmlRoot;

    CIntList* tagLevelList;                                     // stack for taglevels
    CStr selectNameSpace;

	// methods
	void dbgPrintf(eMsgTypes msgType, char* format, ...);

};

extern CGlobalData* gd;											// gloabl pointer to global data


// functions
void dbgPrintf(eMsgTypes type, char* format, ...);
void dbgPrintf(char* format, ...);
int getTrailingZeros(long64 v);
int getLeadingZeros(ulong64 v);
unsigned long long getBitSlice(unsigned char* dataIn, int bitAddress, int bitSize);



void copyAnPt(CAnnotationData **copyfrom, CAnnotationData **copyto);
void pushAnPt(CAnnotationData **copyfrom, int mergedTagCnt);
CAnnotationData** popAnPt(int mergedTagCnt);
CAnnotationData* addAnFunction(CAnnotationData* &addTo, eAnFunction function);
void printAnTags(CAnnotationData* mergedTags[kn_ANGROUP], char* label = NULL);
void printAnTags(CAnnotationData* mergedTags[][kn_ANGROUP], int mergedTagCnt, char* label = NULL);
bool checkGenerateOutput(CAnnotationData* anData[][kn_ANGROUP], int cnt, eAnGroups group);
long64 getAnnotationDataLong(CAnnotationData* anData[][kn_ANGROUP], int cnt, eAnGroups group, eAnFunction function, long64 deflt);
CStr* getAnnotationDataLabel(CAnnotationData* anData[][kn_ANGROUP], int cnt, eAnGroups group, eAnFunction function);
CAnnotationData* findLastAnnotationData(CAnnotationData* anData[][kn_ANGROUP], int cnt, eAnGroups group, eAnFunction function, long* countFind = NULL, long cntMin = 0, bool skipInternal = false);

/* Find last annotation data with <function> and copy this annotation to top of stack. set new value */
CAnnotationData* addInternalData(CAnnotationData* anData[][kn_ANGROUP], int cnt, eAnGroups group, eAnFunction function, char* string, long data, CValue* ptValue = NULL, bool copyOnly=false);

/* Find last annotation data with <function> and copy this annotation to top of stack. set new value. Mark the new annotaion as "internal local, i.e. it won't be copied further 
* this is needed to do local modifications without this this change propagating further down 
*/
CAnnotationData* addInternalDataLocal(CAnnotationData* anData[][kn_ANGROUP], int cnt, eAnGroups group, eAnFunction function, char* string, long data, CValue* ptValue = NULL, bool copyOnly = false);
CAnnotationData* copyInternalData(CAnnotationData* anData[][kn_ANGROUP], int cnt, eAnGroups group, eAnFunction function);
CAnnotationData* copyInternalDataLocal(CAnnotationData* anData[][kn_ANGROUP], int cnt, eAnGroups group, eAnFunction function);

#endif //_H2XML_COMMON_H__INCLUDED_

