#pragma once

#if !defined CD_CNTRLLR
#define CD_CNTRLLR

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "ChangeDetector.h"


// Singleton Class Implementation
// Singleton = There is only 1 (one) controller in the program

class CChangeDetectController 
{

private:

	static CChangeDetectController *singleton; // pointer to the singleton

	CChangeDetector *cdetect;

	// The image to be processed
	cv::Mat m_ReferenceImage;
	cv::Mat m_TestImage;
	int result;

public:
	CChangeDetectController() 
	{ // private constructor
		//setting up the application
		cdetect = new CChangeDetector();
	}

	// Sets the color distance threshold
	void setColorDistanceThreshold(int distance) 
	{
		cdetect->setColorDistanceThreshold(distance);
	}

	// Sets the REFERENCE image. Reads it from file.
	bool setReferenceImage(std::string filename) 
	{
		m_ReferenceImage = cv::imread(filename);

		if (!m_ReferenceImage.data)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	// Returns the current REFERENCE image.
	const cv::Mat getReferenceImage() const
	{
		return m_ReferenceImage;
	}

	// Sets the TEST image. Reads it from file.
	bool setTestImage(std::string filename)
	{
		m_TestImage = cv::imread(filename);

		if (!m_TestImage.data)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	// Returns the current TEST image.
	const cv::Mat getTestImage() const
	{
		return m_TestImage;
	}

	// Performs image processing.
	void process(char* fileRef, char*fileTest) 
	{

		cv::Mat imgReferenceClr = cv::imread(fileRef);
		cv::Mat imgTestClr = cv::imread(fileTest);


		result = cdetect->process(imgReferenceClr, imgTestClr);
	}


	// Returns the image result from the latest processing.
	const int getLastResult() const 
	{
		return result;
	}

	// Deletes all processor objects created by the controller.
	~CChangeDetectController()
	{
		if (cdetect != NULL)
			delete cdetect;
		destroy(); //// new for leak
	}

	// Singleton static members
	static CChangeDetectController *getInstance()
	{
		if (singleton == 0)
		{
			singleton = new CChangeDetectController;

		}

		return singleton;
	}

	// Releases the singleton instance of this controller.
	static void destroy() 
	{
		if (singleton != 0) 
		{
			delete singleton;
			singleton = 0;
		}
	}
};

#endif
