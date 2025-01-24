/**************************************************************************//**
* @file
* Example for h2xmle_defaultList
*
*
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

#define DEFAULT_VALUE 10

/*------------------------------------------------------------------------*//**
 	@h2xmlm_module 					{MODULE_1,0x11111111  }			  
   	@h2xmlm_stacksize 				{1024}	
   	@h2xmlm_dataMaxInputPorts       {4}	
 	@h2xmlm_dataMaxOutputPorts      {6}	
   	@{

----------------------------------------------------------------------------*/
/** @h2xmlp_parameter 	{Parameter1,0x11111199} 
	@h2xmlx_expandArray	{false}
*/
struct param_1{
	short x[6];
	/**<		
		@h2xmle_range 				{0x8000..0x7fff}
		@h2xmle_defaultList 		{1,2,3,4,5,6}
	**/	
	float y[6];
	/**<		
		@h2xmle_range 				{0 .. 10}
		@h2xmle_defaultList 		{1.0,2,3,4,5,0.7532156}
	**/	
	
	double z[6];
	/**<		
		@h2xmle_range 				{0 .. 1.7e15}
		@h2xmle_defaultList 		{1.0,2,3,4,5,30e+5}
	**/	
	
};

/** @h2xmlp_parameter 	{Parameter2,0x22222299} 
	@h2xmlx_expandArray	{true}
*/
struct param_2{
	short x[6];
	/**<		
		@h2xmle_range 				{0x8000..0x7fff}
		@h2xmle_defaultList 		{1,2,3,4,5,DEFAULT_VALUE}
	**/	
		float y[6];
	/**<		
		@h2xmle_range 				{0 .. 10}
		@h2xmle_defaultList 		{1.0,2,3,4,5,0.7532156}
	**/	
	double z[6];
	/**<		
		@h2xmle_range 				{0 .. 1.7e15}
		@h2xmle_defaultList 		{1.0,2,3,4,5,30e+5}
	**/	
};
	
/** @} */							/* End of Module */						

