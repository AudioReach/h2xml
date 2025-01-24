/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

//!	Defines a datastructure element for a C-statement
/*! \file 
*/
#ifndef CSTATEMENT__H__INCLUDED_
#define CSTATEMENT__H__INCLUDED_


#include "CIdentifier.h"
#include "CExpression.h"
#include "CDependencyList.h"
#include "CFunctionList.h"

//!	Class which represents a C-Code statement
/*!
*	Methods to insert/delete into/from a linked list are provided\n
*	Methods to walk throug the linked list and print are provided
*/



class CStatement {
public:
	CStatement() ;
	~CStatement() {};


	// variables	 
	int m_token;						//!< token number for the actual statement
	char* m_codeline;					//!< copy of original codeline, for debug purpos needed
	int m_codeline_cnt;					//!< codeline number of original codeline
	CExpression* m_exp1;				
	CExpression* m_exp2;
	CExpression* m_exp3;
	CIdentifier* m_identifier;
	Cvdt* m_module_name;
	static int m_lastPointer;
	static int m_lastReg;
	static int m_ifCount;
	bool m_use_cond_reg;

	CStatement* list;					//!< start pointer to the compound statements
	CStatement* previous;				//!< pointer to the previous statements in the linked list
	CStatement* next;					//!< pointer to the next statements in the linked list
	CStatement* branch_to;
	CStatement* branch_from;
	cur_mem_assignment* m_mem_assignment;	//!< stores the current memory assignments of pointers and regs
	mem_usage_struct* m_mem_usage;		//!< struct for memory usage


	CDependencyList* Consumer;
	CDependencyList* Producer;
	CIdentifier *m_cur_function;
	CIdentifier *m_last_function;

	int m_save_stack;					//!< set if the stack has to be saved in previous function
//	asmHeaderStatement_s ah;			// additional info for assembler header
	
	// functions
	CStatement* set_next(CStatement* next);				//!< add <next> after this statement into linked list
	CStatement* set_next(CStatementBlock* snext);		//!< add <block> after this statement into linked list
	CStatement* insertBefore(int token) ;
	void insertCompound(CIdentifier* fname, CStatement* compound);
	
	void remove();										//!< remove from list
	void emit();										//!< print this statement (to file: FILE* m_filept)
	void emitAll();										//!< print all statements (to file: FILE* m_filept)
	void emitCompound(CStatement* l);					//!< print compound statements (to file: FILE* m_filept)
	void doEmit(const char* buf);
	void doLabelEmit(const char* buf);
	void emitAsm();								//!< prints this statement in assembler to file *.yasm
	void emitAsmAll();							//!< prints all statements in assembler to file *.yasm
	void emitRegAll();
	void emitReg();
	void emitRegCompound(CStatement* l,bool functionStart);
	void emitAsmCompound(CStatement* l,bool functionStart);					//!< print compound statements in assembler to file *.yasm
	void allocate_memory();
	void allocate_ptis();
	void allocate_pointer();
	void clearLocalPointers(mem_usage_struct* m_mem_usage);
	
	void toYdfpcc();				//!< expand expression into separate items
	void toYdfpccAll();				//!< expand all expression into separate items
	
	// memory management
	void allocateMem(mem_usage_struct* mem_usage);				//!< allocates local memory
	void allocateMemAll(mem_usage_struct* mem_usage);			//!< allocates local memory
	void allocatePointers(mem_usage_struct* mem_usage);			//!< allocates pointer to memory
	void allocatePointersAll(mem_usage_struct* mem_usage);		//!< allocates pointer to memory
	
	// memory management
	void allocateGlobalMem(mem_usage_struct* mem_usage);		//!< allocates global memory
	void allocateGlobalMemAll(mem_usage_struct* mem_usage);		//!< allocates global memory

	void generateDependencyList();								//!< generates dependency list
	void generateDependencyListAll();							//!< generates dependency list
	void printDependencyListAll(FILE *dependencyOut);			//!< prints dependency list
	void printDependencyList(FILE *dependencyOut);				//!< prints dependency list

	void generateFunctionList(CIdentifier *cur_function);		//!< checks number of used pointers in function
	void generateFunctionListAll(CIdentifier *cur_function);	//!< checks number of used pointers in function
	void updatePointer(int pointerOffset,int regOffset);						//!< updates pointer numbers to avoid reloading due to subfunction
	void updatePointerAll(int pointerOffset,int regOffset);					//!< updates pointer numbers to avoid reloading due to subfunction
	void checkPointer(int pointer_nr, int register_nr,bool use_conditional_reg);						//!< checks and resets pointer numbers, if > NR_POINTERS
	void checkPointerAll(int pointer_nr, int register_nr,bool use_conditional_reg);					//!< checks and resets pointer numbers, if > NR_POINTERS
	void setDontAssign(CExpression *ex);						//! don't assign new registers for right hand expressions

	int getCodeline() const { return m_codeline_cnt; }; // returns the codeline in which this statement appeared
	void addRightmostToConsumer(CDependencyList * atRight);	// add element atRight as the rightmost element of the Consumer dependency list

	// static variables
	//static int tabs;									//!< how many tabs before line
	static FILE* m_filept;								//!< pointer to emit output file
	static FILE* m_filept2;								//!< pointer to emit output file
    static int m_functionRamAddress;			//! start address of stack for current function, used for assigning absolute memory address of identifier

};

#endif //CSTATEMENT__H__INCLUDED_

