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

enum processors {
	PROC_DOMAIN_ADSP = APM_PROC_DOMAIN_ID_ADSP, 	/**< @h2xmle_name {ADSP} */
	PROC_DOMAIN_MDSP = APM_PROC_DOMAIN_ID_MDSP, 	/**< @h2xmle_name {MDSP} */
	PROC_DOMAIN_SDSP = APM_PROC_DOMAIN_ID_SDSP,  	/**< @h2xmle_name {SDSP} */
	PROC_DOMAIN_CDSP = APM_PROC_DOMAIN_ID_CDSP  	/**< @h2xmle_name {CDSP} */
};

/** 
	@h2xml_title_date {08/01/2018} 
*/

/****************************************************************
 Subgraph Config
***************************************************************/
#define APM_PARAM_ID_SUB_GRAPH_CONFIG 0x08001001
/**
	@h2xmlgp_config {"SG_CFG", "APM_PARAM_ID_SUB_GRAPH_CONFIG", APM_PARAM_ID_SUB_GRAPH_CONFIG}
	@h2xmlgp_description {Description 1}
	@{ <-- Start of subgraph_cfg -->
*/

#define APM_SUB_GRAPH_PROP_ID_PERF_MODE 0x0800100E
#define APM_SG_PERF_MODE_LOW_POWER 0x1
#define APM_SG_PERF_MODE_LOW_LATENCY 0x2

typedef struct apm_sg_prop_id_perf_mode_t apm_sg_prop_id_perf_mode_t;

/**
	@h2xmlp_property {"APM_SUB_GRAPH_PROP_ID_PERF_MODE", APM_SUB_GRAPH_PROP_ID_PERF_MODE}
	@h2xmlp_description {Description 10}
*/
struct apm_sg_prop_id_perf_mode_t
{
	int perf_mode;
	/**<
		@h2xmle_rangeList {"APM_SG_PERF_MODE_LOW_POWER"=1; "APM_SG_PERF_MODE_LOW_LATENCY"=2}
		@h2xmle_default {1}
		@h2xmle_description {Description 101}
	*/
}
;

#define APM_SUB_GRAPH_PROP_ID_DIRECTION 0x0800100F
#define APM_SUB_GRAPH_DIRECTION_TX 0x1
#define APM_SUB_GRAPH_DIRECTION_RX 0x2

typedef struct apm_sg_prop_id_direction_t apm_sg_prop_id_direction_t;
/**
	@h2xmlp_property {"APM_SUB_GRAPH_PROP_ID_DIRECTION", APM_SUB_GRAPH_PROP_ID_DIRECTION}
	@h2xmlp_description {Description 20}
*/
struct apm_sg_prop_id_direction_t
{
	int direction;
	/**<
		@h2xmle_rangeList {"APM_SUB_GRAPH_DIRECTION_TX"=1; "APM_SUB_GRAPH_DIRECTION_RX"=2}
		@h2xmle_default {1}
		@h2xmle_description {Description 201}
	*/
}
;

#define APM_SUB_GRAPH_PROP_ID_SCENARIO_ID 0x08001010
#define APM_SUB_GRAPH_SID_AUDIO_PLAYBACK 0x1
#define APM_SUB_GRAPH_SID_AUDIO_RECORD 0x2

typedef struct apm_sg_prop_id_scenario_id_t apm_sg_prop_id_scenario_id_t;
/**
	@h2xmlp_property {"APM_SUB_GRAPH_PROP_ID_SCENARIO_ID", APM_SUB_GRAPH_PROP_ID_SCENARIO_ID}
	@h2xmlp_description {Description 30}
*/
struct apm_sg_prop_id_scenario_id_t
{
	int scenario_id;
	/**<
		@h2xmle_rangeList {"APM_SUB_GRAPH_SID_AUDIO_PLAYBACK"=1; "APM_SUB_GRAPH_SID_AUDIO_RECORD"=2}
		@h2xmle_default {1}
		@h2xmle_description {Description 301}
	*/
}
;
/** @} <-- End of subgraph_cfg -->*/


/****************************************************************
 Container Config
***************************************************************/

/**
	@h2xmlgp_config {"CONT_CFG", "APM_PARAM_ID_SUB_GRAPH_CONFIG", APM_PARAM_ID_SUB_GRAPH_CONFIG}
	@h2xmlgp_description {Description 1}
	@{ <-- Start of container_cfg -->
*/


/**
	@h2xmlp_property {"APM_SUB_GRAPH_PROP_ID_PERF_MODE", APM_SUB_GRAPH_PROP_ID_PERF_MODE}
	@h2xmlp_description {Description 10}
*/
struct xapm_sg_prop_id_perf_mode_t
{
	int perf_mode;
	/**<
		@h2xmle_rangeList {"APM_SG_PERF_MODE_LOW_POWER"=1; "APM_SG_PERF_MODE_LOW_LATENCY"=2}
		@h2xmle_default {1}
		@h2xmle_description {Description 101}
	*/
}
;

/**
	@h2xmlp_property {"APM_SUB_GRAPH_PROP_ID_DIRECTION", APM_SUB_GRAPH_PROP_ID_DIRECTION}
	@h2xmlp_description {Description 20}
*/
struct xapm_sg_prop_id_direction_t
{
	int direction;
	/**<
		@h2xmle_rangeList {"APM_SUB_GRAPH_DIRECTION_TX"=1; "APM_SUB_GRAPH_DIRECTION_RX"=2}
		@h2xmle_default {1}
		@h2xmle_description {Description 201}
	*/
}
;

/**
	@h2xmlp_property {"APM_SUB_GRAPH_PROP_ID_SCENARIO_ID", APM_SUB_GRAPH_PROP_ID_SCENARIO_ID}
	@h2xmlp_description {Description 30}
*/
struct xapm_sg_prop_id_scenario_id_t
{
	int scenario_id;
	/**<
		@h2xmle_rangeList {"APM_SUB_GRAPH_SID_AUDIO_PLAYBACK"=1; "APM_SUB_GRAPH_SID_AUDIO_RECORD"=2}
		@h2xmle_default {1}
		@h2xmle_description {Description 301}
	*/
}
;
/** @} <-- End of container_cfg -->*/


/****************************************************************
 VCPM Config
***************************************************************/

/**
	@h2xmlgp_config {"VOC_SG_CFG", "APM_PARAM_ID_SUB_GRAPH_CONFIG", APM_PARAM_ID_SUB_GRAPH_CONFIG}
	@h2xmlgp_description {Description 1}
	@{ <-- Start of VOC_SG_CFG -->
*/

/**
	@h2xmlp_property {"APM_SUB_GRAPH_PROP_ID_PERF_MODE", APM_SUB_GRAPH_PROP_ID_PERF_MODE}
	@h2xmlp_description {Description 10}
	@h2xmlp_isVoice
*/
struct yapm_sg_prop_id_perf_mode_t
{
	int perf_mode;
	/**<
		@h2xmle_rangeList {"APM_SG_PERF_MODE_LOW_POWER"=1; "APM_SG_PERF_MODE_LOW_LATENCY"=2}
		@h2xmle_default {1}
		@h2xmle_description {Description 101}
	*/
}
;

/**
	@h2xmlp_property {"APM_SUB_GRAPH_PROP_ID_DIRECTION", APM_SUB_GRAPH_PROP_ID_DIRECTION}
	@h2xmlp_description {Description 20}
*/
struct yapm_sg_prop_id_direction_t
{
	int direction;
	/**<
		@h2xmle_rangeList {"APM_SUB_GRAPH_DIRECTION_TX"=1; "APM_SUB_GRAPH_DIRECTION_RX"=2}
		@h2xmle_default {1}
		@h2xmle_description {Description 201}
	*/
}
;

/**
	@h2xmlp_property {"APM_SUB_GRAPH_PROP_ID_SCENARIO_ID", APM_SUB_GRAPH_PROP_ID_SCENARIO_ID}
	@h2xmlp_description {Description 30}
	@h2xmlp_isVoice
*/
struct yapm_sg_prop_id_scenario_id_t
{
	int scenario_id;
	/**<
		@h2xmle_rangeList {"APM_SUB_GRAPH_SID_AUDIO_PLAYBACK"=1; "APM_SUB_GRAPH_SID_AUDIO_RECORD"=2}
		@h2xmle_default {1}
		@h2xmle_description {Description 301}
	*/
}
;
/** @} <-- End of VOC_SG_CFG -->*/
