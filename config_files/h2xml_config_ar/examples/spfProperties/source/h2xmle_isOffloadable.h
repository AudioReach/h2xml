/**************************************************************************//**
* @file
* Example for spf properties
*
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

#define APM_PROC_DOMAIN_ID_ADSP                  0x1
#define APM_PROC_DOMAIN_ID_MDSP                  0x2
#define APM_PROC_DOMAIN_ID_SDSP                  0x3
#define APM_PROC_DOMAIN_ID_CDSP                  0x4

#define APM_CONTAINER_CAP_ID_PP                  0x1
#define APM_CONTAINER_CAP_ID_CD                  0x2
#define APM_CONTAINER_CAP_ID_EP                  0x3
#define APM_CONTAINER_CAP_ID_OLC                 0x4

#define APM_PROP_ID_INVALID                       0x0
#define APM_PARAM_ID_CONTAINER_CONFIG             0x08001000
#define APM_CONTAINER_PROP_ID_CAPABILITY_LIST     0x08001011
#define APM_CONTAINER_PROP_ID_PROC_DOMAIN         0x08001014

enum eProcessors {
    PROC_DOMAIN_DONT_CARE       = 0xFFFFFFFF,               /**< @h2xmle_name {DONT_CARE} */
	PROC_DOMAIN_ADSP            = APM_PROC_DOMAIN_ID_ADSP, 	/**< @h2xmle_name {ADSP} */
	PROC_DOMAIN_MDSP            = APM_PROC_DOMAIN_ID_MDSP, 	/**< @h2xmle_name {MDSP} */
	PROC_DOMAIN_SDSP            = APM_PROC_DOMAIN_ID_SDSP,  //#< @h2xmle_name {SDSP} 	
	PROC_DOMAIN_CDSP            = APM_PROC_DOMAIN_ID_CDSP   ///< @h2xmle_name {CDSP} 
};

enum eContainerCap1 {
   APM_CONTAINER_CAP_INVALID    = APM_PROP_ID_INVALID,      /**< @h2xmle_name {INVALID} */
   APM_CONTAINER_CAP_PP         = APM_CONTAINER_CAP_ID_PP,  /**< @h2xmle_name {PP} */
};

enum eContainerCap2 {
   APM_CONTAINER_CAP_CD         = APM_CONTAINER_CAP_ID_CD,  /**< @h2xmle_name {CD} */
   APM_CONTAINER_CAP_EP         = APM_CONTAINER_CAP_ID_EP,  /**< @h2xmle_name {EP} @h2xmle_isOffloadable {false} */
   APM_CONTAINER_CAP_OLC        = APM_CONTAINER_CAP_ID_OLC  /**< @h2xmle_name {OLC}*/
};

/*--------------------------------------------------------------------------------------------------------------------*/
/** h2xmlgp_configType {"CONT_CFG"}
    @h2xmlgp_config {"CONT_CFG", "APM_PARAM_ID_CONTAINER_CONFIG", APM_PARAM_ID_CONTAINER_CONFIG}
    @h2xmlgp_description {ID of the parameter to configure the container properties}
    @{ <-- Start of container_cfg --> */
/*--------------------------------------------------------------------------------------------------------------------*/

/** @h2xmlp_property {"APM_CONTAINER_PROP_ID_CAPABILITY_LIST", APM_CONTAINER_PROP_ID_CAPABILITY_LIST}
    @h2xmlp_description {Container property ID for Capability List} 
*/
typedef struct{
   unsigned int capability_id;
   /**< h2xmle_rangeList {"INVALID"=APM_PROP_ID_INVALID,
                           "PP"=APM_CONTAINER_CAP_ID_PP,
                           "CD"=APM_CONTAINER_CAP_ID_CD,
                           "EP"=APM_CONTAINER_CAP_ID_EP,
                           "OLC"=APM_CONTAINER_CAP_ID_OLC}
        @h2xmle_rangeEnum {eContainerCap1, eContainerCap2}
        @h2xmle_default {APM_PROP_ID_INVALID}
        @h2xmle_description {Capability ID for this container} */
} apm_cont_capability_t;


/** @h2xmlp_property {"APM_CONTAINER_PROP_ID_PROC_DOMAIN", APM_CONTAINER_PROP_ID_PROC_DOMAIN}
    @h2xmlp_description {Container property ID for Processor Domain } */

typedef struct 
{
   unsigned int proc_domain;
   /**< h2xmle_rangeList {"DONT_CARE"=0xFFFFFFFF,
                           "MDSP"=APM_PROC_DOMAIN_ID_MDSP,
                           "ADSP"=APM_PROC_DOMAIN_ID_ADSP,
                           "SDSP"=APM_PROC_DOMAIN_ID_SDSP,
                           "CDSP"=APM_PROC_DOMAIN_ID_CDSP}
      @h2xmle_rangeEnum {eProcessors}

        @h2xmle_default {0xffffffff}
        @h2xmle_description {Container processor domain.} */
}apm_cont_prop_id_proc_domain_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/** @} <-- End of container_cfg -->*/
/*--------------------------------------------------------------------------------------------------------------------*/







