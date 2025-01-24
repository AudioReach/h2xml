/**************************************************************************//**
* @file
* Example for h2xmle_rangeEnum
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

typedef enum  {
  value1=1,
  value2=2,
  value3=3   /** @h2xmle_name {VAL3} */ 
}eRange1;

typedef enum  {
  value11=11,
  value12=12,
  value13=13   
}eRange2;


/** @h2xmlp_parameter 	{Parameter1, 0x12345678} 
*/
struct param_1{
	unsigned short a;	/**<		@h2xmle_rangeEnum {eRange1;eRange2} @h2xmle_default{value12}	*/	
	int b;   			/**<		 	*/

};

/** @} */							/* End of Module */						

