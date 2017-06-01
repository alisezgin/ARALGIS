#include "stdafx.h"


#include "ARALGIS.h"
#include "..\HeaderFiles\ImageFilterProcessing.h"

#include ".\ImageFiltering\HeaderFiles\pixkit-image.hpp"
#include ".\ImageFiltering\HeaderFiles\cvt.hpp"

#include "MainFrm.h"
#include "ARALGISView.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CImageFilterProcessing::CImageFilterProcessing
// 
// DESCRIPTION:	C'tor 
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
////////////////////////////////////////////////////////////////////////////////
CImageFilterProcessing::CImageFilterProcessing()
{
	bRun = FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CImageFilterProcessing::CImageFilterProcessing
// 
// DESCRIPTION:	D'tor
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
////////////////////////////////////////////////////////////////////////////////
CImageFilterProcessing::~CImageFilterProcessing()
{
	Shutdown();
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CImageFilterProcessing::CImageFilterProcessing
// 
// DESCRIPTION:	starts the CameraDBServer Thread
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
////////////////////////////////////////////////////////////////////////////////
BOOL CImageFilterProcessing::Start(CMainFrame* pFrame)
{
	if (bRun)
	{
		TRACE("_beginthreadex(...) failure, for Launch ThreadFilterProcessing::Start\n");
		return FALSE;
	}

	TRACE("Filter Processing Threads 1, 2, and 3 Starting ...\n");

	m_pFrame = pFrame;

	bRun = TRUE;

	ShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// Launch Filter-1 Processing Thread
	ThreadFilter1Processing = (HANDLE)_beginthreadex(NULL,
													 0,
													 Filter1ProcessingThread,
													 this,
													 0,
													 &ThreadID1
												    );
	if (!ThreadFilter1Processing)
	{
		TRACE("_beginthreadex(...) failure, ThreadFilter1Processing::Start\n");
		return FALSE;
	}

	TRACE("ThreadFilter1Processing ThreadID %x ...\n", ThreadID1);


	// Launch Filter-2 Processing Thread
	ThreadFilter2Processing = (HANDLE)_beginthreadex(NULL,
													 0,
													 Filter2ProcessingThread,
													 this,
													 0,
													 &ThreadID2
													);
	if (!ThreadFilter2Processing)
	{
		TRACE("_beginthreadex(...) failure, ThreadFilter2Processing::Start\n");
		return FALSE;
	}

	TRACE("ThreadFilter2Processing ThreadID %x ...\n", ThreadID2);	
	
	
	// Launch Filter-3 Processing Thread
	ThreadFilter3Processing = (HANDLE)_beginthreadex(NULL,
													 0,
													 Filter3ProcessingThread,
													 this,
													 0,
													 &ThreadID3
													);
	if (!ThreadFilter3Processing)
	{
		TRACE("_beginthreadex(...) failure, ThreadFilter3Processing::Start\n");
		return FALSE;
	}

	TRACE("ThreadFilter3Processing ThreadID %x ...\n", ThreadID3);

	////////////////////////////////////////////////////////////////////////

	// Launch RefFilter-1 Processing Thread
	ThreadRefFilter1Processing = (HANDLE)_beginthreadex(NULL,
														0,
														RefFilter1ProcessingThread,
														this,
														0,
														&ThreadRefID1
														);
	if (!ThreadRefFilter1Processing)
	{
		TRACE("_beginthreadex(...) failure, ThreadRefFilter1Processing::Start\n");
		return FALSE;
	}

	TRACE("ThreadRefFilter1Processing ThreadID %x ...\n", ThreadRefID1);


	// Launch RefFilter-2 Processing Thread
	ThreadRefFilter2Processing = (HANDLE)_beginthreadex(NULL,
														0,
														RefFilter2ProcessingThread,
														this,
														0,
														&ThreadRefID2
														);
	if (!ThreadRefFilter2Processing)
	{
		TRACE("_beginthreadex(...) failure, ThreadRefFilter2Processing::Start\n");
		return FALSE;
	}

	TRACE("ThreadRefFilter2Processing ThreadID %x ...\n", ThreadRefID2);


	// Launch RefFilter-3 Processing Thread
	ThreadRefFilter3Processing = (HANDLE)_beginthreadex(NULL,
														0,
														RefFilter3ProcessingThread,
														this,
														0,
														&ThreadRefID3
														);
	if (!ThreadRefFilter3Processing)
	{
		TRACE("_beginthreadex(...) failure, ThreadRefFilter3Processing::Start\n");
		return FALSE;
	}

	TRACE("ThreadRefFilter3Processing ThreadID %x ...\n", ThreadRefID1);


	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CImageFilterProcessing::Shutdown
// 
// DESCRIPTION:	stops the CameraDBServer thread
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
////////////////////////////////////////////////////////////////////////////////
BOOL CImageFilterProcessing::Shutdown()
{
	if (!bRun)
		return FALSE;

	TRACE("ThreadFilterProcessing Shutting down ...\n");

	SetEvent(ShutdownEvent);

	WaitForSingleObject(ThreadFilter1Processing, INFINITE);
	WaitForSingleObject(ThreadFilter2Processing, INFINITE);
	WaitForSingleObject(ThreadFilter3Processing, INFINITE);
	WaitForSingleObject(ThreadRefFilter1Processing, INFINITE);
	WaitForSingleObject(ThreadRefFilter2Processing, INFINITE);
	WaitForSingleObject(ThreadRefFilter3Processing, INFINITE);

	CloseHandle(ShutdownEvent);
	CloseHandle(ThreadFilter1Processing);
	CloseHandle(ThreadFilter2Processing);
	CloseHandle(ThreadFilter3Processing);
	CloseHandle(ThreadRefFilter1Processing);
	CloseHandle(ThreadRefFilter2Processing);
	CloseHandle(ThreadRefFilter3Processing);
	bRun = FALSE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CImageFilterProcessing::Filter1ProcessingThread
// 
// DESCRIPTION:	thread function for starting filter-1 processig for test image 
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
////////////////////////////////////////////////////////////////////////////////
UINT __stdcall CImageFilterProcessing::Filter1ProcessingThread(LPVOID pParam)
{
	CImageFilterProcessing *pFilterProcessing = (CImageFilterProcessing*)pParam;

	TRACE("Filter Processing Thread Number 1 Started\n");

	HANDLE Handles[2];
	Handles[0] = pFilterProcessing->ShutdownEvent;
	Handles[1] = g_ProcessFilter1Event;

	CView * pView = pFilterProcessing->m_pFrame->GetActiveView();
	LPARAM pLparam;
	pLparam = reinterpret_cast<LPARAM>("ARALGIS");

	for (;;)
	{
		DWORD EventCaused = WaitForMultipleObjects( 2,
													Handles,
													FALSE,
													INFINITE);

		if (EventCaused == WAIT_FAILED || EventCaused == WAIT_OBJECT_0)
		{
			if (EventCaused == WAIT_FAILED)
				TRACE("WaitForMultipleObjects failure at FilterProcessingThread Number 1", GetLastError());
			else
				TRACE("FilterProcessingThread Number 1 is shutting Down normally...\n");

			return THREADEXIT_SUCCESS;
		}

		// g_ProcessFilter1Event is received
		else if (EventCaused == WAIT_OBJECT_0 + 1)
		{
			ResetEvent(g_ProcessFilter1Event);

			//pixkit::enhancement_global::RajuNair2014(g_CVImageTest, g_CVImageTestFilter1); // 29577244
																							 // 11054536

			//cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, cv::Size(8, 8));
			//clahe->setClipLimit(4);
			//clahe->apply(g_CVImageTest, g_CVImageTestFilter1);

			if (g_CameraPixelBits == 24)
			{
				// READ RGB color image and convert it to Lab
				cv::Mat lab_image;
				cv::cvtColor(g_CVImageTest, lab_image, CV_BGR2Lab);

				// Extract the L channel
				std::vector<cv::Mat> lab_planes(3);
				cv::split(lab_image, lab_planes);  // now we have the L image in lab_planes[0]

				// apply the CLAHE algorithm to the L channel
				cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
				clahe->setClipLimit(4);
				cv::Mat dst;
				clahe->apply(lab_planes[0], dst);

				// Merge the the color planes back into an Lab image
				//dst.copyTo(lab_planes[0]);
				lab_planes[0] = dst.clone();

				cv::merge(lab_planes, lab_image);

				// convert back to RGB
				cv::Mat image_clahe;
				cv::cvtColor(lab_image, g_CVImageTestFilter1, CV_Lab2BGR);
			}
			else
			{
				cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
				clahe->setClipLimit(4);
				cv::Mat dst;
				clahe->apply(g_CVImageTest, g_CVImageTestFilter1);
			}

			pView->SendMessage(WM_FILTER1_READY, 0, pLparam);
		}

	} // infite for loop

	return THREADEXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CImageFilterProcessing::Filter2ProcessingThread
// 
// DESCRIPTION:	thread function for starting filter-2 processig for test image 
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
////////////////////////////////////////////////////////////////////////////////
UINT __stdcall CImageFilterProcessing::Filter2ProcessingThread(LPVOID pParam)
{
	CImageFilterProcessing *pFilterProcessing = (CImageFilterProcessing*)pParam;

	cv::Mat inGray;

	TRACE("Filter Processing Thread Number 2 Started\n");

	CView * pView = pFilterProcessing->m_pFrame->GetActiveView();
	LPARAM pLparam;
	pLparam = reinterpret_cast<LPARAM>("ARALGIS");

	HANDLE Handles[2];
	Handles[0] = pFilterProcessing->ShutdownEvent;
	Handles[1] = g_ProcessFilter2Event;

	for (;;)
	{
		DWORD EventCaused = WaitForMultipleObjects(2,
												   Handles,
												   FALSE,
												   INFINITE);

		if (EventCaused == WAIT_FAILED || EventCaused == WAIT_OBJECT_0)
		{
			if (EventCaused == WAIT_FAILED)
				TRACE("WaitForMultipleObjects failure at FilterProcessingThread Number 2", GetLastError());
			else
				TRACE("FilterProcessingThread Number 2 is shutting Down normally...\n");

			return THREADEXIT_SUCCESS;
		}

		// g_ProcessFilter2Event is received
		else if (EventCaused == WAIT_OBJECT_0 + 1)
		{
			ResetEvent(g_ProcessFilter2Event);

			if (g_CameraPixelBits == 24)
			{
				cv::cvtColor(g_CVImageTest, inGray, CV_RGB2GRAY);

				cv::equalizeHist(inGray, g_CVImageTestFilter2);
			}
			else
			{
				cv::equalizeHist(g_CVImageTest, g_CVImageTestFilter2);
			}

			//pixkit::enhancement_global::GlobalHistogramEqualization1992(inGray, g_CVImageTestFilter2);

			pView->SendMessage(WM_FILTER2_READY, 0, pLparam);
		}

	} // infite for loop

	return THREADEXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CImageFilterProcessing::Filter3ProcessingThread
// 
// DESCRIPTION:	thread function for starting filter-3 processig for test image 
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
////////////////////////////////////////////////////////////////////////////////
UINT __stdcall CImageFilterProcessing::Filter3ProcessingThread(LPVOID pParam)
{
	CImageFilterProcessing *pFilterProcessing = (CImageFilterProcessing*)pParam;

	cv::Mat inGray;

	TRACE("Filter Processing Thread Number 3 Started\n");

	CView * pView = pFilterProcessing->m_pFrame->GetActiveView();
	LPARAM pLparam;
	pLparam = reinterpret_cast<LPARAM>("ARALGIS");

	HANDLE Handles[2];
	Handles[0] = pFilterProcessing->ShutdownEvent;
	Handles[1] = g_ProcessFilter3Event;

	for (;;)
	{
		DWORD EventCaused = WaitForMultipleObjects(2,
												   Handles,
												   FALSE,
												   INFINITE);

		if (EventCaused == WAIT_FAILED || EventCaused == WAIT_OBJECT_0)
		{
			if (EventCaused == WAIT_FAILED)
				TRACE("WaitForMultipleObjects failure at FilterProcessingThread Number 3", GetLastError());
			else
				TRACE("FilterProcessingThread Number 3 is shutting Down normally...\n");

			return THREADEXIT_SUCCESS;
		}

		// g_ProcessFilter3Event is received
		else if (EventCaused == WAIT_OBJECT_0 + 1)
		{
			ResetEvent(g_ProcessFilter3Event);

			if (g_CameraPixelBits == 24)
			{
				cv::cvtColor(g_CVImageTest, inGray, CV_RGB2GRAY);

				pixkit::enhancement_local::CLAHEnon1987(inGray, g_CVImageTestFilter3, cv::Size(8, 8));
			}
			else
			{
				//pixkit::enhancement_local::CLAHEnon1987(g_CVImageTest, g_CVImageTestFilter3, cv::Size(8, 8));
				//medianBlur(g_CVImageTest, g_CVImageTestFilter3, 25);

				cv::Mat new_image = cv::Mat::zeros(g_CVImageTest.size(), g_CVImageTest.type());
				// create a matrix with all elements equal to 255 for subtraction     
				cv::Mat sub_mat = cv::Mat::ones(g_CVImageTest.size(), g_CVImageTest.type()) * 255;
				//subtract the original matrix by sub_mat to give the negative output new_image     
				cv::subtract(sub_mat, g_CVImageTest, g_CVImageTestFilter3);
			}

			pView->SendMessage(WM_FILTER3_READY, 0, pLparam);
		}

	} // infite for loop

	return THREADEXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CImageFilterProcessing::RefFilter1ProcessingThread
// 
// DESCRIPTION:	thread function for starting filter-1 processig for ref image 
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
////////////////////////////////////////////////////////////////////////////////
UINT __stdcall CImageFilterProcessing::RefFilter1ProcessingThread(LPVOID pParam)
{
	CImageFilterProcessing *pFilterProcessing = (CImageFilterProcessing*)pParam;

	TRACE("Ref-Filter Processing Thread Number 1 Started\n");

	HANDLE Handles[2];
	Handles[0] = pFilterProcessing->ShutdownEvent;
	Handles[1] = g_ProcessFilter4Event;

	CView * pView = pFilterProcessing->m_pFrame->GetActiveView();
	LPARAM pLparam;
	pLparam = reinterpret_cast<LPARAM>("ARALGIS");

	for (;;)
	{
		DWORD EventCaused = WaitForMultipleObjects(2,
													Handles,
													FALSE,
													INFINITE);

		if (EventCaused == WAIT_FAILED || EventCaused == WAIT_OBJECT_0)
		{
			if (EventCaused == WAIT_FAILED)
				TRACE("WaitForMultipleObjects failure at Ref-FilterProcessingThread Number 1", GetLastError());
			else
				TRACE("Ref-FilterProcessingThread Number 1 is shutting Down normally...\n");

			return THREADEXIT_SUCCESS;
		}

		// g_ProcessFilter1Event is received
		else if (EventCaused == WAIT_OBJECT_0 + 1)
		{
			ResetEvent(g_ProcessFilter4Event);

			if (g_CameraPixelBits == 24)
			{
				// READ RGB color image and convert it to Lab
				cv::Mat lab_image;
				cv::Mat refImg;

				EnterCriticalSection(&RefImageCS);
				//g_CVImageRef.copyTo(refImg);
				refImg = g_CVImageRef.clone();
				LeaveCriticalSection(&RefImageCS);

				cv::cvtColor(refImg, lab_image, CV_BGR2Lab);

				// Extract the L channel
				std::vector<cv::Mat> lab_planes(3);
				cv::split(lab_image, lab_planes);  // now we have the L image in lab_planes[0]

				// apply the CLAHE algorithm to the L channel
				cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
				clahe->setClipLimit(4);
				cv::Mat dst;
				clahe->apply(lab_planes[0], dst);

				// Merge the the color planes back into an Lab image
				//dst.copyTo(lab_planes[0]);
				lab_planes[0] = dst.clone();
				cv::merge(lab_planes, lab_image);

				// convert back to RGB
				cv::Mat image_clahe;
				cv::cvtColor(lab_image, g_CVImageRefFilter1, CV_Lab2BGR);
			}
			else
			{
				cv::Mat refImg;
				cv::Mat dummy;


				EnterCriticalSection(&RefImageCS);
				//g_CVImageRef.copyTo(refImg);
				refImg.create(g_CVImageRef.size(), g_CVImageRef.type());
				g_CVImageRefFilter1.create(g_CVImageRef.size(), g_CVImageRef.type());
				refImg = g_CVImageRef.clone();
				LeaveCriticalSection(&RefImageCS);

				cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
				clahe->setClipLimit(4);

				clahe->apply(refImg, g_CVImageRefFilter1);

				//refImg.copyTo(g_CVImageRefFilter1);
				int AAA = 0;
			}

			pView->SendMessage(WM_FILTER4_READY, 0, pLparam);
		}

	} // infite for loop

	return THREADEXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CImageFilterProcessing::RefFilter2ProcessingThread
// 
// DESCRIPTION:	thread function for starting filter-2 processig for ref image 
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
////////////////////////////////////////////////////////////////////////////////
UINT __stdcall CImageFilterProcessing::RefFilter2ProcessingThread(LPVOID pParam)
{
	CImageFilterProcessing *pFilterProcessing = (CImageFilterProcessing*)pParam;

	cv::Mat inGray;

	TRACE("Ref-Filter Processing Thread Number 2 Started\n");

	CView * pView = pFilterProcessing->m_pFrame->GetActiveView();
	LPARAM pLparam;
	pLparam = reinterpret_cast<LPARAM>("ARALGIS");

	HANDLE Handles[2];
	Handles[0] = pFilterProcessing->ShutdownEvent;
	Handles[1] = g_ProcessFilter5Event;

	for (;;)
	{
		DWORD EventCaused = WaitForMultipleObjects(2,
			Handles,
			FALSE,
			INFINITE);

		if (EventCaused == WAIT_FAILED || EventCaused == WAIT_OBJECT_0)
		{
			if (EventCaused == WAIT_FAILED)
				TRACE("WaitForMultipleObjects failure at Ref-FilterProcessingThread Number 2", GetLastError());
			else
				TRACE("Ref-FilterProcessingThread Number 2 is shutting Down normally...\n");

			return THREADEXIT_SUCCESS;
		}

		// g_ProcessFilter2Event is received
		else if (EventCaused == WAIT_OBJECT_0 + 1)
		{
			ResetEvent(g_ProcessFilter5Event);

			if (g_CameraPixelBits == 24)
			{
				cv::Mat refImg;

				EnterCriticalSection(&RefImageCS);
				//g_CVImageRef.copyTo(refImg);
				refImg = g_CVImageRef.clone();
				LeaveCriticalSection(&RefImageCS);

				cv::cvtColor(refImg, inGray, CV_RGB2GRAY);

				cv::equalizeHist(inGray, g_CVImageRefFilter2);
			}
			else
			{
				cv::Mat refImg;
				cv::Mat dummy;

				//Sleep(15000);


				EnterCriticalSection(&RefImageCS);
				//g_CVImageRef.copyTo(refImg);
				refImg.create(g_CVImageRef.size(), g_CVImageRef.type());
				g_CVImageRefFilter2.create(g_CVImageRef.size(), g_CVImageRef.type());
				refImg = g_CVImageRef.clone();
				LeaveCriticalSection(&RefImageCS);


				//refImg = cv::imread("C:\\Users\\bora\\Desktop\\ARALGIS-GitHub-Kopya\\Cars\\ChassisBottom\\cbri-06-fvf-80.bmp", cv::IMREAD_COLOR);

				//cv::equalizeHist(refImg, g_CVImageRefFilter2);
				//refImg.copyTo(g_CVImageRefFilter2);
				//dummy = refImg.clone();
				cv::equalizeHist(refImg, g_CVImageRefFilter2);
				int AAAA = 0;
			}

			pView->SendMessage(WM_FILTER5_READY, 0, pLparam);
		}

	} // infite for loop

	return THREADEXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CImageFilterProcessing::RefFilter3ProcessingThread
// 
// DESCRIPTION:	thread function for starting filter-3 processig for ref image 
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
////////////////////////////////////////////////////////////////////////////////
UINT __stdcall CImageFilterProcessing::RefFilter3ProcessingThread(LPVOID pParam)
{
	CImageFilterProcessing *pFilterProcessing = (CImageFilterProcessing*)pParam;

	cv::Mat inGray;

	TRACE("Ref-Filter Processing Thread Number 3 Started\n");

	CView * pView = pFilterProcessing->m_pFrame->GetActiveView();
	LPARAM pLparam;
	pLparam = reinterpret_cast<LPARAM>("ARALGIS");

	HANDLE Handles[2];
	Handles[0] = pFilterProcessing->ShutdownEvent;
	Handles[1] = g_ProcessFilter6Event;

	for (;;)
	{
		DWORD EventCaused = WaitForMultipleObjects(2,
													Handles,
													FALSE,
													INFINITE);

		if (EventCaused == WAIT_FAILED || EventCaused == WAIT_OBJECT_0)
		{
			if (EventCaused == WAIT_FAILED)
				TRACE("WaitForMultipleObjects failure at Ref-FilterProcessingThread Number 3", GetLastError());
			else
				TRACE("Ref-FilterProcessingThread Number 3 is shutting Down normally...\n");

			return THREADEXIT_SUCCESS;
		}

		// g_ProcessFilter3Event is received
		else if (EventCaused == WAIT_OBJECT_0 + 1)
		{
			ResetEvent(g_ProcessFilter6Event);

			if (g_CameraPixelBits == 24)
			{
				cv::Mat refImg;

				EnterCriticalSection(&RefImageCS);
				//g_CVImageRef.copyTo(refImg);
				refImg = g_CVImageRef.clone();
				LeaveCriticalSection(&RefImageCS);

				cv::cvtColor(refImg, inGray, CV_RGB2GRAY);

				pixkit::enhancement_local::CLAHEnon1987(inGray, g_CVImageRefFilter3, cv::Size(8, 8));
			}
			else
			{
				cv::Mat refImg;

				EnterCriticalSection(&RefImageCS);
				//g_CVImageRef.copyTo(refImg);
				refImg.create(g_CVImageRef.size(), g_CVImageRef.type());
				g_CVImageRefFilter3.create(g_CVImageRef.size(), g_CVImageRef.type());
				refImg = g_CVImageRef.clone();
				LeaveCriticalSection(&RefImageCS);

				//refImg.copyTo(g_CVImageRefFilter3);

				//pixkit::enhancement_local::CLAHEnon1987(refImg, g_CVImageRefFilter3, cv::Size(8, 8));
				//medianBlur(refImg, g_CVImageRefFilter3, 25);

				// initialize the output matrix with zeros     
				cv::Mat new_image = cv::Mat::zeros(refImg.size(), refImg.type());
				// create a matrix with all elements equal to 255 for subtraction     
				cv::Mat sub_mat = cv::Mat::ones(refImg.size(), refImg.type()) * 255;
				//subtract the original matrix by sub_mat to give the negative output new_image     
				cv::subtract(sub_mat, refImg, g_CVImageRefFilter3);
			}

			pView->SendMessage(WM_FILTER6_READY, 0, pLparam);
		}

	} // infite for loop

	return THREADEXIT_SUCCESS;
}

