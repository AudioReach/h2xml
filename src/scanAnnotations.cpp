/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

#include "h2xml_common.h"
#include "scanAnnotations.h"
#include "scanConfig.h"
#include "emit_xml.h"
#include "CXmlTag.h"
#include "CAnnotationData.h"
#include "CIdentList.h"
#include "CModuleList.h"



extern int yyannot_flex_debug;
extern CConfigAttributes* dscattr;
extern int doxyStateTable[30];
extern long lexPushStateDoxy(long state);
extern void pushXmlState(eAnArgTypes arg);
extern void pushXmlStateBracket();
extern void unputcDoxy(const char* in);
void checkStateAtEndOfDoxyComment();
extern void scanStringbuffer(char* string);

void pushTagLevel(int level);
int popTagLevel();

#define ELEMENT_NAME_NOT_INITIALIZED "unknown"

long CAnnotationData::m_countFind = 0;
long CAnnotationData::m_breakFound = 0;
long CAnnotationData::m_countFindLevel = 0;

// called at start of comment, attach attributes to correct tags
void startDoxyComment()
{
	for (int tagType = ANGROUP_NONE; tagType < kn_ANGROUP; tagType++){
		if (as.connect_previous && as.last_identifier) {														// attach to previous Statement
			gd->tagLevel = 0;
			if (!as.last_identifier->m_xD[tagType]) {
				as.last_identifier->m_xD[tagType] = new(CAnnotationData);
				as.last_identifier->m_xD[tagType]->m_anGroup = (eAnGroups)tagType;
				as.last_identifier->m_xD[tagType]->m_anName = "";
			}
			gd->actualTag[gd->tagLevel][tagType] = as.last_identifier->m_xD[tagType];
		}
		else {
			gd->tagLevel = gd->tagLevelPre;
			if (!gd->actualTag[gd->tagLevel][tagType]) {
				gd->actualTag[gd->tagLevel][tagType] = new(CAnnotationData);
				gd->actualTag[gd->tagLevel][tagType]->m_anGroup = (eAnGroups) tagType;
				//gd->actualTag[gd->tagLevel][tagType]->m_name = "";
			}
		}
	}
}

// called at end of comment
void endDoxyComment()
{
    gd->selectNameSpace = "";
	checkStateAtEndOfDoxyComment();
}

void lexBeginModule()
{
	long generateOutput = (long) getAnnotationDataLong(gd->actualTag, gd->tagLevel, ANGROUP_MODULE, ANFUNCTION_GENERATE_OUTPUT, false);
	if (generateOutput) {
		gd->moduleScope = new(CModuleList*);
		*gd->moduleScope = NULL;
		if (gd->identLevel < MAX_IDENT_LIST_LEVEL - 1) {
			gd->identLevel++;
		}
		else {
			gd->dbgPrintf(TERROR, "Maximum nested Module structures <%d> reached\n", gd->identLevel);
		}
	}
	startStructCompound(10);
}

// actually quite similar to Identifier handlig, but modules have no C equivalent ...
void lexEndModule() {
	endStructCompound(10);
	// gd->sociate all attributes collected so far with this identifier
	long generateOutput = (long) getAnnotationDataLong(gd->actualTag, gd->tagLevel, ANGROUP_MODULE, ANFUNCTION_GENERATE_OUTPUT, false);
	if (generateOutput) {
		CAnnotationData* nameTag = findLastAnnotationData(gd->actualTag, gd->tagLevel, ANGROUP_MODULE, ANFUNCTION_NAME);
		if (nameTag && nameTag->m_assignmentList->m_string.isEmpty()) {
			gd->dbgPrintf(TWARNING, "Module name not defined, module ignored\n");
		}
		else {
			CModuleList* modlist = CModuleList::addElementToEnd(gd->moduleList, nameTag->m_assignmentList->m_string.buf);
			*gd->moduleScope = modlist;
			gd->moduleScope = gd->moduleScopeNULL;
			for (int i = 0; i < kn_ANGROUP; i++){
				modlist->m_xD[i] = gd->actualTag[gd->tagLevel][i];
			}
			modlist->m_identList = gd->identList[gd->identLevel];
			gd->identList[gd->identLevel] = NULL;
		}
		if (gd->identLevel > 0) {
			gd->identLevel--;
		}
		else {
			gd->dbgPrintf(TERROR, "Unmatched end of Module structure\n");
		}	
	}

	for (int i = 0; i < kn_ANGROUP; i++){
		gd->actualTag[gd->tagLevel][i] = NULL;				// next collected attributes will not be added but newly created 
	}
}


// called whenever a structure compound statement is called
// x: just a value to trace where it came from
void startStructCompound(int x)
{
	if (gd->tagLevelPre < MAX_TAG_LEVEL - 1) {
		gd->tagLevelPre++;
		gd->tagLevel = gd->tagLevelPre;
	}
	else {
		gd->dbgPrintf(TERROR, "Maximum nested Compound structures <%d> reached\n", gd->tagLevel);
	}
}

// called whenever a structure compound statement is terminated
// x: just a value to trace where it came from
void endStructCompound(int x)
{
	if (gd->tagLevelPre > 1) {
		gd->tagLevelPre--;
		gd->tagLevel = gd->tagLevelPre;
	}
	else {
		gd->dbgPrintf(TERROR, "Unmatched end of Compound structure\n");
	}

}

void identifierCreate(CIdentifier* ident)
{
	for (int i = 0; i < kn_ANGROUP; i++){
		ident->m_xD[i] = gd->actualTag[gd->tagLevelPre][i];
		gd->actualTag[gd->tagLevelPre][i] = NULL;				// next collected attributes will not be added but newly created 
		gd->actualTag[0][i] = NULL;							// next collected attributes will not be added but newly created 
	}

    ident->m_fileName=gd->ptInputFileName;
    ident->m_linecnt = as.linecnt;


    //gd->stateIsBitfield = false;

	// add to identifier list
	CIdentList* identList = CIdentList::addElementToEnd(gd->identList[gd->identLevel], ident);
	identList->m_ident = ident;	
}


static int dbgcnt=0;

///////////////////////////////////////////////////////////
// Keyword handling
///////////////////////////////////////////////////////////
void evaluateKeyword(char* text, bool hasArgument)
{
	dbgcnt++;


	CStr keyword = text;
	keyword.RemoveQuotes();
	if (strstr(keyword.buf, gd->config->m_globalPrefix.buf) != keyword.buf) {
		return;																				// not an htxml keyword, skip
	}
	dscattr = gd->config->m_htab.hashlookup(keyword.buf);
	if (dscattr) {// keyword found
#ifdef _DEBUG

		/*if (strstr(text, "h2xmlk_insertEnum") != 0) { //
		    int dbg = 0;
		}
		*/
#endif
        if ((hasArgument || dscattr->m_default_set) && (dscattr->m_anArgTypeList || dscattr->m_anArgTypeListList)) {
            if (dscattr->m_anArgTypeList) {
                pushXmlState(dscattr->m_anArgTypeList->m_argType);								// set LEX argument state
            }
            else {
                pushXmlState(dscattr->m_anArgTypeListList->m_argType);								// set LEX argument state
            }
		}
		else if (dscattr->m_anArgTypeList!= NULL && dscattr->m_anArgTypeList->m_argType == ANARG_IGNORE){
			gd->dbgPrintf(TWARNING, "Argument missing for '%s'\n", keyword.buf);
		}
        else if (dscattr->m_anArgTypeListList != NULL && dscattr->m_anArgTypeListList->m_argType == ANARG_IGNORE){
            gd->dbgPrintf(TWARNING, "Argument missing for '%s'\n", keyword.buf);
        }

        // check for private keyword
        if (gd->macroLevel <= 1 && dscattr->m_accessSpecPrivate) {
            gd->dbgPrintf(TERROR, "Trying to access internal annotation '%s'\n", keyword.buf);
        }

        // global annotations are not attached to an identifier, but globally stored
        if (dscattr->m_anFunction == ANFUCNTION_CTRL_INTERNAL_RESTORE_TAGLEVEL) {
            popTagLevel();
        }
        if(dscattr->m_globalAnnotation) {
            pushTagLevel(TAGLEVEL_GLOBAL);
        }

        eAnGroups group = dscattr->m_anGroup;

        // if bitfield, move all element annotations to bitfield group
        if (dscattr->m_anFunction == ANFUNCTION_TAG_BITFIELD || dscattr->m_anFunction == ANFUNCTION_ALIAS_BITFIELD) {
            gd->stateIsBitfield = true;
            gd->dbgPrintf(TINFO, "STATE BITFIELD=true\n");
        }
       
        if (group == ANGROUP_ELEMENT && gd->stateIsBitfield) {
            group = ANGROUP_BITFIELD;
        }

        if (gd->stateIsBitfield && dscattr->m_anFunction == ANFUCNTION_CTRL_INTERNAL_BITFIELD_END) {
            gd->stateIsBitfield = false;
            gd->dbgPrintf(TINFO, "STATE BITFIELD=false\n");
        }
        
        gd->xD = CAnnotationData::addElement(gd->actualTag[gd->tagLevel][group], dscattr);
        gd->xD->initData(dscattr);
		gd->xD->m_fileName = gd->ptInputFileName;
		gd->xD->m_linecnt = gd->linecnt;
		gd->xD->m_moduleScope = gd->moduleScope;
        gd->xD->m_selectNameSpace = gd->selectNameSpace;

        if (dscattr->m_anFunction == ANFUNCTION_CTRL_INSERT_COMMANDLINE) {
            gd->dbgPrintf(TINFO, "Insert Command Line, group=%s\n", anGroupNames[group]);
            if (group == ANGROUP_ELEMENT) {
                //int dbg = 0;
            }
            CAnnotationData* an = gd->cmdlineAction[group];
            while (an) {
                CAnnotationData* ancopy= addToEnd(lNEXT, gd->actualTag[gd->tagLevel][group]);
                ancopy->copyData(an);
                an = an->get(lNEXT);
            }    
        }

		if (dscattr->m_showInTag) {
			//int dbg = 0;
		}

		if (yyannot_flex_debug) {
            gd->dbgPrintf(TINFO, "keyword <%s> found, group=%d\n", keyword.buf, group);
		}

		if (!hasArgument) {
			if (dscattr->m_default_set){
				CStr output = CStr("{") + dscattr->m_default.buf + "}";
				gd->xD->m_isDefault = true;
				scanStringbuffer(output.buf);
			}
			else {
				endStateArgument();
			}
		}

	}
	else {
        CStr keyword_skip = keyword + KEYWORD_SKIPPED_PREFIX;
        dscattr = gd->config->m_htab.hashlookup(keyword_skip.buf);
        bool warnOnSkippedAnnotations = false;
        CAnnotationData* an = gd->cmdlineAction[ANGROUP_XMLGEN]->findLastAnnotation(ANFUCNTION_WARN_ON_SKIPPED_ANNOTATIONS, 0);
        if (an) {
            warnOnSkippedAnnotations = an->m_assignmentList->m_value1.l!=0;
        }

        if (!warnOnSkippedAnnotations && dscattr) {
            return;
        }
        else {
            gd->dbgPrintf(TERROR, "Unknown Keyword '%s'\n", keyword.buf);
        }
	}
}

// attention: push data back in reverse order
void endStateArgument()
{
    CAnArgTypeList *arglist = gd->xD->m_anArgTypeList;

    if (arglist == NULL) {
        arglist = gd->xD->m_anArgTypeListList;
    }

    
    if (arglist && arglist->m_argType == ANARG_ENUM_TYPE) {
       // int dbg = 0;
    }
	// check check correct number of parameters
	gd->xD->checkArgList();
	

	// special treatment for some annotations
	if (gd->xD->m_anFunction == ANFUNCTION_GLOBALANNOTATIONS) {
		static long tagLevelSave = gd->tagLevel;
		if (gd->xD->m_assignmentList->m_value1.l!= 0) {		// set global tags
            if (gd->tagLevel != TAGLEVEL_GLOBAL) {
				tagLevelSave = gd->tagLevel;
                gd->tagLevel = TAGLEVEL_GLOBAL;
			}
		}
		else {
            if (gd->tagLevel == TAGLEVEL_GLOBAL) {							// restore normal tags
				gd->tagLevel=tagLevelSave;
			}
		}
	}

    if (gd->xD->m_anFunction == ANFUNCTION_SELECT || gd->xD->m_anFunction == ANFUNCTION_SELECT_COPY) {
		if (!gd->xD->m_assignmentList->m_string.isEmpty()) {
            CStr name = HASH_PREFIX;
			name = name + gd->xD->m_assignmentList->m_string;
			name.ReplaceAll("::", ".");
			Cvdt* tmp = htab.hashlookup(name.buf);
			if (tmp  && tmp->identifier) {
                CIdentifier* ident = tmp->identifier;
                if (gd->xD->m_anFunction == ANFUNCTION_SELECT_COPY) {
                    ident = newCIdentifier(tmp->identifier);
                    as.last_identifier = ident;
                    identifierCreate(ident);
                    for (int i = 0; i < kn_ANGROUP; i++){
                        ident->m_xD[i] = CAnnotationData::copyList(tmp->identifier->m_xD[i]);
                    }
                    //CAnnotationData::printList(ident->m_xD[3]);
                }               
				as.connect_previous = true;
                as.last_identifier = ident;
                gd->selectNameSpace = gd->xD->m_assignmentList->m_string;
                gd->selectNameSpace.ReplaceAll("::", "_");

				startDoxyComment();
			}
			else {
				gd->dbgPrintf(TERROR, "Identifier '%s'not found\n", gd->xD->m_assignmentList->m_string.buf);
			}
		}
	}

    if (gd->xD->m_anFunction == ANFUNCTION_CREATE_STRUCT) {
        as.connect_previous = true;
        as.last_identifier = new(CIdentifier);
        startDoxyComment();
        if (!gd->xD->m_assignmentList->m_string.isEmpty()) {
            CStr name = HASH_PREFIX;
            name = name + gd->xD->m_assignmentList->m_string;
            name.ReplaceAll("\"", "");
            Cvdt* tmp = htab.hashinstall(name.buf);
            if (tmp) {
                tmp->identifier = as.last_identifier;
            }
        }
    }

    if (gd->xD->m_anFunction == ANFUNCTION_INSERT_PARAM) {
        if (as.last_identifier && (as.last_identifier->m_declspec == NULL || as.last_identifier->GetTypeId() == TYPE_STRUCT || as.last_identifier->GetTypeId() == TYPE_UNION || as.last_identifier->GetTypeId() == TYPE_ENUM)){
            CIdentifier* ident = as.last_identifier; 
            CIdentList* identList = CIdentList::addElementToEnd(gd->identList[gd->identLevel], ident);
            identList->m_ident = ident;
            //CAnnotationData::printList(ident->m_xD[3]);

		}
		else {
			if (as.last_identifier && as.last_identifier->m_cvdt) {
				gd->dbgPrintf(TERROR, "Identifier'%s' is not a structure\n", as.last_identifier->m_cvdt->name.buf);
			}
			else {
				gd->dbgPrintf(TERROR, "Last Identifier is not a structure\n");
			}
		}		
	}

	
	// replace arguments
	int AllArguments = gd->xD->m_knArgumentsIn;
	int sets = 0;
    if (gd->xD->m_knArgumentsList > 0) {
        sets = (gd->xD->m_knArgumentsIn - gd->xD->m_knArguments) / gd->xD->m_knArgumentsList;	//
	}

	CStr buffer;
	CStr value="";
	char arg[10];
	char buf1[1000];
	CAssignmentList *list;
	CStr outBuffer = "";
	CStr allArgs = "";

	buffer = dscattr->m_action.buf;
	list = gd->xD->m_assignmentList;
	if (!buffer.isEmpty()){

		/* \0 returns the num name bfore it is expanded into its enum elements */
		buffer.ReplaceAll("\\0", gd->xD->m_enumName);

		for (int i = 0; i < AllArguments; i++){
			if (!list) {
				gd->dbgPrintf(TERROR, "Not enough Arguments\n");
				break;
			}
			sprintf(arg, "\\%d", i+1);
			if (dscattr->m_removeQuotes) {
				//int dbg = 0;
			}
			list->getValueString(&value, !dscattr->m_removeQuotes);
			
			buffer.ReplaceAll(arg, value.buf);
			if (i>0) {
				allArgs.stradd("; ");
			}
			allArgs.stradd(value.buf);
			list = list->get(lNEXT);
		}
		sprintf(buf1, "%d", sets);
		if (buf1[0] == 0) {
			//int dbg = 0;
		}
		buffer.ReplaceAll("\\kn", buf1);
		buffer.ReplaceAll("\\...", allArgs.buf);
		outBuffer.stradd(buffer.buf);
	}

	// for all argument sets
    static int cnt = 0;
    cnt++;
	list = gd->xD->m_assignmentList;
    if (cnt == 9) {
       // int dbg = 0;
    }
    for (int i = 0; i < gd->xD->m_knArguments; i++){
        if (list->get(lNEXT)) {
            list = list->get(lNEXT);
        }
    }
	buffer = dscattr->m_actionList.buf;
	CStr allArgsList = "";
	if (!buffer.isEmpty()){
		for (int i = 0; i <sets; i++){
			buffer = dscattr->m_actionList.buf;
            for (int k = 0; k < gd->xD->m_knArgumentsList; k++){
				sprintf(arg, "\\%d", k + 1);
				list->getValueString(&value, !dscattr->m_removeQuotes);
				buffer.ReplaceAll(arg, value.buf);
				if (i>0) {
					allArgsList.stradd("; ");
				}
				allArgsList.stradd(value.buf);
				list = list->get(lNEXT);
			}
			buffer.ReplaceAll("\\...", allArgsList.buf);
			outBuffer.stradd(buffer.buf);
		}
	}

	buffer = dscattr->m_actionPost.buf;
	list = gd->xD->m_assignmentList;
	if (!buffer.isEmpty()){
		for (int i = 0; i < AllArguments; i++){
			sprintf(arg, "\\%d", i + 1);
			list->getValueString(&value, true);

			buffer.ReplaceAll(arg, value.buf);
			list = list->get(lNEXT);
		}
		sprintf(buf1, "%d", sets);
		if (buf1[0] == 0) {
			//int dbg = 0;
		}
		buffer.ReplaceAll("\\kn", buf1);
		buffer.ReplaceAll("\\...", allArgs.buf);
		outBuffer.stradd(buffer.buf);
	}


    if (gd->stateIsBitfield && dscattr->m_anFunction == ANFUNCTION_ALIAS_BITFIELD_END) {
        outBuffer.stradd(" @");
        outBuffer.stradd(gd->config->m_globalPrefix.buf);
        outBuffer.stradd(gd->config->m_prefix[ANGROUP_ELEMENT].buf);
        outBuffer.stradd("InternalBitfieldEnd");
    }

    //restore taglevel
    if (dscattr->m_globalAnnotation) {
        outBuffer.stradd(" @");
        outBuffer.stradd(gd->config->m_globalPrefix.buf);
        outBuffer.stradd("InternalRestoreTagLevel");
    }

	if (!outBuffer.isEmpty()){
 
		dbgPrintf(TINFO, "LEX Pushback: %s\n", outBuffer.buf);
		scanStringbuffer(outBuffer.buf);
	}

}

void pushTagLevel(int level)
{
    dbgPrintf(TINFO, "push Taglevel old=%d, new=%d\n", gd->tagLevel, level);
    CIntList* tmp = addToEnd(lNEXT, gd->tagLevelList);
    tmp->m_value = gd->tagLevel;
    gd->tagLevel = level;
}

int popTagLevel()
{
    int oldLevel = gd->tagLevel;

    CIntList* tmp = findLast(lNEXT, gd->tagLevelList);
    if (tmp) {
        gd->tagLevel = tmp->m_value;
        deleteFromEnd(lNEXT, gd->tagLevelList);
    }
    else {
        dbgPrintf(TERROR, "try to read invalid value\n");
    }
    dbgPrintf(TINFO, "pop Taglevel old=%d, new=%d\n", oldLevel, gd->tagLevel);
    return gd->tagLevel;
}

