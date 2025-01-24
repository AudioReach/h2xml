/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

//!	Defines CFunctionList class
/*! \file 
*/

#include "CStr.h"

#ifndef CFUNCTIONLIST__H__INCLUDED_
#define CFUNCTIONLIST__H__INCLUDED_

class CFunctionDescriptor;

class CFunctionList {
	CFunctionList(); // prevent no-args constructor from being called

	CStr functionName;
	int nrPointers;
	int nrRegisters;
	int codeLine;
	CFunctionList *next;
public:
	CFunctionList(CStr &_functionName, int _nrPointers, int _nrRegisters, int _codeLine, CFunctionList *_prev);
	int getCodeLine() const { return codeLine; };
	int getNrPointers() const { return nrPointers; };
	int getNrRegisters() const { return nrRegisters; };
	CFunctionList *getNext() const { return next; };
};

#endif	// CFUNCTIONLIST__H__INCLUDED_

