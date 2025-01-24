/**************************************************************************//**
* @file
* Example for h2xmle_default
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
    unsigned short a;   /**<        @h2xmle_default {0x1122}        */  
    int b;              /**<        @h2xmle_default {-0x17654321}   */
    float c;            /**<        @h2xmle_default {0.12345678e-15}    */
	float d;            /**<        @h2xmle_default {42}    */
};

/** @h2xmlp_parameter   {Parameter2,0x22222299} 
*/
struct param_2{
    char a;             /**<        @h2xmle_default {0x44}      */
    long b;             /**<        @h2xmle_default {0x17654321}    */
    double c;           /**<        @h2xmle_default {4.987e200} */ 
};

/** @} */                           /* End of Module */                     

