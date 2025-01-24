/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

#include "CExpression.h"


// a + b, a * b ...
CExpression* CExpression::makeArithBinOp(CExpression* left, long op, CExpression* right, double data, int as_number)
{
	#ifdef _H2XML_DEBUG
	char* tmp_op=OperatorString[op];
	#endif
	
    // check for two constant arguments
	bool check_const=false;
	
	if (left!=NULL && right !=NULL) {
		if (left->m_operator==OP_CONST_INT && right->m_operator==OP_CONST_INT) {
			left->m_data =(long) data;
			left->m_operator=OP_CONST_INT;
			check_const=true;
		} 
		else if ((left->m_operator==OP_CONST_FLOAT && right->m_operator==OP_CONST_FLOAT) ||
				 (left->m_operator==OP_CONST_INT && right->m_operator==OP_CONST_FLOAT)   ||
				 (left->m_operator==OP_CONST_FLOAT && right->m_operator==OP_CONST_FLOAT)){

				left->m_data = data;
				left->m_operator=OP_CONST_FLOAT;
				check_const=true;
		}
	}

    if (left == NULL && right != NULL && (right->m_operator == OP_CONST_INT || right->m_operator == OP_CONST_FLOAT)){
        return right;
    }
    if (check_const) {
    	delete right;
	    return left;
    } else {
	    CExpression* tmp=new (CExpression);
	    tmp->m_operator=op;
	    tmp->m_data=data;
	    tmp->m_operant1=left;
        tmp->m_operant2=right;
		//tmp->checkType();
		//tmp->GetReturnTypeId();
        tmp->assigned_number = as_number;	// insert assigned number
	    return tmp;
	}
}

CExpression* CExpression::makeArithBinOp(CExpression* left, long op, CExpression* right, double data)
{              
	return makeArithBinOp(left, op, right, data, -1);
}

CExpression* CExpression::makeDot(CExpression* left, Cvdt* ident) // struct.x
{
	CExpression* tmp = new CExpression(OP_DOT, left, CExpression::makeIdentifier(ident));
	tmp->checkType();
	tmp->m_rettype->SetCurrentStruct(tmp->m_operant1->m_rettype->m_cvdt);
	return tmp;
}

CExpression* CExpression::makeArrow(CExpression* left, Cvdt* ident) // struct->x
{
	CExpression* tmp = new CExpression(OP_ARROW, left, CExpression::makeIdentifier(ident));
	tmp->checkType();
	return tmp;
}

CExpression* CExpression::makeCommata(CExpression* left, CExpression* right) // exp, exp
{
	return makeArithBinOp(left, OP_COMMATA, right, right->m_data);
}

CExpression* CExpression::makeLogicalOr(CExpression* left, CExpression* right)
{
	return makeArithBinOp(left, OP_LOGIC_OR, right, (long)left->m_data || (long)right->m_data);
}

CExpression* CExpression::makeLogicalAnd(CExpression* left, CExpression* right)
{
	return makeArithBinOp(left, OP_LOGIC_AND, right, (long)left->m_data && (long)right->m_data);
}

CExpression* CExpression::makeArithmeticOr(CExpression* left, CExpression* right)
{
	return makeArithBinOp(left, OP_INCLUSIVE_OR, right, (long)left->m_data | (long)right->m_data);
}

CExpression* CExpression::makeArithmeticXor(CExpression* left, CExpression* right)
{
	return makeArithBinOp(left, OP_EXCLUSIVE_OR, right, (long)left->m_data ^ (long)right->m_data);
}

CExpression* CExpression::makeArithmeticAnd(CExpression* left, CExpression* right)
{
	return makeArithBinOp(left, OP_ANDX, right, (long)left->m_data & (long)right->m_data);
}
// FIXME: Can the number of comparison expressions be reduced, i.e. "exp1 <= exp2" is equal to "!(exp1 > exp2)"
CExpression* CExpression::makeCompEqual(CExpression* left, CExpression* right)
{
	return makeArithBinOp(left, OP_EQUAL, right, left->m_data == right->m_data);
}

CExpression* CExpression::makeCompNotEqual(CExpression* left, CExpression* right)
{
	return makeArithBinOp(left, OP_NOT_EQUAL, right, left->m_data != right->m_data);
}

CExpression* CExpression::makeCompLess(CExpression* left, CExpression* right)
{
	return makeArithBinOp(left, OP_LESS, right, left->m_data < right->m_data);
}

CExpression* CExpression::makeCompGreater(CExpression* left, CExpression* right)
{
	return makeArithBinOp(left, OP_GREATER, right, left->m_data > right->m_data);
}

CExpression* CExpression::makeCompLessEq(CExpression* left, CExpression* right)
{
	return makeArithBinOp(left, OP_LESS_EQ, right, left->m_data <= right->m_data);
}

CExpression* CExpression::makeCompGreaterEq(CExpression* left, CExpression* right)
{
	return makeArithBinOp(left, OP_GREATER_EQ, right, left->m_data >= right->m_data);
}

CExpression* CExpression::makeShiftLeft(CExpression* left, CExpression* right)
{
	return makeArithBinOp(left, OP_SHIFT_LEFT, right, (long)left->m_data << (long)right->m_data);
}

CExpression* CExpression::makeShiftRight(CExpression* left, CExpression* right)
{
	return makeArithBinOp(left, OP_SHIFT_RIGHT, right, (long)left->m_data >> (long)right->m_data);
}

CExpression* CExpression::makePlus(CExpression* left, CExpression* right)
{
	return makeArithBinOp(left, OP_PLUS, right, left->m_data + right->m_data);
}

CExpression* CExpression::makeMinus(CExpression* left, CExpression* right)
{
	return makeArithBinOp(left, OP_MINUS, right, left->m_data - right->m_data);
}

CExpression* CExpression::makeMult(CExpression* left, CExpression* right)
{
	return makeArithBinOp(left, OP_MULT, right, left->m_data * right->m_data);
}

CExpression* CExpression::makeDiv(CExpression* left, CExpression* right)
{
	return makeArithBinOp(left, OP_DIV, right, left->m_data / right->m_data);
}

CExpression* CExpression::makeModulo(CExpression* left, CExpression* right)
{
	return makeArithBinOp(left, OP_MODULO, right, (long)left->m_data % (long)right->m_data);
}

CExpression* CExpression::makePreIncrement(CExpression* left)
{
	// FIXME: Replace this by assignment expression, i.e "exp1 + (exp2++) + exp3" == "exp1 + (exp2 = exp2 + 1) + exp3"
	return makeArithBinOp(left, OP_UNARY_PRE_PLUS_PLUS, NULL, 0);
}

CExpression* CExpression::makePreDecrement(CExpression* left)
{
	return makeArithBinOp(left, OP_UNARY_PRE_MINUS_MINUS, NULL, 0);
}

CExpression* CExpression::makePostIncrement(CExpression* left)
{
	return makeArithBinOp(left, OP_POST_PLUS_PLUS, NULL, 0);
}

CExpression* CExpression::makePostDecrement(CExpression* left)
{
	return makeArithBinOp(left, OP_POST_MINUS_MINUS, NULL, 0);
}

CExpression* CExpression::makeUnaryOp(long op, CExpression* right)
{
    long value = 0;
    if (right->m_operator == OP_CONST_INT){
        switch (op) {
        case OP_UNARY_PLUS:
            value = (long) right->m_data;
            break;
        case OP_UNARY_MINUS:
            value = -(long)right->m_data;
            break;
        case OP_UNARY_NEGATE:
            value = ~((long)right->m_data);
            break;
        case OP_UNARY_INVERT:
            value = !((long)right->m_data);
            break;
        default:
            break;
        }
        right->m_data = value;
    }
	return makeArithBinOp(NULL, op, right,0);
}

CExpression* CExpression::makeAssignmentOp(CExpression* left, long op, CExpression* right)
{
	// FIXME: Directly convert combined arithmetic + assignment op to two expressions, i.e. x+=y -> x = x + y;
	return makeArithBinOp(left, op, right, 0);
}

CExpression* CExpression::makeAssignment(CExpression* left, CExpression* right)
{
	return makeArithBinOp(left, OP_ASSIGN, right, 0);
}

CExpression* CExpression::makeTernaryOp(CExpression* left, CExpression* iftrue, CExpression* iffalse) //x ? a: b
{
	// FIXME, this does not look finished, all subexpressions are missing
	CExpression* tmp=new (CExpression);
	// tmp->m_operator=OP_CONDITIONAL;
	tmp->checkType();
	tmp->m_data = left->m_data ? iftrue->m_data:iffalse->m_data;
    tmp->m_operator = left->m_data ? iftrue->m_operator : iffalse->m_operator;;

	return tmp;
}

CExpression* CExpression::makeArrayAccess(CExpression *left, CExpression *right)
{
	// FIXME: Directly convert this into pointer arithmetic
	return makeArithBinOp(left, OP_ARRAY, right, 0);
}

CExpression* CExpression::makeArrayAccess2(CExpression *left, CExpression *first, CExpression *second)
{
    // Directly convert [expA][expB] to [expA*first_dim + expB] array 
    CExpression* tmpExp1;
    CExpression* tmpExp2;
    
    tmpExp1 =  makeMult (first, makeConstant(left->m_rettype->m_array_dim->dim, 1));
    tmpExp2 =  makePlus(tmpExp1, second);

    return makeArithBinOp(left, OP_ARRAY, tmpExp2 , 0);
    
}

CExpression* CExpression::makeFunCall(CExpression *fn, CExpression *arglist)
{
	return makeArithBinOp(fn, OP_FUNCTION, arglist, 0);
}


// typecast: (int ) xy
CExpression* CExpression::makeTypecast(Cvdt* datatype, long op, CExpression* right)
{
	CExpression* tmp  =new (CExpression);
	CIdentifier* tmp2 =new (CIdentifier);
	
    tmp->m_operator=op;
	tmp->checkType();
	tmp->m_rettype = tmp2;
    tmp->m_rettype->m_cvdt = datatype;
    
    tmp->m_operant1 = right;
    return tmp;
}

// Constant
CExpression* CExpression::makeConstant(double data, long op)
{
	CExpression* tmp=new (CExpression);
	tmp->m_operator=op;
	tmp->m_data=data;
	tmp->checkType();
	return tmp;
}

// Constant-String
CExpression* CExpression::makeStringLiteral(CStr &data)
{
	CExpression* tmp=new (CExpression);
	tmp->m_operator = OP_CONST_STRING;
	//tmp->m_data=data;
	tmp->checkType();
	tmp->m_string = data;
    return tmp;
}


// Identifier
CExpression* CExpression::makeIdentifier(Cvdt* ident, bool err)
{
	CExpression* tmp =new (CExpression);
	
    tmp->m_operator = OP_IDENTIFIER;
    
    tmp->m_rettype=ident->identifier;

	if (tmp->m_rettype==NULL) {
		
		// commented becuase it is not necessary anymore
		// only used as parser
		// ycc_erpnt(100,ident->name.buf);			// identifier undefined
		tmp->m_rettype=as.ident_undef;
	}
    tmp->checkType();

	if (ident->token == TOKEN_UNDEFINED && err) {
		 
		if (ident->identifier == NULL)	
			ycc_erpnt(100,ident->name.buf);
		
	}
    return tmp;
}

// Sizeof
CExpression* CExpression::makeSizeof(Cvdt* ident)
{
	CExpression* tmp =new (CExpression);
	
    tmp->m_operator = SIZEOF;
    
   	CIdentifier* tmp2 = new CIdentifier();
    tmp->m_rettype = tmp2;
    tmp->m_rettype->m_cvdt = htab.hashlookup(ident->name.buf,1);

	if (tmp->m_rettype==NULL) {
		ycc_erpnt(100,ident->name.buf);			// identifier undefined
		tmp->m_rettype=as.ident_undef;
	}
    tmp->checkType();
    return tmp;
}


CExpression* CExpression::makeCopy(const CExpression *copy)
{
	return new CExpression(*copy);
}
