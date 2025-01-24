/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

#include "CFunctionList.h"

CFunctionList::CFunctionList(CStr &_functionName, int _nrPointers, int _nrRegisters, int _codeLine, CFunctionList *_prev)
: functionName(_functionName)
, nrPointers(_nrPointers)
, nrRegisters(_nrRegisters)
, codeLine(_codeLine)
, next(0)
{
	if(_prev)
		_prev->next = this;
}

