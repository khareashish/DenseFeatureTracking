#pragma once

#include "DFTParameters.hpp"


#define CO_SIZE 3
#define CO_OFFS CO_SIZE/2

#define FILTER_SIZE 5
#define FILTER_OFFS FILTER_SIZE/2

typedef ap_fixed<16,1,AP_RND_ZERO> Mask_t ;


void ComputeGradientsHLS(	hls::stream< Pixel_t > &pSrcImage,
							hls::stream< Grad_t  > &pGrad
							);


Grad_t ComputeGradientKernel(Pixel_t Window[CO_SIZE][CO_SIZE]);


void ComputeABC(	hls::stream< Grad_t > &GradStream,
					hls::stream< int > &A_Stream,
					hls::stream< int > &B_Stream,
					hls::stream< int > &C_Stream);


void SmoothABC(	hls::stream< int > &A_Stream,
				hls::stream< int > &B_Stream,
				hls::stream< int > &C_Stream,
				hls::stream< Fixed_t > &A_SmoothStream,
				hls::stream< Fixed_t > &B_SmoothStream,
				hls::stream< Fixed_t > &C_SmoothStream);

Fixed_t FilterKernelOperator(Pixel_t Window[FILTER_SIZE][FILTER_SIZE]);

void ComputeResponseHLS(	hls::stream< Fixed_t > &A_SmoothStream,
							hls::stream< Fixed_t > &B_SmoothStream,
							hls::stream< Fixed_t > &C_SmoothStream,
							hls::stream< Fixed_t > &ResponseStream);



#pragma SDS data access_pattern(pSrcImage:SEQUENTIAL, pResponse:SEQUENTIAL)
#pragma SDS data copy( pSrcImage[0:WIDTH*HEIGHT], pResponse[0:WIDTH*HEIGHT])
void compute_response(	Pixel_t* pSrcImage,
						Fixed_t* pResponse
						);
