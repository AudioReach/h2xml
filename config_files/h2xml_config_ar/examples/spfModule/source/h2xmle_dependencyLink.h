/**************************************************************************//**
* @file
* Example for h2xmle_dependencyLink 
*
*
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

#define DEFAULT_VALUE 10

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
    short target;       
    short interference[5];  
   /**<
      @h2xmle_description {Description}
      @h2xmle_dependencyLink {target=1, "@h2xmle_defaultList{1,2,3,4,5}"}
      @h2xmle_dependencyLink {target=2, "@h2xmle_defaultList{6,7,8,9,10}"}
   */

    long b;             
    /**<        
      @h2xmle_dependencyLink {target=1, "@h2xmle_range {-5..20}"}
      @h2xmle_dependencyLink {target=2, "@h2xmle_range {5..10}"}
      @h2xmle_dependencyLink {target=3, "@h2xmle_default {15}"}
      @h2xmle_dependencyLink {target=4, "@h2xmle_default {5}"}
    */
	
	 float f;             
    /**<        
      @h2xmle_dependencyLink {target=1, "@h2xmle_range {-5..20e-10} @h2xmle_default {-3.75}"}
      @h2xmle_dependencyLink {target=2, "@h2xmle_range {5..10}"}
      @h2xmle_dependencyLink {target=3, "@h2xmle_default {-15.75}"}
      @h2xmle_dependencyLink {target=4, "@h2xmle_default {5.0}"}
    */
};

/** @} */                           /* End of Module */                     

