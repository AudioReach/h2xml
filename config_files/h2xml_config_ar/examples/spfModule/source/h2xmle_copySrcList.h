/**************************************************************************//**
* @file
* Example for h2xmle_copySrcList
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
    @h2xmlp_copySrc     {0x12345678}
*/
struct param_1{
    unsigned short a;      /**<            */  
    unsigned short b[3];   /**< @h2xmle_copySrcList{element1, element2, element3} */ 
};


/** @} */                           /* End of Module */                     

