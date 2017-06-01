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
#include <opencv2/contrib/contrib.hpp>

#include "Settings.h"  


class FalseColour
{

private:

public:

	FalseColour() {}

	void evaluatePixel(IplImage *src, IplImage *heatmap, IplImage *dst)
	{

		IplImage *R, *G, *B;

		//we split the heatmap along the 3 channels
		R = cvCreateImage(cvGetSize(heatmap), heatmap->depth, 1);
		G = cvCloneImage(R);
		B = cvCloneImage(R);
		cvSplit(heatmap, B, G, R, NULL);

		for (int x = 0; x<src->width; x++)
		{
			for (int y = 0; y<src->height; y++)
			{
				//memory access to the destination color image (faster than splitting the 3 channels...)
				unsigned char *data = &((unsigned char*)(dst->imageData + dst->widthStep*y))[x * 3];

				//read the intensity value in the grayscale image
				unsigned char gray = src->imageData[src->widthStep*y + x];

				//remember, OpenCV store images as BGR internally ! 
				//So access [2] for Red, [1] for Green et [3] for Blue
				data[2] = cvGet2D(R, gray / 255.0 * 500, 1).val[0]; //Red channel
				data[1] = cvGet2D(G, gray / 255.0 * 500, 1).val[0]; //Green channel
				data[0] = cvGet2D(B, gray / 255.0 * 500, 1).val[0]; //Blue channel
			}
		}

	}

	cv::Mat createColour(cv::Mat inImage)
	{
		IplImage *heatmap = cvLoadImage("C:\\Users\\bora\\Desktop\\toHTR\\arac1-colour.bmp", CV_LOAD_IMAGE_COLOR); //the heatmap image, color
	
		IplImage *img = cvLoadImage("C:\\Users\\bora\\Desktop\\toHTR\\arac4-mono.bmp", CV_LOAD_IMAGE_GRAYSCALE);

		IplImage* image2 = cvCloneImage(&(IplImage)inImage);

		//IplImage *dst = cvCreateImage(cvGetSize(image2), 8, 3); //the result image, color

		//evaluatePixel(image2, heatmap, dst);

		IplImage *dst = cvCreateImage(cvGetSize(img), 8, 3); //the result image, color

		evaluatePixel(img, heatmap, dst);


		cvNamedWindow("heatmap", CV_WINDOW_AUTOSIZE);
		cvShowImage("heatmap", heatmap);
		cv::waitKey(0);

		cvNamedWindow("img", CV_WINDOW_AUTOSIZE);
		cvShowImage("img", img);
		cv::waitKey(0);

		cvNamedWindow("image", CV_WINDOW_AUTOSIZE);
		cvShowImage("image", dst);
		cv::waitKey(0);


		cv::Mat m = cv::cvarrToMat(dst);

		return m;
	}

	void falseColor()
	{
		cv::Mat im_gray = cv::imread("C:\\Users\\boran\\Desktop\\toHTR\\arac4-mono.bmp", cv::IMREAD_GRAYSCALE);
		cv::Mat im_color;
		cv::applyColorMap(im_gray, im_color, cv::COLORMAP_RAINBOW);

		cv::namedWindow("image", cv::WINDOW_NORMAL);
		cv::imshow("image", im_color);
		cv::waitKey(0);

	}

	void falseColorBora()
	{
		//cv::Mat im_gray = cv::imread("C:\\Users\\boran\\Desktop\\toHTR\\arac4-mono.bmp", cv::IMREAD_GRAYSCALE);
		//cv::Mat im_color(cv::Size(im_gray.rows, im_gray.cols), CV_8UC3);
		////cv::applyColorMap(im_gray, im_color, cv::COLORMAP_RAINBOW);

		//cv::Mat heatmap = cv::imread("C:\\Users\\boran\\Desktop\\toHTR\\arac1-colour.bmp", CV_LOAD_IMAGE_COLOR); //the heatmap image, color


		//// Split the image into different channels
		//std::vector<cv::Mat> rgbChannels(3);
		//cv::split(heatmap, rgbChannels);

		//for (int x = 0; x<im_gray.cols; x++)
		//{
		//	for (int y = 0; y<im_gray.rows; y++)
		//	{
		//		//memory access to the destination color image (faster than splitting the 3 channels...)
		//		unsigned char *data = &((unsigned char*)(im_color->imageData + dst->widthStep*y))[x * 3];

		//		//read the intensity value in the grayscale image
		//		unsigned char gray = src->imageData[src->widthStep*y + x];

		//		//remember, OpenCV store images as BGR internally ! 
		//		//So access [2] for Red, [1] for Green et [3] for Blue
		//		data[2] = cvGet2D(rgbChannels[0], gray / 255.0 * 500, 1).val[0]; //Red channel
		//		data[1] = cvGet2D(rgbChannels[1], gray / 255.0 * 500, 1).val[0]; //Green channel
		//		data[0] = cvGet2D(rgbChannels[2], gray / 255.0 * 500, 1).val[0]; //Blue channel
		//	}
		//}


		//cv::Mat lookUpTable(1, 256, CV_8U);

		//cv::Mat r, g, b;

		//for (i = 0; i < 255; i++)
		//{
		//	r[i] = i;
		//}

		//cv::Mat GGG;

		//cvtColor(im_gray, GGG, cv::COLOR_BGR2GRAY);

		//cv::LUT(im_gray, rgbChannels, im_color);


		////cv::LUT(im_gray, rgbChannels[0], b);
		////cv::LUT(im_gray, rgbChannels[1], g);
		////cv::LUT(im_gray, rgbChannels[2], r);
		////std::vector<cv::Mat> planes;
		////planes.push_back(b);
		////planes.push_back(g);
		////planes.push_back(r);

		////cv::merge(planes, im_color);

		//cv::namedWindow("image", cv::WINDOW_NORMAL);
		//cv::imshow("image", im_color);
		//cv::waitKey(0);

	}

	//cv::Mat applyCustomColorMap(cv::Mat& im_gray)
	void applyCustomColorMap()
	{
		unsigned char b[] = { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 253, 251, 249, 247, 245, 242, 241,
			238, 237, 235, 233, 231, 229, 227, 225, 223, 221, 219, 217, 215, 213, 211, 209, 207, 205, 203, 201, 199, 197, 195, 193,
			191, 189, 187, 185, 183, 181, 179, 177, 175, 173, 171, 169, 167, 165, 163, 161, 159, 157, 155, 153, 151, 149, 147, 145,
			143, 141, 138, 136, 134, 132, 131, 129, 126, 125, 122, 121, 118, 116, 115, 113, 111, 109, 107, 105, 102, 100, 98, 97, 94,
			93, 91, 89, 87, 84, 83, 81, 79, 77, 75, 73, 70, 68, 66, 64, 63, 61, 59, 57, 54, 52, 51, 49, 47, 44, 42, 40, 39, 37, 34, 33,
			31, 29, 27, 25, 22, 20, 18, 17, 14, 13, 11, 9, 6, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

		unsigned char g[] = { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
			255, 255, 255, 254, 252, 250, 248, 246, 244, 242, 240, 238, 236, 234, 232, 230, 228, 226, 224, 222, 220, 218, 216, 214, 212,
			210, 208, 206, 204, 202, 200, 198, 196, 194, 192, 190, 188, 186, 184, 182, 180, 178, 176, 174, 171, 169, 167, 165, 163, 161,
			159, 157, 155, 153, 151, 149, 147, 145, 143, 141, 139, 137, 135, 133, 131, 129, 127, 125, 123, 121, 119, 117, 115, 113, 111,
			109, 107, 105, 103, 101, 99, 97, 95, 93, 91, 89, 87, 85, 83, 82, 80, 78, 76, 74, 72, 70, 68, 66, 64, 62, 60, 58, 56, 54, 52,
			50, 48, 46, 44, 42, 40, 38, 36, 34, 32, 30, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10, 8, 6, 4, 2, 0 };

		unsigned char r[] = { 195, 194, 193, 191, 190, 189, 188, 187, 186, 185, 184, 183, 182, 181, 179, 178, 177, 176, 175, 174, 173, 172, 171, 170, 169, 167,
			166, 165, 164, 163, 162, 161, 160, 159, 158, 157, 155, 154, 153, 152, 151, 150, 149, 148, 147, 146, 145, 143, 142, 141, 140, 139,
			138, 137, 136, 135, 134, 133, 131, 130, 129, 128, 127, 126, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 126,
			126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
			126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
			127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
			127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
			127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
			127, 127, 127, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
			126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126 };

		cv::Mat channels[] = { cv::Mat(256, 1, CV_8U, b), cv::Mat(256, 1, CV_8U, g), cv::Mat(256, 1, CV_8U, r) };
		cv::Mat lut; // Create a lookup table 
		cv::merge(channels, 3, lut);

		cv::Mat im_gray = cv::imread("C:\\Users\\boran\\Desktop\\toHTR\\arac4-mono.bmp", cv::IMREAD_GRAYSCALE);
		//cv::Mat im_color(cv::Size(im_gray.rows, im_gray.cols), CV_8UC3);

		cv::cvtColor(im_gray.clone(), im_gray, cv::COLOR_GRAY2BGR);

		cv::Mat im_color;
		cv::LUT(im_gray, lut, im_color);

		cv::namedWindow("image", cv::WINDOW_NORMAL);
		cv::imshow("image", im_color);
		cv::waitKey(0);

		//return im_color;
	}


};