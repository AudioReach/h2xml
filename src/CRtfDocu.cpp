/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

// RTF spec in: http://www.biblioscape.com/rtf15_spec.htm


#include "CRtfDocu.h"
#include "scanConfig.h"
#include "CXmlTag.h"
#include "CAnnotationData.h"

#if 0

// RTF templates
char rtfHeader[] =
"{\\rtf1\\ansi\\ansicpg1252\\deff0\\deflang1033\\deflangfe1033{\\fonttbl{\\f0\\froman\\fprq2\\fcharset0 Times New Roman;}\n"
"{\\f1\\fswiss\\fprq2\\fcharset0 Arial;}{\\f2\\fmodern\\fprq1\\fcharset0 Courier New;}}\n"
"{\\colortbl ;\\red0\\green0\\blue0;\\red36\\green64\\blue97;\\red0\\green255\\blue255;\\red155\\green211\\blue221;}\n"
"{\\stylesheet{ Normal;}{\\s1 heading 1;}{\\s2 heading 2;}}\n"
"{\\*\\generator Msftedit 5.41.21.2510;}\\viewkind4\\uc1\\pard\\sa200\\sl276\\slmult1\\f0\\fs24 \\par\n"
"\n[@h2xml_RTF_MODULE]:10\n\n"		// Modules
"\\pard\\li720\\sb120\\sa120\\tx2160\\fs22 \n"
"\\par\n"
"}\n"
;


char rtfModule[] =
"\\pard\\brdrb\\brdrs\\brdrw45\\brsp100 \\keep\\keepn\\s1\\sb480\\sa1160\\cf1\\b\\f1\\fs48 1 \\tab\\cf0\\b0 Module [@h2xmlm_name]\\f0\\par\n"			
"\\pard\\li720\\sb120\\sa120\\tx2160\\fs22 [@h2xmlm_description]\\par\n"																	
"\n[@h2xml_RTF_PARAM]:20\n\n"		// Paramter
;

char rtfParameter[] =
"\\pard\\keep\\keepn\\s2\\fi-360\\li360\\sb320\\cf1\\b\\f1\\fs32 1.1 \\tab\\cf0  Parameter [@h2xmlp_name]\\par\n"
"\\pard\\li720\\sb120\\sa120\\tx2160\\b0\\f0\\fs22 [@h2xmlp_description]\\par\n"
"\\par\n"
"\n"
"\\trowd\\trgaph10\\trleft-10\\trpaddl10\\trpaddr10\\trpaddfl3\\trpaddfr3\n"
"\\clcbpat4\\clbrdrl\\brdrw15\\brdrs\\clbrdrt\\brdrw15\\brdrs\\clbrdrb\\brdrw15\\brdrs \n"
"\\cellx2542\\clcbpat4\\clbrdrt\\brdrw15\\brdrs\\clbrdrb\\brdrw15\\brdrs \n"
"\\cellx4540\\clcbpat4\\clbrdrt\\brdrw15\\brdrs\\clbrdrb\\brdrw15\\brdrs \n"
"\\cellx5990\\clcbpat4\\clbrdrt\\brdrw15\\brdrs\\clbrdrr\\brdrw15\\brdrs\\clbrdrb\\brdrw15\\brdrs \n"
"\\cellx9490\\pard\\intbl\\nowidctlpar\\li50\\cf1\\b\\fs20 Name:\\b0\\par\n"
"\\b ID:\\par\n"
"Size:\\b0\\cell [@h2xmlp_name]\\par\n"
"[@h2xmlp_id]\\par\n"
"[@h2xmlp_bytesize]\\cell\\b   \\b0\\cell\\cell\\row\\trowd\\trgaph10\\trleft-10\\trpaddl10\\trpaddr10\\trpaddfl3\\trpaddfr3\n"
"\\clcbpat4\\clbrdrl\\brdrw15\\brdrs\\clbrdrt\\brdrw15\\brdrs\\clbrdrr\\brdrw15\\brdrs\\clbrdrb\\brdrw15\\brdrs \n"
"\\cellx2490\\clcbpat4\\clbrdrl\\brdrw15\\brdrs\\clbrdrt\\brdrw15\\brdrs\\clbrdrr\\brdrw15\\brdrs\\clbrdrb\\brdrw15\\brdrs \n"
"\\cellx3540\\clcbpat4\\clbrdrl\\brdrw15\\brdrs\\clbrdrt\\brdrw15\\brdrs\\clbrdrr\\brdrw15\\brdrs\\clbrdrb\\brdrw15\\brdrs \n"
"\\cellx4040\\clcbpat4\\clbrdrl\\brdrw15\\brdrs\\clbrdrt\\brdrw15\\brdrs\\clbrdrr\\brdrw15\\brdrs\\clbrdrb\\brdrw15\\brdrs \n"
"\\cellx9490\\pard\\intbl\\nowidctlpar\\li50\\fs20 Name\\cell Address (hex)\\cell\\pard\\intbl\\nowidctlpar\\li50\\qc Dir\\cell\\pard\\intbl\\nowidctlpar\\li50 Description\\cell\\row\n"
"\n[@h2xml_RTF_ELEMENT]:30\n\n"
;

char rtfElement[] =
"\\trowd\\trgaph10\\trleft-10\\trpaddl10\\trpaddr10\\trpaddfl3\\trpaddfr3\n"
"\\clbrdrl\\brdrw15\\brdrs\\clbrdrt\\brdrw15\\brdrs\\clbrdrr\\brdrw15\\brdrs\\clbrdrb\\brdrw15\\brdrs \n"
"\\cellx2490\\clbrdrl\\brdrw15\\brdrs\\clbrdrt\\brdrw15\\brdrs\\clbrdrr\\brdrw15\\brdrs\\clbrdrb\\brdrw15\\brdrs \n"
"\\cellx3540\\clbrdrl\\brdrw15\\brdrs\\clbrdrt\\brdrw15\\brdrs\\clbrdrr\\brdrw15\\brdrs\\clbrdrb\\brdrw15\\brdrs \n"
"\\cellx4040\\clbrdrl\\brdrw15\\brdrs\\clbrdrt\\brdrw15\\brdrs\\clbrdrr\\brdrw15\\brdrs\\clbrdrb\\brdrw15\\brdrs \n"
"\\cellx9490\\pard\\intbl\\nowidctlpar\\li50\\cf2 [@h2xmle_name]\n"
"\\cell [@h2xmle_byteOffset]\n"
"\\cell\\pard\\intbl\\nowidctlpar\\li50\\qc\\cf2\\f0 [@h2xmle_readOnly]:31\n"
"\\f0\\cell\\pard\\intbl\\nowidctlpar\\li50\\cf2 [@h2xmle_description]\\par\\par\\cf2\\f0\\b Default:\\b0\\tab [@h2xmle_default]:33\\par\\cf2\\f0\n" 
"\\b Range: \\b0\\tab [@h2xmle_range]:32\\par\n"
"\\f0\\cell\\row\n"
;

#endif

/****************************************************************************
* Methods for CStringPartList
****************************************************************************/
CStringPart::CStringPart()
{
	m_string="";
	m_keyWord="";
	m_printType=0;
	m_parent = NULL;
}

CStringPart::~CStringPart(){

}

void CStringPart::emit(FILE* fp, CXMLTag* xml)
{
	fprintf(fp, "%s", m_string.buf);
	CAnnotationData* an, *an2;
	CXMLTag* newXml, *bitFieldXml;
	CStr buffer, buffer2;

	switch (m_printType){
    case 0:{
               // simply print attribute value
               char* tagKey = strtok(m_keyWord.buf, "@");
               char* attributeKey = strtok(NULL, "@");
               if (attributeKey == NULL) {
                   attributeKey = tagKey;
                   tagKey = NULL;
               }
               newXml = xml;
               if (tagKey){
                   newXml = xml->find(tagKey, 0, false);
               }
               if (newXml && attributeKey){
                   an = newXml->m_attributes->findFirstKeyWord(attributeKey);
                   if (an) {
                       an->m_assignmentList->getValueString(&buffer);
                       buffer.ReplaceAll("\\n", "\\par");
                       fprintf(fp, "%s", buffer.buf);
                   }
               }
               break; }
	case 1:
		// XML List
		newXml = xml->find(m_keyWord.buf, 0, false);
		if (newXml){
			CXMLTag* rangeXML = newXml->m_sub;
			while (rangeXML) {
				an = rangeXML->m_attributes;
				bool first = true;
				bool print = false;
				while (an){
					if (an->m_showInOutput){
						an->m_assignmentList->getValueString(&buffer);
						if (!first) {
							fprintf(fp, "\\tab ");
						}
						fprintf(fp, "%s", buffer.buf);
						print = true;
						first = false;
					}
					an = an->get(lNEXT);

				}
				rangeXML = rangeXML->get(lNEXT);
				if (print && rangeXML) {
					fprintf(fp, "\\par ");
				}
			}
		}
		break;
	case 10:
		// for all modules
		newXml = xml->find("h2xmlm_TAG_Module",0,false);
		if (newXml) {
			while (newXml) {
				m_parent->m_part[RTF_MODULE].emit(fp, newXml);
				newXml = newXml->get(lNEXT);
			}
		}
		break;
	case 20:
		// for all Parameters
		newXml = xml->find("h2xmlp_TAG_PARAMETER", 0, false);
		if (newXml) {
			while (newXml) {
				m_parent->m_part[RTF_PARAM].emit(fp, newXml);
				newXml = newXml->get(lNEXT);
			}
		}
		break;
	case 30:
		// for all Elements
		newXml = xml->find("h2xmle_TAG_ELEMENT", 0, false);
		if (newXml) {
			while (newXml) {
				if (strcmp(newXml->m_keyWord.buf, "h2xmle_TAG_ELEMENT") == 0){
					m_parent->m_part[RTF_ELEMENT].emit(fp, newXml);
					bitFieldXml = newXml->m_sub;
					while (bitFieldXml){
						if (strcmp(bitFieldXml->m_keyWord.buf, "h2xmle_TAG_BITFIELD") == 0){
							m_parent->m_part[RTF_ELEMENT].emit(fp, bitFieldXml);
						}
						bitFieldXml = bitFieldXml->get(lNEXT);
					}
				}
				newXml = newXml->get(lNEXT);
			}
		}
		break;
	case 31:
		// readOnly
		an = xml->m_attributes->findFirstKeyWord(m_keyWord.buf);
		if (an && an->m_assignmentList->m_value1.l) {
			fprintf(fp, "R");
		}
		else {
			fprintf(fp, "RW");
		}
		break;

	case 32:
		// range
		newXml = NULL;
		if (xml->m_sub) {
			newXml = xml->m_sub->find("h2xmle_TAG_RANGE_LIST", 0, false);
		}
		if (newXml){
			CXMLTag* rangeXML = newXml->m_sub;
			while (rangeXML) {
				bool print = false;
				an = rangeXML->m_attributes->findFirstKeyWord("h2xmle_rangeLabel");
				if (an){
					an2 = rangeXML->m_attributes->findFirstKeyWord("h2xmle_rangeValue");
					if (an2) {
						an->m_assignmentList->getValueString(&buffer);
						an2->m_assignmentList->getValueString(&buffer2);
						fprintf(fp, "%s\\tab %s", buffer2.buf, buffer.buf);
						print = true;
					}
				}

				rangeXML = rangeXML->get(lNEXT);
				if (rangeXML && print) {
					fprintf(fp, "\\par ");
				}

			}
		}
		else {
			an = xml->m_attributes->findFirstKeyWord("h2xmle_rangeMin");
			if (an && an->m_showInOutput){
				an2 = xml->m_attributes->findFirstKeyWord("h2xmle_rangeMax");
				if (an2 && an2->m_showInOutput) {
					an->m_assignmentList->getValueString(&buffer);
					an2->m_assignmentList->getValueString(&buffer2);
					fprintf(fp, "%s..%s",buffer.buf,buffer2.buf);
				}
			}
		}
		break;
	case 33:
		// default
		an = xml->m_attributes->findFirstKeyWord(m_keyWord.buf);
		if (an) {
			if (an->m_showInOutput) {
				an->m_assignmentList->getValueString(&buffer);
				fprintf(fp, "%s", buffer.buf);
			}
		}
		else {
			fprintf(fp, "0x%x", 0);
		}
		break;
	case 34:
		// address
        // Bitfield
        an = xml->m_attributes->findFirstKeyWord("h2xmle_bitMask");
        if (an) {
            ulong64 mask = an->m_assignmentList->m_value1.ul;
            newXml = xml->find("h2xmle_TAG_ELEMENT", 1, false);
            an = newXml->m_attributes->findFirstKeyWord(m_keyWord.buf);
            if (an){
                unsigned long addr = (long) an->m_assignmentList->m_value1.ul;
                int bitend = getTrailingZeros(mask);
                int bitstart = 63 - getLeadingZeros(mask);
                fprintf(fp, "0x%2.2lx [%d:%d]", addr, bitstart, bitend);
            }
        }
        else {
            an = xml->m_attributes->findFirstKeyWord(m_keyWord.buf);
            if (an) {
                unsigned long addr = (long) an->m_assignmentList->m_value1.ul;
                an = xml->m_attributes->findFirstKeyWord("h2xmle_byteSize");
                if (an){
                    unsigned long size = (long) an->m_assignmentList->m_value1.ul;
                    if (size > 0) {
                        fprintf(fp, "0x%2.2lx [%lu:%d]", addr, size * 8 - 1, (int)0);
                    }
                    else {
                        fprintf(fp, "0x%2.2lx ", addr);
                    }
                }
            }
        }
		break;
	case 40:
		// for all Bitfields
		newXml = xml->find(m_keyWord.buf, 0, false);
		if (newXml) {
			while (newXml) {
				m_parent->m_part[RTF_ELEMENT].emit(fp, newXml);
				newXml = newXml->get(lNEXT);
			}
		}
		break;
	default:
		dbgPrintf(TERROR, "RTF print: unsupported printType <%d>\n", m_printType);
		break;
	};
}



/****************************************************************************
* Methods for CStringPartList
****************************************************************************/
CStringPartList::CStringPartList()
{
	m_list = NULL;
    m_keyword = CStr("[@") + gd->config->m_globalPrefix;
}

CStringPartList::~CStringPartList()
{
}

void CStringPartList::emit(FILE* fp, CXMLTag* xml)
{

	CStringPart* list = m_list;
	while (list){
		list->emit(fp,xml);
		list = list->get(lNEXT);
	}
}

void CStringPartList::part(char* string) 
{

	CStringPart* strpart = addToEnd(lNEXT, m_list);
	char* foundEnd;
	char* found = CRtfDocu::rtfFind(string, m_keyword.buf, foundEnd);

	if (found) {
		char* end = CRtfDocu::rtfFind(foundEnd, "]", foundEnd);
		if (end) {
			*end = 0;
			CStr keyWord="";
			CRtfDocu::rtfGetString(found + 1, &keyWord);	// without [
			if (keyWord.buf){
				strpart->m_keyWord = keyWord.buf + 1;			//without @
			}

			// extract type
			char* type = strrchr(strpart->m_keyWord.buf, ':');
			if (type){
				*type = 0;
				strpart->m_printType = atol(type + 1);
			}

			end++;
			if (end && *end!=0) {
				part(end);
			}
		}
		*found = 0;
	}

	strpart->m_string = string;
	strpart->m_parent = m_parent;

}


/****************************************************************************
* Methods for CStringPartList
****************************************************************************/
CRtfDocu::CRtfDocu()
{
	for (int i= 0; i < knRtfParts; i++) {
		m_part[i].m_parent = this;
	}
}

CRtfDocu::~CRtfDocu()
{

}


int CRtfDocu::emit(CStr* filename, CXMLTag* xml)
{
    if (xml) {
        FILE* fp;
        if ((fp = fopen(filename->buf, "w")) == NULL){
            dbgPrintf(TERROR, "RTF generator: cannot open RTF file %s\n", filename->buf);
            return RETVAL_ERROR;
        }

        m_part[RTF_HEADER].emit(fp, xml);  // start with global

        fclose(fp);
    }
	return RETVAL_OK;
}

int CRtfDocu::parseRef(CStr* filename)
{




#if 1
	FILE* fpRef;
	if ((fpRef = fopen(filename->buf, "rb")) == NULL){
		dbgPrintf(TERROR, "RTF generator: cannot open reference file %s\n", filename->buf);
		return RETVAL_ERROR;
	}

	CStr infile;
	infile.readFile(fpRef);
	fclose(fpRef);


	// cut into separate portions;
	CStr rtfHeader;
	CStr rtfModule;
	CStr rtfParameter;
	CStr rtfElement;


	try {
		// header
		char* moduleStart = cutCheck(infile.buf, "[H2XML_MODULE_START]");
		rtfHeader = infile.buf;
		char* moduleEnd = cutCheck(moduleStart, "[H2XML_MODULE_END]");
		rtfHeader = rtfHeader + "[@h2xml_RTF_MODULE:10] " + moduleEnd;

		// module
		char* paramStart = cutCheck(moduleStart, "[H2XML_PARAMETER_START]");
		rtfModule = moduleStart;
		char* paramEnd = cutCheck(paramStart, "[H2XML_PARAMETER_END]");
		rtfModule = rtfModule + "[@h2xml_RTF_PARAM:20] " + paramEnd;

		// parameter
		char* elementStart = cutCheck(paramStart, "[H2XML_ELEMENT_START]");
		char* elementStartRow = cutCheck(paramStart, "\\trow",true,true);				// start of row
		char* elementEnd = cutCheck(elementStart, "[H2XML_ELEMENT_END]");
		char* elementEndRow = cutCheck(elementEnd, "\\row",true);

		char hasParetnesis[] = " }";
		if (elementEndRow[0] == ' ' && elementEndRow[1] == '}'){
			elementEndRow += 2;
		}
		else {
			hasParetnesis[0] = 0;
		}

		rtfParameter = paramStart;
		rtfParameter = rtfParameter + "[@h2xml_RTF_ELEMENT:30] " + elementEndRow;

		// element
		rtfElement = CStr("\\trow") + elementStartRow + elementStart + elementEnd + "\\row" + hasParetnesis;

	}

	catch (char* e) {
		dbgPrintf(TERROR, "RTF Reference file %s: token %s not found\n", filename->buf,e);
		return RETVAL_ERROR;
	}
	m_part[RTF_HEADER].part(rtfHeader.buf);
	m_part[RTF_MODULE].part(rtfModule.buf);
	m_part[RTF_PARAM].part(rtfParameter.buf);
	m_part[RTF_ELEMENT].part(rtfElement.buf);

#else
	// for 1st version, use fixed reference strings instead of file ...
	m_part[RTF_HEADER].part(rtfHeader);
	m_part[RTF_MODULE].part(rtfModule);
	m_part[RTF_PARAM].part(rtfParameter);
	m_part[RTF_ELEMENT].part(rtfElement);
#endif

	return RETVAL_OK;
}

//! cut str at token, return pointer to string after token, if dir==true: search from end
char* CRtfDocu::cutCheck(char *str, char*token, bool findmode, bool dir)
{

	char* retval = NULL;
	char* lastChar = NULL;

	if (findmode) {
		retval = CStr::cut(str, token, dir);
	}
	else {
		char* tmp = rtfFind(str, token, lastChar);
		if (dir){
			char* lasttmp = tmp;
			while (tmp) {
				lasttmp = tmp;
				tmp = rtfFind(tmp + 1, token, lastChar);			// find last occurence
			}
			tmp = lasttmp;
		}
		if (tmp != NULL) {
			tmp[0] = 0;
			retval = lastChar;
		}
	}
	
	if (!retval) {
		throw token;
	}

	return retval;
}

// find in rtf file
// token may be separated!
char* CRtfDocu::rtfFind(char *str, char *token, char* &lastChar, bool findHere)
{
	char* found = NULL;
	char* strpt = str;
	while (strpt){
		if (findHere) {
			if (*strpt != token[0]) {
				strpt = NULL;
			}
		}
		else {
			strpt = strchr(strpt, token[0]);
		}
		if (strpt) {
			found = strpt+1;
			for (unsigned int i = 1; i < strlen(token); i++) {
				if (i == 24){
					//int dbg = 0;
				}
				// ignore formatting
				while (found){
					// ignore '{}'
					if (*found == '{' || *found == '}' || *found == 10 || *found == 13) {
						found++;
					}
					else if (*found == '\\') {
						found = strchr(found, ' ');
						if (found) {
							found++;
						}
					}
					else {
						break;
					}
				}

				if (!found) {
					break;
				}
				if (*found == token[i]){
					found++;
				}
				else {
					found = NULL;
					break;
				}
			}
			if (found) {
				break;
			}
			strpt++;
		}
	}
	lastChar = found;
	return strpt;
}

char* CRtfDocu::rtfGetString(char *str, CStr* outStr)
{
	char tmp[] = " ";
	// ignore formatting
	while (str){
		while (str){
			// ignore '{}'
			if (*str == '{' || *str == '}' || *str == 10 || *str == 13) {
				str++;
			}
			else if (*str == '\\') {
				str = strchr(str, ' ');
				if (str) {
					str++;
				}
			}
			else {
				break;
			}
		}

		if (!str||str[0]==0) {
			break;
		}
		tmp[0] = str[0];
		*outStr = *outStr + tmp;
		str++;
	}
	return outStr->buf;
}

