/**************************************************************************//**
* @file
* * Example for h2xml_processors
*
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

#define APM_PROC_DOMAIN_ID_ADSP                  0x1
#define APM_PROC_DOMAIN_ID_MDSP                  0x2
#define APM_PROC_DOMAIN_ID_SDSP                  0x3
#define APM_PROC_DOMAIN_ID_CDSP                  0x4


enum processors {
	PROC_DOMAIN_ADSP = APM_PROC_DOMAIN_ID_ADSP, 	/**< @h2xmle_name {ADSP} */
	PROC_DOMAIN_MDSP = APM_PROC_DOMAIN_ID_MDSP, 	/**< @h2xmle_name {MDSP} */
	PROC_DOMAIN_SDSP = APM_PROC_DOMAIN_ID_SDSP,  	/**< @h2xmle_name {SDSP} */
	PROC_DOMAIN_CDSP = APM_PROC_DOMAIN_ID_CDSP  	/**< @h2xmle_name {CDSP} */
};



/** 
	@h2xml_processors{PROC_DOMAIN_ADSP, PROC_DOMAIN_MDSP}
*/


/*------------------------------------------------------------------------*//**
 	@h2xmlm_module 					{MODULE_1,0x12345678 }
    @h2xmlm_stacksize 				{1024}	
   	@h2xmlm_dataMaxInputPorts       {4}	
 	@h2xmlm_dataMaxOutputPorts      {6}	
   	@{

----------------------------------------------------------------------------*/

/** @} */							/* End of Module */						


