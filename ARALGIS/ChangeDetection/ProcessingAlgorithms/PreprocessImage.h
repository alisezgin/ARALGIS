#pragma once

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

#include "ImageDisplayer.h"  

#include "Settings.h"  


class PreprocessImage
{

private:

	// original image
	cv::Mat img;

	// 32-bit float image containing the Wiener Filtered Image
	cv::Mat wienerFiltered;

	// 32-bit float image containing the Gaussian Filtered Image
	cv::Mat gaussianFiltered;

	// 32-bit float image containing the Bilateral Filtered Image
	cv::Mat bilateralFiltered;

	// Aperture size of the Wiener Filter kernel
	int kernelSizeWiener;

	// Gaussian smooth variance 
	int kernelSizeGauss;

public:

	PreprocessImage() : kernelSizeWiener(3), kernelSizeGauss(3) {}

	// Set the aperture size of the kernel
	void setKernelSizeWiener(int a)
	{
		kernelSizeWiener = a;
	}

	// Get the aperture size of the kernel
	int getKernelSizeWiener() const
	{
		return kernelSizeWiener;
	}


	// Set the aperture size of the kernel
	void setKernelSizeGaussian(int a)
	{
		kernelSizeGauss = a;
	}

	// Get the aperture size of the kernel
	int getKernelSizeGaussian() const
	{
		return kernelSizeGauss;
	}

	// Compute Gaussian
	cv::Mat computeGaussianBlur(const cv::Mat& image)
	{
		// Compute Laplacian
		cv::GaussianBlur(image, gaussianFiltered, cv::Size(kernelSizeGauss, kernelSizeGauss), 0, 0);

		return gaussianFiltered;
	}

	// Compute the floating point Laplacian
	cv::Mat computeWiener(const cv::Mat& image)
	{
		// Compute Laplacian
		cv::GaussianBlur(image, gaussianFiltered, cv::Size(kernelSizeGauss, kernelSizeGauss), 0, 0);

		return gaussianFiltered;
	}

	cv::Mat computeBilateralFilter(const cv::Mat& image)
	{
		int ksize = 9; // Filter size. Has a large effect on speed.
		double sigmaColor = 9; // Filter color strength.
		double sigmaSpace = 7; // Spatial strength. Affects speed.

		cv::bilateralFilter(image, bilateralFiltered, ksize, sigmaColor, sigmaSpace);

		return bilateralFiltered;
	}



	//void resizeSmallImage(const cv::Mat& image1, const cv::Mat& image2, int* resizedImageNo, cv::Mat* resizedImage)
	cv::Mat resizeSmallImage(const cv::Mat& image1, const cv::Mat& image2, int* resizedImageNo)
	{
		int size1 = image1.rows * image1.cols;
		int size2 = image2.rows * image2.cols;

		*resizedImageNo = 0;

		cv::Mat resizedImage;

		if (size1 >= size2)
		{
			resize(image2, resizedImage, image1.size(), (0, 0), (0, 0), cv::INTER_CUBIC);
			*resizedImageNo = 2;
			return resizedImage;
		}
		else if (size1 < size2)


		{
			resize(image1, resizedImage, image2.size(), (0, 0), (0, 0), cv::INTER_CUBIC);
			*resizedImageNo = 1;
			return resizedImage;
		}

		return resizedImage;
	}

	// this function equalizes mean  and std of imageX wrt imageY
	// imageY is given in iReferenceImage parameter
	void normalizedImage(cv::Mat& image1, cv::Mat& image2, int iReferenceImage, cv::Mat* imageNormalizedOut)
	{
		cv::Mat mean1, mean2;
		cv::Mat stddev1, stddev2;

		uchar meanVal1, stddevValue1;
		uchar meanVal2, stddevValue2;


		cv::meanStdDev(image1, mean1, stddev1);
		cv::meanStdDev(image2, mean2, stddev2);

		stddevValue1 = (uchar)(stddev1.at<double>(0, 0) + 0.5);
		meanVal1 = (uchar)(mean1.at<double>(0, 0) + 0.5);

		stddevValue2 = (uchar)(stddev2.at<double>(0, 0) + 0.5);
		meanVal2 = (uchar)(mean2.at<double>(0, 0) + 0.5);

#ifdef  DISPLAY_PRINTS_DEBUG
		std::printf("\n normalizedImage MEAN1 %d STDDEV1 %d", meanVal1, stddevValue1);
		std::printf("\n normalizedImage MEAN2 %d STDDEV2 %d\n", meanVal2, stddevValue2);
#endif

		float dTemp;

		if (iReferenceImage == 1)
		{
			cv::Mat_<uchar>::const_iterator it = image2.begin<uchar>();
			cv::Mat_<uchar>::const_iterator itend = image2.end<uchar>();

			cv::Mat imageNormalized(image2.size(), CV_8U, cv::Scalar(0));;
			cv::Mat_<uchar>::iterator itout = imageNormalized.begin<uchar>();

			for (; it != itend; ++it, ++itout)
			{

				dTemp = (((float)stddevValue1 * (float)((float)*it - (float)meanVal2) / (float)stddevValue2) + (float)meanVal1);

				if (dTemp > 255.0)
				{
					*itout = 255;
#ifdef  DISPLAY_PRINTS_DEBUG
					//printf("%d %.3f ", *it, dTemp);
					//getchar();
#endif
				}
				else if (dTemp < 0.0)
				{
					*itout = 0;
#ifdef  DISPLAY_PRINTS_DEBUG
					//printf("%d %.3f", *it, dTemp);
#endif
				}
				else
				{
					*itout = (uchar)(dTemp + 0.5);
				}
			}

			imageNormalized.copyTo(*imageNormalizedOut);
			
		}
		else if (iReferenceImage == 2)
		{
			cv::Mat_<uchar>::const_iterator it = image1.begin<uchar>();
			cv::Mat_<uchar>::const_iterator itend = image1.end<uchar>();

			//cv::Mat imageNormalized(image1.size(), CV_8U, cv::Scalar(0));;
			cv::Mat_<uchar>::iterator itout = imageNormalizedOut->begin<uchar>();

			for (; it != itend; ++it, ++itout)
			{
				dTemp = (((float)stddevValue2 * (float)((float)*it - (float)meanVal1) / (float)stddevValue1) + (float)meanVal2);

				if (dTemp > 255.0)
				{
					*itout = 255;
				}
				else if (dTemp < 0.0)
				{
					*itout = 0;
#ifdef  DISPLAY_PRINTS_DEBUG
					//printf("%d %.3f", *it, dTemp);
#endif
				}
				else
				{
					*itout = (uchar)(dTemp + 0.5);
				}
			}

		}
		else
		{
			cv::Mat imageNormalized(cv::Mat::eye(image1.cols, image1.rows, CV_8U));
		}
	}

	// this function equalizes mean  and std of imageX wrt imageY
	// imageY is given in iReferenceImage parameter
	cv::Mat normalizeImage2(const cv::Mat& imageToNormalize,
		double* meanVal1, double* stdVal1,
		double* meanVal2, double* stdVal2,
		int iReferenceImage)
	{


		if (iReferenceImage == 1)
		{
			cv::Mat_<uchar>::const_iterator it = imageToNormalize.begin<uchar>();
			cv::Mat_<uchar>::const_iterator itend = imageToNormalize.end<uchar>();

			cv::Mat imageNormalized(imageToNormalize.size(), CV_8U, cv::Scalar(0));
			cv::Mat_<uchar>::iterator itout = imageNormalized.begin<uchar>();

			for (; it != itend; ++it, ++itout)
			{
				*itout = (uchar)(((*stdVal2 * (*it - *meanVal1) / *stdVal1) + *meanVal2) + 0.5);
			}

			return imageNormalized;
		}
		else if (iReferenceImage == 2)
		{
			cv::Mat_<uchar>::const_iterator it = imageToNormalize.begin<uchar>();
			cv::Mat_<uchar>::const_iterator itend = imageToNormalize.end<uchar>();

			cv::Mat imageNormalized(imageToNormalize.size(), CV_8U, cv::Scalar(0));;
			cv::Mat_<uchar>::iterator itout = imageNormalized.begin<uchar>();

			for (; it != itend; ++it, ++itout)
			{
				*itout = (uchar)(((*stdVal1 * (*it - *meanVal2) / *stdVal2) + *meanVal1) + 0.5);
			}

			return imageNormalized;
		}
		else
		{
			cv::Mat imageNormalized(cv::Mat::eye(imageToNormalize.cols, imageToNormalize.rows, CV_8U));
			return imageNormalized;
		}
	}

	// this function equalizes mean  and std of imageX wrt imageY
	// imageY is given in iReferenceImage parameter
	void getMeanVar(const cv::Mat& image1, double* meanVal, double* stdVal)
	{
		cv::Mat mean1, stddev1;

		cv::meanStdDev(image1, mean1, stddev1);

		*stdVal = (double)(stddev1.at<double>(0, 0) + 0.5);
		*meanVal = (double)(mean1.at<double>(0, 0) + 0.5);
	}

	// this function equalizes mean  and std of imageX wrt imageY
	// imageY is given in iReferenceImage parameter
	void calcLog(const cv::Mat& image1, cv::Mat* imgOut)
	{
		cv::Mat_<uchar>::const_iterator it = image1.begin<uchar>();
		cv::Mat_<uchar>::const_iterator itend = image1.end<uchar>();


		cv::Mat_<uchar>::iterator itout = (*imgOut).begin<uchar>();

		for (; it != itend; ++it, ++itout)
		{
			*itout = (uchar)(cv::log(*it + 1) + 0.5);
		}
	}



	void PreprocessStage(cv::Mat* imgReference, cv::Mat* imgTest)
	{

		std::vector<cv::DMatch> matchesCombined;
		std::vector<cv::KeyPoint> keypointsRefCombined, keypointsTestCombined;

		ImageDisplayer imageDisplayer;

#ifdef PREPROCESS_RESIZE_IMAGES
		cv::Mat resizedImage;
		int resizedImgNo;
		resizedImage = resizeSmallImage(*imgReference, *imgTest, &resizedImgNo);

		if (resizedImgNo == 1)
		{
			resizedImage.copyTo(*imgReference);
		}
		else if (resizedImgNo == 2)
		{
			resizedImage.copyTo(*imgTest);
		}
		// end of Resizing The Image

#ifdef DISPLAY_IMAGES_DEBUG_FINAL
		char title2[1000];
		strcpy_s(title2, "-ORIGINALS TOGETHER RESIZED-");
		imageDisplayer.displayImages(*imgReference, *imgTest, title2);
#endif
#endif // PREPROCESS_RESIZE_IMAGES

#ifdef PREPROCESS_SCALEDOWN_IMAGES
		if (imgReference->rows > 1000)
		{
			//// pyramid down once 
			cv::Mat imgRefDown1, imgTestDown1;
			cv::pyrDown(*imgReference, imgRefDown1, cv::Size(imgReference->cols / 2, imgReference->rows / 2));
			cv::pyrDown(*imgTest, imgTestDown1, cv::Size(imgTest->cols / 2, imgTest->rows / 2));


			//cv::Mat imgRefDown2, imgTestDown2;
			//cv::pyrDown(imgRefDown1, imgRefDown2, cv::Size( (imgRefDown1.cols / 2), (imgRefDown1.rows / 2)));
			//cv::pyrDown(imgTestDown1, imgTestDown2, cv::Size(imgTestDown1.cols / 2, imgTestDown1.rows / 2));

			imgRefDown1.copyTo(*imgReference);
			imgTestDown1.copyTo(*imgTest);

#ifdef  DISPLAY_PRINTS_DEBUG
			printf("\n\n Image 1 Width %d  Height %d\n", imgRefDown1.cols, imgRefDown1.rows);
			printf("\n Image 2 Width %d  Height %d\n\n", imgTestDown1.cols, imgTestDown1.rows);
#endif
		}
		else if (imgReference->rows < 500) 
		{
			//// pyramid up once 
			cv::Mat imgRefDown1, imgTestDown1;
			cv::pyrUp(*imgReference, imgRefDown1, cv::Size(imgReference->cols * 2, imgReference->rows * 2));
			cv::pyrUp(*imgTest, imgTestDown1, cv::Size(imgTest->cols * 2, imgTest->rows * 2));

			imgRefDown1.copyTo(*imgReference);
			imgTestDown1.copyTo(*imgTest);

#ifdef  DISPLAY_PRINTS_DEBUG
			printf("\n\n Image 1 Width %d  Height %d\n", imgRefDown1.cols, imgRefDown1.rows);
			printf("\n Image 2 Width %d  Height %d\n\n", imgTestDown1.cols, imgTestDown1.rows);
#endif
		}
#endif //PREPROCESS_SCALEDOWN_IMAGES

#ifdef PREPROCESS_HISTO_EQUALIZE
		/// Apply Histogram Equalization
		cv::Mat histEq1, histEq2;
		cv::equalizeHist(*imgReference, histEq1);
		cv::equalizeHist(*imgTest, histEq2);

		/// Display results
#ifdef  DISPLAY_IMAGES_DEBUG_INITIAL
		cv::namedWindow("IMAGEREFERENCE", cv::WINDOW_NORMAL);
		cv::namedWindow("IMAGEREFHISTEQ", cv::WINDOW_NORMAL);
		cv::imshow("IMAGEREFERENCE", *imgReference);
		cv::imshow("IMAGEREFHISTEQ", histEq1);

		cv::namedWindow("IMAGE=TEST", cv::WINDOW_NORMAL);
		cv::namedWindow("IMAGETESTHISTEQ", cv::WINDOW_NORMAL);
		cv::imshow("IMAGE=TEST", *imgTest);
		cv::imshow("IMAGETESTHISTEQ", histEq2);
#endif
		histEq1.copyTo(*imgReference);
		histEq2.copyTo(*imgTest);

#ifdef DISPLAY_IMAGES_DEBUG_INITIAL
		char title3[1000];
		strcpy_s(title3, "RESIZED IMAGES TOGETHER HISTO-EQUALIZED \0");
		imageDisplayer.displayImages(*imgReference, *imgTest, title3);
#endif // DISPLAY_IMAGES_DEBUG_INITIAL

#endif /// end of Histogram Equalization


#ifdef PREPROCESS_HISTO_MATCH
		////// Match histograms of 'src' to that of 'dst'.
		HistogramMatch histProcess;
		cv::Mat processedImage2 = histProcess.histMatch(*imgReference, *imgTest);
		processedImage2.copyTo(*imgTest);

#ifdef DISPLAY_IMAGES_DEBUG_INITIAL
		//Display results
		namedWindow("HISTO-MATCH", cv::WINDOW_NORMAL);
		imshow("HISTO-MATCH", processedImage2);
		///// end of Histogram Match
#endif
#endif //PREPROCESS_HISTO_EQUALIZE

		// Preprocessing
		// Gaussian Smoothing
		// Mean and Variance Equalization -- optional
		// Resizing the small image to have equal sizes

#ifdef PREPROCESS_IMAGE_NORMALIZE
		// Mean and Variance Equalization 
		// Set the Mean and Variance of Second Image equal To First Image
		cv::Mat normImage2 = cv::Mat::zeros(imgReference->rows, imgReference->cols, imgReference->type());
		normalizedImage(*imgReference, *imgTest, 1, &normImage2);
		*imgTest = normImage2.clone();

#ifdef DISPLAY_IMAGES_DEBUG_INITIAL
		cv::namedWindow("MEAN-STD NORMALIZED IMGTEST", cv::WINDOW_NORMAL);
		imshow("MEAN-STD NORMALIZED IMGTEST", normImage2);
		// end of Mean and Variance Equalization 
#endif
#endif // PREPROCESS_IMAGE_NORMALIZE

#ifdef PREPROCESS_GAUSS_BLUR
		//PreprocessImage preImg2;
		// Gaussian Smoothing of both Image
		setKernelSizeGaussian(11);
		//cv::Mat img1Gauss = computeGaussianBlur(*imgReference);
		cv::Mat img1Gauss;
		cv::GaussianBlur(*imgReference, img1Gauss, cv::Size(kernelSizeGauss, kernelSizeGauss), 0, 0);

		//PreprocessImage preImg3;
		setKernelSizeGaussian(11);
		//cv::Mat img2Gauss = computeGaussianBlur(*imgTest);
		cv::Mat img2Gauss;
		cv::GaussianBlur(*imgTest, img2Gauss, cv::Size(kernelSizeGauss, kernelSizeGauss), 0, 0);

		img1Gauss.copyTo(*imgReference);
		img2Gauss.copyTo(*imgTest);
		// end of Gaussian Smoothing of both Images

#ifdef DISPLAY_IMAGES_DEBUG_INITIAL
		char title4[1000];
		strcpy_s(title4, "GAUSSIAN SMOOTHED TOGETHER RESIZED   \0");
		imageDisplayer.displayImages(*imgReference, *imgTest, title4);
#endif
#endif // PREPROCESS_GAUSS_BLUR
	}

	void PreprocessStageColour(cv::Mat* imgReference, cv::Mat* imgTest, bool isSmallRequired = true)
	{

		std::vector<cv::DMatch> matchesCombined;
		std::vector<cv::KeyPoint> keypointsRefCombined, keypointsTestCombined;

		ImageDisplayer imageDisplayer;

#ifdef  DISPLAY_PRINTS_DEBUG
		printf("\nOriginal imgReference Width %d  Height %d\n", imgReference->cols, imgReference->rows);
		printf("\nOriginal imgTest Width %d  Height %d\n\n", imgTest->cols, imgTest->rows);
#endif

#ifdef PREPROCESS_RESIZE_IMAGES
		cv::Mat resizedImage;
		int resizedImgNo;
		resizedImage = resizeSmallImage(*imgReference, *imgTest, &resizedImgNo);

		if (resizedImgNo == 1)
		{
			resizedImage.copyTo(*imgReference);
		}
		else if (resizedImgNo == 2)
		{
			resizedImage.copyTo(*imgTest);
		}
		// end of Resizing The Image

#ifdef  DISPLAY_PRINTS_DEBUG
		printf("\n\n Resized imgReference Width %d  Height %d\n", imgReference->cols, imgReference->rows);
		printf("\n Resized imgTest Width %d  Height %d\n\n", imgTest->cols, imgTest->rows);
#endif

#ifdef DISPLAY_IMAGES_DEBUG_FINAL
		char title2[1000];
		strcpy_s(title2, "-ORIGINALS COLOUR TOGETHER RESIZED-");
		imageDisplayer.displayImages(*imgReference, *imgTest, title2);
#endif
#endif //PREPROCESS_RESIZE_IMAGES


#ifdef PREPROCESS_SCALEDOWN_IMAGES
		if (isSmallRequired)
		{
			if (imgReference->rows > 1000)
			{
				//// pyramid down once 
				cv::Mat imgRefDown1, imgTestDown1;
				cv::pyrDown(*imgReference, imgRefDown1, cv::Size(imgReference->cols / 2, imgReference->rows / 2));
				cv::pyrDown(*imgTest, imgTestDown1, cv::Size(imgTest->cols / 2, imgTest->rows / 2));


				//cv::Mat imgRefDown2, imgTestDown2;
				//cv::pyrDown(imgRefDown1, imgRefDown2, cv::Size( (imgRefDown1.cols / 2), (imgRefDown1.rows / 2)));
				//cv::pyrDown(imgTestDown1, imgTestDown2, cv::Size(imgTestDown1.cols / 2, imgTestDown1.rows / 2));

				imgRefDown1.copyTo(*imgReference);
				imgTestDown1.copyTo(*imgTest);

#ifdef  DISPLAY_PRINTS_DEBUG
				printf("\n\n Pyramid imgReference  Width %d  Height %d\n", imgRefDown1.cols, imgRefDown1.rows);
				printf("\n Pyramid imgTest Width %d  Height %d\n\n", imgTestDown1.cols, imgTestDown1.rows);
#endif
			}
			else if (imgReference->rows < 750)
			{
				//// pyramid up once 
				cv::Mat imgRefDown1, imgTestDown1;
				cv::pyrUp(*imgReference, imgRefDown1, cv::Size(imgReference->cols * 2, imgReference->rows * 2));
				cv::pyrUp(*imgTest, imgTestDown1, cv::Size(imgTest->cols * 2, imgTest->rows * 2));

				imgRefDown1.copyTo(*imgReference);
				imgTestDown1.copyTo(*imgTest);

#ifdef  DISPLAY_PRINTS_DEBUG
				printf("\n\n Pyramid imgReference Width %d  Height %d\n", imgRefDown1.cols, imgRefDown1.rows);
				printf("\n Pyramid imgTest Width %d  Height %d\n\n", imgTestDown1.cols, imgTestDown1.rows);
#endif
			}
		}
#endif // PREPROCESS_SCALEDOWN_IMAGES
	}
};