#include "stdafx.h"

#ifndef SHARED_HANDLERS
#include "ARALGIS.h"
#endif

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include "opencv2\features2d\features2d.hpp"
#include "opencv2\nonfree\features2d.hpp"
#include "opencv2/opencv.hpp"

#include "..\ProcessingAlgorithms\TransformImage.h" 
#include "..\ProcessingAlgorithms\HistogramMatch.h"  
#include "..\ProcessingAlgorithms\PreprocessImage.h"  
#include "..\ProcessingAlgorithms\RobustMatcher.h"  
#include "..\ProcessingAlgorithms\ImagePartition.h"  
#include "..\ProcessingAlgorithms\ImagePartitionSelect.h"  
#include "..\ProcessingAlgorithms\DisplayMatches.h"
#include "..\ProcessingAlgorithms\HomographyEstimator.h"
#include "..\ProcessingAlgorithms\MatchCombinerEliminator.h"
#include "..\ProcessingAlgorithms\Image.h"
#include "..\ProcessingAlgorithms\SpectralAngleMapper.h"
#include "..\ProcessingAlgorithms\Settings.h"  

#include "..\HeaderFiles\ChangeDetector.h"

int CChangeDetector::process(cv::Mat &aImgReference, const cv::Mat &aImgTest)
{
	cv::Mat imgReferenceClr;
	cv::Mat imgTestClr;
	cv::Mat imgReference;
	cv::Mat imgTest;

	cv::Mat AA1, AA2, AA3, AA4, AA5, AA6;

	if (aImgReference.rows == 0 || aImgReference.cols == 0)
	{
		return -1;
	}

	if (aImgTest.rows == 0 || aImgTest.cols == 0)
	{
		return -1;
	}

	transpose(aImgReference, AA1);
	flip(AA1, AA2, 0); //transpose+flip(1)=CW
	AA2.copyTo(AA3);

	transpose(aImgTest, AA4);
	flip(AA4, AA5, 0); //transpose+flip(1)=CW
	AA5.copyTo(AA6);


	if (g_CameraPixelBits == 24)
	{
		cvtColor(AA3, imgReference, CV_BGR2GRAY);
		cvtColor(AA6, imgTest, CV_BGR2GRAY);

		AA3.copyTo(imgReferenceClr);
		AA6.copyTo(imgTestClr);
	}
	else
	{
		cv::applyColorMap(AA3, imgReferenceClr, cv::COLORMAP_RAINBOW);
		cv::applyColorMap(AA6, imgTestClr, cv::COLORMAP_RAINBOW);

		AA3.copyTo(imgReference);
		AA6.copyTo(imgTest);
	}




	int dRowDiff = abs(imgReferenceClr.rows - imgTestClr.rows);

	if (!imgReference.data || !imgTest.data)
	{
		DEBUG_PRINT(" --(!) Error reading images "); 
		return -1;
	}
	//DEBUG_PRINT("\n\n Image 1 Width %d  Height %d\n", imgReference.cols, imgReference.rows);
	//DEBUG_PRINT("\n Image 2 Width %d  Height %d\n\n", imgTest.cols, imgTest.rows);

	cv::Mat img1, img2;
	imgReference.copyTo(img1);
	imgTest.copyTo(img2);

	//ImageDisplayer imageDisplayer;
	//char title1[1000];
	//strcpy_s(title1, "ORIGINAL IMAGES TOGETHER--");
	//imageDisplayer.displayImages(imgReference, imgTest, title1);


	//ImageDisplayer imageDisplayerClr;
	//char titleClr[1000];
	//strcpy_s(titleClr, "ORIGINAL COLOUR IMAGES TOGETHER--");
	//imageDisplayerClr.displayImages(imgReferenceClr, imgTestClr, titleClr);


#ifdef  DISPLAY_DEBUG_PARTITION
	cv::namedWindow("TEST ONLY", cv::WINDOW_NORMAL);
	cv::imshow("TEST ONLY", imgTestClr);
	cv::waitKey(0);
#endif

	// calculate the duration of matching process
	int64 tick1 = cv::getTickCount();

	// Preprocessing
	// Gaussian Smoothing -- optional
	// Mean and Variance Equalization -- optional
	// Resizing the small image to have equal sizes
	PreprocessImage preImg;
	preImg.PreprocessStage(&imgReference, &imgTest);

#ifdef DEBUG_PRINT_FINAL1
	DEBUG_PRINT("Preprocess Gray Image 1 Width %d  Height %d\n", imgReference.cols, imgReference.rows);
	DEBUG_PRINT("Preprocess Gray  Image 2 Width %d  Height %d\n\n", imgTest.cols, imgTest.rows);
#endif

	cv::applyColorMap(imgReference, imgReferenceClr, cv::COLORMAP_RAINBOW);
	cv::applyColorMap(imgTest, imgTestClr, cv::COLORMAP_RAINBOW);

	/// Preprocess Colour Image
	/// Includes only resize and scale down if selected
	PreprocessImage preImgClr;
	preImgClr.PreprocessStageColour(&imgReferenceClr, &imgTestClr);

#ifdef DEBUG_PRINT_FINAL10
	DEBUG_PRINT("\n Preprocess Colour Image 1 Width %d  Height %d\n", imgReferenceClr.cols, imgReferenceClr.rows);
	DEBUG_PRINT("\n Preprocess Colour  Image 2 Width %d  Height %d\n\n", imgTestClr.cols, imgTestClr.rows);
#endif

#ifdef  DISPLAY_CONCAN_IMAGE_INTERMEDIATE
	ImageDisplayer imageDisplayerClr3;
	char titleClr3[1000];
	strcpy_s(titleClr3, "ORIGINAL IMAGES RESIZED");
	imageDisplayerClr3.displayImages(imgReference, imgTest, titleClr3);
#endif


	// Matching Process ----> Image Registration
	// Prepare the matcher

	// There is 4 Matchers:
	// 1. SURF
	// 2. ORB 
	// 3. STAR
	// 4. FAST
	// Try to find the best matcher among these three...

	cv::Ptr<cv::FeatureDetector> pfd2 =
		new cv::SurfFeatureDetector(1400); // 1400
	cv::Ptr<cv::DescriptorExtractor> pfd22 = new cv::SurfDescriptorExtractor();

	cv::Ptr<cv::FeatureDetector> pfd3 = new cv::OrbFeatureDetector(1024);
	cv::Ptr<cv::DescriptorExtractor> pfd33 = new cv::OrbDescriptorExtractor();

	cv::Ptr<cv::FeatureDetector> pfd4 = new cv::StarFeatureDetector();
	cv::Ptr<cv::DescriptorExtractor> pfd44 = new cv::OrbDescriptorExtractor();

	cv::Ptr<cv::FeatureDetector> pfd5 = new cv::FastFeatureDetector(50, true);
	cv::Ptr<cv::DescriptorExtractor> pfd55 = new cv::OrbDescriptorExtractor();

	//cv::Ptr<cv::FeatureDetector> pfd5 = new cv::SiftFeatureDetector();
	//cv::Ptr<cv::DescriptorExtractor> pfd55 = new cv::SiftDescriptorExtractor();


	// Match the two images
	std::vector<cv::DMatch> matchesSURF;
	std::vector<cv::KeyPoint> keypointsRefSURF, keypointsTestSURF;

	std::vector<cv::DMatch> matches1SURFAll;
	std::vector<cv::KeyPoint> keypoints1SURFAll;
	std::vector<cv::DMatch> matches2SURFAll;
	std::vector<cv::KeyPoint> keypoints2SURFAll;

	// Match the two images
	std::vector<cv::DMatch> matchesFAST;
	std::vector<cv::KeyPoint> keypointsRefFAST, keypointsTestFAST;

	std::vector<cv::DMatch> matches1FASTAll;
	std::vector<cv::KeyPoint> keypoints1FASTAll;
	std::vector<cv::DMatch> matches2FASTAll;
	std::vector<cv::KeyPoint> keypoints2FASTAll;


	// Match the two images
	std::vector<cv::DMatch> matchesSTAR;
	std::vector<cv::KeyPoint> keypointsRefSTAR, keypointsTestSTAR;

	std::vector<cv::DMatch> matches1STARAll;
	std::vector<cv::KeyPoint> keypoints1STARAll;
	std::vector<cv::DMatch> matches2STARAll;
	std::vector<cv::KeyPoint> keypoints2STARAll;

	// Match the two images
	std::vector<cv::DMatch> matchesORB;
	std::vector<cv::KeyPoint> keypointsRefORB, keypointsTestORB;

	std::vector<cv::DMatch> matches1ORBAll;
	std::vector<cv::KeyPoint> keypoints1ORBAll;
	std::vector<cv::DMatch> matches2ORBAll;
	std::vector<cv::KeyPoint> keypoints2ORBAll;

	cv::Mat dRef;
	cv::Mat dTest;

	int kkk = 0;

	TransformImage imgTransformGrey;
	TransformImage imgTransformClr;

	cv::Mat imgTestWarpedPers;


	RobustMatcher rmatcherSURF;
	rmatcherSURF.setRatio(0.7f);
	rmatcherSURF.setFeatureDetector(pfd2);
	rmatcherSURF.setDescriptorExtractor(pfd22);
	rmatcherSURF.setDisplay(true);

	RobustMatcher rmatcherORB;
	rmatcherORB.setRatio(0.7f);
	rmatcherORB.setFeatureDetector(pfd3);
	rmatcherORB.setDescriptorExtractor(pfd33);
	rmatcherORB.setDisplay(true);

#ifdef  DISPLAY_IMAGES_DEBUG_FINAL
	DisplayMatches matchDisplayerSURF;
	DisplayMatches matchDisplayerORB;
#endif


#ifdef INCLUDE_SURF
#ifdef DEBUG_PRINT_FINAL1
	DEBUG_PRINT("SURF Matcher begins\n");
#endif
	/// SURF MATCHER begins
	rmatcherSURF.match(imgReference, imgTest,
		matchesSURF, keypointsRefSURF, keypointsTestSURF,
		matches1SURFAll, keypoints1SURFAll,
		matches2SURFAll, keypoints2SURFAll);

#ifdef  DISPLAY_IMAGES_DEBUG_FINAL
	char titleSURF[1000];
	strcpy_s(titleSURF, "SURF");
	char sNumSURF[20];
	_itoa_s(kkk, sNumSURF, sizeof(sNumSURF), 10);
	strcat_s(titleSURF, sNumSURF);

	matchDisplayerSURF.displayMatchesProcessor(imgReference, imgTest, matchesSURF,
		keypointsRefSURF, keypointsTestSURF, titleSURF);
#endif
#endif

#ifdef INCLUDE_ORB
#ifdef DEBUG_PRINT_FINAL1
	DEBUG_PRINT("ORB Matcher begins\n");
#endif
	/// ORB MATCHER begins
	rmatcherORB.match(imgReference, imgTest,
		matchesORB, keypointsRefORB, keypointsTestORB,
		matches1ORBAll, keypoints1ORBAll,
		matches2ORBAll, keypoints2ORBAll);

#ifdef  DISPLAY_IMAGES_DEBUG_FINAL
	char titleORB[1000];
	strcpy_s(titleORB, "ORB");
	char sNumORB[20];
	_itoa_s(kkk, sNumORB, sizeof(sNumORB), 10);
	strcat_s(titleORB, sNumORB);

	matchDisplayerORB.displayMatchesProcessor(imgReference, imgTest, matchesORB,
		keypointsRefORB, keypointsTestORB, titleORB);
#endif
#endif

	cv::Point2f offset((float)imgReference.cols, 0);
#ifdef DEBUG_PRINT_FINAL10
	DEBUG_PRINT("\n OFFSET %d\n", imgReference.cols);
#endif

#ifdef INCLUDE_STAR
#ifdef DEBUG_PRINT_FINAL1
	DEBUG_PRINT("STAR Matcher begins\n");
#endif

	/// FAST MATCHER begins
	RobustMatcher rmatcherSTAR;
	rmatcherSTAR.setRatio(0.7f);
	rmatcherSTAR.setFeatureDetector(pfd4);
	rmatcherSTAR.setDescriptorExtractor(pfd44);
	rmatcherSTAR.setDisplay(true);

	rmatcherSTAR.match(imgReference, imgTest, // imageTestProcessed1
		matchesSTAR, keypointsRefSTAR, keypointsTestSTAR,
		matches1STARAll, keypoints1STARAll,
		matches2STARAll, keypoints2STARAll);

#ifdef  DISPLAY_IMAGES_DEBUG_FINAL
	char titleSTAR[1000];
	strcpy_s(titleSTAR, "STAR ");
	//char sNumFAST[20];
	//_itoa_s(kkk, sNumFAST, sizeof(sNumFAST), 10);
	//strcat_s(titleFAST, sNumFAST);

	DisplayMatches matchDisplayerSTAR;
	matchDisplayerSTAR.displayMatchesProcessor(imgReference, imgTest, matchesSTAR, // imageTestProcessed1
		keypointsRefSTAR, keypointsTestSTAR, titleSTAR);
#endif
#endif

#ifdef INCLUDE_FAST
#ifdef DEBUG_PRINT_FINAL1
	DEBUG_PRINT("\n FAST Matcher begins\n");
#endif

	int64 tick3333 = cv::getTickCount();
	// time in miliseconds
	double time3333 = ((tick3333 - tick1) / cv::getTickFrequency());
	DEBUG_PRINT("\nExecution Time After STAR Calculation %.3f seconds\n", time3333);

	/// STAR MATCHER begins
	RobustMatcher rmatcherFAST;
	rmatcherFAST.setRatio(0.7f);
	rmatcherFAST.setFeatureDetector(pfd5);
	rmatcherFAST.setDescriptorExtractor(pfd55);
	rmatcherFAST.setDisplay(true);

	rmatcherFAST.match(imgReference, imgTest, // imageTestProcessed1
		matchesFAST, keypointsRefFAST, keypointsTestFAST,
		matches1FASTAll, keypoints1FASTAll,
		matches2FASTAll, keypoints2FASTAll);

#ifdef  DISPLAY_IMAGES_DEBUG_FINAL
	char titleFAST[1000];
	strcpy_s(titleFAST, "FAST ");
	//char sNumSIFT[20];
	//_itoa_s(kkk, sNumSIFT, sizeof(sNumSIFT), 10);
	//strcat_s(titleSIFT, sNumSIFT);

	DisplayMatches matchDisplayerFAST;
	matchDisplayerFAST.displayMatchesProcessor(imgReference, imgTest, matchesFAST, // imageTestProcessed1
		keypointsRefFAST, keypointsTestFAST, titleFAST);
#endif
#endif

	int64 tick333 = cv::getTickCount();
	// time in miliseconds
	double time333 = ((tick333 - tick1) / cv::getTickFrequency());
#ifdef DEBUG_PRINT_FINAL1
	DEBUG_PRINT("\nExecution Time After STAR-FAST Calculation %.3f seconds\n", time333);
#endif

	// Match the two images begins
	MatchCombinerEliminator matchProcessor2;
	cv::Mat homographyCombined;

	std::vector<cv::DMatch> matchesCombined2;
	std::vector<cv::KeyPoint> keypointsRefCombined2, keypointsTestCombined2;


#ifdef USE_ALL_KEYPOINTS

	matchProcessor2.combineEliminateMatches(imgReference,
		imgTest,
		matches1SURFAll,
		keypointsRefSURF,
		keypointsTestSURF,
		matches1ORBAll,
		keypointsRefORB,
		keypointsTestORB,
		matches1FASTAll,
		keypointsRefFAST,
		keypointsTestFAST,
		matches1STARAll,
		keypointsRefSTAR,
		keypointsTestSTAR,
		offset,
		&matchesCombined2,
		&keypointsRefCombined2,
		&keypointsTestCombined2,
		1,
		0);

#else

	matchProcessor2.combineEliminateMatches(imgReference,
											imgTest,
											matchesSURF,
											keypointsRefSURF,
											keypointsTestSURF,
											matchesORB,
											keypointsRefORB,
											keypointsTestORB,
											matchesFAST,
											keypointsRefFAST,
											keypointsTestFAST,
											matchesSTAR,
											keypointsRefSTAR,
											keypointsTestSTAR,
											offset,
											&matchesCombined2,
											&keypointsRefCombined2,
											&keypointsTestCombined2,
											1,
											0);
#endif

#ifdef  DISPLAY_IMAGES_DEBUG
	char title22[1000];
	strcpy_s(title22, "MATCHES  After Elimination Processing");
	char sNum22[20];
	_itoa_s(kkk, sNum22, sizeof(sNum22), 10);
	strcat_s(title22, sNumSIFT);
	DisplayMatches matchDisplayer;
	matchDisplayer.displayMatchesProcessor(dRef, dTest, matchesCombined,
		keypointsRefCombined, keypointsTestCombined, title22);
#endif

	int64 tick33 = cv::getTickCount();
	// time in miliseconds
	double time33 = ((tick33 - tick1) / cv::getTickFrequency());
#ifdef DEBUG_PRINT_FINAL1
	DEBUG_PRINT("Execution Time After Elimination Processing %.3f seconds\n", time33);
#endif

	std::vector<cv::DMatch> matchesCombined3;
	std::vector<cv::KeyPoint> keypointsRefCombined3, keypointsTestCombined3;


	cv::Mat imgTestWarpedPersNormClr;


#ifdef USE_CLUSTER_CLEAN
	ImagePartitionSelector ddd;

	ddd.SelectKP(imgReferenceClr,
				 imgTestClr,
				 offset,
				 matchesCombined2,
				 keypointsRefCombined2,
				 keypointsTestCombined2,
				 &matchesCombined3,
				 &keypointsRefCombined3,
				 &keypointsTestCombined3,
				 2,
				 1);

#ifdef DISPLAY_PARTITION
	char title2233[1000];
	strcpy_s(title2233, "MATCHES  After ImagePartitionSelector");
	DisplayMatches matchDisplayer;
	matchDisplayer.displayMatchesProcessor(imgReferenceClr, imgTestClr,
		matchesCombined3,
		keypointsRefCombined3,
		keypointsTestCombined3,
		title2233);
#endif

#else

	matchesCombined3 = matchesCombined2;
	keypointsRefCombined3 = keypointsRefCombined3
		keypointsTestCombined3 = keypointsTestCombined3;

#endif

	tick333 = cv::getTickCount();
	// time in miliseconds
	time333 = ((tick333 - tick1) / cv::getTickFrequency());
#ifdef DEBUG_PRINT_FINAL1
	DEBUG_PRINT("\nExecution Time After Cluster Clean %.3f seconds\n", time333);
#endif

	//#ifdef USE_CLUSTERING
	if (dRowDiff > 500)
	{
#ifdef DEBUG_PRINT_FINAL1
		DEBUG_PRINT("CLUSTERING Begins for %d CLUSTERS\n", (int)MAX_CLUSTERS);
#endif
		ImagePartitioner zzz;


		cv::Mat img_TestWarpedPartitioned;

		zzz.PartitionImage(imgReferenceClr,
			imgTestClr,
			img_TestWarpedPartitioned,
			offset,
			matchesCombined3,
			keypointsRefCombined3,
			keypointsTestCombined3,
			7,
			1);


		imgTestWarpedPersNormClr = img_TestWarpedPartitioned.clone();
	}
	//#else
	else // process whole image
	{
#ifdef DEBUG_PRINT_FINAL1
		DEBUG_PRINT("\nSINGLE PARTITION HOMOGRAPHY\n");
#endif

		cv::Mat homographyRefined1;
		HomographyEstimator homographyEstimator;

		if (matchesCombined3.size() >= 4)
		{
			bool ishomogCalculated = false;

			ishomogCalculated =
				homographyEstimator.calculateHomography(keypointsTestCombined3,
				keypointsRefCombined3,
				HOMOGRAPHY_REPROJECT_THRESHOLD,
				matchesCombined3,
				&homographyRefined1);

			if (ishomogCalculated == false)
			{
#ifdef DEBUG_PRINT_FINAL1
				DEBUG_PRINT("\n\n\n HOMOGRAPHY can not be calculated!!!! Exiting..................");
#endif
				//getchar();
				//exit(0);
			}
			else
			{
#ifdef DEBUG_PRINT_FINAL1
				DEBUG_PRINT("\n Size after homography %d\n", matchesCombined3.size());
#endif
				int ii = 0;
				for (std::vector<cv::DMatch>::iterator matchIterator = matchesCombined3.begin();
					matchIterator != matchesCombined3.end();
					++matchIterator)
				{
					//std::DEBUG_PRINT("\n DIST Index %d X %.5f ", ii, matchIterator->distance);
					ii++;
				}
#ifdef DEBUG_PRINT_FINAL10
				DEBUG_PRINT("\n\n");
#endif
			}
		}

		int64 tick333 = cv::getTickCount();
		// time in miliseconds
		double time333 = ((tick333 - tick1) / cv::getTickFrequency());
#ifdef DEBUG_PRINT_FINAL1
		DEBUG_PRINT("\nExecution Time After Cluster Clean %.3f seconds\n", time333);
#endif

		cv::Mat* testImageNormalized;
		testImageNormalized = new cv::Mat;
		*testImageNormalized = cv::Mat::zeros(imgTest.size(), CV_8U);

		//PreprocessImage preImg4;
		//preImg4.normalizedImage(imgReference, imgTest, 1, testImageNormalized); // imageTestProcessed1

		*testImageNormalized = imgTest.clone();

		// Perspective transform using homography matrix
		// To see the registration and its quality
		//
		//TransformImage imgTransform;
		cv::Mat imgTestWarpedPersNorm;

		imgTestWarpedPersNorm = imgTransformGrey.transformPerspective(imgReference,
			*testImageNormalized,
			homographyRefined1,
			matchesCombined3,
			keypointsRefCombined3,
			keypointsTestCombined3,
			0);


		imgTestWarpedPersNormClr = imgTransformClr.transformPerspective(imgReferenceClr,
			imgTestClr,
			homographyRefined1,
			matchesCombined3,
			keypointsRefCombined3,
			keypointsTestCombined3,
			1);

		delete testImageNormalized;

#ifdef  DISPLAY_IMAGES_DEBUG
		cv::namedWindow("WARPED TEST IMAGE NORMALIZED ", cv::WINDOW_NORMAL);
		cv::imshow("WARPED TEST IMAGE NORMALIZED ", imgTestWarpedPersNormClr);
		cv::waitKey(0);
#endif
	} /// end of clustering-partitioning
	//#endif
	//

	////////////////////////////////////////////////
	cv::Mat  dChangeImage;

	SpectralAngleMapper SAMEngine;

	cv::Mat resizedImage;
	int resizedImgNo;
	resizedImage = preImgClr.resizeSmallImage(imgReferenceClr, imgTestWarpedPersNormClr, &resizedImgNo);

	tick333 = cv::getTickCount();
	// time in miliseconds
	time333 = ((tick333 - tick1) / cv::getTickFrequency());
#ifdef DEBUG_PRINT_FINAL1
	DEBUG_PRINT("\nExecution Time After Perspective Transform and Resize %.3f seconds\n", time333);
#endif

	if (resizedImgNo == 1)
	{
#ifdef DISPLAY_CONCAN_IMAGE
		DEBUG_PRINT("\nRESIZED REFERENCE SIZE W %d H %d", resizedImage.cols, resizedImage.rows);
		DEBUG_PRINT("\nTEST SIZE W %d H %d", imgTestWarpedPersNormClr.cols, imgTestWarpedPersNormClr.rows);

		ImageDisplayer imageDisplayerClr2;
		char titleClr2[1000];
		strcpy_s(titleClr2, "REF TEST-CONCAN-WRAPED RESIZED TOGETHER");
		imageDisplayerClr2.displayImages(resizedImage, imgTestWarpedPersNormClr, titleClr2);
#endif
		
		if (g_CVTestWrapped.rows != 0 || g_CVTestWrapped.cols != 0)
		{
			g_CVTestWrapped.release();
		}
		g_CVTestWrapped.create(imgTestWarpedPersNormClr.size(), imgTestWarpedPersNormClr.type());
		g_CVTestWrapped = imgTestWarpedPersNormClr.clone();

		SAMEngine.SpectralAngleMapperCalculator(resizedImage, imgTestWarpedPersNormClr, &dChangeImage);
}
	else if (resizedImgNo == 2)
	{
#ifdef DISPLAY_CONCAN_IMAGE
		DEBUG_PRINT("\nREFERENCE SIZE W %d H %d", imgReferenceClr.cols, imgReferenceClr.rows);
		DEBUG_PRINT("\nRESIZED nTEST SIZE W %d H %d", resizedImage.cols, resizedImage.rows);

		ImageDisplayer imageDisplayerClr2;
		char titleClr2[1000];
		strcpy_s(titleClr2, "REF TEST-CONCAN-WRAPED RESIZED TOGETHER");
		imageDisplayerClr2.displayImages(imgReferenceClr, resizedImage, titleClr2);
#endif
		if (g_CVTestWrapped.rows != 0 || g_CVTestWrapped.cols != 0)
		{
			g_CVTestWrapped.release();
		}
		g_CVTestWrapped.create(resizedImage.size(), resizedImage.type());
		g_CVTestWrapped = resizedImage.clone();

		SAMEngine.SpectralAngleMapperCalculator(imgReferenceClr, resizedImage, &dChangeImage);
	}
	//////////////////////////////////////////////////

	//cv::Mat FFF, GGG, HHH;

	//cvtColor(resizedImage, GGG, cv::COLOR_BGR2GRAY);

	//FalseColour colourizer;
	//HHH = colourizer.createColour(GGG);

	//bitwise_and(GGG, dChangeImage, FFF);

	//cv::namedWindow("AND", cv::WINDOW_NORMAL);
	//cv::imshow("AND", HHH);
	//cv::waitKey(0);

	int64 tick2 = cv::getTickCount();

	// time in miliseconds
	double time55 = ((tick2 - tick1) / cv::getTickFrequency());
#ifdef DEBUG_PRINT_FINAL1
	DEBUG_PRINT("\n\n TOTaL Execution Time %.3f seconds\n", time55);
#endif
	//getchar();

	return 0;

}

