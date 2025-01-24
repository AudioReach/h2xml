/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

//!	Defines the entry point for the console application
/*! \file 
*/	
#ifndef AFX_YDFPASM_H__C658E42B_113C_4CB4_9DE8_194CD03850B1__INCLUDED_
#define AFX_YDFPASM_H__C658E42B_113C_4CB4_9DE8_194CD03850B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "h2xml_common.h"
#include "h2xml_global.h"
//#include "cdokuGenerator.h"

#define HASHSIZE 	5000		//Size of Hashtable


Cvdt *hashtab[HASHSIZE];	// Hash Table


void GenerateOutputFiles();
void PrintLisFile(FILE* lisfile, int mode, int* linenr, int endadr);
void MakeDefFile();
long insert_crc(int rom_start, int rom_end, long64* opcode, int er, int &asd_cnt, int rom_insert);

int  fprintf_mic(FILE *stream, const char* format,...);
FILE* fopen_mic(char* a, char *b);
void fclose_mic(FILE* a);
char* long_to_binary(long value, char* outstr,int bits);
int exprCount;
bool generateAssign;
int firstrun;
//CDokuGenerator* outRTF;
//CDokuGenerator* outRTF2;
//CDokuGenerator* outRTF3;

// hashtable
CHashTable htab;

#endif // !defined(AFX_YDFPASM_H__C658E42B_113C_4CB4_9DE8_194CD03850B1__INCLUDED_)

