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

class DifferenceCalculator
{
public:
	DifferenceCalculator() {};

	void computeDifference(cv::Mat& inputRef, cv::Mat& inputTest, cv::Mat& output)
	{
		output = inputRef - inputTest;

		cv::Mat_<uchar>::const_iterator itRef = inputRef.begin<uchar>();
		cv::Mat_<uchar>::const_iterator itRefend = inputRef.end<uchar>();

		cv::Mat_<uchar>::const_iterator itTest = inputTest.begin<uchar>();

		cv::Mat_<uchar>::iterator itout = output.begin<uchar>();

		double dTemp = 0.0;

		for (; itRef != itRefend; ++itRef)
		{

			dTemp = (double)*itRef - (double)*itTest;

			if (dTemp > 255.0)
			{
				*itout = 255;
#ifdef  DISPLAY_PRINTS_DEBUG
				//TRACE("%d %.3f ", *itout, dTemp);
#endif
			}
			else if (dTemp < 0.0)
			{
				*itout = 0;
#ifdef  DISPLAY_PRINTS_DEBUG
				//TRACE("%d %.3f", *itout, dTemp);
#endif
			}
			else
			{
				*itout = (uchar)(dTemp + 0.5);
			}

			++itTest;
		}
	}

	void computeRatio(cv::Mat& inputRef, cv::Mat& inputTest, cv::Mat* output)
	{
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
			dataPtr1 = inputRef.ptr<uchar>(i);
			dataPtr2 = inputTest.ptr<uchar>(i);
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