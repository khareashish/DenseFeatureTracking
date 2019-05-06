#include "ComputeResponse.hpp"




Mask_t GaussianKernel[FILTER_SIZE][FILTER_SIZE] ={\
			{0.0156,    0.0313,    0.0313,    0.0313,    0.0156},	\
			{0.0313,    0.0625,    0.0625,    0.0625,    0.0313},	\
			{0.0313,    0.0625,    0.0625,    0.0625,    0.0313},	\
			{0.0313,    0.0625,    0.0625,    0.0625,    0.0313},	\
			{0.0156,    0.0313,    0.0313,    0.0313,    0.0156}	};


void ComputeGradientsHLS( 	hls::stream< Pixel_t > &pSrcImage,
							hls::stream< Grad_t  > &pGrad
						)
{


	Pixel_t WindowSrc[CO_SIZE][CO_SIZE];							// Window is completely partitioned providing random access
#pragma HLS ARRAY_PARTITION variable=WindowSrc complete dim=0

	Pixel_t SrcImageBuf[CO_SIZE][WIDTH];							// Image Buffer on which the kernel operates.
#pragma HLS ARRAY_PARTITION variable=SrcImageBuf complete dim=1

	Pixel_t SrcRightCol[CO_SIZE]; 									// right-most, incoming column
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
				PixelIn = pSrcImage.read();
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

				pGrad.write( OG_Data );

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





void ComputeABC(	hls::stream< Grad_t > &GradStream,
					hls::stream< int > &A_Stream,
					hls::stream< int > &B_Stream,
					hls::stream< int > &C_Stream)
{



L1:	for (int iRow = 0 ; iRow < HEIGHT ; iRow++ )
	{
#pragma HLS LATENCY min=0
	L2:	for(int iCol = 0 ; iCol < WIDTH ; iCol++ )
		{

#pragma HLS PIPELINE

			Grad_t GradIn = GradStream.read();

			ap_int<16> dIdx = GradIn.dIdx;
			ap_int<16> dIdy = GradIn.dIdy;


			int A = dIdx*dIdx ;
			int B = dIdy*dIdy ;
			int C = dIdx*dIdy ;


			A_Stream.write(A);
			B_Stream.write(B);
			C_Stream.write(C);
		}
	}

}


void SmoothABC(	hls::stream< int > &A_Stream,
				hls::stream< int > &B_Stream,
				hls::stream< int > &C_Stream,
				hls::stream< Fixed_t > &A_SmoothStream,
				hls::stream< Fixed_t > &B_SmoothStream,
				hls::stream< Fixed_t > &C_SmoothStream)
{



#pragma HLS ARRAY_PARTITION variable=GaussianKernel complete dim=0


	Pixel_t windowA[FILTER_SIZE][FILTER_SIZE];							// Sliding window
#pragma HLS ARRAY_PARTITION variable=windowA complete dim=0

	Pixel_t windowB[FILTER_SIZE][FILTER_SIZE];							// Sliding window for gaussian1
#pragma HLS ARRAY_PARTITION variable=windowB complete dim=0

	Pixel_t windowC[FILTER_SIZE][FILTER_SIZE];							// Sliding window for gaussian2
#pragma HLS ARRAY_PARTITION variable=windowC complete dim=0

	Pixel_t srcImageBufA[FILTER_SIZE][WIDTH];							// Image Buffer on which the kernel operates.
#pragma HLS ARRAY_PARTITION variable=srcImageBufA complete dim=1		// Static initializes buffer with zeros

	Pixel_t srcImageBufB[FILTER_SIZE][WIDTH];							// Image Buffer on which the kernel operates.
#pragma HLS ARRAY_PARTITION variable=srcImageBufB complete dim=1		// Static initializes buffer with zeros

	Pixel_t srcImageBufC[FILTER_SIZE][WIDTH];							// Image Buffer on which the kernel operates.
#pragma HLS ARRAY_PARTITION variable=srcImageBufC complete dim=1		// Static initializes buffer with zeros

	Pixel_t right_colA[FILTER_SIZE]; 									// right-most, incoming column
#pragma HLS ARRAY_PARTITION variable=right_colA dim=0

	Pixel_t right_colB[FILTER_SIZE]; 									// right-most, incoming column
#pragma HLS ARRAY_PARTITION variable=right_colB dim=0

	Pixel_t right_colC[FILTER_SIZE]; 									// right-most, incoming column
#pragma HLS ARRAY_PARTITION variable=right_colC dim=0




L1:	for (int iRow = 0 ; iRow < HEIGHT + FILTER_OFFS; iRow++ )
	{
#pragma HLS LATENCY min=0
	L2:	for(int iCol = 0 ; iCol < WIDTH + FILTER_OFFS; iCol++ )
		{
#pragma HLS PIPELINE


			Pixel_t pixelInA, pixelInB, pixelInC;						// Pixel data coming in
			if(iRow<HEIGHT && iCol < WIDTH)
			{
				pixelInA = A_Stream.read();
				pixelInB = B_Stream.read();
				pixelInC = C_Stream.read();
			}
			else
			{
				pixelInA = 0 ;
				pixelInB = 0 ;
				pixelInC = 0 ;
			}

			// Shift image buffer and get new line in
			if(iCol < WIDTH)
			{
				for(unsigned char ii = 0; ii < FILTER_SIZE-1; ii++)
				{
					right_colA[ii] = srcImageBufA[ii][iCol] = srcImageBufA[ii+1][iCol];
					right_colB[ii] = srcImageBufB[ii][iCol] = srcImageBufB[ii+1][iCol];
					right_colC[ii] = srcImageBufC[ii][iCol] = srcImageBufC[ii+1][iCol];
				}
				right_colA[FILTER_SIZE-1] = srcImageBufA[FILTER_SIZE-1][iCol] = pixelInA;
				right_colB[FILTER_SIZE-1] = srcImageBufB[FILTER_SIZE-1][iCol] = pixelInB;
				right_colC[FILTER_SIZE-1] = srcImageBufC[FILTER_SIZE-1][iCol] = pixelInC;
			}
			else
			{
				for(unsigned char ii = 0; ii < FILTER_SIZE; ii++)
				{
					right_colA[ii]=0;
					right_colB[ii]=0;
					right_colC[ii]=0;
				}
			}


			//sliding window:
			//Shift from left to right the sliding window to make room for the new column
			for(unsigned char ii = 0; ii < FILTER_SIZE; ii++)
			{
				for(unsigned char jj = 0; jj < FILTER_SIZE-1; jj++)
				{
					windowA[ii][jj] = windowA[ii][jj+1];
					windowB[ii][jj] = windowB[ii][jj+1];
					windowC[ii][jj] = windowC[ii][jj+1];
				}
			}
			for(unsigned char ii = 0; ii < FILTER_SIZE; ii++)
			{
				windowA[ii][FILTER_SIZE-1] = right_colA[ii];
				windowB[ii][FILTER_SIZE-1] = right_colB[ii];
				windowC[ii][FILTER_SIZE-1] = right_colC[ii];
			}



// operate on the window and output the result on the output stream

			Fixed_t filtOutA, filtOutB, filtOutC ;
			if ( (iRow>=FILTER_OFFS) && (iCol>=FILTER_OFFS)  )
			{


				filtOutA = FilterKernelOperator(windowA);

				filtOutB = FilterKernelOperator(windowB);

				filtOutC = FilterKernelOperator(windowC);


				A_SmoothStream.write(filtOutA);
				B_SmoothStream.write(filtOutB);
				C_SmoothStream.write(filtOutC);

			}
		}//end of L1
	}//end of L2

}



Fixed_t FilterKernelOperator(Pixel_t Window[FILTER_SIZE][FILTER_SIZE])
{

	Fixed_t value = 0 ;				// <32,17> provides better precision than <16,9>

	// Pipeline directive not required as the parent loop is pipelined
	for(int iWinRow = 0 ; iWinRow < FILTER_SIZE; iWinRow++)
	{
		for(int iWinCol = 0 ; iWinCol< FILTER_SIZE; iWinCol++)
		{
				value += Window[iWinRow][iWinCol]*GaussianKernel[iWinCol][iWinRow] ;
		}
	}

	return value;


}





void ComputeResponseHLS(	hls::stream< Fixed_t > &A_SmoothStream,
							hls::stream< Fixed_t > &B_SmoothStream,
							hls::stream< Fixed_t > &C_SmoothStream,
							hls::stream< Fixed_t > &ResponseStream)
{


	for(int i = 0 ; i < WIDTH*HEIGHT; i++)
	{

#pragma HLS PIPELINE

		float A, B, C ;					// all of these are going to be positive


		A = A_SmoothStream.read();

		B = B_SmoothStream.read();

		C = C_SmoothStream.read();

		float C2 = C*C;

		float DiffOfAB2 = (A-B)*(A-B);

		float sqrtTerm = sqrt(DiffOfAB2 + 4*C2);


		float tmpResponse ;

		tmpResponse = A + B - sqrtTerm;


		Fixed_t ResponseOut = tmpResponse ;

		ResponseStream.write( ResponseOut) ;

	}

}



// ================================== Wrapper for Compute Response Function ============================================ //



#pragma SDS data access_pattern(pSrcImage:SEQUENTIAL, pResponse:SEQUENTIAL)
#pragma SDS data copy( pSrcImage[0:WIDTH*HEIGHT], pResponse[0:WIDTH*HEIGHT])
void compute_response(	Pixel_t* pSrcImage,
						Fixed_t* pResponse
						)
{

#pragma HLS INLINE OFF
#pragma HLS DATAFLOW

	hls::stream< Pixel_t > srcImageStream;

	hls::stream< Grad_t > GradientStream;

	hls::stream< int > A_Stream ;
	hls::stream< int > B_Stream ;
	hls::stream< int > C_Stream ;

	hls::stream< Fixed_t > A_SmoothStream;
	hls::stream< Fixed_t > B_SmoothStream;
	hls::stream< Fixed_t > C_SmoothStream;


	hls::stream< Fixed_t > ResponseStream;

	// populate input image stream
	for(int i=0; i<HEIGHT*WIDTH;i++)
	{
		#pragma HLS LOOP_FLATTEN off
		#pragma HLS PIPELINE

		Pixel_t PixelIn = pSrcImage[i];
		srcImageStream.write( PixelIn );
	}



ComputeGradientsHLS(srcImageStream, GradientStream);

ComputeABC(GradientStream, A_Stream, B_Stream, C_Stream);

SmoothABC(A_Stream, B_Stream, C_Stream, A_SmoothStream, B_SmoothStream, C_SmoothStream );

ComputeResponseHLS(A_SmoothStream, B_SmoothStream, C_SmoothStream, ResponseStream);



	for(int i=0; i<HEIGHT*WIDTH;i++)
	{
		#pragma HLS LOOP_FLATTEN off
		#pragma HLS PIPELINE

		pResponse[i] = ResponseStream.read();
	}



}
