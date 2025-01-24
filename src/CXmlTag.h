/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

/**
*	\file 
*/

#include "h2xml_common.h"
#include "CIdentifier.h"
#include "CList.h"


#ifndef __CXMLTAG_H__INCLUDED_
#define __CXMLTAG_H__INCLUDED_


#define DEFAULT_MAXLINELENGTH       120  
#define DEFAULT_TABSIZE             4  

#define knMAXTAGLIST 2000

typedef enum {
    TAG_SOURCE_CNTRL,
    TAG_SOURCE_MODULE,
    TAG_SOURCE_STRUCT,
    TAG_SOURCE_ELEMENT,
    TAG_SOURCE_EMITDATA,

    knTAgSource
}eActiveTagSource_t;

extern char tagSourceName[knTAgSource][20];

class CXMLTag :public CList<CXMLTag> {
public:
	CXMLTag();
	CXMLTag* m_sub;
	CAnnotationData* m_attributes;
	CStr m_tagName;
	CStr m_value;
	CStr m_keyWord;
    CXMLTag *m_tagList[knMAXTAGLIST];
    int m_tagListcnt;
    CXMLTag *m_actual;


	// Display properties
	int m_tabSize;
	int m_lineLength;
	int m_attributeLength;
	bool m_removeRedundantTags;
    bool m_mandatory;

	// methods
	void emit(FILE* fp, int tab, bool printKeyword=false); // printKeyword: emit name and keyword, for debugging only
	void removeRedundantTags();
	bool compareAttributes(CXMLTag* list2);
	CXMLTag* addTag(char*name, char* keyWord=NULL);
	CXMLTag* addSubTag(char*name, bool once, bool SelectExisting, char* keyWord = NULL, CStr* parent = NULL);
	CXMLTag* addAttribute(CAnnotationData* anData[][kn_ANGROUP], int cnt, eAnGroups group, char* nameSpace=NULL); // return actual XML
	CXMLTag* find(char* keyWord, int direction, bool findLast=true);
	CXMLTag* findParent(char* parentName, bool findLast=true, bool noWarning=false);
	CXMLTag* findParent(CStr * parentName, bool findLast = true, bool noWarning = false);
    void saveActiveTag(CAnnotationData* an, eActiveTagSource_t source);
    CXMLTag* restoreActiveTag(CAnnotationData* an, eActiveTagSource_t source);

	static void xmlPrintf(int tabs, int lineLength, int attributeLength, FILE* fp, char* format, ...);
	static void xmlLinePrintf(int tabs, int lineLength, int attributeLength, FILE* fp, char* line);
};

#endif //__CXMLTAG_H__INCLUDED_

