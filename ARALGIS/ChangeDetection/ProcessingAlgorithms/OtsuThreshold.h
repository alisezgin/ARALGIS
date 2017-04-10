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

class OtsuThreshold
{
public:
	OtsuThreshold() {};

	void computeHistogram(cv::Mat& input, float *hist)
	{
		long int N = input.cols * input.rows;
		int i = 0;

		// Initialize array
		for (i = 0; i <= 255; i++) hist[i] = 0;


		//float *in = &(input.at<float>(0, 0)); // Image value
		uchar *in = &(input.at<uchar>(0, 0)); // Image value


		// Iterate image
		for (i = 0; i < N; i++) {
			int value = (int)in[i];
			hist[value] = hist[value] + 1;
		}

		printf("Total # of pixels: %ld\n", N);

		// printf("Printing normalized histogram\n");
		// for (int i = 0; i <= 255; i++){
		//   // printf("%d\t%d\n", i, (int)(hist[i]*N));
		//   // Normalized histogram
		//   printf("%d\t%f\n", i, hist[i]);
		// }
	}

	void computeOtsusSegmentation(cv::Mat& input, float* hist, cv::Mat& output)
	{
		long int N = input.cols * input.rows;

		float sum = 0;
		for (int i = 0; i < 256; i++)
		{
			sum += i * ((int)hist[i]);
		}

		float sumB = 0;
		int q1 = 0;
		int q2 = 0;

		float varMax = 0;
		int threshold = 0;

		for (int i = 0; i < 256; i++)
		{
			q1 += (int)(hist[i] + 0.5);
			if (q1 == 0)
				continue;
			q2 = N - q1;

			if (q2 == 0)
				break;

			sumB += (float)(i * ((int)hist[i]));
			float m1 = sumB / q1;
			float m2 = (sum - sumB) / q2;

			float varBetween = (float)q1 * (float)q2 * (m1 - m2) * (m1 - m2);

			if (varBetween > varMax)
			{
				varMax = varBetween;
				threshold = i;
			}
		}


		printf("OTSU Threshold is: %d\n", threshold);

		// Modify output image

		uchar *in = &(input.at<uchar>(0, 0)); // Image value
		uchar *out = &(output.at<uchar>(0, 0)); // Image value

		// Iterate image
		for (int i = 0; i < N; i++)
		{
			int value = (int)in[i];

			if (value > threshold)
			{
				out[i] = (uchar)255.0; 
			}
			else
			{
				out[i] = (uchar) 0.0f; 
			}
		}
	}


	//! Otsus segmentation algorithm.
	/*!
	\param argc Number of arguments of the program
	\param **argv Arguments of the program
	*/
	void ComputeOtsuThreshold(cv::Mat& inputImg, cv::Mat& thresholdedImg)
	{

		// Input image properties
		int Nx = inputImg.cols;
		int Ny = inputImg.rows;
		long int N = Nx * Ny;

		inputImg.copyTo(thresholdedImg);

		float hist[256];
		computeHistogram(inputImg, hist);

		computeOtsusSegmentation(inputImg, hist, thresholdedImg);

	}
};
