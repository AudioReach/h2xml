/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

#include "scanConfig.h"
#include "h2xml_global.h"

extern int yyconf_flex_debug;
extern FILE* yyconfin;
extern int yyconflex();
void yyconfrestart(FILE *new_file);


CConfig::CConfig()
{
	m_filename = "";
	m_htabList = NULL;
	m_version = "";
    m_h2xmlVersion = "";
    m_globalPrefix = "";
    m_globalAction = "";
	for (int i = 0; i < kn_ANGROUP; i++){
		m_prefix[i] = "";
        m_actualPrefix[i] = "";
        m_prefixList[i] = NULL;
		m_action[i] = "";
        m_AnGroupAvailable[i] = false;
        m_outputGlobal[i] = false;
	}
    m_outputGlobal[ANGROUP_MODULE] = true;  // for compatibility reasons
    m_prefix[ANGROUP_BITFIELD] = "bitfield";

}


CConfig::~CConfig()
{
}

bool CConfig::parseConfigFile()
{
	if (m_filename.buf[0] == '\0') {
		dbgPrintf(TERROR, "config file not defined");
		return false;
	}
	yyconfin = fopen(m_filename.buf, "r");

	// open at least one file
	if (yyconfin == NULL) {
		dbgPrintf(TERROR, "cannot open file <%s>", m_filename.buf);
		return false;
	}
	//yyvi2ccmprestart(yyvi2ccmpin);
	if (yyconf_flex_debug != 0) {
		dbgPrintf(TINFO, "------------------------------------------------------\n");
		dbgPrintf(TINFO, "-- Start Config Scanner\n");
		dbgPrintf(TINFO, "------------------------------------------------------\n");
	}
	yyconflex();
	if (yyconf_flex_debug != 0) {
		dbgPrintf(TINFO, "------------------------------------------------------\n");
		dbgPrintf(TINFO, "-- Exit Config Scanner\n");
		dbgPrintf(TINFO, "------------------------------------------------------\n");
	}
	fclose(yyconfin);
	if (yyconf_flex_debug != 0) {
		emit();
	}

	return true;
}

void CConfig::emit()
{
	dbgPrintf("/***********************************************\n");
	dbgPrintf("* Emit Config File <%s>\n", m_filename.buf);
	dbgPrintf("************************************************/\n");
	CConfigAttributes* listStart=m_htabList;
	while (listStart) {
		listStart->emit();
		listStart = listStart->get(lRIGHT);
	}
}

void CConfig::addCommand(char* cmdName, eAnFunction function, eAnArgTypes argType) {

	for (int i = 0; i < kn_ANGROUP; i++){
		CConfigAttributes* scatr = new(CConfigAttributes);
		scatr->m_anGroup = (eAnGroups) i;
		scatr->hash.cases = KEYWORDS_NOT_CASESENSITIVE;
		scatr->m_anType = ANTYPE_CONTROL;
		scatr->m_keyword = cmdName;
		scatr->m_anFunction = function;
		CAnArgTypeList *list = addToEnd(lNEXT, scatr->m_anArgTypeList);
		list->m_argType = argType;
		if (argType != ANARG_NONE) {
			scatr->m_knArguments = 1;
		}
		CStr keyName = m_globalPrefix + m_prefix[i] + cmdName;
		m_htab.hashinstallInsertEnd(keyName.buf, scatr, &m_htabList);
	}
}

