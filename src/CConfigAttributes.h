/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

#ifndef __CCONFIG_ATTRIBUTES__H__INCLUDED_
#define __CCONFIG_ATTRIBUTES__H__INCLUDED_

#include "h2xml_common.h"
#include "hashtableEx.h"


class CAnArgTypeList : public CList<CAnArgTypeList>
{
public:
	CAnArgTypeList() { m_argType = ANARG_NONE; }
	eAnArgTypes m_argType;
};

typedef enum {
    ARG_INTREF_TYPE_NONE,
    ARG_INTREF_TYPE_INT,
    ARG_INTREF_TYPE_REF
}eArgIntRefType_t;

class CArgIntRef
{
public:
    CArgIntRef();
    eArgIntRefType_t m_type;
    int m_value;
    CStr m_reference;
    void clear();
};

class CConfigAttributes : public CHashElement<CConfigAttributes>
{
public:
	CConfigAttributes();
	~CConfigAttributes();
	CAnArgTypeList *m_anArgTypeList;						// list of argument types
    CAnArgTypeList *m_anArgTypeListList;					// list of list argument types
	eAnGroups m_anGroup;
	unsigned long m_anFunction;								// unique identifier, either address or type eAnFunction
	eAnType m_anType;										// attribute, tag, ...
    eAnArgFormat m_argFormat;                               // special restriction, like odd/even etc
	CStr m_prefix;
	CStr m_keyword;
	CStr m_anName;
	CStr m_default;
	CStr m_action;										    // Action string
	CStr m_actionList;									    // 
	CStr m_actionPost;
	CStr m_parent;
    CArgIntRef m_min;
    CArgIntRef m_max;
	int m_knArguments;
    int m_knArgumentsList;
	int m_tagMaxOccurs;										// maximum occurence of a tag, "unbounded"=0
	bool m_tagSelectExisting;								// if true, select tag if exists, otherwise replace
	bool m_removeRedundantTags;
    bool m_mandatory;                                       // tag is not allowed to stay at default value
	bool m_default_set;
	bool m_min_set;
	bool m_max_set;
    bool m_m1EqualsInfinity;                                // data type range -1 (=Infinity) ... MAX_INT
    bool m_accessSpecPrivate;                                         // may only be accessed from macro
	bool m_showInOutput;
	bool m_removeQuotes;
	bool m_isList;
	bool m_newParent;
	bool m_addToFront;
	bool m_globalAnnotation;								// Annotation does not refer to an module, element or struct


	CStringList* m_labelList;
	CStringList* m_showInTag;

	// methods
	//bool checkLabel(char* text);
	void emit();
    static CConfigAttributes* findKeyword(char* keyWord);
};


#endif // __CCONFIG_ATTRIBUTES__H__INCLUDED_