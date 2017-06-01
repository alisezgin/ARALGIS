//#include <stdio.h>
//#include <iostream>
//#include <iomanip>
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/features2d/features2d.hpp"
//#include "opencv2/nonfree/features2d.hpp"
//#include "opencv2/calib3d/calib3d.hpp"

#pragma once

#include "Settings.h"  

class DisplayMatches
{
private:
	bool m_bIsSeqDisp;

public:

	DisplayMatches() : m_bIsSeqDisp(false) {}

	void setSeqDisplay(bool aVal)
	{
		m_bIsSeqDisp = aVal;
	}

	void displayMatchesInitial(const cv::Mat& image1, const cv::Mat& image2,
		std::vector<std::vector<cv::DMatch>> matches1,
		std::vector<std::vector<cv::DMatch>> matches2,
		std::vector<cv::KeyPoint>& keypoints1,
		std::vector<cv::KeyPoint>& keypoints2,
		char* title)
	{
		cv::Mat resMatches;
		std::vector<cv::DMatch> matchB1, matchB2;

		if (matches1.size() != 0)
		{
			for (int i = 0; i < (int) matches1.size(); i++)
			{

				matchB1.push_back(matches1[i][0]); //img1-->img2
			}
		}

		// optional line 
		if (matches2.size() != 0)
		{
			for (int i = 0; i < (int) matches2.size(); i++)
			{
				matchB2.push_back(matches2[i][0]); // img2--> img1
			}
		}

		//if (matches1.size() >= matches2.size()) //==> keypoints2.size() > keypoints1.size()
		//{
		//	printf("\nYYYYYYYYY matches1.size() >= matches2.size()\n");
		cv::drawMatches(image1, keypoints1, image2, keypoints2,
			matchB2, resMatches,
			cv::Scalar::all(-1), cv::Scalar::all(-1),

			std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS); // DRAW_RICH_KEYPOINTS  NOT_DRAW_SINGLE_POINTS

		cv::Point2f offset((float)image1.cols, 0);
		for (int jj = 0; jj < (int) keypoints2.size(); jj++)
		{
			cv::circle(resMatches, keypoints2[jj].pt + offset, 5, cv::Scalar(0, 0, 255), 10);
		}


		for (int jj = 0; jj < (int) keypoints1.size(); jj++)
		{
			cv::circle(resMatches, keypoints1[jj].pt, 5, cv::Scalar(0, 255, 0), 10); //queryIdx // + offset
		}


		//-- Show detected matches
		cv::namedWindow(title, cv::WINDOW_NORMAL);
		cv::imshow(title, resMatches);
		cv::waitKey(0);
	}


	void displayMatchesRatioTest(const cv::Mat& image1, const cv::Mat& image2,
		std::vector<std::vector<cv::DMatch>> matches1,
		std::vector<std::vector<cv::DMatch>> matches2,
		std::vector<cv::KeyPoint>& keypoints1,
		std::vector<cv::KeyPoint>& keypoints2,
		char* title)
	{
		cv::Mat resMatches;
		std::vector<cv::DMatch> matchB;
		std::vector<cv::KeyPoint> KP1, KP2;

		int i = 0;
		int k = 0;

		// !!!!!!!   very important !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// here there is one important thing
		// only keypoints from img1--> img2 passing ratio test is displayed on screen
		// keypoints from img2--> img1 passing ratio test is not displayed
		// !!!!!!!   very important !!!!!!!!!!
		// matches1 contains keypoints in TEST image that corresponds to keypoints of TRAIN image

		for (std::vector<std::vector<cv::DMatch>>::iterator matchIterator = matches1.begin();
			matchIterator != matches1.end();
			++matchIterator)
		{
			if (matchIterator->size() > 1)
			{
				matchB.push_back(cv::DMatch(k, k,
					(*matchIterator)[0].distance));

				KP1.push_back(keypoints1[matches1[i][0].trainIdx]);
				KP2.push_back(keypoints2[matches1[i][0].queryIdx]);   // trainIdx
				k++;
			}
			i++;
		}

		printf("After Ratio Test Num Matches1 %d III %d KP11 %d KP21 %d KKK %d\n", matchB.size(), i, KP1.size(), KP2.size(), k);

		cv::drawMatches(image1, KP1, image2, KP2,
			matchB, resMatches,
			cv::Scalar::all(-1), cv::Scalar::all(-1),
			std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS); // DRAW_RICH_KEYPOINTS  NOT_DRAW_SINGLE_POINTS

		cv::Point2f offset((float)image1.cols, 0);
		for (int jj = 0; jj < (int) KP1.size(); jj++)
		{
			cv::circle(resMatches, KP1[jj].pt, 10, cv::Scalar(255, 0, 0), 10);
		}

		for (int jj = 0; jj < (int) KP2.size(); jj++)
		{
			cv::circle(resMatches, KP2[jj].pt + offset, 10, cv::Scalar(0, 255, 0), 10);
		}


		//-- Show detected matches
		cv::namedWindow(title, cv::WINDOW_NORMAL);
		cv::imshow(title, resMatches);
		cv::waitKey(0);
	}

	void displayMatchesSymetryTest(const cv::Mat& image1, const cv::Mat& image2,
		std::vector<cv::DMatch> symMatches,
		std::vector<cv::KeyPoint>& keypoints1,
		std::vector<cv::KeyPoint>& keypoints2,
		char* title)
	{
		cv::Mat resMatches;
		std::vector<cv::DMatch> matchB;
		std::vector<cv::KeyPoint> KP1, KP2;

		printf("\n displayMatchesSymetryTest SIZE1 %d SIZE2 %d\n", keypoints1.size(), keypoints2.size());

		int k = 0;
		for (std::vector<cv::DMatch>::iterator matchIterator = symMatches.begin();
			matchIterator != symMatches.end();
			++matchIterator)
		{
			if (symMatches.size() > 1)
			{
				matchB.push_back(cv::DMatch(k, k,
					(*matchIterator).distance));

				KP1.push_back(keypoints1[symMatches[k].trainIdx]);
				KP2.push_back(keypoints2[symMatches[k].queryIdx]);  

				k++;
			}
		}

		printf("\nAfter Symetry Test Num Matches1 %d IIII %d KP111 %d KP2111 %d\n", symMatches.size(), k, KP1.size(), KP2.size());

		if (m_bIsSeqDisp == true)
		{
			cv::Scalar matchColours;
			std::vector<cv::DMatch> tmpMatch;
			for (int i = 0; i < (int) matchB.size(); i++)
			{
				matchColours = (cv::Scalar(0, (i*10), 85 + (i * 20)));
				tmpMatch.push_back(matchB[i]);

				cv::drawMatches(image1, KP1, image2, KP2,
					tmpMatch, resMatches,
					matchColours, cv::Scalar::all(-1),
					std::vector<char>(), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS); // DRAW_RICH_KEYPOINTS  NOT_DRAW_SINGLE_POINTS

				cv::namedWindow(title, cv::WINDOW_NORMAL);
				cv::imshow(title, resMatches);
				cv::waitKey(0);
			}
		}
		else
		{
			cv::drawMatches(image1, KP1, image2, KP2,
				matchB, resMatches,
				cv::Scalar::all(-1), cv::Scalar::all(-1),
				std::vector<char>(), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS); // DRAW_RICH_KEYPOINTS  NOT_DRAW_SINGLE_POINTS
		}

		cv::Point2f offset((float)image1.cols, 0);
		for (int jj = 0; jj < (int) KP1.size(); jj++)
		{
			cv::circle(resMatches, KP1[jj].pt, 2, cv::Scalar(255, 0, 0), 10);
		}


		for (int jj = 0; jj < (int) KP2.size(); jj++)
		{
			cv::circle(resMatches, KP2[jj].pt + offset, 2, cv::Scalar(0, 255, 0), 10);
		}

		if (m_bIsSeqDisp == false)
		{
			//-- Show detected matches
			cv::namedWindow(title, cv::WINDOW_NORMAL);
			cv::imshow(title, resMatches);
			cv::waitKey(0);
		}
	}

	void displayMatchesFundementalMatrix(const cv::Mat& image1, const cv::Mat& image2,
		std::vector<cv::DMatch> matches,
		std::vector<cv::KeyPoint>& keypoints1,
		std::vector<cv::KeyPoint>& keypoints2,
		char* title)
	{
		cv::Mat resMatches;
		std::vector<cv::DMatch> matchB;
		std::vector<cv::KeyPoint> KP1, KP2;

		int k = 0;
		for (std::vector<cv::DMatch>::iterator matchIterator = matches.begin();
			matchIterator != matches.end();
			++matchIterator)
		{
			matchB.push_back(cv::DMatch(k, k,
				(*matchIterator).distance));

			KP1.push_back(keypoints1[matches[k].trainIdx]);
			KP2.push_back(keypoints2[matches[k].queryIdx]);  // queryIdx
			k++;
		}

		printf("After RANSAC Test Num Matches1 %d IIIII %d KP1111 %d KP2111 %d\n", matches.size(), k, KP1.size(), KP2.size());

		cv::drawMatches(image1, KP1, image2, KP2,
			matchB, resMatches,
			cv::Scalar::all(-1), cv::Scalar::all(-1),
			std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS); // DRAW_RICH_KEYPOINTS  NOT_DRAW_SINGLE_POINTS

		cv::Point2f offset((float)image1.cols, 0);
		for (int jj = 0; jj < (int) KP1.size(); jj++)
		{
			cv::circle(resMatches, KP1[jj].pt, 10, cv::Scalar(255, 0, 0), 10);
		}


		for (int jj = 0; jj < (int) KP2.size(); jj++)
		{
			cv::circle(resMatches, KP2[jj].pt + offset, 10, cv::Scalar(0, 255, 0), 10);
		}


		//-- Show detected matches
		cv::namedWindow(title, cv::WINDOW_NORMAL);
		cv::imshow(title, resMatches);
		cv::waitKey(0);
	}

	void displayMatchesHomographyMatrix(const cv::Mat& image1, const cv::Mat& image2,
		std::vector<cv::DMatch> matches,
		std::vector<cv::KeyPoint>& keypoints1,
		std::vector<cv::KeyPoint>& keypoints2,
		char* title)
	{
		cv::Mat resMatches;
		std::vector<cv::DMatch> matchB;
		std::vector<cv::KeyPoint> KP1, KP2;

		int k = 0;
		for (std::vector<cv::DMatch>::iterator matchIterator = matches.begin();
			matchIterator != matches.end();
			++matchIterator)
		{
			matchB.push_back(cv::DMatch(k, k,
				(*matchIterator).distance));

			KP1.push_back(keypoints1[matches[k].trainIdx]);
			KP2.push_back(keypoints2[matches[k].queryIdx]);  // queryIdx

			//printf("I %d QUER %d TRN %d \n", k, matches[k].queryIdx, matches[k].trainIdx);

			k++;
		}

		printf("\nRETURN SIZE %d\n", matches.size());

		printf("After HOMO Test Num Matches1 %d IIIIII %d KP1111 %d KP2111 %d\n", matches.size(), k, KP1.size(), KP2.size());

		cv::drawMatches(image1, KP1, image2, KP2,
			matchB, resMatches,
			cv::Scalar::all(-1), cv::Scalar::all(-1),
			std::vector<char>(), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS); // DRAW_RICH_KEYPOINTS  NOT_DRAW_SINGLE_POINTS

		cv::Point2f offset((float)image1.cols, 0);
		for (int jj = 0; jj < (int) KP1.size(); jj++)
		{
			//cv::circle(resMatches, KP1[jj].pt, 2, cv::Scalar(255, 0, 0), 10);
			//printf("\n KP1 Index %d X %.3f Y %.3f", jj, KP1[jj].pt.x, KP1[jj].pt.y);
		}


		for (int jj = 0; jj < (int) KP2.size(); jj++)
		{
			//cv::circle(resMatches, KP2[jj].pt + offset, 2, cv::Scalar(0, 255, 0), 10);
			//printf("\n KP2 Index %d X %.3f Y %.3f", jj, KP2[jj].pt.x + offset.x, KP2[jj].pt.y + offset.y);

		}


		//-- Show detected matches
		cv::namedWindow(title, cv::WINDOW_NORMAL);
		cv::imshow(title, resMatches);
		cv::waitKey(0);
	}

	void displayMatchesProcessor(const cv::Mat& image1, const cv::Mat& image2,
		std::vector<cv::DMatch> symMatches,
		std::vector<cv::KeyPoint>& keypoints1,
		std::vector<cv::KeyPoint>& keypoints2,
		char* title)
	{
		cv::Mat resMatches;
		std::vector<cv::DMatch> matchB;
		std::vector<cv::KeyPoint> KP1, KP2;

		printf("\n displayMatchesProcessor SIZE1 %d SIZE2 %d\n", keypoints1.size(), keypoints2.size());
		printf("\n displayMatchesProcessor SIZE of MATCHES %d \n", symMatches.size());


		int k = 0;
		for (std::vector<cv::DMatch>::iterator matchIterator = symMatches.begin();
			matchIterator != symMatches.end();
			++matchIterator)
		{
			//if (symMatches.size() > 1)
			{
				matchB.push_back(cv::DMatch(k, k,
					(*matchIterator).distance));

				KP1.push_back(keypoints1[symMatches[k].trainIdx]);
				KP2.push_back(keypoints2[symMatches[k].queryIdx]);  // queryIdx

				k++;
			}
		}

		if (m_bIsSeqDisp == true)
		{
			cv::Scalar matchColours;
			std::vector<cv::DMatch> tmpMatch;
			for (int i = 0; i < (int) matchB.size(); i++)
			{
				matchColours = (cv::Scalar(0, (i * 10), 85 + (i * 20)));
				tmpMatch.push_back(matchB[i]);

				cv::drawMatches(image1, KP1, image2, KP2,
					tmpMatch, resMatches,
					matchColours, cv::Scalar::all(-1),
					std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS); // DRAW_RICH_KEYPOINTS  NOT_DRAW_SINGLE_POINTS

				cv::namedWindow(title, cv::WINDOW_NORMAL);
				cv::imshow(title, resMatches);
				cv::waitKey(0);
			}
		}
		else
		{
			cv::drawMatches(image1, KP1, image2, KP2,
				matchB, resMatches,
				cv::Scalar(0,0,255), cv::Scalar(0,0,255),
				std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS); // DRAW_RICH_KEYPOINTS  NOT_DRAW_SINGLE_POINTS
		}

		cv::Point2f offset((float)image1.cols, 0);

		float aRectSize = 30;
		cv::Point2f vrt1, vrt2;
		for (int jj = 0; jj < (int) KP1.size(); jj++)
		{
			vrt1 = cv::Point2f(KP1[jj].pt.x - aRectSize, KP1[jj].pt.y - aRectSize);
			vrt2 = cv::Point2f(KP1[jj].pt.x + aRectSize, KP1[jj].pt.y + aRectSize);
		}


		for (int jj = 0; jj < (int) KP2.size(); jj++)
		{
			vrt1 = cv::Point2f(KP2[jj].pt.x + offset.x - aRectSize, KP1[jj].pt.y - aRectSize + offset.y);
			vrt2 = cv::Point2f(KP2[jj].pt.x + offset.x + aRectSize, KP1[jj].pt.y + aRectSize + offset.y);
		}

		if (m_bIsSeqDisp == false)
		{
			//-- Show detected matches
			cv::namedWindow(title, cv::WINDOW_NORMAL);
			cv::imshow(title, resMatches);
			cv::waitKey(0);
		}
	}

};