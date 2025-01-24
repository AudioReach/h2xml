/**************************************************************************//**
* @file
* * Example for h2xmlm_supportedContTypes
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


enum {
	MODULE_ID1=0x11111111
};

/*------------------------------------------------------------------------*//**
    @h2xmlm_module                  {MODULE_1,MODULE_ID1  }         
    @h2xmlm_supportedContTypes      {APM_CONTAINER_CAP_PP, APM_CONTAINER_CAP_MX}
    @h2xmlm_stacksize               {1024}  
    @h2xmlm_dataMaxInputPorts       {4}	
 	@h2xmlm_dataMaxOutputPorts      {6}	
   	@{

----------------------------------------------------------------------------*/

/** @} */                           /* End of Module */                     


/*------------------------------------------------------------------------*//**
    @h2xmlm_module                  {MODULE_2,0x22222222  }
    @h2xmlm_supportedContTypes     {APM_CONTAINER_CAP_CD,APM_CONTAINER_CAP_EP,APM_CONTAINER_CAP_OLC}
    @h2xmlm_stacksize               {1024}  
    @h2xmlm_dataMaxInputPorts       {4}	
 	@h2xmlm_dataMaxOutputPorts      {6}	
   	@{
                                
----------------------------------------------------------------------------*/

/** @} */                           /* End of Module */     
        
    
