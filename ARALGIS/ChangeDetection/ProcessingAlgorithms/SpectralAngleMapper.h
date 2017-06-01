#pragma once

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

#include "Settings.h"  
#include "ThresholdSAM.h"  
#include "RosinThreshold.h"  




class SpectralAngleMapper
{

private:

public:

	SpectralAngleMapper() {}


	void SpectralAngleMapperCalculator(cv::Mat& srcImg, cv::Mat& testImg, cv::Mat* SAMMatOutput)
	{
		cv::Mat srcImgHSV;
		cv::Mat testImgHSV; 

		cv::cvtColor(srcImg, srcImgHSV, cv::COLOR_RGB2HSV);
		cv::cvtColor(testImg, testImgHSV, cv::COLOR_RGB2HSV);

		//cv::Mat srcPixel = cv::Mat::zeros(3, 1, CV_8UC1);
		//cv::Mat testPixel = cv::Mat::zeros(3, 1, CV_8UC1);

		//cv::Mat srcPixel = cv::Mat::zeros(12, 1, CV_8UC1);
		//cv::Mat testPixel = cv::Mat::zeros(12, 1, CV_8UC1);

		cv::Mat srcPixel = cv::Mat::zeros(9, 1, CV_8UC1);
		cv::Mat testPixel = cv::Mat::zeros(9, 1, CV_8UC1);

		cv::Mat SAMMat = cv::Mat::zeros(srcImg.rows, srcImg.cols, CV_64FC1);

		int numChannel = srcImgHSV.channels();

		//FILE *fp;
		//fopen_s(&fp, "test.txt", "w+");


		for (int i = 0; i<srcImgHSV.rows; i++)
		{
			uchar* _srcImg = srcImg.ptr<uchar>(i);
			uchar* _testImg = testImg.ptr<uchar>(i);

			uchar* _srcImgHSV = srcImgHSV.ptr<uchar>(i);
			uchar* _testImgHSV = testImgHSV.ptr<uchar>(i);

			double* _samMat = SAMMat.ptr<double>(i);

			for (int j = 0; j < srcImgHSV.cols; j++)
			{
				uchar* _srcPixel = srcPixel.ptr<uchar>(0);
				uchar* _testPixel = testPixel.ptr<uchar>(0);

				uchar* _srcPixelHSV = srcPixel.ptr<uchar>(0);
				uchar* _testPixelHSV = testPixel.ptr<uchar>(0);

				for (int k = 0; k < numChannel; k++)
				{
					_srcPixel[k] = (uchar)*_srcImg++;
					_testPixel[k] = (uchar)*_testImg++;

					//_srcPixel[k] = (uchar)0;
					//_testPixel[k] = (uchar)0;

					//_srcPixel[k+3] = (uchar)*_srcImgHSV++;
					//_testPixel[k+3] = (uchar)*_testImgHSV++;

					//_srcPixelHSV[k] = (uchar)*_srcImgHSV++;
					//_testPixelHSV[k] = (uchar)*_testImgHSV++;
				}

				int iFeatIndex = 6;

				// normalized RGB
				int dTotalSrc = _srcPixel[0] + _srcPixel[1] + _srcPixel[2];
				int dTotalTest = _testPixel[0] + _testPixel[1] + _testPixel[2];
				bool bSrcOK = true;
				bool bTestOK = true;

				//dTotalSrc = 0; dTotalTest = 0; ////debug code, delete it

				if (dTotalSrc == 0)
				{
					_srcPixel[iFeatIndex] = 0;
					_srcPixel[iFeatIndex+1] = 0;
					_srcPixel[iFeatIndex+2] = 0;

					bSrcOK = false;
				}

				if (dTotalTest == 0)
				{
					_testPixel[iFeatIndex] = 0;
					_testPixel[iFeatIndex+1] = 0;
					_testPixel[iFeatIndex+2] = 0;

					bTestOK = false;
				}

				float fTmp;

				for (int k = 0; k < numChannel; k++)
				{
					if (bSrcOK == true)
					{
						fTmp = (float)_srcPixel[k] / (float)dTotalSrc;
						_srcPixel[k + iFeatIndex] = (int)((fTmp * 255.0) + 0.5);

						//_srcPixel[k] = 0;
					}

					if (bTestOK == true)
					{
						fTmp = (float)_testPixel[k] / (float)dTotalTest;
						_testPixel[k + iFeatIndex] = (int)((fTmp * 255.0) + 0.5);

						//_testPixel[k] = 0;
					}
				}
				// end of normalized RGB 

				////// calculate Change Vectors in 2-D
				////// these are: RG, RB, GB
				//cv::Mat srcSpec = cv::Mat(2, 1, CV_8U);
				//cv::Mat testSpec = cv::Mat(2, 1, CV_8U);

				//uchar* _srcSpec = srcSpec.ptr<uchar>(0);
				//uchar* _testSpec = testSpec.ptr<uchar>(0);


				//_srcSpec[0] = _srcPixel[0];
				//_srcSpec[1] = _srcPixel[1];

				//_testSpec[0] = _testPixel[0];
				//_testSpec[1] = _testPixel[1];

				//double dNorm = cv::norm(srcSpec, testSpec, cv::NORM_L2);
				////// End of CVA



				/////// differential values starts
				//_srcPixel[6] = (_srcPixel[2] - _srcPixel[1]);
				//_srcPixel[7] = (_srcPixel[2] - _srcPixel[0]);
				//_srcPixel[8] = (_srcPixel[1] - _srcPixel[0]);

				//_testPixel[6] = (_testPixel[2] - _testPixel[1]);
				//_testPixel[7] = (_testPixel[2] - _testPixel[0]);
				//_testPixel[8] = (_testPixel[1] - _testPixel[0]);

				//_srcPixel[9] = (_srcPixel[5] - _srcPixel[4]);
				//_srcPixel[10] = (_srcPixel[5] - _srcPixel[3]);
				//_srcPixel[11] = (_srcPixel[4] - _srcPixel[3]);

				//_testPixel[9] = (_testPixel[5] - _testPixel[4]);
				//_testPixel[10] = (_testPixel[5] - _testPixel[3]);
				//_testPixel[11] = (_testPixel[4] - _testPixel[3]);
				//////// differential values ends

				int tmp1 = (int)srcPixel.dot(srcPixel);
				int tmp2 = (int)testPixel.dot(testPixel);
				int tmp3 = (int)srcPixel.dot(testPixel);

				double TMPP = sqrt((double)tmp1*(double)tmp2);
				double dSAM = 0.0;

				if ((tmp1 == 0) || (tmp2 == 0) || (tmp3 == 0))
				{
					_samMat[j] = 0.0;
					//printf("\nA%.3f %.3f %.3f %.3f %.3f ", tmp1, tmp2, tmp3, dSAM, _samMat[i]);
				}
				else
				{
					dSAM = (double)tmp3 / (TMPP);
					_samMat[j] = acos(dSAM) * 180.0 / PI;
					if ((dSAM > 1.0) && (dSAM < -1.0))
					{
						printf("\n FFF %.9f ", dSAM);
					}

					if ((_samMat[j] > 180.0) && (_samMat[j] < 0.0))
					{
						printf("\n FFF %.9f ", _samMat[j]);
					}


					//printf("\nB%.3f %.3f %.3f %.3f %.3f ", tmp1, tmp2, tmp3, dSAM, _samMat[i]);
				}
				//fprintf(fp, "\n%d  %d  %d %d %d   %.5f", i,j, tmp1, tmp2, tmp3, _samMat[j]);


				//printf("%.3f %.3f ", dSAM, _samMat[i]);

			}
		}

		double mind, maxd;

		cv::minMaxLoc(SAMMat, &mind, &maxd);

		//printf("\n\n MIN %.5f MAX %.5f \n", mind, maxd);

		double KKK = 254.0 / maxd;

		cv::Mat imgToThreshold = cv::Mat::zeros(srcImg.rows, srcImg.cols, CV_8UC1);

		for (int i = 0; i < srcImg.rows; i++)
		{
			uchar* _srcImg = imgToThreshold.ptr<uchar>(i);
			double* _samMat = SAMMat.ptr<double>(i);

			for (int j = 0; j < srcImg.cols; j++)
			{
				//_srcImg[j] = uchar(_samMat[j] + double(254.0 - maxd) + 0.5);
				_srcImg[j] = uchar(_samMat[j]*(KKK) + 0.5);

			}
		}


		//cv::imwrite("diff-RGB-HSV-NRGB100.jpeg", imgToThreshold);

#ifdef DISPLAY_SAM
		double mini, maxi;
		cv::minMaxLoc(imgToThreshold, &mini, &maxi);
		printf("\n\n MIN %.5f MAX %.5f \n", mini, maxi);

		cv::namedWindow("SAM IMAGE", cv::WINDOW_NORMAL);
		cv::imshow("SAM IMAGE", imgToThreshold);
		cv::waitKey(0);
#endif

		ThresholdSAM SAMThresholder;
		SAMThresholder.thresholdImage(imgToThreshold);

		RosinThreshold SAMRosinThresholder;
		SAMRosinThresholder.ComputeRosinThreshold(imgToThreshold, SAMMatOutput);
	}


};
