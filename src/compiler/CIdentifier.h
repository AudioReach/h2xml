/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

//!	Defines a datastructure element for a C-identifier
/*! \file 
*/
#ifndef CIDENTIFIER__H__INCLUDED_
#define CIDENTIFIER__H__INCLUDED_

#include "global_functions.h"
#include "h2xml_common.h"
#include "hashtable.h"
#include "CStr.h"
#include "h2xml_global.h"
#include "CDataType.h"
#include "compilerYaccyy.hpp"
#include "CDependencyList.h"
#include "CFunctionList.h"
#include "CValue.h"
//#include <list>
//#include <string>

class CIdentifier;

//FIXME: TEMPORARY WRAPPER
class CIdListIterator {
private:

public:
	CIdentifier *current;
	CIdListIterator(CIdentifier *id) : current(id) {};
	bool operator==(const CIdListIterator &rhs) const { return current == rhs.current; };
	bool operator!=(const CIdListIterator &rhs) const { return current != rhs.current; };
	CIdentifier& operator*() const { return *current; };
	CIdentifier* operator->() const { return current; };
	CIdListIterator& operator++();
	CIdListIterator& operator--();
	CIdListIterator operator++(int);
};
/*
typedef std::list<CIdentifier *>::iterator CIdListIterator;
class CIdentifierList : public std::list<CIdentifier *>
{
public:
	//FIXME: TEMPORARY WRAPPER
	CIdListIterator GetFunArgIds_Begin() { return CIdListIterator(next_for_funct_in_struct); };
	CIdListIterator GetFunArgIds_End() { return CIdListIterator(NULL); };
	void SetFunArgIds(CIdListIterator args) { next_for_funct_in_struct = &(*args); };
	CIdListIterator GetIdList_Begin() { return CIdListIterator(this); };
	CIdListIterator GetIdList_Next() { return CIdListIterator(this->next); };
	CIdListIterator GetIdList_End() { return CIdListIterator(NULL); };
}
*/
//!	Class which represents a C-Code identifier
/*!
*	Methods to walk throug the linked list and print are provided
*/
struct typeQualifier;
struct array_dim_struct;
class CDefaultList;
class CIdentifier {
	friend class CIdListIterator;
public:
	CIdentifier* next_for_funct_in_struct;
    CIdentifier* m_previous;
	CIdentifier* m_next;

	CStatement* m_function;				// if function definition: points to function body
										// else points to associated function definition 
	CIdentifier* m_calling_function;	// if function, points to callinf function

	//bool m_idconst;						// constant value (e.g. const pointer)

	CDataType* m_base_struct;			// if this Identifier is a struct meber: point to struct
	Cvdt* m_current_struct;	            // if this Identifier is a struct meber: point to current-struct // FIXME: is this identical to m_base_struct->m_cvdt ?

	long m_scope;						// local, global, rotating
	initializerlist* m_init;			// initialization list
	int m_array_dimension;				// dimension of array: 0: no array
	long m_elements;					// number of elements

	//CIdentifier* m_base_pti;			// deklaraton relative to this pointer
	//long m_offset;						// offset to base pointer

public:
	long m_token;						// variable, function, parameter ....

	Cvdt* m_cvdt;						// pointer to hash table entry, including name etc
	decl_specifiers* m_declspec;		// auto, register,static ,extern, typedef,const, volatile,which kind of memory (fixp24, fixp24s, struct, ...)
	

	long m_pointer_level;				// *x -> level=1; **x -> level=2 ...
	typeQualifier *m_pt_type_qual;		// pointer type qualifier list: int *const volatile *const *xyz
	array_dim_struct* m_array_dim;		// dimension of array



	mem_usage_struct* m_mem_usage;		// points to memory usage struct of function definition
	


	//after mapping
	//	long m_pti_base;				// pointer image base
	//	long m_pti;						// pointer immage
	//	long m_pti_offset;				// offset to pointer immage


	CDependencyList* last_producer;
	CDependencyList* last_consumer;
	CProducerList *last_producerList;
	CFunctionList *functionList;

	int m_hierarchy;					// hierarchy of if/else/for... calls
	int m_memAddr;						// assigned relative memory address
	bool m_isRegister;					// ==1 if identifier is a register
	bool isGlobal;						// ==1 if identifier is global
	int m_assignedPointer;				// assigned pointer number (for arrays)
	int m_bitslice;						// amount of bits in bitslice (for structs)
    int m_bitoffset;					// bit offset within parent element;
	int m_assignedRegister;				// mapped register
	int m_currentAssignedRegister;		// currently assigned register
	bool m_is_termination_function;		// if identifier is a function, ==1 if this is a termination function (no other function calls inside)
	int m_pointer_start_nr;				// pointer start number after all subfunctions have assigned pointers
	int m_reg_start_nr;					// reg start number after all subfunctions have assigned their registers
	int m_nr_total_use;					// counts usage of identifier inside a function
	bool m_updated;						// set if reg/pointer was updated
	bool m_isOffset;					// set if used as offset variable
	int	m_functionParameter;			// ==1 if the Identifier is a function parameter
	bool forceToStack;					// ==1 if variable has to be stored on stack
	int m_attribute;		// e.g. packed or aligned

    char* m_fileName;
    long m_linecnt;
	
//	asmHeaderIdentifier_s ah;			// info for asm header
	CAnnotationData* m_xD[kn_ANGROUP];		// XML atrributes associated with this identifier

	CIdentifier() ;
    CIdentifier(bool init);
	~CIdentifier() {};
	// fuctions
	CIdentifier* set_next(CIdentifier* snext);
    CIdentifier* set_last(CIdentifier* snext);  // last element in chain

	CIdentifier* set_next(CIdentifierBlock* snext);

	char* EmitDeclaration(char* buf);
	void DoEmit(char* buf,const char* str);
	void DoEmit(CStr* buf, const char* str);
	char* EmitAsmDeclaration(char* buf) ;
	void insertIdentifierInit(double value);		// add value to initialization list
	void insertArrayDimension();
	void insertArrayDimension(int dimension);
	void checkParameter(CIdentifier* ident2);		// check function parameters
	void clearFunctionParameter();					// for function deklaration xy(fix24 c); -> variable c is not declared!

	int getLastUseAsConsumer(int def = 0) const; // get the last location where this identifier has been used as a consumer
	int getLastUseAsProducer(int def = 0) const; // get the last location where this identifier has been used as a producer
	int getLastUse(int def = 0) const; // get last location where the identifier has been used
	const char *getName() const;

	/* Append a CProducerList entry to the end of lastProducerList */
	void AppendToLastProducerList(CProducer *producer);
	//FIXME: TEMPORARY WRAPPER
	CIdListIterator GetFunArgIds_Begin() { return CIdListIterator(next_for_funct_in_struct); };
	CIdListIterator GetFunArgIds_End() { return CIdListIterator(NULL); };
	void SetFunArgIds(CIdListIterator args) { next_for_funct_in_struct = &(*args); };
	CIdListIterator GetIdList_Begin() { return CIdListIterator(this); };
	CIdListIterator GetIdList_Next() { return CIdListIterator(this->m_next); };
	CIdListIterator GetIdList_End() { return CIdListIterator(NULL); };

	void SetCallingFunction(CIdentifier *id) { m_calling_function = id; };
	CIdentifier* GetCallingFunction() const { return m_calling_function; };
	void SetFunctionStatement(CStatement *fun) { m_function = fun; };
	CStatement* GetFunctionCompoundStatements() const;
	mem_usage_struct* GetFuncMemUsage() const;
	void adaptInitializerList(initializerlist* list);
	void allocatePointers(mem_usage_struct* mem_usage, int &lastPointer, int &lastReg);
	bool isFunction() const { return m_function != NULL; };
	long GetTypeId() const;
	long GetTypeSize() const;
	long GetTypeAlign() const;
	long GetArraySize() const { return GetTypeSize() * m_elements; };
	long GetArrayDimension() const { return m_array_dimension; };
	char* GetTypeName()  const;
	char* GetTypeName2() const;
	CStr& GetTypeNameStr() const;
	int GetTypeToken() const;
	bool isLocalScope() const { return m_scope == SCOPE_LOCAL; };
	bool isGlobalScope() const { return m_scope == SCOPE_GLOBAL; };
	void SetScope(long scope) { m_scope = scope; };
	bool isArray() const { return m_elements > 1; };
	bool isArray2() const { return m_array_dimension > 0; }; // FIXME: do we really need two array tests, is this equal to isArray()? 
	bool isPointer() const { return m_pointer_level > 0; };
	void SetCurrentStruct(Cvdt* current_struct) { m_current_struct = m_cvdt; }; // FIXME: is m_current_struct identical to m_base_struct->m_cvdt?
	bool isStructElement() { return m_current_struct != 0; }; // is this identifier part of a structure?
	void SetBaseStruct(CDataType *base) { m_base_struct = base; };
	bool HasBaseStruct() const { return m_base_struct != NULL; };
	bool isMemberOfStruct(CDataType *str) const { return m_base_struct == str; };
	CDataType* getDataType()  const;
	CDataType* getDataType2() const;

	void addname(CStr* name, eAnGroups xmlTag);
	char* getshortname(eAnGroups xmlTag);

	// xml functions
    void emitEnum(CXMLTag* xmlTag, eAnGroups anGroup, CStr * nspace, int mergedTagCnt, bool firstcall);
    void emitData(CXMLTag* xmlTag, eAnGroups anGroup, long &address, long &addressNoBitfield, CStr * nspace, char* typeName, int mergedTagCnt, CDefaultList* ptDefaultBuffer, bool firstcall, int compileAttributes);
    void readDefaultFile(eAnGroups anGroup, CDefaultList * &ptDefaultList, long address, long mergedTagCnt, bool &hasConditionalDefaultFile, bool &hasDefaultFile);
    bool checkRanges(eAnGroups anGroup, long &address, int mergedTagCnt, CDefaultList* ptDefaultBuffer, bool firstcall, int compileAttributes);
    bool checkRangeType(CAnnotationData* data); /* check if element type is compatible with data type */
    long m_bitSizeElement;
    long m_arraySize;
    long m_arrayElements;

    long m_addr_increment;
    int m_indirection;
    long m_align;
    long m_anExpandArray;
    long m_anExpandArrayOffset;
    long m_anExpandStructs ;
    long m_anExpandTypedefs;

    CValue m_typeMin;
    CValue m_typeMax;
    bool m_isUnsigned;
    bool m_isArray;
    bool m_isStruct;
    CDefaultList* m_defaultList;
    bool m_hasConditionalDefaultFile;
    bool m_hasDefaultFile;

    CIdentifier* m_identBitField;
};




#endif //CIDENTIFIER__H__INCLUDED_

