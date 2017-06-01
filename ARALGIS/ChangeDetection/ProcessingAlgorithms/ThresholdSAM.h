#pragma once

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"   

#include "OtsuThreshold.h"
#include "KapurThreshold.h"
#include "Settings.h"  

//#ifndef SHARED_HANDLERS
//#include "ARALGIS.h"
//#endif


class ThresholdSAM
{

private:

public:

	ThresholdSAM() {}

	void thresholdImage(cv::Mat& imagetoThreshold)
	{
//		cv::Mat thresholdedImageCV;
//		cv::threshold(imagetoThreshold, thresholdedImageCV, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
//
//#ifdef  DISPLAY_IMAGES_THRESHOLD
//		cv::namedWindow("OTSU THRESHOLDED SAM", cv::WINDOW_NORMAL);
//		cv::imshow("OTSU THRESHOLDED SAM", thresholdedImageCV);
//		cv::waitKey(0);
//#endif

		KapurThreshold KapurthresholdProcessor;
		cv::Mat imageThresholdedKapur;
		imagetoThreshold.copyTo(imageThresholdedKapur);
		KapurthresholdProcessor.ComputeKapurThreshold(imagetoThreshold, imageThresholdedKapur);

#ifdef  DISPLAY_IMAGES_THRESHOLD
		cv::namedWindow("KAPUR THRESHOLDED SAM", cv::WINDOW_NORMAL);
		cv::imshow("KAPUR THRESHOLDED SAM", imageThresholdedKapur);
		cv::waitKey(0);
#endif

		int open_size = OPEN_SIZE;
		cv::Mat elementOpen = cv::getStructuringElement(cv::MORPH_RECT,
			cv::Size(2 * open_size + 1, 2 * open_size + 1),
			cv::Point(open_size, open_size));

		int close_size = CLOSE_SIZE;
		cv::Mat elementClose = cv::getStructuringElement(cv::MORPH_RECT,
			cv::Size(2 * close_size + 1, 2 * close_size + 1),
			cv::Point(close_size, close_size));

		cv::Mat dst;

		// Apply OPENING to image
		cv::morphologyEx(imageThresholdedKapur, dst, cv::MORPH_OPEN, elementOpen);

		//g_CVTestChangeDetect = dst.clone();

		//cv::Mat dstTmp;
		//cv::morphologyEx(imageThresholdedKapur, dstTmp, cv::MORPH_OPEN, elementOpen);
		//cv::morphologyEx(dstTmp, dst, cv::MORPH_CLOSE, elementClose);

#ifdef  DISPLAY_IMAGES_THRESHOLD_OPENED
		cv::namedWindow("KAPUR THRESHOLDED SAM OPENED", cv::WINDOW_NORMAL);
		cv::imshow("KAPUR THRESHOLDED SAM OPENED", dst);
		cv::waitKey(0);
#endif

		cv::Mat thresholdedImageCV2;
		OtsuThreshold OtsuthresholdProcessor;
		cv::Mat imageThresholdedOtsu;
		imagetoThreshold.copyTo(imageThresholdedOtsu);
		OtsuthresholdProcessor.ComputeOtsuThreshold(imageThresholdedOtsu, thresholdedImageCV2);

#ifdef  DISPLAY_IMAGES_THRESHOLD
		cv::namedWindow("OTSU THRESHOLDED SAM ", cv::WINDOW_NORMAL);
		cv::imshow("OTSU THRESHOLDED SAM ", thresholdedImageCV2);
		cv::waitKey(0);
#endif

		// Apply erosion or dilation on the image
		cv::Mat dstOtsuOpened;
		cv::morphologyEx(thresholdedImageCV2, dstOtsuOpened, cv::MORPH_OPEN, elementOpen);

		g_CVTestChangeDetect = dstOtsuOpened.clone();

		//cv::morphologyEx(thresholdedImageCV2, dstTmp, cv::MORPH_OPEN, elementOpen);
		//cv::morphologyEx(dstTmp, dstOtsuOpened, cv::MORPH_CLOSE, elementClose);


#ifdef  DISPLAY_IMAGES_THRESHOLD_OPENED
		cv::namedWindow("OTSU THRESHOLDED SAM OPENED", cv::WINDOW_NORMAL);
		cv::imshow("OTSU THRESHOLDED SAM OPENED", dstOtsuOpened);
		cv::waitKey(0);
#endif

	}
};