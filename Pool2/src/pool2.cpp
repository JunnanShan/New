/*
======================================================
*
* Author:   Junnan Shan (junnan.shan@polito.it)
*
======================================================
*/
#include <algorithm>
#include <math.h>
#include "pool2.h"

typedef float DataType;



//The function need to be accelerated in FPGA.
void pool2(DataType inp_img[IMAGE * INP_IMAGE_CHANNEL],
		DataType out_img[OUT_IMAGE_SIZE * OUT_IMAGE_SIZE * INP_IMAGE_CHANNEL])
{
#pragma HLS INTERFACE m_axi port=inp_img offset=slave bundle=gmem0
#pragma HLS INTERFACE m_axi port=out_img offset=slave bundle=gmem1

#pragma HLS INTERFACE s_axilite port=inp_img bundle=control
#pragma HLS INTERFACE s_axilite port=out_img bundle=control

#pragma HLS INTERFACE s_axilite port=return bundle=control



///////*********************************************************************************

L1:	for(int to=0; to<INP_IMAGE_CHANNEL; to++){//chan of output image

					DataType out[Tr][Tc] = {0};
#pragma HLS ARRAY_PARTITION variable=out complete dim=2
					DataType inp_image_local[INP_IMAGE_SIZE][INP_IMAGE_SIZE];
#pragma HLS ARRAY_PARTITION variable=inp_image_local complete dim=2

I2:						for(int Ri=0; Ri<INP_IMAGE_SIZE; Ri++){
I3:							for(int Li=0; Li<INP_IMAGE_SIZE; Li++){
#pragma HLS PIPELINE
								inp_image_local[Ri][Li] = inp_img[to*IMAGE + Ri*INP_IMAGE_SIZE + Li];
							}
						}

N1:					for(int i=0; i<FILTER_SIZE; i++){//row of filter
N2:						for(int j=0; j<FILTER_SIZE; j++){//col of filter
N3:							for(int trr=0; trr<Tr; trr++){//row tile of input image
#pragma HLS PIPELINE
N4:								for(int tcc=0; tcc<Tc; tcc++){//col tile of input image
											   if(inp_image_local[STRIDE*trr+i][STRIDE*tcc+j] > out[trr][tcc])
												   out[trr][tcc] = inp_image_local[STRIDE*trr+i][STRIDE*tcc+j];
										}
									}
								}
							}


//load output feature maps
O2:				    	for(int R=0; R<Tr; R++){
O3:				    		for(int C=0; C<Tc; C++){
#pragma HLS PIPELINE
								out_img[to*OUT_IMAGE_SIZE*OUT_IMAGE_SIZE+R*OUT_IMAGE_SIZE+C] = out[R][C];
							}
						}

	}//to
}//end of function
