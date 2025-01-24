/**************************************************************************//**
* @file
* Example for h2xmlp_readonly
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
	@h2xmlp_readonly	
*/
struct param_1{
	unsigned short a;	/**<		 */	
	int b;   			/**<		 */
	int c;   			/**<	 	 */
};


/** @} */							/* End of Module */						

