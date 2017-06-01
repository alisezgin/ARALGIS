#pragma once

#include "DisplayMatches.h"  
#include "ImageDisplayer.h"
#include "Settings.h"  

class TransformImage
{

private:

public:

	TransformImage() {}


	cv::Size calcSize(const cv::Mat& imgTest, const cv::Mat& homography)
	{
		cv::Size sizeCalc;

		std::vector<cv::Point2f> imgCornerPT;
		std::vector<cv::Point2f> imgCornerPTTransformed;

		imgCornerPT.push_back(cv::Point(0,0));
		imgCornerPT.push_back(cv::Point(0, imgTest.cols));
		imgCornerPT.push_back(cv::Point(imgTest.rows, 0));
		imgCornerPT.push_back(cv::Point(imgTest.rows, imgTest.cols));

		cv::perspectiveTransform(imgCornerPT, imgCornerPTTransformed, homography); 


		printf("\nPNT 1 X %f Y %f", imgCornerPT[0].x, imgCornerPT[0].y);
		printf("\nPNT 2 X %f Y %f", imgCornerPT[1].x, imgCornerPT[1].y);
		printf("\nPNT 3 X %f Y %f", imgCornerPT[2].x, imgCornerPT[2].y);
		printf("\nPNT 4 X %f Y %f", imgCornerPT[3].x, imgCornerPT[3].y);

		printf("\nPNT TRS 1 X %f Y %f", imgCornerPTTransformed[0].x, imgCornerPTTransformed[0].y);
		printf("\nPNT TRS 2 X %f Y %f", imgCornerPTTransformed[1].x, imgCornerPTTransformed[1].y);
		printf("\nPNT TRS 3 X %f Y %f", imgCornerPTTransformed[2].x, imgCornerPTTransformed[2].y);
		printf("\nPNT TRS 4 X %f Y %f", imgCornerPTTransformed[3].x, imgCornerPTTransformed[3].y);

		float maxX, minX, maxY, minY;

		maxX = imgCornerPTTransformed[0].x;
		minX = imgCornerPTTransformed[0].x;

		maxY = imgCornerPTTransformed[0].y;
		minY = imgCornerPTTransformed[0].y;


		for (int i = 1; i < 4; i++)
		{
			if (imgCornerPTTransformed[i].x > maxX)
				maxX = imgCornerPTTransformed[i].x;

			if (imgCornerPTTransformed[i].x < minX)
				minX = imgCornerPTTransformed[i].x;
		}

		for (int i = 1; i < 4; i++)
		{
			if (imgCornerPTTransformed[i].y > maxY)
				maxY = imgCornerPTTransformed[i].y;

			if (imgCornerPTTransformed[i].y < minY)
				minY = imgCornerPTTransformed[i].y;
		}



		sizeCalc = cv::Size((int)(maxX - minX + 0.5), (int)(maxY - minY + 0.5));

		printf("\n\n FINAL SIZE X %d Y %d", sizeCalc.width, sizeCalc.height);

		return sizeCalc;
	}

	///////////////////////////////////////////////////////////////////////
	// Convert a vector of non-homogeneous 2D points to a vector of homogenehous 2D points.
	void to_homogeneous(const std::vector< cv::Point2f >& non_homogeneous, std::vector< cv::Point3f >& homogeneous)
	{
		homogeneous.resize(non_homogeneous.size());
		for (size_t i = 0; i < non_homogeneous.size(); i++) {
			homogeneous[i].x = non_homogeneous[i].x;
			homogeneous[i].y = non_homogeneous[i].y;
			homogeneous[i].z = 1.0;
		}
	}

	// Convert a vector of homogeneous 2D points to a vector of non-homogenehous 2D points.
	void from_homogeneous(const std::vector< cv::Point3f >& homogeneous, std::vector< cv::Point2f >& non_homogeneous)
	{
		non_homogeneous.resize(homogeneous.size());
		for (size_t i = 0; i < non_homogeneous.size(); i++) {
			non_homogeneous[i].x = homogeneous[i].x / homogeneous[i].z;
			non_homogeneous[i].y = homogeneous[i].y / homogeneous[i].z;
		}
	}

	// Transform a vector of 2D non-homogeneous points via an homography.
	std::vector<cv::Point2f> transform_via_homography(const std::vector<cv::Point2f>& points, const cv::Matx33f& homography)
	{
		std::vector<cv::Point3f> ph;
		to_homogeneous(points, ph);
		for (size_t i = 0; i < ph.size(); i++) {
			ph[i] = homography*ph[i];
		}
		std::vector<cv::Point2f> r;
		from_homogeneous(ph, r);
		return r;
	}

	// Find the bounding box of a vector of 2D non-homogeneous points.
	cv::Rect_<float> bounding_box(const std::vector<cv::Point2f>& p)
	{
		cv::Rect_<float> r;
		float x_min = std::min_element(p.begin(), p.end(), [](const cv::Point2f& lhs, const cv::Point2f& rhs) {return lhs.x < rhs.x; })->x;
		float x_max = std::max_element(p.begin(), p.end(), [](const cv::Point2f& lhs, const cv::Point2f& rhs) {return lhs.x < rhs.x; })->x;
		float y_min = std::min_element(p.begin(), p.end(), [](const cv::Point2f& lhs, const cv::Point2f& rhs) {return lhs.y < rhs.y; })->y;
		float y_max = std::max_element(p.begin(), p.end(), [](const cv::Point2f& lhs, const cv::Point2f& rhs) {return lhs.y < rhs.y; })->y;
		return cv::Rect_<float>(x_min, y_min, x_max - x_min, y_max - y_min);
	}

	// Warp the image src into the image dst through the homography H.
	// The resulting dst image contains the entire warped image, this
	// behaviour is the same of Octave's imperspectivewarp (in the 'image'
	// package) behaviour when the argument bbox is equal to 'loose'.
	// See http://octave.sourceforge.net/image/function/imperspectivewarp.html
	void homography_warp(cv::Mat& src, const cv::Mat& H, cv::Mat& dst)
	{
		std::vector< cv::Point2f > corners;
		corners.push_back(cv::Point2f(0, 0));
		corners.push_back(cv::Point2f((float)src.cols, 0));
		corners.push_back(cv::Point2f(0, (float)src.rows));
		corners.push_back(cv::Point2f((float)src.cols, (float)src.rows));

		std::vector< cv::Point2f > projected = transform_via_homography(corners, H);
		cv::Rect_<float> bb = bounding_box(projected);

		cv::Mat_<double> translation = (cv::Mat_<double>(3, 3) << 1, 0, -bb.tl().x, 0, 1, -bb.tl().y, 0, 0, 1);

		cv::warpPerspective(src, dst, translation*H, bb.size(), cv::INTER_NEAREST, cv::BORDER_REPLICATE);
	}

	//////////////////////////////////////////////

	cv::Mat transformPerspective(cv::Mat& imgTrain, cv::Mat& imgTest,
								 cv::Mat& homography,
								 std::vector<cv::DMatch>& matchesHomography,
								 std::vector<cv::KeyPoint>& keypoints1,
								 std::vector<cv::KeyPoint>& keypoints2,
								 int aDisplayIndex = 0)
	{

#ifdef  DISPLAY_IMAGES_PERSPECTIVE
		char title[100];
		DisplayMatches matchDisplayer;

	    strcpy_s(title, "BORA in transformPerspective ");
		char sNumCnt[20];
		_itoa_s(aDisplayIndex, sNumCnt, sizeof(sNumCnt), 10);
		strcat_s(title, sNumCnt);

		matchDisplayer.displayMatchesHomographyMatrix(imgTrain, imgTest, matchesHomography,
			keypoints1, keypoints2, title);
#endif

		cv::Mat tempImg;

		std::vector<cv::KeyPoint> KeyPointTrain, KeyPointTest;
		std::vector<cv::Point2f> keyPointTrainPT;
		std::vector<cv::Point2f> keyPointTestPT, keyPointTestPTWarped;
		std::vector<cv::Point2f> keypoints2Saved;
		std::vector<cv::KeyPoint> keypoints2Tmp;


		int i = 0;
		for (std::vector<cv::DMatch>::iterator matchIterator = matchesHomography.begin();
			matchIterator != matchesHomography.end();
			++matchIterator)
		{
			KeyPointTrain.push_back(keypoints1[matchesHomography[i].trainIdx]);
			keyPointTrainPT.push_back(KeyPointTrain[i].pt);

			KeyPointTest.push_back(keypoints2[matchesHomography[i].queryIdx]); //queryIdx
			keyPointTestPT.push_back(KeyPointTest[i].pt);

			i++;
		}


		i=0;
		for (std::vector<cv::KeyPoint>::iterator KPIterator = keypoints2.begin();
			KPIterator != keypoints2.end();
			++KPIterator)
		{
			keypoints2Tmp.push_back(keypoints2[i]);
		}


		cv::Mat persImage;

		if (matchesHomography.size() < 5)
		{
			cv::Mat mPersTrns;

			mPersTrns = getPerspectiveTransform(keyPointTrainPT, keyPointTestPT);
			cv::perspectiveTransform(keyPointTestPT, keyPointTestPTWarped, mPersTrns); // cv::WARP_INVERSE_MAP | cv::INTER_CUBIC)
			cv::warpPerspective(imgTest, persImage, mPersTrns, imgTest.size(), cv::INTER_NEAREST, cv::BORDER_REPLICATE);

		}
		else
		{
			cv::perspectiveTransform(keyPointTestPT, keyPointTestPTWarped, homography); // cv::WARP_INVERSE_MAP | cv::INTER_CUBIC)

			//cv::Size transSize = calcSize(imgTest, homography);
			//cv::warpPerspective(imgTest, persImage, homography, transSize, cv::INTER_NEAREST, cv::BORDER_REPLICATE);
			
			
			//cv::warpPerspective(imgTest, persImage, homography, imgTest.size(), cv::INTER_NEAREST, cv::BORDER_REPLICATE);
			cv::warpPerspective(imgTest, persImage, homography, imgTrain.size(), cv::INTER_NEAREST, cv::BORDER_REPLICATE); // BORDER_TRANSPARENT

			//resize(tempImg, persImage, imgTrain.size(), (0, 0), (0, 0), cv::INTER_CUBIC);


			//cv::warpPerspective(imgTest, persImage, homography, imgTrain.size(), cv::INTER_NEAREST | cv::WARP_INVERSE_MAP, cv::BORDER_REPLICATE);


			//cv::warpPerspective(imgTest, persImage, homography, cv::Size(2048, 4096), cv::INTER_NEAREST, cv::BORDER_TRANSPARENT);


			//printf("\nTEST H %d W %d \n", imgTest.rows, imgTest.cols);
			//homography_warp(imgTest, homography, persImage);
			
			//homography_warp(imgTest, homography, tempImg);
			//resize(tempImg, persImage, imgTrain.size(), (0, 0), (0, 0), cv::INTER_CUBIC);
		}


		for (int jj = 0; jj < (int) matchesHomography.size(); jj++)
		{
			//printf("\n RID %d TID %d JJ %d", matchesHomography[jj].trainIdx, matchesHomography[jj].queryIdx, jj);
			keypoints2Tmp[matchesHomography[jj].queryIdx].pt = keyPointTestPTWarped[jj];
		}

#ifdef  DISPLAY_IMAGES_PERSPECTIVE
		strcpy_s(title, "WARP PERSPECTIVE ");
		strcat_s(title, " ");
		_itoa_s(aDisplayIndex, sNumCnt, sizeof(sNumCnt), 10);
		strcat_s(title, sNumCnt);

		matchDisplayer.displayMatchesHomographyMatrix(imgTrain, persImage, matchesHomography,
			keypoints1, keypoints2Tmp, title);
#endif

#ifdef DISPLAY_DEBUG_PARTITION_LESS
		char trialNo[100];
		strcpy_s(trialNo, "BORA in transformPerspective ");
		char sNumCntTrl[20];
		_itoa_s(aDisplayIndex, sNumCntTrl, sizeof(sNumCntTrl), 10);
		strcat_s(trialNo, sNumCntTrl);
		printf("\n%s", trialNo);

		printf("\n Size Ref Image W %d H %d", imgTrain.cols, imgTrain.rows);
		printf("\n Size Test Image W %d H %d", imgTest.cols, imgTest.rows);
		printf("\n Size Warped Test Image W %d H %d", persImage.cols, persImage.rows);
#endif

#ifdef  DISPLAY_DEBUG_PARTITION
		char title1[100];
		//DisplayMatches matchDisplayer;
		char sNumCnt1[20];

		strcpy_s(title1, "PERSPECTIVE TOGETHER");
		strcat_s(title1, " ");
		_itoa_s(aDisplayIndex, sNumCnt1, sizeof(sNumCnt1), 10);
		strcat_s(title1, sNumCnt1);

		ImageDisplayer imgDsp;
		imgDsp.displayImages(imgTrain, persImage, title1);
#endif

		return (persImage);
	}

	cv::Mat transformAffine(cv::Mat& imgTrain, const cv::Mat& imgTest,
							const cv::Mat& homography,
							std::vector<cv::DMatch>& matchesHomography,
							std::vector<cv::KeyPoint>& keypoints1,
							std::vector<cv::KeyPoint>& keypoints2,
							int aDisplayIndex = 0)
	{

#ifdef  DISPLAY_IMAGES_DEBUG
		char title[100];
		DisplayMatches matchDisplayer;
		strcpy_s(title, "BORA in transformAffine ");

		matchDisplayer.displayMatchesHomographyMatrix(imgTrain, imgTest, matchesHomography,
			keypoints1, keypoints2, title);
#endif


		std::vector<cv::KeyPoint> KeyPointTrain, KeyPointTest;
		std::vector<cv::Point2f> keyPointTestPT, keyPointTestPTWarped;
		std::vector<cv::Point2f> keyPointTrainPT;
		//std::vector<cv::Point2f> keypoints2Saved;

		cv::Point2f* srcPoints;
		cv::Point2f* dstPoints;

		srcPoints = new cv::Point2f[matchesHomography.size()];
		dstPoints = new cv::Point2f[matchesHomography.size()];


		std::vector<cv::DMatch> MH;

		int i = 0;
		int j = 0;
		for (std::vector<cv::DMatch>::iterator matchIterator = matchesHomography.begin();
			matchIterator != matchesHomography.end();
			++matchIterator)
		{
			KeyPointTrain.push_back(keypoints1[matchesHomography[i].trainIdx]);
			keyPointTrainPT.push_back(KeyPointTrain[i].pt);

			KeyPointTest.push_back(keypoints2[matchesHomography[i].queryIdx]); //queryIdx
			keyPointTestPT.push_back(KeyPointTest[i].pt);


			srcPoints[j] = keypoints1[i].pt;
			dstPoints[j] = keypoints2[i].pt;

			MH.push_back(
				cv::DMatch((*matchIterator).queryIdx,
				(*matchIterator).trainIdx,
				(*matchIterator).distance));

			j++;

			i++;
		}

		cv::Mat warp_mat(2, 3, CV_32FC1);
		/// Get the Affine Transform
		warp_mat = cv::getAffineTransform(srcPoints, dstPoints);

		cv::Mat persImage;
		cv::warpAffine(imgTest, persImage, warp_mat, imgTrain.size(), cv::INTER_CUBIC, cv::BORDER_REPLICATE);


#ifdef  DISPLAY_IMAGES_PERSPECTIVE
		char title1[1000];
		strcpy_s(title1, "WARP AFFINE ");
		char sNumCnt1[20];
		strcat_s(title1, " ");
		_itoa_s(aDisplayIndex, sNumCnt1, sizeof(sNumCnt1), 10);
		strcat_s(title1, sNumCnt1);

		DisplayMatches matchDisplayer1;
		
		matchDisplayer1.displayMatchesHomographyMatrix(imgTrain, persImage, MH,
			keypoints1, keypoints2, title1);

#endif
		return (persImage);
	}
};