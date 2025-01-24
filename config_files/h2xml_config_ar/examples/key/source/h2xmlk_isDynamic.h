/**************************************************************************//**
* @file
* Example for h2xmlk_isDynamic
*
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

enum AllKeyIds{
	Key1 = 0xA0000000,  /**< @h2xmle_name{Key1_Name} */
	Key2 = 0xB0000000, 
	Key3 = 0xC0000000, 
    Key4 = 0xD0000000, 
};

/**
	@h2xmlk_key {Key1 }
	@h2xmlk_description {Description Key1}
*/
enum Key_1 {
	key1_val_1 = 0xA0000001, /**< @h2xmle_description {Description 101}*/
	key1_val_2 = 0xA0000002, /**< @h2xmle_description {Description 102}*/
};

/**
	@h2xmlk_key {Key2}
	@h2xmlk_isDynamic
	@h2xmlk_description {Description Key2}
*/
enum Key_2 {
	key2_val_1 = 0xB0000001, /**< @h2xmle_description {Description 201}*/
	key2_val_2 = 0xB0000002, /**< @h2xmle_description {Description 202}*/
};

/**
	@h2xmlk_key {Key3}
	@h2xmlk_isDynamic {true}
	@h2xmlk_description {Description Key3}
*/
enum Key_3 {
	key3_val_1 = 0xC0000001, /**< @h2xmle_description {Description 301}*/
	key3_val_2 = 0xC0000002, /**< @h2xmle_description {Description 302}*/
};

/**
	@h2xmlk_key {Key3}
	@h2xmlk_isDynamic {false}
	@h2xmlk_description {Description Key4}
*/
enum Key_4 {
	key4_val_1 = 0xD0000001, /**< @h2xmle_description {Description 401}*/
	key4_val_2 = 0xD0000002, /**< @h2xmle_description {Description 402}*/
};

