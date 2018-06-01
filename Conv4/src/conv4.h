/* 
======================================================
*
* Author:   Junnan Shan (junnan.shan@polito.it)
*
======================================================
*/


#define INP_IMAGE_SIZE 15
#define INP_IMAGE_CHANNEL 384
#define OUT_IMAGE_SIZE 13
#define FILTER_BATCH 384
#define FILTER_CHANNEL 192
#define FILTER_SIZE 3
#define STRIDE 1
#define STRIDE_NEW 3
#define GROUP 2
#define PAD 1
#define FILTER FILTER_SIZE * FILTER_SIZE * FILTER_CHANNEL
#define IMAGE (INP_IMAGE_SIZE-2) * (INP_IMAGE_SIZE-2)
#define Tm 32
#define Tn 8
#define Tr 13
#define Tc 13

