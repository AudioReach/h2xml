//!	Defines a datastructure element for a C-expression
/*! \file 
*/
#ifndef CEXPRESSION__H__INCLUDED_
#define CEXPRESSION__H__INCLUDED_

#include "CIdentifier.h"
/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

#include "CStatement.h"
#include "CDataType.h"
#include "CReg.h"
#include "CFunctionList.h"
#include "global_functions.h"

//!	Class which represents a C-Code expression
/*!
*	Methods to generate YDFP C-code are provided\n
*	Methods to walk throug the linked list and print are provided
*/
class CExpression {
private:
    static int preg_cnt;                //number of used "new" REG
    static bool last_was_preg;          //number of used "new" REG
    static bool break_done;

    static int reg_cnt2;                //number of used "new" REG 
    static int reg_cnt;                 //number of used "new" REG

	long m_operator;

	CExpression* m_operant3;

	CReg *m_regAssign1;
	CReg *m_regAssign2;

	double m_data;						// in case of data
    CStr m_string;				    // in case of string

	bool m_dontAssign;					// don't assign new registers
	bool saveToStack;					// used if expression identifier should be stored to stack

	CExpression* next;					// function argument list
	int assigned_number;
public:
	CExpression();
	CExpression(long op, CExpression *op1=0, CExpression *op2 = 0, CExpression *op3 = 0);
	~CExpression() {};

	CExpression* m_operant1;
	CExpression* m_operant2;

	CIdentifier* m_rettype;				// return type. In case of Identifier -> Identifier 
	static bool pointerToArray;

	//factory methods

private:
	static CExpression* makeArithBinOp(CExpression* left, long op, CExpression* right, double data, int as_number); // a + b, a * b ...
	static CExpression* makeArithBinOp(CExpression* left, long op, CExpression* right, double data);
public:
	static CExpression* makeDot(CExpression* left, Cvdt* ident); // struct.x
	static CExpression* makeArrow(CExpression* left, Cvdt* ident); // struct->x
	static CExpression* makeCommata(CExpression* left, CExpression* right); // exp, exp
	static CExpression* makeLogicalOr(CExpression* left, CExpression* right); // exp || exp
	static CExpression* makeLogicalAnd(CExpression* left, CExpression* right); // exp && exp
	static CExpression* makeArithmeticOr(CExpression* left, CExpression* right); // exp | exp
	static CExpression* makeArithmeticXor(CExpression* left, CExpression* right); // exp ^ exp
	static CExpression* makeArithmeticAnd(CExpression* left, CExpression* right); // exp & exp
	static CExpression* makeCompEqual(CExpression* left, CExpression* right);
	static CExpression* makeCompNotEqual(CExpression* left, CExpression* right);
	static CExpression* makeCompLess(CExpression* left, CExpression* right);
	static CExpression* makeCompGreater(CExpression* left, CExpression* right);
	static CExpression* makeCompLessEq(CExpression* left, CExpression* right);
	static CExpression* makeCompGreaterEq(CExpression* left, CExpression* right);
	static CExpression* makeShiftLeft(CExpression* left, CExpression* right);
	static CExpression* makeShiftRight(CExpression* left, CExpression* right);
	static CExpression* makePlus(CExpression* left, CExpression* right);
	static CExpression* makeMinus(CExpression* left, CExpression* right);
	static CExpression* makeMult(CExpression* left, CExpression* right);
	static CExpression* makeDiv(CExpression* left, CExpression* right);
	static CExpression* makeModulo(CExpression* left, CExpression* right);
	static CExpression* makePreIncrement(CExpression* left);
	static CExpression* makePreDecrement(CExpression* left);
	static CExpression* makePostIncrement(CExpression* left);
	static CExpression* makePostDecrement(CExpression* left);
	static CExpression* makeUnaryOp(long op, CExpression* right);
	static CExpression* makeAssignmentOp(CExpression* left, long op, CExpression* right);
	static CExpression* makeAssignment(CExpression* left, CExpression* right);
	static CExpression* makeTernaryOp(CExpression* left, CExpression* iftrue, CExpression* iffalse);
	static CExpression* makeArrayAccess(CExpression *left, CExpression *right);
	static CExpression* makeArrayAccess2(CExpression *left, CExpression *first, CExpression *second);	
    static CExpression* makeFunCall(CExpression *fn, CExpression *arglist);
	static CExpression* makeTypecast(Cvdt* datatype, long op, CExpression* right);
	static CExpression* makeConstant(double data, long op);
	static CExpression* makeStringLiteral(CStr &data);
	static CExpression* makeIdentifier(Cvdt* ident, bool err=false);
	static CExpression* makeSizeof(Cvdt* ident);
	static CExpression* makeCopy(const CExpression *copy);

public:
	//functions
	CExpression* set_next(CExpression* next);				// add <next> after this statement
	void startStruct();										// begin of struct access: start module statement
	void endStruct();										// end of struct evaluation
	char* emit(char* buf, CExpression* op1, CExpression* op2, const char* op, int coperator, int mode, int saveStack);
	char* emitAll(char* buf, int mode, int saveStack);			
	char* emitAll(char* buf, int mode, int saveStack, bool getIdent, bool dontAssign	);
	CStr emitAsm(char* buf, CExpression* op1, CExpression* op2, const char* op, int coperator, int mode, int saveStack, bool &setInstr);	//!< prints assembler
	char* emitAsmAll(char* buf, long calling_operator, int saveStack, bool &overrideOP, bool &reEmitP, bool &setInstr);																//!< prints assembler
	char* emitReg(char* buf, CExpression* op1, CExpression* op2, const char* op, int coperator, int mode, int saveStack,cur_mem_assignment *mem_assignment);
	char* emitRegAll(char* buf, long calling_operator, int saveStack, bool &overrideOP, bool &reEmitP,cur_mem_assignment *mem_assignment);
    int emit_reg_include(char* type, char* name, bool is_preg);
    int toYdfpcc(int previous_op, CStatement* connect);					//!< expand into separate items
	int checkTerminal(CStatement* connect,int previous=0);
	int checkExpression(CStatement* connect);
    bool checkBreak(int previous_op);									//!< true, if accu pipeline wil be interrupted 
	void insertBreak(CStatement* connect, int incr=1);
	void exchangeOperants(int previous_op);								//!< exchange left and right operant, if more effective
	double getConstant();												//!< return constant expression, error if not constant
	void checkType();                                                    //!<check types for expressions
	void allocatePointers(mem_usage_struct* mem_usage, int &lastPointer, int &lastReg);	//!< allocate memory for all variables in expression	
	void generateDependencyList(CStatement* curStatement, int last_operator, int branch);	//!<  generates dependency list
	CFunctionList* generateFunctionList(CStatement* curStatement,CIdentifier *cur_function,CFunctionList *last_function);	//!<  generates the function dependency list
	void updatePointers(int pointerOffset,int regOffset);	//!< updates the pointer numbers depending on used pointers in subfunction 
	int checkPointer(int start_pointer_nr,int struct_pointer, int start_register_nr, int &saveStack, int &codeline_cnt,bool &use_conditional_reg,cur_mem_assignment *mem_assignment);	//!< assign available pointer number to pointer 
	int checkRegister(int curType);	// checks the register number for available Registers
	int getRetType(CExpression* exp);
	void generateConsumer(CExpression *exp,CStatement* curStatement);		//!<  generates consumers for dependency list
	void assignRegs(mem_usage_struct *mem_usage);		//!< assign mem_usage struct to inserted regs
	CStr emitPointer(CStr &pointerAssignString, CExpression* m_operant1, CExpression* m_operant2, const char* op, int saveStack);
	long GetReturnTypeId() const;
	long GetReturnTypeSize() const;
	char* GetReturnTypeName() const;
	CStr& GetReturnTypeNameStr() const;
	CStr GetReturnTypeNameStrAsm() const;
	int GetReturnTypeToken() const;
	double GetData() const { return m_data; };
	void RecursiveSetDontAssign();
	void toYdfpcc_IF(CStatement *connect, CIdentifier *rettype);
	CDataType* getReturnDataType() const;
	bool isIdentifier() const { return m_operator == OP_IDENTIFIER; };
	long GetOperator() const { return m_operator;};
};

#endif //CEXPRESSION__H__INCLUDED_

