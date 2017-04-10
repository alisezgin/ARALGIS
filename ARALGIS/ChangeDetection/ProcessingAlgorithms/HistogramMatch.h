#pragma once

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

#include "Settings.h"  


class HistogramMatch
{

private:

public:

	HistogramMatch() {}


	// Match histograms of 'src' to that of 'dst'.
	cv::Mat histMatch(cv::Mat& srcImg, const cv::Mat& dstImg)
	{
		cv::Mat src_cdf = cv::Mat::zeros(1, 256, CV_64FC1);
		cv::Mat dst_cdf = cv::Mat::zeros(1, 256, CV_64FC1);

		cv::Mat Mv(1, 256, CV_8UC1);

		uchar* M = Mv.ptr();

		src_cdf.setTo(cv::Scalar(0));
		src_cdf.setTo(cv::Scalar(0));

		double* _src_cdf = (double *)src_cdf.ptr();
		double* _dst_cdf = (double *)dst_cdf.ptr();


		calcHistCDF(srcImg, _src_cdf);
		calcHistCDF(dstImg, _dst_cdf);

		uchar last = 0;
		double const HISTMATCH_EPSILON = 0.000001;

		for (int j = 0; j<src_cdf.cols; j++)
		{
			double F1j = _src_cdf[j];

			for (uchar k = last; k<dst_cdf.cols; k++)
			{
				double F2k = _dst_cdf[k];
				// Note: Two tests were combined into one for efficiency
				//if (abs(F2k - F1j) F1j) {
				if (F2k > F1j - HISTMATCH_EPSILON)
				{
					M[j] = k;
					last = k;
					break;
				}
			}
		}

		cv::Mat lut(1, 256, CV_8UC1, M);

		cv::Mat matchedImage;
		cv::LUT(dstImg, lut, matchedImage);

		return matchedImage;
	}

	// Compute histogram and CDF for an image 
	void calcHistCDF(const cv::Mat& _i, double* cdf)
	{
		double h[256];
		for (int i = 0; i < 256; i++)
			h[i] = 0;

		cv::Mat _t = _i.reshape(1, 1);  // make 1 dim array

		uchar c;

		int numCols = _t.cols;

		for (int p = 0; p<numCols; p++)
		{
			c = _t.at<uchar>(0, p); // Image value
			h[c] += 1.0; // Histogram
		}

		//normalize hist to a max value of 1.0
		cv::Mat _tmp(1, 256, CV_64FC1, h);

		double minVal, maxVal;
		cv::minMaxLoc(_tmp, &minVal, &maxVal);
		_tmp = _tmp / maxVal;

		// Calculate the Cumulative Distribution Function
		cdf[0] = h[0];
		for (int j = 1; j<256; j++)
		{
			cdf[j] = cdf[j - 1] + h[j];
		}

		//normalize CDF to a max value of 1.0
		_tmp.data = (uchar*)cdf; // Array of doubles, but gets a byte pointer.

		cv::minMaxLoc(_tmp, &minVal, &maxVal);
		_tmp = _tmp / maxVal;
	}
};