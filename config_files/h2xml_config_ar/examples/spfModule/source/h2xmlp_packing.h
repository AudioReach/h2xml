/**************************************************************************//**
* @file
* Example for generated attribute h2xmlp_packing
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
	char a;					
	short b;		
	long c;   
	union {
		char x1;
		struct {
			char x2;
			long x3;
		}s1;
	}u1;
};

/** @h2xmlp_parameter 	{Parameter2,0x22222299} 
*/
struct param_2{
	char a;					
	short b;		
	long c;   
	union {
		char x1;
		struct {
			char x2;
			long x3;
		}s1;
	}u1;
} __attribute__ ((packed));

/** @h2xmlp_parameter 	{Parameter3,0x33333399} 
*/
struct param_3{
	char a;					
	short b;		
};

/** @h2xmlp_parameter 	{Parameter4,0x44444499} 
*/
struct param_4{
	char a;					
	char b;		
};

/** @h2xmlp_parameter 	{Parameter5,0x55555599}
	@h2xmlp_bitAlign     {32} 
*/
struct param_5{
	char a;					
	char b;		
};

/** @} */							/* End of Module */						

