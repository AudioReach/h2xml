/**************************************************************************//**
* @file
* Example for h2xmlm_dataInputPorts
*
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/
#define INFINITY -1

/*------------------------------------------------------------------------*//**
    @h2xmlm_module                  {MODULE_1,0x11111111  }         
    @h2xmlm_dataInputPorts          {In0=2;In1=4}   
    @h2xmlm_dataMaxInputPorts       {INFINITY}  
    @h2xmlm_dataMaxOutputPorts      {0}                                 
    @h2xmlm_stacksize               {1024}  
    @{

----------------------------------------------------------------------------*/

/** @} */                           /* End of Module */                     


/*------------------------------------------------------------------------*//**
    @h2xmlm_module                  {MODULE_2,0x22222222  }
    @h2xmlm_dataInputPorts          {In0=0;In1=4;In2=16}    
    @h2xmlm_dataMaxInputPorts       {4} 
    @h2xmlm_dataMaxOutputPorts      {0}                             
    @h2xmlm_stacksize               {1024}  
    @{
                                
----------------------------------------------------------------------------*/

/** @} */                           /* End of Module */                     
