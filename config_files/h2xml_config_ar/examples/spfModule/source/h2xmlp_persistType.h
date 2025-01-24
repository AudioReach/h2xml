/**************************************************************************//**
* @file
* Example for h2xmlp_persistType
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

/** @h2xmlp_parameter 	{Parameter1,0x11111111} 
	@h2xmlp_persistType {None}	
*/
struct param_1{
	unsigned short a;	/**<		 */	
	int b;   			/**<		 */
	int c;   			/**<	 	 */
};

/** @h2xmlp_parameter 	{Parameter1,0x22222222} 
	@h2xmlp_persistType {Shared}	
*/
struct param_2{
	unsigned short a;	/**<		 */	
	int b;   			/**<		 */
	int c;   			/**<	 	 */
};

/** @h2xmlp_parameter 	{Parameter1,0x33333333} 
	@h2xmlp_persistType {Global_Shared}	
*/
struct param_3{
	unsigned short a;	/**<		 */	
	int b;   			/**<		 */
	int c;   			/**<	 	 */
};

/** @h2xmlp_parameter 	{Parameter1,0x44444444} 
*/
struct param_4{
	unsigned short a;	/**<		 */	
	int b;   			/**<		 */
	int c;   			/**<	 	 */
};


/** @} */							/* End of Module */						

