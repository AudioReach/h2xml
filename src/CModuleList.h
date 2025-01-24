/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

/**
*	\file
*/

#ifndef _CMODULELIST_H__INCLUDED_
#define _CMODULELIST_H__INCLUDED_

#include "h2xml_common.h"


class CModuleList : public CList<CModuleList>
{
public:
	CModuleList();
	~CModuleList();
	CIdentList* m_identList;
	CAnnotationData* m_xD[kn_ANGROUP];

	static CModuleList* addElementToEnd(CModuleList* &start, char*name);
	static void emitAll(CXMLTag* xmlTag, CModuleList* modList, int mergedTagCnt);					// all listelements
	void emit(CXMLTag* xmlTag, int mergedTagCnt);													// single elemnet
};


#endif // _CMODULELIST_H__INCLUDED_

