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

typedef struct {
	unsigned int a; 	/**<@h2xmle_default {0x12345678} 	*/		
	unsigned int b;   	/**<@h2xmle_default {0x88776655} 	*/
} sStruct1_t;

typedef struct {
	sStruct1_t s1; 	 		
	sStruct1_t s2;   	/**<@h2xmle_default {0x88776655} 	*/
} sStruct2_t;


 
/** 
	@h2xmlp_parameter 		{Parameter2,0x22222299} 
	@h2xmlx_expandStructs	{true}
*/
struct param1{
	
	sStruct2_t 	sub1;	
	sStruct2_t 	sub2;	
};

/**
 @h2xml_select{param1::sub1::s1::a}
 @h2xmle_default {0x4444}
*/

/**
 @h2xml_select{param1::sub1::s2::a}
 @h2xmle_default {0x11111}
*/
	
/** @} */							/* End of Module */						



