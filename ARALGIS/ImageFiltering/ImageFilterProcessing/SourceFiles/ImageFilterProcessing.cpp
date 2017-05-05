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
BOOL CImageFilterProcessing::Start(SCNOTIFYPROC pNotifyProc, CMainFrame* pFrame)
{
	if (bRun)
	{
		TRACE("_beginthreadex(...) failure, for Launch ThreadFilterProcessing::Start\n");
		return FALSE;
	}

	TRACE("Filter Processing Threads 1, 2, and 3 Starting ...\n");

	m_pFrame = pFrame;
	m_pNotifyProc = pNotifyProc;

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


	bRun = TRUE;

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


	CloseHandle(ShutdownEvent);
	CloseHandle(ThreadFilter1Processing);
	CloseHandle(ThreadFilter2Processing);
	CloseHandle(ThreadFilter3Processing);

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

			pixkit::enhancement_global::RajuNair2014(g_CVImageTest, g_CVImageTestFilter1);

			//cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, cv::Size(8, 8));
			//clahe->setClipLimit(4);
			//clahe->apply(g_CVImageTest, g_CVImageTestFilter1);

			pFilterProcessing->m_pNotifyProc((LPVOID)pFilterProcessing->m_pFrame, FILTER_PROCESS_FILTER1_READY);
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

			cv::cvtColor(g_CVImageTest, inGray, CV_RGB2GRAY);

			cv::equalizeHist(inGray, g_CVImageTestFilter2);

			//pixkit::enhancement_global::GlobalHistogramEqualization1992(inGray, g_CVImageTestFilter2);

			pFilterProcessing->m_pNotifyProc((LPVOID)pFilterProcessing->m_pFrame, FILTER_PROCESS_FILTER2_READY);
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

			cv::cvtColor(g_CVImageTest, inGray, CV_RGB2GRAY);

			pixkit::enhancement_local::CLAHEnon1987(inGray, g_CVImageTestFilter3, cv::Size(8, 8));

			pFilterProcessing->m_pNotifyProc((LPVOID)pFilterProcessing->m_pFrame, FILTER_PROCESS_FILTER3_READY);
		}

	} // infite for loop

	return THREADEXIT_SUCCESS;
}

