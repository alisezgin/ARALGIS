// CameraDataReceiver.cpp: implementation of the CameraDataReceiver class.
//
/////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "..\\HeaderFiles\\ChangeDetectController.h"
#include "MainFrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CChangeDetectController::CChangeDetectController
// 
// DESCRIPTION:	Constructor.
////////////////////////////////////////////////////////////////////////////////
CChangeDetectController::CChangeDetectController()
{
	m_bInit = FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CChangeDetectController::~CChangeDetectController
// 
// DESCRIPTION:	Destructor.
////////////////////////////////////////////////////////////////////////////////
CChangeDetectController::~CChangeDetectController()
{
	Shutdown();
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CChangeDetectController::Start
// 
// DESCRIPTION: starts the CameraDataReceiver thread
// 
////////////////////////////////////////////////////////////////////////////////
bool CChangeDetectController::Start(CMainFrame* pFrame)
{
	TRACE("ChangeDetectController Starting ...\n");

	m_pFrame = pFrame;

	ShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// Launch Add Print Job Thread
	ThreadChangeDetector = (HANDLE)_beginthreadex(NULL,
												  0,
												  ChangeDetectorThread,
												  this,
												  0,
												  &ThreadID
												  );

	if (!ThreadChangeDetector)
	{
		TRACE("_beginthreadex(...) failure, ThreadChangeDetector::Start\n");
		return FALSE;
	}
	TRACE("ThreadChangeDetector ThreadID %x ...\n", ThreadID);

	m_bInit = TRUE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CChangeDetectController::Shutdown
// 
// DESCRIPTION: stops the ChangeDetector thread
// 
////////////////////////////////////////////////////////////////////////////////
bool CChangeDetectController::Shutdown()
{
	if (m_bInit == FALSE)
		return FALSE;

	TRACE("ThreadChangeDetector Shutting down ...\n");

	SetEvent(ShutdownEvent);

	// wait for the thread to stop
	WaitForSingleObject(ThreadChangeDetector, INFINITE);

	CloseHandle(ShutdownEvent);
	CloseHandle(ThreadChangeDetector);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CChangeDetectController::ChangeDetectorThread
// 
// DESCRIPTION: thread function that performs change detection.	 
////////////////////////////////////////////////////////////////////////////////
UINT __stdcall CChangeDetectController::ChangeDetectorThread(LPVOID pParam)
{
	CChangeDetectController *pChangeDetector = (CChangeDetectController*)pParam;

	bool bIsProcessingInProgress = false;

	HANDLE	Handles[2];
	Handles[0] = pChangeDetector->ShutdownEvent;
	Handles[1] = g_StartChangeDetectEvent;

	CView * pView = pChangeDetector->m_pFrame->GetActiveView();
	LPARAM pLparam;
	pLparam = reinterpret_cast<LPARAM>("ARALGIS");

	// this line is very important and must be present for each launched thread.
	// COM is initialized as Multi-Threaded Apartment Model
	// so all of the threads can access the data but I have to put
	// synchronization material manually
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	// boraN code analysis
	// If the above function fails, check the code

	for (;;)
	{
		DWORD EventCaused = WaitForMultipleObjects(2,
												   Handles,
												   FALSE,
												   INFINITE);

		if (EventCaused == WAIT_FAILED)
		{
			DWORD errorCode = GetLastError();
			TRACE("WaitForMultipleObjects failure at CameraDataReceiver\n", GetLastError());
			break;
		}

		else if (EventCaused == WAIT_OBJECT_0)
		{
			TRACE("CameraDataReceiver is shutting Down...\n");
			break;
		}

		else if (EventCaused == (WAIT_OBJECT_0 + 1)) // Start Data Reception From Camera
		{
			// reset the event for further processing
			ResetEvent(g_StartChangeDetectEvent);

			cv::Mat refImg;
			cv::Mat testImg;


			EnterCriticalSection(&RefImageCS);
			
			refImg.create(g_CVImageRef.size(), g_CVImageRef.type());
			refImg = g_CVImageRef.clone();

			LeaveCriticalSection(&RefImageCS);

			testImg.create(g_CVImageTest.size(), g_CVImageTest.type());
			testImg = g_CVImageTest.clone();


			pChangeDetector->m_MyChangeDetector.process(refImg, testImg);

			pView->SendMessage(WM_CHANGEDETECT_FINISHED, 0, pLparam);
		}
	}
	return 1;
}