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


/**
* @file threshold_kapur.c
* Routines for thresholding (binarizing) a grayscale image
*/

#include "Histogram.h"


/**
* @brief Implements the Kapur-Sahoo-Wong (Maximum Entropy) thresholding method
*
* @param[in] img Image pointer { grayscale }
*
* @return Threshold value or INT_MIN
*
* @ref Kapur J.N., Sahoo P.K., and Wong A.K.C. (1985) "A New Method for
Gray-Level Picture Thresholding Using the Entropy of the Histogram"
Graphical Models and Image Processing, 29(3): 273-285
*
*/

class KapurThreshold
{
public:
	KapurThreshold() {};

	void ComputeKapurThreshold(cv::Mat& img, cv::Mat& thresholdedImg)
	{
		SET_FUNC_NAME("threshold_kapur");
		int ih, it;
		int threshold;
		int first_bin;			/* see below */
		int last_bin;			/* see below */
		double tot_ent;		/* total entropy */
		double max_ent;		/* max entropy */
		double ent_back;		/* entropy of the background pixels at a given threshold */
		double ent_obj;		/* entropy of the object pixels at a given threshold */
		double *data;			/* normalized histogram data */
		double *P1;			/* cumulative normalized histogram */
		double *P2;			/* see below */
		Histo *norm_histo;		/* normalized histogram */
		Histo *int_histo;		/* integer histogram */

		if (img.channels() != 1)
		{
			//ERROR_RET("Not a grayscale image !", INT_MIN);
		}

		int_histo = create_histo(img);
		/* Calculate the normalized histogram */
		norm_histo = normalize_histo(int_histo);
		if (IS_NULL(norm_histo))
		{
			//ERROR_RET("normalize_histo() failed !", INT_MIN);
		}

		data = (double *)get_histo_data(norm_histo);

		/* Calculate the cumulative normalized histogram */
		P1 = (double *)accumulate_histo(norm_histo);

		P2 = (double *)malloc(NUM_GRAY * sizeof(double));

		for (ih = 0; ih < NUM_GRAY; ih++)
		{
			P2[ih] = 1.0 - P1[ih];
		}

		/* Determine the first non-zero bin starting from the first bin */
		first_bin = 0;
		for (ih = 0; ih < NUM_GRAY; ih++)
		{
			if (!IS_ZERO(P1[ih]))
			{
				first_bin = ih;
				break;
			}
		}

		/* Determine the first non-one bin starting from the last bin */
		last_bin = MAX_GRAY;
		for (ih = MAX_GRAY; ih >= first_bin; ih--)
		{
			if (!IS_ZERO(P2[ih]))
			{
				last_bin = ih;
				break;
			}
		}

		/* Calculate the total entropy each gray-level and find the threshold that maximizes it */
		threshold = INT_MIN;
		max_ent = DBL_MIN;
		for (it = first_bin; it <= last_bin; it++)
		{
			/* Entropy of the background pixels */
			ent_back = 0.0;
			for (ih = 0; ih <= it; ih++)
			{
				if (!IS_ZERO(data[ih]))
				{
					ent_back -= (data[ih] / P1[it]) * log(data[ih] / P1[it]);
				}
			}

			/* Entropy of the object pixels */
			ent_obj = 0.0;
			for (ih = it + 1; ih < NUM_GRAY; ih++)
			{
				if (!IS_ZERO(data[ih]))
				{
					ent_obj -= (data[ih] / P2[it]) * log(data[ih] / P2[it]);
				}
			}

			/* Total entropy */
			tot_ent = ent_back + ent_obj;

			if (max_ent < tot_ent)
			{
				max_ent = tot_ent;
				threshold = it;
			}
		}

		free_histo(norm_histo);
		free(norm_histo);

		free_histo(int_histo);
		free(int_histo);
		free(P1);
		free(P2);

#ifdef DEBUG_PRINT_FINAL1
		DEBUG_PRINT("Kapur Threshold is: %d\n", threshold);
#endif
		computeKapursSegmentation(img, thresholdedImg, threshold);

	};


	void computeKapursSegmentation(cv::Mat& input, cv::Mat& output, int threshold)
	{
		long int N = input.cols * input.rows;
		
		// Modify output image

		uchar *in = &(input.at<uchar>(0, 0)); // Image value
		uchar *out = &(output.at<uchar>(0, 0)); // Image value

		// Iterate image
		for (int i = 0; i < N; i++)
		{
			int value = (int)in[i];

			if (value > threshold)
			{
				out[i] = (uchar) 255.0f;
			}
			else
			{
				out[i] = (uchar) 0.0f;
			}
		}
	};
};