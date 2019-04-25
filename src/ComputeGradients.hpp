#pragma once

#include "DFTParameters.hpp"


#define CO_SIZE 3
#define CO_OFFS CO_SIZE/2



#pragma SDS data access_pattern(pSrcImage:SEQUENTIAL, pGrad:SEQUENTIAL)
#pragma SDS data copy( pSrcImage[0:WIDTH*HEIGHT], pGrad[0:WIDTH*HEIGHT])

void compute_gradients( Pixel_t* pSrcImage,
						Grad_t* pGrad
						);


Grad_t ComputeGradientKernel(Pixel_t Window[CO_SIZE][CO_SIZE]);
