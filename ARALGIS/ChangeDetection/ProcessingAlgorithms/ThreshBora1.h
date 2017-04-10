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


class ThreshBora1
{

private:

public:

	ThreshBora1() {}

	void thresholdImage(cv::Mat& imagetoThreshold, cv::Mat* output, int thresholdMethod = 0)
	{
		cv::Mat thresholdedImageCV;
		cv::threshold(imagetoThreshold, thresholdedImageCV, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

#ifdef  DISPLAY_IMAGES_THRESHOLD
		cv::namedWindow("OTSU THRESHOLDED MEAN GREY DIFFEENCE", cv::WINDOW_NORMAL);
		cv::imshow("OTSU THRESHOLDED MEAN GREY DIFFEENCE", thresholdedImageCV);
		cv::waitKey(0);
#endif

		KapurThreshold KapurthresholdProcessor;
		cv::Mat imageThresholdedKapur;
		imagetoThreshold.copyTo(imageThresholdedKapur);
		KapurthresholdProcessor.ComputeKapurThreshold(imagetoThreshold, imageThresholdedKapur);

#ifdef  DISPLAY_IMAGES_THRESHOLD
		cv::namedWindow("KAPUR THRESHOLDED MEAN GREY DIFFEENCE", cv::WINDOW_NORMAL);
		cv::imshow("KAPUR THRESHOLDED MEAN GREY DIFFEENCE", imageThresholdedKapur);
		cv::waitKey(0);
#endif

		int open_size = OPEN_SIZE;
		cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,
			cv::Size(2 * open_size + 1, 2 * open_size + 1),
			cv::Point(open_size, open_size));

		cv::Mat dst;

		// Apply erosion or dilation on the image
		//cv::erode(imageThresholdedKapur, dst, element);  // dilate(image,dst,element);
		cv::morphologyEx(imageThresholdedKapur, dst, cv::MORPH_OPEN, element);

		*output = dst;

#ifdef  DISPLAY_IMAGES_THRESHOLD
		cv::namedWindow("KAPUR THRESHOLDED MEAN GREY DIFFEENCE OPENED", cv::WINDOW_NORMAL);
		cv::imshow("KAPUR THRESHOLDED MEAN GREY DIFFEENCE OPENED", dst);
		cv::waitKey(0);
#endif
	}
};