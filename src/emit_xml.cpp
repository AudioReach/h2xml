/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

#include "emit_xml.h"
#include "scanAnnotations.h"
#include "scanConfig.h"
#include "CXmlTag.h"
#include "CAnnotationData.h"
#include <float.h>

#define MAX_ARRAY_DIMENSIONS 100
static bool getNextArray(int dim, long* max, long *cnt);
CDefaultList defaultListEmpty;
//CIdentifier identBitfield;

//#define EXPAND_DEFAULT_ARRAYS

void CIdentifier::emitEnum(CXMLTag* xmlTag, eAnGroups anGroup, CStr * nspace, int mergedTagCnt,  bool firstcall)
{
    xmlTag->saveActiveTag(m_xD[anGroup], TAG_SOURCE_ELEMENT);

    if (!firstcall) {
        pushAnPt(gd->configAction[anGroup], ++mergedTagCnt);
        pushAnPt(m_xD, ++mergedTagCnt);
        pushAnPt(NULL, ++mergedTagCnt);

    }

    addInternalData(gd->mergedTags, mergedTagCnt, anGroup, ANFUNCTION_NAME, getshortname(anGroup), 0);
    addInternalData(gd->mergedTags, mergedTagCnt, anGroup, ANFUNCTION_ELEMENT_NAME, getshortname(ANGROUP_NONE), 0);

    // merge attributes and settings with previous
    if (m_isStruct) {
        CIdentifier *dt_elements = getDataType()->GetElements();
        CStr name = *nspace;
        addname(&name, anGroup);
       
        for (CIdListIterator actual_ident = dt_elements->GetIdList_Begin();
            actual_ident != dt_elements->GetIdList_End();
            actual_ident++) {
            if (actual_ident->m_cvdt){
                actual_ident->emitEnum(xmlTag, anGroup, &name, mergedTagCnt, false);
            }
        }
    }
    else {
        if (m_cvdt){
            CValue initvalue;
            initvalue.setLong((long64)m_cvdt->data, m_cvdt->valstr.buf, ANARG_INTEGER);
            addInternalData(gd->mergedTags, mergedTagCnt, anGroup, ANFUNCTION_INITVALUE, NULL, 0, &initvalue);

            // init name
            CAnnotationData* an = findLastAnnotationData(gd->mergedTags, mergedTagCnt, anGroup, ANFUNCTION_INITREFERENCE);
            if (an){
                if (an->m_assignmentList->m_string.buf[0] != '@') {
                    an->errorPrnt(TERROR, "Reference annotation '%s' must start with '@'\n", an->m_assignmentList->m_string.buf);
                }
                else {
                    if (!m_cvdt->enumName.isEmpty()){
                        CValue reference;
                        CAnnotationData* anNew = addInternalData(gd->mergedTags, mergedTagCnt, anGroup, ANFUNCTION_INITREFERENCE, NULL, 0, &reference);
                        anNew->m_assignmentList->m_argType = ANARG_REFERENCE;
                        anNew->m_assignmentList->m_string = m_cvdt->enumName + an->m_assignmentList->m_string;
                    }
                    else {
						CStr fileName = m_fileName;
						fileName.ReplaceAll("\\", "/");
                        dbgPrintf(TERROR, "%s(%d): Initializer value '%s' is not an enum value\n",fileName.buf,m_linecnt, m_cvdt->valstr.buf);
                    }
                }
            }
        }
        xmlTag->addAttribute(gd->mergedTags, mergedTagCnt, anGroup);
    }
    xmlTag->restoreActiveTag(m_xD[anGroup], TAG_SOURCE_ELEMENT);
}


void CIdentifier::emitData(CXMLTag* xmlTag, eAnGroups anGroup, long &address, long &addressNoBitfield, CStr * nspace, char* typeName, int mergedTagCnt, CDefaultList* ptDefaultList, bool firstcall, int compileAttributes)
{
    xmlTag->saveActiveTag(m_xD[anGroup], TAG_SOURCE_EMITDATA);
    long identTypeId = GetTypeId();


    // merge attributes and settings with previous
    if (!firstcall) {
        pushAnPt(gd->configAction[anGroup], ++mergedTagCnt);
        pushAnPt(m_xD, ++mergedTagCnt);
        pushAnPt(NULL, ++mergedTagCnt);
    }
    compileAttributes = compileAttributes | m_attribute;

    // general flags
    m_anExpandArray = (long) getAnnotationDataLong(gd->mergedTags, mergedTagCnt, ANGROUP_XMLGEN, ANFUNCTION_EXPAND_ARRAY, DEFAULT_EXPANDARRAY);
    m_anExpandArrayOffset = (long) getAnnotationDataLong(gd->mergedTags, mergedTagCnt, ANGROUP_XMLGEN, ANFUNCTION_EXPAND_ARRAY_OFFSET, DEFAULT_EXPAND_ARRAY_OFFSET);
    m_anExpandStructs = (long) getAnnotationDataLong(gd->mergedTags, mergedTagCnt, ANGROUP_XMLGEN, ANFUNCTION_EXPAND_STRUCTS, DEFAULT_EXPANDSTRUCTS);
    m_anExpandTypedefs = (long) getAnnotationDataLong(gd->mergedTags, mergedTagCnt, ANGROUP_XMLGEN, ANFUNCTION_EXPAND_TYPEDEFS, DEFAULT_EXPANDTYPEDEFS);

    long anExpandStructs = m_anExpandStructs;
    long anExpandArray = m_anExpandArray;

    if (firstcall){
        anExpandStructs = true;
    }

    // Align address
    long oldaddress = address;
    if (GetTypeId() != TYPE_STRUCT && GetTypeId() != TYPE_UNION) {
        if (m_bitslice != -1){
            m_addr_increment = m_bitslice;
        }
        else {
            address = ((address + m_addr_increment - 1) / m_addr_increment)*m_addr_increment;
            //addressNoBitfield = address;
        }
    }
    else {

        if (m_align > 0) {
            address = ((address + m_align - 1) / m_align)*m_align;
            //addressNoBitfield = address;
        }
        else {
        }
    }

    // m_align address + evaluate bitslice field
    if (compileAttributes & COMPILE_ATTR_PACKED) {
        long newaddress = ((oldaddress + 8 - 1) / 8) * 8;
        address = newaddress;
    }


	// total number of elements in m_elements
    long maxElemements[MAX_ARRAY_DIMENSIONS];
    long cnt[MAX_ARRAY_DIMENSIONS];
	char arraystr[1000];
	char arrayDimstr[1000];
	array_dim_struct *ad = m_array_dim;

	// initialize dimension string for XML output
	int i = 0;
	arrayDimstr[0] = 0;

	// initialize dimension arrays
    if (m_arrayElements == 0 || m_hasConditionalDefaultFile) {
        anExpandArray = false;
    }
    sprintf(arrayDimstr, "%ld", m_arrayElements);


    // initialize array dimension table
    for (i = 0; i < m_array_dimension && i < MAX_ARRAY_DIMENSIONS; i++) {
        if (m_array_dimension == 1){
            maxElemements[i] = m_arrayElements;         // may be modified if unique default string
        }
        else {
            maxElemements[i] = ad->dim;
        }
        cnt[i] = 0;
        ad = ad->next;
    }
    maxElemements[i] = -1;
    cnt[i] = 0;
        

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// initialize element specific parameters
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	// Find base element type
	char dataTypeName[10000];
	CIdentifier* ident = this;
	if (m_anExpandTypedefs) {
		while (ident){
			if (ident->m_declspec && ident->m_declspec->datatype && ident->m_declspec->datatype->m_cvdt &&  ident->m_declspec->datatype->m_cvdt->identifier) {
				ident = ident->m_declspec->datatype->m_cvdt->identifier;
			}
			else {
				break;
			}
		}
	}
	bool isNotBaseType = ident->m_declspec && ident->m_declspec->datatype && ident->m_declspec->datatype->m_cvdt &&  ident->m_declspec->datatype->m_cvdt->identifier;


	if (ident->getDataType2() && !isNotBaseType) {
		sprintf(dataTypeName, "%s %s", ident->getDataType2()->GetTypeNameStr().buf, ident->getDataType()->GetTypeShortNameStr().buf);
	}
	else {
		sprintf(dataTypeName, "%s", ident->getDataType()->GetTypeShortNameStr().buf);
	}

    // always Expand unnamed structs
    if (dataTypeName[0] == '\0'){
        anExpandStructs = true;
    }

    addInternalDataLocal(gd->mergedTags, mergedTagCnt, anGroup, ANFUNCTION_ELEMENTTYPE, dataTypeName, 0);

    // set unsigned annotation
    if (m_isUnsigned) {
        CAnnotationData* annotationSigned = addInternalDataLocal(gd->mergedTags, mergedTagCnt, anGroup, ANFUNCTION_ELEMENTTYPE_UNSIGNED, "true", 0, 0);
        if (annotationSigned) {
            annotationSigned->m_showInOutput = true;
        }
    }

    CAnnotationData* ptAnRangeMin = addInternalDataLocal(gd->mergedTags, mergedTagCnt, anGroup, ANFUNCTION_RANGE_MIN, NULL, 0, &m_typeMin);
    if (ptAnRangeMin && m_isStruct) {
        ptAnRangeMin->m_showInOutput = false;  
    }

    CAnnotationData* ptAnRangeMax = addInternalDataLocal(gd->mergedTags, mergedTagCnt, anGroup, ANFUNCTION_RANGE_MAX, NULL, 0, &m_typeMax);
    if (ptAnRangeMax && m_isStruct) {
        ptAnRangeMax->m_showInOutput = false;
    }
   
	CAnnotationData* ptAnArrayElements = addInternalDataLocal(gd->mergedTags, mergedTagCnt, anGroup, ANFUNCTION_ARRAYELEMENTS, arrayDimstr, 0);
    if (ptAnArrayElements && (!m_isArray || anExpandArray)){
        ptAnArrayElements->m_showInOutput = false;
	}

    addInternalDataLocal(gd->mergedTags, mergedTagCnt, anGroup, ANFUNCTION_BYTESIZE, NULL, m_bitSizeElement / 8);
    addInternalDataLocal(gd->mergedTags, mergedTagCnt, anGroup, ANFUNCTION_BITESIZE, NULL, m_bitSizeElement);
	CAnnotationData* anByteOffset= addInternalDataLocal(gd->mergedTags, mergedTagCnt, anGroup, ANFUNCTION_BYTEOFFSET, NULL, address / 8);
	CAnnotationData* anBitOffset= addInternalDataLocal(gd->mergedTags, mergedTagCnt, anGroup, ANFUNCTION_BITOFFSET, NULL, address);
	CAnnotationData* anName = addInternalDataLocal(gd->mergedTags, mergedTagCnt, anGroup, ANFUNCTION_NAME, getshortname(anGroup), 0);
    addInternalDataLocal(gd->mergedTags, mergedTagCnt, anGroup, ANFUNCTION_ELEMENT_NAME, getshortname(ANGROUP_NONE), 0);


	// save original name, modify with namespace
	CStr baseName = "";
	CStr fullName = "";
	if (anName) {
		baseName = anName->m_assignmentList->m_string;
		fullName = baseName.buf;
		char elementName[1000] = "";
		if (!nspace->isEmpty()) {
			_snprintf(elementName, 999, "%s_%s", nspace->buf, baseName.buf);
			anName->m_assignmentList->m_string = elementName;
			fullName = elementName;
		}
	}

 
    CDefaultList *ptDefaultListLocal = ptDefaultList;
    if (ptDefaultListLocal == NULL && !m_hasConditionalDefaultFile) {
        ptDefaultListLocal=m_defaultList;
    }


    // no single default if default file
    CAnnotationData* ptAnDefault = copyInternalDataLocal(gd->mergedTags, mergedTagCnt, anGroup, ANFUNCTION_DEFAULT);

    if (ptAnDefault){
        ptAnDefault->signExtendtoElementtype(m_isUnsigned, identTypeId, m_bitSizeElement);

        if (m_hasDefaultFile){
            if (!((m_isArray && anExpandArray) || (m_isStruct && anExpandStructs))) {
                ptAnDefault->m_showInOutput = false;
            }
        }
        if (m_isStruct) {
            ptAnDefault->m_showInOutput = false;
        }
    }

    // default array buffer
    CAnnotationData* ptAnArrayDefault = NULL;
    if (!m_hasConditionalDefaultFile){
        ptAnArrayDefault=copyInternalDataLocal(gd->mergedTags, mergedTagCnt, anGroup, ANFUNCTION_ARRAY_DEFAULT);
        if (ptAnArrayDefault) {
            if ((m_isArray && anExpandArray) || (m_isStruct && anExpandStructs)) {
                ptAnArrayDefault->m_showInOutput = false;
            }
        }
    }

    // no default values for bitfields
    if (ptAnDefault && m_xD[ANGROUP_BITFIELD] && m_xD[ANGROUP_BITFIELD]->get(lNEXT)) {
        ptAnDefault->m_showInOutput = false;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // Emit Arrays if not expanded
    //////////////////////////////////////////////////////////////////////////////////////////////////////

    long addressStart = address;
    long addressEnd = addressStart + m_bitSizeElement*m_arrayElements;
    bool expandArray = m_isArray && anExpandArray;
    if (m_isArray && !anExpandArray) {
        anExpandStructs = false;
    }

    // show or hide arraysize 
    if (ptAnArrayElements) {
        if (m_isArray && !anExpandArray) {
            ptAnArrayElements->m_showInOutput = true;
        }
        else {
            ptAnArrayElements->m_showInOutput = false;
        }
    }


    do { // for all array entries
        arraystr[0] = 0;
        if (expandArray){ // generate array appendix string
            if (m_array_dimension >= 1) {
                for (int i = m_array_dimension - 1; i >= 0; i--) {
                    int index = cnt[i];
                    index += m_anExpandArrayOffset;						//indices in documentation start from 1
                    sprintf(arraystr, "%s_%d", arraystr, index);
                }
            }
        }
        long startaddress = address;
        if (!anExpandStructs) {
            //int dbg = 0;
        }
        if (m_isStruct && anExpandStructs) { //struct or union, but no pointer to struct, recursively emit all elements
            CStr name = *nspace;

            if (!firstcall) {
                addname(&name, anGroup);
                if (strstr(name.buf, "<%") == NULL) {
                    name = name + arraystr;
                }
            }

            CIdentifier *dt_elements = getDataType()->GetElements();
            long maxaddress = 0; 
            for (CIdListIterator actual_ident = dt_elements->GetIdList_Begin();
                actual_ident != dt_elements->GetIdList_End();
                actual_ident++) {
                if (actual_ident->m_cvdt){
                    if (GetTypeId() == TYPE_UNION) {
                        address = startaddress;
                    }
                    actual_ident->emitData(xmlTag, anGroup, address, addressNoBitfield, &name, typeName, mergedTagCnt, ptDefaultListLocal, false, compileAttributes);
                    if (address > maxaddress) maxaddress = address;
                }
            }
            address = maxaddress;
        }
        else {

            if (anName && (arraystr[0] != 0)) {
                char elementName[1000] = "";
                _snprintf(elementName, 999, "%s%s", fullName.buf, arraystr);
                anName->m_assignmentList->m_string = elementName;
            }

            // modify byteOffset
            if (anByteOffset){
                anByteOffset->setUIntValue(startaddress / 8);
            }

            if (anBitOffset){
                anBitOffset->setUIntValue(startaddress);
            }

            // add init value
            if (m_cvdt){
                CValue initvalue;
                initvalue.setLong((long64)m_cvdt->data, m_cvdt->valstr.buf, ANARG_INTEGER);
                addInternalDataLocal(gd->mergedTags, mergedTagCnt, anGroup, ANFUNCTION_INITVALUE, NULL, 0, &initvalue);
            }

            // add default value
            if (ptDefaultListLocal && ptDefaultListLocal->m_type == DEFAULT_TYPE_LIST) {
                if (m_isStruct || (m_isArray && !expandArray )) {
                    CStr elementDefaultString = "";
                    char defaultStr[20] = "";
                    int bytesize = m_addr_increment / 8;
                    if (!expandArray) {
                        bytesize *= m_arrayElements;
                    }
                    if (m_arrayElements == 0){
                        bytesize = ptDefaultListLocal->m_defaultBufferSize;
                    }
                    for (int i = 0; i < bytesize; i++) {
                        sprintf(defaultStr, "%2.2x", ptDefaultListLocal->m_defaultBuffer[(address - ptDefaultListLocal->m_startAddress) / 8 + i]);
                        elementDefaultString.stradd(defaultStr);
                    }
                    if (!elementDefaultString.isEmpty()) {
                        if (ptAnArrayDefault) {
                            ptAnArrayDefault->m_assignmentList->m_string = elementDefaultString;
                            ptAnArrayDefault->m_showInOutput = true;
                        }
                        if (ptAnDefault) {
                            ptAnDefault->m_showInOutput = false;
                        }
                    }
                }
                else {
                    if (ptAnDefault && ptAnDefault->m_showInOutput && ((unsigned) m_bitSizeElement <= sizeof(long long)* 8)) {
                        ptAnDefault->m_assignmentList->m_value1.l = 0;
                        ptAnDefault->m_assignmentList->m_value1.ul=getBitSlice(ptDefaultListLocal->m_defaultBuffer, address - ptDefaultListLocal->m_startAddress, m_bitSizeElement);
                        ptAnDefault->m_assignmentList->m_value1.format = ANARG_UNSIGNED_INTEGER;
                        if (identTypeId == TYPE_FLOAT) {
                            ptAnDefault->m_assignmentList->m_value1.format = ANARG_FLOAT32;
                        } else if (identTypeId == TYPE_DOUBLE) {
                            ptAnDefault->m_assignmentList->m_value1.format = ANARG_DOUBLE;
                        }
                        ptAnDefault->signExtendtoElementtype(m_isUnsigned, identTypeId, m_bitSizeElement);
                        ptAnDefault->m_showInOutput = true;
                        if (ptAnArrayDefault){
                            ptAnArrayDefault->m_showInOutput = false;
                        }
                    }
                }
            }
            CStr fullInstanceName = typeName;
            fullInstanceName = fullInstanceName + "_" + fullName.buf;
            xmlTag->addAttribute(gd->mergedTags, mergedTagCnt, anGroup, fullInstanceName.buf);

 
            CIdentifier* identBitfield = m_identBitField;
            while (identBitfield) {
                long newAddress = startaddress + identBitfield->m_bitoffset;
                identBitfield->emitData(xmlTag, ANGROUP_ELEMENT, newAddress, addressNoBitfield, nspace, typeName, mergedTagCnt, ptDefaultListLocal, firstcall, compileAttributes);
                identBitfield = identBitfield->m_next;
            }


            address = startaddress + m_addr_increment;
        }
    } while (expandArray && getNextArray(m_array_dimension, maxElemements, cnt));

    address = addressEnd;

    xmlTag->restoreActiveTag(m_xD[anGroup], TAG_SOURCE_EMITDATA);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void CIdentifier::readDefaultFile(eAnGroups anGroup, CDefaultList * &ptDefaultList, long address, long mergedTagCnt, bool &m_hasConditionalDefaultFile, bool &m_hasDefaultFile)
{
    eAnFunction functionList[] = { ANFUNCTION_DEFAULTLIST, ANFUNCTION_DEFAULTLIST_CONDITIONAL, ANFUNCTION_DEFAULTFILE, ANFUNCTION_DEFAULTFILE_CONDITIONAL };
    Cvdt* incdirs = as.include_dirs;
    CAssignmentList *list = NULL;
    int listByteCnt = 0;
    long identTypeId = GetTypeId();
    m_hasConditionalDefaultFile = false;
    m_hasDefaultFile = false;
    int startcnt = 0;
    int bytesize;
    eAnArgTypes defaultDataType = ANARG_UNSIGNED_INTEGER;
    while (1) {
        CStr elementDefaultString = "";
        CAnnotationData* annotationDefaultFile = m_xD[anGroup]->findFirstAnnotationFunctionList(functionList, sizeof(functionList) / sizeof(eAnFunction), startcnt);
        if (annotationDefaultFile) {

            startcnt = CAnnotationData::m_countFind;
            FILE * pFile = NULL;
            size_t lSize = 0;
            size_t result = 0;
            unsigned char* defaultBuffer = NULL;

            switch (annotationDefaultFile->m_anFunction) {
            case ANFUNCTION_DEFAULTFILE_CONDITIONAL:
                m_hasConditionalDefaultFile = true;
            case ANFUNCTION_DEFAULTFILE:
                if (annotationDefaultFile->m_anFunction == ANFUNCTION_DEFAULTFILE){
                    m_hasDefaultFile = true;
                }

                // check all include paths
                incdirs = as.include_dirs;
                while (incdirs != NULL) {
                    CStr inputFile = incdirs->name + annotationDefaultFile->m_assignmentList->m_string.buf;
                    inputFile.AdjustForUnix();
                    if ((pFile = fopen(inputFile.buf, "rb")) == NULL) {
                        incdirs = incdirs->next;
                    }
                    else {
                        incdirs = NULL;
                    }
                }

                if (pFile) {
                    // obtain file size:
                    fseek(pFile, 0, SEEK_END);
                    lSize = ftell(pFile);
                    rewind(pFile);
                    defaultBuffer = (unsigned char*)malloc(sizeof(char)*lSize);
                    if (defaultBuffer == NULL) {
                        annotationDefaultFile->errorPrnt(TERROR, "Default file '%s' Memory Error\n", annotationDefaultFile->m_assignmentList->m_string.buf);
                        break;
                    }
                    result = fread(defaultBuffer, 1, lSize, pFile);
                    if (result != lSize) {
                        annotationDefaultFile->errorPrnt(TERROR, "Error reading default file '%s'\n", annotationDefaultFile->m_assignmentList->m_string.buf);
                        free(defaultBuffer);
                        defaultBuffer = NULL;
                    }
                    fclose(pFile);
                }
                else {
                    annotationDefaultFile->errorPrnt(TERROR, "Cannot open default file '%s'\n", annotationDefaultFile->m_assignmentList->m_string.buf);
                    break;
                }
                break;
            case ANFUNCTION_DEFAULTLIST_CONDITIONAL:
                m_hasConditionalDefaultFile = true;
            case ANFUNCTION_DEFAULTLIST:
                if (annotationDefaultFile->m_anFunction == ANFUNCTION_DEFAULTLIST){
                    m_hasDefaultFile = true;
                }
                // obtain file size
                list = annotationDefaultFile->m_assignmentList;
                bytesize = m_bitSizeElement / 8;
                if (m_isStruct) {
                    bytesize = 4;
                }
                if (bytesize > 8) {
                    bytesize = 8;
                }
                while (list){
                    list = list->get(lNEXT);
                    lSize += bytesize;
                }
                defaultBuffer = (unsigned char*)malloc(sizeof(char)*lSize);
                if (defaultBuffer == NULL) {
                    annotationDefaultFile->errorPrnt(TERROR, "Default list '%s' Memory Error\n", annotationDefaultFile->m_assignmentList->m_string.buf);
                    break;
                }
                // copy values into defaultBuffer
                list = annotationDefaultFile->m_assignmentList;
                listByteCnt = 0;
                while (list){
                    int bytesize = m_bitSizeElement / 8;
                    if (m_isStruct) {
                        bytesize = 4;
                    }
                    if (bytesize > 8) {
                        bytesize = 8;
                    }
                    CValue tmp;
                    tmp.l = list->m_value1.l;
                    tmp.format = list->m_value1.format;
                    tmp.convertByteTypes(identTypeId);
                    defaultDataType = tmp.format;
                    for (int i = 0; i < bytesize; i++) {
                        defaultBuffer[listByteCnt] = tmp.c[i];
                        listByteCnt++;
                    }
                    list = list->get(lNEXT);
                }
                break;
            default:
                break;
            }

            // check correct size in case of fixed size array
            if (m_arraySize > 0) {
                if (lSize * 8 != (unsigned)m_arraySize) {
                    annotationDefaultFile->errorPrnt(TERROR, "Default file size (%d bytes) != array size (%d bytes)\n", lSize, m_arraySize / 8);
                }
            }
            else {
                // check if size is multiple of element size
                if (lSize * 8 % m_bitSizeElement != 0) {
                    annotationDefaultFile->errorPrnt(TERROR, "Default file size (%d bytes) must be multiple of element size (%d bytes)\n", lSize, m_bitSizeElement / 8);
                }
            }

            if (defaultBuffer){
                CDefaultList* actualDefaultList = addToEnd(lNEXT, ptDefaultList);
                actualDefaultList->m_type = DEFAULT_TYPE_LIST;
                actualDefaultList->m_startAddress = address;
                actualDefaultList->m_defaultBufferSize = lSize;
                actualDefaultList->m_defaultBuffer = defaultBuffer;
                actualDefaultList->m_anData = annotationDefaultFile;
                actualDefaultList->m_defaultBufferBitSize = lSize*8;
                actualDefaultList->m_format = defaultDataType;

                // update element default string
                char defaultStr[20] = "";
                for (size_t i = 0; i < lSize; i++) {
                    sprintf(defaultStr, "%2.2x", actualDefaultList->m_defaultBuffer[i]);
                    elementDefaultString.stradd(defaultStr);
                }
                if (!elementDefaultString.isEmpty()) {
                    CAnnotationData* arrayDefault = m_xD[anGroup]->findFirstAnnotationFunction(ANFUNCTION_ARRAY_DEFAULT, startcnt);
                    if (arrayDefault) {
                        startcnt = CAnnotationData::m_countFind;
                        arrayDefault->m_assignmentList->m_string = elementDefaultString;
                        arrayDefault->m_showInOutput = true;
                    }
                }
            }
        }
        else {
            break;
        }
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////
// Check various ranges
//  1. If list values, check if values within type range
//  2. for all conditional defaults
//          if (array && !expandArray && defaultList) for all array elements
//              if (struct && !expandStruct
//  2. for all default values
//         
//////////////////////////////////////////////////////////////////////////////////////////////
bool CIdentifier::checkRanges(eAnGroups anGroup, long &address, int mergedTagCnt, CDefaultList* ptDefaultList, bool firstcall, int compileAttributes)
{
    bool retval = true;
    long identTypeId = GetTypeId();
    compileAttributes = compileAttributes | m_attribute;
    // debugging breakpoint only ...
    if (m_cvdt != NULL && m_cvdt->shortname.buf != NULL && strcmp(m_cvdt->shortname.buf, "pFirstFunction") == 0) {
        //int dbg = 0;
    }

    // merge attributes and settings with previous
    if (!firstcall) {
        pushAnPt(gd->configAction[anGroup], ++mergedTagCnt);
        pushAnPt(m_xD, ++mergedTagCnt);
    }


    m_addr_increment = GetTypeSize() * 8;
    m_indirection = m_pointer_level - m_array_dimension;
    m_align = m_declspec->datatype->m_align * 8;

    if (m_align <= 0 || m_align>8 * 8) {
        //int dbg = 0;											// debug breakpoint
    }
    if (m_indirection>0) {
        m_addr_increment = as.pointerSize * 8;					// pointer size in bits, (default 32 bits)
        m_align = as.pointerSize * 8;
    }
    if (identTypeId == TYPE_ENUM) {
        m_addr_increment = as.enumSize * 8;						// pointer size in bits, (default 32 bits)
        m_align = as.enumSize * 8;
    }

    long oldaddress = address;
    if (identTypeId != TYPE_STRUCT && identTypeId != TYPE_UNION) {
        if (m_bitslice != -1){
            m_addr_increment = m_bitslice;
        }
        else {
            address = ((address + m_addr_increment - 1) / m_addr_increment)*m_addr_increment;
            //addressNoBitfield = address;
        }
    }
    else {

        if (m_align > 0) {
            address = ((address + m_align - 1) / m_align)*m_align;
            //addressNoBitfield = address;
        }
        else {
        }
    }

    // m_align address + evaluate bitslice field
    if (compileAttributes & COMPILE_ATTR_PACKED) {
        long newaddress = ((oldaddress + 8 - 1) / 8) * 8;
        if (newaddress != address && gd->warnPackedData) {
            CAnnotationData* an = findLastAnnotationData(gd->mergedTags, mergedTagCnt, ANGROUP_STRUCTURE, ANFUNCTION_NAME);
            if (an && an->m_fileName) {
                an->errorPrnt(TWARNING, "packed data address missmatch (-WPacked): packed (0x%x), unpacked (0x%x)\n", newaddress, address);
            }
        }
        address = newaddress;
    }

    m_isArray = false;       // true, if array but m_anExpandArray=false

    m_bitSizeElement = m_addr_increment;
    m_arrayElements = 1;
    m_arraySize = m_addr_increment;

    if (m_array_dimension > 0){
        array_dim_struct *adtmp = m_array_dim;
        for (int i = 0; i < m_array_dimension; i++) {
            m_arrayElements *= adtmp->dim;
            adtmp = adtmp->next;
        }
        // in case array is not expanded
        m_arraySize = m_addr_increment * m_arrayElements;
        m_isArray = true;
    }

    m_isStruct = false;
    if ((identTypeId == TYPE_STRUCT || identTypeId == TYPE_UNION) && m_indirection <= 0) { //struct or union, but no pointer to struct
        m_isStruct = true;
    }

    // Data type
    m_isUnsigned = false;
    CAnnotationData *ptAnUnsigned = m_xD[ANGROUP_ELEMENT]->findLastAnnotation(ANFUNCTION_ELEMENTTYPE_UNSIGNED);
    if (ptAnUnsigned) {
        if (ptAnUnsigned->m_assignmentList->m_value1.ul) {
            m_isUnsigned = true;
        }
    }
    else {
        CDataType* tsign = getDataType2();
        if (tsign&& tsign->GetTypeId() == TYPE_UNSIGNED) {
            m_isUnsigned = true;
        }
    }




    m_typeMin.ul = 0;
    m_typeMax.ul = 0;

    // check if Min and Max range within type size
    CAnnotationData* ptAnRangeMin = NULL;
    CAnnotationData* ptAnRangeMax = NULL;
    if (!m_isStruct) {
        int type = identTypeId;
        switch (type) {
        case TYPE_FLOAT:
            m_typeMin.f = -FLT_MAX;
            m_typeMax.f = FLT_MAX;
            m_typeMin.format = ANARG_DOUBLE;
            m_typeMax.format = ANARG_DOUBLE;
            break;
        case TYPE_DOUBLE:
            m_typeMin.f = -DBL_MAX;
            m_typeMax.f = DBL_MAX;
            m_typeMin.format = ANARG_DOUBLE;
            m_typeMax.format = ANARG_DOUBLE;
            break;
        default:
            if (m_isUnsigned) {
                m_typeMin.ul = 0;
                m_typeMax.ul = 0xffffffffffffffff >> (0x40 - m_bitSizeElement);
                m_typeMin.format = ANARG_UNSIGNED_INTEGER;
                m_typeMax.format = ANARG_UNSIGNED_INTEGER;
            }
            else {
                m_typeMin.ul = 0xffffffffffffffff << (m_bitSizeElement - 1);
                m_typeMax.ul = 0x7fffffffffffffff >> (0x40 - m_bitSizeElement);
                m_typeMin.format = ANARG_INTEGER;
                m_typeMax.format = ANARG_INTEGER;
            }
            break;
        }

        m_typeMin.m_bitSize = m_bitSizeElement;
        m_typeMax.m_bitSize = m_bitSizeElement;

        /* make a copy so the range value can be adjusted for element type */
        ptAnRangeMin = copyInternalData(gd->mergedTags, mergedTagCnt, anGroup, ANFUNCTION_RANGE_MIN);
        if (ptAnRangeMin) {
            checkRangeType(ptAnRangeMin);
            ptAnRangeMin->signExtendtoElementtype(m_isUnsigned, identTypeId, m_bitSizeElement);
            if (ptAnRangeMin->m_assignmentList->m_value1.lt(&m_typeMin)) {
                char buf1[100], buf2[100];
                ptAnRangeMin->errorPrnt(TERROR, "Lower range '%s' < type range '%s'\n", ptAnRangeMin->m_assignmentList->m_value1.getValuStr(buf1), m_typeMin.getValuStr(buf2));
            }
        }

        /* make a copy so the range value can be adjusted for element type */
        ptAnRangeMax = copyInternalData(gd->mergedTags, mergedTagCnt, anGroup, ANFUNCTION_RANGE_MAX);
        if (ptAnRangeMax) {
            checkRangeType(ptAnRangeMax);
            ptAnRangeMax->signExtendtoElementtype(m_isUnsigned, identTypeId, m_bitSizeElement);
            if (ptAnRangeMax->m_assignmentList->m_value1.gt(&m_typeMax)) {
                char buf1[100], buf2[100];
                ptAnRangeMax->errorPrnt(TERROR, "Upper range '%s' > type range '%s'\n", ptAnRangeMax->m_assignmentList->m_value1.getValuStr(buf1), m_typeMax.getValuStr(buf2));
            }
        }

        /* local copy of default value*/
        CAnnotationData* ptAnDefault = copyInternalData(gd->mergedTags, mergedTagCnt, ANGROUP_ELEMENT, ANFUNCTION_DEFAULT);
        if (ptAnDefault) {
            ptAnRangeMax->signExtendtoElementtype(m_isUnsigned, identTypeId, m_bitSizeElement);
        }

        /* set to element specific value */
        if (ptAnRangeMin && ptAnRangeMin->m_isDefault) {
            ptAnRangeMin->m_assignmentList->m_value1 = m_typeMin;
        }

        if (ptAnRangeMax && ptAnRangeMax->m_isDefault) {
            ptAnRangeMax->m_assignmentList->m_value1 = m_typeMax;
        }

    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // Sign extention for conditional default values
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    long startcnt = 0;
    while (1) {
        CAnnotationData* annotationDefault = m_xD[anGroup]->findFirstAnnotationFunction(ANFUNCTION_DEFAULT_CONDITIONAL, startcnt);
        if (annotationDefault) {
            annotationDefault->signExtendtoElementtype(m_isUnsigned, identTypeId, m_bitSizeElement);
            startcnt = CAnnotationData::m_countFind;
        }
        else {
            break;
        }
    }

#if 0
    CAnnotationData* ptDefault = findLastAnnotationData(gd->mergedTags, mergedTagCnt, anGroup, ANFUNCTION_DEFAULT);
    CAnnotationData anDefaultSave;
    anDefaultSave.copyData(ptDefault);
    if (ptDefault) {
        checkRangeType(ptDefault);
        ptDefault->signExtendtoElementtype(m_isUnsigned, identTypeId, m_bitSizeElement);
    }

#endif

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // check range values within type size
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    long lastRangeValue = 0;
    CAnnotationData* ptRangeValue = NULL;
    CAssignmentList* rangeListValues = NULL;
    do {
        ptRangeValue = findLastAnnotationData(gd->mergedTags, mergedTagCnt, anGroup, ANFUNCTION_RANGE_VALUE, &lastRangeValue, lastRangeValue);
        if (ptRangeValue) {
            ptRangeValue->signExtendtoElementtype(m_isUnsigned, identTypeId, m_bitSizeElement);
            CAssignmentList* newRange = addToEnd(lNEXT, rangeListValues);

            if (ptRangeValue->m_assignmentList->m_argType == ANARG_REFERENCE) {
                ptRangeValue->m_assignmentList->resolveReferences(ptRangeValue);
            }
            newRange->m_value1 = ptRangeValue->m_assignmentList->m_value1;
            newRange->m_argType = ptRangeValue->m_assignmentList->m_argType;
            newRange->m_pointer = ptRangeValue;
            // check min / max value
            char buf1[100], buf2[100];
            if (ptRangeValue->m_assignmentList->m_value1.lt(&m_typeMin)) {
                ptRangeValue->errorPrnt(TERROR, "List value '%s' < type range '%s'\n", ptRangeValue->m_assignmentList->m_value1.getValuStr(buf1), m_typeMin.getValuStr(buf2));
            }
            if (ptRangeValue->m_assignmentList->m_value1.gt(&m_typeMax)) {
                ptRangeValue->errorPrnt(TERROR, "List value '%s' > type range '%s'\n", ptRangeValue->m_assignmentList->m_value1.getValuStr(buf1), m_typeMax.getValuStr(buf2));
            }
        }
    } while (ptRangeValue);



    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // Read default files
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    m_hasConditionalDefaultFile = false;
    m_hasDefaultFile = false;

    CDefaultList *ptDefaultListLocal = ptDefaultList;
    if (ptDefaultListLocal == NULL) {
        readDefaultFile(anGroup, ptDefaultListLocal, address, mergedTagCnt, m_hasConditionalDefaultFile, m_hasDefaultFile);
        m_defaultList = ptDefaultListLocal;
    }

    // only if default list was not already passed as argument
    if (!ptDefaultList){
        // update number of array elements for zero size arrays if size is unambigously defined by a single default file
#ifdef EXPAND_DEFAULT_ARRAYS
        if (m_isArray && m_arrayElements == 0 && !m_hasConditionalDefaultFile && m_hasDefaultFile) {
            m_arrayElements = ptDefaultListLocal->m_defaultBufferBitSize / m_bitSizeElement;
        }
#endif       
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // add condition defaults to default list
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        startcnt = 0;
        eAnFunction functionList[] = { ANFUNCTION_DEFAULT_CONDITIONAL };
        while (1) {
            CAnnotationData* annotationDefault = m_xD[anGroup]->findFirstAnnotationFunctionList(functionList, sizeof(functionList) / sizeof(eAnFunction), startcnt);
            if (annotationDefault && annotationDefault->m_showInOutput) {
                startcnt = CAnnotationData::m_countFind;
                annotationDefault->signExtendtoElementtype(m_isUnsigned, identTypeId, m_bitSizeElement);
                CDefaultList* newDefault = addToList(lNEXT, ptDefaultListLocal);
                newDefault->m_type = DEFAULT_TYPE_VALUE;
                newDefault->m_anData = annotationDefault;
            }
            else {
                break;
            }
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // add unconditional defaults to default list
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        if (!m_hasDefaultFile){
            CAnnotationData* annotationDefault = findLastAnnotationData(gd->mergedTags, mergedTagCnt, anGroup, ANFUNCTION_DEFAULT);

            if (annotationDefault && annotationDefault->m_showInOutput) {
                checkRangeType(annotationDefault);
                annotationDefault->signExtendtoElementtype(m_isUnsigned, identTypeId, m_bitSizeElement);
                CDefaultList* newDefault = addToList(lNEXT, ptDefaultListLocal);
                newDefault->m_type = DEFAULT_TYPE_VALUE;
                newDefault->m_anData = annotationDefault;
            }
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // if no default at all
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        if (!ptDefaultListLocal) {
            ptDefaultListLocal = &defaultListEmpty;
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // check default lists
    ///////////////////////////////////////////////////////////////////////////

    if (ptDefaultListLocal == NULL) {
        ptDefaultListLocal = &defaultListEmpty;
    }

    // initialize default value
    CAnnotationData AnDefaultFill;          // write actual list default value to this item
    AnDefaultFill.copyData(ptAnRangeMin);   // just as template
    AnDefaultFill.m_assignmentList->m_value1.dataStr = "";
    AnDefaultFill.m_isDefault = false;

    long addressStart = address;
    long addressEnd = addressStart + m_bitSizeElement*m_arrayElements;

   CDefaultList* defaultList = ptDefaultListLocal;
    while (defaultList) {
        if (defaultList->m_anData){
            AnDefaultFill.m_fileName = defaultList->m_anData->m_fileName;
            AnDefaultFill.m_linecnt = defaultList->m_anData->m_linecnt;
        }
        else {
            AnDefaultFill.m_fileName = "";
            AnDefaultFill.m_linecnt = 0;
        }
        address = addressStart;
        long arrayElementsList = 1;
        if (m_isArray) {
            if (defaultList->m_type == DEFAULT_TYPE_LIST) {
                arrayElementsList = m_arrayElements;
                if (m_arrayElements == 0 && !ptDefaultList){
                    arrayElementsList = defaultList->m_defaultBufferBitSize / m_bitSizeElement;
                }
            }
        }

        for (int arraycnt = 0; arraycnt < arrayElementsList;arraycnt++) {
            long startaddress = address;
            if (m_isStruct) { //struct or union, but no pointer to struct, recursively emit all elements

                // only propagate default a single default list
                CDefaultList newList;
                CDefaultList* ptNewList = NULL;
                if (defaultList->m_type == DEFAULT_TYPE_LIST) {
                    newList.copy(defaultList);
                    ptNewList = &newList;
                }

                CIdentifier *dt_elements = getDataType()->GetElements();
                long maxaddress = 0; 
                for (CIdListIterator actual_ident = dt_elements->GetIdList_Begin();
                    actual_ident != dt_elements->GetIdList_End();
                    actual_ident++) {

                    if (m_cvdt != NULL){
                        if (identTypeId == TYPE_UNION) {
                            address = startaddress;
                        }
                    }

                    actual_ident->checkRanges(anGroup, address, mergedTagCnt, ptNewList, false, compileAttributes);
                    if (address > maxaddress) maxaddress = address;
                }
                address = maxaddress;

                // update in case of packed! Dirty fix ...
                if (compileAttributes & COMPILE_ATTR_PACKED) {
                    m_bitSizeElement = maxaddress - startaddress;
                    addressEnd = addressStart + m_bitSizeElement*m_arrayElements;
                }

            }
            else {

                // add default value
                CAnnotationData* ptDefault = NULL;
                if (defaultList->m_type==DEFAULT_TYPE_LIST) {
                    AnDefaultFill.m_assignmentList->m_value1.ul = getBitSlice(defaultList->m_defaultBuffer, address - defaultList->m_startAddress, m_bitSizeElement);
                    //AnDefaultFill.m_assignmentList->m_value1.format = defaultList->m_format;
                    AnDefaultFill.m_assignmentList->m_value1.format = ANARG_UNSIGNED_INTEGER;
                    if (identTypeId == TYPE_FLOAT) {
                        AnDefaultFill.m_assignmentList->m_value1.format = ANARG_FLOAT32;
                    }
                    else if (identTypeId == TYPE_DOUBLE) {
                        AnDefaultFill.m_assignmentList->m_value1.format = ANARG_DOUBLE;
                    }
                    AnDefaultFill.signExtendtoElementtype(m_isUnsigned, identTypeId, m_bitSizeElement);
                    ptDefault = &AnDefaultFill;
                }
                else if (defaultList->m_type == DEFAULT_TYPE_VALUE){
                    ptDefault = defaultList->m_anData;
                }

                if (ptDefault){
                    static int cnt = 0;
                    cnt++;

                    if (cnt == 0x21) {
                        //int dbg=0;
                    }
                      ptDefault->checkRange(ptAnRangeMin, ptAnRangeMax, rangeListValues);
                }

                // for all bitfields

                long startcnt = 0;
                CIdentifier** actualIdent = &m_identBitField;

                while (1) {
                    CAnnotationData* ptAnBitfieldStart = m_xD[ANGROUP_BITFIELD]->findFirstAnnotationFunction(ANFUNCTION_ALIAS_BITFIELD, startcnt);
                    if (ptAnBitfieldStart) {
                        startcnt = CAnnotationData::m_countFind;
                        CAnnotationData* ptAnBitfieldEnd = m_xD[ANGROUP_BITFIELD]->findFirstAnnotationFunction(ANFUNCTION_ALIAS_BITFIELD_END, startcnt);
                        if (ptAnBitfieldEnd) {
                            startcnt = CAnnotationData::m_countFind;
                            CAnnotationData* ptAnBitfield = ptAnBitfieldStart;
                            CIdentifier* identBitfield = new(CIdentifier);
                            *actualIdent = identBitfield;
                            actualIdent = &identBitfield->m_next;
                            identBitfield->m_xD[ANGROUP_ELEMENT] = NULL;
                            addAnFunction(identBitfield->m_xD[ANGROUP_ELEMENT], ANFUCNTION_CTRL_BREAK);
                            while (ptAnBitfield && ptAnBitfield != ptAnBitfieldEnd){
                                CAnnotationData *newAnData = addToEnd(lNEXT, identBitfield->m_xD[ANGROUP_ELEMENT]);
                                newAnData->copyData(ptAnBitfield);
                                ptAnBitfield = ptAnBitfield->get(lNEXT);
                            }
                            CAnnotationData *ptAnMask = identBitfield->m_xD[ANGROUP_ELEMENT]->findLastAnnotation(ANFUNCTION_BITMASK);
                            if (ptAnMask) {
                                identBitfield->m_declspec = m_declspec;
                                ulong64 mask = ptAnMask->m_assignmentList->m_value1.ul;
                                identBitfield->m_bitoffset = getTrailingZeros(mask);
                                int bitstart = 63 - getLeadingZeros(mask);
                                identBitfield->m_bitslice = bitstart - identBitfield->m_bitoffset + 1;
                                long newAddress = startaddress + identBitfield->m_bitoffset;

                                CDefaultList newList;
                                CDefaultList* ptNewList = NULL;
                                if (defaultList->m_type == DEFAULT_TYPE_LIST) {
                                    newList.copy(defaultList);
                                    ptNewList = &newList;
                                }
                                identBitfield->checkRanges(anGroup, newAddress, mergedTagCnt, ptNewList, false, compileAttributes);
                            }
                        }
                        else {
                            break;
                        }
                    }
                    else {
                        break;
                    }
                }

                address = startaddress + m_addr_increment;
            }
        }
        defaultList = defaultList->get(lNEXT);
    }
    
    address = addressEnd;

    return retval;
}

void CIdentifier::addname(CStr* name, eAnGroups xmlTag)
{
	char* append = getshortname(xmlTag);
	if (append[0] != 0 && name->buf[0] != 0) {
		*name = *name + "_";
	}
	*name = *name + append;
}

char* CIdentifier::getshortname(eAnGroups xmlTag)
{
	static char fill[] = "";

	if (m_xD[xmlTag]) {
		CAnnotationData* nameTag = m_xD[xmlTag]->findLastAnnotation(ANFUNCTION_NAME);
		if (nameTag && !nameTag->m_isDefault && !nameTag->m_assignmentList->m_string.isEmpty()) {
			return nameTag->m_assignmentList->m_string.buf;
		}
	}
	
		
	if (m_cvdt != NULL){
		return m_cvdt->shortname.buf;						// return variable name
	}

    if (getDataType()->GetTypeShortNameStr().buf == NULL) {
        //int dbg = 0;
    }

    if (getDataType()) {
        return getDataType()->GetTypeShortNameStr().buf;
    }
	
	

	return fill;
}

/* check if element type is compatible with data type */
bool CIdentifier::checkRangeType(CAnnotationData* data)
{
    bool retval = true;
    int elementType = GetTypeId();
    int dataType = data->m_assignmentList->m_value1.format;
    
    switch (elementType) {
    case TYPE_FLOAT:
    case TYPE_DOUBLE:
        break;
    default:
        if (dataType == ANARG_DOUBLE) {
            retval = false;
        }
    }

    if (retval == false) {
        data->errorPrnt(TERROR, "element of type <%s> is initialized with type <%s>\n", GetTypeName(), anArgNames[dataType]);

    }
    return retval;
}

bool getNextArray(int dim, long* maxElemements, long *cnt) {

	cnt[0]++;
	if (cnt[0] >= maxElemements[0]) {
		cnt[0] = 0;
		if (dim>1) {
			return getNextArray(dim - 1, maxElemements + 1, cnt + 1);
		}
		else {
			return false;
		}
	}
	return true;
}


