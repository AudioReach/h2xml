/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////////////
//! \brief					 CStatement class implementation									\file
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CStatement.h"
#include <algorithm>


int tabs = 0;
int label_cnt = 0;
FILE*  CStatement::m_filept = NULL;
FILE* CStatement::m_filept2 = NULL;
int branch = 0;
int CStatement::m_functionRamAddress;
int CStatement::m_lastPointer;
int CStatement::m_lastReg;
int CStatement::m_ifCount;

int kn_global_mem = 0, kn_global_array_mem = 0, kn_global_pointer_mem = 0, kn_global_struct_mem = 0, decr = 0;
bool emit_pointer = 0, functionStart = false, insertRetVar = false, use_conditional_reg = false;
extern bool isDereference;
char *oldCodeLine = "test";


CStatement::CStatement()
{
	memset(this, 0, sizeof(CStatement));
}


// add  next after this statement
CStatement* CStatement::set_next(CStatement* snext)
{
	next = snext;
	if (snext != NULL) {
		snext->previous = this;
		return snext;
	}
	else {
		return this;
	}
}

// add  next after this statement
CStatement* CStatement::set_next(CStatementBlock* snext)
{
	next = snext->first;
	if (snext != NULL && snext->first != NULL) {
		snext->first->previous = this;
		return snext->last;
	}
	else {
		return this;
	}
}

// insert new CStatement before this statement
CStatement* CStatement::insertBefore(int token)
{
	CStatement* tmp = new CStatement();
	tmp->m_token = token;
	tmp->previous = previous;
	tmp->m_codeline = m_codeline;
	tmp->m_codeline_cnt = m_codeline_cnt;
	if (previous == NULL) {
		as.ycc_start = tmp;			//if very first Statement
	}
	else {
		if (previous->list == this) {
			previous->list = tmp;		//compound statement
		}
		else {
			previous->next = tmp;
		}
	}
	previous = tmp;
	tmp->next = this;
	return tmp;
}

void CStatement::remove()	//remove statement from list 
{
	if (previous != NULL) previous->next = next;
	if (next != NULL) next->previous = previous;
}

// insert function compound statement
// insert pointer to function definiton to all declared variables
// sort declared variables as followes: 
//			pointer (local + global)
//			variablen
//			structs
//			arrays
void CStatement::insertCompound(CIdentifier* fname, CStatement* compound)
{
	list = compound;
	m_identifier = fname;
	fname->SetFunctionStatement(this);
	m_mem_usage = new(mem_usage_struct);
	memset(m_mem_usage, 0, sizeof(mem_usage_struct));
	char name[1000] = "pti_";
	strcat(name, m_identifier->m_cvdt->name.buf);
	m_mem_usage->m_base_pt = newCIdentifier(as.type_fixp24, name);
	m_mem_usage->m_base_pt->m_pointer_level = 1;

	/*CStatement* actual_statement=compound->list;
	while (actual_statement!=NULL) {
	if (actual_statement->m_token==STATEMENT_DEKLARATION) {
	actual_statement->m_identifier->m_mem_usage=m_mem_usage;
	int element_size=actual_statement->m_identifier->GetTypeSize();
	int datatype=actual_statement->m_identifier->GetTypeId();
	// Arrays
	if (actual_statement->m_identifier->isArray()) {
	actual_statement->m_identifier->m_memAddr = align(m_mem_usage->kn_local_array_mem, 8) + m_functionRamAddress;
	m_mem_usage->kn_local_array_mem += align(actual_statement->m_identifier->m_elements*element_size, 8);
	}
	// Pointer
	else if (actual_statement->m_identifier->isPointer()) {
	actual_statement->m_identifier->m_memAddr = align(m_mem_usage->kn_local_pointer_mem, element_size) + m_functionRamAddress;
	m_mem_usage->kn_local_pointer_mem+=POINTER_SIZE;
	}
	// structs
	else if (datatype==TYPE_STRUCT || datatype==TYPE_UNION) {
	actual_statement->m_identifier->m_memAddr = align(m_mem_usage->kn_local_struct_mem, 8) + m_functionRamAddress;
	m_mem_usage->kn_local_struct_mem += align(element_size, 8);
	}
	// other variables
	else {
	actual_statement->m_identifier->m_memAddr=align(m_mem_usage->kn_local_mem, element_size)+m_functionRamAddress;
	m_mem_usage->kn_local_mem+=element_size;

	}
	}
	actual_statement=actual_statement->next;
	}

	// resort the elements
	int newMem=m_mem_usage->kn_local_pointer_mem;
	m_mem_usage->kn_local_mem=0;
	for(int r=0;r<5;r++)
	{
	int actual_size;
	if(r==0)actual_size=1;
	else actual_size=r*2;
	newMem = align(newMem, actual_size);
	actual_statement=compound->list;
	while (actual_statement!=NULL && r!=3) {
	if (actual_statement->m_token==STATEMENT_DEKLARATION) {
	int element_size=actual_statement->m_identifier->GetTypeSize();
	int datatype=actual_statement->m_identifier->GetTypeId();
	// Arrays
	if (actual_statement->m_identifier->m_elements>1) {
	}
	// Pointer
	else if (actual_statement->m_identifier->isPointer()) {

	}
	// structs
	else if (datatype==TYPE_STRUCT || datatype==TYPE_UNION) {
	}
	// other variables
	else {
	if(element_size==actual_size)
	{
	actual_statement->m_identifier->m_memAddr=newMem;
	newMem+=element_size;

	}
	}
	}
	actual_statement=actual_statement->next;
	}
	}
	m_mem_usage->kn_local_mem=newMem;
	// update element addresses
	actual_statement=compound->list;
	while (actual_statement!=NULL) {
	if (actual_statement->m_token==STATEMENT_DEKLARATION) {
	int datatype=actual_statement->m_identifier->GetTypeId();
	// Arrays
	if (actual_statement->m_identifier->m_elements>1) {
	actual_statement->m_identifier->m_memAddr+=m_mem_usage->kn_local_mem+m_mem_usage->kn_local_struct_mem+m_mem_usage->kn_local_pointer_mem;
	actual_statement->m_identifier->m_memAddr = align(actual_statement->m_identifier->m_memAddr, 8);
	}
	// Pointer
	else if (actual_statement->m_identifier->isPointer()) {

	}
	// structs
	else if (datatype==TYPE_STRUCT || datatype==TYPE_UNION) {
	actual_statement->m_identifier->m_memAddr+=m_mem_usage->kn_local_mem+m_mem_usage->kn_local_pointer_mem;
	actual_statement->m_identifier->m_memAddr = align(actual_statement->m_identifier->m_memAddr, 8);
	}
	// other variables
	else {
	//actual_statement->m_identifier->m_memAddr+=m_mem_usage->kn_local_pointer_mem;

	}
	}
	actual_statement=actual_statement->next;
	}*/
}


void CStatement::emit()
{
    char *buf = tmpBuf;
	char *buf2= tmpBuf2;
    buf[0] = '\0';
    buf2[0] = '\0';

	bool extra_line = false;
	bool semicolon = true;
	isDereference = false;
	CStatement* compound = NULL;

	switch (m_token) {
	case STATEMENT_UNDEFINED:
		break;
	case STATEMENT_DEKLARATION:
		m_identifier->EmitDeclaration(buf);
		break;
	case STATEMENT_DEKLARATION_TYPE:
	case STATEMENT_FUNCTION_DEKLARATION:
		m_identifier->EmitDeclaration(buf);
		break;
	case STATEMENT_FUNCTION_DEFINITION:
		sprintf(buf2, "\n\n// local variables memory: %ld Bytes\n", m_mem_usage->kn_local_mem);
		strcat(buf, buf2);
		sprintf(buf2, "// local array memory: %ld Bytes\n", m_mem_usage->kn_local_array_mem);
		strcat(buf, buf2);
		sprintf(buf2, "// local struct memory: %ld Bytes\n", m_mem_usage->kn_local_struct_mem);
		strcat(buf, buf2);
		sprintf(buf2, "// local pointer memory: %ld Bytes\n", m_mem_usage->kn_local_pointer_mem);
		strcat(buf, buf2);

		if (emit_pointer)
		{
			for (int i = 0; i<NR_POINTERS; i++)
			{
				if (m_mem_usage->ydfpPointers[i].type != 0)
				{
					sprintf(buf2, "// Pointer %d: Type '%s' ; isArray '%d' ; assignOffset %d Bytes\n", i, m_mem_usage->ydfpPointers[i].typeName.buf, m_mem_usage->ydfpPointers[i].isArray, m_mem_usage->ydfpPointers[i].assignOffset);
					strcat(buf, buf2);
				}
			}
		}
		m_identifier->EmitDeclaration(buf);
		compound = list;
		break;
	case STATEMENT_LABEL:
		if (m_identifier->m_cvdt != NULL)
			sprintf(buf2, "%s:", m_identifier->m_cvdt->name.buf);
		doEmit(buf2);
		//emitCompound(list); //print statements list linkes to
		semicolon = false;
		if (list != NULL) list->emitAll();
		break;
	case STATEMENT_CASE:
		sprintf(buf2, "case ");
		m_exp1->emitAll(buf2, 0, 0);
		sprintf(buf2, "%s :", buf2);
		doEmit(buf2);
		emitCompound(list); //print statements list linkes to
		break;
	case STATEMENT_DEFAULT:
		sprintf(buf2, "default : ");
		doEmit(buf2);
		emitCompound(list); //print statements list linkes to
		break;
	case STATEMENT_EXPRESSION:
		if (next){
			m_exp1->emitAll(buf, 0, next->m_save_stack);
		}
		else{
			m_exp1->emitAll(buf, 0, 0);
		}
		break;
	case STATEMENT_COMPOUND_STATEMENT:
		emitCompound(list);
		break;
	case STATEMENT_IF:
		if (m_exp1->isIdentifier()) sprintf(buf2, "if (");
		else sprintf(buf2, "if ");
		m_exp1->emitAll(buf2, 0, 0);
		if (m_exp1->isIdentifier()) strcat(buf2, ")");
		doEmit(buf2);
		emitCompound(list);
		extra_line = 1;
		break;
	case STATEMENT_IFELSE:
		if (m_exp1->isIdentifier()) sprintf(buf2, "if (");
		else sprintf(buf2, "if ");
		m_exp1->emitAll(buf2, 0, 0, 1, 0);	// get identifier (reg, pointer,..)
		if (m_exp1->isIdentifier()) strcat(buf2, ")");
		doEmit(buf2);
		emitCompound(list); //print statements list linkes to
		sprintf(buf2, "else ");
		strcat(buf, buf2);
		doEmit(buf2);
		sprintf(buf, "%s", "");
		emitCompound(branch_to);
		extra_line = 1;
		break;
	case STATEMENT_SWITCH:
		sprintf(buf2, "switch ( ");
		m_exp1->emitAll(buf2, 0, 0);
		strcat(buf2, " )");
		doEmit(buf2);
		emitCompound(list);
		break;
	case STATEMENT_WHILE:
		if (m_exp1->isIdentifier()) sprintf(buf2, "while (");
		else sprintf(buf2, "while ");
		m_exp1->emitAll(buf2, 0, 0);
		if (m_exp1->isIdentifier()) strcat(buf2, ")");
		doEmit(buf2);
		emitCompound(list); //print statements list linkes to
		break;
	case STATEMENT_DO:
	case STATEMENT_DOWHILE:
		sprintf(buf2, "do ");
		doEmit(buf2);
		emitCompound(list); //print statements list linkes to
		if (m_exp1->isIdentifier()) sprintf(buf2, "while (");
		else sprintf(buf2, "while ");
		m_exp1->emitAll(buf2, 0, 0);
		if (m_exp1->isIdentifier()) strcat(buf2, ")");
		doEmit(buf2);
		break;
	case STATEMENT_FOR:
		//doEmit(""); //print emty line
		if (m_exp1 == NULL) sprintf(buf2, "for (;;)");
		else {
			sprintf(buf2, "for ( ");
			m_exp1->emitAll(buf2, 0, 0, 0, 1); strcat(buf2, "; ");
			m_exp2->emitAll(buf2, 0, 0, 0, 1); strcat(buf2, "; ");
			m_exp3->emitAll(buf2, 0, 0, 0, 1);
			strcat(buf2, " )");
		}
		doEmit(buf2);
		emitCompound(list);
		break;
	case STATEMENT_GOTO:
		sprintf(buf, "goto %s", this->m_identifier->m_cvdt->name.buf);
		break;
	case STATEMENT_CONTINUE:
		doEmit("continue");
		break;
	case STATEMENT_BREAK:
		doEmit("break");
		break;
	case STATEMENT_RETURN:
		strcat(buf, "return ");
		if (m_exp1 != NULL) m_exp1->emitAll(buf, 0, 0);
		break;
	case STATEMENT_PREPROC_IFDEF:
	case STATEMENT_PREPROC_DEFINE:
		semicolon = false;
	default:
		break;
	}

	if (compound == NULL && semicolon) strcat(buf, ";");
	if (extra_line)
		sprintf(buf2, "%-60s//%s,%s,LC: %d\n", buf, StatementString[m_token], m_codeline, m_codeline_cnt);
	else
		sprintf(buf2, "%-60s//%s,%s,LC: %d", buf, StatementString[m_token], m_codeline, m_codeline_cnt);

	if (m_token == STATEMENT_DEKLARATION)
	{
		sprintf(buf, "// Memory Address: %d", m_identifier->m_memAddr);
		strcat(buf2, buf);
	}
	doEmit(buf2);
	if (compound != NULL) {
		compound->emit();
	}

}

// output this statement and all following statements
void CStatement::emitAll()
{
	CStatement* actual_statement = this;
	while (actual_statement != NULL) {
		actual_statement->emit();
		actual_statement = actual_statement->next;
	}
}

void CStatement::emitCompound(CStatement *l)
{
	if (list != NULL) {
		doEmit("{");
		tabs++;
		l->emitAll();
		tabs--;
		doEmit("}");
	}
}

void CStatement::doEmit(const char* buf)
{
	int tabs_safe;

	if (tabs<0) tabs = 0;
	tabs_safe = tabs;
	if (m_token == STATEMENT_LABEL) tabs = 0;

	if (m_filept != NULL) {
		fprintf(m_filept, "%-*s%s\n", 4 * tabs, "", buf);
	}
	else {
		printf("%-*s%s\n", 4 * tabs, "", buf);
	}
	tabs = tabs_safe;
}

void CStatement::doLabelEmit(const char* buf)
{
	if (m_filept != NULL) {
		fprintf(m_filept, "%s", buf);
	}
	else {
		printf("%s", buf);
	}
}

