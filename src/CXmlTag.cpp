/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

#include "CXmlTag.h"
#include "CAnnotationData.h"
#include "scanConfig.h"

char tagSourceName[knTAgSource][20] = {
    "TAG_SOURCE_CNTRL ",
    "TAG_SOURCE_MODULE",
    "TAG_SOURCE_STRUCT",
    "TAG_SOURCE_ELEMENT",
    "TAG_SOURCE_EMITDATA"
};

// #define DBGPRINT_XMLTREE


//////////////////////////////////////////////////////////////////////////
// class CXMLTag
//////////////////////////////////////////////////////////////////////////
CXMLTag::CXMLTag()
{
	m_attributes = NULL;
	m_tagName = "";
	m_value = "";
	m_keyWord = "";
	m_sub = NULL;
	m_removeRedundantTags = false;
    m_mandatory = false;
    m_tagListcnt = 0;
    m_actual = this;

	m_tabSize = DEFAULT_TABSIZE;
	m_lineLength = DEFAULT_MAXLINELENGTH;
	m_attributeLength = DEFAULT_MAXLINELENGTH;
}

// methods
void CXMLTag::emit(FILE* fp, int tab, bool printKeyword)
{
    if (fp == NULL) {
        fp = stderr;
        printKeyword = true;
    }
	CXMLTag *list = this;
	while (list) {
        if (printKeyword){
            xmlPrintf(tab, m_lineLength, m_attributeLength, fp, "<%s,%s", list->m_tagName.buf, list->m_keyWord.buf);
        }
        else {
            xmlPrintf(tab, m_lineLength, m_attributeLength, fp, "<%s", list->m_tagName.buf);
        }
		if (list->m_attributes) {
            list->m_attributes->emitAll(fp, tab + m_tabSize, m_lineLength, m_attributeLength, printKeyword);
		}
		if (list->m_sub) {
			xmlPrintf(tab, m_lineLength, m_attributeLength, fp, ">\n");
            list->m_sub->emit(fp, tab + m_tabSize, printKeyword);
            if (printKeyword){
                xmlPrintf(tab, m_lineLength, m_attributeLength, fp, "</%s,%s>\n", list->m_tagName.buf, list->m_keyWord.buf);
            }
            else {
                xmlPrintf(tab, m_lineLength, m_attributeLength, fp, "</%s>\n", list->m_tagName.buf);
            }
		}
		else {
			xmlPrintf(tab, m_lineLength, m_attributeLength, fp, "/>\n");

		}
		list = list->get(lNEXT);
	}
}

// methods
void CXMLTag::removeRedundantTags()
{
	CXMLTag *list = this;
	while (list) {
		if (list->m_sub) {
			list->m_sub->removeRedundantTags();
		}

		if (list->m_removeRedundantTags) {
			// for all sub tags
			CXMLTag *list1 = list->m_sub;
			while (list1){
				CXMLTag* lastList = list1;
				CXMLTag *list2 = list1->get(lNEXT);
				while (list2){
					if (list1->compareAttributes(list2) && list2->compareAttributes(list1)) {
						lastList->lpt[lNEXT] = list2->lpt[lNEXT];
					}
					else {
						lastList = list2;
					}
					list2 = list2->get(lNEXT);
				}
				list1 = list1->get(lNEXT);
			}
		}

		list = list->get(lNEXT);
	}
}

bool CXMLTag::compareAttributes(CXMLTag* list2)
{
	CAnnotationData* actualData1 = m_attributes;					// Parameter attributes
	CAnnotationData* actualData2 = list2->m_attributes;					// Parameter attributes

	bool isEqual = true;
	if (actualData1 && !actualData2) {
		isEqual = false;
	}
	while (actualData1 && isEqual) {
		if (actualData1->m_showInOutput){
			CAnnotationData* found=actualData2->findFirstName(actualData1->m_anName);
			if (found) {
				CStr val1 = "";
				CStr val2 = "";
				if (strcmp(actualData1->m_assignmentList->getValueString(&val1), found->m_assignmentList->getValueString(&val2)) != 0){
					isEqual = false;
				}
				else {
					//int dbg = 0;
				}
			}
			else {
				isEqual = false;
			}
		}
		actualData1 = actualData1->get(lNEXT);
	}
	return isEqual;
}

void CXMLTag::saveActiveTag(CAnnotationData* an, eActiveTagSource_t source)
{

   // printf("%x save Tag=%d\n", this, m_tagListcnt);
    static int cnt = 0;
    cnt++;
    if (cnt == 544){
        //int dbg = 0;
    }
    dbgPrintf(TINFO, "save Tag %s, source=%s\n", m_actual->m_keyWord.buf, tagSourceName[source]);
    m_tagList[m_tagListcnt] = m_actual;
    if (m_tagListcnt >= knMAXTAGLIST-1) {
        if (an){
            an->errorPrnt(TERROR, "Tag stack exceeded maximum size\n");
        }
        else {
            dbgPrintf(TERROR, "Tag stack exceeded maximum size\n");
        }
    }
    else {
        m_tagListcnt++;
    }
}

CXMLTag* CXMLTag::restoreActiveTag(CAnnotationData* an, eActiveTagSource_t source)
{
  //  printf("%x restore Tag=%d\n",this,  m_tagListcnt);
    if (m_tagListcnt == 36) {
        //int dbg = 0;
    }
    CXMLTag* newTag=NULL;
    m_tagListcnt--;
    if (m_tagListcnt < 0) { 
        if (an){
            an->errorPrnt(TERROR, "Tag stack mismatch\n");
        }
        else {
            dbgPrintf(TERROR, "Tag stack mismatch\n");
        }
        m_tagListcnt = 0;
    }
    else {
        newTag = m_tagList[m_tagListcnt];
        dbgPrintf(TINFO, "restore Tag %s, source=%s\n", newTag->m_keyWord.buf, tagSourceName[source]);
    }
    if (newTag) {
        m_actual = newTag;
    }
    return newTag;
}


// direction: 0=next, 1= previous
CXMLTag* CXMLTag::find(char* keyWord, int direction, bool findLast )
{
	CXMLTag *retval = NULL;
	if (strcmp(m_keyWord.buf, keyWord) == 0) {
		retval = this;
		if (direction == 0 && findLast){
			while (retval && retval->get(lNEXT) && strcmp(retval->get(lNEXT)->m_keyWord.buf, keyWord) == 0) {
				retval = retval->get(lNEXT);
			}
		}
	}
	else {
		if (m_sub != NULL && direction == 0) {
			retval = m_sub->find(keyWord, direction, findLast);
		}
		if (retval == NULL) {
			if (get(lNEXT^direction) != NULL) {
				retval = get(lNEXT^direction)->find(keyWord, direction, findLast);
			}
		}
	}
	return retval;
}

// search name1.name2.name3
//-> name1 first ->name2 ...
CXMLTag* CXMLTag::findParent(CStr * parentName, bool findLast, bool noWarning)
{
    dbgPrintf(TINFO, "XML find parent: root: %s, parent=%s\n", this->m_keyWord.buf, parentName->buf);
    
	CXMLTag* addTo = this;
	CStr fullParent = *parentName;
	char* tmp = strchr(fullParent.buf, '.');
	if (tmp) {
		*tmp = '\0';
		addTo = findParent(&fullParent, findLast, noWarning);
		if (addTo) {
			CStr newParent = tmp + 1;
			addTo = addTo->findParent(&newParent, findLast, noWarning);
		}
	}
	else {
		if (parentName && !parentName->isEmpty()) {
			addTo = find(parentName->buf, 0, findLast);
			if (!addTo) {
				addTo = find(parentName->buf, 1);
			}
		}
	}
	if (!addTo && !noWarning) {
		dbgPrintf(TERROR, "Parent Element '%s' not found\n", parentName->buf);
	}
	return addTo;
}

CXMLTag* CXMLTag::findParent(char* parentName, bool findLast, bool noWarning){
	CStr pname = parentName;
	return findParent(&pname, findLast, noWarning);
}

CXMLTag* CXMLTag::addSubTag(char* name, bool once, bool SelectExisting, char* keyWord, CStr* parent)
{
	if (strstr(name, "DEFAULT_DEPENDENCIES")!= 0) {
		//int dbg = 0;
	}
	CXMLTag* tag = NULL;
	CXMLTag* addTo = this;
	static char emptyChar[] = "";
	char* parentPt;
	if (parent && !parent->isEmpty()) {
		addTo = findParent(parent);
		parentPt = parent->buf;
	}
	else {
		parentPt = emptyChar;
	}

	if (addTo) {
		if (once) {		// only add once, search for first occurence and replace
			CXMLTag* find = addTo->m_sub;
			while (find){
				if (strcmp(find->m_tagName.buf, name) == 0) {
					tag = find;
					if (!SelectExisting) {
						tag->m_sub = NULL;
					}
					dbgPrintf(TINFO, "XML: replacing SUB_TAG %s (keyword=%s, parent=%s)\n", name, keyWord, parentPt);
					break;
				}
				find = find->get(lNEXT);
			}
		}
		if (tag == NULL) {
			CXMLTag* lastSub = addTo->m_sub;
			tag = addToEnd(lNEXT, addTo->m_sub);
			if (lastSub == NULL){
				tag->lpt[lPREVIOUS] = addTo;
#ifdef LIST_DEBUG
				tag->d_previous = addTo;
#endif
			}
			tag->m_tagName = name;
			dbgPrintf(TINFO, "XML: adding SUB_TAG %s (keyword=%s, parent=%s)\n", name, keyWord, parentPt);
		}

		if (keyWord) {
			tag->m_keyWord = keyWord;
		}
	}
	return tag;
}

CXMLTag* CXMLTag::addAttribute(CAnnotationData* anData[][kn_ANGROUP], int cnt, eAnGroups group, char* nameSpace)
{
	//CXMLTag *actual = this;
	CXMLTag *newActual = NULL;
	int copyCnt = 0;
    
    CAnnotationData* ptAnBreak=findLastAnnotationData(anData, cnt, group, ANFUCNTION_CTRL_BREAK);

    for (int i = CAnnotationData::m_countFindLevel; i <= cnt; i++){
		CAnnotationData* list = anData[i][group];
        if (i == CAnnotationData::m_countFindLevel && ptAnBreak){
            list = ptAnBreak;
        }

        CAnnotationData* listStart = list;
		while (list) {

#ifdef DBGPRINT_XMLTREE
            if (as.verbose) {
                dbgPrintf(TINFO, "\n/************ Current XML Tree ******************/\n");
                gd->xmlRoot->m_sub->emit(NULL, 0);  // print actual xml tree
                dbgPrintf(TINFO, "/************ END XML Tree ******************/\n\n");

            }
#endif

            //check for variable ranges
            if (list->m_max.m_type == ARG_INTREF_TYPE_REF) {
                CAnnotationData* ref = listStart->findLastAnnotationKeyword(&list->m_max.m_reference.buf[1]);
                 if (ref && !ref->m_isDefault) {
                    if (!(ref->m_m1EqualsInfinity && ref->m_assignmentList->m_value1.l == -1)){
                        if (list->m_assignmentList->m_value1.l > ref->m_assignmentList->m_value1.l){
                            list->errorPrnt(TERROR, "%s (%lld) exceeds maximum range (%d)\n", list->m_anName.buf, list->m_assignmentList->m_value1.l, ref->m_assignmentList->m_value1.l);
                        }
                    }
                }
            }

            if (list->m_min.m_type == ARG_INTREF_TYPE_REF) {
                CAnnotationData* ref = listStart->findLastAnnotationKeyword(&list->m_min.m_reference.buf[1]);
                bool mandatoryError = ref->m_isDefault && ref->m_mandatory; //allow mandatory error to be displayed 
                if (ref && !mandatoryError) {
                    if ((ref->m_m1EqualsInfinity && ref->m_assignmentList->m_value1.l == -1) || list->m_assignmentList->m_value1.l < ref->m_assignmentList->m_value1.l){
                        list->errorPrnt(TERROR, "%s (%lld) below minimum range (%d)\n", list->m_anName.buf, list->m_assignmentList->m_value1.l, ref->m_assignmentList->m_value1.l);
                    }
                }
            }

            if (strcmp(list->m_keywordName.buf, "h2xml_TAG_PROCESSOR") == 0){
                //int dbg = 0;
            }
            
            bool checkNameSpace = true;
            if (!list->m_selectNameSpace.isEmpty() && nameSpace!=NULL && nameSpace[0]!='\0') {
                // check right bound if strings match
                int maxLen = strlen(nameSpace) ;
                int maxLen2 = strlen(list->m_selectNameSpace.buf);
                if (maxLen > maxLen2) {
                    maxLen = maxLen2;
                }
    
                for (int i = 0; i < maxLen; i++){
                    if (list->m_selectNameSpace.buf[i] != nameSpace[i]) {
                        if (i>0) {
                            checkNameSpace = false;
                        }
                        break;
                    }
                }
            }

            if (checkNameSpace && ((*list->m_moduleScope == NULL) || (*list->m_moduleScope == gd->actualModule))){

                if (as.verbose) {
                    CStr buffer = "";
                    char* bufferc = list->m_assignmentList->getValueString(&buffer);
                    //int len = strlen(bufferc);
                    dbgPrintf(TINFO, "XML emit Keyword: %s = %s\n", list->m_keywordName.buf, bufferc);
                }
				if (list->m_anType == ANTYPE_TAG){
					CXMLTag* newElement = m_actual->addSubTag(list->m_anName.buf, list->m_tagMaxOccurs == 1, list->m_tagSelectExisting, list->m_keywordName.buf, &list->m_parent);
					if (newElement) {
						// update tab and lineLength for printout
						newElement->m_removeRedundantTags = list->m_removeRedundantTags;
						newElement->m_tabSize = (long) getAnnotationDataLong(anData, cnt, ANGROUP_XMLGEN, ANFUNCTION_XML_TAB_SIZE, DEFAULT_TABSIZE);
						newElement->m_lineLength = (long) getAnnotationDataLong(anData, cnt, ANGROUP_XMLGEN, ANFUNCTION_XML_LINE_LENGTH, DEFAULT_MAXLINELENGTH);
						newElement->m_attributeLength = (long) getAnnotationDataLong(anData, cnt, ANGROUP_XMLGEN, ANFUNCTION_XML_ATTRIBUTE_LENGTH, DEFAULT_MAXLINELENGTH);
						if (list->m_newParent){
							m_actual = newElement;
						}
					}
					else {
						//int dbg = 0;
					}
				}
				else if (list->m_anType == ANTYPE_CONTROL) {
					switch (list->m_anFunction) {
					case ANFUCNTION_CTRL_SAVEACTIVETAG:
                        saveActiveTag(list, TAG_SOURCE_CNTRL);
                        break;
					case ANFUCNTION_CTRL_RESTOREACTIVETAG:
                        newActual = restoreActiveTag(list, TAG_SOURCE_CNTRL);
						break;
					case ANFUCNTION_CTRL_SETACTIVETAG:
						newActual = findParent(&list->m_assignmentList->m_string);
						if (newActual){
							m_actual = newActual;
						}
						break;
					}

				}
				else if (list->m_anType == ANTYPE_ATTRIBUTE){
					// attributes
					if (strcmp(list->m_keywordName.buf, "h2xmlm_name") == 0){
						//int dbg = 0;
					}
					//if (list->m_showInOutput){
					CXMLTag* addTo = m_actual;
					if (!list->m_parent.isEmpty()) {
						addTo = m_actual->findParent(&list->m_parent);
					}
					if (addTo) {
						if (list->m_showInTag ){
							//int dbg = CStringList::findInList(list->m_showInTag, addTo->m_keyWord.buf);
						}

						if (list->m_showInTag == NULL || CStringList::findInList(list->m_showInTag, addTo->m_keyWord.buf)){

							CAnnotationData* newData = addTo->m_attributes->findFirstAnnotation(list);
							if (!newData){
								newData = addToEnd(lNEXT, addTo->m_attributes);
							}
							else {
								//int dbg = 0;
							}

							newData->copyData(list);

                            if (newData->m_assignmentList->m_argType == ANARG_REFERENCE) {
                                newData->m_assignmentList->resolveReferences(newData);
                            }

							copyCnt++;
						}
					}
				}
			}
			else {
				//int dbg = 0;
			}
			list = list->get(lNEXT);
		}
	}
	return m_actual;
}

void CXMLTag::xmlLinePrintf(int tabs, int lineLength, int attributeLength, FILE* fp, char* line)
{
	static int m_pos = 0;					// actual position
	static bool m_bol = true;				// begin of line
	static bool m_inString = false;		// is inside comment?
	static int lenNl = strlen("\n");		// length of newLine

	bool wasBol = false;
	if (m_bol && *line) {
		fprintf(fp, "%*.0s", tabs, "\t");
		m_pos = tabs;
		m_bol = false;
		wasBol = true;
	}

	// search for next appropriate line break
	char* check = line;
	int lastSpace = -1;
	bool inStringLastSpace = m_inString;		// is inside comment?

	int cnt = 0;
	while (*check) {
		if (*check == ' ') {
			lastSpace = cnt;
			inStringLastSpace = m_inString;
		}
		if (m_inString && *check == '\"') {
			m_inString = false;
			//fprintf(fp, "STRING: %d %d", m_inString, tmpcnt);
		}else if (!m_inString && *(check) == '\"') {
			m_inString = true;
			//fprintf(fp, "STRING: %d %d", m_inString, tmpcnt);
		}
		// newline
		if (check == strstr(check, "\n")) {
			cnt += lenNl;
			fprintf(fp, "%s", line);
#ifdef _DEBUG
			fflush(fp);
#endif
			m_bol = true;
			xmlLinePrintf(tabs, lineLength, attributeLength, fp, check + lenNl);
			break;
		}

		// check for linelength exceeded
		if ((!m_inString && (cnt + m_pos > lineLength)) || (m_inString && (cnt + m_pos > attributeLength)) ||
			(m_inString && (cnt + m_pos > lineLength) && (lastSpace>0) && !inStringLastSpace)) {
			if (lastSpace == 0) {
				m_inString = inStringLastSpace;
				if (wasBol){
					xmlLinePrintf(tabs, lineLength, attributeLength, fp, line + lastSpace + 1);
				}
				else {
                    if (m_inString) {
                       fprintf(fp, "#[");
                    }
                    fprintf(fp, "\n");
#ifdef _DEBUG
					fflush(fp);
#endif
                    m_bol = true;
                    if (m_inString) {
                        xmlLinePrintf(tabs, lineLength, attributeLength, fp, "#] ");
                    }
					xmlLinePrintf(tabs, lineLength, attributeLength, fp, line + lastSpace + 1);
					break;
				}

			}
			else if (lastSpace>0) {
				if (inStringLastSpace){
					m_inString = inStringLastSpace;
					fprintf(fp, "%.*s#[\n", lastSpace, line);
#ifdef _DEBUG
					fflush(fp);
#endif
					m_bol = true;
					xmlLinePrintf(tabs, lineLength, attributeLength, fp, "#] ");
					xmlLinePrintf(tabs, lineLength, attributeLength, fp, line + lastSpace + 1);
					break;
				}
				else
				{
					m_inString = inStringLastSpace;
					fprintf(fp, "%.*s\n", lastSpace, line);
#ifdef _DEBUG
					fflush(fp);
#endif
					m_bol = true;
					xmlLinePrintf(tabs, lineLength, attributeLength, fp, line + lastSpace + 1);
					break;
				}
			}
			else if (m_inString) {
				fprintf(fp, "%.*s#[\n", cnt, line);
#ifdef _DEBUG
				fflush(fp);
#endif
				m_bol = true;
				xmlLinePrintf(tabs, lineLength, attributeLength, fp, "#]");
				xmlLinePrintf(tabs, lineLength, attributeLength, fp, line + cnt);
				break;

			}

		}

		check++;
		cnt++;
	}

    // check for '"' in last character
#if 0
    if (cnt > 0) {
        if (m_inString && *(check - 1) == '\"') {
            m_inString = false;
        }
        if (!m_inString && *(check - 1) == '\"') {
            m_inString = true;
        }
    }
#endif   
    if (!*check) {
		m_pos += cnt;
		fprintf(fp, "%s", line);
#ifdef _DEBUG
		fflush(fp);
#endif
	}
}


void CXMLTag::xmlPrintf(int tabs, int lineLength, int attributeLength, FILE* fp, char* format, ...)
{

	char line[MAX_XML_STRSIZE*2];
	va_list ap;
	va_start(ap, format);
	vsprintf_s(line, MAX_XML_STRSIZE * 2-1, format, ap);
	xmlLinePrintf(tabs, lineLength, attributeLength, fp, line);
}
