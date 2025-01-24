/**************************************************************************//**
* @file
* Example for h2xmle_ignore and h2xmle_include
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
	K_BitWidth = 0xB0000000, 
};

/**
	@h2xmlk_key {Samplerate }
	@h2xmlk_sampleRate
	@h2xmlk_description {Description 10}
*/
enum Key_Samplerate {
    Kv_SampleRate_8000                      =0xA0001D01,/**< @h2xmle_sampleRate{8000 }  @h2xmle_description {Kv_SampleRate_8000  }*/
    Kv_SampleRate_11025                     =0xA0001D02,/**< @h2xmle_sampleRate{11025}  @h2xmle_description {Kv_SampleRate_11025 }*/
    Kv_SampleRate_12000                     =0xA0001D03,/**< @h2xmle_sampleRate{12000}  @h2xmle_description {Kv_SampleRate_12000 }*/
    Kv_SampleRate_16000                     =0xA0001D04,/**< @h2xmle_sampleRate{16000}  @h2xmle_description {Kv_SampleRate_16000 }*/
    Kv_SampleRate_22050                     =0xA0001D05,/**< @h2xmle_sampleRate{22050}  @h2xmle_description {Kv_SampleRate_22050 }*/
    Kv_SampleRate_24000                     =0xA0001D06,/**< @h2xmle_sampleRate{24000}  @h2xmle_description {Kv_SampleRate_24000 }*/
    Kv_SampleRate_32000                     =0xA0001D07,/**< @h2xmle_sampleRate{32000}  @h2xmle_description {Kv_SampleRate_32000 }*/
    Kv_SampleRate_44100                     =0xA0001D08,/**< @h2xmle_sampleRate{44100}  @h2xmle_description {Kv_SampleRate_44100 }*/
    Kv_SampleRate_48000                     =0xA0001D09,/**< @h2xmle_sampleRate{48000}  @h2xmle_description {Kv_SampleRate_48000 }*/
    Kv_SampleRate_88200                     =0xA0001D0A,/**< @h2xmle_sampleRate{88200}  @h2xmle_description {Kv_SampleRate_88200 }*/
    Kv_SampleRate_96000                     =0xA0001D0B,/**< @h2xmle_sampleRate{96000}  @h2xmle_description {Kv_SampleRate_96000 }*/
    Kv_SampleRate_176400                    =0xA0001D0C,/**< @h2xmle_sampleRate{176400} @h2xmle_description {Kv_SampleRate_176400} */
    Kv_SampleRate_192000                    =0xA0001D0D,/**< @h2xmle_sampleRate{192000} @h2xmle_description {Kv_SampleRate_192000} */
    Kv_SampleRate_352800                    =0xA0001D0E,/**< @h2xmle_sampleRate{352800} @h2xmle_description {Kv_SampleRate_352800} */
    Kv_SampleRate_384000                    =0xA0001D0F,/**< @h2xmle_sampleRate{384000} @h2xmle_description {Kv_SampleRate_384000} */
};

/**
@h2xmlk_key {K_BitWidth}
@h2xmlk_description {Signal Bit Width.}
*/
typedef enum {
    Kv_BitWidth_16                      =0xA0001E01,/**< @h2xmle_description{Kv_BitWidth_16} */
    Kv_BitWidth_24                      =0xA0001E02,/**< @h2xmle_description{Kv_BitWidth_24} */
    Kv_BitWidth_32                      =0xA0001E03,/**< @h2xmle_description{Kv_BitWidth_32} */
} KV_BITWIDTH;


#define ModuleTag1 0xE0000000
#define ModuleTag2 0xF0000000


/**
	@h2xmlk_modTag {"ModuleTag1", ModuleTag1}
	@h2xmlk_description {Description 60}
*/
enum ModuleTag1_Keys {
	tk1_samplerate = Samplerate, /**< @h2xmle_ignore{Kv_SampleRate_11025, Kv_SampleRate_44100} */
	tk1_bitwidth = K_BitWidth
};

/**
	@h2xmlk_modTag {"ModuleTag2", ModuleTag2}
	@h2xmlk_description {Description 70}
*/
enum ModuleTag2_Keys {
	tk2_samplerate = Samplerate, /**< @h2xmle_include{Kv_SampleRate_11025, Kv_SampleRate_44100} */
	tk2_bitwidth = K_BitWidth
};



