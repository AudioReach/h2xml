/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

#ifndef _CANNOTATION_DATA_H__INCLUDED_
#define _CANNOTATION_DATA_H__INCLUDED_

#include "h2xml_common.h"
#include "CValue.h"
#include "CConfigAttributes.h"

#define MAX_XML_STRSIZE 10000



class CAssignmentList : public CList<CAssignmentList>
{
public:
	CAssignmentList();
	eAnArgTypes m_argType;								// String, Integer , assignment, ..
	CStr m_string;
	CValue m_value1;
    void* m_pointer;
	//	bool m_hasString;
	char* getValueString(CStr* buffer, bool addQuotes=false);
    bool resolveReferences(CAnnotationData* anData=NULL);
    void emitAttribute(CStr* str, int tab, int lineLength, int attributeLength, CAnnotationData* an);

	CAssignmentList* copy();
};

class CXMLConfigData;
class CAnArgTypeList;
class CAnnotationData :public CList<CAnnotationData> {
public:
	//CAnnotationData();
	CAnnotationData(eAnGroups m_anGroup = ANGROUP_NONE);
	~CAnnotationData();
	eAnGroups m_anGroup;												// Module, Parameter, Element
	unsigned long m_anFunction;								            // unique identifier, either address or type eAnFunction
	eAnType m_anType;													// attribute, tag, ...
    eAnArgFormat m_argFormat;                                           // special restriction, like odd/even etc

	CStr m_anName;														// name of the attrtribute
	CStr m_keywordName;													// name of the keyword
    CStr m_prefix;
	CStr m_parent;
    CStr m_enumName;                                                    // name of the enum before it is resolved
    CStr m_selectNameSpace;                                             // complete instance name (including structs etc.) if identifier has been selected via @h2xml_select
	CAnArgTypeList *m_anArgTypeList;
    CAnArgTypeList *m_anArgTypeListList;
	CAssignmentList *m_assignmentList;
	bool m_showInOutput;
	bool m_removeQuotes;
	bool m_isList;
	bool m_newParent;
	bool m_isDefault;													// element contains the default value
	int m_knArguments;
    int m_knArgumentsList;
	int m_knArgumentsIn;

	CStringList* m_labelList;
	CStringList* m_showInTag;
    CArgIntRef m_min;
    CArgIntRef m_max;
	bool m_min_set;
	bool m_max_set;
    bool m_m1EqualsInfinity;
    bool m_accessSpecPrivate;                                                     // may only be accessed from macro
	int m_tagMaxOccurs;													// maximum occurence of a tag, "unbounded"=0
	bool m_tagSelectExisting;
	bool m_removeRedundantTags;
    bool m_mandatory;                                                   // tag is not allowed to stay at default value
    bool m_isInternalcopy;                                              /* is an internal copy of the original data, may be modified */
    int m_createCnt;                                                    /* counter for debugging */
	// Scope for global parameter annotations defined within a specific module
	CModuleList **m_moduleScope;

	// for error messages
	char* m_fileName;
	long m_linecnt;


	void clear(eAnGroups anGroup = ANGROUP_NONE);
    CAnnotationData* emitAll(FILE* fp, int tab, int lineLength, int attributeLength, bool noChecks=false);
	void emitAttribute(FILE* fp, int tab, int lineLength, int attributeLength);

	static CAnnotationData* addElementToEnd(CAnnotationData* &start, char* name, bool alwayAdd);
	static CAnnotationData* addElementToStart(CAnnotationData* &start, char* name);
	static CAnnotationData* addElement(CAnnotationData* &start, CConfigAttributes* confAttr);

	static long m_countFind;
    static long m_breakFound;
    static long m_countFindLevel;

	CAnnotationData* copyData(CAnnotationData* from);
    static CAnnotationData* copyList(CAnnotationData* from);
    static void printList(CAnnotationData* from); // for debugging
    CAnnotationData* initData(CConfigAttributes* from);

	CAnnotationData* findFirstAnnotation(CAnnotationData* an, long cntMin=0);
	CAnnotationData* findFirstAnnotationFunction(eAnFunction function, long cntMin);
	CAnnotationData* findFirstAnnotationFunctionList(eAnFunction function[], long knFunctions, long cntMin);

	CAnnotationData* findFirstKeyWord(char* keyWord);
	CAnnotationData* findFirstName(char* name);
	CAnnotationData* findLastAnnotation(eAnFunction function, long cntMin = 0, bool skipInternal = false);
    CAnnotationData* findLastAnnotationKeyword(char* keyWord, long cntMin = 0);
    void signExtendtoElementtype(bool isUnsigned, long identTypeId, long bitsize);

	//	bool checkGenerateOutput();
    CAssignmentList* insertValues(eAnArgTypes argType, char* text, long64 valueL, double valuef, eAnArgDPLYType dlyType = ANARG_DPLY_NORMAL, bool add=false);			// called from lex
    CAssignmentList* addValueToEnd();
    CAssignmentList* addValue(CAssignmentList* insertAfter);
    CAssignmentList* removeValue(CAssignmentList* &elementToRemove);
	void setLabel(char* label);
	void setIntValue(long value);
	void setUIntValue(unsigned long value);

    bool getArgListString(CStr *str1, CStr *str2, CStr *str3);
	bool checkArgList();
	int checkSingleArg(eAnArgTypes arg1, eAnArgTypes arg2);
	void errorPrnt(eMsgTypes msgType, char* format, ...);
    bool checkRange(CAnnotationData* ptAnMin, CAnnotationData* ptAnMax, CAssignmentList* ptRangeList);
};

#endif //_CANNOTATION_DATA_H__INCLUDED_

