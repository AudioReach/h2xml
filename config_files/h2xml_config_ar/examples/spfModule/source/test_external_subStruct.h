/**************************************************************************//**
* @file
* Example for globally defined sub-Structures that are used in several modules 
*
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

/** @h2xmlp_subStruct */		//<!-- structure defined outside of module   -->
typedef struct {
	int exa;		/**< @h2xmle_default		{0x1}*/
	int exb;		/**< @h2xmle_default		{0x2}*/
	int exc;		/**< @h2xmle_default		{0x3}*/
} extStruct1;

/** @h2xmlp_subStruct */
struct extStruct2{
	int exx;		/**< @h2xmle_default		{0x4}*/
	char exy;		/**< @h2xmle_default		{0x5}*/
	short exz;		/**< @h2xmle_default		{0x6}*/
};


/*------------------------------------------------------------------------*//**
 	@h2xmlm_module 					{MODULE_1,0x11111111  }
   	@h2xmlm_DataInputPorts 				{In1=2;In2=4}							
   	@h2xmlm_DataOutputPorts 				{Out1=3;Out2=5}					  
    @h2xmlm_stacksize 				{1024}	
   	@h2xmlm_dataMaxInputPorts       {4}	
 	@h2xmlm_dataMaxOutputPorts      {6}	
   	@{

----------------------------------------------------------------------------*/

/**
	@h2xml_Select					{extStruct1}					<!-- within the module select the parameter -->
    @h2xmlm_InsertParameter											<!-- add the structure to the module  -->
*/

/**
	@h2xml_Select					{extStruct2}
    @h2xmlm_InsertStructure	
	@h2xmlp_description				{Description External Parameter2 Module 1}
	@h2xml_Select					{extStruct2::exx}	
	@h2xmle_default					{0x111}
	@h2xml_Select					{extStruct2::exy}	
	@h2xmle_default					{0x32}
*/

/** 
	@h2xmlp_parameter 			{Parameter1,0x11111199} 
	@h2xmlx_expandStructs		{false}
*/
struct param_1{
	short 	a1;		/**< @h2xmle_default		{0x33} */
	int 	b1;   	/**< @h2xmle_default		{0x44} */
	extStruct1	s1;
	extStruct2	s2;
};

				
/** @} */								/* End of Module */