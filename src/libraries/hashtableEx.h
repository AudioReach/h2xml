/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

//!	Symbol Table for a CPU simulator
/*! \file 
*/
#ifndef HASHTABLEEX__H__INCLUDED_
#define HASHTABLEEX__H__INCLUDED_

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "CStr.h"
#include "CList.h"


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
// data elements for Hash table
enum hashToken {
	 HASH_TOKEN_NONE, HASH_TOKEN_UNDEFINED, HASH_TOKEN_FNUM, HASH_TOKEN_INUM, HASH_TOKEN_ADDRESS, HASH_TOKEN_ADDRESS_DNL,
	 HASH_TOKEN_STRING, HASH_TOKEN_LIST, HASH_TOKEN_OPCODE, HASH_TOKEN_MEMORY, HASH_TOKEN_IDENTIFIER, 
	 HASH_TOKEN_TYPE, HASH_TOKEN_NAMESPACE, HASH_TOKEN_DEFINE
};

//!	Class for the Hash table implementation
/*!
*
*	*/

enum hashError{
	HASH_NO_ERROR,
	HASH_NOT_DEFINED,
	HASH_CASE_NOT_DEFINE,				// not found because of case sensitivity
	HASH_ALREADY_DEFINED
};

template<class T> class CHashTableEx
{
	public:
	CHashTableEx();
	CHashTableEx(int size);
	~CHashTableEx();

	bool m_found;
	int m_hashsize;
	long m_ercode;
	T **hashtab;
	T* m_list_start;
	unsigned hash(const char*s);
	T* hashlookup(const char* name);						// input to hash=string
	T* hashinstall(const char* name, T* np_new=NULL);		// input to hash=string
	T* hashlookup(long hval);								// input to hash=integer
	T* hashinstall(long hval, T* np_new=NULL);				// input to hash=integer
	T* hashinstallInsert(const char* name, T* np_new=NULL, T** list_start=NULL);	// install new elelemt, also connect to beginning of list m_list_start
	T* hashinstallInsert(long hval, T* np_new=NULL);		// install new elelemt, also connect to beginning of list m_list_start
	T* hashinstallInsertEnd(const char* name, T* np_new=NULL, T** list_start=NULL);	// install new elelemt, also connect to END of list list_start (if list_start==NULL) use m_list_start
	T* newT(int token);

	public:


	inline bool GetValidy(void) { return(m_found); }
	void SetValue(const char* name, double value, int token=HASH_TOKEN_FNUM, int cases=0);
	void SetString(const char* name, const char* string, int token=HASH_TOKEN_STRING,int cases=0);
	char* GetString(const char* name);
	double GetValue(const char* name);
	void hashClear();
};
/*
template<class T> struct sHashElement {
	CStr name;
	long hashint;
	int cases;				//0: case insensitive, 1: case sensitive
	int token;
	T* next;
};*/

template<class T> class CHashElement : public CList<T>{
public: 
	CHashElement();			//constructor
	CHashElement(int listElements);
	~CHashElement();		//destruktor
	struct shash {
		CStr name;
		long hashint;
		int cases;				//0: case insensitive, 1: case sensitive
		int token;
		T* next;
	} hash;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//! \brief					 Hashtable Template class implementation							\file
////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
// Constructor and destructor
///////////////////////////////////////////////////////////////////
template<class T> CHashTableEx<T>::CHashTableEx(int size)
{
	m_hashsize=size;
	m_found=false;
	m_list_start=NULL;
	m_ercode=HASH_NO_ERROR;
	hashtab=new T* [m_hashsize];
	for (int i=0;i<m_hashsize;i++)
		hashtab[i]=NULL;
}

template<class T> CHashTableEx<T>::CHashTableEx()
{
	m_found=false;
	m_list_start=NULL;
	m_ercode=HASH_NO_ERROR;
	m_hashsize=DEFAULT_HASHSIZE;
	hashtab=new T* [m_hashsize];
	for (int i=0;i<m_hashsize;i++)
		hashtab[i]=NULL;
}


template<class T> CHashTableEx<T>::~CHashTableEx()
{
		//hashClear();
		//delete[] hashtab;
}

template<class T> void CHashTableEx<T>::hashClear()
{
	for (int i=0;i<m_hashsize;i++){
			if(hashtab[i]!=NULL) 
				delete hashtab[i];
			hashtab[i]=NULL;
	}
}

///////////////////////////////////////////////////////////////////
// liefert Zeiger auf T, falls Eintrag vorhanden, sonst null
///////////////////////////////////////////////////////////////////
template<class T> T* CHashTableEx<T>::hashlookup(const char* name)
{
	m_found=false;
	if (name==NULL) return NULL;
	char* name_in=(char*) name;										// remove preceeding ":" (in label :xyz)
	if (name[0]==':') name_in=(char*) &name[1];
	T *np;
	for (np=hashtab[hash(name_in)];np!=NULL;np=np->hash.next) {
		if(strcmp(name_in,np->hash.name.buf) == 0 && np->hash.cases==0) {	// Case Sensitive
			m_found=true;
			m_ercode=HASH_NO_ERROR;
			return np;											// gefunden
		}
		if(strcasecmp(name_in,np->hash.name.buf) == 0) {				// NOT Case Sensitive
			if (np->hash.cases>0) {
				m_found=true;
				m_ercode=HASH_NO_ERROR;
				return np;										// gefunden
			}
			else {
				m_ercode=HASH_CASE_NOT_DEFINE;
				return NULL;									// Error Message: No match because of case sensitivity!
			}
		}
	}
	m_ercode=HASH_NOT_DEFINED;
	return NULL;												//nicht gefunden
}


///////////////////////////////////////////////////////////////////
// find numerical Hash entry
// liefert Zeiger auf T, falls Eintrag vorhanden, sonst null
///////////////////////////////////////////////////////////////////
template<class T> T* CHashTableEx<T>::hashlookup(long hdata)
{
	m_found=false;
	T *np;
	for (np=hashtab[hdata % m_hashsize ];np!=NULL;np=np->hash.next) {
		if(hdata==np->hash.hashint) {	
			m_found=true;
			m_ercode=HASH_NO_ERROR;
			return np;											// gefunden
		}
	}
	m_ercode=HASH_NOT_DEFINED;
	return NULL;												//nicht gefunden
}


///////////////////////////////////////////////////////////////////
// insert existing element into hash table
// if np_new==NULL generate new element
///////////////////////////////////////////////////////////////////
template<class T> T* CHashTableEx<T>::hashinstall(const char* name, T* np_new)
{
	if (name==NULL) return NULL;
	T* np=NULL;
	unsigned hashval;
	char* name_in=(char*) name;										// remove preceeding ":" (in label :xyz)
	if (name[0]==*":") name_in=(char*) &name[1];
	if ((np=hashlookup(name_in))==NULL){							// not available
		m_ercode=HASH_NO_ERROR;
		if (np_new==NULL) {
			np=newT(0);
		} else {
			np=np_new;
		}
		if(np==NULL )
			printf("Compiler error, cannot create Hashtable entry!\n");
		np->hash.name=name_in;
		hashval=hash(name_in);
		np->hash.next=hashtab[hashval];
		hashtab[hashval]=np;

	} 
	else	
	{
		m_ercode=HASH_ALREADY_DEFINED;
	}
	return np;
}


///////////////////////////////////////////////////////////////////
// insert existing element into hash table
// if np_new==NULL generate new element
// if name does not exist yet: add element to list_start
///////////////////////////////////////////////////////////////////
template<class T> T* CHashTableEx<T>::hashinstallInsert(const char* name, T* np_new, T** list_start)
{
	if (list_start==NULL) {
		list_start=&m_list_start;
	}
	m_ercode=HASH_NO_ERROR;
	T* tmp=hashinstall(name,np_new);
	if (m_ercode==HASH_NO_ERROR) {
		insertInList(lRIGHT,*list_start,tmp );
	}
	return tmp;
}



///////////////////////////////////////////////////////////////////
// insert existing element into hash table
// if np_new==NULL generate new element
// if name does not exist yet: add element to end of m_list_start
///////////////////////////////////////////////////////////////////
template<class T> T* CHashTableEx<T>::hashinstallInsertEnd(const char* name, T* np_new, T** list_start)
{
	if (list_start==NULL) {
		list_start=&m_list_start;
	}
	m_ercode=HASH_NO_ERROR;
	T* tmp=hashinstall(name,np_new);
	if (m_ercode==HASH_NO_ERROR) {
		insertToEnd(lRIGHT,*list_start,tmp );
	}
	return tmp;
}

///////////////////////////////////////////////////////////////////
// liefert Zeiger auf T, neuer Eintrag, falls nicht vorhanden
// numerical input
///////////////////////////////////////////////////////////////////
template<class T> T* CHashTableEx<T>::hashinstall(long hdata, T* np_new)
{
	T *np=NULL;
	m_ercode=HASH_NO_ERROR;
	if ((np=hashlookup(hdata))==NULL){	//Nicht vorhanden
		if (np_new==NULL) {
			np=newT(0);
		} else {
			np=np_new;
		}
		if(np==NULL )
			printf("Compiler error, cannot create Hashtable entry!\n");
		np->hash.hashint=hdata;
		np->hash.next=hashtab[hdata % m_hashsize ];
		hashtab[hdata % m_hashsize]=np;

	} 
	else	
	{
		m_ercode=HASH_ALREADY_DEFINED;
	}
	return np;
}


template<class T> T* CHashTableEx<T>::newT(int token)
{
	T* tmp=new(T);
	tmp->hash.token=token;
	return tmp;
}



///////////////////////////////////////////////////////////////////
// liefert Hash-Wert fuer Hash-Tabelle
///////////////////////////////////////////////////////////////////
template<class T> unsigned CHashTableEx<T>::hash(const char *s)
{
	unsigned hashval=0;
	for (hashval=0;*s != 0;s++)
		hashval=toupper(*s)+31*hashval;		//case insensitiv -> umwandlung in Capitals
	return hashval % m_hashsize;
}


///////////////////////////////////////////////////////////////////
// CHashElement
///////////////////////////////////////////////////////////////////
template<class T> CHashElement<T>::CHashElement()
{
	hash.hashint=0;
	hash.cases=0;				//0: case insensitive, 1: case sensitive
	hash.token=0;
	hash.next=NULL;
	hash.name="";
}

template<class T> CHashElement<T>::CHashElement(int listElements)
	:CList<T>(listElements)			//constructor
{
	hash.hashint=0;
	hash.cases=0;				//0: case insensitive, 1: case sensitive
	hash.token=0;
	hash.next=NULL;
	hash.name="";
}

template<class T> CHashElement<T>::~CHashElement()
{
	/*if (hash.next!=NULL) {
		delete(hash.next);
	}*/
}


#endif //HASHTABLEEX__H__INCLUDED_



