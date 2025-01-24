/**************************************************************************//**
* @file
* Example for h2xmle_visibility
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

/** @h2xmlp_parameter 	{Parameter1,0x11111199} 
	@h2xmle_visibility	{show}									<--! default for this parameter -->
*/
struct param_1{
	unsigned short a;	/**<									*/	
	int b;   			/**<		@h2xmle_visibility	{hide} 	*/
	int c;   			/**<		@h2xmle_visibility	{show} 	*/
};

/** @h2xmlp_parameter 	{Parameter2,0x22222299} 
	@h2xmle_visibility	{hide}									<--! default for this parameter -->
*/
struct param_2{
	unsigned short a;	/**<									*/	
	int b;   			/**<		@h2xmle_visibility	{hide} 	*/
	int c;   			/**<		@h2xmle_visibility	{show} 	*/
};

/** @h2xmlp_parameter 	{Parameter3,0x33333399} 
																<--! no default specified -->
*/
struct param_3{
	unsigned short a;	/**<									*/	
	int b;   			/**<		@h2xmle_visibility	{hide} 	*/
	int c;   			/**<		@h2xmle_visibility	{show} 	*/
};

/** @} */							/* End of Module */						

