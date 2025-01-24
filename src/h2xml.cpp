/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////////////
//! \brief					 entry point:	main() implementation								\file
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "h2xml.h"
#include "global_functions.h"
#include "version.h"
#include "scanConfig.h"
#include "scanAnnotations.h"
#include "emit_xml.h"
#include "CXmlTag.h"
#include "CModuleList.h"
#include "CIdentList.h"
#include "CValue.h"
#include "CRtfDocu.h"

#define VERBOSE_FILENAME "h2xml_verbose.txt"
extern int yycompiler_flex_debug;		// not defined in releasee version
extern int yyannot_flex_debug;			// not defined in releasee version
extern int yyconf_flex_debug;
extern int yydebug;						// yacc debug
extern void lex_WriteToDoxygenParser(char* text);
#include "commandLineInfo.txt"			// comand line info string

FILE* fpOutFile=NULL;					// globally defined, so file can be erased if error
CStr outputfile = "";
CStr rtfFilename = "";
CStr rtfOutputfile = "";
CGlobalData* gd;						// global pointer to global data

char* strtok_Quotes(char* line);

//#define GENERATE_DEFAULT_FILE
int processMain(int argc, char* argv[], char* envp[]);
int main(int argc, char* argv[], char* envp[])
{
    /* generate default file for testing*/
#ifdef GENERATE_DEFAULT_FILE
    FILE* fp1 = fopen("defaultfileFloat.bin", "w");
    for (int i = 0; i < 32; i++) {
        union {
            char c[4];
            float f;
        } x;
        x.f = (float) i;
        for (int k = 0; k < 4; k++) {
            fputc(x.c[k],fp1);
        }
    }
    fclose(fp1);

    FILE* fp2 = fopen("defaultfileDouble.bin", "w");
    for (int i = 0; i < 32; i++) {
        union {
            char c[8];
            double f;
        } x;
        x.f = i;
        for (int k = 0; k < 8; k++) {
            fputc(x.c[k], fp2);
        }
    }
    fclose(fp2);

    FILE* fp3 = fopen("defaultfileMixed.bin", "w");
    struct {
        double a;
        long long b;
        float c;
        long d;
    } smix = { 1.0,2,3.0,4 };

    char * ptsmix = (char*)&smix;
    for (int k = 0; k < (int) sizeof(smix); k++) {
        fputc(ptsmix[k], fp3);
    }
    fclose(fp3);

#endif

	// read commandline from file
	int success=processMain(argc, argv, envp);
	if (success!=RETVAL_MESSAGE) {

		int knErrors = as.errorcount + gd->errorcount;
		int knWarnings = as.warningcount + gd->warningcount;
		if (knErrors == 0 && knWarnings == 0) {
			dbgPrintf(TINFO, "H2XML completed sucessfully: %d Errors, %d Warnings\n", knErrors, knWarnings);
			printf("H2XML completed sucessfully: %d Errors, %d Warnings\n", knErrors, knWarnings);
		}
		else {
			dbgPrintf(TINFO, "H2XML completed: %d Errors, %d Warnings\n", knErrors, knWarnings);
			printf("H2XML completed: %d Errors, %d Warnings\n", knErrors, knWarnings);
		}

		if (knErrors) {

			//#if !defined(_H2XML_DEBUG)
			if (fpOutFile){
				fclose(fpOutFile);
			}
			if (!outputfile.isEmpty()){
				remove(outputfile.buf);
			}
			if (!rtfOutputfile.isEmpty()){
				remove(rtfOutputfile.buf);
			}
			//#endif
			exit(EXIT_FAILURE);
		}
	}
	return(EXIT_SUCCESS);	
}

#define MAXARGS 100
int processMain(int argc, char* argvIn[], char* envp[])
{
		double start_clocks=clock();
        CStr commandLineAnnotations = "";
        CStringList* inputFilesList=NULL;

		generateAssign = false;
		firstrun=0;
		kn_global_mem=0;

		gd = new(CGlobalData);
		gd->init();

		initYdfpcc();

		// command line options
		// make copy of argument pointers, so arguments can be extended via file
		char* argv[MAXARGS];
        if (argc >= MAXARGS) {
            dbgPrintf(TERROR, "Maximum number of command line arguments exceeded (%d)\n", MAXARGS);
            return RETVAL_ERROR;
        }

		//for (int n = 0; n < argc; n++) {
		//	if (n < MAXARGS) {
		//		argv[n] = argvIn[n];
		//	}
		//}
		
		int maxsize=argc;
		if (argc>MAXARGS) {
			maxsize=MAXARGS;
		}
		memmove(argv,argvIn,maxsize*sizeof(char*));

		int i;
		for (i=1;i<argc;i++)
		{
			// -conf: config file
			if (strcmp(argv[i], "-conf") == 0){
				if (argc>i + 1) {
					gd->config->m_filename= (argv[i + 1]);
				}
				else {
					dbgPrintf(TERROR,"Error in Command Line: -conf must be followed by <configuration file>\n");
					return RETVAL_ERROR;
				}
				i++;
			}

            // -a <Annotation>
            else if (strcmp(argv[i], "-a") == 0){
                if (argc>i + 1) {
                    CStr newAnnotation = argv[i + 1];
                    newAnnotation.RemoveQuotes();                                           // remove leading and trailing quotes
                    commandLineAnnotations = commandLineAnnotations + newAnnotation;
                    commandLineAnnotations = commandLineAnnotations + " ";                 // start new line after annotation
                }
                else {
                    dbgPrintf(TERROR, "Error in Command Line: -a must be followed by <coomand line annotations>\n");
                    return RETVAL_ERROR;
                }
                i++;
            }


			// commandline file
			else if (strcmp(argv[i], "-f") == 0){
				if (argc>i + 1) {
					FILE* fpcmd;
					if ((fpcmd = fopen(argv[i + 1], "r")) == NULL){
						dbgPrintf(TERROR, "cannot open input command line file <%s>\n", argv[i + 1]);
						return RETVAL_ERROR;
					}
					i++;

					CStr* cmdline=new (CStr);
					*cmdline = "";
                    tmpBuf[0]='\0';
					while (!feof(fpcmd)) {
                        tmpBuf[0] = '\0';
						char* buf=fgets(tmpBuf, TMP_BUF_SIZE-1, fpcmd);
						if (buf) { 
							char *pos;
							if ((pos = strchr(tmpBuf, '\n')) != NULL) {
								*pos = ' ';
								*(pos + 1) = 0;
							}
							if (tmpBuf[0] != '#') {
								cmdline->stradd(tmpBuf);
							}
                        }
					}

					// parse command line string and insert into command arguments
                    

                    // parse command line string and insert into command arguments
                    char *cmd = strtok_Quotes(cmdline->buf);
                    int newcnt = i + 1;
                    while (cmd && argc < MAXARGS - 1)
                    {
                        for (int k = MAXARGS - 1; k>newcnt; k--) {
                            argv[k] = argv[k - 1];
                        }

                        argv[newcnt] = cmd;
                        newcnt++;
                        cmd = strtok_Quotes(0);
                        argc++;
					}

					fclose(fpcmd);
				}
				else {
					dbgPrintf(TERROR, "Error in Command Line: -f must be followed by <command line file name>\n");
					return RETVAL_ERROR;
				}
			}

			// -rtf: rtf reference file
			else if (strcmp(argv[i], "-rtf") == 0){
				if (argc>i + 1) {
					rtfFilename = (argv[i + 1]);
				}
				else {
					dbgPrintf(TERROR, "Error in Command Line: -rtf must be followed by <rtf template file>\n");
					return RETVAL_ERROR;
				}
				i++;
			}

			// -o: output directory
			else if(strcmp(argv[i],"-o")==0){
				if(argc>i+1) {
					as.outputpath=(argv[i+1]);
				}
				else {
					dbgPrintf(TERROR,"Error in Command Line: -o must be followed by <output_file>\n");
					return RETVAL_ERROR;
				}
				i++;
			}

            // -t: config type
            else if (strcmp(argv[i], "-t") == 0){
                if (argc>i + 1) {
                    CStringList* list = addToEnd(lNEXT, gd->cmdTypeList);
                    // for compatibility, exchange "Module" with "geckoModule"
                    if (strcasecmp(argv[i + 1], "module") == 0) {
                        list->m_string = "geckoModule";
                    }
                    else {
                        list->m_string = (argv[i + 1]);
                    }
                }
                else {
                    dbgPrintf(TERROR, "Error in Command Line: -o must be followed by <configType>\n");
                    return RETVAL_ERROR;
                }
                i++;
            }

			// -ortf: rtf output directory
			else if (strcmp(argv[i], "-ortf") == 0){
				if (argc>i + 1) {
					as.outputpathRtf = (argv[i + 1]);
				}
				else {
					dbgPrintf(TERROR, "Error in Command Line: -ortf must be followed by <output_file>\n");
					return RETVAL_ERROR;
				}
				i++;
			}

			// -I: include path
			else if (strstr(argv[i], "-I") == argv[i]){
				if (argv[i][2]!=0) {
                    CStr tmpStr = &argv[i][2];
                    tmpStr.ReplaceAll("\"", "");

                    Cvdt* tmp = CvdtAddToEnd(as.include_dirs, tmpStr.buf);
					tmp->name=tmp->name+"\\";
				}
				else if (argc > i + 1) {
					i++;
					Cvdt* tmp = CvdtAddToEnd(as.include_dirs, argv[i]);
					tmp->name = tmp->name + "\\";
				}
				else {
					dbgPrintf(TERROR,"Error in Command Line: -I must be followed by <include_directory>\n");
					return RETVAL_ERROR;
				}
			}	
			// -D: Preprocessor flag
			// allowed formats:
			// -Dname
			// -D name
			// -Dname=definition
		    // -D name=definition
			else if (strstr(argv[i], "-D") == argv[i]){
				Cvdt* tmp;
				char* name;
				if (argv[i][2] != 0) {
					name = &argv[i][2];
					
				}
				else if (argc > i + 1) {
					i++;
					name = &argv[i][0];
				}
				else {
					dbgPrintf(TERROR, "Error in Command Line: -D must be followed by <Preprocessor Flag>\n");
					return RETVAL_ERROR;
				}

				strtok(name, "=");
				char* def = strtok(NULL, "=");
				tmp = htab.hashinstall(name);
				tmp->token = TOKEN_DEFINE;
				tmp->valstr = "1";
				if (def) {
					tmp->valstr = def;
				}
			}

			else if(strcmp(argv[i],"-assign")==0){
				generateAssign = 1;
				//i++;
			}	
			// -v: 
			else if(strcmp(argv[i],"-v")==0){
                as.printVersion = true;
				dbgPrintf(TMSGALL, "%s\n%s\n", VERSION_INFO_COMPL, VER_COPYRIGHT_STR);
                if (!gd->config->m_version.isEmpty()) {
                    dbgPrintf(TMSGALL, "ConfigFile Version: %s\n", gd->config->m_version.buf);
                }
				if (as.verbose) {
					CStr dateTime = RCS_DATETIME;
					dateTime.Extract("$", " $");
					dbgPrintf(TMSGALL,"%s\n",dateTime.buf);
					CStr change = RCS_CHANGE;
					change.Extract("$", " $");
					dbgPrintf(TMSGALL, "%s\n", change.buf);
					CStr revision = RCS_REVISION;
					revision.Extract("$", " $");
					dbgPrintf(TMSGALL, "%s\n", revision.buf);
				}
			}
			// -verbose: 
			else if(strcmp(argv[i],"-verbose")==0){
				as.verbose=true;
			}
            // -WPacked
			else if (strcasecmp(argv[i], "-WPacked") == 0){
				gd->warnPackedData = true;
			}

			// 16 bit pointer
			else if(strcmp(argv[i],"-pointerSize16") == 0){
				as.pointerSize=2;
			}
            // 32 bit pointer
            else if (strcmp(argv[i], "-pointerSize32") == 0){
                as.pointerSize = 4;
            }
            // 64 bit pointer
            else if (strcmp(argv[i], "-pointerSize64") == 0){
                as.pointerSize = 8;
            }
            // -h: 
			else if(strcmp(argv[i],"-h")==0){
				dbgPrintf(TMSGALL, "%s\n%s\n", VERSION_INFO_COMPL, VER_COPYRIGHT_STR);
				dbgPrintf(TMSGALL, "%s", commandInfo);
				return RETVAL_MESSAGE;
			}
            // -i
			else if (strcmp(argv[i], "-i") == 0){
				if (argc>i + 1) {
                    CStringList* list = addToEnd(lNEXT, inputFilesList); 
                    list->m_string = (argv[i + 1]);                   
				}
				else {
					dbgPrintf(TERROR, "Error in Command Line: -i must be followed by <input_file>\n");
					return RETVAL_ERROR;
				}
				i++;
			}
            // -of
            else if (strcmp(argv[i], "-of") == 0){
                if (argc>i + 1) {
                    as.outputfile = (argv[i + 1]);
                }
                else {
                    dbgPrintf(TERROR, "Error in Command Line: -of must be followed by <output_file>\n");
                    return RETVAL_ERROR;
                }
                i++;
            }
			else if (argc-1==i) {
                CStringList* list = addToEnd(lNEXT, inputFilesList);
                list->m_string = argv[i];
			}
			else {
				dbgPrintf(TERROR,"Unknown command line argument <%s>\n", argv[i]);
				dbgPrintf(TINFO, "%s\n%s\n", VERSION_INFO_COMPL, VER_COPYRIGHT_STR);
				dbgPrintf(TINFO, "%s", commandInfo);
				return RETVAL_ERROR;
			}  		
	
		}

       // as.inputfile = argv[i];

        CStringList* lastInputfile = findLast(lNEXT, inputFilesList);

        if (lastInputfile==NULL || lastInputfile->m_string.len == 0) {
            if (as.printVersion) {
                return RETVAL_MESSAGE;
            }
            else {
                dbgPrintf(TERROR, "No input file specified\n%s\n", commandInfo);
                return RETVAL_ERROR;
            }
		}

        as.inputfile = lastInputfile->m_string.buf;

		gd->ptInputFileName = _strdup(as.inputfile.buf);

		char *extention=strrchr(as.inputfile.buf,'.');
		// check, if input is a .c or .h or.cpp file
		if (extention!=NULL && (strcmp(extention,".c")==0 || strcmp(extention,".h")==0 || strcmp(extention,".cpp")==0)) {

		}
		else {
			dbgPrintf(TERROR, "wrong input file type, file extention must be <.h>\n");
			return RETVAL_ERROR;
		}

		// Input Path berechnen
		as.inputpath=as.inputfile;
		char *tmp=strrchr(as.inputpath.buf,*"/");		//Unix
		char *tmp2=strrchr(as.inputpath.buf,*"\\");		//DOS
		if (tmp!=NULL) {
			as.inputname=&tmp[1];
			tmp[1]=0;									//terminate String
		}
		else if (tmp2!=NULL) {
			as.inputname=&tmp2[1];
			tmp2[1]=0;									//terminate String
		}
		else {
			as.inputpath.buf[0]=0;						//terminate String
			as.inputname=as.inputfile;
		}
		as.inputname_main=as.inputname;

		// default include directories: absolut path and source path
		CvdtAddToBegin(as.include_dirs,as.inputpath.buf);	
		CvdtAddToEnd(as.include_dirs,"");

		CStr inputname_no_extention=as.inputname;
        if (!as.outputfile.isEmpty()) {
            inputname_no_extention = as.outputfile;
        }
		char* tmpstr=strrchr(inputname_no_extention.buf,'.');
		if (tmpstr !=NULL) tmpstr[0]=0;								// end string here
		outputfile=as.outputpath+"\\"+inputname_no_extention+".xml";
		outputfile.AdjustForUnix();


		// Inits
		if (as.verbose) {
			//printf (" add to include path: %s\n", as.inputpath.buf);
            CStr errfile = as.outputpath + "\\" + inputname_no_extention + "_verbose.txt"; //CStr errfile=as.inputpath+as.outputpath+"/"+"errout";
			errfile.AdjustForUnix();
            as.fpStderr = freopen(errfile.buf, "w", stderr);
            if (as.fpStderr) {
				yydebug = 1;					//  yacc debug
				yycompiler_flex_debug = 1;			//  lex debug
				yyannot_flex_debug = 1;			//  lex debug
				yyconf_flex_debug = 1;			//  lex debug
			}
		}
		else {
			yydebug = 0;
			yycompiler_flex_debug = 0;
			yyannot_flex_debug = 0;
			yyconf_flex_debug = 0;
		}

		as.warningcount = 0;
		as.errorcount = 0;

		// read configuration file
		gd->config->parseConfigFile();
		if (gd->errorcount){
			return RETVAL_ERROR;
		}

        if (as.printVersion && !gd->config->m_version.isEmpty()) {
            dbgPrintf(TMSGALL, "ConfigFile Version: %s\n", gd->config->m_version.buf);
        }

        // check if any config type from command line matches config types from config file
        if (gd->configTypeList != NULL && !gd->configTypeFound){
            CStr typeList = "";
            gd->configTypeList->printList(&typeList);
            dbgPrintf(TERROR, "None of the types defined in config file are selected in command line with -t <type>.\n"
                "At least one type must be selected. Available types are: %s\n", typeList.buf);
            return RETVAL_ERROR;
        }

		// add predefined functions
		gd->config->addCommand("saveActiveTag", ANFUCNTION_CTRL_SAVEACTIVETAG, ANARG_NONE);
		gd->config->addCommand("restoreActiveTag", ANFUCNTION_CTRL_RESTOREACTIVETAG, ANARG_NONE);
		gd->config->addCommand("setActiveTag", ANFUCNTION_CTRL_SETACTIVETAG, ANARG_LABEL);
        gd->config->addCommand("InternalBitfieldEnd", ANFUCNTION_CTRL_INTERNAL_BITFIELD_END, ANARG_NONE);               // will be inserted at end of each macro, argument is anFunction of macro
        gd->config->addCommand("InternalRestoreTagLevel", ANFUCNTION_CTRL_INTERNAL_RESTORE_TAGLEVEL, ANARG_NONE);       // will be inserted at end of each macro after previuous pushTagLevel
        gd->config->addCommand("InsertCommandLine", ANFUNCTION_CTRL_INSERT_COMMANDLINE, ANARG_NONE);                   // insert string from command line, only annotations of the respective type(module, param, element etc.)
		gd->config->addCommand("incHirachyLevel", ANFUNCTION_CTRL_INC_HIRACHY_LEVEL, ANARG_NONE);
		gd->config->addCommand("decHirachyLevel", ANFUNCTION_CTRL_DEC_HIRACHY_LEVEL, ANARG_NONE);
        
        /*  run global action from comand line string buffer to initialize gd->cmdlineAction
            gd->cmdlineAction will be needed in actions from global string buffer
            erase any global annotations from commandline afterwards, these will be processed
            in a second step and placed AFTER the actions from global string buffer
        */
        if (!commandLineAnnotations.isEmpty()) {
            gd->checkEnumArgsElement = false;  // signal command line handling: some definitions may not be available, 
            lex_WriteToDoxygenParser(commandLineAnnotations.buf);
            gd->checkEnumArgsElement = true;

        }
        copyAnPt(gd->actualTag[2], gd->cmdlineAction);
        copyAnPt(NULL, gd->actualTag[2]);	            // clear
        copyAnPt(NULL, gd->actualTag[TAGLEVEL_GLOBAL]);	// clear

        /* run global action from config string buffer */
        gd->config->m_action[ANGROUP_GLOBAL] = gd->config->m_globalAction + gd->config->m_action[ANGROUP_GLOBAL].buf;
        gd->macroLevel++;   // internal (private) commands allowed from config file
		for (int i = 0; i < kn_ANGROUP; i++){
			copyAnPt(NULL, gd->actualTag[2]); // clear
			if (!gd->config->m_action[i].isEmpty()){
				lex_WriteToDoxygenParser(gd->config->m_action[i].buf);
			}
            copyAnPt(gd->actualTag[2], gd->configAction[i]);
		}
        copyAnPt(NULL, gd->actualTag[2]); // clear
        gd->macroLevel--;

        /* run global action from comand line string buffer
           only global annotations, place after global action in TAGLEVEL_GLOBAL
        */
        if (!commandLineAnnotations.isEmpty()) {
            gd->checkEnumArgsElement = false;  // signal command line handling: some definitions may not be available, 
            lex_WriteToDoxygenParser(commandLineAnnotations.buf);
            gd->checkEnumArgsElement = true;
        }
        copyAnPt(NULL, gd->actualTag[2]);	// clear

		/////////////////////////////////////////////////////////////////////////
		// initialize constants
		/////////////////////////////////////////////////////////////////////////
		insertstr("outputfile",outputfile.buf,1);

		if (as.verbose) {
			printf ("H2XML version: %s\n",VERSION_INFO_COMPL);
			printf ("source file: %s\n\n",as.inputfile.buf);
		} 

		init_values();

		newline_ydfpcc();

		
        /////////////////////////////////////////////////////////////////////////   
		// start parsing
		/////////////////////////////////////////////////////////////////////////

        CStringList* actualInputfile = inputFilesList;
        while (actualInputfile) {

            as.linecnt = 1;
            process_include(actualInputfile->m_string.buf);

            if (yycompilerin == NULL){
                dbgPrintf(TERROR, "cannot open input file <%s>\n", actualInputfile->m_string.buf);
                return RETVAL_ERROR;
            }

            /*
            as.inputfile = actualInputfile->m_string.buf;
            if ((yycompilerin = fopen(as.inputfile.buf, "r")) == NULL){
                dbgPrintf(TERROR, "cannot open input file <%s>\n", as.inputfile.buf);
                return RETVAL_ERROR;
            }
            as.infile = yycompilerin;

            */
            while (!feof(yycompilerin)){
                yyparse();
            }
            actualInputfile = actualInputfile->get(lNEXT);
        }
			
		if (gd->tagLevelPre != 2) {
			gd->dbgPrintf(TERROR, "Unmatched end of Compound structure\n");
		}

		if (gd->identLevel != 0) {
			gd->dbgPrintf(TERROR, "Unmatched end of Module structure\n");
		}

		if (as.errorcount || gd->errorcount){
			return RETVAL_ERROR;
		}

		//generate output files
		if (strlen(outputfile.buf)>0) {
			if ((fpOutFile = fopen(outputfile.buf, "w")) == NULL){
				dbgPrintf(TERROR, "cannot open %s for File Output\n", outputfile.buf);
				return RETVAL_ERROR;
            }
            else {
                //emitXml(fpOutFile, EMIT_PRESTART, NULL);
                gd->xmlRoot = new (CXMLTag);
                gd->xmlRoot->m_tagName = "Root";
                gd->xmlRoot->m_keyWord = "H2XML_Root";


                //setup merged attributes:
                int mergedTagCnt = 0;
                pushAnPt(gd->configAction[ANGROUP_GLOBAL], mergedTagCnt);
                pushAnPt(gd->configAction[ANGROUP_XMLGEN], ++mergedTagCnt);

                // global annotations
                pushAnPt(gd->actualTag[TAGLEVEL_GLOBAL], ++mergedTagCnt);

                /*
                    only insert global and xml command line action
                */
                CAnnotationData* tmpCmdLineAction[7];
                for (int i = 0; i < kn_ANGROUP; i++) {
                    tmpCmdLineAction[i] = NULL;
                    if (i == ANGROUP_GLOBAL || i == ANGROUP_XMLGEN) {
                        gd->dbgPrintf(TINFO, "Insert Command Line, group=%s\n", anGroupNames[i]);
                        tmpCmdLineAction[i] = gd->cmdlineAction[i];
                    }
                }
                pushAnPt(tmpCmdLineAction, ++mergedTagCnt);

#if DEBUG_PRINT_TAGS
                printAnTags(gd->cmdlineAction, "gd->cmdlineAction");
                printAnTags(tmpCmdLineAction, "tmpCmdLineAction");
                printAnTags(gd->actualTag, TAGLEVEL_GLOBAL, "gd->actualTag");
                printAnTags(gd->mergedTags, mergedTagCnt, "gd->mergedTags");
#endif

                // add tabSize and LineSize to root
                gd->xmlRoot->m_tabSize = (long)getAnnotationDataLong(gd->mergedTags, mergedTagCnt, ANGROUP_XMLGEN, ANFUNCTION_XML_TAB_SIZE, DEFAULT_TABSIZE);
                gd->xmlRoot->m_lineLength = (long)    getAnnotationDataLong(gd->mergedTags, mergedTagCnt, ANGROUP_XMLGEN, ANFUNCTION_XML_LINE_LENGTH, DEFAULT_MAXLINELENGTH);
                CValue::globalFloatPrecision = (long) getAnnotationDataLong(gd->mergedTags, mergedTagCnt, ANGROUP_XMLGEN, ANFUNCTION_XML_FLOAT_PRECISION, DEFAULT_FLOAT_PRECISION);

                // start with global definitions, will be executed once
                gd->xmlRoot->addAttribute(gd->mergedTags, mergedTagCnt, ANGROUP_GLOBAL);
                //gd->xmlRoot->addAttribute(gd->mergedTags, mergedTagCnt, ANGROUP_XMLGEN);

                if (gd->config->m_outputGlobal[ANGROUP_STRUCTURE] ) {
                    CIdentList::emitAll(gd->xmlRoot, gd->identList[0], mergedTagCnt);
                }
                if (gd->config->m_outputGlobal[ANGROUP_MODULE]) {
                    CModuleList::emitAll(gd->xmlRoot, gd->moduleList, mergedTagCnt);
                }

				gd->xmlRoot->removeRedundantTags();

                if (as.errorcount + gd->errorcount>0) {
                    return RETVAL_ERROR;
                }

				// write generated XML list to file
				CStr* dataFormat = getAnnotationDataLabel(gd->mergedTags, mergedTagCnt, ANGROUP_XMLGEN, ANFUNCTION_XML_NUMBER_FORMAT);
				if (dataFormat) {
					if (strcasecmp(dataFormat->buf, "hex")==0) {
						CValue::globalDplyType = ANARG_DPLY_HEX;
					}
					if (strcasecmp(dataFormat->buf, "int")==0) {
                        CValue::globalDplyType = ANARG_DPLY_INTEGER;
					}
				}

				if (gd->xmlRoot->m_sub) { // skip root directory
					gd->xmlRoot->m_sub->emit(fpOutFile, 0);
				}

				// all paramteres not defined within a module
                /*
				CAnnotationData* mergedAttributes[kn_ANGROUP];
				for (int i = 0; i < kn_ANGROUP; i++){
					mergedAttributes[i] = NULL;									// no global attribute initialization
				}
				CIdentList::emitAll(fpOutFile, gd->identList[0],mergedAttributes);
				emitXml(fpOutFile, EMIT_POSTEND, NULL);
				*/
				fclose(fpOutFile);
				fpOutFile = NULL;

                if (as.errorcount + gd->errorcount>0) {
                    return RETVAL_ERROR;
                }

				// generate RTF docu
				if (!rtfFilename.isEmpty()){
					rtfOutputfile = as.outputpath + "\\" + inputname_no_extention + ".rtf";
					if (!as.outputpathRtf.isEmpty()) {
						rtfOutputfile = as.outputpathRtf + "\\" + inputname_no_extention + ".rtf";
					}
					rtfOutputfile.AdjustForUnix();
					rtfFilename.AdjustForUnix();
					CRtfDocu rtfDocu;
					rtfDocu.parseRef(&rtfFilename);
					rtfDocu.emit(&rtfOutputfile, gd->xmlRoot->m_sub);
				}
			}
		}

		double compiletime=double(clock()-start_clocks)/CLOCKS_PER_SEC;
		#ifndef _CUSTOMER
		if (as.verbose) {
			dbgPrintf(TINFO,"parser time           %2.2fs\n",compiletime);
		}
		#endif

  	return 0;
}

char* strtok_Quotes(char* line)
{
    
    static char* thisLine = NULL;
    if (line) {
        thisLine = line;
    }
    while (*thisLine == ' ') {
        thisLine++;
    }

    if (*thisLine == '\0') {
        return NULL;
    }
    bool openQuotes = false;
    bool wasBlank = false;
    char* lineStart = thisLine;
    while (*thisLine){
        if (openQuotes) {
            while (*thisLine != '\"' && *thisLine != 0) {
                thisLine++;
            }
            if (*thisLine == '\"') {
                *thisLine = '\0';
            }
            openQuotes = false;
        }
        else {
            if (*thisLine == '\"') {
                openQuotes = true;
                lineStart = thisLine + 1;
            }
            else {
                while (*thisLine == ' ') {
                    *thisLine = '\0';
                    wasBlank = true;
                }
                if (wasBlank) {
                    thisLine++;
                    break;
                }
            }
        }
        thisLine++;
    }
    return lineStart;
}

void checkVersion() {
    if (!gd->config->m_h2xmlVersion.isEmpty()) {
        bool versionCorrect = true;
        CStr versionString = gd->config->m_h2xmlVersion.buf;
        char* value = strtok(versionString.buf, ".");
        long versionArray[4] = { VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION, VERSION_RELEASE_CANDIDATE };
        for (int i = 0; i < 4; i++) {
            if (value) {
                long version = atol(value);
                if (versionArray[i]>version) {
                    break;
                }
                if (versionArray[i] < version) {
                    versionCorrect = false;
                    break;
                }
                value = strtok(NULL, ".");
            }
            else {
                versionCorrect = false;
                break;
            }
        }
        
        if (versionCorrect == false) {
            dbgPrintf(TERROR, "H2XML version <%s> required for config file. Actual H2XML version is <%s>\n", gd->config->m_h2xmlVersion.buf, VER_FILE_VERSION_STR);
        }
    }
}


#ifdef __SOLARIS__
	double pow(int a,int b) {
		return pow(double(a),b);
	}
	double fmod(int a,int b) {
		return fmod(double(a),double(b));
	}
#endif

#ifdef WIN32
	double pow(int a,int b) {
		return pow(double(a),b);
	}
	double fmod(int a,int b) {
		return fmod(double(a),double(b));
	}

	double log(int a) {
		return log(double(a));
	}
#endif




