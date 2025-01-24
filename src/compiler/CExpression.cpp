/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////////////
//! \brief					 CExpression class implementation								   \file
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CExpression.h"
#include "CProducerList.h"



extern int exprCount;
extern int firstrun;
extern int tabs;
extern bool generateAssign;		// generateAssign is set with commandline option -assign
//CPointer ydfpPointer[NR_POINTERS];
// add  next after this statement

bool CExpression::last_was_preg = false;
bool CExpression::break_done = false;
bool CExpression::pointerToArray = false;
int  CExpression::preg_cnt = 0;
int  CExpression::reg_cnt = 0;
int  CExpression::reg_cnt2 = -1;
int  actualPointer = -1;
int  lastReg = 0;
bool isDereference = false, isDeref = false, storeCopyReg = false, forceRegIncrement = false, cannotUseStack = false, fromPointer = false;
char tmpEmit[1000];
extern int decr;
CStr toCopyReg1, toCopyReg1Name, toCopyReg2, toCopyReg2Name, curWidth, tmpStackCopy;

CExpression::CExpression()
: m_operator(0)
, m_operant3(0)
, m_regAssign1(0)
, m_regAssign2(0)
, m_data(0.)
, m_string()
, m_dontAssign(false)
, saveToStack(false)
, next(0)
, assigned_number(-1)
, m_operant1(0)
, m_operant2(0)
, m_rettype(0)
{
}

CExpression::CExpression(long op, CExpression *op1, CExpression *op2, CExpression *op3)
: m_operator(op)

, m_operant3(op3)
, m_regAssign1(0)
, m_regAssign2(0)
, m_data(0.)
, m_string()
, m_dontAssign(false)
, saveToStack(false)
, next(0)
, assigned_number(-1)
, m_operant1(op1)
, m_operant2(op2)
, m_rettype(0)
{
}

CExpression* CExpression::set_next(CExpression* snext)
{
	next = snext;
	if (snext != NULL) {
		//snext->previous=this;
		return snext;
	}
	else {
		return this;
	}
}

void CExpression::checkType()
{
#ifdef _H2XML_DEBUG
	char* tmp_op = OperatorString[m_operator];
#endif
	int index1 = 0;
	int index2 = 0;


	if (m_operant1 != NULL && m_operant1->m_operator != SIZEOF && m_operant1->m_operator != OP_TYPECAST && m_operant1->m_operator != OP_TYPECASTP)
		index1 = m_operant1->GetReturnTypeId();

	if (m_operant2 != NULL && m_operant2->m_operator != SIZEOF && m_operant2->m_operator != OP_TYPECAST && m_operant2->m_operator != OP_TYPECASTP)// &&
		//m_operant2->m_operator!=OP_POST_PLUS_PLUS && m_operant2->m_operator!=OP_POST_MINUS_MINUS)
	{
		index2 = m_operant2->GetReturnTypeId();
	}

	switch (m_operator) {
	case OP_IDENTIFIER:
	case SIZEOF:
		//case OP_POST_PLUS_PLUS:
		//case OP_POST_MINUS_MINUS:
		//case OP_TYPECAST:
		break;
	case OP_FUNCTION: {
						  m_rettype = m_operant1->m_rettype;
						  // compare types with declaration

						  if (m_operant1->m_rettype->m_token == IDENTIFIER_FUNCTION || m_operant1->m_rettype->m_token == IDENTIFIER_FUNCTION_NOARGS) {
							  bool err = false;

							  CIdListIterator tmp1 = m_operant1->m_rettype->GetIdList_Next();  // parameter-list in function-declaration
							  CIdListIterator tmp1_end = m_operant1->m_rettype->GetIdList_End();
							  CExpression* tmp2 = m_operant2;                   // parameter-list in function-call

							  while (tmp1 != tmp1_end && tmp2 != NULL && tmp2->m_rettype != NULL && tmp2->m_rettype->m_declspec != NULL && err == false) {
								  index1 = tmp1->GetTypeId();
								  index2 = tmp2->GetReturnTypeId();
								  //if (memcmp(tmp1->m_declspec,tmp2->m_rettype->m_declspec,sizeof(decl_specifiers))!=0) {
								  //	err=true };
								  err = TypeConvTable[OP_ASSIGN][index1][index2].error!=0; //check if an assign would be allowed
								  tmp1++;
								  tmp2 = tmp2->next;
							  }

							  if (err) {
								  ycc_erpnt(105, m_operant1->m_rettype->m_cvdt->name.buf); //wrong types
							  }
							  else if (tmp1 != NULL || tmp2 != NULL) {
								  ycc_erpnt(104, m_operant1->m_rettype->m_cvdt->name.buf); //number of parameters do not match
							  }
						  }
						  else {
							  ycc_erpnt(103); // not a function
						  }
	}
		break;
	case OP_DOT: {
					 m_rettype = as.ident_undef;	//default
					 if ((index1 == TYPE_STRUCT || index1 == TYPE_UNION) && !m_operant1->m_rettype->isPointer()){
						 // check if member of struct
						 if (m_operant2->m_rettype->isMemberOfStruct(m_operant1->getReturnDataType())) {
							 m_rettype = m_operant2->m_rettype;
						 }
						 else {

							 // removed type checking for structs
							 // as we are only using as parser at the moment,
							 // this is not needed.
							 //ycc_erpnt(108,m_operant1->GetReturnTypeName());
							 m_rettype = m_operant2->m_rettype;

						 }
					 }
					 else {
						 if (m_operant1->m_rettype->m_cvdt != NULL){
							 ycc_erpnt(106, m_operant2->m_rettype->m_cvdt->name.buf);
						 }
						 else {
							 ycc_erpnt(106, "");
						 }
					 }
	}
		break;
	case OP_ARROW: {
					   m_rettype = as.ident_undef;	//default
					   if ((index1 == TYPE_STRUCT || index1 == TYPE_UNION) && m_operant1->m_rettype->m_pointer_level == 1){
						   // check if member of struct
						   if (m_operant2->m_rettype->isMemberOfStruct(m_operant1->getReturnDataType())) {
							   m_rettype = m_operant2->m_rettype;
						   }
						   else {
							   ycc_erpnt(108, m_operant1->GetReturnTypeName());
						   }
					   }
					   else {
						   if (m_operant1->m_rettype->m_cvdt != NULL){
							   ycc_erpnt(107, m_operant2->m_rettype->m_cvdt->name.buf);
						   }
						   else {
							   ycc_erpnt(107, "");
						   }
					   }
	}
		break;
	case OP_UNARY_STAR: {
							m_rettype = as.ident_undef;	//default
							if (m_operant2->m_rettype->isPointer()){
								m_rettype = newCIdentifier(m_operant2->m_rettype);
								m_rettype->m_pointer_level--;
							}
							else {
								ycc_erpnt(111);
							}
	}
		break;
	case OP_ARRAY: {
					   m_rettype = as.ident_undef;	//default
					   if (m_operant1->m_rettype->isPointer()){
						   m_rettype = newCIdentifier(m_operant1->m_rettype);
						   m_rettype->m_pointer_level--;
					   }
					   else {
						   ycc_erpnt(110);
					   }
	}
		break;
	default: {
				 m_rettype = TypeConvTable[m_operator][index1][index2].rettype;
				 int error = TypeConvTable[m_operator][index1][index2].error;
				 if (error != 0) {
					 char errorstr[1000] = "";
					 if (m_operant1 != NULL)
						 strcat(errorstr, Typenames[m_operant1->GetReturnTypeId()]);
					 strcat(errorstr, OperatorString[m_operator]);
					 if (m_operant2 != NULL)
						 strcat(errorstr, Typenames[m_operant2->GetReturnTypeId()]);
					 ycc_erpnt(error, errorstr);
				 }
	}
		break;
	}
}

void CExpression::startStruct()
{
	as.ycc_structname = GetReturnTypeName();
	//insert_module(GetReturnTypeName(),1);
}

void CExpression::endStruct()
{
	as.ycc_structname = NULL;
	//insert_module(GetReturnTypeName(),1);
}

char* CExpression::emit(char* buf, CExpression* op1, CExpression* op2, const char* op, int coperator, int mode, int saveStack)
{
	/*if(emit_pointer)
	{
	strcat(buf,"\t\t\t\t\t\t//(");
	}
	else
	{*/
	strcat(buf, "(");
	//}
	if (op1 != NULL) {
		op1->emitAll(buf, mode, saveStack);
	}
	isDeref = isDereference;
	strcat(buf, " ");
	strcat(buf, op);
	strcat(buf, " ");
	if (op2 != NULL) {
		op2->emitAll(buf + strlen(buf), mode, saveStack);
	}

	if (coperator == OP_ARRAY) {
		strcat(buf, "]");
	}

	/*if(!emit_pointer)*/strcat(buf, ")");
	return buf;
}

CStr CExpression::emitPointer(CStr &pointerAssignString, CExpression* m_operant1, CExpression* m_operant2, const char* op, int saveStack)
{
	char nBuf[1000] = "";
	char nBuf2[1000] = "";
	char tmpBuf[1000] = "";
	char pBuf[1000] = "";
	bool reEmitP = 0;
	CExpression* testExpression = m_operant1;
	bool increment_stack = false;
	CStr copyString;

	while (testExpression != 0)
	{

		// insert pointer load

		if (testExpression->m_regAssign1 != NULL &&  testExpression->m_rettype->isArray2())	// if array -> insert load pointer
		{
			//sprintf(nBuf,";\n\t\t(%s*)p[%d] = &%s",testExpression->GetReturnTypeName(),testExpression->m_regAssign1->pointerNr,testExpression->m_rettype->m_cvdt->name.buf);

			//sprintf(nBuf,";\n\t\tp[%d] = &%s",testExpression->m_regAssign1->pointerNr,testExpression->m_rettype->m_cvdt->name.buf);
			sprintf(nBuf, ";\n\t\tp[%d] = &((char*)p[P_STACK])[%i]", testExpression->m_regAssign1->pointerNr, testExpression->m_rettype->m_memAddr/*+testExpression->m_rettype->m_array_dim->dim*testExpression->GetReturnTypeSize()*/);

			pointerAssignString = pointerAssignString + nBuf;

		}
		if (testExpression->m_regAssign1 != NULL &&  testExpression->m_rettype->isPointer())	// if pointer -> insert load pointer
		{
			//sprintf(nBuf,";\n\t\tp[%d] = &%s",testExpression->m_regAssign1->pointerNr,m_operant2->m_rettype->m_cvdt->name.buf);
			//pointerAssignString = pointerAssignString+nBuf;
		}
		CExpression* tmpExpression = NULL;
		if (testExpression->m_operator == OP_DOT)	// struct access
		{
			tmpExpression = testExpression;
		}
		else if ((m_operant2 != NULL) && m_operant2->m_operator == OP_DOT)	// struct access
		{
			tmpExpression = m_operant2;
		}
		if (tmpExpression != NULL)
		{
			if (tmpExpression->m_operant2->m_operator == OP_IDENTIFIER)
			{
				// build assignment pointer
				CExpression* tExp = tmpExpression;
				CStr pString = "";
				CStr tmpString = "";
				CStr dotString = ".";
				while (tExp->m_operant1->m_operator != OP_IDENTIFIER)
				{
					tmpString = pString;
					if (tExp->m_operant1->m_operant2 != NULL)pString = dotString + tExp->m_operant1->m_operant2->m_rettype->m_cvdt->name;
					if (tmpString.len != 0) pString = pString + tmpString;
					tExp = tExp->m_operant1;
					if (tExp->m_operant1 != NULL)tExp->m_operant1->m_rettype->m_hierarchy++;
				}
				if (tExp->m_operant1->m_regAssign1 != NULL)
				{
					//sprintf(nBuf,";\n\t\t(%s*)p[%d] = &%s",testExpression->m_operant2->GetReturnTypeName(),tExp->m_operant2->m_regAssign1->pointerNr,(tExp->m_operant1->m_rettype->m_cvdt->name+pString).buf);
					if (tmpExpression->m_operator == OP_DOT && tmpExpression->m_operant1->m_rettype->m_hierarchy == 0)
					{
						//sprintf(nBuf,";\n\t\tp[%d] = &%s",tExp->m_operant2->m_regAssign1->pointerNr,(tExp->m_operant1->m_rettype->m_cvdt->name+pString).buf);

						sprintf(nBuf, ";\n\t\tp[%d] = &((char*)p[P_STACK])[%i]", tExp->m_operant2->m_regAssign1->pointerNr, tExp->m_operant1->m_rettype->m_memAddr/*+tExp->m_operant1->GetReturnTypeSize()*/);

						//else sprintf(nBuf,";\n\t\tp[%d] = %s",tExp->m_operant2->m_regAssign1->pointerNr,(tExp->m_operant1->m_rettype->m_cvdt->name+pString).buf);
						pointerAssignString = pointerAssignString + nBuf;
					}
					tmpExpression = tmpExpression->m_operant2;
				}
			}
		}
		tmpExpression = NULL;
		/*if(testExpression->m_operator==OP_ARROW)	// pointer access
		{
		tmpExpression=testExpression;
		}
		else if(m_operant2->m_operator==OP_ARROW)	// pointer access
		{
		tmpExpression=m_operant2;
		}
		if(tmpExpression!=NULL)
		{
		if(tmpExpression->m_operant2->m_operator == OP_IDENTIFIER)
		{
		// build assignment pointer
		CExpression* tExp=tmpExpression;
		CStr pString="";
		CStr tmpString="";
		CStr dotString=".";
		while(tExp->m_operant1->m_operator != OP_IDENTIFIER)
		{
		tmpString=pString;
		if(tExp->m_operant1->m_operant2!=NULL)pString=dotString+tExp->m_operant1->m_operant2->m_rettype->m_cvdt->name;
		if(tmpString.len!=0) pString=pString+tmpString;
		tExp=tExp->m_operant1;
		}
		if(tExp->m_operant1->m_regAssign1!=NULL)
		{
		//sprintf(nBuf,";\n\t\t(%s*)p[%d] = &%s",testExpression->m_operant2->GetReturnTypeName(),tExp->m_operant2->m_regAssign1->pointerNr,(tExp->m_operant1->m_rettype->m_cvdt->name+pString).buf);
		if(tmpExpression->m_operator==OP_DOT )sprintf(nBuf,";\n\t\tp[%d] = &%s",tExp->m_operant2->m_regAssign1->pointerNr,(tExp->m_operant1->m_rettype->m_cvdt->name+pString).buf);
		//else sprintf(nBuf,";\n\t\tp[%d] = %s",tExp->m_operant2->m_regAssign1->pointerNr,(tExp->m_operant1->m_rettype->m_cvdt->name+pString).buf);
		pointerAssignString = pointerAssignString+nBuf;
		tmpExpression=tmpExpression->m_operant2;
		}
		}
		}*/
		/*		if(testExpression->m_operator==OP_UNARY_STAR)	// pointer access
		{
		if(m_operant2->m_operator == OP_IDENTIFIER || m_operant2->m_operator == OP_CONST_INT || m_operant2->m_operator == OP_CONST_FLOAT || ( (m_operant2->m_operator >  OP_LOGIC_OR) && (m_operant2->m_operator <= OP_MODULO) ))
		{
		// build assignment pointer
		CExpression* tExp=m_operant1->m_operant2;
		CStr type="";
		CStr tmpString="";
		long type1=m_operant2->m_operator;
		long type2=tExp->GetReturnTypeId();
		if(type1 == TYPE_CONST_INT || type2==TYPE_INT || type2==TYPE_CHAR || type2==TYPE_SHORT)
		{
		type="i";
		}
		else if (type1 == TYPE_CONST_FLOAT || type2==TYPE_FLOAT)
		{
		type="f";
		}
		else
		{
		type="undef";
		}

		if(m_operant2->m_regAssign1!=NULL)		// if identifier
		{
		if(m_operant2->m_regAssign1->regNr==-1)	// if float
		{

		sprintf(nBuf2,"ACCU");
		}
		else if(m_operant2->m_rettype->isPointer())	// if pointer
		{
		sprintf(nBuf2,"REG[%d]",testExpression->m_regAssign1->regNr);
		}
		else
		{
		sprintf(nBuf2,"REG[%d]",m_operant2->m_regAssign1->regNr);//testExpression->m_regAssign1->regNr);
		}
		}
		if(m_rettype->m_scope=SCOPE_LOCAL)
		{
		sprintf(tmpBuf,";\n\t\tp[%d]= reinterpret_cast<int*>(((int*)p[P_STACK])[%d])",tExp->m_regAssign1->pointerNr,tExp->m_regAssign1->offset,tExp->m_regAssign1->offset);
		}
		else
		{
		sprintf(tmpBuf,";\n\t\tp[%d]= reinterpret_cast<int*>(((int*)p[%i])[%d])",tExp->m_regAssign1->pointerNr,GLOBAL_POINTER,tExp->m_regAssign1->offset,tExp->m_regAssign1->offset);
		}
		if(m_operant2->m_operator == OP_CONST_INT || m_operant2->m_operator == OP_CONST_FLOAT)
		{
		sprintf(nBuf,"%s;\n\t\t((%s*)p[%d])[0] = %f",tmpBuf,testExpression->GetReturnTypeName(),tExp->m_regAssign1->pointerNr,m_operant2->m_data);
		}
		else
		{
		sprintf(nBuf,"%s;\n\t\t((%s*)p[%d])[0] = %s.%s",tmpBuf,testExpression->GetReturnTypeName(),tExp->m_regAssign1->pointerNr,nBuf2,type.buf);
		}
		pointerAssignString = pointerAssignString+nBuf;
		}
		}*/
		if (testExpression->m_regAssign1 != NULL && (testExpression->m_operator == OP_IDENTIFIER || testExpression->m_operator == OP_UNARY_STAR))
		{

			//////////////////////
			if (testExpression->m_operator == OP_UNARY_STAR)	// pointer access
			{
				if (m_operant2->m_operator == OP_IDENTIFIER || m_operant2->m_operator == OP_CONST_INT || m_operant2->m_operator == OP_CONST_FLOAT || ((m_operant2->m_operator >  OP_LOGIC_OR) && (m_operant2->m_operator <= OP_MODULO)))
				{
					// build assignment pointer
					CExpression* tExp = m_operant1->m_operant2;
					CStr type = "";
					CStr tmpString = "";
					long type1 = m_operant2->m_operator;
					long type2 = tExp->GetReturnTypeId();
					if (type1 == TYPE_CONST_INT || type2 == TYPE_INT || type2 == TYPE_CHAR || type2 == TYPE_SHORT || type2 == TYPE_LONG)
					{
						type = "i";
					}
					else if (type1 == TYPE_CONST_FLOAT || type2 == TYPE_FLOAT)
					{
						type = "f";
					}
					else
					{
						type = "undef";
					}

					if (m_operant2->m_regAssign1 != NULL)		// if identifier
					{
						if (m_operant2->m_regAssign1->regNr == -1)	// if float
						{

							sprintf(nBuf2, "ACCU");
						}
						else if (m_operant2->m_rettype->isPointer())	// if pointer
						{
							sprintf(nBuf2, "REG[%d]", testExpression->m_regAssign1->regNr);
						}
						else
						{
							sprintf(nBuf2, "REG[%d]", m_operant2->m_regAssign1->regNr);//testExpression->m_regAssign1->regNr);
						}
					}
					if (m_rettype->isLocalScope())
					{
						sprintf(tmpBuf, ";\n\t\tp[%d]= reinterpret_cast<%s*>(((int*)p[P_STACK])[%d])", tExp->m_regAssign1->pointerNr, tExp->GetReturnTypeName(), tExp->m_regAssign1->offset);
					}
					else
					{
						sprintf(tmpBuf, ";\n\t\tp[%d]= reinterpret_cast<%s*>(((int*)p[%i])[%d])", tExp->m_regAssign1->pointerNr, tExp->GetReturnTypeName(), GLOBAL_POINTER, tExp->m_regAssign1->offset);
					}
					if (m_operant2->m_operator == OP_CONST_INT || m_operant2->m_operator == OP_CONST_FLOAT)
					{
						sprintf(nBuf, "%s;\n\t\t((%s*)p[%d])[0] = %f", tmpBuf, testExpression->GetReturnTypeName(), tExp->m_regAssign1->pointerNr, m_operant2->m_data);
					}
					else
					{
						//sprintf(nBuf,"%s;\n\t\t((%s*)p[%d])[0] = %s.%s",tmpBuf,testExpression->GetReturnTypeName(),tExp->m_regAssign1->pointerNr,nBuf2,type.buf);
						sprintf(nBuf, "%s;\n\t\t((%s*)p[%d])[0] ", tmpBuf, testExpression->GetReturnTypeName(), tExp->m_regAssign1->pointerNr);
					}
					//pointerAssignString = pointerAssignString+nBuf;
				}
			}

			//////////////////
			if (testExpression->m_rettype->m_isRegister) //testExpression->m_regAssign1->pointerNr==-1)							// if Register
			{
				if (testExpression->m_regAssign1->regNr == -1)	// if float
				{

					sprintf(nBuf, ";\n\t\tACCU.f ");
				}
				else if (testExpression->m_rettype->isPointer())	// if pointer
				{
					sprintf(nBuf, ";\n\t\tREG[%d].p", testExpression->m_regAssign1->regNr);
				}
				else
				{
					sprintf(nBuf, ";\n\t\tREG[%d].i ", testExpression->m_regAssign1->regNr);
				}
				// check type
				/*CStr type="";
				if(m_operant2!=NULL)
				{
				long type1=m_operant2->m_operator;
				long type2=m_operant2->GetReturnTypeId();
				if((m_operant2->m_rettype->isPointer() && !m_operant2->m_rettype->isArray2()) || m_operant2->m_operator == OP_UNARY_AND)
				{
				type="p";
				}
				else if(type1 == TYPE_CONST_INT || type2==TYPE_INT ||type1 == TYPE_CHAR || type2==TYPE_CHAR || type1 == TYPE_SHORT || type2==TYPE_SHORT)
				{
				type="i";
				}
				else if (type1 == TYPE_CONST_FLOAT ||type2==TYPE_FLOAT)
				{
				type="f";
				}
				else
				{
				type="undef";
				}
				}
				sprintf(nBuf,";\n\t\t%s.%s ",testExpression->m_rettype->m_cvdt->name.buf,type.buf);	*/
				//if(testExpression->m_rettype->isPointer() && !testExpression->m_rettype->isArray2())sprintf(nBuf,";\n\t\tp[%d] ",testExpression->m_regAssign1->pointerNr);
			}
			else if (testExpression->m_regAssign1->offset == -1)sprintf(nBuf, ";\n\t\t((%s*)p[%d])[REG[%i].i] ", testExpression->GetReturnTypeName(), testExpression->m_regAssign1->pointerNr, OFFSETREG);
			else if (testExpression->m_regAssign1->pointerNr == STACK_POINTER && testExpression->m_operator != OP_UNARY_STAR)
			{
				char *pdest;
				pdest = strstr(testExpression->m_rettype->m_cvdt->name.buf, "return");

				if (saveStack || pdest != NULL /*|| testExpression->m_rettype->m_functionParameter*/ || testExpression->saveToStack || testExpression->m_rettype->forceToStack)
				{
					sprintf(nBuf, ";\n\t\t((%s*)p[P_STACK])[%d] ", testExpression->GetReturnTypeName(), testExpression->m_regAssign1->offset);
				}
				else
				{
					CStr t = "i";
					if (testExpression->GetReturnTypeId() == TYPE_FLOAT)t = "f";
					sprintf(nBuf, ";\n\t\tREG[%d].%s ", testExpression->m_regAssign1->regNr, t.buf);
				}

			}
			else if (testExpression->m_rettype->isPointer() && !testExpression->m_rettype->isArray2())
			{

				if (testExpression->m_rettype->isGlobalScope())	// SCOPE_GLOBAL
				{
					sprintf(nBuf, ";\n\t\t((%s*)p[P_GLOBAL])[%d] ", testExpression->GetReturnTypeName(), testExpression->m_regAssign1->offset);
				}
				else		//global
				{
					sprintf(nBuf, ";\n\t\t((int*)p[P_STACK])[%d] "/*,testExpression->GetReturnTypeName()*/, testExpression->m_regAssign1->offset);
				}
				sprintf(pBuf, ";\n\t\tp[%d] = ", testExpression->m_regAssign1->pointerNr);
				reEmitP = true;
			}
			else if (testExpression->m_operator != OP_UNARY_STAR)
			{
				if (testExpression->GetReturnTypeId() == TYPE_STRUCT)
				{
					if (m_operant1->m_operant2 != NULL)
					{
						if ((m_operant1->m_operant2->m_rettype->isPointer()))
						{
							sprintf(nBuf, ";\n\t\tp[%d] ", m_operant1->m_operant2->m_regAssign1->pointerNr);
						}
						else
						{
							sprintf(nBuf, ";\n\t\t((%s*)p[%d])[%d] ", m_operant1->m_operant2->GetReturnTypeName(), testExpression->m_regAssign1->pointerNr, m_operant1->m_operant2->m_regAssign1->offset);
						}
					}
					else
					{
						sprintf(nBuf, ";\n\t\tp[%d] ", m_operant1->m_rettype->m_assignedPointer);
					}
					//sprintf(nBuf,";\n\t\t((%s*)p[%d])[%d] ",testExpression->m_regAssign1->identifier->getDataType()->GetTypeName(),testExpression->m_regAssign1->pointerNr,testExpression->m_regAssign1->offset);

				}
				else
				{
					sprintf(nBuf, ";\n\t\t((%s*)p[%d])[%d] ", testExpression->GetReturnTypeName(), testExpression->m_regAssign1->pointerNr, testExpression->m_regAssign1->offset);
				}
			}
			pointerAssignString = pointerAssignString + nBuf;
			if (m_operator == OP_POST_PLUS_PLUS || m_operator == OP_POST_MINUS_MINUS)
			{
				if (testExpression->m_rettype->isPointer())
				{
					if (m_operator == OP_POST_PLUS_PLUS)sprintf(nBuf, " += sizeof(%s) ", testExpression->GetReturnTypeName());
					else if (m_operator == OP_POST_MINUS_MINUS)sprintf(nBuf, " -= sizeof(%s) ", testExpression->GetReturnTypeName());
					pointerAssignString = pointerAssignString + nBuf/*OperatorString[m_operator]*/;
				}
				else
				{
					pointerAssignString = pointerAssignString + OperatorString[m_operator];
				}
			}
			copyString = pointerAssignString;

			// second part
			int decr = 0;

			if (m_operant2 != NULL)
			{
				if (m_operant2->m_operator == OP_FUNCTION)
				{
					CStr tmp, param = "\n\t\t// Insert Paramter list\n";
					// Get stack decrement
					mem_usage_struct *tmpMem = m_operant2->m_operant1->m_rettype->GetFuncMemUsage();
					decr = tmpMem->kn_local_array_mem + tmpMem->kn_local_mem + tmpMem->kn_local_pointer_mem + tmpMem->kn_local_struct_mem;

					// copy parameter to stack

					CIdListIterator ident = m_operant2->m_operant1->m_rettype->GetIdList_Next();
					CExpression *tmpExpr = m_operant2->m_operant2;
					while (tmpExpr)
					{
						if (tmpExpr->m_operator == OP_CONST_INT)sprintf(nBuf, "\n\t\t((int*)p[P_STACK])[%ld] = %i;", (ident->m_memAddr - decr) / ident->GetTypeSize(), (int)tmpExpr->m_data);
						else if (tmpExpr->m_operator == OP_CONST_FLOAT)sprintf(nBuf, "\n\t\t((float*)p[P_STACK])[%ld] = %f;", (ident->m_memAddr - decr) / ident->GetTypeSize(), tmpExpr->m_data);
						else
						{
							sprintf(nBuf, "\n\t\t((int*)p[P_STACK])[%ld] = ((int*)p[P_STACK])[%ld];", (ident->m_memAddr - decr) / ident->GetTypeSize(), tmpExpr->m_rettype->m_memAddr / ident->GetTypeSize());
						}
						tmpExpr = tmpExpr->next;
						ident++;
						//param=nBuf;
						param = param + nBuf;
					}

					param = param + "\n";

					// insert stack decrement;

					CStr param2 = "\n\t\t// Insert Stack Decrement\n";

					sprintf(nBuf, "\n\t\tp[P_STACK]=&((char*)p[P_STACK])[-%i];", decr);
					tmp = nBuf;
					pointerAssignString = param + param2 + tmp + pointerAssignString;
					increment_stack = true;


				}
				if (m_operant2->m_regAssign1 != NULL)
				{
					pointerAssignString = pointerAssignString + OperatorString[m_operator];
					if (m_operant2->m_regAssign1->pointerNr == -1)		// if Register
					{
						if (m_operant2->m_regAssign1->regNr == -1)	// if float
						{

							sprintf(nBuf, "ACCU.f ");
						}
						else if (m_operant2->m_rettype->isPointer())	// if pointer
						{
							sprintf(nBuf, "REG[%d].p", m_operant2->m_regAssign1->regNr);
							sprintf(nBuf2, "(int)REG[%d].p", m_operant2->m_regAssign1->regNr);
						}
						else
						{
							sprintf(nBuf, "REG[%d].i ", m_operant2->m_regAssign1->regNr);
						}
						/*	long type1=testExpression->GetReturnTypeId();
						bool isPointer=0;

						if(type1==TYPE_STRUCT)	// get the type inside the struct
						{
						if(m_operant1->m_rettype->isPointer())
						{
						type1=	testExpression->m_regAssign1->identifier->GetTypeId();
						isPointer = true;
						}
						else
						{
						type1=m_operant1->m_operant2->GetReturnTypeId();//testExpression->m_regAssign1->identifier->GetTypeId();
						}
						}

						// check type
						CStr type="";
						if((testExpression->m_rettype->isPointer() && !testExpression->m_rettype->isArray2() )|| isPointer )//|| type1 == TYPE_STRUCT)
						{
						type="p";
						}
						else if(type1==TYPE_INT || type1 == TYPE_CHAR || type1 == TYPE_SHORT )
						{
						type="i";
						}
						else if (type1==TYPE_FLOAT)
						{
						type="f";
						}
						else
						{
						type="undef";
						}
						sprintf(nBuf," %s.%s ",m_operant2->m_rettype->m_cvdt->name.buf,type.buf);	*/
					}
					else if (m_operant2->m_regAssign1->pointerNr == STACK_POINTER)
					{
						sprintf(nBuf, " ((%s*)p[P_STACK])[%d] ", m_operant2->GetReturnTypeName(), m_operant2->m_regAssign1->offset);

						// if register is assigned to this stack variable
						if (m_operant2->m_regAssign1->regNr != -1)
						{
							CStr t = "i";
							if (m_operant2->GetReturnTypeToken() == TYPE_FLOAT)t = "f";
							sprintf(nBuf, "REG[%d].%s ", m_operant2->m_regAssign1->regNr, t.buf);
						}

					}
					else sprintf(nBuf, " ((%s*)p[%d])[%d] ", m_operant2->GetReturnTypeName(), m_operant2->m_regAssign1->pointerNr, m_operant2->m_regAssign1->offset);
					if (reEmitP)
					{
						pointerAssignString = pointerAssignString + nBuf2 + pBuf + nBuf;
					}
					else
					{
						pointerAssignString = pointerAssignString + nBuf;
					}
				}
				else if (m_operant2->m_operator == OP_DOT || m_operant2->m_operator == OP_ARRAY)	// struct/array access
				{
					if (m_operant2->m_operant1->m_regAssign1 != NULL)
					{
						// insert load pointer
						/*sprintf(nBuf,";\n\t\tp[%d] = &%s",m_operant2->m_operant1->m_regAssign1->pointerNr,m_operant2->m_operant1->m_rettype->m_cvdt->name.buf);
						CStr t=nBuf;

						pointerAssignString = t + pointerAssignString + OperatorString[m_operator];*/
						pointerAssignString = pointerAssignString + OperatorString[m_operator];
						if (m_operant2->m_operant1->m_regAssign1->offset == -1)sprintf(nBuf, " ((%s*)p[%d])[REG[%i].i] ", m_operant2->GetReturnTypeName(), m_operant2->m_operant1->m_regAssign1->pointerNr, OFFSETREG);
						else sprintf(nBuf, " ((%s*)p[%d])[%d] ", m_operant2->GetReturnTypeName(), m_operant2->m_operant1->m_regAssign1->pointerNr, m_operant2->m_operant1->m_regAssign1->offset);
						pointerAssignString = pointerAssignString + nBuf;
					}

				}
				else if ((m_operant2->m_operator >  OP_ASSIGN) && (m_operant2->m_operator <= OP_MODULO))
				{
					//pointerAssignString = pointerAssignString + OperatorString[m_operator];
					// op1
					if (m_operant2->m_operant1->m_regAssign1 != 0)
					{
						if (m_operant2->m_operant1->m_regAssign1->pointerNr == STACK_POINTER)
						{

							sprintf(nBuf, "( ((%s*)p[P_STACK])[%d] ", m_operant2->m_operant1->GetReturnTypeName(), m_operant2->m_operant1->m_regAssign1->offset);

							// if register is assigned to this stack variable
							if (m_operant2->m_operant1->m_regAssign1->regNr != -1)
							{
								CStr t = "i";
								if (m_operant2->m_operant1->GetReturnTypeToken() == TYPE_FLOAT)t = "f";
								sprintf(nBuf, "(REG[%d].%s ", m_operant2->m_operant1->m_regAssign1->regNr, t.buf);
							}

						}
						else if (m_operant2->m_operant1->m_rettype->m_isRegister == true)
						{
							if (m_operant2->m_operant1->m_regAssign1->regNr == -1)	// if float
							{

								sprintf(nBuf, "( ACCU.f ");
							}
							else if (m_operant2->m_operant1->m_rettype->isPointer())	// if pointer
							{
								sprintf(nBuf, "( REG[%d].p", m_operant2->m_operant1->m_regAssign1->regNr);
							}
							else
							{
								sprintf(nBuf, "( REG[%d].i ", m_operant2->m_operant1->m_regAssign1->regNr);
							}
						}
						else
						{
							sprintf(nBuf, "( ((%s*)p[%d])[%d] ", m_operant2->m_operant1->GetReturnTypeName(), m_operant2->m_operant1->m_regAssign1->pointerNr, m_operant2->m_operant1->m_regAssign1->offset);
						}
					}
					else
					{
						sprintf(nBuf, "(%f", (float)m_operant2->m_operant1->m_data);
					}

					//op2
					if (m_operant2->m_operant2->m_regAssign1 != 0)
					{
						if (m_operant2->m_operant2->m_regAssign1->pointerNr == STACK_POINTER)
						{
							sprintf(nBuf2, " ((%s*)p[P_STACK])[%d] )", m_operant2->m_operant2->GetReturnTypeName(), m_operant2->m_operant2->m_regAssign1->offset);

							// if register is assigned Fo this stack variable
							if (m_operant2->m_operant2->m_regAssign1->regNr != -1)
							{
								CStr t = "i";
								if (m_operant2->m_operant2->GetReturnTypeToken() == TYPE_FLOAT)t = "f";
								sprintf(nBuf2, "REG[%d].%s )", m_operant2->m_operant2->m_regAssign1->regNr, t.buf);
							}
						}
						else if (m_operant2->m_operant2->m_rettype->m_isRegister == true)
						{
							if (m_operant2->m_operant2->m_regAssign1->regNr == -1)	// if float
							{

								sprintf(nBuf2, " ACCU.f )");
							}
							else if (m_operant2->m_operant2->m_rettype->isPointer())	// if pointer
							{
								sprintf(nBuf2, " REG[%d].p)", m_operant2->m_operant2->m_regAssign1->regNr);
							}
							else
							{
								sprintf(nBuf2, " REG[%d].i )", m_operant2->m_operant2->m_regAssign1->regNr);
							}
						}
						else
						{
							sprintf(nBuf2, " ((%s*)p[%d])[%d] )", m_operant2->m_operant2->GetReturnTypeName(), m_operant2->m_operant2->m_regAssign1->pointerNr, m_operant2->m_operant2->m_regAssign1->offset);
						}
					}
					else
					{
						sprintf(nBuf2, "%f)", (float)m_operant2->m_operant2->m_data);
					}
					//sprintf(nBuf," ((%s*)p[%d])[%d] ",m_operant2->GetReturnTypeName(),m_operant2->m_operant1->m_regAssign1->pointerNr,m_operant2->m_operant1->m_regAssign1->offset);
					pointerAssignString = pointerAssignString + OperatorString[m_operator] + nBuf + OperatorString[m_operant2->m_operator] + nBuf2;
				}
				else
				{
					pointerAssignString = pointerAssignString + OperatorString[m_operator];
					if (m_operant2->m_operator == OP_CONST_FLOAT)sprintf(nBuf, " %f ", m_operant2->m_data);
					else if (m_operant2->m_operator == OP_UNARY_AND)
					{
						//sprintf(nBuf," &%s ",m_operant2->m_operant2->m_rettype->m_cvdt->name);
						if (m_operant2->m_operant2->m_rettype->isArray2())	// if array address
						{
							//if(
						}
						else
						{
							if (m_operant2->m_operant2->m_regAssign1 != NULL && m_operant2->m_operant2->m_regAssign1->pointerNr != -1)
							{
								sprintf(nBuf, " &((char*)p[%i])[%i] ", m_operant2->m_operant2->m_regAssign1->pointerNr, m_operant2->m_operant2->m_regAssign1->offsetBytes);
							}
							else
							{
								int offsetSum = 0;
								if (m_operant2->m_operant2->m_operator == OP_DOT)	// if struct
								{
									// get sum of offset
									offsetSum = m_operant2->m_operant2->m_operant1->m_rettype->m_memAddr + m_operant2->m_operant2->m_operant2->m_rettype->m_memAddr;
								}
								else
								{
									offsetSum = m_operant2->m_operant2->m_regAssign1->offsetBytes;
								}
								if (testExpression->m_rettype->isGlobalScope())	// SCOPE_GLOBAL
								{
									sprintf(nBuf, " &((char*)p[P_GLOBAL])[%i] ", offsetSum);//m_operant2->m_operant2->m_regAssign1->offsetBytes);
								}
								else
								{
									sprintf(nBuf, " &((char*)p[P_STACK])[%i] ", offsetSum);
								}
							}
						}
					}
					else if (m_operant2->m_operator == OP_FUNCTION)
					{
						// get parameter
						CStr param = "";
						CExpression *tmpExpr = m_operant2->m_operant2;
						while (tmpExpr)
						{
							if (tmpExpr->m_operator == OP_CONST_INT)sprintf(nBuf, "%i", (int)tmpExpr->m_data);
							else if (tmpExpr->m_operator == OP_CONST_FLOAT)sprintf(nBuf, "%f", tmpExpr->m_data);
							else
							{
								sprintf(nBuf, "%s", tmpExpr->m_rettype->m_cvdt->name.buf);
							}

							//param = param + ",";
							param = param + nBuf;
							tmpExpr = tmpExpr->next;
							if (tmpExpr)param = param + ",";
						}
						sprintf(nBuf, " %s(%s) ", m_operant2->m_operant1->m_rettype->m_cvdt->name.buf, param.buf);
					}
					else sprintf(nBuf, " %d ", (int)m_operant2->m_data);
					pointerAssignString = pointerAssignString + nBuf;
				}
				/*if(nBuf2[0]!=0)
				{
				pointerAssignString = pointerAssignString+";\n\t\t"+nBuf2+nBuf;
				}*/
				if (increment_stack)
				{
					// insert stack increment;

					CStr param2 = "\n\n\t\t// Insert Stack Increment\n";
					mem_usage_struct *tmpMem = m_operant2->m_operant1->m_rettype->GetFuncMemUsage();
					sprintf(nBuf, ";\n\t\tp[P_STACK]=&((char*)p[P_STACK])[%ld];", (tmpMem->kn_local_array_mem + tmpMem->kn_local_mem + tmpMem->kn_local_pointer_mem + tmpMem->kn_local_struct_mem));
					CStr tmp = nBuf;
					pointerAssignString = pointerAssignString + param2 + tmp;
					increment_stack = false;

					// find return value

					CStatement *state = m_operant2->m_rettype->GetFunctionCompoundStatements()->list;
					CIdentifier *ident = NULL;
					char *pdest = NULL;
					while (state)
					{
						if (state->m_identifier)
						{
							ident = state->m_identifier;
							pdest = strstr(ident->m_cvdt->name.buf, "return");
							if (pdest != NULL)break;

						}
						state = state->next;
					}
					if (ident != NULL)
					{
						param2 = "\n\n\t\t// Get the return value\n";
						copyString = copyString + OperatorString[m_operator];
						sprintf(nBuf, " ((%s*)p[P_STACK])[%ld]", ident->GetTypeName(), (ident->m_memAddr - decr) / ident->GetTypeSize());
						copyString = copyString + nBuf;
						pointerAssignString = pointerAssignString + param2 + copyString;
					}

				}
			}
		}
		if (testExpression->m_operator == OP_ARROW)	// pointer access
		{
			testExpression = testExpression->m_operant2;
		}
		else testExpression = testExpression->m_operant1;
	}
#ifdef emitPointerOnly
	pointerAssignString.Replace(";", "");
#endif
	return copyString;
}
char* CExpression::emitAll(char* buf, int mode, int saveStack)
{
	emitAll(buf, mode, saveStack, 0, 0);
	return buf;
}

char* CExpression::emitAll(char* buf, int mode, int saveStack, bool getIdent, bool dontAssign)
{

	char buf2[10000] = "";
	CStr assignString = ";\n\t\tassign(";
	char nBuf[1000] = "";
	CStr pointerAssignString = "";
	CStr structAssignString = "";
	CStr retAssignString = "";
	CStr tmpString = "";
	CStr derefString = "";
	CExpression* testExpression = m_operant1;

	switch (m_operator) {
	case OP_NOT_DEFINED:
		break;
	case OP_CONST_INT:
		_itoa((int)m_data, buf + strlen(buf), 10);
		break;
	case OP_CONST_FLOAT:
		sprintf(buf2, "%f ", m_data);
		strcat(buf, buf2);
		break;
	case OP_CONST_STRING:
		sprintf(buf2, "%s ", m_string.buf);
		strcat(buf, buf2);
		break;
	case OP_IDENTIFIER:

		if (emit_pointer && getIdent)
		{
			CStr t = "i";
			if (GetReturnTypeId() == TYPE_FLOAT)t = "f";
			sprintf(nBuf, "(REG[%i].%s)", m_rettype->m_currentAssignedRegister, t.buf);
			pointerAssignString = nBuf;
			strcat(buf, pointerAssignString.buf);
		}
		else
		{
			strcat(buf, m_rettype->m_cvdt->name.buf);
		}
		break;
	case OP_FUNCTION:
		emit(buf, m_operant1, m_operant2, "(", m_operator, mode, saveStack);
		strcat(buf, ")");
		break;
	case SIZEOF:
		strcat(buf, "sizeof( ");
		strcat(buf, m_rettype->m_cvdt->name.buf);
		strcat(buf, " )");
		break;
	case OP_ASSIGN:
	case OP_MUL_ASSIGN:
	case OP_DIV_ASSIGN:
	case OP_MOD_ASSIGN:
	case OP_ADD_ASSIGN:
	case OP_SUB_ASSIGN:
	case OP_SHIFT_LEFT_ASSIGN:
	case OP_SHIFT_RIGHT_ASSIGN:
	case OP_XOR_ASSIGN:
	case OP_OR_ASSIGN:
	case OP_AND_ASSIGN:
		emit(buf, m_operant1, m_operant2, OperatorString[m_operator], m_operator, mode, saveStack);

		if (generateAssign && emit_pointer == false && !dontAssign)
		{
			if (testExpression->m_rettype->isPointer())
			{
				break;
			}
			if (testExpression->m_operator == OP_ARRAY)	// array access
			{
				if (testExpression->m_operant2->m_operator == OP_CONST_INT)
				{
					sprintf(nBuf, "[%d]", (int)testExpression->m_operant2->m_data);
					tmpString = nBuf;
				}
				else
				{
					sprintf(nBuf, "[%s]", testExpression->m_operant2->m_rettype->m_cvdt->name.buf);
					tmpString = nBuf;
				}
			}
			if (testExpression->m_operator == OP_DOT || testExpression->m_operator == OP_ARROW)	// struct access
			{
				if (testExpression->m_operant2->m_operator == OP_IDENTIFIER)
				{
					// build assignment pointer
					CExpression* tExp = testExpression;
					CStr dotString = OperatorString[testExpression->m_operator];
					CStr pString = "";
					while (tExp->m_operant1->m_operator != OP_IDENTIFIER)
					{
						tmpString = pString;
						if (tExp->m_operant1->m_operant2 != NULL)pString = dotString + tExp->m_operant1->m_operant2->m_rettype->m_cvdt->name;
						if (tmpString.len != 0) pString = pString + tmpString;
						tExp = tExp->m_operant1;
					}
					/*if(testExpression->m_operator==OP_DOT )*/sprintf(nBuf, "%s", (tExp->m_operant1->m_rettype->m_cvdt->name + pString).buf);
					structAssignString = structAssignString + nBuf + dotString;
				}
			}
			if (isDeref)derefString = "*";
			assignString = assignString + structAssignString + derefString + m_operant1->m_rettype->m_cvdt->name + tmpString + " , ";

			if (m_operant1->m_rettype->m_cvdt->token == 0)	// if Identifier is not known, try to  get second part
			{
				if (m_operant1->m_operant2 != NULL && (m_operant1->m_operant2->m_operator == OP_POST_PLUS_PLUS || m_operant1->m_operant2->m_operator == OP_POST_MINUS_MINUS))
				{
					// assign new pointer and decremnt it
					CStr test = ";\n\t(";
					test = test + m_operant1->m_operant2->m_operant1->m_rettype->GetTypeName();
					test = test + "*&) tmp = ";
					test = test + m_operant1->m_operant2->m_operant1->m_rettype->m_cvdt->name;

					strcat(buf, test.buf);
					assignString = ";\n\t\tassign(";
					if (m_operant1->m_operant2->m_operator == OP_POST_PLUS_PLUS)assignString = assignString + "*(--(";
					else assignString = assignString + "*(++(";
					assignString = assignString + m_operant1->m_operant2->m_operant1->m_rettype->GetTypeName();
					assignString = assignString + "*&)tmp)" + tmpString + " , ";

				}
			}
			if (firstrun)
			{
				sprintf(nBuf, "\"s%d\" )", exprCount);
				assignString = assignString + nBuf;
				assigned_number = exprCount;
				exprCount++;
				strcat(buf, assignString.buf);
			}
			else if (assigned_number != -1)
			{
				sprintf(nBuf, "\"s%d\" )", assigned_number);
				assignString = assignString + nBuf;
				strcat(buf, assignString.buf);
			}

		}
		if (emit_pointer)
		{
			retAssignString = emitPointer(pointerAssignString, m_operant1, m_operant2, OperatorString[m_operator], saveStack);
		}
#ifdef emitPointerOnly
		strcpy(buf, pointerAssignString.buf);
#else
		strcat(buf, pointerAssignString.buf);
#endif

		if (generateAssign && emit_pointer == true && !dontAssign)
		{
			/*	if(testExpression->m_operator==OP_ARRAY )	// array access
			{
			if(testExpression->m_operant2->m_operator == OP_CONST_INT)
			{
			//sprintf(nBuf,"[%d]",(int)testExpression->m_operant2->m_data);
			//tmpString = nBuf;
			}
			}
			if(testExpression->m_operator==OP_DOT || testExpression->m_operator==OP_ARROW)	// struct access
			{
			if(testExpression->m_operant2->m_operator == OP_IDENTIFIER)
			{
			// build assignment pointer
			CExpression* tExp=testExpression;
			CStr dotString=".";
			CStr pString="";
			while(tExp->m_operant1->m_operator != OP_IDENTIFIER)
			{
			tmpString=pString;
			if(tExp->m_operant1->m_operant2!=NULL)pString=dotString+tExp->m_operant1->m_operant2->m_rettype->m_cvdt->name;
			if(tmpString.len!=0) pString=pString+tmpString;
			tExp=tExp->m_operant1;
			}
			if(testExpression->m_operator==OP_DOT )sprintf(nBuf,"%s",(tExp->m_operant1->m_rettype->m_cvdt->name+pString).buf);
			structAssignString = structAssignString+nBuf+dotString;
			}
			}

			//assignString =  assignString + structAssignString + m_operant1->m_rettype->m_cvdt->name  + tmpString + " , ";
			CStr name;
			if(m_operant1->m_rettype->m_assignedRegister==-1)
			{
			if(m_operant1->m_rettype->m_scope!=SCOPE_GLOBAL)	// SCOPE_GLOBAL
			{
			sprintf(nBuf,"((%s*)p[P_STACK])[%d] ",m_operant1->GetReturnTypeName(),m_operant1->m_rettype->m_memAddr/m_operant1->GetReturnTypeSize());
			}
			else		//global
			{
			sprintf(nBuf,"((%s*)p[P_GLOBAL])[%d] ",m_operant1->GetReturnTypeName(),m_operant1->m_rettype->m_memAddr/m_operant1->GetReturnTypeSize());
			}

			}
			else
			{
			if(m_operant1->m_regAssign1->regNr==-1)	// if float
			{

			sprintf(nBuf," ACCU.f ");
			}
			else if(m_operant1->m_rettype->isPointer())	// if pointer
			{
			sprintf(nBuf," REG[%d].p",m_operant1->m_regAssign1->regNr);
			}
			else
			{
			sprintf(nBuf," REG[%d].i ",m_operant1->m_regAssign1->regNr);
			}

			}
			name=nBuf;
			assignString =  assignString + structAssignString + name  + tmpString + " , ";
			*/
			if (firstrun)
			{
				sprintf(nBuf, "\"s%d\" )", exprCount);
				assignString = assignString + retAssignString + nBuf;
				assigned_number = exprCount;
				exprCount++;
				strcat(buf, assignString.buf);
			}
			else if (assigned_number != -1)
			{
				sprintf(nBuf, "\"s%d\" )", assigned_number);
				CStr ext = retAssignString;
				bool e = ext.Extract(";", ";", 1);
				if (e)retAssignString.Replace(ext.buf, "");
				retAssignString.Replace("\n", "");
				retAssignString.Replace("\n", "");
				retAssignString.Replace("\t", "");
				retAssignString.Replace("\t", "");
				retAssignString.Replace(";", "");
				retAssignString = retAssignString + " , ";
				assignString = assignString + retAssignString + nBuf;
				strcat(buf, assignString.buf);
			}

		}

		break;

	case OP_TYPECAST:
		strcat(buf, "( ");
		strcat(buf, m_rettype->m_cvdt->name.buf);
		strcat(buf, " )");
		emit(buf, m_operant1, m_operant2, "", 0, 1, saveStack);
		break;
	case OP_TYPECASTP:
		strcat(buf, "( ");
		strcat(buf, m_rettype->m_cvdt->name.buf);
		strcat(buf, " * )");
		emit(buf, m_operant1, m_operant2, "", 0, 1, saveStack);
		break;
	case OP_CONDITIONAL:
	case OP_COMMATA:
	case OP_LOGIC_OR:
	case OP_LOGIC_AND:
	case OP_INCLUSIVE_OR:
	case OP_EXCLUSIVE_OR:
	case OP_ANDX:
	case OP_SHIFT_LEFT:
	case OP_SHIFT_RIGHT:
	case OP_PLUS:
	case OP_MINUS:
	case OP_MULT:
	case OP_DIV:
	case OP_MODULO:
	case OP_POST_PLUS_PLUS:
	case OP_POST_MINUS_MINUS:
	case OP_UNARY_AND:
	case OP_UNARY_STAR:
	case OP_UNARY_PLUS:
	case OP_UNARY_MINUS:
	case OP_UNARY_NEGATE:
	case OP_UNARY_INVERT:
	case OP_ARRAY:
	case OP_DOT:
	case OP_ARROW:
		emit(buf, m_operant1, m_operant2, OperatorString[m_operator], m_operator, mode, saveStack);
		if (m_operator == OP_UNARY_STAR && generateAssign)isDereference = true;
		break;
	case OP_UNARY_PRE_PLUS_PLUS:
	case OP_UNARY_PRE_MINUS_MINUS:
		emit(buf, m_operant2, m_operant1, OperatorString[m_operator], m_operator, mode, saveStack);
		if (m_operator == OP_UNARY_STAR && generateAssign)isDereference = true;
		break;
	case OP_EQUAL:
	case OP_NOT_EQUAL:
	case OP_LESS:
	case OP_GREATER:
	case OP_LESS_EQ:
	case OP_GREATER_EQ:

		emit(buf, m_operant1, m_operant2, OperatorString[m_operator], m_operator, mode, saveStack);
		if (emit_pointer)
		{
			emitPointer(pointerAssignString, m_operant1, m_operant2, OperatorString[m_operator], saveStack);
		}
#ifdef emitPointerOnly
		strcpy(buf, pointerAssignString.buf);
#else
		strcat(buf, pointerAssignString.buf);
#endif
		break;
	default:
		break;
	}

	if (next != NULL) {
		strcat(buf, ",");
		next->emitAll(buf, 0, saveStack);
	}
	return buf;
}

int CExpression::emit_reg_include(char* type, char* name, bool is_preg){

	if (CStatement::m_filept2 != NULL) {
		if (is_preg)
			fprintf(CStatement::m_filept2, "\t\t\t\t%s* \t%s;\n", type, name);
		else
			fprintf(CStatement::m_filept2, "%s \t%s;\n", type, name);

	}

	return 0;
}


// return constant expression, error if not constant
double CExpression::getConstant()
{
	if (m_operator == OP_CONST_INT || m_operator == OP_CONST_FLOAT) {
		return m_data;
	}
	else {
		char buf[10000] = "";
		emitAll(buf, 0, 0);
		ycc_erpnt(1, buf);			//Expression must be constant
		return 0;
	}
}


void initTypeConvTable() {
	for (int i = 0; i<kn_EXP_OPERATORS; i++){
		for (int k = 0; k<kn_TYPES; k++) {
			for (int j = 0; j<kn_TYPES; j++) {
				if (k != 0 && j != 0) TypeConvTable[i][j][k].error = 1000;		// no error for undefined values: error already issued
				TypeConvTable[i][j][k].rettype = as.ident_undef;
			}
		}
	}

	insertTypeTable(OP_CONST_INT, 0, 0, as.ident_const_int);
	//insertTypeTable(OP_CONST_LONG,0,0,as.ident_const_long);
	insertTypeTable(OP_CONST_FLOAT, 0, 0, as.ident_const_float);
	insertTypeTable(OP_CONST_STRING, 0, 0, as.ident_const_string);
	insertTypeTable(OP_IDENTIFIER, 0, 0, 0);			            		// insert basic type


	//TypeConvTable[OP_ASSIGN][EXPTYPE_FIXP24][EXPTYPE_CONST_INT]
	/*
	insertTypeTable(OP_PLUS,TYPE_FIXP24,TYPE_FIXP24, as.ident_fixp24);
	insertTypeTable(OP_PLUS,TYPE_FIXP24,TYPE_CONST_INT, as.ident_fixp24);
	insertTypeTable(OP_PLUS,TYPE_FIXP24,TYPE_CONST_FLOAT, as.ident_fixp24);
	insertTypeTable(OP_MULT,TYPE_FIXP24,TYPE_FIXP24, as.ident_fixp24);
	insertTypeTable(OP_MULT,TYPE_FIXP24,TYPE_CONST_INT, as.ident_fixp24);
	insertTypeTable(OP_MULT,TYPE_FIXP24,TYPE_CONST_FLOAT, as.ident_fixp24);
	insertTypeTable(OP_MULT,TYPE_CONST_INT,TYPE_FIXP24, as.ident_fixp24);
	insertTypeTable(OP_MULT,TYPE_CONST_FLOAT,TYPE_FIXP24, as.ident_fixp24);
	insertTypeTable(OP_ASSIGN,TYPE_FIXP24,TYPE_FIXP24, as.ident_fixp24);
	insertTypeTable(OP_ASSIGN,TYPE_FIXP24,TYPE_CONST_INT, as.ident_fixp24);
	insertTypeTable(OP_ASSIGN,TYPE_FIXP24,TYPE_CONST_FLOAT, as.ident_fixp24);
	insertTypeTable(OP_ASSIGN,TYPE_FIXP24,TYPE_REG_ACCU, as.ident_fixp24);
	insertTypeTable(OP_ASSIGN,TYPE_REG_ACCU,TYPE_CONST_FLOAT, as.ident_accu);
	insertTypeTable(OP_ASSIGN,TYPE_REG_ACCU,TYPE_CONST_INT, as.ident_accu);
	insertTypeTable(OP_ASSIGN,TYPE_REG_ACCU,TYPE_FIXP24, as.ident_accu);
	insertTypeTable(OP_ANDX,TYPE_FIXP24,TYPE_FIXP24, as.ident_fixp24);
	insertTypeTable(OP_ADD_ASSIGN,TYPE_REG_ACCU,TYPE_CONST_FLOAT, as.ident_accu);
	insertTypeTable(OP_ADD_ASSIGN,TYPE_REG_ACCU,TYPE_CONST_INT, as.ident_accu);
	insertTypeTable(OP_ADD_ASSIGN,TYPE_REG_ACCU,TYPE_FIXP24, as.ident_accu);
	*/

	//
	// Type Conversion Table for type: INT
	// assignment (native)
	insertTypeTable(OP_ASSIGN, TYPE_INT, TYPE_INT, as.ident_int);   // =
	insertTypeTable(OP_MUL_ASSIGN, TYPE_INT, TYPE_INT, as.ident_int);   // *=
	insertTypeTable(OP_DIV_ASSIGN, TYPE_INT, TYPE_INT, as.ident_int);   // /=
	insertTypeTable(OP_MOD_ASSIGN, TYPE_INT, TYPE_INT, as.ident_int);   // %=
	insertTypeTable(OP_ADD_ASSIGN, TYPE_INT, TYPE_INT, as.ident_int);   // +=
	insertTypeTable(OP_SUB_ASSIGN, TYPE_INT, TYPE_INT, as.ident_int);   // -=
	insertTypeTable(OP_SHIFT_LEFT_ASSIGN, TYPE_INT, TYPE_INT, as.ident_int);   // <<=
	insertTypeTable(OP_SHIFT_RIGHT_ASSIGN, TYPE_INT, TYPE_INT, as.ident_int);   // >>=
	insertTypeTable(OP_AND_ASSIGN, TYPE_INT, TYPE_INT, as.ident_int);   // &=
	insertTypeTable(OP_XOR_ASSIGN, TYPE_INT, TYPE_INT, as.ident_int);   // ^=
	insertTypeTable(OP_OR_ASSIGN, TYPE_INT, TYPE_INT, as.ident_int);   // |=
	insertTypeTable(OP_ASSIGN, TYPE_INT, TYPE_CONST_INT, as.ident_int);   //  =
	insertTypeTable(OP_MUL_ASSIGN, TYPE_INT, TYPE_CONST_INT, as.ident_int);   // *=
	insertTypeTable(OP_DIV_ASSIGN, TYPE_INT, TYPE_CONST_INT, as.ident_int);   // /=
	insertTypeTable(OP_MOD_ASSIGN, TYPE_INT, TYPE_CONST_INT, as.ident_int);   // %=
	insertTypeTable(OP_ADD_ASSIGN, TYPE_INT, TYPE_CONST_INT, as.ident_int);   // +=
	insertTypeTable(OP_SUB_ASSIGN, TYPE_INT, TYPE_CONST_INT, as.ident_int);   // -=
	insertTypeTable(OP_SHIFT_LEFT_ASSIGN, TYPE_INT, TYPE_CONST_INT, as.ident_int);   // <<=
	insertTypeTable(OP_SHIFT_RIGHT_ASSIGN, TYPE_INT, TYPE_CONST_INT, as.ident_int);   // >>=
	insertTypeTable(OP_AND_ASSIGN, TYPE_INT, TYPE_CONST_INT, as.ident_int);   // &=
	insertTypeTable(OP_XOR_ASSIGN, TYPE_INT, TYPE_CONST_INT, as.ident_int);   // ^=
	insertTypeTable(OP_OR_ASSIGN, TYPE_INT, TYPE_CONST_INT, as.ident_int);   // |=
	// for assembler-representaion
	insertTypeTable(OP_ASSIGN, TYPE_INT, TYPE_REG_ACCU, as.ident_int);       // =
	insertTypeTable(OP_ASSIGN, TYPE_REG_ACCU, TYPE_CONST_INT, as.ident_int); // =
	insertTypeTable(OP_ASSIGN, TYPE_REG_ACCU, TYPE_INT, as.ident_int);       // =
	insertTypeTable(OP_ADD_ASSIGN, TYPE_REG_ACCU, TYPE_CONST_INT, as.ident_int); // +=


	// arithmetic :
	insertTypeTable(OP_PLUS, TYPE_INT, TYPE_INT, as.ident_int);            // +
	insertTypeTable(OP_PLUS, TYPE_INT, TYPE_CONST_INT, as.ident_int);
	insertTypeTable(OP_PLUS, TYPE_CONST_INT, TYPE_INT, as.ident_int);
	insertTypeTable(OP_PLUS, TYPE_CONST_INT, TYPE_CONST_INT, as.ident_int);
	insertTypeTable(OP_MINUS, TYPE_INT, TYPE_INT, as.ident_int);            // -
	insertTypeTable(OP_MINUS, TYPE_INT, TYPE_CONST_INT, as.ident_int);
	insertTypeTable(OP_MINUS, TYPE_CONST_INT, TYPE_INT, as.ident_int);
	insertTypeTable(OP_MINUS, TYPE_CONST_INT, TYPE_CONST_INT, as.ident_int);
	insertTypeTable(OP_MULT, TYPE_INT, TYPE_INT, as.ident_int);            // *
	insertTypeTable(OP_MULT, TYPE_INT, TYPE_CONST_INT, as.ident_int);
	insertTypeTable(OP_MULT, TYPE_CONST_INT, TYPE_INT, as.ident_int);
	insertTypeTable(OP_MULT, TYPE_CONST_INT, TYPE_CONST_INT, as.ident_int);
	insertTypeTable(OP_DIV, TYPE_INT, TYPE_INT, as.ident_int);            // div
	insertTypeTable(OP_DIV, TYPE_INT, TYPE_CONST_INT, as.ident_int);
	insertTypeTable(OP_DIV, TYPE_CONST_INT, TYPE_INT, as.ident_int);
	insertTypeTable(OP_DIV, TYPE_CONST_INT, TYPE_CONST_INT, as.ident_int);

	insertTypeTable(OP_MODULO, TYPE_INT, TYPE_CONST_INT, as.ident_int);   // %
	insertTypeTable(OP_POST_MINUS_MINUS, TYPE_INT, TYPE_INT, as.ident_int);         // x--
	insertTypeTable(OP_UNARY_PRE_MINUS_MINUS, TYPE_INT, TYPE_INT, as.ident_int);         // --x
	insertTypeTable(OP_POST_PLUS_PLUS, TYPE_INT, TYPE_INT, as.ident_int);         // x++
	insertTypeTable(OP_UNARY_PRE_PLUS_PLUS, TYPE_INT, TYPE_INT, as.ident_int);         // ++x

	// logic :
	insertTypeTable(OP_GREATER, TYPE_INT, TYPE_INT, as.ident_int);           // >
	insertTypeTable(OP_GREATER, TYPE_INT, TYPE_CONST_INT, as.ident_int);
	insertTypeTable(OP_GREATER_EQ, TYPE_INT, TYPE_INT, as.ident_int);           // >=
	insertTypeTable(OP_GREATER_EQ, TYPE_INT, TYPE_CONST_INT, as.ident_int);
	insertTypeTable(OP_LESS, TYPE_INT, TYPE_INT, as.ident_int);           // <
	insertTypeTable(OP_LESS, TYPE_INT, TYPE_CONST_INT, as.ident_int);
	insertTypeTable(OP_LESS_EQ, TYPE_INT, TYPE_INT, as.ident_int);           // <=
	insertTypeTable(OP_LESS_EQ, TYPE_INT, TYPE_CONST_INT, as.ident_int);
	insertTypeTable(OP_EQUAL, TYPE_INT, TYPE_INT, as.ident_int);           // ==
	insertTypeTable(OP_EQUAL, TYPE_INT, TYPE_CONST_INT, as.ident_int);
	insertTypeTable(OP_NOT_EQUAL, TYPE_INT, TYPE_INT, as.ident_int);           // !=
	insertTypeTable(OP_NOT_EQUAL, TYPE_INT, TYPE_CONST_INT, as.ident_int);
	insertTypeTable(OP_LOGIC_AND, TYPE_INT, TYPE_INT, as.ident_int);           // &&
	insertTypeTable(OP_LOGIC_AND, TYPE_INT, TYPE_CONST_INT, as.ident_int);
	insertTypeTable(OP_LOGIC_OR, TYPE_INT, TYPE_INT, as.ident_int);           // ||
	insertTypeTable(OP_LOGIC_OR, TYPE_INT, TYPE_CONST_INT, as.ident_int);
	insertTypeTable(OP_UNARY_INVERT, TYPE_INT, TYPE_INT, as.ident_int);           // !

	// bitwise :
	insertTypeTable(OP_ANDX, TYPE_INT, TYPE_INT, as.ident_int);           // &
	insertTypeTable(OP_ANDX, TYPE_INT, TYPE_CONST_INT, as.ident_int);
	insertTypeTable(OP_INCLUSIVE_OR, TYPE_INT, TYPE_INT, as.ident_int);           // |
	insertTypeTable(OP_INCLUSIVE_OR, TYPE_INT, TYPE_CONST_INT, as.ident_int);
	insertTypeTable(OP_INCLUSIVE_OR, TYPE_LONG, TYPE_CONST_INT, as.ident_int);

	insertTypeTable(OP_EXCLUSIVE_OR, TYPE_INT, TYPE_INT, as.ident_int);           // ^
	insertTypeTable(OP_EXCLUSIVE_OR, TYPE_INT, TYPE_CONST_INT, as.ident_int);
	insertTypeTable(OP_SHIFT_LEFT, TYPE_INT, TYPE_INT, as.ident_int);           // <<
	insertTypeTable(OP_SHIFT_LEFT, TYPE_INT, TYPE_CONST_INT, as.ident_int);
	insertTypeTable(OP_SHIFT_RIGHT, TYPE_INT, TYPE_INT, as.ident_int);           // >>
	insertTypeTable(OP_SHIFT_RIGHT, TYPE_INT, TYPE_CONST_INT, as.ident_int);
	insertTypeTable(OP_UNARY_NEGATE, TYPE_INT, TYPE_INT, as.ident_int);           // ~


	//
	// Type Conversion Table for type: FLOAT
	// assignment (native)
	insertTypeTable(OP_ASSIGN, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);   // =
	insertTypeTable(OP_MUL_ASSIGN, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);   // *=
	insertTypeTable(OP_DIV_ASSIGN, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);   // /=
	insertTypeTable(OP_MOD_ASSIGN, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);   // %=
	insertTypeTable(OP_ADD_ASSIGN, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);   // +=
	insertTypeTable(OP_SUB_ASSIGN, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);   // -=
	insertTypeTable(OP_SHIFT_LEFT_ASSIGN, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);   // <<=
	insertTypeTable(OP_SHIFT_RIGHT_ASSIGN, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);   // >>=
	insertTypeTable(OP_AND_ASSIGN, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);   // &=
	insertTypeTable(OP_XOR_ASSIGN, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);   // ^=
	insertTypeTable(OP_OR_ASSIGN, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);   // |=
	insertTypeTable(OP_ASSIGN, TYPE_FLOAT, TYPE_CONST_FLOAT, as.ident_float);   //  =
	insertTypeTable(OP_MUL_ASSIGN, TYPE_FLOAT, TYPE_CONST_FLOAT, as.ident_float);   // *=
	insertTypeTable(OP_DIV_ASSIGN, TYPE_FLOAT, TYPE_CONST_FLOAT, as.ident_float);   // /=
	insertTypeTable(OP_MOD_ASSIGN, TYPE_FLOAT, TYPE_CONST_FLOAT, as.ident_float);   // %=
	insertTypeTable(OP_ADD_ASSIGN, TYPE_FLOAT, TYPE_CONST_FLOAT, as.ident_float);   // +=
	insertTypeTable(OP_SUB_ASSIGN, TYPE_FLOAT, TYPE_CONST_FLOAT, as.ident_float);   // -=
	insertTypeTable(OP_SHIFT_LEFT_ASSIGN, TYPE_FLOAT, TYPE_CONST_FLOAT, as.ident_float);   // <<=
	insertTypeTable(OP_SHIFT_RIGHT_ASSIGN, TYPE_FLOAT, TYPE_CONST_FLOAT, as.ident_float);   // >>=
	insertTypeTable(OP_AND_ASSIGN, TYPE_FLOAT, TYPE_CONST_FLOAT, as.ident_float);   // &=
	insertTypeTable(OP_XOR_ASSIGN, TYPE_FLOAT, TYPE_CONST_FLOAT, as.ident_float);   // ^=
	insertTypeTable(OP_OR_ASSIGN, TYPE_FLOAT, TYPE_CONST_FLOAT, as.ident_float);   // |=


	// for assembler-representaion
	insertTypeTable(OP_ASSIGN, TYPE_FLOAT, TYPE_REG_ACCU, as.ident_float);         // =
	insertTypeTable(OP_ASSIGN, TYPE_REG_ACCU, TYPE_CONST_FLOAT, as.ident_float);   // =
	insertTypeTable(OP_ASSIGN, TYPE_REG_ACCU, TYPE_FLOAT, as.ident_float);         // =

	// arithmetic :
	insertTypeTable(OP_PLUS, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);          // +
	insertTypeTable(OP_PLUS, TYPE_FLOAT, TYPE_CONST_FLOAT, as.ident_float);
	insertTypeTable(OP_MINUS, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);          // -
	insertTypeTable(OP_MINUS, TYPE_FLOAT, TYPE_CONST_FLOAT, as.ident_float);
	insertTypeTable(OP_MULT, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);          // *
	insertTypeTable(OP_MULT, TYPE_FLOAT, TYPE_CONST_FLOAT, as.ident_float);
	insertTypeTable(OP_DIV, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);          // div
	insertTypeTable(OP_DIV, TYPE_FLOAT, TYPE_CONST_FLOAT, as.ident_float);

	insertTypeTable(OP_POST_MINUS_MINUS, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);          // x--
	insertTypeTable(OP_UNARY_PRE_MINUS_MINUS, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);          // --x
	insertTypeTable(OP_POST_PLUS_PLUS, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);          // x++
	insertTypeTable(OP_UNARY_PRE_PLUS_PLUS, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);          // ++x

	// logic :
	insertTypeTable(OP_GREATER, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);           // >
	insertTypeTable(OP_GREATER, TYPE_FLOAT, TYPE_CONST_FLOAT, as.ident_float);
	insertTypeTable(OP_GREATER_EQ, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);           // >=
	insertTypeTable(OP_GREATER_EQ, TYPE_FLOAT, TYPE_CONST_FLOAT, as.ident_float);
	insertTypeTable(OP_LESS, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);           // <
	insertTypeTable(OP_LESS, TYPE_FLOAT, TYPE_CONST_FLOAT, as.ident_float);
	insertTypeTable(OP_LESS_EQ, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);           // <=
	insertTypeTable(OP_LESS_EQ, TYPE_FLOAT, TYPE_CONST_FLOAT, as.ident_float);
	insertTypeTable(OP_EQUAL, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);           // ==
	insertTypeTable(OP_EQUAL, TYPE_FLOAT, TYPE_CONST_FLOAT, as.ident_float);
	insertTypeTable(OP_NOT_EQUAL, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);           // !=
	insertTypeTable(OP_NOT_EQUAL, TYPE_FLOAT, TYPE_CONST_FLOAT, as.ident_float);
	insertTypeTable(OP_LOGIC_AND, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);           // &&
	insertTypeTable(OP_LOGIC_AND, TYPE_FLOAT, TYPE_CONST_FLOAT, as.ident_float);
	insertTypeTable(OP_LOGIC_OR, TYPE_FLOAT, TYPE_FLOAT, as.ident_float);           // ||
	insertTypeTable(OP_LOGIC_OR, TYPE_FLOAT, TYPE_CONST_FLOAT, as.ident_float);

}


void insertTypeTable(int i, int j, int k, CIdentifier* rettype, int error, int p_level1, int p_level2)
{
	TypeConvTable[i][j][k].rettype = rettype;
	TypeConvTable[i][j][k].error = error;
	TypeConvTable[i][j][k].p_level1 = p_level1;
	TypeConvTable[i][j][k].p_level2 = p_level2;
}



CDataType* CExpression::getReturnDataType() const {
	return m_rettype->m_declspec->getDataType();
};

long CExpression::GetReturnTypeId() const {
	return m_rettype->GetTypeId();
};

long CExpression::GetReturnTypeSize() const {
	return m_rettype->GetTypeSize();
};

char* CExpression::GetReturnTypeName() const {
	return m_rettype->GetTypeName();
};

CStr& CExpression::GetReturnTypeNameStr() const {
	return m_rettype->GetTypeNameStr();
};

CStr CExpression::GetReturnTypeNameStrAsm() const {
	CStr retStr = "";
	int tmpInt = m_rettype->GetTypeSize();
	if (tmpInt == 4)
	{
		retStr = "w";
	}
	else if (tmpInt == 2)
	{
		retStr = "s";
	}
	else
	{
		retStr = "b";
	}
	return retStr;
};

int CExpression::GetReturnTypeToken() const {
	return m_rettype->GetTypeToken();
};


