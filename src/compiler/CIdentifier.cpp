/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////////////
//! \brief					 CIdentifier class implementation									\file
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CIdentifier.h"

CIdentifier::CIdentifier()
: next_for_funct_in_struct(0)
, m_previous(0)
, m_next(0)
, m_function(0)
, m_calling_function(0)
, m_base_struct(0)
, m_current_struct(0)
, m_scope(0)
, m_init(0)
, m_array_dimension(0)
, m_elements(1)
, m_token(0)
, m_cvdt(0)
, m_declspec(0)
, m_pointer_level(0)
, m_pt_type_qual(0)
, m_array_dim(0)
, m_mem_usage(0)
, last_producer(0)
, last_consumer(0)
, last_producerList(0)
, functionList(0)
, m_hierarchy(0)
, m_memAddr(0)
, m_isRegister(0)
, isGlobal(0)
, m_assignedPointer(-1)
, m_bitslice(-1)
, m_bitoffset(0)
, m_assignedRegister(-1)
, m_currentAssignedRegister(-1)
, m_is_termination_function(0)
, m_pointer_start_nr(0)
, m_reg_start_nr(0)
, m_nr_total_use(0)
, m_updated(0)
, m_isOffset(0)
, m_functionParameter(0)
, forceToStack(0)
, m_attribute(0)
, m_bitSizeElement(0)
, m_arraySize(0)
, m_arrayElements(0)
, m_addr_increment(0)
, m_indirection(0)
, m_align(0)
, m_anExpandArray(0)
, m_anExpandArrayOffset(0)
, m_anExpandStructs(0)
, m_anExpandTypedefs(0)
, m_isUnsigned(0)
, m_isArray(0)
, m_isStruct(0)
, m_defaultList(NULL)
, m_hasConditionalDefaultFile(false)
, m_hasDefaultFile(false)
, m_identBitField(NULL)
{
	as.last_identifier=this;

	// associate all attributes collected so far with this identifier
    if (gd) {
        identifierCreate(this);
    }

}


// is called to initialize e.g. accu, dummy etc.
Cvdt* InsertCIdentifier(const char* name, const char* datatype, bool isRegister)
{
	Cvdt* cvdt=htab.hashinstall(name);
	as.global_scope=2;
	as.ycc_pointer_level=0;
	as.declspec=new decl_specifiers(htab.hashlookup(datatype,0)->datatype, 0, 0);
	newCIdentifier(cvdt,isRegister);
	return cvdt;
}

Cvdt* InsertCIdentifier(const char* name, const char* datatype)	// overloaded function
{
	Cvdt* cvdt=InsertCIdentifier(name, datatype, 0);
	return cvdt;
}

// is called for Identifier deklaration
CIdentifier* newCIdentifier(Cvdt* cvdt, bool isRegister){
	CIdentifier* tmp;
	if (cvdt==NULL || cvdt->identifier==NULL) {
		tmp= new CIdentifier();
		tmp->SetScope(as.global_scope);
		tmp->m_declspec=as.declspec;
		tmp->m_cvdt=cvdt;
		tmp->m_isRegister=isRegister;
		if (cvdt!=NULL) {
			cvdt->identifier=tmp;
			cvdt->token=TOKEN_IDENTIFIER;
    		if (tmp->m_declspec->isExtern())
                tmp->m_token = IDENTIFIER_EXTERN;
        }
	}
	else {
		tmp=cvdt->identifier;

        if ((tmp->m_token!=IDENTIFIER_FUNCTION) && (tmp->m_token!=IDENTIFIER_FUNCTION_NOARGS && tmp->m_token!=IDENTIFIER_EXTERN && tmp->m_token!=IDENTIFIER_LABEL)) {
			// check if complete name==name -> error, else initialize identifier
			char* complete_name=getname(&as.module_name,cvdt->name.buf);
			if (strcmp(complete_name,cvdt->name.buf) !=0) {
				Cvdt* current_vdt=htab.hashinstall(complete_name);
				if (current_vdt!=NULL) {
					tmp=newCIdentifier(current_vdt);
				}
			} else {
				ycc_erpnt(101,cvdt->name.buf);
			}
		}
	}
    return tmp;
}

CIdentifier* newCIdentifier(Cvdt* cvdt)		// overloaded function
{
	CIdentifier* tmp=NULL;
	if (cvdt!=NULL && cvdt->identifier!=NULL && cvdt->identifier->m_token<=IDENTIFIER_VARIABLE) {
		
		// Check if found identifier is in same scope (lex may have returned pointer to identifier with same name one hirachy level above)
		char* actual_name=getname(&as.module_name,cvdt->shortname.buf);
		if (strcmp(actual_name,cvdt->name.buf) ==0) {
			ycc_erpnt(115,cvdt->shortname.buf);
			return cvdt->identifier;
		} else {
			// create new identifier with full name
			Cvdt* current_vdt=htab.hashinstall(actual_name);
			current_vdt->shortname=cvdt->shortname.buf;
			current_vdt->token=TOKEN_UNDEFINED;
			cvdt=current_vdt;
		}
	} else {
		//tmp=newCIdentifier(cvdt,0);	
	}

	tmp=newCIdentifier(cvdt,0);	
	return tmp;
}

CIdentifier* newCIdentifier(CDataType* type, const char* name, bool isRegister)
{
	Cvdt* tmpvdt=htab.hashinstall(name);
	CIdentifier* tmp= new CIdentifier();
	tmp->m_declspec = new decl_specifiers(type, 0, 0);
	tmp->m_cvdt=tmpvdt;
	tmp->m_isRegister=isRegister;
	tmpvdt->identifier=tmp;
	return tmp;
}

CIdentifier* newCIdentifier(CDataType* type, const char* name)	// overloaded function provided fo compatibility
{
	CIdentifier* tmp= newCIdentifier(type, name, 0);
	return tmp;
}

CIdentifier* newCIdentifier(CIdentifier* ident)
{
	CIdentifier* tmp= new CIdentifier();
	memmove(tmp,ident,sizeof(CIdentifier));

	return tmp;
}

// memory initialization
void CIdentifier::insertIdentifierInit(double data)
{
	//if (as.initmem==NULL) as.initcnt=0;
	initializerlist* tmp=m_init;
	m_init=new initializerlist;
	m_init->data=data;
	m_init->next=tmp;
	m_init->exp=NULL;
}

// add to memory initialization list
void CIdentifier::insertArrayDimension()
{
    // allow tail-padded-structures, as defined in C99 Standard
	// e.g. x[]
	
	
	insertArrayDimension(0);
    //printf("%d", -1);
}

// add to memory initialization list
void CIdentifier::insertArrayDimension(int dimension)
{
	array_dim_struct* tmp=m_array_dim;
	m_array_dim=new array_dim_struct;
	m_array_dim->dim=dimension;
	m_array_dim->next=tmp;
	m_pointer_level++;
	m_array_dimension++;
	m_elements *= dimension;
    //printf("%d", dimension);
}

// add  next after this statement
CIdentifier* CIdentifier::set_next(CIdentifier* snext) 
{
	m_next=snext;
	if (snext!=NULL) {
		snext->m_previous=this;
		return snext;
	}
	else {
		return this;
	}
}

CIdentifier* CIdentifier::set_last(CIdentifier* snext)
{
    CIdentifier* last = this;
    while (last->m_next) {
        last = last->m_next;
    }
    last->m_next = snext;
    if (snext != NULL) {
        snext->m_previous = last;
        return snext;
    }
    else {
        return this;
    }
}

CIdentifier* CIdentifier::set_next(CIdentifierBlock* snext) 
{
	m_next=snext->first;
	if (snext->first!=NULL) {
		snext->first->m_previous=this;
		return snext->last;
	}
	else {
		return this;
	}
}


// compare parameter list between function deklaration and function definition
void CIdentifier::checkParameter(CIdentifier* ident2)
{
	if (GetIdList_Next() != GetIdList_End()){ //parameter already defined // Fixme use empty() on parameter list
		bool err=false;
		CIdListIterator tmp1 = GetIdList_Next();
		CIdListIterator tmp2 = ident2->GetIdList_Begin();
		while (tmp1 != tmp1->GetIdList_End() && tmp2 != ident2->GetIdList_End() && err==false) {
			if (memcmp(tmp1->m_declspec,tmp2->m_declspec,sizeof(decl_specifiers))!=0) { // FIXME: memcmp not reliable on structures
				err=true;
				if ( (tmp1->m_declspec->getDataType()->GetTypeId() == TYPE_ENUM) && (tmp2->m_declspec->getDataType()->GetTypeId() == TYPE_ENUM) ) {
					//memcmp does not wotk with enums, so comparison is skipped
					err=false;
				};
				}
			tmp1++;
			tmp2++;
		}
		if (tmp1 != GetIdList_End() || tmp2 != ident2->GetIdList_End()) err=true;
		if (err) ycc_erpnt(102,m_cvdt->name.buf);
	} 
	if (m_function==NULL) m_next=ident2;
}

// for function deklaration xy(fix24 c); -> variable c is not declared!
void CIdentifier::clearFunctionParameter()
{
	if (m_token==IDENTIFIER_FUNCTION) {
		for(CIdListIterator tmp1 = GetIdList_Next(); tmp1 != GetIdList_End(); tmp1++) {
			tmp1->m_cvdt->identifier=NULL;
		}
	}
}


// print declaration
char* CIdentifier::EmitDeclaration(char* buf) 
{
    char buf2[10000]="";
    bool is_typedef = m_declspec->isTypedef();
   
	DoEmit(buf, m_declspec->getStorageSpecifierString());
	DoEmit(buf,m_declspec->getTypeQualifierString());

/*	char struct_union[10]="union";
	if (getDataType() !=NULL) {
		switch (GetTypeId()) {
		case TYPE_STRUCT:
			strcpy(struct_union,"struct");
		case TYPE_UNION:{
			DoEmit(buf,struct_union);
			DoEmit(buf,getDataType()->m_cvdt->name.buf);
			DoEmit(buf,"{\n");
			CIdentifier* actual_ident = getDataType()->m_elements;
			while (actual_ident!=NULL) {
				actual_ident->EmitDeclaration(buf);
				DoEmit(buf,";\n");
				actual_ident=actual_ident->next;
			}
			DoEmit(buf,"}");}
			break;
		default:
		DoEmit(buf, getDataType()->m_cvdt->name.buf);
		break;
		}
	}*/
	if (m_token==IDENTIFIER_TYPEDEF) {
        if (GetTypeId() == TYPE_ENUM) {
			DoEmit(buf,"enum ");
            if (getDataType()->m_cvdt != NULL)
                DoEmit(buf,GetTypeName());
		    DoEmit(buf,"{\n");
		    char nBuf[100];
			CIdentifier *dt_elements = getDataType()->GetElements();
			for(CIdListIterator actual_ident = dt_elements->GetIdList_Begin();
				actual_ident != dt_elements->GetIdList_End();
				actual_ident++) {
                sprintf(nBuf,"%s = %d", actual_ident->m_cvdt->name.buf, (int) actual_ident->m_cvdt->data);
	    		strcat(buf,nBuf);
                sprintf(nBuf,",\t\t//relative Memory Address: %d",actual_ident->m_memAddr);
	    		strcat(buf,nBuf);
		    	DoEmit(buf,"\n");
			}
		    DoEmit(buf,"}");
        } else {
            if (GetTypeId() == TYPE_STRUCT) {
			    DoEmit(buf,"struct ");
                
                if (CStatement::m_filept2!=NULL) {
                    fprintf(CStatement::m_filept2,"struct %s;\n",GetTypeName());
                }

			} else {
			    DoEmit(buf,"union ");
		    }

		    if (!is_typedef) DoEmit(buf,GetTypeName());
		    DoEmit(buf,"{\n");
		    char nBuf[100];
			CIdentifier *dt_elements = getDataType()->GetElements();
		    for(CIdListIterator actual_ident = dt_elements->GetIdList_Begin();
				actual_ident != dt_elements->GetIdList_End();
				actual_ident++) {
			    actual_ident->EmitDeclaration(buf);
                sprintf(nBuf,";\t\t//relative Memory Address: %d",actual_ident->m_memAddr);
	    		strcat(buf,nBuf);
		    	DoEmit(buf,"\n");
		    }
		    DoEmit(buf,"}");
            if (is_typedef) DoEmit(buf,GetTypeName());
		    
        }

	} else {
		DoEmit(buf,GetTypeName());  //print type : int, short
		DoEmit(buf,GetTypeName2()); //print additional type unsigned/signed ...
	}

	//cpointer
	if (as.ycc_pointer_level==-1) {
		DoEmit(buf,"&");
	}
    
    
	// variable name
    if ((m_cvdt!=NULL) && (m_token==IDENTIFIER_pFUNCTION || m_token==IDENTIFIER_pFUNCTION_NOARGS)){
        if (m_pointer_level>0)  DoEmit(buf,"*");
        DoEmit(buf,"(*");
        DoEmit(buf,m_cvdt->name.buf);
        if (m_array_dimension != 0){
            sprintf(buf2,"[%ld]", m_elements);
            DoEmit(buf,buf2);
        }
        if ( m_token==IDENTIFIER_pFUNCTION)
            DoEmit(buf,")");
        else
    	    DoEmit(buf,")()");
  }
    else{
        typeQualifier* tmp_ptqual=m_pt_type_qual;
	    while (tmp_ptqual!=NULL) {
		    DoEmit(buf,"*");
		    if((tmp_ptqual->type_qualifier & 1) !=0) DoEmit(buf,"const");
		    if((tmp_ptqual->type_qualifier & 2) !=0) DoEmit(buf,"volatile");
		    tmp_ptqual=tmp_ptqual->next;
        }
        if (m_cvdt!=NULL) DoEmit(buf,m_cvdt->name.buf);
    }

 	// function arguments
	if (m_token==IDENTIFIER_FUNCTION || m_token==IDENTIFIER_pFUNCTION) {
		DoEmit(buf,"(");
		for(CIdListIterator actual_ident = GetFunArgIds_Begin(); actual_ident != GetFunArgIds_End();) {
			actual_ident->EmitDeclaration(buf);
			actual_ident++;
			if(actual_ident != GetFunArgIds_End()) DoEmit(buf, ","); 
		}
		DoEmit(buf,")");
	}
    if (m_token==IDENTIFIER_FUNCTION_NOARGS || m_token==IDENTIFIER_FUNCTION_NOARGS) {
    		DoEmit(buf,"()");
	}
 
    // identifier is an array
    if (m_array_dimension != 0 && m_token!=IDENTIFIER_pFUNCTION && m_token!=IDENTIFIER_pFUNCTION_NOARGS){
        sprintf(buf2,"[%ld]", m_elements);
        DoEmit(buf,buf2);
       }
	
	// identifier has a bitslice-field
	if ( m_bitslice != -1 ){
		DoEmit(buf," :");
		sprintf(buf2,"%d", m_bitslice);
        DoEmit(buf,buf2);
	}

	// identifier has an direct-initializer
    if ( m_init != NULL){
        if (m_init->list == NULL){ // e.g. int a = 5;
            DoEmit(buf,"=");
            m_init->exp->emitAll(buf,0,0);
        } 
        else {
 
            initializerlist*  tmplist;
            DoEmit(buf,"=");
            DoEmit(buf,"{");

            tmplist = m_init->list;
            while (tmplist != NULL) {
                tmplist->exp->emitAll(buf,0,0);
        
                //if (GetTypeId() == TYPE_INT || GetTypeId() == TYPE_SHORT || GetTypeId() == TYPE_LONG)
                //    sprintf(buf2,"%d", (int)tmplist->data);
                //else
                //    sprintf(buf2,"%f", tmplist->data);

                //DoEmit(buf,buf2);
                tmplist = tmplist->next;
                if (tmplist != NULL) DoEmit(buf,",");
            }
            DoEmit(buf,"}");
        }       
    
	
	}
    return buf;
}

void CIdentifier::DoEmit(char* buf, const char* str)
{
	if(str != NULL) {
		strcat(buf,str);
		strcat(buf," ");
	}
}

void CIdentifier::DoEmit(CStr* buf, const char* str)
{
	if(str != NULL) {
		buf->stradd(str);
		//*buf=*buf+str;
	}
}

char* CIdentifier::EmitAsmDeclaration(char* buf) 
{
    char buf2[10000]="";	
	//bool is_typedef = false;
/*
	DoEmit(buf, m_declspec->getStorageSpecifierString());
	DoEmit(buf, m_declspec->getTypeQualifierString());
*/
	if (m_token==IDENTIFIER_TYPEDEF) {
        if (GetTypeId() == TYPE_ENUM) {
			DoEmit(buf,"enum ");
            if (getDataType()->m_cvdt != NULL)
                DoEmit(buf,getDataType()->m_cvdt->name.buf);
		    DoEmit(buf,"{\n");
		    //CIdentifier* actual_ident = getDataType()->GetElements();
		    char nBuf[100];
			CIdentifier *dt_elements =getDataType()->GetElements();
			for(CIdListIterator actual_ident = dt_elements->GetIdList_Begin();
				actual_ident != dt_elements->GetIdList_End();
				actual_ident++) {
                sprintf(nBuf,"%s = %d", actual_ident->m_cvdt->name.buf, (int) actual_ident->m_cvdt->data);
	    		strcat(buf,nBuf);
                sprintf(nBuf,",\t\t! relative Memory Address: %d",actual_ident->m_memAddr);
	    		strcat(buf,nBuf);
		    	DoEmit(buf,"\n");
			}
		    DoEmit(buf,"}");
        } else {
            if (GetTypeId() == TYPE_STRUCT) {
			    //DoEmit(buf,"struct ");
                
                if (CStatement::m_filept2!=NULL) {
                    fprintf(CStatement::m_filept2,"!struct %s;\n",GetTypeName());
                }


		    } else {
			    DoEmit(buf,"!union ");
		    }

		    //DoEmit(buf, getDataType()->m_cvdt->name.buf);
		//    DoEmit(buf,"{\n");
		    //CIdentifier* actual_ident = getDataType()->GetElements();
		    char nBuf[100];
			CIdentifier *dt_elements = getDataType()->GetElements();
		    for(CIdListIterator actual_ident = dt_elements->GetIdList_Begin();
				actual_ident != dt_elements->GetIdList_End();
				actual_ident++) {
			    actual_ident->EmitAsmDeclaration(buf);
                sprintf(nBuf,"\t\t!relative Memory Address: %d",actual_ident->m_memAddr);
	    		strcat(buf,nBuf);
		    	strcat(buf,"\n\t");
		    }
		    //DoEmit(buf,"}");
            //if (is_typedef) DoEmit(buf,GetTypeName());
		    
        }

	} else {
		//	DoEmit(buf, getDataType()->m_cvdt->name.buf);
	}

	//cpointer
	if (as.ycc_pointer_level==-1) {
		DoEmit(buf,"&");
	}
	//typeQualifier* tmp_ptqual=m_pt_type_qual;
	/*while (tmp_ptqual!=NULL) {
		DoEmit(buf,"*");
		if((tmp_ptqual->type_qualifier & 1) !=0) DoEmit(buf,"const");
		if((tmp_ptqual->type_qualifier & 2) !=0) DoEmit(buf,"volatile");
		tmp_ptqual=tmp_ptqual->next;
	}*/
	// function arguments
	if (m_function!=NULL/*m_token==IDENTIFIER_FUNCTION*/) {
		strcpy(buf,":");
		//CIdentifier* actual_ident=next;
		/*while (actual_ident!=NULL) {
            actual_ident->EmitDeclaration(buf);
            actual_ident=actual_ident->next;
			if (actual_ident!=NULL) DoEmit(buf,",");
		}*/
		//DoEmit(buf,")");
	}

	// variable name
	if (m_cvdt!=NULL) DoEmit(buf,m_cvdt->name.buf);
	if (m_function==NULL && m_token!=IDENTIFIER_TYPEDEF)
	{
		DoEmit(buf,"\t\t.equ");
		sprintf(buf2,"\t%d",m_memAddr);
		DoEmit(buf,buf2);
	}

        
  /*  if (m_token==IDENTIFIER_FUNCTION_NOARGS) {
    		DoEmit(buf,"()");
	}*/
    
// identifier is an array
 /*   if (m_array_dimension != 0){
        sprintf(buf2,"[%d]", m_elements);
        DoEmit(buf,buf2);
       }
    
    // identifier has an direct-initializer
    if ( m_init != NULL){
	    DoEmit(buf,"=");
        m_init->exp->emitAll(buf);
    }*/
    
    return buf;
}

void CIdentifier::adaptInitializerList(initializerlist* list)
{
    int i,j;    //defined array dimensions
    int k=0;    //initialised array elements
    int l=0;    //initialised array elements
    int sp=0;
    initializerlist*  tmplist;
    initializerlist*  tmplist2;
    
	m_init = list;
    //no array
    if ( !(m_array_dimension > 0)) return;

	m_init->exp=NULL;	// force exp to NULL, otherwise undefined 
    //one dimensional array, no width specified: count initializerlist
    tmplist = list->list;
    if (m_array_dim != NULL && m_array_dim->dim == -1){
        while (tmplist != NULL){
            k++;
            tmplist = tmplist->next;
        }
        m_array_dim->dim = k;
        m_elements = k;
        m_array_dimension = 1;
    }

 
    //two dimensional array, no width specified for second dimension: count
    if (m_array_dimension==1 && m_pointer_level==2){
        tmplist = list->list;
        int c1,c2;
        c1=c2=0;
        
        if (m_array_dim != NULL && m_array_dim->next->dim == -1){
            while (tmplist != NULL){
                
                tmplist2 = tmplist->list;
                if (tmplist2 == NULL) {
                    c1++;            
                    tmplist = tmplist->next;
                    continue;
                }
                
                if (tmplist->list != NULL) c2++;
                tmplist = tmplist->next;
            }
            
            if (c2==0)      m_array_dim->next->dim = (c1+1) / (m_array_dim->dim);    // a = {1,2,3,4}       : c1=4, c2=0
            else if (c1==0) m_array_dim->next->dim = c2;                             // a = {{1,2},{3,4}}   : c1=0, c2=2
            else            m_array_dim->next->dim = c2+1;                           // a = {1,{3,4}        : c1:1, c2=1 
            
            m_elements = m_array_dim->next->dim * m_array_dim->dim;
            m_array_dimension = 2;
        }
    }

    if (m_array_dimension == 2) { 
        j = m_array_dim->dim;
        i = m_array_dim->next->dim;    
    }
    else {
        j = m_array_dim->dim;
        i = 1;    
    }

    CExpression** values = new CExpression* [i*j];
    CExpression** stack  = new CExpression* [i*j];
    bool*  dim    = new bool[i*j];
 
	for (int c=0; c<i*j; c++){ values[c]= CExpression::makeConstant(0.0, OP_CONST_INT);}
    for (int c=0; c<i*j; c++){ dim[c]=false;}

    //put values on "STACK" to reverse order 
    tmplist = list->list;
    while (tmplist != NULL) {
        tmplist2 = tmplist->list;
        if (tmplist2 == NULL) {
            stack[sp++]   = (tmplist->exp);
            tmplist = tmplist->next;
            continue;
        }
        while (tmplist2 != NULL){
            stack[sp++]   = tmplist2->exp;
            tmplist2 = tmplist2->next;
        }
        dim[sp] = true;
        tmplist = tmplist->next;
    }
    
    //stuff with 0 for one-dim array initializer-list
    l=1; k=0; sp--;
    while (sp >= 0){
        values[k] = stack[sp];
        if (dim[sp] == true) {
            k = l*j-1;
            l++;
        }
        sp--; k++;
    }

    //put in new one-dimensional full-blown initializer-list
    tmplist = new initializerlist();
    list->list = tmplist;
    for (l=0; l<i*j; l++){
        tmplist->exp = values[l];
        if (l != (i*j-1)) {
            tmplist->next = new initializerlist();
            tmplist=tmplist->next;
        }
    }
    return;
}


const char *CIdentifier::getName() const { 
	return m_cvdt->name.buf;
};

void CIdentifier::AppendToLastProducerList(CProducer *producer)
{
	if(!last_producerList)
		last_producerList = new CProducerList;

	last_producerList->push_back(producer);
}


//FIXME: TEMPORARY WRAPPER
CIdListIterator CIdListIterator::operator++(int) { // postfix
	CIdListIterator it = *this;
	if(current != NULL) current = current->m_next;
	return it;
};
//FIXME: TEMPORARY WRAPPER
CIdListIterator& CIdListIterator::operator++() {
	if(current != NULL) current = current->m_next;
	return *this;
};

CIdListIterator& CIdListIterator::operator--() {
	if(current != NULL) current = current->m_previous;
	return *this;
};

mem_usage_struct* CIdentifier::GetFuncMemUsage() const {
	return m_function->m_mem_usage;
};

CStatement* CIdentifier::GetFunctionCompoundStatements() const {
	return m_function->list;
};

long CIdentifier::GetTypeId() const {
		return getDataType()->GetTypeId();
};

long CIdentifier::GetTypeSize() const {
	// check for pointer
	int indirection=m_pointer_level-m_array_dimension;
	if (indirection>0) {
		return as.pointerSize;		//pointer always 32 bits
	}
	return getDataType()->GetTypeSize();
};

long CIdentifier::GetTypeAlign() const {
	return getDataType()->GetTypeAlign();
};

char* CIdentifier::GetTypeName() const {
	return getDataType()->GetTypeName();
};

char* CIdentifier::GetTypeName2() const {
	if (getDataType2() != NULL)
		return getDataType2()->GetTypeName();
	else return NULL;
};

CStr& CIdentifier::GetTypeNameStr() const {
	return getDataType()->GetTypeNameStr();
};

int CIdentifier::GetTypeToken() const {
	return getDataType()->GetTypeToken();
};

CDataType* CIdentifier::getDataType() const {
	if(m_declspec!=NULL)return m_declspec->getDataType();
	return NULL;
};

CDataType* CIdentifier::getDataType2() const {
 	if(m_declspec->datatype2 != NULL) 
		return m_declspec->getDataType2();
	else return NULL;
};
