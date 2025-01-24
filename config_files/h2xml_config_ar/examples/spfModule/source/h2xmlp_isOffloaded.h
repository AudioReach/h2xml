/**************************************************************************//**
* @file
* Example for h2xmlp_isOffloaded
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
	@h2xmlp_isOffloaded	
*/
struct param_1{
	unsigned short a;	/**<		 */	
	int b;   			/**<		 */
	int c;   			/**<	 	 */
};

/** @h2xmlp_parameter 	{Parameter2,0x22222299} 
	@h2xmlp_isOffloaded{true}	
*/
struct param_2{
	unsigned short a;	/**<		 */	
	int b;   			/**<		 */
	int c;   			/**<	 	 */
};

/** @h2xmlp_parameter 	{Parameter3,0x33333399} 
	@h2xmlp_isOffloaded{false}	
*/
struct param_3{
	unsigned short a;	/**<		 */	
	int b;   			/**<		 */
	int c;   			/**<	 	 */
};


/** @} */							/* End of Module */						

