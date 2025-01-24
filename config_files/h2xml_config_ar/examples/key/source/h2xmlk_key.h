/**************************************************************************//**
* @file
* Example for h2xmlk_key
*
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

#define APM_PROC_DOMAIN_ID_ADSP                  0x1
#define APM_PROC_DOMAIN_ID_MDSP                  0x2
#define APM_PROC_DOMAIN_ID_SDSP                  0x3
#define APM_PROC_DOMAIN_ID_CDSP                  0x4


enum platforms {
	
	PLATFORM_LA = 1, 	/**< @h2xmle_name {LA} */
	PLATFORM_QNX = 2,    /**< @h2xmle_name {QNX} */
};


/**
  	@h2xml_platforms{PLATFORM_LA, PLATFORM_QNX}
*/


enum AllKeyIds{
	Samplerate = 0xA0000000, /**< @h2xmle_name{srate} */
	Bitwidth = 0xB0000000, 
	Channel = 0xC0000000, 
};

/**
	@h2xmlk_key {Samplerate }
	@h2xmlk_sampleRate
	@h2xmlk_description {Description 10}
*/
enum Key_Samplerate {
	samplerate1 = 0xA0000001, /**< @h2xmle_sampleRate{8000} @h2xmle_description {Description 101}*/
	samplerate2 = 0xA0000002, /**< @h2xmle_sampleRate{16000} @h2xmle_description {Description 102}*/
};

/**
	@h2xmlk_key {Bitwidth}
	@h2xmlk_description {Description 20}
*/
enum Key_Bitwidth {
	bitwidth1 = 0xB0000001, /**< @h2xmle_description {Description 201}*/
	bitwidth2 = 0xB0000002, /**< @h2xmle_description {Description 202}*/
};

/**
	@h2xmlk_key {Channel}
	@h2xmlk_description {Description 30}
*/
enum Key_Channel {
	channel1 = 0xC0000001, /**< @h2xmle_description {Description 301}*/
	channel2 = 0xC0000002, /**< @h2xmle_description {Description 302}*/
};

/**
	@h2xmlk_gkeys
	@h2xmlk_description {Description 40}
*/
enum Graph_Keys {
	gk_samplerate = Samplerate,
	gk_bitwidth = Bitwidth
};


/**
	@h2xmlk_ckeys
	@h2xmlk_description {Description 50}
*/
enum Cal_Keys {
	ck_samplerate = Samplerate,
	ck_bitwidth = Bitwidth,
};


#define ModuleTag1 0xE0000000
#define ModuleTag2 0xF0000000


/**
	@h2xmlk_modTag {"ModuleTag1", ModuleTag1}
	@h2xmlk_description {Description 60}
	@h2xmlk_isVoice 
*/
enum ModuleTag1_Keys {
	tk1_samplerate = Samplerate,
	tk1_bitwidth = Bitwidth
};

/**
	@h2xmlk_modTag {"ModuleTag2", ModuleTag2}
	@h2xmlk_description {Description 70}
*/
enum ModuleTag2_Keys {
	tk2_samplerate = Samplerate,
	tk2_channel = Channel
};



