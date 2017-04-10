#pragma once

#if !defined CHANGEDETECT
#define CHANGEDETECT

#include <opencv2/core/core.hpp>

class CChangeDetector 
{

private:

	// minimum acceptable distance
	int minDist;

	// target color
	cv::Vec3b target;

	// image containing resulting binary map
	cv::Mat result;



public:

	// empty constructor
	CChangeDetector() : minDist(100) 
	{
		// default parameter initialization here
		target[0] = target[1] = target[2] = 0;
	}

	// Getters and setters

	// Sets the color distance threshold.
	// Threshold must be positive, otherwise distance threshold
	// is set to 0.
	void setColorDistanceThreshold(int distance) 
	{
		if (distance<0)
			distance = 0;
		minDist = distance;
	}


	// Processes the image. Returns a 1-channel binary image.
	int process(cv::Mat &aImgReference, const cv::Mat &aImgTest);
};


#endif
