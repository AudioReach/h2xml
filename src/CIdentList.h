/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

/**
*	\file
*/

#ifndef _CIDENTLIST_H__INCLUDED_
#define _CIDENTLIST_H__INCLUDED_

#include "h2xml_common.h"

class CIdentifier;
class CIdentList : public CList<CIdentList>
{
public:
	CIdentList();
	~CIdentList();
	CIdentifier* m_ident;

	static CIdentList* addElementToEnd(CIdentList* &start, CIdentifier* ident);
	static void emitAll(CXMLTag* xmlTag, CIdentList* identList, int mergedTagCnt);					// all listelements
	void emit(CXMLTag* xmlTag, int mergedTagCnt);													// single elemnet
};


#endif //_CIDENTLIST_H__INCLUDED_

