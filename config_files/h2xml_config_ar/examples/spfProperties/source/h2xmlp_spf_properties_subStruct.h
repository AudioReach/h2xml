/**************************************************************************//**
* @file
* Example for subStruct and variableArraySize
*
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/
typedef unsigned int uint32_t;

enum {
	APM_CONTAINER_CAP_ID_PP,
    APM_CONTAINER_CAP_ID_CD,
    APM_CONTAINER_CAP_ID_EP,
    APM_CONTAINER_CAP_ID_OLC
};

enum {
	APM_PARAM_ID_CONTAINER_CONFIG=0x1000
}

struct apm_cont_capability_t
{
   uint32_t capability_id;
   /**< @h2xmle_rangeList {"PP"=APM_CONTAINER_CAP_ID_PP,
                           "CD"=APM_CONTAINER_CAP_ID_CD,
                           "EP"=APM_CONTAINER_CAP_ID_EP,
                           "OLC"=APM_CONTAINER_CAP_ID_OLC}
        @h2xmle_default {1}
        @h2xmle_description {Capability ID for this container} */
};

typedef struct apm_cont_capability_t apm_cont_capability_t;

/** h2xmlgp_configType {"CONT_CFG"}
    @h2xmlgp_config {"CONT_CFG", "APM_PARAM_ID_CONTAINER_CONFIG", APM_PARAM_ID_CONTAINER_CONFIG}
    @h2xmlgp_description {ID of the parameter to configure the container properties}
    @{ <-- Start of container_cfg --> */
/*--------------------------------------------------------------------------------------------------------------------*/


/** 
	@h2xmlp_insertSubStruct{apm_cont_capability_t}
*/


#define APM_CONTAINER_PROP_ID_CAPABILITY_LIST     0x08001011
/** @h2xmlp_property {"APM_CONTAINER_PROP_ID_CAPABILITY_LIST", APM_CONTAINER_PROP_ID_CAPABILITY_LIST}
    @h2xmlp_description {Container property ID for Capability List} */

struct apm_cont_prop_id_cap_list_t
{
   uint32_t num_capability_id;
   /**< @h2xmle_range {1..0xFFFFFFFF}
        @h2xmle_default {1}
        @h2xmle_description {Number of capabilities for this container} */

#ifdef __H2XML__
   apm_cont_capability_t cont_capability_id_list[0];
   /**< @h2xmle_range {1..0xFFFFFFFF}
        @h2xmle_default {1}
        @h2xmle_description {Capability ID list for this container}
        @h2xmle_variableArraySize {num_capability_id}*/
#endif
};
typedef struct apm_cont_prop_id_cap_list_t apm_cont_prop_id_cap_list_t;

/** @} <-- End of container_cfg -->*/