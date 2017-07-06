#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <numeric>
#include <functional>
#include <iomanip>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"

#include "HomographyEstimator.h"  
#include "Settings.h"  

class ImagePartitionSelector
{
private:

	// member variables to hold 
	// reference and test images
	cv::Mat m_imgRef;
	cv::Mat m_imgTest;

	cv::Mat m_imgTestWarpedConcan;

	struct MATCH_INFO
	{
		cv::KeyPoint refKP;
		cv::KeyPoint testKP;
		float distance;
	};

	std::vector<MATCH_INFO> m_ClusterInfo[MAX_CLUSTERS];
	cv::Point2f m_offsetKPRef[MAX_CLUSTERS];
	cv::Point2f m_offsetKPTest[MAX_CLUSTERS];

	float m_ClsRefFirstPoints[MAX_CLUSTERS];
	float m_ClsTestFirstPoints[MAX_CLUSTERS];

	float m_ClsRefLastPoints[MAX_CLUSTERS];
	float m_ClsTestLastPoints[MAX_CLUSTERS];

	std::vector<cv::DMatch> m_matchesCombinedCleaned;
	std::vector<cv::KeyPoint> m_keypointsRefCombinedCleaned;
	std::vector<cv::KeyPoint> m_keypointsTestCombinedCleaned;

	int kpCnt;
	std::vector<cv::DMatch> m_matchesPartition;
	std::vector<cv::KeyPoint> m_keypointsRefPartition;
	std::vector<cv::KeyPoint> m_keypointsTestPartition;

	std::vector<std::pair<int, float>> m_firstPointRefPairVect;
	std::vector<std::pair<int, float>> m_firstPointTestPairVect;

	std::vector<std::pair<int, float>> m_lastPointRefPairVect;
	std::vector<std::pair<int, float>> m_lastPointTestPairVect;

	cv::Mat m_partitonedRefImage[MAX_CLUSTERS];
	cv::Mat m_partitonedTestImage[MAX_CLUSTERS];

	cv::Mat m_partitonedTestImageTransformed[MAX_CLUSTERS];

	int sizeRef;
	int sizeTest;
	int sizeTestTrans;
	int m_numNEW;

	std::vector<cv::DMatch> m_matchesNEW;
	std::vector<cv::KeyPoint> m_keypointsRefNEW;
	std::vector<cv::KeyPoint> m_keypointsTestNEW;

public:



	ImagePartitionSelector() {}

	void SelectKP(  cv::Mat& imgReference,
					cv::Mat& imgTest,
					cv::Point2f offset,
					std::vector<cv::DMatch> matchesCombinedCleaned,
					std::vector<cv::KeyPoint> keypointsRefCombinedCleaned,
					std::vector<cv::KeyPoint> keypointsTestCombinedCleaned,
					std::vector<cv::DMatch>* matchesNEW,
					std::vector<cv::KeyPoint>* keypointsRefNEW,
					std::vector<cv::KeyPoint>* keypointsTestNEW,
					int aDisplayCnt = 0,
					int aThreshSelect = 0)

	{
		imgReference.copyTo(m_imgRef);
		imgTest.copyTo(m_imgTest);

#ifdef DEBUG_PRINT_FINAL1
		DEBUG_PRINT("PartitionImageSelector begins\n");
		DEBUG_PRINT("Ref  Width %d Height %d\n", m_imgRef.cols, m_imgRef.rows);
		DEBUG_PRINT("Test Width %d Height %d\n", m_imgTest.cols, m_imgTest.rows);
#endif

		//PreprocessImage preImgClr;
		//preImgClr.PreprocessStageColour(&m_imgRef, &m_imgTest);

#ifdef DEBUG_PRINT_FINAL1
		DEBUG_PRINT("PartitionImageSelector continues\n");
		DEBUG_PRINT("Ref  Width %d Height %d\n", m_imgRef.cols, m_imgRef.rows);
		DEBUG_PRINT("Test Width %d Height %d\n", m_imgTest.cols, m_imgTest.rows);
#endif

		sizeRef = 0;
		sizeTest = 0;
		sizeTestTrans = 0;

		kpCnt = 0;

		m_matchesCombinedCleaned = matchesCombinedCleaned;
		m_keypointsRefCombinedCleaned = keypointsRefCombinedCleaned;
		m_keypointsTestCombinedCleaned = keypointsTestCombinedCleaned;

		clusterAndDisplayRefKeypoints();
		clusterAndDisplayTestKeypoints();

		partitionImageRefKeypoints();

		transformPartitions();

		int k = 0;
		for (std::vector<cv::DMatch>::const_iterator it = m_matchesNEW.begin();
			it != m_matchesNEW.end();
			++it)
		{
			matchesNEW->push_back(m_matchesNEW[k]);
			keypointsRefNEW->push_back(m_keypointsRefNEW[k]);
			keypointsTestNEW->push_back(m_keypointsTestNEW[k]);
			k++;
		}
	}

	void clusterAndDisplayRefKeypoints()
	{
		int clusterCount = MAX_CLUSTERS;
		int sampleCount = (int)m_keypointsRefCombinedCleaned.size();
		std::vector<cv::Point2f> points(sampleCount);
		cv::Mat centers;

		//for (int k = 0; k < m_keypointsRefCombinedCleaned.size(); k++)
		//{
		//	points[k] = m_keypointsRefCombinedCleaned[k].pt;
		//}

		for (int k = 0; k < (int)m_keypointsRefCombinedCleaned.size(); k++)
		{
			points[k].y = m_keypointsRefCombinedCleaned[m_matchesCombinedCleaned[k].trainIdx].pt.y;
			points[k].x = 100.0;
		}


		cv::Mat img;
		cv::Mat labels;

		cv::kmeans(points, clusterCount, labels,
			cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0),
			3, cv::KMEANS_RANDOM_CENTERS, centers);


		MATCH_INFO matchData;
		int k = 0;
		for (std::vector<cv::DMatch>::iterator matchIterator = m_matchesCombinedCleaned.begin();
			matchIterator != m_matchesCombinedCleaned.end();
			++matchIterator)
		{
			matchData.refKP = m_keypointsRefCombinedCleaned[m_matchesCombinedCleaned[k].trainIdx];
			matchData.testKP = m_keypointsTestCombinedCleaned[m_matchesCombinedCleaned[k].queryIdx];

			matchData.distance = matchIterator->distance;

			m_ClusterInfo[labels.at<int>(k)].push_back(matchData);
			k++;
		}

#ifdef DISPLAY_PARTITION_REF_KP
		cv::Mat resMatches;
		std::vector<cv::DMatch> matchB;
		std::vector<cv::KeyPoint> KP1, KP2;

		int clsCntr[MAX_CLUSTERS];

		for (int k = 0; k <MAX_CLUSTERS; k++)
		{
			clsCntr[k] = 0;
		}

		int kk = 0;

		for (std::vector<cv::DMatch>::iterator matchIterator = m_matchesCombinedCleaned.begin();
			matchIterator != m_matchesCombinedCleaned.end();
			++matchIterator)
		{
			matchB.push_back(cv::DMatch(kk, kk,
				(*matchIterator).distance));

			KP1.push_back(m_keypointsRefCombinedCleaned[m_matchesCombinedCleaned[kk].trainIdx]);
			KP2.push_back(m_keypointsTestCombinedCleaned[m_matchesCombinedCleaned[kk].queryIdx]);  // queryIdx
			kk++;
		}

		cv::Scalar matchColours = (cv::Scalar(255, 255, 255));
		int clusterIdx;

		cv::drawMatches(m_imgRef, KP1, m_imgTest, KP2,
			matchB, resMatches,
			matchColours, cv::Scalar::all(-1),
			std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
		for (int i = 0; i < (int)matchB.size(); i++)
		{
			clusterIdx = labels.at<int>(i);

			clsCntr[clusterIdx]++;

			if (clusterIdx == 0)
				matchColours = (cv::Scalar(255, 255, 255));
			else if (clusterIdx == 1)
				matchColours = (cv::Scalar(127, 127, 255));
			else if (clusterIdx == 2)
				matchColours = (cv::Scalar(255, 0, 0));
			else if (clusterIdx == 3)
				matchColours = (cv::Scalar(0, 255, 0));
			else if (clusterIdx == 4)
				matchColours = (cv::Scalar(0, 0, 255));
			else if (clusterIdx == 5)
				matchColours = (cv::Scalar(0, 255, 255));
			else if (clusterIdx == 6)
				matchColours = (cv::Scalar(255, 0, 255));

			cv::Point2f offset((float)m_imgRef.cols, 0);
			cv::circle(resMatches, KP1[i].pt, 2, matchColours, 5);
			cv::circle(resMatches, KP2[i].pt + offset, 2, matchColours, 5);
		}

		cv::namedWindow("CLUSTER KEYPOINT REF", cv::WINDOW_NORMAL);
		cv::imshow("CLUSTER KEYPOINT REF", resMatches);
		cv::waitKey(0);

		DEBUG_PRINT("Clusters for REF Points\n");
		for (int kkk = 0; kkk <MAX_CLUSTERS; kkk++)
		{
			DEBUG_PRINT("\n  CLS_ID %d CLS_CNT %d", kkk, clsCntr[kkk]);
		}
		DEBUG_PRINT("\n");
#endif

	}

	void determineClusterOrderOnImage()
	{
		std::pair<int, float> tmpPair;
		float dYPoint;

		////// sort reference points
		/// first and last points are sorted
		for (int i = 0; i < MAX_CLUSTERS; i++)
		{
			dYPoint = m_ClsRefFirstPoints[i];
			tmpPair.first = i;
			tmpPair.second = dYPoint;
			m_firstPointRefPairVect.push_back(tmpPair);

			dYPoint = m_ClsRefLastPoints[i];
			tmpPair.first = i;
			tmpPair.second = dYPoint;
			m_lastPointRefPairVect.push_back(tmpPair);
		}

		// sort in decreasing order
		struct sort_pred
		{
			bool operator()(const std::pair<int, float> &left, const std::pair<int, float> &right)
			{
				return left.second > right.second;
			}
		};

		std::sort(m_firstPointRefPairVect.begin(), m_firstPointRefPairVect.end(), sort_pred());
		std::sort(m_lastPointRefPairVect.begin(), m_lastPointRefPairVect.end(), sort_pred());

#ifdef DISPLAY_DEBUG_PARTITION
		DEBUG_PRINT("\nm_firstPointRefPairVect\n");
		for (int i = 0; i < MAX_CLUSTERS; i++)
		{
			DEBUG_PRINT("m_firstPointRefPairVect %d %d %f\n", i, m_firstPointRefPairVect[i].first, m_firstPointRefPairVect[i].second);
		}

		DEBUG_PRINT("\nm_lastPointRefPairVect\n");
		for (int i = 0; i < MAX_CLUSTERS; i++)
		{
			DEBUG_PRINT("m_lastPointRefPairVect %d %d %f\n", i, m_lastPointRefPairVect[i].first, m_lastPointRefPairVect[i].second);
		}
#endif

#ifdef DISPLAY_PARTITION
		/// debug code begins
		int AkpCnt = 0;
		std::vector<cv::DMatch> A_matchesPartition;
		std::vector<cv::KeyPoint> A_keypointsRefPartition;
		std::vector<cv::KeyPoint> A_keypointsTestPartition;

		tmpPair = m_firstPointRefPairVect[0];

#ifdef DISPLAY_DEBUG_PARTITION
		DEBUG_PRINT("\n\nBORAAAAAAA 1 %d %f %f \n\n", m_firstPointRefPairVect[0].first, m_firstPointRefPairVect[0].second, m_keypointsRefPartition[2 * tmpPair.first].pt.y);
#endif
		// add first point
		A_keypointsRefPartition.push_back(m_keypointsRefPartition[2 * tmpPair.first]);
		A_keypointsTestPartition.push_back(m_keypointsTestPartition[2 * tmpPair.first]);
		A_matchesPartition.push_back(cv::DMatch(AkpCnt, AkpCnt, 0));
		AkpCnt++;

		tmpPair = m_lastPointRefPairVect[0];

#ifdef DISPLAY_DEBUG_PARTITION
		DEBUG_PRINT("\n\nBORAAAAAAA 2 %d %f %f\n\n", m_lastPointRefPairVect[0].first, m_lastPointRefPairVect[0].second, m_keypointsRefPartition[2 * tmpPair.first + 1].pt.y);
#endif
		// add first point
		A_keypointsRefPartition.push_back(m_keypointsRefPartition[2 * tmpPair.first + 1]);
		A_keypointsTestPartition.push_back(m_keypointsTestPartition[2 * tmpPair.first + 1]);
		A_matchesPartition.push_back(cv::DMatch(AkpCnt, AkpCnt, 0));
		AkpCnt++;

		cv::Mat resMatches;
		cv::Scalar matchColours = (cv::Scalar(255, 255, 127));

		cv::drawMatches(m_imgRef, A_keypointsRefPartition, m_imgTest, A_keypointsTestPartition,
			A_matchesPartition, resMatches,
			matchColours, cv::Scalar::all(-1),
			std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

		cv::namedWindow("FIRST LAST KP DEBUG", cv::WINDOW_NORMAL);
		cv::imshow("FIRST LAST KP DEBUG", resMatches);
		cv::waitKey(0);
#endif
		/// debug code ends
		//// end of reference points sorting

		/////////////////////////////////
		////// sort test points
		/// first and last points are sorted 
		for (int i = 0; i < MAX_CLUSTERS; i++)
		{
			dYPoint = m_ClsTestFirstPoints[i];
			tmpPair.first = i;
			tmpPair.second = dYPoint;
			m_firstPointTestPairVect.push_back(tmpPair);

			dYPoint = m_ClsTestLastPoints[i];
			tmpPair.first = i;
			tmpPair.second = dYPoint;
			m_lastPointTestPairVect.push_back(tmpPair);
		}

		std::sort(m_firstPointTestPairVect.begin(), m_firstPointTestPairVect.end(), sort_pred());
		std::sort(m_lastPointTestPairVect.begin(), m_lastPointTestPairVect.end(), sort_pred());
		//// end of test points sorting
	}

	void determineClusterFirstLastPointsRef(int clsID, std::vector<std::pair<int, float>> keyPointPairVect)
	{
		std::pair<int, float> tmpPair;

		std::vector<MATCH_INFO>::iterator matchIterator = m_ClusterInfo[clsID].begin();

		tmpPair = keyPointPairVect[0];
		matchIterator += tmpPair.first;
		m_ClsRefFirstPoints[clsID] = matchIterator->refKP.pt.y;
		// or only
		//m_ClsRefFirstPoints[clsID].push_back(tmpPair.second);

		// add first point
		m_keypointsRefPartition.push_back(matchIterator->refKP);
		m_keypointsTestPartition.push_back(matchIterator->testKP);
		m_matchesPartition.push_back(cv::DMatch(kpCnt, kpCnt, 0));
		// matchIterator->distance does not exist here

		///
#ifdef DISPLAY_DEBUG_PARTITION
		DEBUG_PRINT("\ndetermineClusterFirstLastPointsRef CLS_ID %d \n", clsID);
		DEBUG_PRINT("CLS REF First Point %f\n", matchIterator->refKP.pt.y);
		DEBUG_PRINT("CLS Test First Point %f\n", matchIterator->testKP.pt.y);
#endif
		///

		kpCnt++;

		// line below is only for the time being
		// in fact I have to write a sort for test points and then determine the 
		// first and last points.
		// then I have to  evaluate if the orders are correct.
		m_ClsTestFirstPoints[clsID] = matchIterator->testKP.pt.y;


		matchIterator = m_ClusterInfo[clsID].begin();
		tmpPair = keyPointPairVect[keyPointPairVect.size() - 1];
		matchIterator += tmpPair.first;
		m_ClsRefLastPoints[clsID] = matchIterator->refKP.pt.y;

		// line below is only for the time being
		// in fact I have to write a sort for test points and then determine the 
		// first and last points.
		// then I have to  evaluate if the orders are correct.
		m_ClsTestLastPoints[clsID] = matchIterator->testKP.pt.y;

		// add last point
		m_keypointsRefPartition.push_back(matchIterator->refKP);
		m_keypointsTestPartition.push_back(matchIterator->testKP);
		m_matchesPartition.push_back(cv::DMatch(kpCnt, kpCnt, 0));
		//matchIterator->distance does not exist here
		kpCnt++;

#ifdef DISPLAY_DEBUG_PARTITION
		DEBUG_PRINT("\ndetermineClusterFirstLastPointsRef CLS_ID %d NUM_POINTS %d\n", clsID, kpCnt);
		DEBUG_PRINT("CLS REF First Point %f\n", m_ClsRefFirstPoints[clsID]);
		DEBUG_PRINT("CLS Test First Point %f\n", m_ClsTestFirstPoints[clsID]);
		DEBUG_PRINT("CLS REF Last Point %f\n", m_ClsRefLastPoints[clsID]);
		DEBUG_PRINT("CLS Test Last Point %f\n", m_ClsTestLastPoints[clsID]);

		///
		DEBUG_PRINT("\ndetermineClusterFirstLastPointsRef CLS_ID %d \n", clsID);
		DEBUG_PRINT("CLS REF Last Point %f\n", matchIterator->refKP.pt.y);
		DEBUG_PRINT("CLS Test Last Point %f\n", matchIterator->testKP.pt.y);
#endif
		///
	}

	void drawFirstLast()
	{
		cv::Mat resMatches;
		cv::Scalar matchColours = (cv::Scalar(255, 255, 127));

		cv::drawMatches(m_imgRef, m_keypointsRefPartition, m_imgTest, m_keypointsTestPartition,
			m_matchesPartition, resMatches,
			matchColours, cv::Scalar::all(-1),
			std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

		for (int i = 0; i < (int)m_matchesPartition.size(); i++)
		{
			if (i == 0)
				matchColours = (cv::Scalar(255, 255, 255));
			else if (i == 1)
				matchColours = (cv::Scalar(255, 255, 255));
			else if (i == 2)
				matchColours = (cv::Scalar(0, 0, 255));
			else if (i == 3)
				matchColours = (cv::Scalar(0, 0, 255));
			else if (i == 4)
				matchColours = (cv::Scalar(0, 255, 0));
			else if (i == 5)
				matchColours = (cv::Scalar(0, 255, 0));
			else if (i == 6)
				matchColours = (cv::Scalar(255, 0, 0));
			if (i == 7)
				matchColours = (cv::Scalar(255, 0, 0));
			else if (i == 8)
				matchColours = (cv::Scalar(255, 255, 0));
			else if (i == 9)
				matchColours = (cv::Scalar(255, 255, 0));
			else if (i == 10)
				matchColours = (cv::Scalar(255, 0, 255));
			else if (i == 11)
				matchColours = (cv::Scalar(255, 0, 255));
			else if (i == 12)
				matchColours = (cv::Scalar(127, 127, 127));
			else if (i == 13)
				matchColours = (cv::Scalar(127, 127, 127));

			cv::Point2f offset((float)m_imgRef.cols, 0);
			cv::circle(resMatches, m_keypointsRefPartition[i].pt, 2, matchColours, 10);
			cv::circle(resMatches, m_keypointsTestPartition[i].pt + offset, 2, matchColours, 10);
		}

		cv::namedWindow("FIRST LAST KP", cv::WINDOW_NORMAL);
		cv::imshow("FIRST LAST KP", resMatches);
		cv::waitKey(0);
	}

	void findClusterExtremumPointsRef(int clsID)
	{
		std::vector<std::pair<int, float>> keyPointRefPairVect;
		std::vector<std::pair<int, float>> keyPointTestPairVect;
		std::pair<int, float> tmpPair;
		float dYPoint;

		int i = 0;
		for (std::vector<MATCH_INFO>::iterator matchIterator = m_ClusterInfo[clsID].begin();
			matchIterator != m_ClusterInfo[clsID].end();
			++matchIterator)
		{
			dYPoint = matchIterator->refKP.pt.y;

			tmpPair.first = i;
			tmpPair.second = dYPoint;

			keyPointRefPairVect.push_back(tmpPair);

			i++;
		}

		// sort function
		// sort in decreasing order
		struct sort_pred
		{
			bool operator()(const std::pair<int, float> &left, const std::pair<int, float> &right)
			{
				return left.second > right.second;
			}
		};

		std::sort(keyPointRefPairVect.begin(), keyPointRefPairVect.end(), sort_pred());

#ifdef DISPLAY_DEBUG_PARTITION
		DEBUG_PRINT("\nfindClusterExtremumPointsRef CLS_ID %d\n", clsID);
		for (int k = 0; k < keyPointRefPairVect.size(); k++)
		{
			DEBUG_PRINT("INDEX %d point %f\n", keyPointRefPairVect[k].first, keyPointRefPairVect[k].second);
		}
#endif

		determineClusterFirstLastPointsRef(clsID, keyPointRefPairVect);
	}

	void findTestHighestPartition()
	{
		/// image coordinates are as follows
		/// x1,y1 is upper-leftmost coordinate
		/// x4,y4 is the lower-rightmost coordinate 
		/// x1,y1 ...................... x2,y2
		/// ..........
		/// ..........
		/// .........
		/// x3,y3 ...................... x4,y4

		int val1 = (int)(m_lastPointTestPairVect[MAX_CLUSTERS - 2].second + 0.0);
		int val2 = (int)(m_firstPointTestPairVect[MAX_CLUSTERS - 1].second + 0.0);
		int diff = val1 - val2;

		int offSet;

		if (diff % 2 == 0) // if even number
		{
			offSet = diff / 2;
		}
		else
		{
			offSet = (int)(diff / 2) + 1;
		}

#ifdef DISPLAY_DEBUG_PARTITION
		DEBUG_PRINT("\nTEST MATTTT\n");
		DEBUG_PRINT("\nTEST IMAGE width %d height %d\n", m_imgTest.cols, m_imgTest.rows);
		DEBUG_PRINT("TEST m_firstPointRefPairVect SIZE %d\n", m_firstPointTestPairVect.size());
		DEBUG_PRINT("TEST m_lastPointRefPairVect SIZE %d\n", m_lastPointTestPairVect.size());
#endif

#ifdef DISPLAY_DEBUG_PARTITION_LESS
		DEBUG_PRINT("\n\n");
		DEBUG_PRINT("\nTEST highest m_lastPointTestPairVect[MAX_CLUSTERS - 2] %d", (int)(m_lastPointTestPairVect[MAX_CLUSTERS - 2].second + 0.0));
		DEBUG_PRINT("\nTEST highest m_firstPointTestPairVect[MAX_CLUSTERS - 1] %d", (int)(m_firstPointTestPairVect[MAX_CLUSTERS - 1].second + 0.0));
		DEBUG_PRINT("\nTEST highest DIFF %d", diff);
		DEBUG_PRINT("\nTEST highest offsef %d", offSet);
		DEBUG_PRINT("\nTEST heighest begin %d", 0);
		DEBUG_PRINT("\nTEST highest WIDTH %d\n", (int)m_firstPointTestPairVect[MAX_CLUSTERS - 1].second + offSet + 1);
#endif
		// matrix --> submatrix
		m_partitonedTestImage[MAX_CLUSTERS - 1] = m_imgTest(cv::Rect(0,  // x
			0,  // y
			m_imgTest.cols,  // width
			(int)m_firstPointTestPairVect[MAX_CLUSTERS - 1].second + offSet + 1)).clone();  // height

		m_offsetKPTest[MAX_CLUSTERS - 1].x = (float)0;
		m_offsetKPTest[MAX_CLUSTERS - 1].y = (float)0;

		sizeTest += m_partitonedTestImage[MAX_CLUSTERS - 1].rows;

#ifdef DISPLAY_DEBUG_PARTITION_LESS
		DEBUG_PRINT("\n findTestHighestPartition W %d H %d", m_partitonedTestImage[MAX_CLUSTERS - 1].cols, m_partitonedTestImage[MAX_CLUSTERS - 1].rows);
#endif

#ifdef DISPLAY_PARTITION
		cv::namedWindow("m_partitonedTestImage[MAX_CLUSTERS-1]", cv::WINDOW_NORMAL);
		cv::imshow("m_partitonedTestImage[MAX_CLUSTERS-1]", m_partitonedTestImage[MAX_CLUSTERS - 1]);
		cv::waitKey(0);
#endif
	}

	// lowest partiton is the part with the greatest coordinates (x and y values)
	// this is the lowest part of the image on the screen
	// these values are the first values in the sorted data
	void findTestLowestPartition()
	{
		int val1 = (int)(m_lastPointTestPairVect[0].second + 0.0);
		int val2 = (int)(m_firstPointTestPairVect[1].second + 0.0);
		int diff = val1 - val2;

		int offSet;

		offSet = (int)(diff / 2);

#ifdef DISPLAY_DEBUG_PARTITION_LESS
		DEBUG_PRINT("\n\n");
		DEBUG_PRINT("\nTEST lowest m_lastPointTestPairVect[0] %d", (int)(m_lastPointTestPairVect[0].second + 0.0));
		DEBUG_PRINT("\nTEST lowest m_firstPointTestPairVect[1] %d", (int)(m_firstPointTestPairVect[1].second + 0.0));
		DEBUG_PRINT("\nTEST lowest DIFF %d", diff);
		DEBUG_PRINT("\nTEST lowest offsef %d", offSet);
		DEBUG_PRINT("\nTEST lowest begin %d\n", (int)m_lastPointTestPairVect[0].second - offSet + 1);
		DEBUG_PRINT("\nTEST lowest WIDTH %d\n", m_imgTest.rows - (int)(m_lastPointTestPairVect[0].second) + offSet - 1);
#endif

		m_partitonedTestImage[0] = m_imgTest(cv::Rect(0,  // x
			(int)m_lastPointTestPairVect[0].second - offSet + 1, // y
			m_imgTest.cols,  // width
			m_imgTest.rows - (int)(m_lastPointTestPairVect[0].second) + offSet - 1)).clone(); // height

		m_offsetKPTest[0].x = (float)0;
		m_offsetKPTest[0].y = (float)(m_lastPointTestPairVect[0].second - (float)offSet + 1);

		sizeTest += m_partitonedTestImage[0].rows;

#ifdef DISPLAY_DEBUG_PARTITION_LESS
		DEBUG_PRINT("\n findTestLowestPartition W %d H %d", m_partitonedTestImage[0].cols, m_partitonedTestImage[0].rows);
#endif


#ifdef DISPLAY_PARTITION
		cv::namedWindow("m_partitonedTestImage[0]", cv::WINDOW_NORMAL);
		cv::imshow("m_partitonedTestImage[0]", m_partitonedTestImage[0]);
		cv::waitKey(0);
#endif
	}

	void findTestOtherPartitions()
	{
		int offSetLower = 0;
		int offSetHigher = 0;
		int ptYBegin = 0;
		int ptYEnd = 0;
		int dWidth = 0;

		int val1;
		int val2;
		int diff;

		for (int i = 1; i < MAX_CLUSTERS - 1; i++)
		{
			val1 = (int)(m_lastPointTestPairVect[i - 1].second + 0.0);
			val2 = (int)(m_firstPointTestPairVect[i].second + 0.0);
			diff = val1 - val2;

			if (diff % 2 == 0) // if even number
			{
				offSetLower = diff / 2;
			}
			else
			{
				offSetLower = (int)(diff / 2) + 1;
			}

			val1 = (int)(m_lastPointTestPairVect[i].second + 0.0);
			val2 = (int)(m_firstPointTestPairVect[i + 1].second + 0.0);
			diff = val1 - val2;

			if (diff % 2 == 0) // if even number
			{
				offSetHigher = diff / 2;
			}
			else
			{
				offSetHigher = (int)(diff / 2) - 0;
			}

			ptYBegin = (int)m_lastPointTestPairVect[i].second - offSetHigher + 1;

			ptYEnd = (int)m_firstPointTestPairVect[i].second + offSetLower;

			dWidth = ptYEnd - ptYBegin + 1;

			m_partitonedTestImage[i] = m_imgTest(cv::Rect(0, // x
				ptYBegin, // y
				m_imgTest.cols,  // width
				dWidth)).clone();  // height

#ifdef DISPLAY_DEBUG_PARTITION_LESS
			DEBUG_PRINT("\n");
			DEBUG_PRINT("TEST offSetLower %d\n", offSetLower);
			DEBUG_PRINT("TEST offSetHigher %d\n", offSetHigher);
			DEBUG_PRINT("TEST ptYBegin %d\n", ptYBegin);
			DEBUG_PRINT("TEST ptYEnd %d\n", ptYEnd);
			DEBUG_PRINT("TEST dWidth %d\n", dWidth);
			DEBUG_PRINT("TEST m_lastPointTestPairVect[i - 1].second %f\n", m_lastPointTestPairVect[i - 1].second);
			DEBUG_PRINT("TEST m_firstPointTestPairVect[i].second %f\n", m_firstPointTestPairVect[i].second);
			DEBUG_PRINT("TEST m_lastPointTestPairVect[i].second %f\n", m_lastPointTestPairVect[i].second);
			DEBUG_PRINT("TEST m_firstPointTestPairVect[i+1].second %f\n", m_firstPointTestPairVect[i + 1].second);
#endif

			m_offsetKPTest[i].x = (float)0;
			m_offsetKPTest[i].y = (float)ptYBegin;

			sizeTest += m_partitonedTestImage[i].rows;

#ifdef DISPLAY_DEBUG_PARTITION_LESS
			DEBUG_PRINT("\n findTestOtherPartitions %d  W %d H %d", i, m_partitonedTestImage[i].cols, m_partitonedTestImage[i].rows);
#endif

#ifdef DISPLAY_PARTITION
			char title[1000];
			strcpy_s(title, "m_partitonedTestImage ");

			strcat_s(title, " ");
			char sNumCnt[20];
			_itoa_s(i, sNumCnt, sizeof(sNumCnt), 10);
			strcat_s(title, sNumCnt);

			cv::namedWindow(title, cv::WINDOW_NORMAL);
			cv::imshow(title, m_partitonedTestImage[i]);
			cv::waitKey(0);
#endif
		}

	}

	void performTestImagePartition()
	{
		findTestHighestPartition();

		findTestLowestPartition();

		findTestOtherPartitions();
	}

	// highest partiton is the part with the smallest coordinates (x and y values)
	// this is the highest part of the image on the screen
	// these values are the last values in the sorted data
	void findRefHighestPartition()
	{
		/// image coordinates are as follows
		/// x1,y1 is upper-leftmost coordinate
		/// x4,y4 is the lower-rightmost coordinate 
		/// x1,y1 ...................... x2,y2
		/// ..........
		/// ..........
		/// .........
		/// x3,y3 ...................... x4,y4

		int val1 = (int)(m_lastPointRefPairVect[MAX_CLUSTERS - 2].second + 0.0);
		int val2 = (int)(m_firstPointRefPairVect[MAX_CLUSTERS - 1].second + 0.0);
		int diff = val1 - val2;

		int offSet;

		if (diff % 2 == 0) // if even number
		{
			offSet = diff / 2;
		}
		else
		{
			offSet = (int)(diff / 2) + 1;
		}


#ifdef DISPLAY_DEBUG_PARTITION_LESS
		DEBUG_PRINT("\n\n");
		DEBUG_PRINT("\nREF highest m_lastPointRefPairVect[MAX_CLUSTERS - 2] %d", (int)m_lastPointRefPairVect[MAX_CLUSTERS - 2].second);
		DEBUG_PRINT("\nREF highest m_firstPointRefPairVect[MAX_CLUSTERS - 1] %d", (int)m_firstPointRefPairVect[MAX_CLUSTERS - 1].second);
		DEBUG_PRINT("\nREf highest DIFF %d", diff);
		DEBUG_PRINT("\nREF highest offsef %d", offSet);
		DEBUG_PRINT("\nREF heighest begin %d", 0);
		DEBUG_PRINT("\nREF highest WIDTH %d", (int)m_firstPointRefPairVect[MAX_CLUSTERS - 1].second + offSet + 1);
#endif


		// matrix --> submatrix
		m_partitonedRefImage[MAX_CLUSTERS - 1] = m_imgRef(cv::Rect(0,  // x
			0,  // y
			m_imgRef.cols,  // width
			(int)m_firstPointRefPairVect[MAX_CLUSTERS - 1].second + offSet + 1)).clone();  // height

		m_offsetKPRef[MAX_CLUSTERS - 1].x = (float)0;
		m_offsetKPRef[MAX_CLUSTERS - 1].y = (float)0;

		sizeRef += m_partitonedRefImage[MAX_CLUSTERS - 1].rows;

#ifdef DISPLAY_DEBUG_PARTITION_LESS
		DEBUG_PRINT("\nfindRefHighestPartition  W %d H %d\n", m_partitonedRefImage[MAX_CLUSTERS - 1].cols, m_partitonedRefImage[MAX_CLUSTERS - 1].rows);
#endif

#ifdef DISPLAY_PARTITION
		cv::namedWindow("m_partitonedRefImage[MAX_CLUSTERS-1]", cv::WINDOW_NORMAL);
		cv::imshow("m_partitonedRefImage[MAX_CLUSTERS-1]", m_partitonedRefImage[MAX_CLUSTERS - 1]);
		cv::waitKey(0);
#endif
	}

	// lowest partiton is the part with the greatest coordinates (x and y values)
	// this is the lowest part of the image on the screen
	// these values are the first values in the sorted data

	void findRefLowestPartition()
	{
		int val1 = (int)(m_lastPointRefPairVect[0].second + 0.0);
		int val2 = (int)(m_firstPointRefPairVect[1].second + 0.0);
		int diff = val1 - val2;

		int offSet;

		offSet = (int)(diff / 2);

#ifdef DISPLAY_DEBUG_PARTITION_LESS
		DEBUG_PRINT("\n\n");
		DEBUG_PRINT("\nREF lowest m_lastPointRefPairVect[0] %d", (int)m_lastPointRefPairVect[0].second);
		DEBUG_PRINT("\nREF lowest m_firstPointRefPairVect[1] %d", (int)m_firstPointRefPairVect[1].second);
		DEBUG_PRINT("\nREF lowest DIFF %d", diff);
		DEBUG_PRINT("\nREF lowest offsef %d", offSet);
		DEBUG_PRINT("\nREF lowest begin %d", (int)m_lastPointRefPairVect[0].second - offSet + 1);
		DEBUG_PRINT("\nref lowest WIDTH %d\n", m_imgRef.rows - (int)(m_lastPointRefPairVect[0].second) + offSet - 1);
#endif

		m_partitonedRefImage[0] = m_imgRef(cv::Rect(0,  // x
			(int)m_lastPointRefPairVect[0].second - offSet + 1, // y
			m_imgRef.cols,  // width
			m_imgRef.rows - (int)(m_lastPointRefPairVect[0].second) + offSet - 1)).clone(); // height

		m_offsetKPRef[0].x = (float)0;
		m_offsetKPRef[0].y = (float)(m_lastPointRefPairVect[0].second - (float)offSet + 1);

		sizeRef += m_partitonedRefImage[0].rows;

#ifdef DISPLAY_DEBUG_PARTITION_LESS
		DEBUG_PRINT("findRefLowestPartition W %d H %d\n", m_partitonedRefImage[0].cols, m_partitonedRefImage[0].rows);
#endif

#ifdef DISPLAY_PARTITION
		cv::namedWindow("m_partitonedRefImage[0]", cv::WINDOW_NORMAL);
		cv::imshow("m_partitonedRefImage[0]", m_partitonedRefImage[0]);
		cv::waitKey(0);
#endif
	}

	void findRefOtherPartitions()
	{
		int offSetLower = 0;
		int offSetHigher = 0;
		int ptYBegin = 0;
		int ptYEnd = 0;
		int dWidth = 0;

		int val1;
		int val2;
		int diff;

		for (int i = 1; i < MAX_CLUSTERS - 1; i++)
		{
			val1 = (int)(m_lastPointRefPairVect[i - 1].second + 0.0);
			val2 = (int)(m_firstPointRefPairVect[i].second + 0.0);
			diff = val1 - val2;

			if (diff % 2 == 0) // if even number
			{
				offSetLower = diff / 2;
			}
			else
			{
				offSetLower = (int)(diff / 2) + 1;
			}


			val1 = (int)(m_lastPointRefPairVect[i].second + 0.0);
			val2 = (int)(m_firstPointRefPairVect[i + 1].second + 0.0);
			diff = val1 - val2;

			if (diff % 2 == 0) // if even number
			{
				offSetHigher = diff / 2;
			}
			else
			{
				offSetHigher = (int)(diff / 2) - 0;
			}

			ptYBegin = (int)m_lastPointRefPairVect[i].second - offSetHigher + 1;

			ptYEnd = (int)m_firstPointRefPairVect[i].second + offSetLower;

			dWidth = ptYEnd - ptYBegin + 1;

			m_partitonedRefImage[i] = m_imgRef(cv::Rect(0, // x
				ptYBegin, // y
				m_imgRef.cols,  // width
				dWidth)).clone();  // height

			m_offsetKPRef[i].x = (float)0;
			m_offsetKPRef[i].y = (float)ptYBegin;

			sizeRef += m_partitonedRefImage[i].rows;

#ifdef DISPLAY_DEBUG_PARTITION_LESS
			DEBUG_PRINT("\n");
			DEBUG_PRINT("REF offSetLower %d\n", offSetLower);
			DEBUG_PRINT("REF offSetHigher %d\n", offSetHigher);
			DEBUG_PRINT("REF ptYBegin %d\n", ptYBegin);
			DEBUG_PRINT("REF ptYEnd %d\n", ptYEnd);
			DEBUG_PRINT("REF dWidth %d\n", dWidth);
			DEBUG_PRINT("REF m_lastPointRefPairVect[i - 1].second %f\n", m_lastPointRefPairVect[i - 1].second);
			DEBUG_PRINT("REF m_firstPointRefPairVect[i].second %f\n", m_firstPointRefPairVect[i].second);
			DEBUG_PRINT("REF m_lastPointRefPairVect[i].second %f\n", m_lastPointRefPairVect[i].second);
			DEBUG_PRINT("REF m_firstPointRefPairVect[i+1].second %f\n", m_firstPointRefPairVect[i + 1].second);
			DEBUG_PRINT("REF findRefOtherPartitions %d  W %d H %d\n", i, m_partitonedRefImage[i].cols, m_partitonedRefImage[i].rows);
#endif

#ifdef DISPLAY_PARTITION
			char title[1000];
			strcpy_s(title, "m_partitonedRefImage ");

			strcat_s(title, " ");
			char sNumCnt[20];
			_itoa_s(i, sNumCnt, sizeof(sNumCnt), 10);
			strcat_s(title, sNumCnt);

			cv::namedWindow(title, cv::WINDOW_NORMAL);
			cv::imshow(title, m_partitonedRefImage[i]);
			cv::waitKey(0);
#endif
		}

	}

	void performRefImagePartition()
	{
		findRefHighestPartition();

		findRefLowestPartition();

		findRefOtherPartitions();
	}
	void partitionImageRefKeypoints()
	{
		for (int i = 0; i < MAX_CLUSTERS; i++)
		{
			findClusterExtremumPointsRef(i);
		}

#ifdef DISPLAY_PARTITION
		drawFirstLast();
#endif

		determineClusterOrderOnImage();

		performRefImagePartition();

		performTestImagePartition();
	}

	void clusterAndDisplayTestKeypoints()
	{
		int clusterCount = MAX_CLUSTERS;
		int sampleCount = (int)m_keypointsTestCombinedCleaned.size();
		std::vector<cv::Point2f> points(sampleCount);
		cv::Mat centers;

		//for (int k = 0; k < m_keypointsTestCombinedCleaned.size(); k++)
		//{
		//	points[k] = m_keypointsTestCombinedCleaned[k].pt;
		//}

		for (int k = 0; k < (int)m_keypointsTestCombinedCleaned.size(); k++)
		{
			points[k].y = m_keypointsTestCombinedCleaned[m_matchesCombinedCleaned[k].trainIdx].pt.y;
			points[k].x = 100.0;
		}


		cv::Mat img;
		cv::Mat labels;

		cv::kmeans(points, clusterCount, labels,
			cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0),
			3, cv::KMEANS_PP_CENTERS, centers);


#ifdef DISPLAY_PARTITION
		cv::Mat resMatches;
		std::vector<cv::DMatch> matchB;
		std::vector<cv::KeyPoint> KP1, KP2;


		int k = 0;
		for (std::vector<cv::DMatch>::iterator matchIterator = m_matchesCombinedCleaned.begin();
			matchIterator != m_matchesCombinedCleaned.end();
			++matchIterator)
		{
			{
				matchB.push_back(cv::DMatch(k, k,
					(*matchIterator).distance));

				KP1.push_back(m_keypointsRefCombinedCleaned[m_matchesCombinedCleaned[k].trainIdx]);
				KP2.push_back(m_keypointsTestCombinedCleaned[m_matchesCombinedCleaned[k].queryIdx]);  // queryIdx
				k++;
			}
		}

		cv::Scalar matchColours;
		int clusterIdx;

		int clsCntr[MAX_CLUSTERS];

		for (int k = 0; k <MAX_CLUSTERS; k++)
		{
			clsCntr[k] = 0;
		}

		cv::drawMatches(m_imgRef, KP1, m_imgTest, KP2,
			matchB, resMatches,
			matchColours, cv::Scalar::all(-1),
			std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

		for (int i = 0; i < matchB.size(); i++)
		{
			clusterIdx = labels.at<int>(i);
			clsCntr[clusterIdx]++;

			if (clusterIdx == 0)
				matchColours = (cv::Scalar(255, 255, 255));
			else if (clusterIdx == 1)
				matchColours = (cv::Scalar(127, 127, 255));
			else if (clusterIdx == 2)
				matchColours = (cv::Scalar(255, 0, 0));
			else if (clusterIdx == 3)
				matchColours = (cv::Scalar(0, 255, 0));
			else if (clusterIdx == 4)
				matchColours = (cv::Scalar(0, 0, 255));
			else if (clusterIdx == 5)
				matchColours = (cv::Scalar(0, 255, 255));
			else if (clusterIdx == 6)
				matchColours = (cv::Scalar(255, 0, 255));

			cv::Point2f offset((float)m_imgRef.cols, 0);
			cv::circle(resMatches, KP1[i].pt, 2, matchColours, 5);
			cv::circle(resMatches, KP2[i].pt + offset, 2, matchColours, 5);
		}

		cv::namedWindow("CLUSTER KEYPOINT TEST", cv::WINDOW_NORMAL);
		cv::imshow("CLUSTER KEYPOINT TEST", resMatches);
		cv::waitKey(0);

		DEBUG_PRINT("Clusters for Test Points\n");
		for (int k = 0; k <MAX_CLUSTERS; k++)
		{
			DEBUG_PRINT("\n  CLS_ID %d CLS_CNT %d", k, clsCntr[k]);
		}
		DEBUG_PRINT("\n");
#endif

	}

	void performSelectUsingHomography(int clsID)
	{
		cv::Mat homographyRefined;
		HomographyEstimator homographyEstimator;

		std::vector<cv::DMatch> matchesKP;
		std::vector<cv::KeyPoint> keypointsRef, keypointsTest;


		std::vector<cv::DMatch> matchesKPHomo;
		std::vector<cv::KeyPoint> keypointsRefHomo, keypointsTestHomo;

		cv::KeyPoint kp1, kp2;

		int clsIDKP = m_firstPointRefPairVect[clsID].first;
#ifdef DISPLAY_DEBUG_PARTITION_LESS
		DEBUG_PRINT("\nREALCLS_ID %d CLS_ID %d\n", clsIDKP, clsID);
#endif

		int k = 0;
		for (std::vector<MATCH_INFO>::iterator matchIterator = m_ClusterInfo[clsIDKP].begin();
			matchIterator != m_ClusterInfo[clsIDKP].end();
			++matchIterator)
		{
			matchesKP.push_back(cv::DMatch(k, k, matchIterator->distance));

			kp1 = matchIterator->refKP;
			kp1.pt = matchIterator->refKP.pt - m_offsetKPRef[clsID];

			kp2 = matchIterator->testKP;
			kp2.pt = matchIterator->testKP.pt - m_offsetKPTest[clsID];


			keypointsRef.push_back(kp1);
			keypointsTest.push_back(kp2);
			k++;
		}

#ifdef DEBUG_PRINT_FINAL1
		DEBUG_PRINT("Size before homography %d\n", matchesKP.size());
#endif

		homographyEstimator.selectUsingHomography(matchesKP,
												 keypointsRef,
												 keypointsTest,
												 &matchesKPHomo,
												 &keypointsRefHomo,
												 &keypointsTestHomo);

#ifdef DEBUG_PRINT_FINAL1
		DEBUG_PRINT("Size after homography %d\n", matchesKPHomo.size());
#endif

		if (clsID == 0)
		{
			m_numNEW = 0;
		}



		k = 0;


		for (std::vector<cv::DMatch>::const_iterator it = matchesKPHomo.begin();
			it != matchesKPHomo.end();
			++it)
		{
			m_matchesNEW.push_back(cv::DMatch(m_numNEW, m_numNEW, 0));

			kp1 = keypointsRefHomo[k];
			kp1.pt = keypointsRefHomo[k].pt + m_offsetKPRef[clsID];

			kp2 = keypointsTestHomo[k];
			kp2.pt = keypointsTestHomo[k].pt + m_offsetKPTest[clsID];


			m_keypointsRefNEW.push_back(kp1);
			m_keypointsTestNEW.push_back(kp2);
			k++;
			m_numNEW++;
		}
	}
	
	void transformPartitions()
	{
		for (int i = 0; i < MAX_CLUSTERS; i++)
		{
			performSelectUsingHomography(i);
		}
	}
};