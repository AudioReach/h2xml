/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

//!	Includes YDFP specific definitions
/*! \file 
*/
#ifndef h2xml_definitions_H__INCLUDED_
#define h2xml_definitions_H__INCLUDED_
// definition for datatype sizes in Bytes

#define SIZE_CHAR		1
#define SIZE_SHORT		2
#define SIZE_INT		4
#define SIZE_FLOAT		4
#define SIZE_LONG		4
#define SIZE_LONG_LONG	8
#define SIZE_DOUBLE		8
#define SIZE_SIGNED     4
#define SIZE_UNSIGNED   4
#define SIZE_VOID       4


#define NR_REGISTERS	23
#define MIN_REG			4
#define NR_FL_REGISTERS	8
#define MIN_POINTER		2
#define OFFSETREG		24
#define COPYREG1		26
#define COPYREG2		27
#define CONDITIONALREG1	28
#define CONDITIONALREG2	29
#define NR_POINTERS		13
#define POINTER_SIZE	4
#define STACK_POINTER	15
#define GLOBAL_POINTER	14

#endif

