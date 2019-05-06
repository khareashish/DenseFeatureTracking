#include "NonMaxSupression.hpp"



#pragma SDS data access_pattern(pResponse:SEQUENTIAL, pKeypointImage:SEQUENTIAL)
#pragma SDS data copy( pResponse[0:WIDTH*HEIGHT], pKeypointImage[0:WIDTH*HEIGHT])


void non_max_suppresion(	 Fixed_t* pResponse,
							 Pixel_t*  pKeypointImage )
{


	Fixed_t WindowSrc[SNM_SIZE][SNM_SIZE];						// Window is completely partitioned providing random access
#pragma HLS ARRAY_PARTITION variable=WindowSrc complete dim=0

	Fixed_t SrcImageBuf[SNM_SIZE][WIDTH];						// Image Buffer on which the kernel operates.
#pragma HLS ARRAY_PARTITION variable=SrcImageBuf complete dim=1

	Fixed_t SrcRightCol[SNM_SIZE]; 							// right-most, incoming column
#pragma HLS ARRAY_PARTITION variable=SrcRightCol dim=0

int idx = 0 ;

L1:	for (int iRow = 0 ; iRow < HEIGHT + SNM_OFFS; iRow++ )
	{
#pragma HLS LATENCY min=0
	L2:	for(int iCol = 0 ; iCol < WIDTH + SNM_OFFS; iCol++ )
		{
	#pragma HLS PIPELINE

			Fixed_t ResponseIn;

			if(iRow<HEIGHT && iCol < WIDTH)
			{
				ResponseIn = pResponse[iRow*WIDTH + iCol];
			}
			else
			{
				ResponseIn = 0 ;
			}


			// Shift image buffer and get new line in
			if(iCol < WIDTH)
			{
				for(unsigned char ii = 0; ii < SNM_SIZE - 1; ii++)
				{
					SrcRightCol[ii] = SrcImageBuf[ii][iCol]=SrcImageBuf[ii+1][iCol];
				}
				SrcRightCol[SNM_SIZE-1] = SrcImageBuf[SNM_SIZE-1][iCol] = ResponseIn;
			}
			else
			{
				for(unsigned char ii = 0; ii < SNM_SIZE; ii++)
				{
					SrcRightCol[ii] = 0 ;
				}
			}



			//Shift from left to right the sliding window to make room for the new column
			for(unsigned char ii = 0; ii < SNM_SIZE; ii++)
			{
				for(unsigned char jj = 0; jj < SNM_SIZE-1; jj++)
				{
					WindowSrc[ii][jj] = WindowSrc[ii][jj+1];					// scale 1
				}
			}

			for(unsigned char ii = 0; ii < SNM_SIZE; ii++)
			{
				WindowSrc[ii][SNM_SIZE-1] = SrcRightCol[ii];

			}


			// operate on the window and output the result on the output stream

			if ( (iRow >= SNM_OFFS) && (iCol >= SNM_OFFS)  )
			{

				Pixel_t PixelOut;

				bool bKeypointDetected = IsLocalExtremum(WindowSrc);

				if(bKeypointDetected)
					PixelOut = 255 ;
				else
					PixelOut = 0 ;

				pKeypointImage[idx] = PixelOut;

				idx++ ;
			}


		}//end of L2
	}//end of L1



}







bool IsLocalExtremum(Fixed_t window[SNM_SIZE][SNM_SIZE])
{

	Fixed_t centerPixel = window[SNM_OFFS][SNM_OFFS]; 		//	center pixel that needs to be compared to the neighbours

	bool local_maxima;
	bool local_minima;
	bool extrema;


	bool bMoreThanTH = centerPixel > RESP_TH;

	// look for maxima
	local_maxima =	(centerPixel>window[0][0])&& 	\
					(centerPixel>window[0][1])&& 	\
					(centerPixel>window[0][2])&& 	\
					(centerPixel>window[1][0])&& 	\
					(centerPixel>window[1][2])&& 	\
					(centerPixel>window[2][0])&& 	\
					(centerPixel>window[2][1])&&	\
					(centerPixel>window[2][2]) ;


	extrema = local_maxima && bMoreThanTH;

	return extrema ;

}

