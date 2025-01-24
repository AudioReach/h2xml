/**************************************************************************//**
* @file
* Example for h2xmle_alignment
*
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/


/*------------------------------------------------------------------------*//**
    @h2xmlm_module                  {MODULE_1,0x11111111  }           
    @h2xmlm_stacksize               {1024}  
    @h2xmlm_dataMaxInputPorts       {4} 
    @h2xmlm_dataMaxOutputPorts      {6} 
    @{

----------------------------------------------------------------------------*/

/** @h2xmlp_parameter   {Parameter1,0x11111199} 
*/
struct param_1{
    unsigned short a;   /**<            */  
    unsigned short b;   /**< @h2xmle_alignment{1} */ 
    unsigned short c;   /**< @h2xmle_alignment{2} */   
};


/** @} */                           /* End of Module */                     

