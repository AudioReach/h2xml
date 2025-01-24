/**************************************************************************//**
* @file
* Example for h2xmle_rangeListRange and hybrid range definition
*
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/
#define NUMM 2

/*------------------------------------------------------------------------*//**
 	@h2xmlm_module 					{MODULE_1,0x11111111  }			  
    @h2xmlm_stacksize 				{1024}	
   	@h2xmlm_dataMaxInputPorts       {4}	
 	@h2xmlm_dataMaxOutputPorts      {6}	
   	@{

----------------------------------------------------------------------------*/

/** @h2xmlp_parameter 	{Parameter1,NUMM} 
*/
struct param_1{
	char c;				
	/**<		
		@h2xmle_range			{-3..100}
		@h2xmle_rangeListRange 	{50..60} 
		@h2xmle_rangeListRange 	{70..80} 

		@h2xmle_rangeList 		{a=-1; b=0x20, c=30} 
		@h2xmle_default 		{32} 
	*/
	
	float f;				
	/**<		
		@h2xmle_range			{-3..100}
		@h2xmle_rangeListRange 	{50..60.5} 
		@h2xmle_rangeListRange 	{70.1..80} 

		@h2xmle_rangeList 		{a=-1.5; b=0x20, c=30e0} 
		@h2xmle_default 		{-1.5} 
	*/
	
};



/** @} */							/* End of Module */						

