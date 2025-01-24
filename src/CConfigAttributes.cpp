/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

#include "scanConfig.h"
#include "CConfigAttributes.h"


CArgIntRef::CArgIntRef()
{
    clear();
}

void CArgIntRef::clear()
{
    m_type = ARG_INTREF_TYPE_INT;
    m_value = 0;
    m_reference = "";
}

CConfigAttributes::CConfigAttributes()
:m_min(), m_max()
{
	m_keyword = "";
	m_prefix = "";
	m_anName = "";
	m_anArgTypeList = NULL;
    m_anArgTypeListList = NULL;
	m_anGroup = ANGROUP_NONE;
	m_min_set = false;
	m_max_set = false;
    m_m1EqualsInfinity = false;
    m_accessSpecPrivate = false;
	m_labelList = NULL;
	m_showInTag = NULL;
	m_default_set = false;
	m_default = "";
	m_action = "";
	m_actionList = "";
	m_actionPost = "";
	m_showInOutput = true;
	m_removeQuotes = false;
	m_anFunction = ANFUNCTION_GENERAL;
	m_isList=false;
	m_parent = "";
	m_newParent = false;
	m_anType = ANTYPE_UNDEFIEND;
    m_argFormat = ANARGFORAMT_NORMAL;
	m_knArguments = 0;
    m_knArgumentsList = 0;
	m_addToFront = 0;
	m_tagMaxOccurs = 0;
	m_tagSelectExisting = false;
	m_removeRedundantTags = false;
    m_mandatory = false;
	m_globalAnnotation=0;								// Annotation does not refer to an module, element or struct
}

void CConfigAttributes::emit()
{
	dbgPrintf("\n");
	dbgPrintf("m_keyword	= %s\n", m_keyword.buf);
	dbgPrintf("m_prefix 	= %s\n", m_prefix.buf);
	dbgPrintf("m_anName		= %s\n", m_anName.buf);
	dbgPrintf("m_anGroup	= %s\n", anGroupNames[m_anGroup]);
	dbgPrintf("HashEntry	= %s\n", hash.name.buf);

	if (m_min_set) {
		dbgPrintf("m_min		= %d\n", m_min.m_value);
	}
	if (m_max_set) {
		dbgPrintf("m_max		= %d\n", m_max.m_value);
	}
	if (m_default_set) {
		dbgPrintf("m_default	= %s\n", m_default.buf);
	}
	dbgPrintf("m_isList   	= %d\n", m_isList);

	CAnArgTypeList *argList = m_anArgTypeList;
	while (argList) {
		dbgPrintf("\tArgument Type = %s\n", anArgNames[argList->m_argType]);
		argList = argList->get(lNEXT);
	}
    argList = m_anArgTypeListList;
    while (argList) {
        dbgPrintf("\tList Argument Type = %s\n", anArgNames[argList->m_argType]);
        argList = argList->get(lNEXT);
    }

	CStringList* list = m_labelList;
	while (list) {
		dbgPrintf("\tLabelListElement = %s\n", list->m_string.buf);
		list = list->get(lNEXT);
	}

	list = m_showInTag;
	while (list) {
		dbgPrintf("\tshowInTag = %s\n", list->m_string.buf);
		list = list->get(lNEXT);
	}
	
}

CConfigAttributes* CConfigAttributes::findKeyword(char* keyWord)
{
    return gd->config->m_htab.hashlookup(keyWord);
}

#if 0
// check if text is included in string list
bool CConfigAttributes::checkLabel(char* text)
{
	CStringList* list = m_labelList;
	if (list == NULL) {
		return true;
	}
	while (list) {
		if (strcmp(list->m_string.buf, text) == 0) {
			return true;
		};
		list = list->get(lNEXT);
	}
	gd->dbgPrintf(TERROR, "Keyword '%s', argument '%s' out of range\n",m_keyword.buf, text);
	return false;
}
#endif

