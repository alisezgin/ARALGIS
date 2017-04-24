#pragma once

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

#include "Settings.h"  

class MeanDifferenceCalculator
{
public:
	MeanDifferenceCalculator() {};

	void computeDifferenceMean(cv::Mat& inputRef, cv::Mat& inputTest, cv::Mat* output, int aKernelSize = 3)
	{
		/// Declare variables
		cv::Mat mRefMean, mTestMean;

		mRefMean = cv::Mat::zeros(inputRef.size(), cv::DataType<uchar>::type); // CV_8UC1 
		mTestMean = cv::Mat::zeros(inputRef.size(), cv::DataType<uchar>::type); // CV_8UC1 

		cv::Mat kernel;
		cv::Point anchor;
		double delta;
		int ddepth;

		/// Initialize arguments for the filter
		anchor = cv::Point(-1, -1);
		delta = 0;
		ddepth = -1; // CV_32F; // -1

		/// Update kernel size for a normalized box filter
		kernel = cv::Mat::ones(aKernelSize, aKernelSize, CV_32F) / (float)(aKernelSize*aKernelSize);


		// Apply filter
		filter2D(inputRef, mRefMean, ddepth, kernel, anchor, delta, cv::BORDER_DEFAULT);
		filter2D(inputTest, mTestMean, ddepth, kernel, anchor, delta, cv::BORDER_DEFAULT);


#ifdef  DISPLAY_IMAGES_MEAN_FILTER
		char* windowNameRef = "Mean Reference ";
		cv::namedWindow(windowNameRef, cv::WINDOW_NORMAL);
		cv::imshow(windowNameRef, mRefMean);

		char* windowNameTest = "Mean Test ";
		cv::namedWindow(windowNameTest, cv::WINDOW_NORMAL);
		cv::imshow(windowNameTest, mTestMean);

		cv::waitKey(0);
#endif  

		int nRows = inputRef.rows;
		int nCols = inputRef.cols;

		int dTemp = 0;
		uchar* dataPtr1;
		uchar* dataPtr2;
		uchar* dataPtr3;


		for (int i = 0; i < nRows; i++)
		{
			dataPtr1 = mRefMean.ptr<uchar>(i);
			dataPtr2 = mTestMean.ptr<uchar>(i);
			dataPtr3 = output->ptr<uchar>(i);


			for (int j = 0; j < nCols; j++)
			{
				dTemp = (uchar)dataPtr1[j] - (uchar)dataPtr2[j];


				if (dTemp > 255)
				{
					dataPtr3[j] = 255;
#ifdef  DISPLAY_PRINTS_DEBUG
					//TRACE("%d %.3f ", *itout, dTemp);
#endif
				}
				else if (dTemp < 0)
				{
					dataPtr3[j] = 0;
#ifdef  DISPLAY_PRINTS_DEBUG
					//TRACE("%d %.3f", *itout, dTemp);
#endif
				}
				else
				{
					dataPtr3[j] = (uchar)(dTemp + 0.5);
				}
			}
		}

#ifdef  DISPLAY_IMAGES_MEAN_FILTER
		cv::Mat dummy = inputRef - mRefMean + 77;
		char* dummyChar = "Original - Mean Reference ";
		cv::namedWindow(dummyChar, cv::WINDOW_NORMAL);
		cv::imshow(dummyChar, dummy);
		cv::waitKey(0);
#endif  

	}

	void computeRatioMean(cv::Mat& inputRef, cv::Mat& inputTest, cv::Mat* output, int aKernelSize = 3)
	{
		/// Declare variables
		cv::Mat mRefMean, mTestMean;

		mRefMean = cv::Mat::zeros(inputRef.size(), cv::DataType<uchar>::type); // CV_8UC1 
		mTestMean = cv::Mat::zeros(inputRef.size(), cv::DataType<uchar>::type); // CV_8UC1 

		cv::Mat kernel;
		cv::Point anchor;
		double delta;
		int ddepth;

		/// Initialize arguments for the filter
		anchor = cv::Point(-1, -1);
		delta = 0;
		ddepth = -1; // CV_32F; // -1

		/// Update kernel size for a normalized box filter
		kernel = cv::Mat::ones(aKernelSize, aKernelSize, CV_32F) / (float)(aKernelSize*aKernelSize);


		// Apply filter
		filter2D(inputRef, mRefMean, ddepth, kernel, anchor, delta, cv::BORDER_DEFAULT);
		filter2D(inputTest, mTestMean, ddepth, kernel, anchor, delta, cv::BORDER_DEFAULT);

		int nRows = inputRef.rows;
		int nCols = inputRef.cols;

		float dTemp1 = 0.0;
		float dTemp2 = 0.0;
		uchar dTemp;

		uchar* dataPtr1;
		uchar* dataPtr2;
		uchar* dataPtr3;


		for (int i = 0; i < nRows; i++)
		{
			dataPtr1 = mRefMean.ptr<uchar>(i);
			dataPtr2 = mTestMean.ptr<uchar>(i);
			dataPtr3 = output->ptr<uchar>(i);


			for (int j = 0; j < nCols; j++)
			{
				dTemp1 = (float)dataPtr1[j] / (float)dataPtr2[j];

				if (dTemp1 > 1.0)
				{
					dTemp2 = (float)dataPtr2[j] / (float)dataPtr1[j];
					dTemp = (uchar)((1.0 - dTemp2) * 255.0);
				}
				else
				{
					dTemp = (uchar)((1.0 - dTemp1) * 255.0);
				}



				if (dTemp > 255.0)
				{
					dataPtr3[j] = 255;
				}
				else if (dTemp < 0.0)
				{
					dataPtr3[j] = 0;
				}
				else
				{
					dataPtr3[j] = dTemp;
				}
			}
		}
	}


};




