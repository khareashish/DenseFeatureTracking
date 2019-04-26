#include "xf_headers.h"
#include "string.h"
#include "DFTParameters.hpp"

#include "ComputeGradients.hpp"
#include "ComputeResponse.hpp"
int main()
{


	Pixel_t *pSrcImage 	= (Pixel_t*)sds_alloc(HEIGHT*WIDTH*sizeof(Pixel_t) ) ;
	Grad_t 	*pGrad 		= (Grad_t*)sds_alloc(HEIGHT*WIDTH*sizeof(Grad_t) ) 	;
	Fixed_t	*pResponse	= (Fixed_t*)sds_alloc(HEIGHT*WIDTH*sizeof(Fixed_t))	;

	printf("Reading image..\n");

	cv::Mat mSrcImage = cv::imread("wheat.jpg", CV_LOAD_IMAGE_GRAYSCALE);


	for(int i = 0 ; i < WIDTH*HEIGHT; i++)
	{
		pSrcImage[i] = *(mSrcImage.data+i);
	}

	printf("computing gradients..\n");


	unsigned long clock_start, clock_end;
	clock_start = sds_clock_counter();

#pragma SDS async(1)
	compute_gradients(pSrcImage, pGrad);
#pragma SDS async(2)
	compute_response(pGrad, pResponse );
#pragma SDS wait(1)
#pragma SDS wait(2)

	clock_end = sds_clock_counter();
	printf("Time to compute Gradients = %f ms \n", (1000.0/sds_clock_frequency())*(double)(clock_end-clock_start));


	printf("Writing gradients to file..\n");

	cv::Mat  mGradX, mGradY, mResponse;

	mGradX.create(HEIGHT, WIDTH, CV_32S);
	mGradY.create(HEIGHT, WIDTH, CV_32S);
	mResponse.create(HEIGHT, WIDTH, CV_32F);

	for(int iRow =  0; iRow < HEIGHT ; iRow++)
	{
		for(int iCol = 0 ; iCol < WIDTH; iCol++ )
		{
			mGradX.at<int>(iRow, iCol) =  (int)(pGrad[iRow*WIDTH + iCol].dIdx);
			mGradY.at<int>(iRow, iCol) =  (int)(pGrad[iRow*WIDTH + iCol].dIdy);

			mResponse.at<float>(iRow, iCol) =  (float)(pResponse[iRow*WIDTH + iCol]);

		}
	}


	cv::FileStorage fs;

	fs.open("GradX.yml",  cv::FileStorage::WRITE);
	fs<<"GradX" << mGradX;

	fs.open("GradY.yml",  cv::FileStorage::WRITE);
	fs<<"GradY" << mGradY;

	fs.open("ShiTomasiResponse.yml",  cv::FileStorage::WRITE);
	fs<<"Response" << mResponse;

	fs.release();


return 0 ;
}
