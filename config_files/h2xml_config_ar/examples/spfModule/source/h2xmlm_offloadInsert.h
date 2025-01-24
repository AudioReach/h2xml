/**************************************************************************//**
* @file
* Example for h2xmlm_offloadInsert
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

/** @} */                           /* End of Module */                     

/*------------------------------------------------------------------------*//**
    @h2xmlm_module                  {MODULE_2,0x22222222  }
    @h2xmlm_offloadInsert           {WR_CLIENT}    
    @h2xmlm_stacksize               {1024}  
    @h2xmlm_dataMaxInputPorts       {4} 
    @h2xmlm_dataMaxOutputPorts      {6} 
    @{
                                
----------------------------------------------------------------------------*/

/** @} */                           /* End of Module */     
/*------------------------------------------------------------------------*//**
    @h2xmlm_module                  {MODULE_3,0x33333333  }
    @h2xmlm_offloadInsert           {WR_EP} 
    @h2xmlm_stacksize               {1024}  
    @h2xmlm_dataMaxInputPorts       {4} 
    @h2xmlm_dataMaxOutputPorts      {6} 
    @{
                                
----------------------------------------------------------------------------*/

/** @} */                           /* End of Module */     
        
/*------------------------------------------------------------------------*//**
    @h2xmlm_module                  {MODULE_4,0x44444444  }
    @h2xmlm_offloadInsert           {RD_CLIENT}  
    @h2xmlm_stacksize               {1024}  
    @h2xmlm_dataMaxInputPorts       {4} 
    @h2xmlm_dataMaxOutputPorts      {6} 
    @{
                                
----------------------------------------------------------------------------*/

/** @} */                           /* End of Module */                     


/*------------------------------------------------------------------------*//**
    @h2xmlm_module                  {MODULE_5,0x55555555  }
    @h2xmlm_offloadInsert           {RD_EP}  
    @h2xmlm_stacksize               {1024}  
    @h2xmlm_dataMaxInputPorts       {4} 
    @h2xmlm_dataMaxOutputPorts      {6} 
    @{
                                
----------------------------------------------------------------------------*/

/** @} */                           /* End of Module */            