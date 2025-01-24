/**************************************************************************//**
* @file
* Example for h2xmlp_subStruct
*
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

/*------------------------------------------------------------------------*//**
 	@h2xmlm_module 					{MODULE_1,0x11111111  }			  
    @h2xmlm_stacksize 				{1024}	
   	@h2xmlm_dataMaxInputPorts       {4}	
 	@h2xmlm_dataMaxOutputPorts      {6}	
   	@{

----------------------------------------------------------------------------*/

/** @h2xmlp_subStruct */
typedef struct {
	unsigned short a; 	/**<@h2xmle_default {0x1122} 		*/
	unsigned int b;   	/**<@h2xmle_default {0x88776655} 	*/
} sStruct1_t;

/** @h2xmlp_subStruct */
struct struct2{
	unsigned short a; 	/**<@h2xmle_default {0x1122} 		*/
	unsigned int b;   	/**<@h2xmle_default {0x88776655} 	*/
} ;

/** 
	@h2xmlp_parameter 		{Parameter2,0x22222299} 
	@h2xmlx_expandStructs	{false}
*/
struct param1{
	char a;				/**<@h2xmle_default {0x55} 		 */
	long b;   			/**<@h2xmle_default {0x11111111} */
	sStruct1_t 	sub1;	
	struct2 	sub2[4];
};

/** @} */							/* End of Module */						

