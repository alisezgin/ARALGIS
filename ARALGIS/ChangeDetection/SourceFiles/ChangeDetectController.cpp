// CameraDataReceiver.cpp: implementation of the CameraDataReceiver class.
//
/////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "..\\HeaderFiles\\ChangeDetectController.h"
#include "MainFrm.h"
#include ".\\ErrorDisplay\\ThreadSafeQueue\\HeaderFiles\\ThreadSafeQueue.h"


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
	EnterCriticalSection(&g_ChangeDetectCS);
	Shutdown();
	LeaveCriticalSection(&g_ChangeDetectCS);

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

	// Launch change detector Thread
	ThreadChangeDetector = (HANDLE)_beginthreadex(NULL,
												  0,
												  ChangeDetectorThread,
												  this,
												  0,
												  &ThreadID
												  );

	if (!ThreadChangeDetector)
	{
		TRACE("_beginthreadex(...) failure, CChangeDetectController::Start::ThreadChangeDetector\n");
		return FALSE;
	}
	TRACE("ThreadChangeDetector ThreadID %x ...\n", ThreadID);

	// Launch change detector communication
	ThreadComm = (HANDLE)_beginthreadex(NULL,
										0,
										ChangeDetectorCommThread,
										this,
										0,
										&ThreadIDComm
										);

	if (!ThreadComm)
	{
		TRACE("_beginthreadex(...) failure, CChangeDetectController::Start::ThreadComm\n");
		return FALSE;
	}
	TRACE("ThreadChangeDetector ThreadID %x ...\n", ThreadIDComm);

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
	WaitForSingleObject(ThreadComm, INFINITE);

	CloseHandle(ShutdownEvent);
	CloseHandle(ThreadChangeDetector);
	CloseHandle(ThreadComm);

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
	int dRetVal = 0;

	HANDLE	Handles[2];
	Handles[0] = pChangeDetector->ShutdownEvent;
	Handles[1] = g_StartChangeDetectEvent;

	CARALGISView * pView = (CARALGISView*)pChangeDetector->m_pFrame->GetActiveView();
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
			TRACE("WaitForMultipleObjects failure at ChangeDetectorThread\n", GetLastError());
			break;
		}

		else if (EventCaused == WAIT_OBJECT_0)
		{
			TRACE("ChangeDetectorThread is shutting Down...\n");
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


			if ((refImg.cols == 0) || (refImg.rows == 0))
			{
				ERRORMESSAGETYPE dTmp;
				char* cText = "Kayýtlý araç altý görüntüsü bozuk. Lütfen tekrar deneyiniz";
				strncpy_s(dTmp._OdroidText, (size_t)(MAX_PATH), cText, (size_t)(MAX_PATH));
				CThreadSafeQueue<_errorMessageType>::getInstance().push(dTmp);
			}
			else if ((testImg.cols == 0) || (testImg.rows == 0))
			{
				ERRORMESSAGETYPE dTmp;
				char* cText = "Yeni araç altý görüntüsü bozuk. Lütfen tekrar deneyiniz";
				strncpy_s(dTmp._OdroidText, (size_t)(MAX_PATH), cText, (size_t)(MAX_PATH));
				CThreadSafeQueue<_errorMessageType>::getInstance().push(dTmp);
			}
			else
			{
				EnterCriticalSection(&g_ChangeDetectCS);
				pChangeDetector->m_MyChangeDetector.SetStopFlag(false);
				dRetVal = pChangeDetector->m_MyChangeDetector.process(refImg, testImg);

				if (pView->m_IsActive == false)
				{
					LeaveCriticalSection(&g_ChangeDetectCS);
				}
				else
				{
					if (dRetVal == 1)
					{
						pView->SendMessage(WM_CHANGEDETECT_FINISHED, 0, pLparam);
					}
					LeaveCriticalSection(&g_ChangeDetectCS);

				}
			}

			
		}
	}
	return THREADEXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CChangeDetectController::ChangeDetectorCommThread
// 
// DESCRIPTION: thread function that communicates with system.	 
////////////////////////////////////////////////////////////////////////////////
UINT __stdcall CChangeDetectController::ChangeDetectorCommThread(LPVOID pParam)
{
	CChangeDetectController *pChangeDetector = (CChangeDetectController*)pParam;

	bool bIsProcessingInProgress = false;

	HANDLE	Handles[2];
	Handles[0] = pChangeDetector->ShutdownEvent;
	Handles[1] = g_StopChangeDetectionEvent;

	//CView * pView = pChangeDetector->m_pFrame->GetActiveView();
	//LPARAM pLparam;
	//pLparam = reinterpret_cast<LPARAM>("ARALGIS");

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

		else if (EventCaused == (WAIT_OBJECT_0 + 1)) // g_StopChangeDetectionEvent
		{
			// reset the event for further processing
			ResetEvent(g_StopChangeDetectionEvent);
			pChangeDetector->m_MyChangeDetector.SetStopFlag(true);
		}
	}
	return THREADEXIT_SUCCESS;
}