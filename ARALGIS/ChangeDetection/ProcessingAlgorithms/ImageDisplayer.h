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


#include "Settings.h"  


class ImageDisplayer
{
public:

	ImageDisplayer() {}

	void displayImages2(const cv::Mat& imgReference, const cv::Mat& imgTest, const char* title)
	{
		cv::Mat resultNULL;
		std::vector<cv::KeyPoint> KP1, KP2;
		std::vector<cv::DMatch> matchesNULL;

		resultNULL = cv::Mat::zeros(1, 1, CV_8UC1);


		cv::drawMatches(imgReference, KP1, imgTest, KP2,
			matchesNULL, resultNULL,
			cv::Scalar::all(-1), cv::Scalar::all(-1),
			std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS); // DRAW_RICH_KEYPOINTS  NOT_DRAW_SINGLE_POINTS

		cv::namedWindow(title, cv::WINDOW_NORMAL);
		cv::imshow(title, resultNULL);
		cv::waitKey(0);

	}

	void displayImages(const cv::Mat& img1, const cv::Mat& img2, const char* title)
	{
//#ifdef  DISPLAY_IMAGES_DEBUG
		cv::Mat outImg1;
		cv::Mat outImg2;
		cv::Mat outImg;

		cv::Size size(img1.cols + img2.cols, MAX(img1.rows, img2.rows));

		outImg.create(size, img1.type());

		if (size.width > outImg.cols || size.height > outImg.rows)
			CV_Error(CV_StsBadSize, "outImg has size less than need to draw img1 and img2 together");

		outImg1 = outImg(cv::Rect(0, 0, img1.cols, img1.rows));
		outImg2 = outImg(cv::Rect(img1.cols, 0, img2.cols, img2.rows));

		img1.copyTo(outImg1);
		img2.copyTo(outImg2);

		cv::namedWindow(title, cv::WINDOW_NORMAL);
		cv::imshow(title, outImg);
		cv::waitKey(0);
//#endif
	}
};
