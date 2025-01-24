/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

#pragma once

#include "h2xml_common.h"
#include "CConfigAttributes.h"


#define DEFAULT_EXPANDENUMS			true       
#define DEFAULT_EXPANDSTRUCTS       true 
#define DEFAULT_EXPANDTYPEDEFS      false  
#define DEFAULT_EXPANDARRAY			true  
#define DEFAULT_EXPAND_ARRAY_OFFSET 1  


#define KEYWORDS_NOT_CASESENSITIVE 1			// 0: case-senitive; 1: case-insensitive
#define KEYWORD_SKIPPED_PREFIX "_skipped"

class CConfigAttributes;
class CXMLConfigData;
class CConfig
{
public:
	CConfig();
	~CConfig();

	// Attribute prefixes
    CStr m_prefix[kn_ANGROUP];
	CStr m_actualPrefix[kn_ANGROUP];
    CStringList* m_prefixList[kn_ANGROUP];
	CStr m_action[kn_ANGROUP];
    bool m_AnGroupAvailable[kn_ANGROUP];
    bool m_outputGlobal[kn_ANGROUP];

	CStr m_version;
    CStr m_h2xmlVersion;
    CStr m_globalPrefix;
    CStr m_globalAction;

	CStr m_filename;
	CConfigAttributes* m_htabList;
	CXMLConfigData *m_xmlConfigData;
	CHashTableEx<CConfigAttributes> m_htab;
	bool parseConfigFile();
	void emit();
	void addCommand(char* cmdName, eAnFunction function, eAnArgTypes argType = ANARG_NONE);
};

