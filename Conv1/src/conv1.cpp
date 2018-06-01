/*
======================================================
*
* Author:   Junnan Shan (junnan.shan@polito.it)
*
======================================================
*/
#include <algorithm>
#include <math.h>
#include "conv1.h"

typedef float DataType;



//The function need to be accelerated in FPGA.
void conv1(DataType inp_img[IMAGE * INP_IMAGE_CHANNEL],
		DataType out_img[OUT_IMAGE_SIZE * OUT_IMAGE_SIZE * FILTER_BATCH],
		DataType filter[FILTER_BATCH * INP_IMAGE_CHANNEL/GROUP * FILTER_SIZE * FILTER_SIZE],
		DataType bias[FILTER_BATCH])
{
#pragma HLS INTERFACE m_axi port=inp_img offset=slave bundle=gmem0
#pragma HLS INTERFACE m_axi port=out_img offset=slave bundle=gmem1
#pragma HLS INTERFACE m_axi port=filter offset=slave bundle=gmem0
#pragma HLS INTERFACE m_axi port=bias offset=slave bundle=gmem0

#pragma HLS INTERFACE s_axilite port=inp_img bundle=control
#pragma HLS INTERFACE s_axilite port=out_img bundle=control
#pragma HLS INTERFACE s_axilite port=filter bundle=control
#pragma HLS INTERFACE s_axilite port=bias bundle=control

#pragma HLS INTERFACE s_axilite port=return bundle=control



///////*********************************************************************************

	DataType filter_local[Tm*Tn][FILTER_SIZE*FILTER_SIZE];
#pragma HLS ARRAY_PARTITION variable=filter_local complete dim=1

W1:					for(int Bi=0; Bi<Tm; Bi++){//load weights
W2:						for(int Ci=0; Ci<Tn; Ci++){
W3:							for(int Ri=0; Ri<FILTER_SIZE*FILTER_SIZE; Ri++){
#pragma HLS PIPELINE
								filter_local[Bi*Tn+Ci][Ri] = filter[(Bi)*FILTER + Ci*FILTER_SIZE*FILTER_SIZE + Ri];
			}
		}
	}

L3:			for(int row=0; row<OUT_IMAGE_SIZE; row+=Tr){
L4:				for(int col=0; col<OUT_IMAGE_SIZE; col+=Tc){

					DataType out[Tm][Tr][Tc] = {0};
#pragma HLS ARRAY_PARTITION variable=out complete dim=1
					DataType inp_image_local[Tn][(Tr-1)*STRIDE+FILTER_SIZE][(Tc-1)*STRIDE+FILTER_SIZE];
#pragma HLS ARRAY_PARTITION variable=inp_image_local complete dim=1

I1:					for(int Ci=0; Ci<Tn; Ci++){//load input feature maps
I2:						for(int Ri=0; Ri<((Tr-1)*STRIDE+FILTER_SIZE); Ri++){
I3:							for(int Li=0; Li<(Tc-1)*STRIDE+FILTER_SIZE; Li++){
#pragma HLS PIPELINE
								inp_image_local[Ci][Ri][Li] = inp_img[Ci*IMAGE + (Ri+row*STRIDE)*INP_IMAGE_SIZE + col*STRIDE + Li];
							}
						}
					}


N1:					for(int i=0; i<FILTER_SIZE; i++){//row of filter
N2:						for(int j=0; j<FILTER_SIZE; j++){//col of filter
N3:							for(int trr=0; trr<Tr; trr++){//row tile of input image
N4:								for(int tcc=0; tcc<Tc; tcc++){//col tile of input image
#pragma HLS PIPELINE
N5:									for(int too=0; too<Tm; too++){//chan tile of output image
N6:										for(int tii=0; tii<Tn; tii++){//chan tile of input image
											out[too][trr][tcc] += filter_local[too*Tn+tii][i*FILTER_SIZE+j] * inp_image_local[tii][STRIDE*trr+i][STRIDE*tcc+j];
										}
									}
								}
							}
						}
					}
//load output feature maps
O1:					for(int CH=0; CH<Tm; CH++){
O2:				    	for(int R=0; R<Tr; R++){
O3:				    		for(int C=0; C<Tc; C++){
#pragma HLS PIPELINE
								DataType tmp=out[CH][R][C] + bias[CH];
								if (tmp<0) tmp=0;
								out_img[(CH)*OUT_IMAGE_SIZE*OUT_IMAGE_SIZE+(R+row)*OUT_IMAGE_SIZE+C+col] = tmp;
							}
						}
					}
		}//col
	}//row
}//end of function





