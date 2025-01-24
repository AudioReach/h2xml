/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////////////
//! \brief					 CPointer class implementation									\file
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CPointer.h"

CPointer::CPointer()
{
	assignOffset=0;
	type=0;
	isArray=0;
	isGlobal=0;
	curAssignedIdent=NULL;
	lastUse=0;
}

