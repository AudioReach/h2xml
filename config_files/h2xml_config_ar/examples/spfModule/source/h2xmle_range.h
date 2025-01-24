/**************************************************************************//**
* @file
* Example for h2xmle_range
*
* If name is not speciefied, the variabel name is used as name
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
*/
struct param_1{
	unsigned short a;	/**<		@h2xmle_range {0..3} 			*/	
	int b;   			/**<		@h2xmle_range {-10..20} 		*/
	float f;			/**<		@h2xmle_range {-1.0..3.5}  @h2xmle_default {0}    */
	double d;			/**<		@h2xmle_range {-10..129.7} @h2xmle_default {10.5} */
	double e;
	float  g;			/**<		@h2xmle_default {-11.13}		*/		
};

/** @h2xmlp_parameter 	{Parameter2,0x22222299} 
*/
struct param_2{
	char a;				/**<		@h2xmle_range {5..20} @h2xmle_default {10}*/
	long b;   			/**<		@h2xmle_range {-5..20} */
};

/** @} */							/* End of Module */						

