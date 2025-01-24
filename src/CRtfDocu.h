/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

/**
*	\file
*/

#ifndef _CRTFDOCU_H__INCLUDED_
#define _CRTFDOCU_H__INCLUDED_

#include "h2xml_common.h"
#include "CList.h"

typedef enum {
	RTF_HEADER,
	RTF_MODULE,
	RTF_PARAM,
	RTF_ELEMENT,
	RTF_FOOTER,
	knRtfParts
} eRtfElements;

class CXMLTag;
class CRtfDocu;

class CStringPart :public CList<CStringPart>{
public:
	CStringPart();
	~CStringPart();

	// data
	CStr m_string;
	CStr m_keyWord;
	int m_printType;
	CRtfDocu* m_parent;
	void emit(FILE* fp, CXMLTag* xml);
};

class CStringPartList {
public:
	CStringPartList();
	~CStringPartList();
	CStringPart* m_list;
	CStr m_keyword;
	CRtfDocu* m_parent;

	void emit(FILE* fp, CXMLTag* xml);
	void part(char* string);
};


class CRtfDocu {
public:
	CRtfDocu();
	~CRtfDocu();

	CStringPartList m_part[knRtfParts];

	int parseRef(CStr* filename);
	char* cutCheck(char *str, char *token, bool findmode=false, bool dir = false); //! cut str at token, return pointer to string after token, findmode: if true: normal find, else rtf find; if dir==true: search from end
	static char* rtfFind(char *str, char *token, char* &lastChar, bool findHere=false);
	static char* rtfGetString(char *str, CStr* outStr);
	int emit(CStr* filename, CXMLTag* xml);
};

#endif //_CRTFDOCU_H__INCLUDED_

