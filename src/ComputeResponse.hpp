#pragma once

#include "DFTParameters.hpp"



#pragma SDS data access_pattern(pGrad:SEQUENTIAL, pResponse:SEQUENTIAL)
#pragma SDS data copy( pGrad[0:WIDTH*HEIGHT], pResponse[0:WIDTH*HEIGHT])

void compute_response(	Grad_t* pGrad,
						Fixed_t* pResponse
						);
