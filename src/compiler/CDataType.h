/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

//!	Defines a datastructure element for a C-datatype
/*! \file 
*/
#ifndef CDATATYPE__H__INCLUDED_
#define CDATATYPE__H__INCLUDED_

#include "hashtable.h"
#include "CIdentifier.h"

//!	Class which represents a C-Code datatype
/*!
*	
*/
class CDataType {
public:
	CDataType(); // prevent constructor from being called

	CIdentifier* m_elements;		// struct, union, ..
	long	m_type;					// basic type, e.g. fixp24
	long	m_size;					// number of words
    bool    m_hasZeroSizeArrays;    // indicates that any substructure or element has undfined size
	CDataType* next;
public:
	CDataType(const char* name, CDataType* datatype); 
	~CDataType();	

	Cvdt*   m_cvdt;					// pointer to hash table entry, including name etc

	// functions
	void insertElements(CIdentifier* ident);
	void generateName(CIdentifier* ident, char* buf, int count=0);

	void initEnum();
	CIdentifier* GetElements() const { return m_elements; };
	long	m_align;
	long GetTypeId() const { return m_type; };
	long GetTypeSize() const { return m_size; };
	long GetTypeAlign() const { return m_align; };
    char* GetTypeName() const {
        if (m_cvdt) { return m_cvdt->name.buf; }
        else { return NULL; }
    };
    CStr& GetTypeNameStr() const {
        if (m_cvdt) { return m_cvdt->name; }
        else { return m_unnamed; }
        };

    CStr& GetTypeShortNameStr() const {
        if (m_cvdt) { return m_cvdt->shortname; }
        else { return m_unnamed; }
    };
	int GetTypeToken() const { return m_cvdt->token; };

	static CDataType *newCDataType(const char* name, long type, long size, CIdentifier* m_elements=NULL);
	static CDataType *newCDatatype(Cvdt* cvdt, int type);
	static CDataType* newCDatatype(int type);

private:
    static CStr m_unnamed;      // empty string, used as return value if name is not available
};


#endif //CDATATYPE__H__INCLUDED_

