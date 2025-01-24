/**************************************************************************//**
* example for h2xmlk_modKeys
*
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

enum values{
    v1 = 0xa9000000
};


enum TEST_KEYS1 {
   key21 = 21,      /**< @h2xmle_name{k21} @h2xmle_description {Description k21}    */
   key22 = 22,      /**< @h2xmle_name{k22} @h2xmle_description {Description k22}    */
   key23 = 23,      /**< @h2xmle_name{k23} @h2xmle_description {Description k23}    */
   key24 = v1        /**< @h2xmle_name{k24} @h2xmle_description {Description k24}    */
};

enum TEST_KEYS2 {
   key3,
   key4
};

/**
	@h2xmlk_modTagList
*/
typedef enum {
    TAG1,  /**< @h2xmle_name{tag1} @h2xmle_description {Description 1}    */
    TAG2,  /**< @h2xmle_name{tag2} @h2xmle_description {Description 2}    */
    TAG3,  /**< @h2xmle_name{tag3} @h2xmle_description {Description 3} @h2xmlk_tagKeys{TEST_KEYS1}     */
    TAG4,  /**< @h2xmle_name{tag4} @h2xmlk_tagKeys{TEST_KEYS2} @h2xmle_description {Description 4}    */
} TEST_TAGS;        
