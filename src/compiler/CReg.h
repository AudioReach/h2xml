/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

//!	Defines CReg class
/*! \file 
*/

#include "CIdentifier.h"

#ifndef CREG__H__INCLUDED_
#define CREG__H__INCLUDED_

class CReg {

public:
	CReg();
	int pointerNr;
	int regNr;		// General Purpose Register number (-1 for ACCU)
	int offset;
	int offsetBytes;
	long type;
	bool loadFromStack;
	CIdentifier* identifier;
};

#endif	// CREG__H__INCLUDED_

