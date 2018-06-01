/*
======================================================
*
* Author:   Junnan Shan (junnan.shan@polito.it)
*
======================================================
*/
#include <algorithm>
#include <math.h>
#include "conv3.h"
#include <stdio.h>
#include <cstring>
#include <iostream>

typedef float DataType;



//The function need to be accelerated in FPGA.
void conv3(DataType inp_img[IMAGE * INP_IMAGE_CHANNEL],
		DataType out_img[OUT_IMAGE_SIZE * OUT_IMAGE_SIZE * FILTER_BATCH],
		DataType filter[FILTER_BATCH * INP_IMAGE_CHANNEL/GROUP * FILTER_SIZE * FILTER_SIZE],
		DataType bias[FILTER_BATCH])

{
#pragma HLS INTERFACE m_axi port=inp_img offset=slave bundle=gmem0
#pragma HLS INTERFACE m_axi port=out_img offset=slave bundle=gmem1
#pragma HLS INTERFACE m_axi port=filter offset=slave bundle=gmem2
#pragma HLS INTERFACE m_axi port=bias offset=slave bundle=gmem0

#pragma HLS INTERFACE s_axilite port=inp_img bundle=control
#pragma HLS INTERFACE s_axilite port=out_img bundle=control
#pragma HLS INTERFACE s_axilite port=filter bundle=control
#pragma HLS INTERFACE s_axilite port=bias bundle=control

#pragma HLS INTERFACE s_axilite port=return bundle=control



///////*********************************************************************************

L1:			for(int to=0; to<FILTER_BATCH/GROUP; to+=Tm){//chan of output image

				DataType out[Tm][Tr][Tc];
#pragma HLS ARRAY_PARTITION variable=out complete dim=1

L2:				for(int ti=0; ti<INP_IMAGE_CHANNEL/GROUP; ti+=Tn){//chan of input image

					DataType inp_image_local[Tn][Tr+2*PAD][Tc+2*PAD];
//					DataType inp_image_local[Tn][Tr+2*PAD][Tc+2*PAD] = {0};
#pragma HLS ARRAY_PARTITION variable=inp_image_local complete dim=1

M2:				for(int CC=0; CC<Tc+2; CC++){
#pragma HLS PIPELINE
					for(int RR=0; RR<Tr+2; RR++){
						for(int MM=0; MM<Tn; MM++){
							inp_image_local[MM][RR][CC] = 0;
						}
					}
				}

//load input feature maps
I1:					for(int Ci=0; Ci<Tn; Ci++){
I2:						for(int Ri=0; Ri<(INP_IMAGE_SIZE-2*PAD)*(INP_IMAGE_SIZE-2*PAD); Ri++){
//I2:						for(int Ri=0; Ri<(INP_IMAGE_SIZE-2*PAD); Ri++){
//I3:							for(int Li=0; Li<(INP_IMAGE_SIZE-2*PAD); Li++){
#pragma HLS PIPELINE
							inp_image_local[Ci][Ri/(INP_IMAGE_SIZE-2*PAD)+PAD][Ri%(INP_IMAGE_SIZE-2*PAD)+PAD] = inp_img[(ti+Ci)*IMAGE + Ri];
//								inp_image_local[Ci][Ri+PAD][Li+PAD] = inp_img[(ti+Ci)*IMAGE + Ri*(INP_IMAGE_SIZE-2*PAD) + Li];
//							}
						}
					}

					DataType filter_local[Tm*Tn][FILTER_SIZE*FILTER_SIZE];
#pragma HLS ARRAY_PARTITION variable=filter_local complete dim=1

W1:					for(int Bi=0; Bi<Tm; Bi++){//load weights
W2:						for(int Ci=0; Ci<Tn*FILTER_SIZE*FILTER_SIZE; Ci++){
#pragma HLS PIPELINE
							filter_local[Bi*Tn+Ci/9][Ci%9] = filter[(Bi+to)*FILTER +ti*FILTER_SIZE*FILTER_SIZE + Ci];
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
				}
				//load output feature maps
O1:				for(int CH=0; CH<Tm; CH++){
O2:				    for(int R=0; R<Tr; R++){
O3:				    	for(int C=0; C<Tc; C++){
#pragma HLS PIPELINE
							DataType tmp=out[CH][R][C] + bias[to+CH];
							out[CH][R][C] = 0;
                            if (tmp<0) tmp=0;
				    		out_img[(to+CH)*OUT_IMAGE_SIZE*OUT_IMAGE_SIZE+R*OUT_IMAGE_SIZE+C] = tmp;
						}
				    }
				}
			}
}//end of function





