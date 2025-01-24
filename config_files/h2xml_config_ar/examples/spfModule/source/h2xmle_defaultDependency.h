/**************************************************************************//**
* @file
* Example for h2xmle_defaultDependency
*
*
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/


/*------------------------------------------------------------------------*//**
 	@h2xmlm_module 					{MODULE_1,0x11111111  }			  
    @h2xmlm_stacksize 				{1024}	
   	@h2xmlm_dataMaxInputPorts       {4}	
 	@h2xmlm_dataMaxOutputPorts      {6}	
   	@{

----------------------------------------------------------------------------*/

/** @h2xmlp_parameter 	{Parameter1,0x11111199} 
*/
struct param_1{
	char a;				
	/**<		
		@h2xmle_range 				{0x80..0x7f} 
		@h2xmle_default 			{10}
		@h2xmle_defaultDependency 	{direction="TX", device=Handset, AudioVoice=Audio, Samplerate="8000", default="2"}
		@h2xmle_defaultDependency 	{direction="RX", device="Handset", AudioVoice="Audio", Samplerate="16000", default="3"}
		@h2xmle_defaultDependency 	{direction="TX", device="Handset", AudioVoice="Audio", Samplerate="48000", default="4"}
		
	*/
	long b;   			
	/**<		
		@h2xmle_range {-5..20} 
	*/
	char c;				
	/**<		
		@h2xmle_range 				{0x80..0x7f} 
		@h2xmle_default 			{10}
		@h2xmle_defaultDependency 	{direction="TX", device=Handset, AudioVoice=Audio, default="2"}
		@h2xmle_defaultDependency 	{direction="RX", device="Handset", Samplerate="16000", default="3"}
		@h2xmle_defaultDependency 	{direction="TX", device="Handset", AudioVoice="Audio", Samplerate="48000", default="40"}
		@h2xmle_defaultDependency 	{direction="RX", device="Handset", AudioVoice="Audio", Samplerate="16000", default="3"}
		@h2xmle_defaultDependency 	{direction="RX", device="Handset", AudioVoice="Audio", Samplerate="24000", default="3"}

		
	*/
	
	short d;				
	/**<		
		@h2xmle_range 				{0x8000..0x7f00} 
		@h2xmle_default 			{10}
		@h2xmle_defaultDependency 	{direction="TX", device=Handset, AudioVoice=Audio, default="2"}
		@h2xmle_defaultDependency 	{direction="RX", device="Handset", Samplerate="16000", default="3"}
		@h2xmle_defaultDependency 	{direction="TX", device="Handset", AudioVoice="Audio", Samplerate="48000", default="0xfffe"}
		@h2xmle_defaultDependency 	{direction="RX", device="Handset", AudioVoice="Audio", Samplerate="16000", default="-1549"}
		@h2xmle_defaultDependency 	{direction="RX", device="Handset", AudioVoice="Audio", Samplerate="24000", default="3"}

		
	*/
	
	float f;				
	/**<		
		@h2xmle_range 				{-10..20.0} 
		@h2xmle_default 			{10}
		@h2xmle_defaultDependency 	{direction="TX", device=Handset, AudioVoice=Audio, default="2"}
		@h2xmle_defaultDependency 	{direction="RX", device="Handset", Samplerate="16000", default="3.5"}
		@h2xmle_defaultDependency 	{direction="TX", device="Handset", AudioVoice="Audio", Samplerate="48000", default="-2"}
		@h2xmle_defaultDependency 	{direction="RX", device="Handset", AudioVoice="Audio", Samplerate="16000", default="0x4"}
		@h2xmle_defaultDependency 	{direction="RX", device="Handset", AudioVoice="Audio", Samplerate="24000", default="0.756"}

	*/
};

/** @h2xmlp_parameter 	{Parameter2,0x22222299} 
*/
struct param_2{
	char a;				
	/**<		
		@h2xmle_range 				{0x80..0x7f} 
		@h2xmle_default 			{10}
		@h2xmle_defaultDependency 	{direction="TX", device=Handset, AudioVoice=Audio, Samplerate="8000", default="2"}
		@h2xmle_defaultDependency 	{direction="RX", device="Handset", AudioVoice="Audio", Samplerate="16000", default="3"}
		@h2xmle_defaultDependency 	{direction="TX", device="Handset", AudioVoice="Audio", Samplerate="48000", default="4"}	
		@h2xmle_defaultDependency 	{direction="TX", device="Handset", AudioVoice="Audio", VoiceBandWidth="WB", Samplerate="48000",  default="4"}	*/

	long b;   			
	/**<		
		@h2xmle_range {-5..20} 
	*/
};

/** @} */							/* End of Module */						

