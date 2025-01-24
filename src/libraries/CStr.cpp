/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////////////
//! \brief					 CStr class implementation											\file
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "CStr.h"
#include <ctype.h>


typedef enum  {
	TMSG,
	TINFO,
	TWARNING,
	TERROR,
	knErTypes
} eMsgTypes;

extern void dbgPrintf(eMsgTypes type, char* format, ...);


//#define CSTR_DEBUG _DEBUG

CStr::CStr()
{
	buf=NULL;
	bufAlloc = buf;
	len=0;
	buflen = 0;
}

CStr::CStr(const char *str)
{
	len=(int) strlen(str);
	buflen=len+2;
	buf= new char[buflen];
	bufAlloc = buf;
	memmove(buf,str,len+1);

#ifdef CSTR_DEBUG
	check();
#endif

}

CStr::CStr(const CStr& cstr)
{
	buf=NULL;
	bufAlloc = buf;
	len=0;
	*this=cstr;

#ifdef CSTR_DEBUG
	check();
#endif
}


CStr::~CStr()
{
	if (bufAlloc != NULL){
		delete[] bufAlloc;
		buflen = 0;
		len = 0;
	}
}

void CStr::insert(const char* ins)
{
	if (buf!=NULL){
		delete[] buf;
	}
	len=(int) strlen(ins);
	buf=new char[len+2];
	bufAlloc = buf;
	memmove(buf,ins,len+1);
	buflen = len + 2;
#ifdef CSTR_DEBUG
	check();
#endif

}

void CStr::add(const char* in1, const char* in2)
{
	int len1=(int) strlen(in1);
	int len2=(int) strlen(in2);
	len=len1+len2;
	char* buf_new=new char[len+2];
	memmove(buf_new,in1,len1);
	memmove(&buf_new[len1],in2,len2+1);
	if (bufAlloc != NULL){
		delete[] bufAlloc;
	}
	buf=buf_new;
	bufAlloc = buf;
	buflen = len+2;
#ifdef CSTR_DEBUG
	check();
#endif
}

// fast string add for large strings
void CStr::stradd(const char* in, int minsize) {
	int inlen=strlen(in);
	if ((len+inlen+2)>buflen) {
		int size=4*(len+inlen)+2;
		if (size<minsize) size = minsize;
		char* buf_new=new char[size];
		buflen=size;
		if (bufAlloc != NULL){
			strcpy(buf_new, buf);
			delete[] bufAlloc;
		}
		else{
			buf_new[0] = '\0';
		}
		buf=buf_new;
		bufAlloc = buf;
	} 
	strcpy(buf + len, in);
	len = inlen + len;

#ifdef CSTR_DEBUG
	check();
#endif
}

void CStr::clear()
{
	if (buf && buflen) {
		buf[0] = 0;
		len = 0;
	}
}


void CStr::initialize(char init, int n, int size) 
{
	if (size < n + 2) {
		size = n + 2;
	}

	if (bufAlloc != NULL){
		delete[] bufAlloc;
	}
	buf = new char[size];
	bufAlloc = buf;
	memset(buf, 0, size);
	memset(buf, init, n);
	buf[n] = 0;
	len = n;
	buflen = size;

#ifdef CSTR_DEBUG
	check();
#endif
}


// if dir = true: search from end	
bool CStr::Replace(const char* a, const char* b, bool dir)
{
	char* tmp=strstr(buf,a);
	if (dir) {
		char * lasttmp=tmp;
		while (tmp!=NULL) {
			lasttmp=tmp;
			tmp=strstr(&lasttmp[1],a);
		}
		tmp=lasttmp;		// search last occurence
	}
	bool ret=false;
	if (tmp!=NULL) {
		int lena = strlen(a);
		int newlength = (int)strlen(buf) - lena + (int)strlen(b) + 2;
		char* newbuf = new char[newlength];
		int start=(int) (tmp-buf);
		strncpy(newbuf,buf,start);
		strcpy(&newbuf[start],b);
		strcat(&newbuf[start], &buf[(int)(start)+lena]);
		newbuf[newlength-2]=0;
		if (bufAlloc){
			delete[] bufAlloc;
		}
		buf=newbuf;
		bufAlloc = buf;
		len=(int) newlength-2;
		buflen = newlength;
		ret=true;

	}
#ifdef CSTR_DEBUG
	check();
#endif
	return ret;
}

int CStr::ReplaceAll(const char* a, const char* b)
{
	int cnt=0;
	while (Replace(a,b)) {
		cnt++;
	}
#ifdef CSTR_DEBUG
	check();
#endif
	return cnt;
}

int CStr::RemoveQuotes()
{
	RemoveLeadingBlanks();
	if (buf[0] == '"' || buf[0] == '\''){
		buf++;
		len--;
		buflen--;
	}
	RemoveTrailingBlanks();
	if (buf[len - 1] == '"' || buf[len - 1] == '\''){
		buf[len - 1] = '\0';
		len--;
	}
	return 0;
	// return ReplaceAll("\"", "");
}

char* CStr::RemoveBlanks()	//! remove blanks, skip escaped blanks
{
	int ind1=0;
	int ind2=0;
	if (buf!=NULL) {
		while(buf[ind2]!=0){
			if (buf[ind2]=='\\' && buf[ind2+1]==' '){
				buf[ind1]=' ';
				ind1++;
				ind2++;
			}
			else if (buf[ind2]==' '){
			} else {
				buf[ind1]=buf[ind2];
				ind1++;
			}
			ind2++;
		}
		buf[ind1]=0;
	}
	return buf;
}

char* CStr::RemoveLeadingBlanks()	//! remove blanks, skip escaped blanks
{
	while (*buf==' ' || *buf=='\t') {
		buf++;
		len--;
		buflen--;
	}

#ifdef CSTR_DEBUG
	check();
#endif

	return buf;
}

char* CStr::RemoveTrailingBlanks()	//! remove blanks, skip escaped blanks
{
	char* end=&buf[strlen(buf)-1];
	while (end>buf && (*end==' ' || *end=='\t')) {
		*end='\0';
		end--;
	}
	len = strlen(buf);

#ifdef CSTR_DEBUG
	check();
#endif

	return buf;
}

// replace "\" by "/" for linux , solaris
void CStr::AdjustForUnix()
{
	#ifndef WIN32
		ReplaceAll("\\","/");
	#else
		ReplaceAll("/","\\");
	#endif

#ifdef CSTR_DEBUG
		check();
#endif

}


// extract string between start and end. If mode =0 without start / end condition,
// if mode=1 with start / end conditions
bool CStr::Extract(const char* start, const char* end, int mode)
{
	char* istart=strstr(buf,start);
	char* iend;
	bool retval = false;
	if (istart!=NULL)	
		 iend=strstr(&istart[(int) strlen(start)],end);
	
	if (istart!=NULL && iend!=NULL && iend-istart-(int) strlen(start)>0){
		if (mode==0){
			memmove(buf, &istart[(int)strlen(start)], iend - istart - (int)strlen(start));
			buf[iend-istart-(int) strlen(start)]=0;
		}
		else {
			memmove(buf, istart, iend - istart + (int)strlen(end));
			buf[iend-istart+(int) strlen(end)]=0;
		}
		len=(int) strlen(buf);
		retval= true;
	}
#ifdef CSTR_DEBUG
	check();
#endif
	return retval;
}

// extract string between inner ${ and }, {s can be nested!
bool CStr::ExtractReplace()
{
	char start[]="${";
	char end[]="}";
	int mode=0;
	char* istart=this->InStringLast(start);
	char* iend;
	bool retval = false;
	if (istart!=NULL)	
		 iend=strstr(&istart[(int) strlen(start)],end);
	
	if (istart!=NULL && iend!=NULL && iend-istart-(int) strlen(start)>0){
		if (mode==0){
			memmove(buf, &istart[(int)strlen(start)], iend - istart - (int)strlen(start));
			buf[iend-istart-(int) strlen(start)]=0;
		}
		else {
			memmove(buf, istart, iend - istart + (int)strlen(end));
			buf[iend-istart+(int) strlen(end)]=0;
		}
		len=(int) strlen(buf);
		retval=true;
	}
#ifdef CSTR_DEBUG
	check();
#endif
	return retval;
}


// find <find> in String, return 0 if not found
char* CStr::InString(const char* find)
{
	return strstr(buf,find);
}

char* CStr::cut(const char* in1)						//! cut string at first occurence of in1
{
	char* retval = NULL;
	char* tmp=strstr(buf,in1);
	if (tmp != NULL) {
		tmp[0] = 0;
		retval = tmp + strlen(in1);
	}
	return retval;
}

//! cut str at token, return pointer to string after token; if dir==true: search from end
char* CStr::cut(char *str, char* token, bool dir)
{
	char* retval = NULL;

	char* tmp = strstr(str, token);
	if (dir){
		char* lasttmp=tmp;
		while (tmp) {
			lasttmp = tmp;
			tmp = strstr(tmp+1, token);			// find last occurence
		}
		tmp = lasttmp;
	}
	if (tmp != NULL) {
		tmp[0] = 0;
		retval = tmp + strlen(token);
	}
	return retval;
}


// find last occurence of <find> in string, return pointer to <find>, Null if not found
char* CStr::InStringLast(const char* find)
{
	char* tmp1=buf;
	char* tmp2=NULL;
	while(tmp1!=NULL) {
			tmp1=strstr(tmp1,find);
			if (tmp1!=NULL) {
				tmp2=tmp1;
				tmp1++;
			}

	}
	return tmp2;
}


CStr CStr::operator+ (CStr str)
{
	CStr tmp;
	int len1=(int) strlen(buf);
	int len2=(int) strlen(str.buf);
	tmp.buf= new char[len1+len2+2];
	tmp.bufAlloc = tmp.buf;
	memmove(tmp.buf,buf,len1);
	memmove(&(tmp.buf[len1]),str.buf,len2+1);

	//strcat(tmp.buf,str.buf);
	tmp.len=len1+len2;
	tmp.buflen = len1 + len2 + 2;
#ifdef CSTR_DEBUG
	tmp.check();
#endif
	return tmp;
}

CStr& CStr::operator=(const CStr& str)
{
	char * instr;
	char tmp[]="";
	if (&str==NULL) {
		instr=tmp;
	}
	else
	{
		if(str.buf==NULL)
		{
			instr=tmp;
		}
		else
		{
			instr=str.buf;
		}
	}


	len=(int) strlen(instr);
	if (bufAlloc != NULL){
		delete[] bufAlloc;
	}
	buf= new char[len+2];
	bufAlloc = buf;
	memmove(buf,instr,len+1);
	buflen=len+2;

#ifdef CSTR_DEBUG
	check();
#endif
	return *this;	
}

CStr&  CStr::operator= (const char* str)
{
	if (bufAlloc != NULL) {
		delete[] bufAlloc;
	}
	if (str==NULL) {
		buf= new char[2];
		bufAlloc = buf;
		strcpy(buf,"");
		len=0;
	}
	else {
		len=(int) strlen(str);
		buf= new char[len+2];
		bufAlloc = buf;
		memmove(buf,str,len+1);
	}
	buflen=len+2;

#ifdef CSTR_DEBUG
	check();
#endif
	return *this;
}


void CStr::toUpper()
{
	int len=strlen(buf);
	if (buf!=NULL && len>0 ) {
		for (int i=0;i<len;i++){
			buf[i]=toupper(buf[i]);
		}
	}
}



// replace tabs with spaces, tab length=n
CStr CStr::TabToSpace(int n)
{
	if (buf!=NULL && len>0 && n>=0) {
		char* tmp=new char[len*(n+1)];   //temporary buffer
		tmp[0]=0;
		int cnt=0;
		for (int i=0;i<len;i++){
			if(buf[i]==*"\t") {
				int inc=n-(cnt % n);
				strncat(tmp,"                            ",inc);
				cnt+=inc;
			}
			else {
				strncat(tmp,&buf[i],1);
				cnt++;
			}
		}
		tmp[cnt]=0;
		*this=tmp;
		delete[] tmp;
	}
#ifdef CSTR_DEBUG
	check();
#endif
	return *this;
}

// capitize string
void CStr::toCapital()
{
	unsigned int i = 0;
	for (i = 0; i<strlen(buf); i++){ //
		buf[i] = toupper(buf[i]);
	}
}

// tolower string
void CStr::toLower()
{
	unsigned int i = 0;
	for (i = 0; i<strlen(buf); i++){ //
		buf[i] = tolower(buf[i]);
	}
}

// Wildcard pattern matching
// This instance contains the pattern, e.g. "abc*.yini"
// example:
//	CStr pattern="abd*.yini";
//	bool test= pattern.checkWildcards("abdcesodufhnv.yini");
bool CStr::checkWildcards(char* string)
{
	CStr pattern = buf;
	char *found = string;
	char *match = strtok(pattern.buf, "*");

	while (found != NULL && match != NULL) {
		found = strstr(found, match);
		if (found != NULL) {
			found = found + strlen(match);
		}
		match = strtok(NULL, "*");
	}

	bool retval = false;
	if (found != NULL && found[0] == '\0') {
		retval = true;
	}
#ifdef CSTR_DEBUG
	check();
#endif
	return retval;
}

// extract string between inner ${ and }, {s can be nested!
// If mode =0 without start / end condition,
// if mode=1 with start / end conditions
bool CStr::ExtractReplace(const char* start, const char* end, int mode)
{
	char* istart = this->InStringLast(start);
	char* iend;
	if (istart != NULL)
		iend = strstr(&istart[(int)strlen(start)], end);

	if (istart != NULL && iend != NULL && iend - istart - (int)strlen(start)>0){
		if (mode == 0){
			memmove(buf, &istart[(int)strlen(start)], iend - istart - (int)strlen(start));
			buf[iend - istart - (int)strlen(start)] = 0;
		}
		else {
			memmove(buf, istart, iend - istart + (int)strlen(end));
			buf[iend - istart + (int)strlen(end)] = 0;
		}
		len = (int)strlen(buf);
		return true;
	}
#ifdef CSTR_DEBUG
	check();
#endif
	return false;
}

// replace with environment variable, declared by %var%
bool CStr::replaceEnv()
{
	CStr tmp = *this;
	bool retval = false;
	if (tmp.Extract("%", "%", 0)) {
		char* tmp2 = getenv(tmp.buf);
		if (tmp2 != NULL){
			tmp = *this;
			tmp.Extract("%", "%", 1);
			Replace(tmp.buf, tmp2);
			retval = true;
		}
	}
#ifdef CSTR_DEBUG
	check();
#endif
	return retval;
}

bool CStr::isEmpty()
{
    if (buf == NULL) {
        return true;
    };
	return buf[0] == '\0';
}

void CStr::check()
{
#ifdef CSTR_DEBUG
	int _len = strlen(buf);
	if (len != _len) {
		dbgPrintf(TERROR,"Strlen doesn_t match: Strlen=%d, len=%d\n", _len, len);
	}
	if (buflen < _len + 2) {
		dbgPrintf(TERROR,"Buffersize too small: buffer=%d, strlen=%d\n", buflen, _len);
	}
#endif
}

int CStr::readFile(FILE* fp)
{
	size_t lSize, result=0;
	fseek(fp, 0, SEEK_END);
	lSize = ftell(fp);
	rewind(fp);
	if (bufAlloc){
		delete bufAlloc;
	}
	buf = new char[lSize + 2];
	bufAlloc = buf;
	if (bufAlloc) {
		result = fread(buf, 1, lSize, fp);
		if (result != lSize) {
			delete[] bufAlloc;
			buflen = 0;
			len = 0;
			result = 0;
		}
		else {
			buflen = lSize + 2;
			len = lSize;
		}
	}

	return result;
}