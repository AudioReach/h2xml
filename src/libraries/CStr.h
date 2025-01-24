/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

//!	special string functions for C 
/*! \file 
*/	
#ifndef AFX_YDFPASM_H__CSTR__INCLUDED_
#define AFX_YDFPASM_H__CSTR__INCLUDED_

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>


// Simple CStr
#define CSTR_START_BUF 100
//!	Class for simple C-like string manipulation
/*!
*
*/
class CStr
{
	public:
	CStr();
	CStr(const char *str);
	CStr(const CStr& str);
	~CStr();
	void insert(const char* ins);
	void add(const char* in1, const char* in2);		//! x=in1+in2
	void stradd(const char* in, int minsize=40000) ;//! fast string add for large strings
	void clear();									//! set length to 0 without deleting buffer
	void initialize(char init, int n, int size);	//! initialize first n charactes with init, allocate buffer size
	char* cut(const char* in1);						//! cut string at first occurence of in1
	static char* cut(char *str, char*token, bool dir=false);//! cut str at token, return pointer to string after token, if dir==true: search from end
	void toUpper();									//! to capitalize

	bool Replace(const char* a, const char* b, bool dir=false);		//! replace first (last if dir=true) occurence of a with b, return true if successful
	int ReplaceAll(const char* a, const char* b);	//! replace all occurence of a with b, return number of replacements
	int RemoveQuotes();								//! rmove all Quotes (")
	void AdjustForUnix();							//! replace "\" by "/" for linux , solaris, or vise versa
	char* InString(const char* find);				//! return pointer to first occurence of <find>, NULL if not found
	char* InStringLast(const char* find);			//! return pointer to last occurence of <find>, NULL if not found
	char* RemoveBlanks();							//! remove leading blanks
	char* RemoveLeadingBlanks();						//! remove trailing blanks
	char* RemoveTrailingBlanks();					//! 
	bool Extract(const char* start, const char* end, int mode=0);
	bool ExtractReplace();
	CStr TabToSpace(int n);
	void toCapital();								//! capitalize string
	void toLower();									//! convert string tolower
	bool ExtractReplace(const char* start, const char* end, int mode = 0);
	bool replaceEnv();
	bool checkWildcards(char* string);				//! Wildcard pattern matching. THIS: wildcard pattern e.g. *.yini 
	bool isEmpty();
	void check();
	int readFile(FILE* fp);							//! initialize with text file


	CStr  operator+ (CStr str);
	CStr& operator= (const CStr& str);
	CStr& operator= (const char* str);
	operator char*()  { return buf; }

	char *buf;										//! Pointer to string
	char *bufAlloc;									//! allocated buffer
	int len;
	int buflen;
};

#endif // !defined(AFX_YDFPASM_H__CSTR__INCLUDED_)

