#include "stdafx.h"

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\features2d\features2d.hpp"
#include "opencv2\nonfree\features2d.hpp"
#include "opencv2\calib3d\calib3d.hpp"

#include "..\ProcessingAlgorithms\TransformImage.h" 
#include "..\ProcessingAlgorithms\HistogramMatch.h"  
#include "..\ProcessingAlgorithms\PreprocessImage.h"  
#include "..\ProcessingAlgorithms\RobustMatcher.h"  
#include "..\ProcessingAlgorithms\ImagePartition.h"  
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

	cvtColor(aImgReference, imgReference, CV_BGR2GRAY);
	cvtColor(aImgTest, imgTest, CV_BGR2GRAY);
	
	aImgReference.copyTo(imgReferenceClr);
	aImgTest.copyTo(imgTestClr);

	if (!imgReference.data || !imgTest.data)
	{
		std::cout << " --(!) Error reading images " << std::endl; return -1;
	}
	//printf("\n\n Image 1 Width %d  Height %d\n", imgReference.cols, imgReference.rows);
	//printf("\n Image 2 Width %d  Height %d\n\n", imgTest.cols, imgTest.rows);

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

	printf("\n Preprocess Gray Image 1 Width %d  Height %d\n", imgReference.cols, imgReference.rows);
	printf("\n Preprocess Gray  Image 2 Width %d  Height %d\n\n", imgTest.cols, imgTest.rows);

	/// Preprocess Colour Image
	/// Includes only resize and scale down if selected
	PreprocessImage preImgClr;
	preImgClr.PreprocessStageColour(&imgReferenceClr, &imgTestClr);

	printf("\n Preprocess Colour Image 1 Width %d  Height %d\n", imgReferenceClr.cols, imgReferenceClr.rows);
	printf("\n Preprocess Colour  Image 2 Width %d  Height %d\n\n", imgTestClr.cols, imgTestClr.rows);

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

	cv::Ptr<cv::FeatureDetector> pfd5 = new cv::FastFeatureDetector();
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

	MatchCombinerEliminator matchProcessor1;
	std::vector<cv::DMatch> matchesCombined;
	std::vector<cv::KeyPoint> keypointsRefCombined, keypointsTestCombined;

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
	printf("\n SURF Matcher begins");
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
	printf("\n ORB Matcher begins");

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
	printf("\n OFFSET %d\n", imgReference.cols);

	matchProcessor1.combineEliminateMatches(imgReference,
		imgTest,
		matchesSURF,
		keypointsRefSURF,
		keypointsTestSURF,
		matchesORB,
		keypointsRefORB,
		keypointsTestORB,
		offset,
		&matchesCombined,
		&keypointsRefCombined,
		&keypointsTestCombined,
		0,
		0);

#ifdef  DISPLAY_IMAGES_DEBUG
	char title22[1000];
	strcpy_s(title22, "MATCHES  After SURF-SIFT Slope-Distance Processing");
	char sNum22[20];
	_itoa_s(kkk, sNum22, sizeof(sNum22), 10);
	strcat_s(title22, sNumSIFT);
	DisplayMatches matchDisplayer;
	matchDisplayer.displayMatchesProcessor(imgReference, imgTest, matchesCombined,
		keypointsRefCombined, keypointsTestCombined, title22);
#endif

	matchesSURF.erase(matchesSURF.begin(), matchesSURF.end());

	keypointsRefSURF.erase(keypointsRefSURF.begin(), keypointsRefSURF.end());

	keypointsTestSURF.erase(keypointsTestSURF.begin(), keypointsTestSURF.end());

	matchesORB.erase(matchesORB.begin(), matchesORB.end());

	keypointsRefORB.erase(keypointsRefORB.begin(), keypointsRefORB.end());

	keypointsTestORB.erase(keypointsTestORB.begin(), keypointsTestORB.end());

	matches1SURFAll.erase(matches1SURFAll.begin(), matches1SURFAll.end());

	keypoints1SURFAll.erase(keypoints1SURFAll.begin(), keypoints1SURFAll.end());

	matches2SURFAll.erase(matches2SURFAll.begin(), matches2SURFAll.end());

	keypoints2SURFAll.erase(keypoints2SURFAll.begin(), keypoints2SURFAll.end());

	matches1ORBAll.erase(matches1ORBAll.begin(), matches1ORBAll.end());

	keypoints1ORBAll.erase(keypoints1ORBAll.begin(), keypoints1ORBAll.end());

	matches2ORBAll.erase(matches2ORBAll.begin(), matches2ORBAll.end());

	keypoints2ORBAll.erase(keypoints2ORBAll.begin(), keypoints2ORBAll.end());

	int64 tick22 = cv::getTickCount();

	// time in miliseconds
	double time22 = ((tick22 - tick1) / cv::getTickFrequency());
	printf("\nExecution Time After SURF-ORB PROCESSING %.3f seconds\n", time22);

	// get the matches, transformed and combined image
	//cv::Mat imageTestProcessed1 = imgTest;


#ifdef  DISPLAY_PRINTS_DEBUG
	printf("\nCalculating FAST \n");
#endif

#ifdef INCLUDE_STAR
	printf("\n STAR Matcher begins");

	/// FAST MATCHER begins
	RobustMatcher rmatcherFAST;
	rmatcherFAST.setRatio(0.7f);
	rmatcherFAST.setFeatureDetector(pfd4);
	rmatcherFAST.setDescriptorExtractor(pfd44);
	rmatcherFAST.setDisplay(true);

	rmatcherFAST.match(imgReference, imgTest, // imageTestProcessed1
		matchesFAST, keypointsRefFAST, keypointsTestFAST,
		matches1FASTAll, keypoints1FASTAll,
		matches2FASTAll, keypoints2FASTAll);

#ifdef  DISPLAY_IMAGES_DEBUG_FINAL
	char titleFAST[1000];
	strcpy_s(titleFAST, "FAST ");
	//char sNumFAST[20];
	//_itoa_s(kkk, sNumFAST, sizeof(sNumFAST), 10);
	//strcat_s(titleFAST, sNumFAST);

	DisplayMatches matchDisplayerFAST;
	matchDisplayerFAST.displayMatchesProcessor(imgReference, imgTest, matchesFAST, // imageTestProcessed1
		keypointsRefFAST, keypointsTestFAST, titleFAST);
#endif
#endif

#ifdef INCLUDE_FAST
	printf("\n FAST Matcher begins");

	int64 tick3333 = cv::getTickCount();
	// time in miliseconds
	double time3333 = ((tick3333 - tick1) / cv::getTickFrequency());
	printf("\nExecution Time After FAST Calculation %.3f seconds\n", time3333);

	/// STAR MATCHER begins
	RobustMatcher rmatcherSTAR;
	rmatcherSTAR.setRatio(0.7f);
	rmatcherSTAR.setFeatureDetector(pfd5);
	rmatcherSTAR.setDescriptorExtractor(pfd55);
	rmatcherSTAR.setDisplay(true);

	rmatcherSTAR.match(imgReference, imgTest, // imageTestProcessed1
		matchesSTAR, keypointsRefSTAR, keypointsTestSTAR,
		matches1STARAll, keypoints1STARAll,
		matches2STARAll, keypoints2STARAll);

#ifdef  DISPLAY_IMAGES_DEBUG_FINAL
	char titleSTAR[1000];
	strcpy_s(titleSTAR, "STAR ");
	//char sNumSIFT[20];
	//_itoa_s(kkk, sNumSIFT, sizeof(sNumSIFT), 10);
	//strcat_s(titleSIFT, sNumSIFT);

	DisplayMatches matchDisplayerSTAR;
	matchDisplayerSTAR.displayMatchesProcessor(imgReference, imgTest, matchesSTAR, // imageTestProcessed1
		keypointsRefSTAR, keypointsTestSTAR, titleSTAR);
#endif
#endif

	int64 tick333 = cv::getTickCount();
	// time in miliseconds
	double time333 = ((tick333 - tick1) / cv::getTickFrequency());
	printf("\nExecution Time After STAR-FAST Calculation %.3f seconds\n", time333);

	// Match the two images begins
	MatchCombinerEliminator matchProcessor2;
	std::vector<cv::DMatch> matchesCombined1;
	std::vector<cv::KeyPoint> keypointsRefCombined1, keypointsTestCombined1;
	cv::Mat homographyCombined;


	matchProcessor2.combineEliminateMatches(imgReference,
		imgTest,
		matchesFAST,
		keypointsRefFAST,
		keypointsTestFAST,
		matchesSTAR,
		keypointsRefSTAR,
		keypointsTestSTAR,
		offset,
		&matchesCombined1,
		&keypointsRefCombined1,
		&keypointsTestCombined1,
		1,
		0);

#ifdef  DISPLAY_IMAGES_DEBUG
	char title22[1000];
	strcpy_s(title22, "MATCHES  After FAST-STAR Slope-Distance Processing");
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
	printf("\nExecution Time After STAR-FAST PROCESSING %.3f seconds\n", time33);

	std::vector<cv::DMatch> matchesCombined2;
	std::vector<cv::KeyPoint> keypointsRefCombined2, keypointsTestCombined2;

	MatchCombinerEliminator processFinal;
	processFinal.combineEliminateMatches(imgReference,
		imgTest,
		matchesCombined,
		keypointsRefCombined,
		keypointsTestCombined,
		matchesCombined1,
		keypointsRefCombined1,
		keypointsTestCombined1,
		offset,
		&matchesCombined2,
		&keypointsRefCombined2,
		&keypointsTestCombined2,
		2,
		1);

	int64 tick44 = cv::getTickCount();
	// time in miliseconds
	double time44 = ((tick44 - tick1) / cv::getTickFrequency());
	printf("\nExecution Time After FINAL PROCESSING %.3f seconds\n", time44);

	cv::Mat imgTestWarpedPersNormClr;

	if ((int)MAX_CLUSTERS > 1)  // more than 1 cluster, i.e., real partitioning
	{
		printf("\nCLUSTERING Begins for %d CLUSTERS\n", (int)MAX_CLUSTERS);

		ImagePartitioner ddd;

		cv::Mat img_TestWarpedPartitioned;

		ddd.PartitionImage(imgReferenceClr,
			imgTestClr,
			img_TestWarpedPartitioned,
			offset,
			matchesCombined2,
			keypointsRefCombined2,
			keypointsTestCombined2,
			2,
			1);


		imgTestWarpedPersNormClr = img_TestWarpedPartitioned.clone();
	}
	else // process whole image
	{
		printf("\nSINGLE PARTITION HOMOGRAPHY\n");

		cv::Mat homographyRefined1;
		HomographyEstimator homographyEstimator;

		if (matchesCombined2.size() >= 4)
		{
			bool ishomogCalculated = false;

			ishomogCalculated =
				homographyEstimator.calculateHomography(keypointsTestCombined2,
				keypointsRefCombined2,
				HOMOGRAPHY_REPROJECT_THRESHOLD,
				matchesCombined2,
				&homographyRefined1);

			if (ishomogCalculated == false)
			{
				printf("\n\n\n HOMOGRAPHY can not be calculated!!!! Exiting..................");
				//getchar();
				//exit(0);
			}
			else
			{
				printf("\n Size after homography %d\n\n", matchesCombined2.size());
				int ii = 0;
				for (std::vector<cv::DMatch>::iterator matchIterator = matchesCombined2.begin();
					matchIterator != matchesCombined2.end();
					++matchIterator)
				{
					//std::printf("\n DIST Index %d X %.5f ", ii, matchIterator->distance);
					ii++;
				}
				std::printf("\n\n");
			}
		}

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
			matchesCombined2,
			keypointsRefCombined2,
			keypointsTestCombined2,
			0);


		imgTestWarpedPersNormClr = imgTransformClr.transformPerspective(imgReferenceClr,
			imgTestClr,
			homographyRefined1,
			matchesCombined2,
			keypointsRefCombined2,
			keypointsTestCombined2,
			1);

#ifdef  DISPLAY_IMAGES_DEBUG
		cv::namedWindow("WARPED TEST IMAGE NORMALIZED ", cv::WINDOW_NORMAL);
		cv::imshow("WARPED TEST IMAGE NORMALIZED ", imgTestWarpedPersNormClr);
		cv::waitKey(0);
#endif
	} /// end of clustering-partitioning

	SpectralAngleMapper SAMEngine;

	cv::Mat resizedImage;
	int resizedImgNo;
	resizedImage = preImgClr.resizeSmallImage(imgReferenceClr, imgTestWarpedPersNormClr, &resizedImgNo);

	if (resizedImgNo == 1)
	{
#ifdef DISPLAY_CONCAN_IMAGE
		printf("\nRESIZED REFERENCE SIZE W %d H %d", resizedImage.cols, resizedImage.rows);
		printf("\nTEST SIZE W %d H %d", imgTestWarpedPersNormClr.cols, imgTestWarpedPersNormClr.rows);

		ImageDisplayer imageDisplayerClr2;
		char titleClr2[1000];
		strcpy_s(titleClr2, "REF TEST-CONCAN-WRAPEDT RESIZED TOGETHER");
		imageDisplayerClr2.displayImages(resizedImage, imgTestWarpedPersNormClr, titleClr2);
#endif
		SAMEngine.SpectralAngleMapperCalculator(resizedImage, imgTestWarpedPersNormClr);
	}
	else if (resizedImgNo == 2)
	{
#ifdef DISPLAY_CONCAN_IMAGE
		printf("\nREFERENCE SIZE W %d H %d", imgReferenceClr.cols, imgReferenceClr.rows);
		printf("\nRESIZED nTEST SIZE W %d H %d", resizedImage.cols, resizedImage.rows);

		ImageDisplayer imageDisplayerClr2;
		char titleClr2[1000];
		strcpy_s(titleClr2, "REF TEST-CONCAN-WRAPEDT RESIZED TOGETHER");
		imageDisplayerClr2.displayImages(imgReferenceClr, resizedImage, titleClr2);
#endif
		SAMEngine.SpectralAngleMapperCalculator(imgReferenceClr, resizedImage);
	}


	/////////
	// TEST Begin
	/*

	// Image Change Detection
	// First Method is Image Differencing
	cv::Mat differenceImageGreyMean;
	cv::Mat ratioImage;
	cv::Mat imgTestWarpedGrey;

	cv::cvtColor(imgReferenceClr, imgReference, CV_RGB2GRAY);
	cv::cvtColor(imgTestWarpedPersNormClr, imgTestWarpedGrey, CV_RGB2GRAY);

	PreprocessImage preImg3;
	preImg3.PreprocessStage(&imgReference, &imgTestWarpedGrey);

	differenceImageGreyMean = cv::Mat::zeros(imgReference.size(), cv::DataType<uchar>::type);

	ratioImage = cv::Mat::zeros(imgReference.size(), CV_8UC1);

	// Ratio Image (From Grey Images)
	DifferenceCalculator imageDifferencer;
	imageDifferencer.computeRatio(imgReference, imgTestWarpedGrey, &ratioImage);

	// Mean Difference Image (From Grey Images)
	MeanDifferenceCalculator imageMeanDifferencer;
	imageMeanDifferencer.computeDifferenceMean(imgReference, imgTestWarpedGrey, &differenceImageGreyMean, 5);
	double min, max;
	cv::minMaxLoc(differenceImageGreyMean, &min, &max);
	differenceImageGreyMean = differenceImageGreyMean + ((255 - max) / 4);

	//#ifdef  DISPLAY_IMAGES_DEBUG_DIFF
	cv::namedWindow("MEAN GREY DIFFERENCE ", cv::WINDOW_NORMAL);
	cv::imshow("MEAN GREY DIFFERENCE ", differenceImageGreyMean);
	cv::waitKey(0);

	cv::namedWindow("ORG GREY RATIO", cv::WINDOW_NORMAL);
	cv::imshow("ORG GREY RATIO", ratioImage);
	cv::waitKey(0);
	//#endif


	cv::Mat clrDiffKapurOpen;
	cv::Mat greyMeanDiffKapurOpen;
	cv::Mat greyRatioKapurOpen;

	ThreshBora imgThresholder2;
	imgThresholder2.thresholdImage(ratioImage, &greyRatioKapurOpen, 0);

	ThreshBora1 imgThresholder3;
	imgThresholder3.thresholdImage(differenceImageGreyMean, &greyMeanDiffKapurOpen, 0);

	*/
	////////
	//// TEST END

	int64 tick2 = cv::getTickCount();

	// time in miliseconds
	double time55 = ((tick2 - tick1) / cv::getTickFrequency());
	printf("\n\n TOTaL Execution Time %.3f seconds\n\n", time55);

	getchar();

	return 0;

}

