/**************************************************************************//**
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

#ifndef __VERSION_H__
#define __VERSION_H__

#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)

// Define all version information
#define VERSION_MAJOR				1
#define VERSION_MINOR				0
#define VERSION_REVISION			0


#define VER_COMPANY_NAME			"Qualcomm Innovation Center, Inc."
#define VER_FILE_DESCRIPTION_STR	"H2XML build tool"
#define VER_COPYRIGHT_STR			"Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved."
#define VER_PRODUCTNAME_STR			"h2xml"

// derived defines
#define VERSION_BUILD				$WCREV$
#define VERSION_BUILD_RANGE			"$WCREV$$WCMIXED?_MR:$$WCMODS?_LM:$"
#define VERSION_BUILD_DATE			"$WCDATE$"
#define VER_FILE_VERSION			VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION, VERSION_BUILD
#define VER_FILE_VERSION_STR		STRINGIZE(VERSION_MAJOR)     \
	"." STRINGIZE(VERSION_MINOR) \
	"." STRINGIZE(VERSION_REVISION) \
	"." VERSION_BUILD_RANGE \

#define VER_PRODUCT_VERSION			VER_FILE_VERSION
#define VER_PRODUCT_VERSION_STR		VER_FILE_VERSION_STR
#define VER_ORIGINAL_FILENAME_STR   VER_PRODUCTNAME_STR ".exe"
#define VER_INTERNAL_NAME_STR		VER_ORIGINAL_FILENAME_STR

#define VERSION_INFO_COMPL			VER_PRODUCTNAME_STR" - "VER_FILE_VERSION_STR

#endif

  
