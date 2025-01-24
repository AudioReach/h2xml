/**************************************************************************//**
* @file
* Example for h2xmle_defaultFile
*
* @note Copyright (c) Qualcomm Innovation Center, Inc. All rights reserved.
*       SPDX-License-Identifier: BSD-3-Clause
******************************************************************************/

typedef struct {
	long a;
	short b;
	short c;
} sStruct1_t;

typedef struct {
	long x[8];
} sStruct2_t;

typedef struct {
        double a;
        long long b;
        float c;
        long d;
} sStructMix_t;
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
	unsigned char a[32];	/**<		@h2xmle_defaultFile {defaultFile2.bin} 		*/	
	float f[32]; 			/**<		@h2xmle_defaultFile {defaultfileFloat.bin} 	*/
	double d[32]; 			/**<		@h2xmle_defaultFile {defaultfileDouble.bin} */

};

/** @h2xmlp_parameter 	{Parameter2,0x22222299} 
*/
struct param_2{
	unsigned char a[32];	/**<		@h2xmle_defaultFile {defaultFile2.bin} 		*/	
	sStruct1_t   s1[4];		/**<		@h2xmle_defaultFile {defaultFile2.bin} 		*/	
	sStruct2_t	 s2;		/**<		@h2xmle_defaultFile {defaultFile2.bin} 		*/		
	char b;					/**<		@h2xmle_default {0x44} 		*/
	long c;   				/**<		@h2xmle_default {0x17654321} 	*/
	sStruct1_t   s3[0];		/**<		@h2xmle_defaultFile {defaultFile2.bin} 		*/	
	sStructMix_t   smix; 	/**<		@h2xmle_defaultFile {defaultfileMixed.bin} 	*/	
};

/** @h2xmlp_parameter 		{Parameter3,0x33333399} 
	@h2xmlx_ExpandStructs	{true}
*/
struct param_3{
	unsigned char a[32];	/**<		@h2xmle_defaultFile {defaultFile2.bin} 		*/	
	sStruct1_t   s1[4];		/**<		@h2xmle_defaultFile {defaultFile2.bin} 		*/	
	sStruct2_t	 s2;		/**<		@h2xmle_defaultFile {defaultFile2.bin} 		*/	
	char b;					/**<		@h2xmle_default {0x44} 		*/
	long c;   				/**<		@h2xmle_default {0x17654321} 	*/
	sStruct1_t   s3[0];		/**<		@h2xmle_defaultFile {defaultFile2.bin} 		*/	
};

/** @h2xmlp_parameter 		{Parameter4,0x44444499} 
	@h2xmlx_ExpandStructs	{true}
	@h2xmlx_ExpandArray		{true}
*/
struct param_4{
	unsigned char a[32];	/**<		@h2xmle_defaultFile {defaultFile2.bin} 		*/	
	sStruct1_t   s1[4];		/**<		@h2xmle_defaultFile {defaultFile2.bin} 		*/
	sStruct2_t	 s2;		/**<		@h2xmle_defaultFile {defaultFile2.bin} 		*/		
	char b;					/**<		@h2xmle_default {0x44} 		*/
	long c;   				/**<		@h2xmle_default {0x17654321} 	*/
	sStruct1_t   s3[0];		/**<		@h2xmle_defaultFile {defaultFile2.bin} 		*/	
	
    float f[32]; 			/**<		@h2xmle_defaultFile {defaultfileFloat.bin} 	*/
	double d[32]; 			/**<		@h2xmle_defaultFile {defaultfileDouble.bin} */
	sStructMix_t   smix; 	/**<		@h2xmle_defaultFile {defaultfileMixed.bin} 	*/	

};



/** @} */							/* End of Module */						

