/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

#include "h2xml_common.h"
#include "CValue.h"
#include "CAnnotationData.h"
#include "CConfigAttributes.h"
#include "CIdentList.h"
#include "CModuleList.h"
#include "CXmlTag.h"
#include "scanConfig.h"
#include "scanAnnotations.h"


extern	int yyannot_flex_debug;


CGlobalData::CGlobalData()
{
	config = new(CConfig);
	actualConfig = new(CConfig);
	verbose = false;
	xD = NULL;
	tagLevelPre = 2;
	tagLevel = 0;
	identLevel = 0;
	insertInitialze = false;
	warningcount=0;
	errorcount=0;
	linecnt = 0;

	moduleList = NULL;							// List of all modules
	moduleScopeNULL = new(CModuleList*);
	*moduleScopeNULL = NULL;
	moduleScope = moduleScopeNULL;
    actualModule = *moduleScopeNULL;
	warnPackedData = false;
    stateIsBitfield = false;
    tagLevelList = NULL;
    cmdTypeList = NULL;
    configTypeList=NULL;                               
    configTypeFound = false;
    checkEnumArgsElement = true;
    macroLevel = 0;
    selectNameSpace = "";
}

void CGlobalData::init()
{
	mergedTagCnt = 0;
	for (int i = 0; i < MAX_TAG_LEVEL; i++){
		for (int j = ANGROUP_NONE; j < kn_ANGROUP; j++) {
			actualTag[i][j] = NULL;
			mergedTags[i][j] = NULL;
		}
	}
	// initialize global tag settings
	for (int tagType = ANGROUP_NONE; tagType < kn_ANGROUP; tagType++){
		actualTag[TAGLEVEL_GLOBAL][tagType] = new(CAnnotationData);
		actualTag[TAGLEVEL_GLOBAL][tagType]->m_anGroup = (eAnGroups)tagType;
		actualTag[TAGLEVEL_GLOBAL][tagType]->m_anName = "";
	}

	for (int i = 0; i < kn_ANGROUP; i++){
		for (int j = 0; j < kn_ANGROUP; j++) {
            configAction[i][j] = NULL;
		}
	}

    for (int j = 0; j < kn_ANGROUP; j++) {
        cmdlineAction[j] = NULL;
    }

	for (int i = 0; i < MAX_IDENT_LIST_LEVEL; i++){
		identList[i] = NULL;
	}

}
CGlobalData::~CGlobalData()
{

}

void CGlobalData::dbgPrintf(eMsgTypes msgType, char* format, ...) {
	char tmp[4000];
	char tmp2[4000];
	va_list ap;
	va_start(ap, format);

	// adjust for Linux, so references for Linux and Win are equal
	if (ptInputFileName){
		CStr fileName = ptInputFileName;
		fileName.ReplaceAll("\\", "/");
		sprintf(tmp, "%s(%d): %s", fileName.buf, linecnt, format);
		vsprintf_s(tmp2, 3999, tmp, ap);
		::dbgPrintf(msgType, tmp2);
	}
	else {
		vsprintf_s(tmp2, 3999, format, ap);
		::dbgPrintf(msgType, tmp2);
	}
}

void dbgPrintf(char* format, ...)
{
	char str1[10000];
	va_list ap;
	va_start(ap, format);
	vsnprintf_s(str1, 9999, format, ap);
	dbgPrintf(TMSG, str1);
}

void dbgPrintf(eMsgTypes type, char* format, ...)
{
	static char typesStr[][20] = { "","", "Info: ", "Warning: ", "Error: " };
	static char str1[1000001];
	static char str2[1000001];
	va_list ap;
	va_start(ap, format);
	vsnprintf_s(str1, 1000000, format, ap);
	_snprintf_s(str2, 1000000, "%s%s", typesStr[type], str1);
	if (type==TMSGALL || type >= TWARNING) {
		printf("%s", str2);
	}

    if (yyannot_flex_debug && stderr && as.verbose) {
		fprintf(stderr, "%s", str2);
		fflush(stderr);
        fflush(as.fpStderr);
	}

	if (type == TWARNING) {
		gd->warningcount++;
	}
	if (type == TERROR) {
		gd->errorcount++;
	}

}

CStringList::CStringList()
:CList<CStringList>()
{
	m_string = "";
}

// return true, if findString in list
bool CStringList::findInList(CStringList* list, char* findString)
{
	CStringList* ll = list;
	bool labelFound = false;
	while (ll){
		if (strcasecmp(ll->m_string.buf, findString) == 0){
			labelFound = true;
			break;
		}
		ll = ll->get(lNEXT);
	}
	return labelFound;
}

// return true, if findString in list
bool CStringList::removeFromList(CStringList* list, char* findString)
{
    CStringList* ll = list;
    bool labelFound = false;
    while (ll){
        if (strcasecmp(ll->m_string.buf, findString) == 0){
            labelFound = true;
            deleteFromList(lNEXT, ll);
            break;
        }
        ll = ll->get(lNEXT);
    }
    return labelFound;
}

void CStringList::appendStringList(CStringList* &list1, CStringList* list2)
{
    CStringList* llfrom = list2;
    CStringList** llto = &list1;

    while (llfrom){
        CStringList* list = addToEnd(lNEXT, *llto);
        list->m_string = llfrom->m_string;
        llfrom = llfrom->get(lNEXT);
    }
}
void CStringList::deleteStringList(CStringList* &list1)
{
    deleteList(lNEXT, list1);
}

// print all strings in list, ',' separated
void CStringList::printList(CStr* outList)
{
    CStringList* ll = this;
    bool first = true;
    while (ll) {
        if (!first) {
            outList->stradd(", ");
        }
        first = false;
        outList->stradd(ll->m_string.buf);
        ll = ll->get(lNEXT);
    }
}


CIntList::CIntList()
:CList<CIntList>()
{
    m_value =0;
}

CDefaultList::CDefaultList()
{
	m_defaultBuffer = NULL;
	m_defaultBufferBitSize = 0;
	m_defaultBufferSize = 0;
	m_startAddress = 0;
    m_type = DEFAULT_TYPE_UNDEFINED;
    m_format = ANARG_UNSIGNED_INTEGER;
}

CDefaultList::~CDefaultList()
{
	if (m_defaultBuffer) {
		free(m_defaultBuffer);
	}
	m_defaultBuffer = NULL;
    m_defaultBufferBitSize = 0;
	m_defaultBufferSize = 0;
	m_startAddress = 0;
}

void CDefaultList::copy(CDefaultList* source)
{
    m_defaultBufferBitSize = source->m_defaultBufferBitSize;
	m_defaultBufferSize = source->m_defaultBufferSize;
	m_startAddress = source->m_startAddress;
    m_anData = source->m_anData;
    m_type = source->m_type;

	if (source->m_defaultBuffer) {
		m_defaultBuffer = (unsigned char*)malloc(m_defaultBufferSize);
		memmove(m_defaultBuffer, source->m_defaultBuffer, m_defaultBufferSize);
	}
	else {
		m_defaultBuffer = NULL;
	}

}


void copyAnPt(CAnnotationData **copyfrom, CAnnotationData **copyto)
{
	if (copyfrom) {
		for (int i = 0; i < kn_ANGROUP; i++){
			copyto[i] = copyfrom[i];
		}
	}
	else {
		for (int i = 0; i < kn_ANGROUP; i++){
			copyto[i] = NULL;
		}
	}
}


void pushAnPt(CAnnotationData **copyfrom, int mergedTagCnt)
{
	if (mergedTagCnt < MAX_TAG_LEVEL) {
		copyAnPt(copyfrom, gd->mergedTags[mergedTagCnt]);
	}
	else {
		dbgPrintf(TERROR, "Maximum taglevel reached\n");
	}


}
CAnnotationData ** popAnPt(int mergedTagCnt)
{
	CAnnotationData **retval = NULL;
	if (gd->mergedTagCnt >0) {
		gd->mergedTagCnt--;

		retval = gd->mergedTags[mergedTagCnt];
	}
	else {
		dbgPrintf(TERROR, "Maximum taglevel reached\n");
		retval = gd->mergedTags[0];
	}
	return retval;
}

CAnnotationData* addAnFunction(CAnnotationData* &addTo, eAnFunction function)
{
    CAnnotationData *newAnData = addToEnd(lNEXT, addTo);
    newAnData->m_anFunction = function;
    return newAnData;
}

// for debugging
void printAnTags(CAnnotationData* mergedTags[kn_ANGROUP], char* label)
{
    if (label) {
        printf("***************** %s *******************\n", label);
    }
    else {
        printf("************************************\n");
    }
    for (int k = 0; k < kn_ANGROUP; k++) {
        CAnnotationData* tmp = mergedTags[k];
        printf("AnGroup=%s: ", anGroupNames[k]);
        while (tmp) {
            if (tmp->m_keywordName.buf) {
                printf("%s; ", tmp->m_keywordName.buf);
            }
            tmp = tmp->get(lNEXT);
        }
        printf("\n");
    }
};

// for debugging
void printAnTags(CAnnotationData* mergedTags[][kn_ANGROUP], int mergedTagCnt, char* label)
{
    if (label) {
        printf("***************** %s *******************\n", label);
    }
    else {
        printf("************************************\n");
    }
    for (int i = 0; i <= mergedTagCnt; i++) {
        printf("<<<<Tagcount=%d\n", i);
        for (int k = 0; k < kn_ANGROUP; k++) {
            CAnnotationData* tmp = mergedTags[i][k];
            printf("AnGroup=%s: ", anGroupNames[k]);
            while (tmp) {
                if (tmp->m_keywordName.buf) {
                    printf("%s; ", tmp->m_keywordName.buf);
                }
                tmp = tmp->get(lNEXT);
            }
            printf("\n");
        }
    }
};

bool checkGenerateOutput(CAnnotationData* anData[][kn_ANGROUP], int cnt, eAnGroups group)
{
	bool retval = false;
	CAnnotationData* an = findLastAnnotationData(anData, cnt, group, ANFUNCTION_GENERATE_OUTPUT);
	if (an && an->m_assignmentList->m_value1.l != 0) {
		retval = true;
	}
	return retval;
}

long64 getAnnotationDataLong(CAnnotationData* anData[][kn_ANGROUP], int cnt, eAnGroups group, eAnFunction function, long64 deflt)
{
	long64 retval = deflt;
	CAnnotationData* an = findLastAnnotationData(anData, cnt, group, function);
	if (an) {
		retval = an->m_assignmentList->m_value1.l;
	}
	return retval;
}

CStr* getAnnotationDataLabel(CAnnotationData* anData[][kn_ANGROUP], int cnt, eAnGroups group, eAnFunction function)
{
	CStr* retval = NULL;
	CAnnotationData* an = findLastAnnotationData(anData, cnt, group, function);
	if (an) {
		retval = &an->m_assignmentList->m_string;
	}
	return retval;
}


// search all structures from back, return last found
CAnnotationData* findLastAnnotationData(CAnnotationData* anData[][kn_ANGROUP], int cnt, eAnGroups group, eAnFunction function, long* countFind, long cntMin, bool skipInternal)
{
	CAnnotationData::m_countFind = 0;
    CAnnotationData::m_countFindLevel = 0;
	CAnnotationData* retval = NULL;
	for (int i = cnt; i >= 0; i--) {
		retval = anData[i][group]->findLastAnnotation(function, cntMin, skipInternal);
        if (retval  ||  CAnnotationData::m_breakFound) {
            CAnnotationData::m_countFindLevel = i;
			break;
		}
	}
	if (countFind) {
		*countFind = CAnnotationData::m_countFind; // number of elements parsed until found
	}
	return retval;
}

CAnnotationData* copyInternalDataLocal(CAnnotationData* anData[][kn_ANGROUP], int cnt, eAnGroups group, eAnFunction function)
{
    return addInternalDataLocal(anData, cnt, group, function, NULL, 0, NULL, true);
}

CAnnotationData* copyInternalData(CAnnotationData* anData[][kn_ANGROUP], int cnt, eAnGroups group, eAnFunction function)
{
    return addInternalData(anData, cnt, group, function, NULL, 0, NULL,true);
}

CAnnotationData* addInternalDataLocal(CAnnotationData* anData[][kn_ANGROUP], int cnt, eAnGroups group, eAnFunction function, char* string, long data, CValue* ptValue, bool copyOnly)
{
    CAnnotationData* newAnData = NULL;
    newAnData=addInternalData(anData, cnt, group, function, string, data, ptValue, copyOnly);
    if (newAnData) {
        newAnData->m_isInternalcopy = true;
    }
    return newAnData;
}

CAnnotationData* addInternalData(CAnnotationData* anData[][kn_ANGROUP], int cnt, eAnGroups group, eAnFunction function, char* string, long data, CValue* ptValue, bool copyOnly)
{
    static int createCnt = 0;
    createCnt++;
    if (createCnt == 5) {
        //int dbg = 0;
    }
	CAnnotationData* oldAnData;
	CAnnotationData* newAnData = NULL;
	oldAnData = findLastAnnotationData(anData, cnt, group, function, NULL, 0, true);
	if (oldAnData){
		newAnData = addToEnd(lNEXT, anData[cnt][group]);
		newAnData->copyData(oldAnData);
        if (!copyOnly && oldAnData->m_isDefault) {
            newAnData->m_assignmentList = new(CAssignmentList);
            if (string) {
                newAnData->setLabel(string);
            }
            else {
                newAnData->setIntValue(data);
				if (oldAnData->m_assignmentList->m_argType == ANARG_BOOL) {
					newAnData->m_assignmentList->m_argType = ANARG_BOOL;
					newAnData->m_assignmentList->m_value1.format = ANARG_BOOL;

				}
                if (ptValue) {
                    newAnData->m_assignmentList->m_value1.format = ptValue->format;
                    newAnData->m_assignmentList->m_value1.l = ptValue->l;
                    newAnData->m_assignmentList->m_value1.m_bitSize = ptValue->m_bitSize;
                }
            }
        }
        newAnData->m_createCnt = createCnt;
	}
	else {
		newAnData = oldAnData;
	}
	return newAnData;
}

int getTrailingZeros(long64 v)
{
	int r;													// the result goes here
	float f = (float)((ulong64)v & -v);				        // cast the least significant bit in v to a float
	unsigned int i;
	memmove(&i, &f, sizeof(float));							// strict aliasing rule in c++: use memmove
	r = (i >> 23) - 0x7f;
	if (v == 0) r = 64;
	return r;
}

int getLeadingZeros(ulong64 v)
{
	if (v == 0) return 64;
    double a = (double)v;
    ulong64* n = (ulong64*)&a;
    long z = (*n >> (64 - 12)) - 1023;
    ulong64 mask = ((ulong64) 1) << z;
    if (!(mask & v)) {
        z -= 1;
    }
    return 63 - z;
}

unsigned long long getBitSlice(unsigned char* dataIn, int bitAddress, int bitSize)
{
    unsigned long baseAddr = bitAddress / 8; // byte address, aligned to 8 bit
    unsigned long64 data = 0;
    unsigned byteCount = (bitSize + 7) / 8;
    if (byteCount > 8) {
        byteCount = 8;
    }
    unsigned bitoffset = bitAddress % 8;
    for (unsigned i = 0; i <= byteCount; i++){
        int shift = (i * 8 - bitoffset);
        if (shift >= 0 && shift < 64) {
            data += ((long64)dataIn[baseAddr + i]) << shift;
        }
        else if (shift < 0) {
            data += ((long64)dataIn[baseAddr + i]) >> (-shift);

        }
    }
    unsigned long64 mask = 0xffffffffffffffff >> (64 - bitSize);
    data = data & mask;

    return data;

}
