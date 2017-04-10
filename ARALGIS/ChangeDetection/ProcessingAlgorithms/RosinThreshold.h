#pragma once
/* threshold and image by calculating the histogram and finding the bend
* using the point of maximum deviation from the fitted straight line
*
* start the line either:
*     at the beginning of the histogram
*     or at the largest peak (default)
*
* can delete 1st bin
*     useful if it is more than an order of magnitude larger than all other bins
*     e.g. edge maps
*
* can use cumulative histogram
*
* can input just the histogram
*
* Paul Rosin
* Brunel University
* September 1998
*
* March 2005 - made -p option default
* May 2015 - added -h option - useful if there is a central unimodal peak such as optical flow example in paper
*
* the method is described in my paper:
* P.L. Rosin, "Unimodal thresholding",
* Pattern Recognition, vol. 34, no. 11, pp. 2083-2096, 2001.
*
* --------------------------------------------------------------------------------
*
* NOT yet updated to include dynamic selection of bin size
*
* if the histogram has gaps, e.g. due to intensity stretching or equalisation, this
* can cause the algorithm to fail
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef FALSE
# define FALSE 0
# define TRUE (!FALSE)
#endif

#define SQR(x)   ((x) * (x))
#define ABS(x)   (((x)<0.0) ? -(x): (x))

#define MAX_SIZE 5000
#define MAX_HIST 256

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

cv::Mat imageTrackbar;
const int thresholdMax = 254;
int thresholdSelected;

cv::Mat imageTrackbarAdapt;
int windowSize = 3;

class RosinThreshold
{
public:
	RosinThreshold() {};

	int hist[MAX_HIST];
	int height, width, depth;
	int divide = 1;
	int delete_bin = -1;

	int iCounter = 0;



	int calculateThreshold(cv::Mat& input)
	{
		int i, pk, pk2, fi, st, d1, d2, thresh;
		int tmp;

		int big_peak = TRUE;
		int start_one = FALSE;
		int smooth = FALSE;
		int cumulative = FALSE;
		int deleteBora = FALSE;
		int delete_first = FALSE;
		int do_invert = FALSE;
		double ratio;
		int suppress_invert = FALSE;

		deleteBora = FALSE; // FALSE
		cumulative = FALSE;
		smooth = FALSE;
		delete_first = TRUE;  // FALSE
		delete_bin = -1;  // -1
		start_one = TRUE;
		big_peak = FALSE;
		big_peak = TRUE;
		suppress_invert = TRUE;
		divide = 1;

#ifdef DISPLAY_ROSIN
		funcThresholdImage(input);
#endif

		for (i = 0; i < MAX_HIST; i++)
		{
			hist[i] = 0;
		}

		int width = input.cols;
		int height = input.rows;;

		if (width*height < 256 * 256)
		{
			printf("WARNING: small image\n");
			printf("++++++++ dynamic selection of bin width has not yet been implemented\n");
		}

		long int N = input.cols * input.rows;
		i = 0;
		uchar *in = &(input.at<uchar>(0, 0)); // Image value
		
		// Iterate image
		for (i = 0; i < N; i++)
		{
			int value = (int)in[i];
			hist[value] = hist[value] + 1;
		}

		iCounter++; //1
		//drawHistogram(hist);

		if (smooth)
		{
			for (i = 0; i < MAX_HIST - 1; i++)
			{
				hist[i] = (hist[i] + hist[i + 1]);
			}
		}

		iCounter++; //2
		//drawHistogram(hist);

		if (delete_first)
		{
			i = 0;
			while (hist[i] == 0)
				i++;
			hist[i] = 0;
		}

		iCounter++; //3
		//drawHistogram(hist);

		if (delete_bin != -1)
		{
			hist[delete_bin] = 0;
		}

		iCounter++; //4
		//drawHistogram(hist);

		if (deleteBora)
		{
			pk = find_start(hist, MAX_HIST);
			hist[pk] = 0;
			printf("deleting largest bin: %d\n", pk);
		}

		iCounter++; //5
		//drawHistogram(hist);


		/* check to see if the peak is closest to the zero end of the histogram;
		* if not the image (or actually its histogram) is inverted to make it so
		*/
		pk = find_start(hist, MAX_HIST);
		st = find_first(hist, MAX_HIST);
		fi = find_end(hist, MAX_HIST);
		d1 = pk - st;
		d2 = fi - pk;
		if ((d1 < 0) || (d2 < 0))
		{
			fprintf(stderr, "ERROR: histogram peak in strange location\n");
			printf("ST %d  PK %d  FI %d\n", st, pk, fi);
			printf("D %d %d\n", d1, d2);
			exit(-1);
		}

		if (!suppress_invert)
			if (d1 > d2)
			{
				do_invert = TRUE;
				printf("inverting histogram\n");
			}

		/* invert image - actually just invert histogram */
		if (do_invert)
		{
			for (i = 0; i < MAX_HIST / 2; i++)
			{
				tmp = hist[i];
				hist[i] = hist[MAX_HIST - 1 - i];
				hist[MAX_HIST - 1 - i] = tmp;
			}
		}

		pk2 = find_second(hist, MAX_HIST, pk);

		/// boraN bug fix kasým 2016 begin
		//ratio = hist[pk] / hist[pk2];
		if (hist[pk2] != 0)
		{
			ratio = hist[pk] / hist[pk2];
		}
		else
		{
			ratio = 11;
		}
		/// boraN bug fix kasým 2016 end


		if (ratio > 10)
		{
			printf("WARNING: ratio of largest to second largest histogram bin = %f\n", ratio);
			printf("++++++++ maybe you should delete the largest histogram bin using the -D option\n");
		}

		if (big_peak)
		{
			st = find_start(hist, MAX_HIST);
		}
		else if (start_one)
		{
			st = 1;
		}
		else
		{
			st = 0;
		}

		printf("starting from peak at position %d\n", st);

		if (cumulative)
		{
			for (i = MAX_HIST - 2; i >= 0; i--)
			{
				hist[i] += hist[i + 1];
			}
		}

		thresh = find_corner2(hist, st, MAX_HIST);

		/* invert threshold back again */
		if (do_invert)
		{
			thresh = 255 - thresh;
		}

		printf("ROSIN thresholding at %d (= %d)\n", thresh, thresh*divide);

		return thresh;
	}

	void drawHistogram(int b_hist[])
	{
		// Draw the histograms for B, G and R
		int hist_w = 512;
		int hist_h = 400;

		int bin_w = cvRound((double)hist_w / MAX_HIST);

		cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

		//cv::Mat histoCV(1, MAX_HIST, CV_32S, cv::Scalar(0, 0, 0));

		//int *out = &(histoCV.at<int>(0, 0)); // histogram value

		float iTotal = 0;
		for (int i = 0; i < MAX_HIST; i++)
		{
			iTotal += (float)b_hist[i];

		}

		float b_histT[MAX_HIST];

		for (int i = 0; i < MAX_HIST; i++)
		{
			b_histT[i] = (float)b_hist[i] / iTotal ;

		}

		//for (int i = 0; i < MAX_HIST; i++)
		//{
		//	out[i] = (int)b_hist[i];
		//	printf("\n HIST %d %d %d", i, out[i], (int)b_hist[i]);

		//}

		//cv::Mat histoCVV;

		/// Normalize the result to [ 0, histImage.rows ]
		//cv::normalize(histoCV, histoCVV, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

		//float *out1 = &(histoCVV.at<float>(0, 0)); // histogram value

		// Iterate image

		//printf("\n\n");
		//for (int i = 0; i < MAX_HIST; i++)
		//{
		//	printf("\n HIST %d %f", i, out1[i]);

		//}

		/// Draw for each channel
		for (int i = 1; i < MAX_HIST; i++)
		{
			line(histImage, cv::Point(bin_w*(i - 1), hist_h - cvRound(250+b_histT[i-1]*300)),
				cv::Point(bin_w*(i), hist_h - cvRound(250+b_histT[i]*300)),
				cv::Scalar(255, 0, 0), 2, 8, 0);

			//line(histImage, cv::Point(bin_w*(i - 1), hist_h - cvRound(histoCV.at<float>(i - 1))),
			//	cv::Point(bin_w*(i), hist_h - cvRound(histoCV.at<float>(i))),
			//	cv::Scalar(255, 0, 0), 2, 8, 0);
		}

		/// Display
		char title[1000];
		char sNum[20];
		_itoa_s(iCounter, sNum, sizeof(sNum), 10);
		strcpy_s(title, sNum);
		strcat_s(title, " calcHist Demo");
		
		cv::namedWindow(title, cv::WINDOW_NORMAL);
		cv::imshow(title, histImage);

		cv::waitKey(0);

	}

	void computeRosinSegmentation(cv::Mat& input, int threshold, int do_invert, cv::Mat& output)
	{
		long int N = input.cols * input.rows;

		printf("ROSIN Threshold is: %d\n", threshold);

		// Modify output image

		uchar *in = &(input.at<uchar>(0, 0)); // Image value
		uchar *out = &(output.at<uchar>(0, 0)); // Image value

		// Iterate image

		if (do_invert)
		{
			for (int i = 0; i < N; i++)
			{
				int value = (int)in[i];

				if (value < threshold)
				{
					out[i] = (uchar)255.0; //0.0
				}
				else
				{
					out[i] = (uchar) 0.0; //255.0
				}
			}
		}
		else
		{
			for (int i = 0; i < N; i++)
			{
				int value = (int)in[i];

				if (value > threshold)
				{
					out[i] = (uchar)255.0; // ö.0
				}
				else
				{
					out[i] = (uchar) 0.0; //255.0
				}
			}
		}
	}

	/* find largest peak */
	int find_start(int Y[], int no_pts)
	{
		int i, st, max;

		/* find largest peak */
		st = 0; max = Y[0];
		for (i = 1; i < no_pts; i++)
		{
			if (Y[i] > max)
			{
				max = Y[i];
				st = i;
			}
		}

		return(st);
	}

	/* find 2nd largest peak */
	int find_second(int Y[], int no_pts, int peak)
	{
		int i, st, max;

		/* find 2nd largest peak */
		st = max = -1;
		for (i = 0; i < no_pts; i++) 
		{
			if (i == peak) continue;

			if (Y[i] > max) 
			{
				max = Y[i];
				st = i;
			}
		}

		return(st);
	}

	/* find first non-zero bin */
	int find_first(int Y[], int no_pts)
	{
		int i, st;

		st = 0;
		for (i = 0; i < no_pts; i++)
			if (Y[i] > 0) {
				st = i;
				break;
			}

		return(st);
	}

	/* find last non-zero bin */
	int find_end(int Y[], int no_pts)
	{
		int i, fi;

		fi = 0;
		for (i = 1; i < no_pts; i++)
		{
			if (Y[i] > 0)
			{
				fi = i;
			}
		}

		return(fi);
	}

	/* find `corner' in curve - simple version */
	int find_corner2(int Y[], int st, int no_pts)
	{
		int X[MAX_HIST];
		int i;
		float dist;
		float max_dist = -1;
		int thresh = -1;
		int end;
		double m, xi, yi, x1, y1, x2, y2;

		for (i = st; i < no_pts; i++)
		{
			X[i] = i;
		}

		end = no_pts - 1;

		while ((Y[end] == 0) && (end >= 0))
			end--;

		no_pts = end;

		if (end <= 0)
		{
			fprintf(stderr, "ERROR: empty histogram\n");
		}

		for (i = st; i <= no_pts; i++) 
		{
			dist = (float) ( (Y[st] - Y[no_pts - 1]) * X[i] -
				(X[st] - X[no_pts - 1]) * Y[i] -
				X[no_pts - 1] * Y[st] +
				X[st] * Y[no_pts - 1] );

			dist = SQR(dist) /
				(float)(SQR(X[st] - X[no_pts - 1]) + SQR(Y[st] - Y[no_pts - 1]));
			dist = ABS(dist);

			if (dist > max_dist) 
			{
				max_dist = dist;
				thresh = i;
			}
		}

		/* find intersection point of perpendicular - just for display */
		m = (Y[st] - Y[no_pts - 1]) / (X[st] - X[no_pts - 1]);
		x1 = X[thresh]; y1 = Y[thresh];
		x2 = X[st]; y2 = Y[st];
		xi = (x1 + m*(m*x2 + y1 - y2)) / (1 + SQR(m));
		yi = (m*x1 - m*x2 + SQR(m)*y1 + y2) / (1 + SQR(m));
		printf("intersection point %f %f\n", xi, yi);

		return thresh;
	}

	

	void ComputeRosinThreshold(cv::Mat& inputImg)
	{

		// Input image properties
		//int Nx = inputImg.cols;
		//int Ny = inputImg.rows;
		//long int N = Nx * Ny;

		cv::Mat thresholdedImg;

		inputImg.copyTo(thresholdedImg);

		int threshold = calculateThreshold(inputImg);

		computeRosinSegmentation(inputImg, threshold, false, thresholdedImg);


#ifdef  DISPLAY_IMAGES_THRESHOLD
		cv::namedWindow("ROSIN THRESHOLDED SAM", cv::WINDOW_NORMAL);
		cv::imshow("ROSIN THRESHOLDED SAM", thresholdedImg);
		cv::waitKey(0);
#endif

		int open_size = OPEN_SIZE;
		cv::Mat elementOpen = cv::getStructuringElement(cv::MORPH_RECT,
			cv::Size(2 * open_size + 1, 2 * open_size + 1),
			cv::Point(open_size, open_size));

		cv::Mat dst;

		// Apply erosion or dilation on the image
		cv::morphologyEx(thresholdedImg, dst, cv::MORPH_OPEN, elementOpen);

		//int close_size = CLOSE_SIZE;
		//cv::Mat elementClose = cv::getStructuringElement(cv::MORPH_RECT,
		//	cv::Size(2 * close_size + 1, 2 * close_size + 1),
		//	cv::Point(close_size, close_size));

		//cv::Mat dstTmp;
		//cv::morphologyEx(thresholdedImg, dstTmp, cv::MORPH_OPEN, elementOpen);
		//cv::morphologyEx(dstTmp, dst, cv::MORPH_CLOSE, elementClose);

#ifdef  DISPLAY_IMAGES_THRESHOLD
		cv::namedWindow("ROSIN THRESHOLDED SAM OPENED", cv::WINDOW_NORMAL);
		cv::imshow("ROSIN THRESHOLDED SAM OPENED", dst);
		cv::waitKey(0);
#endif
	}

#ifdef	DISPLAY_ROSIN
	static void onTrackbarThreshold(int, void*)
	{
		cv::Mat new_image = cv::Mat::zeros(imageTrackbar.size(), imageTrackbar.type());

		long int N = imageTrackbar.cols * imageTrackbar.rows;


		printf("Threshold is: %d\n", thresholdSelected);

		// Modify output image

		uchar *in = &(imageTrackbar.at<uchar>(0, 0)); // Image value
		uchar *out = &(new_image.at<uchar>(0, 0)); // Image value

		// Iterate image

			for (int i = 0; i < N; i++)
			{
				int value = (int)in[i];

				if (value < thresholdSelected)
				{
					out[i] = (uchar)0.0;
				}
				else
				{
					out[i] = (uchar) 255.0;
				}
			}

		imshow("Threshold Manual - New Image - Rosin", new_image);
	}


	int funcThresholdImage(cv::Mat& imageToThreshold)
	{

		/// Read image given by user
		imageToThreshold.copyTo(imageTrackbar);

		/// Initialize values
		thresholdSelected = 0;

		/// Create Windows
		cv::namedWindow("Threshold Manual - New Image - Rosin", cv::WINDOW_NORMAL);

		/// Create Trackbars
		cv::createTrackbar("Threshold", "Threshold Manual - New Image - Rosin", &thresholdSelected, thresholdMax, onTrackbarThreshold);

		/// Show some stuff
		cv::imshow("Threshold Manual - New Image - Rosin", imageTrackbar);

		/// Wait until user press some key
		cv::waitKey();
		return 0;
	}
#endif
};
