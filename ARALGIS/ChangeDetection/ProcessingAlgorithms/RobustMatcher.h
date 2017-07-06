#pragma once

#include "DisplayMatches.h"   
#include "Settings.h"  


class RobustMatcher
{
private:
	// pointer to the feature point detector object
	cv::Ptr<cv::FeatureDetector> detector;

	// pointer to the feature descriptor extractor object
	cv::Ptr<cv::DescriptorExtractor> extractor;

	float ratio; // max ratio between 1st and 2nd NN

	bool isImageToBeDisplayed;

public:
	RobustMatcher() : ratio(0.7f)
	{
		// SURF is the default feature
		detector = new cv::SurfFeatureDetector();
		extractor = new cv::SurfDescriptorExtractor();
	}

	// Set the feature detector
	void setFeatureDetector(
		cv::Ptr<cv::FeatureDetector>& detect)
	{
		detector = detect;
	}

	// Set the descriptor extractor
	void setDescriptorExtractor(
		cv::Ptr<cv::DescriptorExtractor>& desc)
	{
		extractor = desc;
	}

	void setRatio(float aRatio)
	{
		ratio = aRatio;
	}

	void setDisplay(bool aDisplay)
	{
		isImageToBeDisplayed = aDisplay;
	}


	// Match feature points using symmetry test and RANSAC
	// returns homography matrix
	void match(cv::Mat& image1, cv::Mat& image2, // input images
			  // output matches and keypoints
			  std::vector<cv::DMatch>& matchesOut,
			  std::vector<cv::KeyPoint>& keypoints1,
			  std::vector<cv::KeyPoint>& keypoints2,
			  std::vector<cv::DMatch>& matches1All,
			  std::vector<cv::KeyPoint>& keypoints1All,
			  std::vector<cv::DMatch>& matches2All,
			  std::vector<cv::KeyPoint>& keypoints2All)
	{
		int64 tick1, tick2, tick3;

		// 1b. Extraction of the SURF descriptors
		cv::Mat descriptors1, descriptors2;

		// 1a. Detection of the SURF features
		tick1 = cv::getTickCount();

		detector->detect(image1, keypoints1);

		tick2 = cv::getTickCount();

		extractor->compute(image1, keypoints1, descriptors1);

		tick3 = cv::getTickCount();

#ifdef DEBUG_PRINT_FINAL1
		DEBUG_PRINT("Image1 KeyPoint %.3f secs Descriptor %.3f TOTaL TIME %.3f\n",
			((tick2 - tick1) / cv::getTickFrequency()), ((tick3 - tick2) / cv::getTickFrequency()), ((tick3 - tick1) / cv::getTickFrequency()));
#endif

		tick1 = cv::getTickCount();

		detector->detect(image2, keypoints2);

		tick2 = cv::getTickCount();

		extractor->compute(image2, keypoints2, descriptors2);

		tick3 = cv::getTickCount();

#ifdef DEBUG_PRINT_FINAL1
		DEBUG_PRINT("Image2 KeyPoint %.3f secs Descriptor %.3f TOTaL TIME %.3f\n",
			((tick2 - tick1) / cv::getTickFrequency()), ((tick3 - tick2) / cv::getTickFrequency()), ((tick3 - tick1) / cv::getTickFrequency()));
#endif
		// 2. Match the two image descriptors
		// Construction of the matcher
		cv::BFMatcher matcher;

		// from image 1 to image 2
		// based on k nearest neighbours (with k=2)
		std::vector<std::vector<cv::DMatch>> matches1;

#ifdef DEBUG_PRINT_FINAL1
		DEBUG_PRINT("starting 1st KNN\n");
		DEBUG_PRINT("Keypoint Sizes %d %d\n", keypoints1.size(), keypoints2.size());
#endif
		// void DescriptorMatcher::knnMatch(const Mat& queryDescriptors, const Mat& trainDescriptors, 
		//                                  vector<vector<DMatch>>& matches, int k, 
		//                                  const Mat& mask=Mat(), bool compactResult=false )
		// !!!!! fiirst one is the TEST Image, second one is the TRAIN Image
		matcher.knnMatch(descriptors2, descriptors1, matches1, 2); // vector of matches (up to 2 per entry)
																   // return 2 nearest neighbours

		DEBUG_PRINT("starting 2nd KNN\n");

		// from image 2 to image 1
		// based on k nearest neighbours (with k=2)
		std::vector<std::vector<cv::DMatch>> matches2;

		matcher.knnMatch(descriptors1, descriptors2, matches2, // vector of matches (up to 2 per entry)
			                                               2); // return 2 nearest neighbours

#ifdef DEBUG_PRINT_FINAL1
		DEBUG_PRINT("KNN finshed\n");
#endif


#ifdef  DISPLAY_PRINTS_DEBUG
		DEBUG_PRINT("Initial Num Matches1 %d Num Matches2 %d \n\n", matches1.size(), matches2.size());
#endif

		/// New code to get all of the data
		for (std::vector<std::vector<cv::DMatch>>::iterator
			matchIterator = matches1.begin();
			matchIterator != matches1.end(); ++matchIterator)
		{
			matches1All.push_back((*matchIterator)[0]);
		}

		for (std::vector<std::vector<cv::DMatch>>::iterator
			matchIterator = matches2.begin();
			matchIterator != matches2.end(); ++matchIterator)
		{
			matches2All.push_back((*matchIterator)[0]);
		}

		for (int jj = 0; jj < (int) keypoints1.size(); jj++)
		{
			keypoints1All.push_back(keypoints1[jj]);
		}

		for (int jj = 0; jj < (int) keypoints2.size(); jj++)
		{
			keypoints2All.push_back(keypoints2[jj]);
		}
		//////////// end of new code 


		/////////////   bora 1   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#ifdef  DISPLAY_IMAGES_DEBUG_MATCHER
		if (isImageToBeDisplayed)
		{
			char title1[1000];
			DisplayMatches matchDisplayer1;

			strcpy_s(title1, "MATCHES img1-->img2 Feature-1 \0");
			matchDisplayer1.displayMatchesInitial(image1, image2, matches1, matches2,
				keypoints1, keypoints2, title1);
		}
#endif
		/////////////   bora 1   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		// 3. Remove matches for which NN ratio is
		// > than threshold
		// clean image 1 -> image 2 matches
		int removed1 = ratioTest(matches1);

		// clean image 2 -> image 1 matches
		int removed2 = ratioTest(matches2);

#ifdef  DISPLAY_PRINTS_DEBUG
		DEBUG_PRINT("After Ratio Test Num Matches1 %d Num Matches2 %d RM1 %d RM2 %d\n\n", matches1.size() - removed1, matches2.size() - removed2, removed1, removed2);
#endif

		/////////////   bora 2 ###############################
#ifdef  DISPLAY_IMAGES_DEBUG_MATCHER
		if (isImageToBeDisplayed)
		{
			DisplayMatches matchDisplayer2;
			char title2[1000];
			strcpy_s(title2, "MATCHES img1-->img2 Features After Ratio Test \0");
			matchDisplayer2.displayMatchesRatioTest(image1, image2, matches1, matches2,
				keypoints1, keypoints2, title2);
		}
#endif
		/////////////   bora 2 ###############################3

		// 4. Remove non-symmetrical matches
		//std::vector<cv::DMatch> symMatches;
		symmetryTest(matches1, matches2, matchesOut); //matchesOut // symMatches

#ifdef  DISPLAY_PRINTS_DEBUG
		DEBUG_PRINT("After Symetry Test Num Matches1 %d \n\n", matchesOut.size()); //symMatches
#endif

		/////////////   bora 3 ###############################
#ifdef  DISPLAY_IMAGES_DEBUG_MATCHER 
		if (isImageToBeDisplayed)
		{
			DisplayMatches matchDisplayer3;
			char title3[1000];
			strcpy_s(title3, "MATCHES img1-->img2 Features After Symetry Test \0");
			matchDisplayer3.displayMatchesSymetryTest(image1, image2, matchesOut, //symMatches
				keypoints1, keypoints2, title3);
		}
#endif
	}


	// Clear matches for which NN ratio is > than threshold
	// return the number of removed points
	// (corresponding entries being cleared,
	// i.e. size will be 0)
	int ratioTest(std::vector<std::vector<cv::DMatch>> &matches)
	{
		int removed = 0;

		// for all matches
		for (std::vector<std::vector<cv::DMatch>>::iterator
			matchIterator = matches.begin();
			matchIterator != matches.end(); ++matchIterator)
		{
			// if 2 NN has been identified
			if (matchIterator->size() > 1)
			{
				// check distance ratio
				if ((*matchIterator)[0].distance /
					(*matchIterator)[1].distance > ratio)
				{
					matchIterator->clear(); // remove match
					removed++;
				}
			}
			else
			{ // does not have 2 neighbours
				matchIterator->clear(); // remove match
				removed++;
			}
		}

		return removed;
	}


	// Insert symmetrical matches in symMatches vector
	void symmetryTest(
		const std::vector<std::vector<cv::DMatch>>& matches1,
		const std::vector<std::vector<cv::DMatch>>& matches2,
		std::vector<cv::DMatch>& symMatches)
	{
		// for all matches image 1 -> image 2
		for (std::vector<std::vector<cv::DMatch>>::const_iterator matchIterator1 = matches1.begin();
			matchIterator1 != matches1.end();
			++matchIterator1)
		{
			// ignore deleted matches
			if (matchIterator1->size() >= 2)
			{
				// for all matches image 2 -> image 1
				for (std::vector<std::vector<cv::DMatch>>::const_iterator matchIterator2 = matches2.begin();
					matchIterator2 != matches2.end();
					++matchIterator2)
				{
					// ignore deleted matches
					if (matchIterator2->size() >= 2)
					{
						// Match symmetry test
						if (((*matchIterator1)[0].queryIdx == (*matchIterator2)[0].trainIdx) &&
							((*matchIterator2)[0].queryIdx == (*matchIterator1)[0].trainIdx))
						{
							// add symmetrical match
							symMatches.push_back(
								cv::DMatch((*matchIterator1)[0].queryIdx,
								(*matchIterator1)[0].trainIdx,
								(*matchIterator1)[0].distance));

							break; // next match in image 1 -> image 2
						}
					}
				}
			}
		}
	}




	// Identify good matches using RANSAC
	// Return homography matrix
	cv::Mat homographyTest(
		const std::vector<cv::DMatch>& matches,
		const std::vector<cv::KeyPoint>& keypoints1,
		const std::vector<cv::KeyPoint>& keypoints2,
		std::vector<cv::DMatch>& inlier_matchesOut)
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
		std::vector<cv::KeyPoint> inliers1, inliers2;

		if (matches.size() >= 4)
		{
			const double ransac_thresh = 2.5f; // RANSAC inlier threshold

			homography = findHomography(points1, points2,
				cv::RANSAC, ransac_thresh, inlier_mask);


			for (unsigned i = 0; i < matches.size(); i++)
			{
				if (inlier_mask.at<uchar>(i))
				{
					int new_i = static_cast<int>(inliers1.size());
					inliers1.push_back(keypoints2[matches[i].queryIdx]);
					inliers2.push_back(keypoints1[matches[i].trainIdx]);
					//inlier_matchesOut.push_back(DMatch(new_i, new_i, 0));
					inlier_matchesOut.push_back(matches[i]);
				}
			}
		}

		return homography;
	}
};