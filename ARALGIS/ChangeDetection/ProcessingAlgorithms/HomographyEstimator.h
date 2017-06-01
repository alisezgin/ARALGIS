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

class HomographyEstimator
{
private:
	bool    m_enableHomographyRefinement;
	float   m_homographyReprojectionThreshold;
	cv::Mat m_roughHomography;
	cv::Mat m_refinedHomography;


public:
	HomographyEstimator() : m_enableHomographyRefinement(false),
							m_homographyReprojectionThreshold(3) {}


	void HomographyEstimator::setHomographyRefinement(bool setVal)
	{
		m_enableHomographyRefinement = setVal;
	}

	bool HomographyEstimator::getHomographyRefinement()
	{
		return m_enableHomographyRefinement;
	}

	bool HomographyEstimator::calculateHomography(const std::vector<cv::KeyPoint>& testKeypoints,
												 const std::vector<cv::KeyPoint>& referenceKeypoints,
												 float reprojectionThreshold,
												 std::vector<cv::DMatch>& matches,
												 cv::Mat* homography)
	{
		const int minNumberMatchesAllowed = 4;

		if (matches.size() < minNumberMatchesAllowed)
		{
			printf("\nrefineMatchesWithHomography exiting due to error .....\n");
			return false;
		}

		// Prepare data for cv::findHomography
		std::vector<cv::Point2f> srcPoints(matches.size());
		std::vector<cv::Point2f> dstPoints(matches.size());

		for (size_t i = 0; i < matches.size(); i++)
		{
			srcPoints[i] = referenceKeypoints[matches[i].trainIdx].pt;
			dstPoints[i] = testKeypoints[matches[i].queryIdx].pt;
		}

		// Find homography matrix and get inliers mask
		std::vector<unsigned char> inliersMask(srcPoints.size());

		*homography = cv::findHomography(dstPoints,
										 srcPoints,
										 CV_FM_RANSAC,
										 reprojectionThreshold,
										 inliersMask);

		std::vector<cv::DMatch> inliers;
		for (size_t i = 0; i<inliersMask.size(); i++)
		{
			if (inliersMask[i])
				inliers.push_back(matches[i]);
		}

		//printf("\nAFTER HOMO MATCHES %d\n", matches.size());

		matches.swap(inliers);
		return (matches.size() >= minNumberMatchesAllowed);
	}

	// Identify good matches using RANSAC
	// Return homography matrix
	void selectUsingHomography(	const std::vector<cv::DMatch>& matches,
								const std::vector<cv::KeyPoint>& keypoints1,
								const std::vector<cv::KeyPoint>& keypoints2,
								std::vector<cv::DMatch>* inlier_matchesOut,
								std::vector<cv::KeyPoint>* inliers1Out, 
								std::vector<cv::KeyPoint>* inliers2Out)
	{
		// Convert keypoints into Point2f
		std::vector<cv::Point2f> points1, points2;

		for (std::vector<cv::DMatch>::const_iterator it = matches.begin();
			it != matches.end();
			++it)
		{
			// Get the position of left keypoints
			float x = keypoints2[it->queryIdx].pt.x;
			float y = keypoints2[it->queryIdx].pt.y;

			points1.push_back(cv::Point2f(x, y));

			// Get the position of right keypoints
			x = keypoints1[it->trainIdx].pt.x;
			y = keypoints1[it->trainIdx].pt.y;

			points2.push_back(cv::Point2f(x, y));
		}

		// Compute F matrix using RANSAC
		std::vector<uchar> inliers(points1.size(), 0);

		cv::Mat inlier_mask, homography;

		if (matches.size() >= 4)
		{
			const double ransac_thresh = m_homographyReprojectionThreshold; // RANSAC inlier threshold

			homography = findHomography(points1, points2,
				cv::RANSAC, ransac_thresh, inlier_mask);


			for (unsigned i = 0; i < matches.size(); i++)
			{
				if (inlier_mask.at<uchar>(i))
				{
					int new_i = static_cast<int>(inliers1Out->size());

					//inliers1Out->push_back(keypoints2[matches[i].queryIdx]);
					//inliers2Out->push_back(keypoints1[matches[i].trainIdx]);

					inliers1Out->push_back(keypoints1[matches[i].queryIdx]);
					inliers2Out->push_back(keypoints2[matches[i].trainIdx]);

					inlier_matchesOut->push_back(cv::DMatch(new_i, new_i, 0));

					//inlier_matchesOut->push_back(matches[i]);
				}
			}
		}
		else
		{
			printf("\n\nHOMOGRAPHY ESTIMATOR Not enough points to estimate homography\n");
			exit(0);
		}

		//return homography;
	}
};
