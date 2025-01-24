/**************************************************************************//**
* @file
* Example for h2xmlp_isVoice
*
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/


/****************************************************************
 Subgraph Config
***************************************************************/
#define APM_PARAM_ID_SUB_GRAPH_CONFIG 0x08001001
/**
	@h2xmlgp_config {"SG_CFG", "CONFIG1", 0x10000000}
	@h2xmlgp_description {Description 1}
	@{ <-- Start of subgraph_cfg -->
*/
/**
	@h2xmlp_property {"SG1", 0x10000001}
*/
typedef struct 
{
	int value1;
} sg1_t;
/** @} <-- End of subgraph_cfg -->*/

/**
	@h2xmlgp_config {"SG_CFG", "CONFIG2", 0x20000000}
	@h2xmlgp_description {Description 2}
	@{ <-- Start of subgraph_cfg -->
*/
/**
	@h2xmlp_property {"SG2", 0x20000001}
    @h2xmlp_isVoice
*/
typedef struct 
{
	int value1;
} sg2_t;
/** @} <-- End of subgraph_cfg -->*/

/**
	@h2xmlgp_config {"SG_CFG", "CONFIG3", 0x30000000}
	@h2xmlgp_description {Description 3}
	@{ <-- Start of subgraph_cfg -->
*/
/**
	@h2xmlp_property {"SG3", 0x20000001}
	@h2xmlp_isVoice {true}
*/
typedef struct 
{
	int value1;
} sg3_t;
/** @} <-- End of subgraph_cfg -->*/

/**
	@h2xmlgp_config {"SG_CFG", "CONFIG4", 0x40000000}
	@h2xmlgp_description {Description 4}
	@{ <-- Start of subgraph_cfg -->
*/
/**
	@h2xmlp_property {"SG4", 0x40000001}
	@h2xmlp_isVoice {false}
*/
typedef struct 
{
	int value1;
} sg4_t;
/** @} <-- End of subgraph_cfg -->*/


