/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

//!	YDFP C Compiler: global functions and variable
/*!
*	\file 
*/

#ifndef GLOBAL_FUNCTIONS_H__INCLUDED_
#define GLOBAL_FUNCTIONS_H__INCLUDED_

#include "CIdentifier.h"
#include "h2xml_global.h"
#include "CExpression.h"
#include "CStatement.h"
#include "CDataType.h"
#include "CIdentifier.h"
#include "h2xml_definitions.h"
#include "CPointer.h"
#include "CList.h"


#define HASH_PREFIX "_parser_"

#define TMP_BUF_SIZE 10000
extern char tmpBuf[TMP_BUF_SIZE];
extern char tmpBuf2[TMP_BUF_SIZE];


////////////////////////////////////////////////////////////////////
// Structure definitions
////////////////////////////////////////////////////////////////////

//! struct for types
struct type_struct {
	CIdentifier* rettype;
	int error;					// 0: no error
	int p_level1;				// 0=normal, 1= pointer, 2=pointer to pointer ... PLEVEL_INC: increment pointer level
	int p_level2;
};

//! struct for list initialisation
struct initializerlist {
	double data;
	CExpression* exp;
    initializerlist* next;
	initializerlist* list;
};


//! struct for memory usage
struct mem_usage_struct {
	CIdentifier* m_base_pt;					//!< base pointer
	long kn_local_pointer_mem;
	long kn_global_pointer_mem;
	long kn_local_struct_mem;
	long kn_local_array_mem;
	long kn_local_mem;
	long kn_global_mem;
	long m_local_cnt;
	CPointer ydfpPointers[10000];
	CIdentifierBlock* m_list_local_read;
	CIdentifierBlock* m_list_local_write;
	CIdentifierBlock* m_list_global_read;
	CIdentifierBlock* m_list_global_write;
	CStatementBlock* m_caller;					// list of all functions that call this function

	long	m_nr_local_pointers;
	int m_nr_local_Regs;
	int m_cur_type;						// used for structs
	CStr m_cur_type_name;
	int m_cur_mem_addr;
	int m_hierarchy;
};
//! struct for storing the current assignment of pointers and regs (after check pointer)



//! struct for the array dimension
struct array_dim_struct {
	long dim;
	array_dim_struct* next;
};


//! struct for the declaration specifier
struct decl_specifiers {
public:
	decl_specifiers(CDataType* dtype, int storage, int tqualifier);

private:
	int storage_specifier;				//!< auto, register,static ,extern, typedef
	int type_qualifier;					//!< const, volatile
public:
	CDataType* datatype;				//!< which kind of memory (fixp24, fixp24s, struct, ...)
	CDataType* datatype2;				//!< which additional kind of memory (signed/unsigned, long int)
	
	CDataType* getDataType()  const { return datatype; };
	CDataType* getDataType2() const { return datatype2;};
	void setDataType(CDataType* dtype);
	void setTypeQualifier(int tqualifier) { type_qualifier = tqualifier; };
	void setStorageSpecifier(int sspecifier);
	bool isTypedef() const;
	bool isExtern() const;
	char *getTypeQualifierString() const;
	char *getStorageSpecifierString() const;
};



////////////////////////////////////////////////////////////////////
// Variable Deklaration: Identifier and Data Types
////////////////////////////////////////////////////////////////////

initializerlist* addInitializer(initializerlist* list, initializerlist* next, double data);

Cvdt* InsertCIdentifier(const char* name, const char* datatype);
Cvdt* InsertCIdentifier(const char* name, const char* datatype,bool isRegister);
typeQualifier* addTypeQualifier(typeQualifier* list,int type);

CIdentifier* newCIdentifier(Cvdt* cvdt);
CIdentifier* newCIdentifier(Cvdt* cvdt,bool isRegister);
CIdentifier* newCIdentifier(CDataType* type, const char* name);
CIdentifier* newCIdentifier(CDataType* type, const char* name,bool isRegister);
CIdentifier* newCIdentifier(CIdentifier* ident);
void		 startStructCompound(int x);							// called whenever a structure compound statement is called
void		 endStructCompound(int x);							// called whenever a structure compound statement is terminated
void		 identifierCreate(CIdentifier* ident);				// called after creation of identifier

CStatement* new_CStatement(int token, CIdentifier* ident=NULL, int linenumber=0);


extern type_struct TypeConvTable[kn_EXP_OPERATORS][kn_TYPES][kn_TYPES];

// Global Function
void insertTypeTable(int i, int j, int k ,CIdentifier* rettype, int error=0, int p_level1=0, int p_level2=0);
void initTypeConvTable();
void ycc_erpnt(int en, const char* message=NULL, int value=-1, int warning=0,int linecnt=-1, char* inputfile=NULL);
void ycc_erpnt(int en, int value);
void initYdfpcc();
void checkTypedef(CIdentifier* id);
void fix_var_names(CDataType* type);
void fix_id_after_struct(CDataType* type, Cvdt* cvd);
void insert_struct_bitslice (CIdentifier* exp, int value);
CIdentifier* fix_noVar_in_struct (int value);

void yyerror (char const *s);
void er(int ercode);  // Error Handling
void init_values();

// YCC functions
void newline_ydfpcc();
void SetDefaults();

static inline long align(long value, long alignment) { return ((value + alignment - 1) / alignment) * alignment; };
void insertEnum(Cvdt* vdt, long64 data);
void insertShortName(CStatement* stat);
void insertShortNameStruct(CStatement* stat);		// for struct xyz {};

Cvdt* findIdentifier(char* identName);

#endif //GLOBAL_FUNCTIONS_H__INCLUDED_

