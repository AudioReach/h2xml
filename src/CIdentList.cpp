/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

#include "CModuleList.h"
#include "CIdentList.h"
#include "CXmlTag.h"
#include "CAnnotationData.h"

//////////////////////////////////////////////////////////////////////////////
// Class CIdentList
//////////////////////////////////////////////////////////////////////////////
CIdentList::CIdentList()
{
	m_ident = NULL;
};

CIdentList::~CIdentList()
{

}

CIdentList* CIdentList::addElementToEnd(CIdentList* &start, CIdentifier* ident) {
	CIdentList* list = start;
	CIdentList* found = NULL;
	while (list && ident!=NULL) {
		if (list->m_ident && list->m_ident == ident) {
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
void CIdentList::emitAll(CXMLTag* xmlTag, CIdentList* identList, int mergedTagCnt)
{
	while (identList) {
		identList->emit(xmlTag, mergedTagCnt);
		identList = identList->get(lNEXT);
	}
}

// one listelement
void CIdentList::emit(CXMLTag* xmlTag, int mergedTagCnt)
{
	if (m_ident){
        xmlTag->saveActiveTag(NULL, TAG_SOURCE_STRUCT);

        pushAnPt(gd->configAction[ANGROUP_STRUCTURE], ++mergedTagCnt);
		pushAnPt(m_ident->m_xD, ++mergedTagCnt);

		int indirection = m_ident->m_pointer_level - m_ident->m_array_dimension;

		if (m_ident->m_declspec == NULL) { // empty parameter, no payload, only message
			xmlTag->addAttribute(gd->mergedTags, mergedTagCnt, ANGROUP_STRUCTURE);
		} else if (m_ident->GetTypeId() == TYPE_ENUM) {
#if 0
			CStr nspace = "";
			long address = 0;
            long addressNoBitfield = 0;
			CDefaultList *defaultData=NULL;
            m_ident->emitData(xmlTag, ANGROUP_ELEMENT, address, addressNoBitfield, &nspace, mergedTagCnt, defaultData, true, 0);
#endif

            CIdentifier *dt_elements = m_ident->getDataType()->GetElements();
            if (dt_elements != NULL) {

                if (checkGenerateOutput(gd->mergedTags, mergedTagCnt, ANGROUP_STRUCTURE)) {
					addInternalData(gd->mergedTags, mergedTagCnt, ANGROUP_STRUCTURE, ANFUNCTION_NAME, m_ident->getshortname(ANGROUP_STRUCTURE), 0);
                    addInternalData(gd->mergedTags, mergedTagCnt, ANGROUP_STRUCTURE, ANFUNCTION_STRUCT_NAME, m_ident->getshortname(ANGROUP_NONE), 0);
                    CXMLTag* newElement = xmlTag->addAttribute(gd->mergedTags, mergedTagCnt, ANGROUP_STRUCTURE);
                    CStr nspace = "";
                    m_ident->m_isStruct = true;
                    m_ident->emitEnum(newElement, ANGROUP_ELEMENT, &nspace, mergedTagCnt, true);
                }
            }
		}
		else if ((m_ident->GetTypeId() == TYPE_STRUCT || m_ident->GetTypeId() == TYPE_UNION || m_ident->GetTypeId() == TYPE_ENUM) && indirection <= 0) {

			CIdentifier *dt_elements = m_ident->getDataType()->GetElements();
			if (dt_elements != NULL) {

				if (checkGenerateOutput(gd->mergedTags, mergedTagCnt, ANGROUP_STRUCTURE)) {

					CStr nspace = "";
					long address = 0;
                    long addressNoBitfield = 0;

					// Insert Compiler generated data
					pushAnPt(NULL, ++mergedTagCnt);
					CAnnotationData* name=addInternalData(gd->mergedTags, mergedTagCnt, ANGROUP_STRUCTURE, ANFUNCTION_NAME, m_ident->getshortname(ANGROUP_STRUCTURE), 0);
					addInternalData(gd->mergedTags, mergedTagCnt, ANGROUP_STRUCTURE, ANFUNCTION_STRUCT_NAME, m_ident->getshortname(ANGROUP_NONE), 0);

                    bool hasZeroSizeArrays = false;
					if (m_ident->m_declspec && m_ident->m_declspec->datatype) {
						addInternalData(gd->mergedTags, mergedTagCnt, ANGROUP_STRUCTURE, ANFUNCTION_BITALIGN, NULL, m_ident->m_declspec->datatype->m_align*8);
                        if (m_ident->m_declspec->datatype->m_hasZeroSizeArrays) {
                            hasZeroSizeArrays = true;
                        }
					}
					if ((m_ident->m_attribute & COMPILE_ATTR_PACKED)) {
						addInternalData(gd->mergedTags, mergedTagCnt, ANGROUP_STRUCTURE, ANFUNCTION_PACKING, NULL, true);
					}

                    int typeSize = m_ident->GetTypeSize();
                    if (hasZeroSizeArrays) {
                        typeSize = 0;
                    }
                    addInternalData(gd->mergedTags, mergedTagCnt, ANGROUP_STRUCTURE, ANFUNCTION_BYTESIZE, NULL, typeSize);
                    addInternalData(gd->mergedTags, mergedTagCnt, ANGROUP_STRUCTURE, ANFUNCTION_BITESIZE, NULL, typeSize * 8);

					CXMLTag* newElement = xmlTag->addAttribute(gd->mergedTags, mergedTagCnt, ANGROUP_STRUCTURE);

					CDefaultList *defaultData=NULL;
                    m_ident->checkRanges(ANGROUP_ELEMENT, address, mergedTagCnt, NULL, true, 0);

                    address = 0;
                    char* typeName = NULL;
                    if (m_ident->m_cvdt) {
                        typeName=m_ident->m_cvdt->shortname;
                    }
                    if (typeName==NULL && m_ident->getDataType()) {
                        typeName= m_ident->getDataType()->GetTypeShortNameStr().buf;
                    }

                    m_ident->emitData(newElement, ANGROUP_ELEMENT, address, addressNoBitfield, &nspace, typeName, mergedTagCnt, defaultData, true, 0);

					// add byte size
					if ((m_ident->m_attribute & COMPILE_ATTR_PACKED)) {

						CAnnotationData* byteSize = newElement->m_attributes->findLastAnnotation(ANFUNCTION_BYTESIZE);
						if (byteSize){
							long size = ((address + 7) / 8);
							if (gd->warnPackedData && byteSize->m_assignmentList->m_value1.l != size && name && name->m_fileName) {
								name->errorPrnt(TWARNING, "packed byte size missmatch (-WPacked): packed (0x%x), unpacked (0x%x)\n", size, byteSize->m_assignmentList->m_value1.l);
							}
							byteSize->setIntValue(size);
						}

						CAnnotationData* biteSize = newElement->m_attributes->findLastAnnotation(ANFUNCTION_BITESIZE);
						if (biteSize){
							biteSize->setIntValue(address);
						}
					}
				}
			}

		}
		else{
			//int dbg = 0;
		}
        xmlTag->restoreActiveTag(NULL, TAG_SOURCE_STRUCT);
	}
}



