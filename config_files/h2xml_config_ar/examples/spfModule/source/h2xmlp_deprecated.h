/**************************************************************************//**
* @file
* Example for h2xmlp_deprecated
*
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/


/*------------------------------------------------------------------------*//**
 	@h2xmlm_module 					{MODULE_1,0x11111111  }	
 	@h2xmlm_deprecated						  
   	@h2xmlm_stacksize 				{1024}
   	@h2xmlp_deprecated
   	@h2xmlm_dataMaxInputPorts       {4}	
 	@h2xmlm_dataMaxOutputPorts      {6}	
   	@{

----------------------------------------------------------------------------*/
/** 
	@h2xmlp_parameter {Parameter1,0x11111199} 
*/
struct param_1{
	unsigned short a;
	unsigned int b;   	
};

/** 
	@h2xmlp_parameter  {Parameter2,0x22222299} 
	h2xmlp_deprecated  {false}
*/
struct param_2{
	char a;
	long b;   	
};

/** @} */							/* End of Module */						

