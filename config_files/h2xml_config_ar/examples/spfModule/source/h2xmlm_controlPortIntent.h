/**************************************************************************//**
* @file
* * Example for h2xmlm_ctrlDynamicPortIntent
*
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

/*------------------------------------------------------------------------*//**
    @h2xmlm_module                  {MODULE_1,0x11111111  } 
    @h2xmlm_ctrlStaticPort          {cntrlPort0 = 0, 
                                     intent1=1,
                                     intent2=2}
    @h2xmlm_ctrlStaticPort          {cntrlPort1 = 1, 
                                     intent3=3}
            
    @h2xmlm_ctrlDynamicPortIntent   { intent1=2,maxPorts=5 }
    @h2xmlm_ctrlDynamicPortIntent   { intent2=7,maxPorts=3 }


    @h2xmlm_stacksize               {1024}  
    @h2xmlm_dataMaxInputPorts       {4} 
    @h2xmlm_dataMaxOutputPorts      {6} 
    @{

----------------------------------------------------------------------------*/

/** @} */                           /* End of Module */                     


/*------------------------------------------------------------------------*//**
    @h2xmlm_module                  {MODULE_2,0x22222222  }
    @h2xmlm_ctrlDynamicPortIntent  { intent1=2,maxPorts=2 }
    @h2xmlm_ctrlDynamicPortIntent  { intent2=7,maxPorts=3 }
    @h2xmlm_ctrlDynamicPortIntent  { intent3=2,maxPorts=4 }
    @h2xmlm_ctrlDynamicPortIntent  { intent4=7,maxPorts=5 }
    @h2xmlm_stacksize               {1024}  
    @h2xmlm_dataMaxInputPorts       {4} 
    @h2xmlm_dataMaxOutputPorts      {6} 
    @{
                                
----------------------------------------------------------------------------*/

/** @} */                           /* End of Module */     
        
/*------------------------------------------------------------------------*//**
    @h2xmlm_module                  {MODULE_3,0x33333333  }
    @h2xmlm_stacksize               {1024}  
    @h2xmlm_dataMaxInputPorts       {4} 
    @h2xmlm_dataMaxOutputPorts      {6} 
    @{
                                
----------------------------------------------------------------------------*/

/** @} */                           /* End of Module */                     
