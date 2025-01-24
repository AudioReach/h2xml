/**************************************************************************//**
* @file
* Example for h2xmlx_xmlTabSize
*
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

/**
	@h2xml_globalAnnotations		{true}
 	@h2xmlx_xmlTabSize				{1}	
	@h2xmlx_xmlLineLength			{10}	
 	@h2xmlx_xmlAttributeLength		{80}	
 	@h2xml_globalAnnotations		{false}
*/


/*------------------------------------------------------------------------*//**
 	@h2xmlm_module 					{MODULE_1,0x11111111  }
 	@h2xmlm_DataInputPorts 				{In1=2;In2=4}							
   	@h2xmlm_DataOutputPorts 				{Out1=3;Out2=5}				
 	@h2xmlx_xmlTabSize				{2}	
    @h2xmlm_stacksize 				{1024}	
   	@h2xmlm_dataMaxInputPorts       {4}	
 	@h2xmlm_dataMaxOutputPorts      {6}	
   	@{

----------------------------------------------------------------------------*/

/** @h2xmlp_parameter 			{Parameter1,0x11111111} */
struct param_1{		
	char a;					
	/**< @h2xmle_description	{
	This is a very long description blahblahblah
	blahblahblah blahblahblah blahblahblah
	blahblahblah blahblahblah blahblahblah
	blahblahblah blahblahblah blahblahblah
	blahblahblah blahblahblah blahblahblah	
	blahblahblah blahblahblah blahblahblah	
	blahblahblah blahblahblah blahblahblah	
	blahblahblah blahblahblah blahblahblah	
	} 
	@h2xmle_readOnly			{true}
	*/ 				
};

/** @h2xmlp_parameter 			{Parameter2,0x22222222} 
 	@h2xmlx_xmlTabSize{4}	
*/
struct param_2{
	int a[100];						/**< @h2xmlx_expandArray{true} */
	short b[20];   					/**< @h2xmlx_expandArray{true} */
	char c[30][2];					/**< @h2xmlx_expandArray{true} */ 					
	long d;					
};

/** @} */							/* End of Module */			

/*------------------------------------------------------------------------*//**
 	@h2xmlm_module 					{MODULE_2,0x22222222  }
 	@h2xmlm_DataInputPorts 				{In1=2;In2=4}							
   	@h2xmlm_DataOutputPorts 				{Out1=3;Out2=5}				
 	@h2xmlx_xmlTabSize				{6}	
    @h2xmlm_stacksize 				{1024}	
   	@h2xmlm_dataMaxInputPorts       {4}	
 	@h2xmlm_dataMaxOutputPorts      {6}	
   	@{

----------------------------------------------------------------------------*/

/** @h2xmlp_parameter 				{Parameter1,0x33333333} 
 	@h2xmlx_xmlTabSize{2}	
*/
struct param_3{		
	char a;					
	/**< @h2xmle_description	{
	This is a very long description blahblahblah
	blahblahblah blahblahblah blahblahblah
	blahblahblah blahblahblah blahblahblah
	blahblahblah blahblahblah blahblahblah
	blahblahblah blahblahblah blahblahblah	
	blahblahblah blahblahblah blahblahblah	
	blahblahblah blahblahblah blahblahblah	
	blahblahblah blahblahblah blahblahblah	
	} 
	@h2xmle_readOnly			{true}
	*/ 				
};

/** @h2xmlp_parameter 			{Parameter2,0x44444444} */
struct param_4{
	int a[100];						/**< @h2xmlx_expandArray{true} */
	short b[20];   					/**< @h2xmlx_expandArray{true} */
	char c[30][2];					/**< @h2xmlx_expandArray{true} */ 					
	long d;					
};

/** @} */							/* End of Module */							