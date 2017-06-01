// CameraDataReceiver.cpp: implementation of the CameraDataReceiver class.
//
/////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "..\\HeaderFiles\\CameraDataReceiver.h"
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
// FUNCTION:	CCameraDataReceiver::CCameraDataReceiver
// 
// DESCRIPTION:	Constructor.
////////////////////////////////////////////////////////////////////////////////
CCameraDataReceiver::CCameraDataReceiver()
{
	m_bInit = FALSE;


	///////////////////////////// TIMER ///////////////////////////////////////
	m_hTimer = NULL;

	// Create an unnamed waitable timer.
	m_hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	if (NULL == m_hTimer)
	{
		TRACE("CreateWaitableTimer failed (%d)\n", GetLastError());
	}
	///////////////////////////// TIMER ///////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CCameraDataReceiver::~CCameraDataReceiver
// 
// DESCRIPTION:	Destructor.
////////////////////////////////////////////////////////////////////////////////
CCameraDataReceiver::~CCameraDataReceiver()
{
	Shutdown();
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CCameraDataReceiver::Start
// 
// DESCRIPTION: starts the CameraDataReceiver thread
// 
////////////////////////////////////////////////////////////////////////////////
bool CCameraDataReceiver::Start(CMainFrame* pFrame)
{
	TRACE("CameraDataReceiver Starting ...\n");

	m_pFrame = pFrame;

	ShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// Launch Add Print Job Thread
	ThreadCameraDataReceiver = (HANDLE)_beginthreadex(NULL,
		0,
		CameraDataReceiverThread,
		this,
		0,
		&ThreadID
		);

	if (!ThreadCameraDataReceiver)
	{
		TRACE("_beginthreadex(...) failure, ThreadCameraDataReceiver::Start\n");
		return FALSE;
	}
	TRACE("ThreadCameraDataReceiver ThreadID %x ...\n", ThreadID);

	m_bInit = TRUE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CCameraDataReceiver::Shutdown
// 
// DESCRIPTION: stops the CameraDataReceiver thread
// 
////////////////////////////////////////////////////////////////////////////////
bool CCameraDataReceiver::Shutdown()
{
	if (m_bInit == FALSE)
		return FALSE;

	TRACE("ThreadCameraDataReceiver Shutting down ...\n");

	SetEvent(ShutdownEvent);

	// wait for the thread to stop
	WaitForSingleObject(ThreadCameraDataReceiver, INFINITE);

	CloseHandle(ShutdownEvent);
	CloseHandle(ThreadCameraDataReceiver);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CCameraDataReceiver::CameraDataReceiverThread
// 
// DESCRIPTION: thread function that receive image data from line scan camera.	 
////////////////////////////////////////////////////////////////////////////////
UINT __stdcall CCameraDataReceiver::CameraDataReceiverThread(LPVOID pParam)
{
	CCameraDataReceiver *pServer = (CCameraDataReceiver*)pParam;

	bool bIsProcessingInProgress = false;

	HANDLE	Handles[14];
	Handles[0] = pServer->ShutdownEvent;
	Handles[1] = g_CameraStartDataRecieveEvent;
	Handles[2] = g_CameraStopDataRecieveEvent;
	Handles[3] = pServer->m_hTimer;
	Handles[4] = g_KillTimerEvent;

	Handles[5] = g_CameraChangeSampleRateEvent;  // not implemented yet
	Handles[6] = g_SetTimerFrameRateEvent;  // not used  for the time being


	// this line is very important and must be present for each launched thread.
	// COM is initialized as Multi-Threaded Apartment Model
	// so all of the threads can access the data but I have to put
	// synchronization material manually
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	// boraN code analysis
	// If the above function fails, check the code

	for (;;)
	{
		DWORD EventCaused = WaitForMultipleObjects( 7,
													Handles,
													FALSE,
													INFINITE);

		if (EventCaused == WAIT_FAILED)
		{
			DWORD errorCode = GetLastError();
			TRACE("WaitForMultipleObjects failure at CameraDataReceiver\n",GetLastError());
			break;
		}

		else if (EventCaused == WAIT_OBJECT_0)
		{
			TRACE("CameraDataReceiver is shutting Down...\n");
			break;
		}

		else if (EventCaused == (WAIT_OBJECT_0 + 1)) // Start Data Reception From Camera
		{
			if (bIsProcessingInProgress == false)
			{
				bIsProcessingInProgress = true;

				g_IntermediateCounter = 0;

				pServer->m_MyCamera.StartDataReception();
			}

			// reset the event for further processing
			ResetEvent(g_CameraStartDataRecieveEvent);
		}

		else if (EventCaused == (WAIT_OBJECT_0 + 2)) // Stop Data Reception From Camera
		{
			// reset the event for further processing
			ResetEvent(g_CameraStopDataRecieveEvent);

			pServer->m_MyCamera.StopDataReception();

			SetEvent(g_KillTimerEvent); 
		}

		else if (EventCaused == (WAIT_OBJECT_0 + 3)) // timer event  
		{
			//pServer->m_MyCamera.OnTimerCamera();
			pServer->m_MyCamera.OnWaitableTimer();
		}

		else if (EventCaused == (WAIT_OBJECT_0 + 4)) // kill timer 
		{
			if (bIsProcessingInProgress == true)
			{
				bIsProcessingInProgress = false;

				ResetEvent(g_KillTimerEvent);

				CancelWaitableTimer(pServer->m_hTimer);

				EnterCriticalSection(&g_IntermediateTestImgCS);
				// send image data to Doc class
				pServer->m_MyCamera.GetCameraDataAsMat();
				//SetEvent(g_CameraDataReadyEvent);
				LeaveCriticalSection(&g_IntermediateTestImgCS);
			}

			::InterlockedDecrement((long*)&g_carsDetectedByPTSCnt);
			TRACE("dec g_carsDetectedByPTSCnt %d \n", g_carsDetectedByPTSCnt);

			// boraN code analysis
			// use Interlocked function below for g_carsDetectedByPTSCnt if check
			if (g_carsDetectedByPTSCnt > 0)
			{
				SetEvent(g_CameraStartDataRecieveEvent);
			}

		}

		else if (EventCaused == (WAIT_OBJECT_0 + 5)) // Change Camera Sample Rate
		{
			// reset the event for further processing
			ResetEvent(g_CameraChangeSampleRateEvent);

			//pServer->m_MyCamera.StopDataReception();
		}

		else if (EventCaused == (WAIT_OBJECT_0 + 6)) // set timer frame rate 
		{
			ResetEvent(g_SetTimerFrameRateEvent);
			int tFrameTime = pServer->m_MyCamera.GetFrameTime();   /// boraN time iþine bak
			/////// boraN tFrameTime iþini iyice öðren

			__int64 qwDueTime;
			LARGE_INTEGER   liDueTime;

			qwDueTime = -10 * tFrameTime; // -10 is: negative since relative time
			                              //       : multiplied by 10, since it is in 100 nanoseconds 
			                              //         and  tFrameTime is in miliseconds

			liDueTime.QuadPart = (long long)qwDueTime;

			//Set a timer to wait for XX seconds.
			if (!SetWaitableTimer(pServer->m_hTimer, &liDueTime, tFrameTime, NULL, NULL, 0))
			{
				TRACE("SetWaitableTimer failed (%d)\n", GetLastError());
				return 2;
			}
		}
	}
	return 1;
}