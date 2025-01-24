/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

/**
*	\file
*/

#ifndef _CVALUE_H__INCLUDED_
#define _CVALUE_H__INCLUDED_

#include "h2xml_common.h"

class CValue {
public:
	CValue();

	union {
        long64 l;
        ulong64 ul;
		//long l;
		//unsigned long ul;
		unsigned char c[8];
		double f;
        float f32;
	};
	int m_bitSize;
	CStr dataStr;									// data as string as read by parser
	CStr labelStr;									// in case value was evaluated by define or enum, this string contains the original label (before expansion)
	eAnArgTypes format;
	eAnArgDPLYType dplyType;						// type of input data
	static eAnArgDPLYType globalDplyType;			// force output to hex or integer for all values
    static int globalFloatPrecision;                // number of digits for float output


	bool isSet;

	void setLong(long64 v, char* text, eAnArgTypes f = ANARG_INTEGER);
	void setFloat(double v, char* text);
	char* getValuStr(char*buf, bool expandEnums = true);
	bool gt(CValue* cmpValue);			// greater than (signed / unsigned)
	bool lt(CValue* cmpValue);			// less than (signed / unsigned)
	void signExtend(int bitsize=-1);
    void copy(CValue* from);
    void convertByteTypes(int identTypeId);
};

#endif //_CVALUE_H__INCLUDED_

