/**************************************************************************//**
* @file
* Example for driver modules
*
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

#define APM_CONTAINER_CAP_ID_PP                    0x1
#define APM_CONTAINER_CAP_ID_CD                    0x2
#define APM_CONTAINER_CAP_ID_EP                    0x3
#define APM_CONTAINER_CAP_ID_MX                    0x4
#define APM_CONTAINER_CAP_ID_OLC                   0x5

enum containerCap {
    APM_CONTAINER_CAP_PP     = APM_CONTAINER_CAP_ID_PP, /**< @h2xmle_name {PP} */
    APM_CONTAINER_CAP_CD     = APM_CONTAINER_CAP_ID_CD, /**< @h2xmle_name {CD} */  
    APM_CONTAINER_CAP_EP     = APM_CONTAINER_CAP_ID_EP,     
    APM_CONTAINER_CAP_MX     = APM_CONTAINER_CAP_ID_MX,      
    APM_CONTAINER_CAP_OLC    = APM_CONTAINER_CAP_ID_OLC    
};

/*------------------------------------------------------------------------*//**
 	@h2xmlm_module 					{MODULE_1,0x11111111}		
 	@h2xmlm_builtin 				{true}
   	@{

----------------------------------------------------------------------------*/

/** @h2xmlp_parameter 	{Parameter1,0x11111111}
	@h2xmlp_persistType {Shared} 
*/
struct param_1{
	unsigned short a;	/**<		 */	
	int b;   			/**<		 */
	int c;   			/**<	 	 */
	long d;   			/**<	 	 */
};

/** @h2xmlp_parameter 	{Parameter1,0x22222222} 
	@h2xmlp_persistType {None}
    @h2xmlp_maxSize 	{0x22} 

*/
struct param_2{
	unsigned short a;	/**<		 */	
	int b;   			/**<		 */
	int c;   			/**<	 	 */
};


/** @} */							/* End of Module */						

