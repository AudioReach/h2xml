/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

//!	Defines CPointer class
/*! \file 
*/

#include "CStr.h"

#ifndef CPOINTER__H__INCLUDED_
#define CPOINTER__H__INCLUDED_

class CIdentifier;	// forward declaration 

class CPointer {
public:
	CPointer();
	int assignOffset;
	long type;
	CStr typeName;
	bool isArray;
	bool isGlobal;
	int lastUse;
	CIdentifier *curAssignedIdent;

	void clearPointer(){memset(this,0,sizeof(CPointer));};
};

#endif	// CPOINTER__H__INCLUDED_

