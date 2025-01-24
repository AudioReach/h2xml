/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////////////
//! \brief					 CDataType class implementation										\file
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CDataType.h"
#include <set>

CStr CDataType::m_unnamed = "";

CDataType::CDataType() 
: m_elements(0)
, m_type(0)
, m_size(0)
, m_hasZeroSizeArrays(false)
, next(0)
, m_cvdt(0)
{
}

CDataType::CDataType(const char* name, CDataType* datatype)
: m_elements(0)
, m_type(0)
, m_size(0)
, m_hasZeroSizeArrays(false)
, next(0)
, m_cvdt(0)
{
	Cvdt* tmp=htab.hashinstall(name);
	tmp->token=TOKEN_TYPE;
	tmp->datatype=new CDataType(); // FIXME: Huh? tmp->datatype=this should be easier
	memmove(tmp->datatype,datatype,sizeof(CDataType));
}

// connect struct elements to struct
// calculate size of struct
void CDataType::insertElements(CIdentifier* ident) {

	m_elements=ident;
	m_size=0;
	m_align=0;
    if (ident->m_attribute) {
        //int dbg = 0;
    }

	for(CIdListIterator it = ident->GetIdList_Begin(); it != ident->GetIdList_End(); it++) {
		it->SetBaseStruct(this);

        if (it->m_attribute) {
            //int dbg = 0;
        }


		long align_actual=32;
		if (it->m_declspec!=NULL && it->m_declspec->datatype!=NULL) {
			align_actual=it->m_declspec->datatype->m_align;
            if (it->m_declspec->datatype->m_hasZeroSizeArrays) {
                m_hasZeroSizeArrays = true;
            }
		} else {
			break;
		}
		int indirection=it->m_pointer_level-it->m_array_dimension;
		if (indirection>0) {
			align_actual=as.pointerSize;									//pointer size in bytetes (default 4 bytes)
		}

		if( align_actual>m_align) {
			m_align=align_actual;
		}
		int size=8* it->GetArraySize();

        if (size == 0) {
            m_hasZeroSizeArrays = true;
        }
		if (it->GetTypeId()==TYPE_ENUM) {
			size=as.enumSize*8;
		}
		if (m_type!=TYPE_UNION){
			if (it->m_bitslice>0) {
				m_size += it->m_bitslice;
			} else {
				if (align_actual>0) m_size=align(m_size,align_actual*8);		// align start of each element
				m_size += size;
			}
		} else {
			if (it->m_bitslice>0) {
				size = it->m_bitslice;
			}
			if (size>m_size) {
				m_size=size;
			}
		}
	}
	m_size = (m_size+7)/8;								// convert bit size in byte size
	// must be aligned to largest element!
	if (m_align>0) m_size = align(m_size, m_align);		// strct size always multiple of 8 Bytes

    if (m_hasZeroSizeArrays) {
        m_size = 0;
    }

	// generate name entry in hash table for parser
	if (m_cvdt!=NULL && m_cvdt->shortname.buf!=NULL) {
        strncpy(tmpBuf, HASH_PREFIX, TMP_BUF_SIZE-1);
		strncat(tmpBuf,m_cvdt->shortname.buf, TMP_BUF_SIZE-100);
		generateName(ident, tmpBuf);
	}
}

// generate name with type.element -> with all subelements
void CDataType::generateName(CIdentifier* ident, char* buf, int count)
{

//	printf ("generatename %s %s\n",buf,ident->m_cvdt->name.buf);
	if (ident!=NULL) {
		int len=strlen(buf);
		CIdentifier* actualID= ident;
		while (actualID!=NULL) {
			buf[len]=0;
			if (actualID->m_cvdt!=NULL) {
				char* tmp=strcat(buf,".");
				if (actualID->m_cvdt->shortname.len != 0) {
					strcat(tmp,actualID->m_cvdt->shortname.buf);
					Cvdt* tmp=htab.hashinstall(buf);
					tmp->identifier=actualID;
					//testtest
					//printf("buffer: %s \n",buf);
				}
			}

			if (actualID->m_declspec==NULL || actualID->m_declspec->datatype==NULL) {
				break;
			}
			CIdentifier* nesteID=actualID->m_declspec->datatype->m_elements;
			int indirection=actualID->m_pointer_level-actualID->m_array_dimension;
			if ( indirection==0 &&(actualID->m_declspec->datatype->m_type== TYPE_STRUCT || actualID->m_declspec->datatype->m_type== TYPE_UNION)) { //
				if (count < 500) //this is a sanitycheck (more than 500 elements in a union/structs	--> something went wrong with parsing	
					generateName(nesteID,buf, count+1);
				else 
					ycc_erpnt(201); //number of parameters do not match
			}
			
			actualID=actualID->m_next;
		}
	}

	/*for(CIdListIterator it = ident->GetIdList_Begin(); it != ident->GetIdList_End(); it++) 
	{
		buf[len]=0;
		generateName(it.current,buf);
		printf("buffer: %s\n",buf);
	}*/
}


void CDataType::initEnum()
{
	std::set<int> values;

	for(CIdListIterator it = m_elements->GetIdList_Begin(); it != m_elements->GetIdList_End(); it++) {
		int enum_value = (int)it->m_cvdt->data;
		values.insert(enum_value);
	/*
		if(values.find(enum_value) == values.end()) {// entry not yet in set
			values.insert(enum_value);
		else{
            //ycc_erpnt(116, enum_value); // FIXME: enums are allowed to contain duplicate values, should be only warning
        
        //it->getDataType()->m_type = TYPE_ENUM;
		*/
	}

	int i=1; // FIXME: this seems to violate the semantics of C's enum type, enums start at 0
	for(CIdListIterator it = m_elements->GetIdList_Begin(); it != m_elements->GetIdList_End(); it++) {
        if (((int)it->m_cvdt->data) == 0) {
            while (values.find(i) != values.end()) i++;
            it->m_cvdt->data = i;
            values.insert(i);
        }
	}
}

CDataType* CDataType::newCDatatype(Cvdt* cvdt, int type)
{
	
	if (cvdt->datatype != NULL && cvdt->datatype->m_type == TYPE_STRUCT){
	}
	//	int dbg=0;
	//	ycc_erpnt(202,cvdt->datatype->GetTypeName());

	if (cvdt->datatype == NULL) {
		CDataType* tmp= new CDataType();
		tmp->m_type=type;	
		tmp->m_cvdt=cvdt;
		tmp->m_size=0;
		tmp->m_align=0;
		cvdt->datatype=tmp;
		cvdt->token=TOKEN_TYPE;
		
		return tmp;
	}
	else {
		return cvdt->datatype;
	}

}

CDataType* CDataType::newCDatatype(int type)
{
	CDataType* tmp= new CDataType();
	tmp->m_type=type;
	tmp->m_size=0;
	tmp->m_align=0;
	return tmp;
}


CDataType* CDataType::newCDataType(const char* name, long type, long size, CIdentifier* m_elements) 
{
	Cvdt* tmp=htab.hashinstall(name);
	tmp->token=TOKEN_TYPE;
	tmp->datatype=new CDataType();
	tmp->datatype->m_type=type;
	tmp->datatype->m_size=size;
	tmp->datatype->m_align=size;
	tmp->datatype->m_cvdt=tmp;
    tmp->datatype->m_elements = m_elements;
    tmp->shortname = name;
    return tmp->datatype;
}
