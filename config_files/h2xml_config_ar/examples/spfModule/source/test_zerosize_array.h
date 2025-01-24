/**************************************************************************//**
* @file
* * Example for zero size arrays
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
	int s1;
	int s2[0];
}sSub1_t;

/** @h2xmlp_parameter 			{Parameter1,0x11111111} 
	@h2xmlx_expandArrayOffset	{0}	
	@h2xmlx_expandArray			{true}
*/
struct param_1{
	int a;						/**< @h2xmle_default		{0x11111111}*/
	int b;						/**< @h2xmle_default		{0x22222222}*/
	int c[3];					/**< @h2xmle_default		{0x33333333}*/
	int d[0];					/**< @h2xmle_default		{0x44444444}*/
	int e;						/**< @h2xmle_default		{0x55555555}*/
};

/** @h2xmlp_parameter 			{Parameter2,0x22222222} 
	@h2xmlx_expandArrayOffset	{0}	
	@h2xmlx_expandArray			{false}
*/
struct param_2{
	int a;						/**< @h2xmle_default		{0x11111111}*/
	int b;						/**< @h2xmle_default		{0x22222222}*/
	int c[3];					/**< @h2xmle_default		{0x33333333}*/
	sSub1_t d;						/**< @h2xmle_default		{0x44444444}*/
	int e;						/**< @h2xmle_default		{0x55555555}*/
};

/** @h2xmlp_parameter 			{Parameter3,0x33333333} 
	@h2xmlx_expandArrayOffset	{0}	
	@h2xmlx_expandArray			{false}
*/
struct param_3{
	int a;						/**< @h2xmle_default		{0x11111111}*/
	int b;						/**< @h2xmle_default		{0x22222222}*/
	int c[3];					/**< @h2xmle_default		{0x33333333}*/
	int d[5];					/**< @h2xmle_default		{0x44444444}*/
	int e;						/**< @h2xmle_default		{0x55555555}*/
};


/** @} */							/* End of Module */			
				