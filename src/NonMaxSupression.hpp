#pragma once

#include "DFTParameters.hpp"

#define SNM_SIZE 3
#define SNM_OFFS SNM_SIZE/2

#define RESP_TH 0.01

#pragma SDS data access_pattern(pResponse:SEQUENTIAL, pKeypointImage:SEQUENTIAL)
#pragma SDS data copy( pResponse[0:WIDTH*HEIGHT], pKeypointImage[0:WIDTH*HEIGHT])

void non_max_suppresion(	 Fixed_t* pResponse,
							 Pixel_t*  pKeypointImage );


bool IsLocalExtremum(Fixed_t window[SNM_SIZE][SNM_SIZE]);
