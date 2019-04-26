#include "ComputeResponse.hpp"



#pragma SDS data access_pattern(pGrad:SEQUENTIAL, pResponse:SEQUENTIAL)
#pragma SDS data copy( pGrad[0:WIDTH*HEIGHT], pResponse[0:WIDTH*HEIGHT])

void compute_response(	Grad_t* pGrad,
						Fixed_t* pResponse)
{


	for(int i = 0 ; i < WIDTH*HEIGHT; i++)
	{

#pragma HLS PIPELINE

		ap_uint<32> A, B, C2 ;					// all of these are going to be positive
		int C ;									// this could be negative

		ap_int<16> gradX = pGrad[i].dIdx;
		ap_int<16> gradY = pGrad[i].dIdy;

		A = gradX*gradX;

		B = gradY*gradY;

		C = gradY*gradX;

		ap_uint<64> DiffOfAB2 = (A-B)*(A-B);

		float sqrtTerm = sqrt(DiffOfAB2 + 4*C2);


		float tmpResponse = A + B - sqrtTerm;

		if(i == 245759)
			printf("%d -- %f, sqrtTerm = %f, insideSqrt = %d",i, tmpResponse, sqrtTerm, (int)(DiffOfAB2 + 4*C2));

		Fixed_t ResponseOut = tmpResponse ;

		pResponse[i] = ResponseOut;

	}

}
