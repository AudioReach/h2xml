/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////////////
//! \brief					 Hashtable class implementation										\file
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "hashtable.h"
#include "h2xml_global.h"

//#define TESTHASH

#ifdef TESTHASH

int main()
{
	CHashTable tmp;

	// insert values
	tmp.ReadFile("ydfp1_1.def");
	tmp.SetValue("abc", 33);
	tmp.SetString("def", "hallo");

	// get values
	double a = tmp.GetValue("ydfp1_mod128");
	double b = tmp.GetValue("abc");
	CStr c = tmp.GetString("def");
	CStr d = tmp.GetString("ydfp1_mod128");

	// erase table
	tmp.EraseTable();

	return 0;
}

#endif // #ifdef TESTHASH


///////////////////////////////////////////////////////////////////
// Funktionen fuer die Hash Tabelle und zur Verarbeitung von vdt
///////////////////////////////////////////////////////////////////
CHashTable::CHashTable(int size)
{
	m_hashsize = size;
	hashtab = new Cvdt*[m_hashsize];
	for (int i = 0; i<m_hashsize; i++)
		hashtab[i] = NULL;
	m_default = "";
}

CHashTable::CHashTable()
{
	m_hashsize = DEFAULT_HASHSIZE;
	hashtab = new Cvdt*[m_hashsize];
	for (int i = 0; i<m_hashsize; i++)
		hashtab[i] = NULL;
	m_default = "";
}


CHashTable::~CHashTable()
{
	hashClear();
	delete[] hashtab;
}

void CHashTable::hashClear()
{
	for (int i = 0; i<m_hashsize; i++){
		if (hashtab[i] != NULL) delete hashtab[i];
		hashtab[i] = NULL;
	}
}

///////////////////////////////////////////////////////////////////
// liefert Hash-Wert fuer Hash-Tabelle
///////////////////////////////////////////////////////////////////
unsigned CHashTable::hash(const char *s)
{
	unsigned hashval = 0;
	for (hashval = 0; *s != 0; s++)
		hashval = toupper(*s) + 31 * hashval;		//case insensitiv -> umwandlung in Capitals
	return hashval % m_hashsize;
}

///////////////////////////////////////////////////////////////////
// liefert Zeiger auf vdt, falls Eintrag vorhanden, sonst null
///////////////////////////////////////////////////////////////////
Cvdt* CHashTable::hashlookup(const char* name, int er)
{
	m_found = false;
	if (name == NULL) return NULL;
	char* name_in = (char*)name;										// remove preceeding ":" (in label :xyz)
	if (name[0] == ':') name_in = (char*)&name[1];
	Cvdt *np;
	//int ercode=102;
	for (np = hashtab[hash(name_in)]; np != NULL; np = np->next) {
		if (strcmp(name_in, np->name.buf) == 0 && np->cases == 0) {	// Case Sensitive
			m_found = true;
			return np;											// gefunden
		}
		if (strcasecmp(name_in, np->name.buf) == 0) {				// NOT Case Sensitive
			if (np->cases>0) {
				m_found = true;
				return np;										// gefunden
			}
			else {
				//ercode=104;									// Error Message: No match because of case sensitivity!
			}
		}
	}
	return NULL;												//nicht gefunden
}

///////////////////////////////////////////////////////////////////
// liefert Zeiger auf vdt, neuer Eintrag, falls nicht vorhanden
// skipperr: 0	yields an error if lable already defined
//			 1	no error output if label already defined
///////////////////////////////////////////////////////////////////
Cvdt* CHashTable::hashinstall(const char* name, int skiperr)
{
	if (name == NULL) return NULL;
	Cvdt *np = NULL;
	unsigned hashval;
	char* name_in = (char*)name;										// remove preceeding ":" (in label :xyz)
	if (name[0] == *":") name_in = (char*)&name[1];
	if ((np = hashlookup(name_in, 1)) == NULL){	//Nicht vorhanden
		np = newCvdt(0);
		if (np == NULL)
			printf("Compiler error, cannot create Hashtable entry!\n");
		np->name = name_in;
		hashval = hash(name_in);
		np->next = hashtab[hashval];
		hashtab[hashval] = np;

	}
	else
	{
		//already defined
	}
	return np;
}

Cvdt* CHashTable::newCvdt(int token)
{
	Cvdt* tmp = new(Cvdt);
	tmp->token = token;
	return tmp;
}


void CHashTable::SetValue(const char* name, double value, int token, int cases)
{
	Cvdt *tmp = htab.hashinstall(name);
	tmp->data = value;
	tmp->cases = cases;
	char tmpstr[100];
	sprintf(tmpstr, "%f", value);
	//itoa(value,tmpstr,10);
	tmp->valstr = tmpstr;
	tmp->token = token;
}

double CHashTable::GetValue(const char* name)
{
	double value = (-1);
	Cvdt* tmp = hashlookup(name);
	if (tmp != NULL) value = tmp->data;
	return value;
}

char* CHashTable::GetString(const char* name)
{
	CStr string;
	Cvdt* tmp = hashlookup(name);
	if (tmp != NULL) return tmp->valstr.buf;
	else return m_default.buf;
}

void CHashTable::SetString(const char* name, const char* string, int token, int cases)
{
	Cvdt *tmp = htab.hashinstall(name);
	tmp->data = 0;
	tmp->cases = cases;
	tmp->valstr = string;
	tmp->token = token;
}

///////////////////////////////////////////////////////////
// CListelements
///////////////////////////////////////////////////////////
Cvdt* newCvdt(int token)
{
	Cvdt* tmp = new(Cvdt);
	memset(tmp, 0, sizeof(Cvdt));
	tmp->name = "";
	tmp->token = token;
	return tmp;
}

Cvdt::Cvdt()
{
	memset(this, 0, sizeof(Cvdt));
    shortname = "";
    enumName = "";
}

Cvdt::Cvdt(int token)
{
	next = NULL;
}

Cvdt::~Cvdt()
{
	if (this->next != NULL) delete this->next;
}

// insert new vdt element at end of list
Cvdt* CvdtAddToEnd(Cvdt* &vdt, const char* name)
{
	Cvdt* tmp = newCvdt(0);
	tmp->name = name;

	Cvdt* list = vdt;

	if (list != NULL)
	{
		while (list->next != NULL)
			list = list->next;
		list->next = tmp;
	}
	else
	{
		vdt = tmp;
	}
	return tmp;
}

// insert new vdt element at Beginning of list
Cvdt* CvdtAddToBegin(Cvdt* &vdt, const char* name)
{
	Cvdt* tmp = newCvdt(0);
	tmp->name = name;
	tmp->next = vdt;
	tmp->filename = as.inputfile;
	tmp->linecnt = as.linecnt;
	vdt = tmp;
	return tmp;
}

// insert new vdt listelement at Beginning of list
Cvdt* CvdtAddToBegin(Cvdt* &vdt, Cvdt* list)
{
	Cvdt* tmp = newCvdt(0);
	tmp->list = list;
	tmp->next = vdt;
	tmp->filename = as.inputfile;
	tmp->linecnt = as.linecnt;
	vdt = tmp;
	return tmp;
}

// insert new vdt element at Beginning of list
Cvdt* CvdtAddToBegin(Cvdt* &vdt, double value)
{
	Cvdt* tmp = newCvdt(0);
	tmp->data = value;
	tmp->next = vdt;
	tmp->filename = as.inputfile;
	tmp->linecnt = as.linecnt;
	vdt = tmp;
	return tmp;
}

// remove vdt listelement at Beginning of list
Cvdt* CvdtRemoveFromBegin(Cvdt* &vdt)
{
	Cvdt* tmp = vdt;

	if (vdt != NULL) {
		vdt = vdt->next;

		// delete object
		tmp->next = NULL;
		tmp->list = NULL;
		delete tmp;
	}
	return vdt;
}

Cvdt* insertint(const char* name, int value, int cases, int skiperr)
{
	Cvdt *tmp = htab.hashinstall(name, skiperr);
	tmp->data = value;
	tmp->cases = cases;
	char tmpstr[1000];
	sprintf(tmpstr, "%d", value);
	//itoa(value,tmpstr,10);
	tmp->valstr = tmpstr;
	tmp->token = TOKEN_INUM;
	return tmp;
}

void insertvalstr(const char* name, const char* valstr, int cases)
{
	Cvdt *tmp = htab.hashinstall(name);
	tmp->data = 0;
	tmp->cases = cases;
	tmp->valstr = valstr;
	tmp->token = TOKEN_OPCODE;
}

void insertstr(const char* name, const char* valstr, int cases)
{
	Cvdt *tmp = htab.hashinstall(name);
	tmp->data = 0;
	tmp->cases = cases;
	tmp->valstr = CStr("\"") + valstr + "\"";
	tmp->token = TOKEN_STRING;
}


