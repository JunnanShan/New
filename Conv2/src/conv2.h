/* 
======================================================
*
* Author:   Junnan Shan (junnan.shan@polito.it)
*
======================================================
*/


#define INP_IMAGE_SIZE 31
#define INP_IMAGE_CHANNEL 96
#define OUT_IMAGE_SIZE 27
#define FILTER_BATCH 256
#define FILTER_CHANNEL 48
#define FILTER_SIZE 5
#define STRIDE 1
#define STRIDE_NEW 5
#define GROUP 2
#define PAD 2
#define FILTER FILTER_SIZE * FILTER_SIZE * FILTER_CHANNEL
#define IMAGE (INP_IMAGE_SIZE-4) * (INP_IMAGE_SIZE-4)
#define Tm 64
#define Tn 8
#define Tr 27
#define Tc 27

