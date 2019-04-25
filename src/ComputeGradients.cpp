

#include "ComputeGradients.hpp"


#pragma SDS data access_pattern(pSrcImage:SEQUENTIAL, pGrad:SEQUENTIAL)
#pragma SDS data copy( pSrcImage[0:WIDTH*HEIGHT], pGrad[0:WIDTH*HEIGHT])


void compute_gradients( Pixel_t* pSrcImage,
						Grad_t* pGrad
						)
{


	Pixel_t WindowSrc[CO_SIZE][CO_SIZE];						// Window is completely partitioned providing random access
#pragma HLS ARRAY_PARTITION variable=WindowSrc complete dim=0

	Pixel_t SrcImageBuf[CO_SIZE][WIDTH];						// Image Buffer on which the kernel operates.
#pragma HLS ARRAY_PARTITION variable=SrcImageBuf complete dim=1

	Pixel_t SrcRightCol[CO_SIZE]; 							// right-most, incoming column
#pragma HLS ARRAY_PARTITION variable=SrcRightCol dim=0

int idx = 0 ;

L1:	for (int iRow = 0 ; iRow < HEIGHT + CO_OFFS; iRow++ )
	{
#pragma HLS LATENCY min=0
	L2:	for(int iCol = 0 ; iCol < WIDTH + CO_OFFS; iCol++ )
		{
	#pragma HLS PIPELINE

			Pixel_t PixelIn;

			if(iRow<HEIGHT && iCol < WIDTH)
			{
				PixelIn = pSrcImage[iRow*WIDTH + iCol];
			}
			else
			{
				PixelIn = 0 ;

			}


			// Shift image buffer and get new line in
			if(iCol < WIDTH)
			{
				for(unsigned char ii = 0; ii < CO_SIZE - 1; ii++)
				{
					SrcRightCol[ii] = SrcImageBuf[ii][iCol]=SrcImageBuf[ii+1][iCol];
				}
				SrcRightCol[CO_SIZE-1] = SrcImageBuf[CO_SIZE-1][iCol] = PixelIn;
			}
			else
			{
				for(unsigned char ii = 0; ii < CO_SIZE; ii++)
				{
					SrcRightCol[ii] = 0 ;
				}
			}



			//Shift from left to right the sliding window to make room for the new column
			for(unsigned char ii = 0; ii < CO_SIZE; ii++)
			{
				for(unsigned char jj = 0; jj < CO_SIZE-1; jj++)
				{
					WindowSrc[ii][jj] = WindowSrc[ii][jj+1];					// scale 1
				}
			}

			for(unsigned char ii = 0; ii < CO_SIZE; ii++)
			{
				WindowSrc[ii][CO_SIZE-1] = SrcRightCol[ii];

			}


			// operate on the window and output the result on the output stream

			if ( (iRow >= CO_OFFS) && (iCol >= CO_OFFS)  )
			{

				Grad_t  OG_Data = ComputeGradientKernel(WindowSrc);		// Oriented Gradient

				pGrad[idx] = OG_Data;

				idx++ ;
			}


		}//end of L2
	}//end of L1



}


Grad_t ComputeGradientKernel(Pixel_t Window[CO_SIZE][CO_SIZE])
{

	ap_int<16> gradx = Window[CO_OFFS][CO_OFFS + 1] - Window[CO_OFFS][CO_OFFS -1] ;
	ap_int<16> grady = Window[CO_OFFS + 1][CO_OFFS] - Window[CO_OFFS - 1][CO_OFFS] ;

	Grad_t GradOut ;

	GradOut.dIdx = gradx;
	GradOut.dIdy = grady;


	return GradOut;

}





