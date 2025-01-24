/**************************************************************************//**
* @file
* Example for h2xmle_expandTypeDefs
*
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

typedef unsigned int uint32;
typedef uint32  newUint32_t;


typedef struct {
	newUint32_t s1a;
	unsigned char s1b;					
}sStruct1_t;



/*------------------------------------------------------------------------*//**
 	@h2xmlm_module 					{MODULE_1,0x11111111  }			  
    @h2xmlm_stacksize 				{1024}	
   	@h2xmlm_dataMaxInputPorts       {4}	
 	@h2xmlm_dataMaxOutputPorts      {6}	
   	@{

----------------------------------------------------------------------------*/

/** 
	@h2xmlp_parameter 			{Parameter1,0x00000099} 
*/
struct param_0{
	short 				a;				
	unsigned short 		b;			
	uint32				c;
	newUint32_t			d;
	sStruct1_t 			e;									
};

/** 
	@h2xmlp_parameter 			{Parameter1,0x11111199} 
	@h2xmlx_expandTypeDefs		{false}
*/
struct param_1{
	short 				a;				
	unsigned short 		b;			
	uint32				c;
	newUint32_t			d;
	sStruct1_t 			e;									
};

/** 
	@h2xmlp_parameter 			{Parameter2,0x22222299} 
	@h2xmlx_expandTypeDefs		{true}
*/
struct param_2{
	short 				a;				
	unsigned short 		b;			
	uint32				c;
	newUint32_t			d;
	sStruct1_t 			e;
};

/** @} */							/* End of Module */						

