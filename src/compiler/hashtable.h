/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

//!	Symbol Table for a CPU simulator
/*! \file 
*/
#ifndef HASHTABLE__H__INCLUDED_
#define HASHTABLE__H__INCLUDED_

// ================================================
//				hashtable.h
// ================================================

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "CStr.h"


// definiert den typ 64bit integer
#ifdef WIN32
	#ifdef __BORLANDC__
	#define	strcasecmp	stricmp
	#else
	#define	strcasecmp	_stricmp 
	#endif
#endif



#define DEFAULT_HASHSIZE 	10000		//Size of Hashtable

/////////////////////////////////////////////
// data elemnts for Hash table
enum token {TOKEN_NONE, TOKEN_UNDEFINED,TOKEN_FNUM,TOKEN_INUM,TOKEN_ADDRESS,TOKEN_ADDRESS_DNL,TOKEN_STRING,
			TOKEN_LIST,TOKEN_OPCODE,TOKEN_MEMORY, TOKEN_IDENTIFIER, TOKEN_TYPE, TOKEN_NAMESPACE, TOKEN_DEFINE
};

class CIdentifier;
class CDataType;
class CStatement;

//!	Class to represent a Symbol Table for a CPU simulator
/*!
*
*	*/
class Cvdt
{
	public:
	Cvdt();
	Cvdt(int token);
	~Cvdt();

	int token;
    bool isEnumElement;
    CStr enumName;          // name befor resolve, in case it is an enum
	double data;
	CStr name;
	CStr filename;
	int linecnt;			// installed at line?
	int total_linecnt;		// consecutive line count (counts also inclueds etc);
	int cases;				// case sensitiv??? -> 1
	long pointer_data;		// pointer data for optimization: //bit[15:0] modulo, bit[21:16] connect, bit[25:22] flags, bit[31]=1 -> pointer data
	long pointer_count;		// is incremented with each pointer access
	CStr valstr;
	CStr shortname;			// name without namespace
	Cvdt *list;				// list entry for .equ lists
	Cvdt *next;				// Next Entry
	CStatement* labeled_statement; //if this cvdt is a label, points to the addicted statement
    CIdentifier *identifier;// pointer to memory struct
	CDataType *datatype;	// pointer to data type
	FILE* fp;				// file pointer
	int active;
};


//!	Class for the Hash table implementation
/*!
*
*	*/
class CHashTable
{
	public:
	int m_hashsize;
	Cvdt **hashtab;
	unsigned hash(const char*s);
	Cvdt* hashlookup(const char* name, int er=0);
	Cvdt* hashinstall(const char* name, int skiperr=0);
	Cvdt* newCvdt(int token);
	CStr m_default;
	bool m_found;


	public:
	CHashTable();
	CHashTable(int size);
	~CHashTable();

	inline bool GetValidy(void) { return(m_found); }
	void SetValue(const char* name, double value, int token=TOKEN_FNUM, int cases=0);
	void SetString(const char* name, const char* string, int token=TOKEN_STRING,int cases=0);
	char* GetString(const char* name);
	double GetValue(const char* name);
	void hashClear();
};

Cvdt* newCvdt(int token);
Cvdt* CvdtAddToEnd(Cvdt* &vdt,const char* name);
Cvdt* CvdtAddToBegin(Cvdt* &vdt,const char* name);
Cvdt* CvdtAddToBegin(Cvdt* &vdt,Cvdt* list);	// insert new vdt listelement at Beginning of list
Cvdt* CvdtAddToBegin(Cvdt* &vdt,double value);	// insert new vdt listelement at Beginning of list
Cvdt* CvdtRemoveFromBegin(Cvdt* &vdt);			// remove first element from list

Cvdt* insertint(const char* name, int value, int cases=0, int skiperr=0);
void insertvalstr(const char* name, const char* valstr, int cases=0);
void insertstr(const char* name, const char* valstr, int cases=0);

#endif //HASHTABLE__H__INCLUDED_



