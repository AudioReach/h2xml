/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

#include "h2xml_common.h"
#include "CAnnotationData.h"
#include "CXmlTag.h"
#include "CConfigAttributes.h"
#include "h2xml_common.h"

enum eCheckArgRetval {
    ARG_OK,
    ARG_ERROR,
    ARG_RESOLVED_ENUM1,
    ARG_RESOLVED_ENUM2,
    ARG_ENUMTYPE
};

CAssignmentList::CAssignmentList()
: CList<CAssignmentList>()
{
	m_argType = ANARG_NONE;
	m_string = "";
    m_pointer = NULL;
};

CAssignmentList* CAssignmentList::copy()
{
    CAssignmentList* retval = new(CAssignmentList);
    retval->m_argType = m_argType;								// String, Integer , assignment, ..
    retval->m_string = m_string;
    retval->m_value1.copy(&m_value1);
    CAssignmentList* next = get(lNEXT);
    if (next){
        insertInList(lNEXT, retval, next->copy());
    }
	return retval;
}

char* CAssignmentList::getValueString(CStr* buffer, bool addQuotes)
{
	char buf1[1000];
	*buffer = "";
	switch (m_argType) {

	case ANARG_NONE:
	case ANARG_IGNORE:
		break;
	case ANARG_LABEL:
    case ANARG_ENUM_ELEMENT:
    case ANARG_ENUM_TYPE:
		if (addQuotes) {
			*buffer="\"";
			buffer->stradd(m_string.buf);
			buffer->stradd("\"");
		}
		else {
			*buffer = m_string.buf;
		}

		break;
	case ANARG_CONSTLABEL:
		if (addQuotes) {
			*buffer = "\"";
			buffer->stradd(m_string.buf);
			buffer->stradd("\"");
		}
		else {
			*buffer = m_string.buf;
		}
#ifdef CONST_LABELS_TO_CAPITAL
		buffer->toCapital();
#endif
		break;
	case ANARG_STRING:
		*buffer = m_string.buf;
		break;
	case ANARG_BOOL:
	case ANARG_INTEGER:
	case ANARG_UNSIGNED_INTEGER:
    case ANARG_FLOAT32:
	case ANARG_DOUBLE:
		*buffer = m_value1.getValuStr(buf1);
		break;
	default:
		break;
	}
	return buffer->buf;
}

bool CAssignmentList::resolveReferences(CAnnotationData* anData) {
    bool retval = false;
    if (m_argType == ANARG_REFERENCE) {
        if (!m_string.isEmpty()) {
            CStr reference = m_string;
            char* identName = strtok(reference.buf, "@");
            char* keyName = strtok(NULL, "@");
            CStr name = HASH_PREFIX;
            name = name + identName;
            name.ReplaceAll("::", ".");
            Cvdt* tmp = htab.hashlookup(name.buf);      // structure
            if (tmp == NULL) {
                tmp = htab.hashlookup(identName);       // enum, or referenced by enumName::element
            }
            if (tmp  && tmp->identifier) {

                // check for valid keyword
                CStr keyword = keyName;
                keyword.RemoveQuotes();
                CConfigAttributes* dscattr;
                dscattr = CConfigAttributes::findKeyword(keyword.buf);
                if (dscattr) {// keyword found
                    switch (dscattr->m_anFunction) {
                    case ANFUNCTION_NAME: {
                        char* name = tmp->identifier->getshortname(ANGROUP_ELEMENT);
                        m_string = name;
                        m_value1.dataStr = name;
                        m_argType = ANARG_LABEL;
                        retval = true;
                        break; }
                    case ANFUNCTION_ELEMENT_NAME: {
                        char* name = tmp->identifier->getshortname(ANGROUP_NONE);
                        m_string = name;
                        m_value1.dataStr = name;
                        m_argType = ANARG_LABEL;
                        retval = true;
                        break; }
                    case ANFUNCTION_INITVALUE: {
                        if (tmp->identifier->m_cvdt) {
                            m_value1.setLong((long64) tmp->identifier->m_cvdt->data, tmp->identifier->m_cvdt->valstr.buf, ANARG_UNSIGNED_INTEGER);
                            m_argType = ANARG_UNSIGNED_INTEGER;
                            retval = true;
                        }
                        break; }
                    default: {
                        CAnnotationData* ref = tmp->identifier->m_xD[dscattr->m_anGroup]->findLastAnnotationKeyword(keyName);
                        if (ref) {
                            anData->m_assignmentList = ref->m_assignmentList->copy();
                            anData->m_knArguments = ref->m_knArguments;
                            anData->m_knArgumentsList = ref->m_knArgumentsList;
                            anData->m_knArgumentsIn = ref->m_knArgumentsIn;
                            anData->m_anArgTypeList = ref->m_anArgTypeList;
                            anData->m_anArgTypeListList = ref->m_anArgTypeListList;
                            retval = true;
                        }
                        else {
                            if (anData) {
                                anData->m_showInOutput = false;
                            }
                        }

                        if (anData) {
                            //anData->errorPrnt(TERROR, "Keyword '%s'not supported for REPLACE operator\n", keyword.buf);
                        }
                    }
                    }
                }
                else {
                    if (anData) {
                        anData->errorPrnt(TERROR, "Keyword '%s'not found\n", keyword.buf);
                    }
                }
            }
            else {
                if (anData) {
                    anData->errorPrnt(TERROR, "Identifier '%s'not found\n", identName);
                }
            }
        }
    }
    return retval;
}

void CAssignmentList::emitAttribute(CStr* str, int tab, int lineLength, int attributeLength, CAnnotationData* an)
{  
    char buf1[1000];
    CStr label;
    switch (m_argType) {
        case ANARG_NONE:
        case ANARG_IGNORE:
        case ANARG_STRING: 
            break;
        case ANARG_LABEL:
        case ANARG_ENUM_ELEMENT:
        case ANARG_ENUM_TYPE:
            str->stradd(m_string.buf);
            break;
        case ANARG_CONSTLABEL:
            label = m_string.buf;
    #ifdef CONST_LABELS_TO_CAPITAL
            label.toCapital();
    #endif
            str->stradd(label.buf);
            break;
        case ANARG_BOOL:
        case ANARG_INTEGER:
        case ANARG_UNSIGNED_INTEGER:
        case ANARG_FLOAT32:
        case ANARG_DOUBLE: {
            char* valstr = m_value1.getValuStr(buf1);
            str->stradd(valstr);
            break; }
        default:
            str->stradd("unknown argument Type");
    }
}

CAnnotationData::CAnnotationData(eAnGroups tagType)
{
	m_assignmentList = new(CAssignmentList);
	clear(tagType);
}

CAnnotationData::~CAnnotationData()
{

}

void CAnnotationData::clear(eAnGroups tagType)
{
	m_anGroup = tagType;
	m_anName = "";
	m_keywordName = "";
	m_anFunction = ANFUNCTION_GENERAL;
	m_showInOutput = true;
	m_removeQuotes = false;
	m_isList = false;
	m_parent = "";
	m_newParent = false;
	m_anType = ANTYPE_UNDEFIEND;
    m_argFormat = ANARGFORAMT_NORMAL;
	m_knArguments = 0;
    m_knArgumentsList = 0;
	m_knArgumentsIn = 0;
	m_isDefault = false;
	m_anArgTypeList = NULL;
    m_anArgTypeListList = NULL;
	m_labelList = NULL;
	m_showInTag = NULL;
    m_min.clear();
    m_max.clear();
	m_min_set = false;
	m_max_set = false;
    m_m1EqualsInfinity = false;
    m_accessSpecPrivate = false;
	m_tagMaxOccurs = 0;
	m_tagSelectExisting = false;
	m_removeRedundantTags = false;
    m_mandatory=false;                                       // tag is not allowed to stay at default value
	m_fileName = NULL;
	m_linecnt = 0;
    m_enumName = "";
	m_moduleScope = gd->moduleScopeNULL;
    m_selectNameSpace = "";
    m_isInternalcopy = false;
    m_createCnt = 0;
}


CAnnotationData* CAnnotationData::emitAll(FILE* fp, int tab, int lineLength, int attributeLength, bool noChecks)
{
	CAnnotationData* retval = NULL;
	CAnnotationData* actualData = this;					// Parameter attributes
	while (actualData) {
		if (actualData->m_showInOutput){
            if (!noChecks && actualData->m_isDefault && actualData->m_mandatory) {
                actualData->errorPrnt(TERROR, "Keyword '%s': mandatory annotation not defined\n", actualData->m_keywordName.buf);
            }

			actualData->emitAttribute(fp, tab, lineLength, attributeLength);
		}
		actualData = actualData->get(lNEXT);
	}
	return retval;
}


void CAnnotationData::emitAttribute(FILE* fp, int tab, int lineLength, int attributeLength)
{
    if (strcmp(m_keywordName.buf, "h2xmle_default") == 0) {
        //int dbg = 0;
    }
    if (m_assignmentList->m_argType==ANARG_STRING) {
        char* str = m_assignmentList->m_string.buf;
        int len = strlen(str);
        if (len <= MAX_XML_STRSIZE) {
            CXMLTag::xmlPrintf(tab, lineLength, attributeLength, fp, " %s=\"%s\"", m_anName.buf, str);
        }
        else {
            CXMLTag::xmlPrintf(tab, lineLength, attributeLength, fp, " %s=\"", m_anName.buf);
            while (len > 0) {
                char tmpchr;
                if (len > MAX_XML_STRSIZE) {
                    tmpchr = str[MAX_XML_STRSIZE];
                    str[MAX_XML_STRSIZE] = 0;
                }
                CXMLTag::xmlPrintf(tab, lineLength, attributeLength, fp, "%s", str);
                if (len > MAX_XML_STRSIZE) {
                    str[MAX_XML_STRSIZE] = tmpchr;
                }
                len -= MAX_XML_STRSIZE;
                str += MAX_XML_STRSIZE;
            }
            CXMLTag::xmlPrintf(tab, lineLength, attributeLength, fp, "\"");
        }
    }
    else {
        char tmpbuf[1000];
        CStr attrStr="";
        sprintf_s(tmpbuf, 999, " %s=\"", m_anName.buf);
        attrStr.stradd(tmpbuf);

        CAssignmentList *assignmentList = m_assignmentList;
        while (assignmentList) {
            assignmentList->emitAttribute(&attrStr, tab, lineLength, attributeLength, this);
            assignmentList = assignmentList->get(lNEXT);
            if (assignmentList) {
                attrStr.stradd(", ");
            }
        }

        attrStr.stradd("\"");

        CXMLTag::xmlPrintf(tab, lineLength, attributeLength, fp, attrStr.buf);
    }
}

CAnnotationData* CAnnotationData::addElementToEnd(CAnnotationData* &start, char* name, bool alwayAdd) {
	CAnnotationData* found = NULL;
	if (!alwayAdd) {
		CAnnotationData* list = findLast(lNEXT, start);
		while (list) {												
			if (strcmp(list->m_keywordName.buf, name) == 0) {
				found = list;
				break;
			}
			list = list->get(lPREVIOUS);
		}
	}

	if (!found) {
		found = addToEnd(lNEXT, start);
	}
	return found;
}

CAnnotationData* CAnnotationData::addElementToStart(CAnnotationData* &start, char* name)
{
	CAnnotationData* newData = addToList(lNEXT, start);
	newData->m_anName = name;
	newData->m_keywordName = name;
	return newData;
}

CAnnotationData* CAnnotationData::addElement(CAnnotationData* &start, CConfigAttributes* confAttr)
{
	CAnnotationData* retval = NULL;
	if (confAttr->m_addToFront) {
		retval=addElementToStart(start, confAttr->m_keyword);
	}
	else {
		retval=addElementToEnd(start, confAttr->m_keyword, true);
	}
	return retval;
}

CAnnotationData* CAnnotationData::copyList(CAnnotationData* from)
{
    CAnnotationData* listStart = NULL;
    while (from){
        CAnnotationData* newData = addToEnd(lNEXT, listStart);
        newData->copyData(from);
        from = from->get(lNEXT);
    }

    return listStart;
}

void CAnnotationData::printList(CAnnotationData* from)
{
    int i = 0;
    while (from){
        gd->dbgPrintf(TINFO, "%i: %s\n", i,from->m_keywordName.buf);
        //printf("%i: %s\n", i, from->m_keywordName);

        i++;
        from = from->get(lNEXT);
    }
}

CAnnotationData* CAnnotationData::copyData(CAnnotationData* from)
{
    if (from) {
        m_anGroup = from->m_anGroup;								// Module, Parameter, Element
        m_anName = from->m_anName;
        m_keywordName = from->m_keywordName;
        m_anFunction = from->m_anFunction;
        m_showInOutput = from->m_showInOutput;
        m_removeQuotes = from->m_removeQuotes;
        m_assignmentList = from->m_assignmentList->copy();
        m_isList = from->m_isList;
        m_knArguments = from->m_knArguments;
        m_knArgumentsList = from->m_knArgumentsList;
        m_knArgumentsIn = from->m_knArgumentsIn;
        m_newParent = from->m_newParent;
        m_anType = from->m_anType;
        m_argFormat = from->m_argFormat;
        m_parent = from->m_parent;
        m_isDefault = from->m_isDefault;
        m_anArgTypeList = from->m_anArgTypeList;
        m_anArgTypeListList = from->m_anArgTypeListList;
        m_labelList = from->m_labelList;
        m_showInTag = from->m_showInTag;
        m_min = from->m_min;
        m_max = from->m_max;
        m_min_set = from->m_min_set;
        m_max_set = from->m_max_set;
        m_m1EqualsInfinity = from->m_m1EqualsInfinity;
        m_accessSpecPrivate = from->m_accessSpecPrivate;
        m_tagMaxOccurs = from->m_tagMaxOccurs;
        m_tagSelectExisting = from->m_tagSelectExisting;
        m_removeRedundantTags = from->m_removeRedundantTags;
        m_mandatory=from->m_mandatory;         
        m_selectNameSpace = from->m_selectNameSpace;

        m_fileName = from->m_fileName;
        m_linecnt = from->m_linecnt;
        m_moduleScope = from->m_moduleScope;
    }

	return this;
}

CAnnotationData* CAnnotationData::initData(CConfigAttributes* from)
{
    if (from) {
        m_prefix = from->m_prefix.buf;
        m_keywordName = m_prefix + from->m_keyword.buf;
        m_anFunction = from->m_anFunction;
        m_showInOutput = from->m_showInOutput;
        m_removeQuotes = from->m_removeQuotes;
        m_isList = from->m_isList;
        m_knArguments = from->m_knArguments;
        m_knArgumentsList = from->m_knArgumentsList;
        m_newParent = from->m_newParent;
        m_anType = from->m_anType;
        m_argFormat = from->m_argFormat;
        m_parent = from->m_parent;
        m_anArgTypeList = from->m_anArgTypeList;
        m_anArgTypeListList = from->m_anArgTypeListList;
        m_labelList = from->m_labelList;
        m_showInTag = from->m_showInTag;
        m_min = from->m_min;
        m_max = from->m_max;
        m_min_set = from->m_min_set;
        m_max_set = from->m_max_set;
        m_m1EqualsInfinity = from->m_m1EqualsInfinity;
        m_accessSpecPrivate = from->m_accessSpecPrivate;
        m_tagMaxOccurs = from->m_tagMaxOccurs;
        m_tagSelectExisting = from->m_tagSelectExisting;
        m_removeRedundantTags = from->m_removeRedundantTags;
        m_mandatory = from->m_mandatory;
        m_anGroup = from->m_anGroup;

        if (from->m_anName.isEmpty()) {
            m_anName = from->m_keyword;
        }
        else {
            m_anName = from->m_anName;
        }

    }
    return this;
}


void CAnnotationData::signExtendtoElementtype(bool isUnsigned, long identTypeId, long bitsize)
{
    //if (m_assignmentList->m_value1.isSet == false) {
    if (m_isDefault) {
        //errorPrnt(TERROR, "Value not initialized\n");
    }
    if (!isUnsigned) {
        m_assignmentList->m_value1.m_bitSize = bitsize;
        m_assignmentList->m_value1.signExtend();
    }

    if (identTypeId == TYPE_FLOAT || identTypeId == TYPE_DOUBLE) {
        static int cnt = 0;
        cnt++;
        CValue* value = &m_assignmentList->m_value1;
        if (value->format != ANARG_DOUBLE && value->format != ANARG_FLOAT32) {
            long64 valueL =  value->l;
            value->f = (double)valueL;
            value->format = ANARG_DOUBLE;
        }
    } 
}


CAnnotationData* CAnnotationData::findFirstKeyWord(char* keyWord)
{
	CAnnotationData* retval = NULL;
	CAnnotationData* actual = this;
	while (actual) {
		if (strcasecmp(actual->m_keywordName, keyWord)==0){
			retval = actual;
			break;
		}
		actual = actual->get(lNEXT);
	}
	return retval;
}

CAnnotationData* CAnnotationData::findFirstAnnotation(CAnnotationData* an, long cntMin)
{
	CAnnotationData* retval = NULL;
	CAnnotationData* actual = this;
	m_countFind = 0;
	while (actual) {
		m_countFind++;
		if ((actual->m_anFunction == an->m_anFunction) && (m_countFind > cntMin || cntMin == 0)){
			retval = actual;
			break;
		}
		actual = actual->get(lNEXT);
	}
	return retval;
}

CAnnotationData* CAnnotationData::findFirstAnnotationFunction(eAnFunction function, long cntMin)
{
	CAnnotationData* retval = NULL;
	CAnnotationData* actual = this;
	m_countFind = 0;
	while (actual) {
		m_countFind++;
		if ((actual->m_anFunction == (unsigned long) function) && (m_countFind > cntMin || cntMin == 0) && ((*actual->m_moduleScope == NULL) || (*actual->m_moduleScope == gd->actualModule))){
			retval = actual;
			break;
		}
		actual = actual->get(lNEXT);
	}
	return retval;
}

CAnnotationData* CAnnotationData::findFirstAnnotationFunctionList(eAnFunction function[], long knFunctions, long cntMin)
{
	CAnnotationData* retval = NULL;
	CAnnotationData* actual = this;
	m_countFind = 0;
	while (actual) {
		m_countFind++;
		if ((m_countFind > cntMin || cntMin == 0) && ((*actual->m_moduleScope == NULL) || (*actual->m_moduleScope == gd->actualModule))){
			for (int i = 0; i < knFunctions; i++){
				if (actual->m_anFunction == (unsigned long)function[i]){
					retval = actual;
					break;
				}
			}
			if (retval){
				break;
			}
		}
		actual = actual->get(lNEXT);
	}
	return retval;
}


CAnnotationData* CAnnotationData::findFirstName(char* name)
{
	CAnnotationData* retval = NULL;
	CAnnotationData* actual = this;
	while (actual) {
		if (strcasecmp(actual->m_anName, name) == 0){
			retval = actual;
			break;
		}
		actual = actual->get(lNEXT);
	}
	return retval;
}

CAnnotationData* CAnnotationData::findLastAnnotation(eAnFunction function, long cntMin, bool skipInternal)
{
	CAnnotationData* retval = NULL;
	CAnnotationData* actual = findLast(lNEXT, this);

    m_breakFound = 0;
    int hirachyLevel = 0;
	while (actual) {
		m_countFind++;
		if (hirachyLevel ==0 && actual->m_anFunction == (unsigned long) function && (m_countFind > cntMin || cntMin == 0) &&
            ((*actual->m_moduleScope == NULL) || (*actual->m_moduleScope == gd->actualModule))){
            if (!skipInternal || !actual->m_isInternalcopy) {
                retval = actual;
                break;
            }
        }
        else if (actual->m_anFunction == ANFUCNTION_CTRL_BREAK) {
            m_breakFound = 1;
            break;
        }
        else if (actual->m_anFunction == ANFUNCTION_CTRL_INC_HIRACHY_LEVEL) {
            hirachyLevel ++;
        }
        else if (actual->m_anFunction == ANFUNCTION_CTRL_DEC_HIRACHY_LEVEL) {
           hirachyLevel --;
        }
		actual = actual->get(lPREVIOUS);
	}
	return retval;
}

CAnnotationData* CAnnotationData::findLastAnnotationKeyword(char* keyWord, long cntMin)
{
    CAnnotationData* retval = NULL;
    CAnnotationData* actual = findLast(lNEXT, this);

    m_breakFound = 0;
    while (actual) {
        m_countFind++;
        if (strcasecmp(actual->m_keywordName, keyWord) == 0 && (m_countFind > cntMin || cntMin == 0) && ((*actual->m_moduleScope == NULL) || (*actual->m_moduleScope == gd->actualModule))){
            retval = actual;
            break;
        }
        else if (actual->m_anFunction == ANFUCNTION_CTRL_BREAK) {
            m_breakFound = 1;
            break;
        }
        actual = actual->get(lPREVIOUS);
    }
    return retval;
}

CAssignmentList* CAnnotationData::insertValues(eAnArgTypes argType, char* text, long64 valueL, double valuef, eAnArgDPLYType dplyType, bool add )
{
	// to do: check types
	eAnArgTypes newArgType = argType;
	CAssignmentList* list = findLast(lNEXT, m_assignmentList);
	CValue* ptValue = &list->m_value1;
	ptValue->dplyType = dplyType;
	switch (argType) {
	case ANARG_NONE:
	case ANARG_IGNORE:
		break;
	case ANARG_STRING:
		break;
    case ANARG_ENUM_ELEMENT:
    case ANARG_ENUM_TYPE:
	case ANARG_LABEL:
	case ANARG_CONSTLABEL:
		if (add) {
			list->m_string.stradd(text);
		}
		else {
			list->m_string = text;
		}
		if (m_labelList) {
			newArgType = ANARG_CONSTLABEL;
		}
		list->m_string.RemoveQuotes();
        ptValue->labelStr = text;
		break;
    case ANARG_REFERENCE:
        if (add) {
            list->m_string.stradd(text);
        }
        else {
            list->m_string = text;
        }
        list->m_string.ReplaceAll("\"", "");
        break;
	case ANARG_BOOL:
	case ANARG_INTEGER:
	case ANARG_UNSIGNED_INTEGER:
		ptValue->setLong(valueL, text, argType);
		ptValue->l = valueL;
		break;
	case ANARG_DOUBLE:
		ptValue->setFloat(valuef, text);
		break;
	default:
		break;
	}
	list->m_argType = newArgType;
	if (!add) {
		m_knArgumentsIn++;
	}
    return list;
}

CAssignmentList* CAnnotationData::addValueToEnd()
{
	return addToEnd(lNEXT, m_assignmentList);
}


CAssignmentList* CAnnotationData::addValue(CAssignmentList* insertAfter)
{
    return addToList(lNEXT, insertAfter);
}


CAssignmentList* CAnnotationData::removeValue(CAssignmentList* &elementToRemove)
{
    
    return deleteFromList(lNEXT, elementToRemove);
}


void CAnnotationData::setLabel(char* label)
{
	m_assignmentList->m_string = label;
	//	m_assignmentList->m_hasString = true;
	m_assignmentList->m_argType = ANARG_STRING;
}
void CAnnotationData::setIntValue(long value)
{
	m_assignmentList->m_argType = ANARG_INTEGER;
	m_assignmentList->m_value1.setLong(value, "");
}
void CAnnotationData::setUIntValue(unsigned long value)
{
	m_assignmentList->m_argType = ANARG_UNSIGNED_INTEGER;
	m_assignmentList->m_value1.setLong(value, "", ANARG_UNSIGNED_INTEGER);
}

bool CAnnotationData::getArgListString(CStr *str1, CStr *str2, CStr *str3)
{
	CAnArgTypeList *arglist = m_anArgTypeList;
	while (arglist) {
		str1->stradd(anArgNames[arglist->m_argType]);
		if (arglist->get(lNEXT)) {
			str1->stradd(", ");
		}
		arglist = arglist->get(lNEXT);
	}

    arglist = m_anArgTypeListList;
    while (arglist) {
        str2->stradd(anArgNames[arglist->m_argType]);
        if (arglist->get(lNEXT)) {
            str2->stradd(", ");
        }
        arglist = arglist->get(lNEXT);
    }

	CAssignmentList *asmList = m_assignmentList;
	while (asmList) {
		str3->stradd(anArgNames[asmList->m_argType]);
		if (asmList->get(lNEXT)) {
			str3->stradd(", ");
		}
		asmList = asmList->get(lNEXT);
	}
	return true;
}

bool CAnnotationData::checkArgList()
{
	bool retval = true;
    if (m_knArguments == 0 && m_knArgumentsList==0 && m_knArgumentsIn == 0){
		return retval;
	}
	CStr arg1 = "";
	CStr arg2 = "";
    CStr arg3 = "";
    if (m_knArguments == 0 && m_knArgumentsList == 0 && m_knArgumentsIn > 0) {
		retval = false;
        getArgListString(&arg1, &arg2, &arg3);
		gd->dbgPrintf(TERROR, "@%s: Unexpected arguments: found '%s' \n", m_keywordName.buf, arg3.buf);
	}
	else {
        // legacy
        if (m_isList && m_anArgTypeListList == 0) {
            m_anArgTypeListList = m_anArgTypeList;
            m_knArgumentsList = m_knArguments;
            m_anArgTypeList = NULL;
            m_knArguments = 0;
        }

        if (m_knArguments > 0 && m_knArgumentsList > 0) {
            if (m_knArgumentsIn <= m_knArguments || (m_knArgumentsIn - m_knArguments) % m_knArgumentsList != 0){
                retval = false;
                //getArgListString(&arg1, &arg2, &arg3);
                gd->dbgPrintf(TERROR, "@%s: Wrong number of arguments: expected %d + multiple of %d, actual number of arguments: %d\n", m_keywordName.buf, m_knArguments, m_knArgumentsList, m_knArgumentsIn);
            }
        }
        else if (m_knArguments > 0) {
            if (m_knArguments != m_knArgumentsIn){
                retval = false;
                //getArgListString(&arg1, &arg2, &arg3);
                gd->dbgPrintf(TERROR, "@%s: Wrong number of arguments: expected '%s', found '%s' \n", m_keywordName.buf, arg1.buf, arg3.buf);
            }
        }
        else if (m_knArgumentsList > 0) {
            if (m_knArgumentsIn % m_knArgumentsList !=0){
                retval = false;
                getArgListString(&arg1, &arg2, &arg3);
                gd->dbgPrintf(TERROR, "@%s: Wrong number of arguments: expected multiple of %d, actual number of arguments: %d\n", m_keywordName.buf, m_knArgumentsList, m_knArgumentsIn);
            }
        }
	}

	if (!retval) {
		return retval;
	}

	// check all arguments, number of arguments should be correct

	CAnArgTypeList *arglist = m_anArgTypeList;
	CAssignmentList *asmList = m_assignmentList;


    if (arglist == NULL) {
        arglist = m_anArgTypeListList;
    }
	int i = 0;
	while (asmList) {
		int argStatus = checkSingleArg(arglist->m_argType, asmList->m_argType);
        switch (argStatus) {
        case ARG_OK              :
            break;
        case ARG_ERROR           :
            break;
        case ARG_RESOLVED_ENUM1  :
            if (!asmList->m_value1.labelStr.isEmpty()) {
                asmList->m_argType = ANARG_LABEL; // replace resolved enum with label
            }
            break;
        case ARG_RESOLVED_ENUM2  :
            if (asmList->m_value1.labelStr.isEmpty()) {
                if (gd->checkEnumArgsElement) {
                    argStatus = ARG_ERROR;
                }
            }
            else {
                asmList->m_argType = ANARG_LABEL; // replace resolved enum with label
                asmList->m_value1.labelStr.RemoveQuotes();
                Cvdt* tmp = findIdentifier(asmList->m_value1.labelStr.buf);
                if (!tmp || !tmp->identifier || !tmp->isEnumElement) {
                    if (gd->checkEnumArgsElement) {
                        argStatus = ARG_ERROR;
                    }
                }
            }
            break;
        case ARG_ENUMTYPE: {
            argStatus = ARG_ERROR;
            CStr name = HASH_PREFIX;
            m_enumName = asmList->m_string;
            name = name + asmList->m_string;
            name.ReplaceAll("::", ".");
            Cvdt* vdt = htab.hashlookup(name.buf);
            if (vdt  && vdt->identifier && vdt->identifier->GetTypeId() == TYPE_ENUM){
                argStatus = ARG_OK;
                int cnt = 0;
                CIdentifier *dt_elements = vdt->identifier->getDataType()->GetElements();
                //m_removeQuotes = true;
                for (CIdListIterator actual_ident = dt_elements->GetIdList_Begin();
                    actual_ident != dt_elements->GetIdList_End();
                    actual_ident++) {
                    if (actual_ident->m_cvdt) {
                        if (cnt != 0) {
                            asmList = addValue(asmList);
                            m_knArgumentsIn++;
                        }
                        char* name = actual_ident->m_cvdt->name;
                        asmList->m_string = name;
                        asmList->m_argType = ANARG_ENUM_ELEMENT;
                        cnt++;
                    }
                }
            }
            // replace arg with enum elements
        }
            break;
        }

        if (argStatus == ARG_ERROR) { 
            CStr buffer;
            gd->dbgPrintf(TERROR, "@%s: Wrong argument %d: expected '%s', found '%s' (%s) \n", m_keywordName.buf, i + 1, anArgNames[arglist->m_argType], anArgNames[asmList->m_argType], asmList->getValueString(&buffer));           
            break;
        }
        
		// check for valid range
		if (arglist->m_argType == ANARG_INTEGER || arglist->m_argType == ANARG_UNSIGNED_INTEGER) {
            if (m_min_set && m_min.m_type == ARG_INTREF_TYPE_INT && asmList->m_value1.l < m_min.m_value && 
                (!m_m1EqualsInfinity || (m_m1EqualsInfinity && asmList->m_value1.l!=-1))) {
                gd->dbgPrintf(TERROR, "@%s: argument_%d=%lld below minimum range (%d) \n", m_keywordName.buf, i + 1, asmList->m_value1.l, m_min.m_value);
			}
            if (m_max_set && m_max.m_type == ARG_INTREF_TYPE_INT && asmList->m_value1.l > m_max.m_value) {
                gd->dbgPrintf(TERROR, "@%s: argument_%d=%lld exceeds maximum range (%d) \n", m_keywordName.buf, i + 1, asmList->m_value1.l, m_max.m_value);
            }
            if (m_argFormat == ANARGFORAMT_EVEN && (asmList->m_value1.l & 1) != 0){
                gd->dbgPrintf(TERROR, "@%s: argument_%d=%lld must be even \n", m_keywordName.buf, i + 1, asmList->m_value1.l);
            }
            if (m_argFormat == ANARGFORAMT_ODD && (asmList->m_value1.l & 1) != 1){
                gd->dbgPrintf(TERROR, "@%s: argument_%d=%lld must be odd \n", m_keywordName.buf, i + 1, asmList->m_value1.l);
            }
		}
		else if (arglist->m_argType == ANARG_LABEL || arglist->m_argType == ANARG_CONSTLABEL) {
			bool labelFound = false;
			if (m_labelList == NULL) {
				labelFound = true;
			}
			else {
				CStr tmp;
				labelFound = CStringList::findInList(m_labelList, asmList->getValueString(&tmp));
				
				if (!labelFound) {
					gd->dbgPrintf(TERROR, "@%s: argument %d '%s' out of range \n", m_keywordName.buf, i + 1, asmList->getValueString(&tmp));
				}
			}

		}

		asmList = asmList->get(lNEXT);
		arglist = arglist->get(lNEXT);
		if (arglist == NULL){
            arglist = m_anArgTypeListList;
		}
		i++;
	}


	return retval;
}

// Arg1: requested, Arg2: actual
int CAnnotationData::checkSingleArg(eAnArgTypes arg1, eAnArgTypes arg2)
{
    if (arg1 == ANARG_VARIANT) {
       // int dbg = 0;
    }
    if (arg2 == ANARG_DOUBLE) {
      //  int dbg = 0;
    }
    eCheckArgRetval retval = ARG_OK;
	switch (arg1) {
	case ANARG_NONE:
		switch (arg2) {
		case ANARG_NONE:
			retval = ARG_OK;
			break;
		case ANARG_IGNORE:
		case ANARG_STRING:
		case ANARG_LABEL:
        case ANARG_ENUM_ELEMENT:
        case ANARG_ENUM_TYPE:
		case ANARG_CONSTLABEL:
		case ANARG_BOOL:
		case ANARG_INTEGER:
		case ANARG_UNSIGNED_INTEGER:
        case ANARG_FLOAT32:
		case ANARG_DOUBLE:
		default:
			retval = ARG_ERROR;
			break;
		};
		break;
	case ANARG_IGNORE:
		switch (arg2) {
		case ANARG_IGNORE:
			retval = ARG_OK;
			break;
		case ANARG_NONE:
		case ANARG_STRING:
		case ANARG_LABEL:
        case ANARG_ENUM_ELEMENT:
        case ANARG_ENUM_TYPE:
		case ANARG_CONSTLABEL:
		case ANARG_BOOL:
		case ANARG_INTEGER:
		case ANARG_UNSIGNED_INTEGER:
        case ANARG_FLOAT32:
		case ANARG_DOUBLE:
		default:
			retval = ARG_ERROR;
			break;
		};
		break;
	case ANARG_STRING:
		switch (arg2) {
		case ANARG_STRING:
		case ANARG_BOOL:
		case ANARG_INTEGER:
		case ANARG_UNSIGNED_INTEGER:
		case ANARG_DOUBLE:
        case ANARG_FLOAT32:
        case ANARG_REFERENCE:
			retval = ARG_OK;
			break;
		case ANARG_IGNORE:
		case ANARG_NONE:
		default:
			retval = ARG_ERROR;
			break;
		};
		break;
	case ANARG_LABEL:
	case ANARG_CONSTLABEL:
		switch (arg2) {
		case ANARG_LABEL:
		case ANARG_CONSTLABEL:
		case ANARG_DOUBLE:
        case ANARG_FLOAT32:
        case ANARG_REFERENCE:
			retval = ARG_OK;
			break;
        case ANARG_INTEGER:
        case ANARG_UNSIGNED_INTEGER:
            retval = ARG_RESOLVED_ENUM1;
            break;
		case ANARG_IGNORE:
		case ANARG_NONE:
		case ANARG_STRING:
		case ANARG_BOOL:
		default:
			retval = ARG_ERROR;
			break;
		};
		break;
    case ANARG_ENUM_TYPE:
        switch (arg2) {
        case ANARG_LABEL:
        case ANARG_CONSTLABEL:
            retval = ARG_ENUMTYPE;
            break;
        case ANARG_INTEGER:
        case ANARG_UNSIGNED_INTEGER:
        case ANARG_REFERENCE:
        case ANARG_DOUBLE:
        case ANARG_FLOAT32:
        case ANARG_IGNORE:
        case ANARG_NONE:
        case ANARG_STRING:
        case ANARG_BOOL:
        default:
            retval = ARG_ERROR;
            break;
        };
        break;
    case ANARG_ENUM_ELEMENT:
        switch (arg2) {
        case ANARG_LABEL:
        case ANARG_CONSTLABEL:
        case ANARG_INTEGER:
        case ANARG_UNSIGNED_INTEGER:
            retval = ARG_RESOLVED_ENUM2;
            break;
        case ANARG_REFERENCE:
        case ANARG_DOUBLE:
        case ANARG_FLOAT32:
        case ANARG_IGNORE:
        case ANARG_NONE:
        case ANARG_STRING:
        case ANARG_BOOL:
        default:
            retval = ARG_ERROR;
            break;
        };
        break;
	case ANARG_BOOL:
		switch (arg2) {
		case ANARG_BOOL:
		case ANARG_INTEGER:
		case ANARG_UNSIGNED_INTEGER:
        case ANARG_REFERENCE:
			retval = ARG_OK;
			break;
		case ANARG_LABEL:
        case ANARG_ENUM_ELEMENT:
        case ANARG_ENUM_TYPE:
		case ANARG_CONSTLABEL:
		case ANARG_IGNORE:
		case ANARG_NONE:
		case ANARG_STRING:
		case ANARG_DOUBLE:
        case ANARG_FLOAT32:
		default:
			retval = ARG_ERROR;
			break;
		};
		break;
	case ANARG_INTEGER:
		switch (arg2) {
		case ANARG_BOOL:
		case ANARG_INTEGER:
		case ANARG_UNSIGNED_INTEGER:
        case ANARG_REFERENCE:
			retval = ARG_OK;
			break;
		case ANARG_LABEL:
        case ANARG_ENUM_ELEMENT:
        case ANARG_ENUM_TYPE:
		case ANARG_CONSTLABEL:
		case ANARG_IGNORE:
		case ANARG_NONE:
		case ANARG_STRING:
		case ANARG_DOUBLE:
        case ANARG_FLOAT32:
		default:
			retval = ARG_ERROR;
			break;
		};
		break;
	case ANARG_UNSIGNED_INTEGER:
		switch (arg2) {
		case ANARG_BOOL:
		case ANARG_UNSIGNED_INTEGER:
        case ANARG_REFERENCE:
			retval = ARG_OK;
			break;
		case ANARG_INTEGER:
		case ANARG_LABEL:
        case ANARG_ENUM_ELEMENT:
        case ANARG_ENUM_TYPE:
		case ANARG_CONSTLABEL:
		case ANARG_IGNORE:
		case ANARG_NONE:
		case ANARG_STRING:
		case ANARG_DOUBLE:
        case ANARG_FLOAT32:
		default:
			retval = ARG_ERROR;
			break;
		};
		break;
	case ANARG_DOUBLE:
    case ANARG_VARIANT:
		switch (arg2) {
		case ANARG_BOOL:
		case ANARG_UNSIGNED_INTEGER:
		case ANARG_INTEGER:
		case ANARG_DOUBLE:
        case ANARG_FLOAT32:
        case ANARG_REFERENCE:
			retval = ARG_OK;
			break;
		case ANARG_LABEL:
        case ANARG_ENUM_ELEMENT:
        case ANARG_ENUM_TYPE:
		case ANARG_CONSTLABEL:
		case ANARG_IGNORE:
		case ANARG_NONE:
		case ANARG_STRING:
		default:
			retval = ARG_ERROR;
			break;
		};
		break;
	default:
		break;
	};
	return retval;
}

void CAnnotationData::errorPrnt(eMsgTypes msgType, char* format, ...) {
	char tmp[4000];
	char tmp2[4000];
	va_list ap;
	va_start(ap, format);
    if (this){
        if (m_fileName) {
            // adjust for Linux, so references for Linux and Win are equal
            CStr fileName = m_fileName;
            fileName.ReplaceAll("\\", "/");

            sprintf(tmp, "%s(%ld): %s", fileName.buf, m_linecnt, format);
        }
        else {
            sprintf(tmp, "%s(%ld): %s", "this file", m_linecnt, format);
        }
    }
    else {
        sprintf(tmp, "%s", format);
    }
	vsprintf_s(tmp2, 3999, tmp, ap);
	dbgPrintf(msgType, tmp2);
}

bool CAnnotationData::checkRange(CAnnotationData* ptAnRangeMin, CAnnotationData* ptAnRangeMax, CAssignmentList* ptRangeList)
{
    // check default range
    bool retval = true;
    CAnnotationData* prnt;
    if (ptAnRangeMin && ptAnRangeMin->m_showInOutput) {
        if (m_assignmentList->m_value1.lt(&ptAnRangeMin->m_assignmentList->m_value1)) {
            char buf1[100], buf2[100];
            prnt = ptAnRangeMin;
            if (!ptAnRangeMin->m_fileName || ptAnRangeMax->m_linecnt == 0) {
                prnt = this;
            }
            if (prnt) {
                prnt->errorPrnt(TERROR, "Default Value '%s' < RANGE_MIN '%s'\n", m_assignmentList->m_value1.getValuStr(buf1), ptAnRangeMin->m_assignmentList->m_value1.getValuStr(buf2));
            }
            else {
                dbgPrintf(TERROR, "Default Value '%s' < RANGE_MIN '%s'\n", m_assignmentList->m_value1.getValuStr(buf1), ptAnRangeMin->m_assignmentList->m_value1.getValuStr(buf2));
            }
            retval = false;
        }
    }
    if (ptAnRangeMax && ptAnRangeMax->m_showInOutput) {
        if (m_assignmentList->m_value1.gt(&ptAnRangeMax->m_assignmentList->m_value1)) {
            char buf1[100], buf2[100];
            prnt = ptAnRangeMax;
            if (!ptAnRangeMax->m_fileName || ptAnRangeMax->m_linecnt==0) {
                prnt = this;
            }
            if (prnt) {
                prnt->errorPrnt(TERROR, "Default Value '%s' > RANGE_MAX '%s'\n", m_assignmentList->m_value1.getValuStr(buf1), ptAnRangeMax->m_assignmentList->m_value1.getValuStr(buf2));
            }
            else {
                dbgPrintf(TERROR, "Default Value '%s' > RANGE_MAX '%s'\n", m_assignmentList->m_value1.getValuStr(buf1), ptAnRangeMax->m_assignmentList->m_value1.getValuStr(buf2));
            }
            retval = false;
        }
    }

    // check dedicated values
    CAssignmentList *list = ptRangeList;
    bool valueFound = false;
    while (list){
            if (m_assignmentList->m_value1.ul == list->m_value1.ul || m_assignmentList->m_argType==ANARG_REFERENCE) {
                valueFound = true;
            }
            list = list->get(lNEXT);
    } 
    if (ptRangeList && !valueFound) {
        char buf1[100];
        CAnnotationData* ptAnList = (CAnnotationData*) ptRangeList->m_pointer;
        if (ptAnList && ptAnList->m_fileName) {
            ptAnList->errorPrnt(TERROR, "Default Value '%s' not in Range List\n", m_assignmentList->m_value1.getValuStr(buf1));
        }
        else {
            dbgPrintf(TERROR, "Default Value '%s' not in Range List\n", m_assignmentList->m_value1.getValuStr(buf1));
        }
        retval = false;
    }
    return retval;
}




