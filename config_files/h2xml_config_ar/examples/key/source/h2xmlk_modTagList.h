/**************************************************************************//**
* example for h2xmlk_modTagList
*
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

typedef enum {
    PLATFORM_WP                                 = 1,     /**< @h2xmle_name {WP} */
} WP_PLATFORM;

/**
	@h2xmlk_modTagList
*/

typedef enum {
    RxWrtieShmemEP           =0xB0000000,  /**< @h2xmle_description {RxWrtieShmemEP        }    */
    PcmDecodeConvOutFormat   =0xB0000100,  /**< @h2xmle_description {PcmDecodeConvOutFormat}    */
    StreamRxSourceMfc        =0xB0000200,  /**< @h2xmle_description {StreamRxSourceMfc     }    */
    StreamRxSinkMfc          =0xB0000300,  /**< @h2xmle_description {StreamRxSinkMfc       }    */
    StreamPosition           =0xB0000400,  /**< @h2xmle_description {StreamPosition        }    */
    DeviceRxSourceMfc        =0xB0000500,  /**< @h2xmle_description {DeviceRxSourceMfc     }    */
    DeviceRxSinkMfc          =0xB0000600,  /**< @h2xmle_description {DeviceRxSinkMfc       }    */
    StreamRxVolume           =0xB0000700,  /**< @h2xmle_description {StreamRxVolume        }    */
    StreamRxVolumeRamp       =0xB0000800,  /**< @h2xmle_description {StreamRxVolumeRamp    }    */
    StreamRxMute             =0xB0000900,  /**< @h2xmle_description {StreamRxMute          }    */
    StreamRxMuteRamp         =0xB0000A00,  /**< @h2xmle_description {StreamRxMuteRamp      }    */
    StreamRxPeakmeter        =0xB0000B00,  /**< @h2xmle_description {StreamRxPeakmeter     }    */
    DeviceRxPeakmeter        =0xB0000C00,  /**< @h2xmle_description {DeviceRxPeakmeter     }    */
    DeviceRxVolume           =0xB0000D00,  /**< @h2xmle_description {DeviceRxVolume        }    */
    DeviceRxVolumeRamp       =0xB0000E00,  /**< @h2xmle_description {DeviceRxVolumeRamp    }    */
    DeviceRxMute             =0xB0000F00,  /**< @h2xmle_description {DeviceRxMute          }    */
    DeviceRxMuteRamp         =0xB0001000,  /**< @h2xmle_description {DeviceRxMuteRamp      }    */
    EpRxHw                   =0xB0001100,  /**< @h2xmle_description {EpRxHw                }    */
    EpTxSpkrProtHw           =0xB0001200,  /**< @h2xmle_description {EpTxSpkrProtHw        }    */
    SpeakerProtectionRx      =0xB0001300,  /**< @h2xmle_description {SpeakerProtectionRx   }    */
    SpeakerProtectionTx      =0xB0001400,  /**< @h2xmle_description {SpeakerProtectionTx   }    */
    StreamRxEos              =0xB0001500,  /**< @h2xmle_description {StreamRxEos           }    */
    RegUnderflowEvt          =0xB0001600,  /**< @h2xmle_description {RegUnderflowEvt       }    */
    RegWaterMarkEvt          =0xB0001700,  /**< @h2xmle_description {RegWaterMarkEvt       }    */
} WX_PLAYBACK_GRAPH_TAG_KEY;        
