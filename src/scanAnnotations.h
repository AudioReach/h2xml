/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

#ifndef __SCAN_ANNOTATIONS__H__INCLUDED_
#define __SCAN_ANNOTATIONS__H__INCLUDED_
#include "h2xml_common.h"

void lexBeginModule();
void lexEndModule();
void lexBeginParameter(char* text);
void lexBeginElement(char* text);
void evaluateKeyword(char* text, bool hasArgument);	
void endStateArgument();
CAnnotationData* addModule(CAnnotationData* listStart, CStr* name);
CAnnotationData* addXmlStatement(CAnnotationData* listStart, CStr* name);
void startDoxyComment();
void endDoxyComment();




#endif  //__SCAN_ANNOTATIONS__H__INCLUDED_