/**************************************************************************//**
* @file
* Example for driver properties
*
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

typedef unsigned int uint32_t;

#define SUB_GRAPH_PROP_ID   0x08000001
#define ROUTING_ID_ARM      0x1
#define ROUTING_ID_ADSP     0x2
#define FLUSHABLE_FALSE     0x0
#define FLUSHABLE_TRUE      0x1

typedef struct sg_prop_t sg_prop_t;

/**
	@h2xmlp_property {"SUB_GRAPH_PROP", SUB_GRAPH_PROP_ID}
	@h2xmlp_description {description1}
*/
struct sg_prop_t
{
    uint32_t routing_id;
    /**<
	    @h2xmle_rangeList {"ARM"=1; " ADSP"=2}
	    @h2xmle_default {2}
	    @h2xmle_description {description2}
    */
    
    uint32_t is_flushable;
    /**<
	    @h2xmle_rangeList {"FALSE"=0; "TRUE"=1}
	    @h2xmle_default {1}
	    @h2xmle_description {description3}
    */
};

#define SUB_GRAPH_TYPE_ID               0x08000010
#define SUB_GRAPH_TYPE_STREAM           0x1
#define SUB_GRAPH_TYPE_DEVCICE          0x2
#define SUB_GRAPH_TYPE_STREAM_DEVICE    0x3
#define SUB_GRAPH_TYPE_STREAM_PP        0x4
#define SUB_GRAPH_TYPE_DEVCICE_PP       0x5

typedef struct sg_type_t sg_type_t;

/**
@h2xmlp_property {"SUB_GRAPH_TYPE_ID", SUB_GRAPH_TYPE_ID}
@h2xmlp_description {description10}
*/
struct sg_type_t
{
    uint32_t sg_type;
    /**<
    @h2xmle_rangeList {"SUB_GRAPH_TYPE_STREAM"=SUB_GRAPH_TYPE_STREAM;
        "SUB_GRAPH_TYPE_DEVCICE"=SUB_GRAPH_TYPE_DEVCICE;
        "SUB_GRAPH_TYPE_STREAM_DEVICE"=SUB_GRAPH_TYPE_DEVCICE;
        "SUB_GRAPH_TYPE_STREAM_PP"=SUB_GRAPH_TYPE_STREAM_PP;
        "SUB_GRAPH_TYPE_DEVCICE_PP"=SUB_GRAPH_TYPE_DEVCICE_PP}
    @h2xmle_default {1}
    @h2xmle_description {description11}
    */
};

