/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

#include "CModuleList.h"
#include "CIdentList.h"
#include "CXmlTag.h"
#include "CAnnotationData.h"


//////////////////////////////////////////////////////////////////////////////
// Class ModuleList
//////////////////////////////////////////////////////////////////////////////
CModuleList::CModuleList()
{
	m_identList = NULL;
	for (int i = 0; i < kn_ANGROUP; i++){
		m_xD[i] = NULL;
	}
};

CModuleList::~CModuleList()
{
}

CModuleList* CModuleList::addElementToEnd(CModuleList* &start, char* name) {
	CModuleList* list = start;
	CModuleList* found = NULL;
	while (list) {
		if (list->m_xD[ANGROUP_MODULE] && strcmp(list->m_xD[ANGROUP_MODULE]->m_keywordName.buf, name) == 0) {
			found = list;
		}
		list = list->get(lNEXT);
	}

	if (!found) {
		found = addToEnd(lNEXT, start);
	}
	return found;
}

// all listelements
void CModuleList::emitAll(CXMLTag* xmlTag, CModuleList* modList, int mergedTagCnt)
{
	while (modList) {
		gd->actualModule = modList;
		modList->emit(xmlTag, mergedTagCnt);
		modList = modList->get(lNEXT);
	}
}

// one listelement
void CModuleList::emit(CXMLTag* xmlTag, int mergedTagCnt)
{
	if (m_xD[ANGROUP_MODULE]){
        xmlTag->saveActiveTag(NULL, TAG_SOURCE_MODULE);

        pushAnPt(gd->configAction[ANGROUP_MODULE], ++mergedTagCnt);
		pushAnPt(m_xD, ++mergedTagCnt);

		CXMLTag* newElement = xmlTag->addAttribute(gd->mergedTags, mergedTagCnt, ANGROUP_MODULE);
		CIdentList::emitAll(newElement, m_identList, mergedTagCnt);
        xmlTag->restoreActiveTag(NULL, TAG_SOURCE_MODULE);

	}
}


