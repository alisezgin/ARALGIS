// CameraDataReceiver.cpp: implementation of the CameraDataReceiver class.
//
/////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "..\\HeaderFiles\\CameraDataReceiver.h"

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


	///////////////////////////// CAMERA ///////////////////////////////////////

	//m_MyCamera.
	///////////////////////////// CAMERA ///////////////////////////////////////


	///////////////////////////// TIMER ///////////////////////////////////////
	m_hTimer = NULL;

	// Create an unnamed waitable timer.
	m_hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	if (NULL == m_hTimer)
	{
		printf("CreateWaitableTimer failed (%d)\n", GetLastError());
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
// FUNCTION:	CCameraDataReceiver::CameraDataReceiverThread
// 
// DESCRIPTION: thread function that receive image data from line scan camera.	 
////////////////////////////////////////////////////////////////////////////////
UINT __stdcall CCameraDataReceiver::CameraDataReceiverThread(LPVOID pParam)
{
	CCameraDataReceiver *pServer = (CCameraDataReceiver*)pParam;

	HANDLE	Handles[14];
	Handles[0] = pServer->ShutdownEvent;
	Handles[1] = g_CameraStartDataRecieveEvent;
	Handles[2] = g_CameraStopDataRecieveEvent;
	Handles[3] = g_CameraPauseDataRecieveEvent;
	Handles[4] = g_CameraConfigFileChangeEvent;
	Handles[5] = g_CameraSelectStreamFileEvent;
	Handles[6] = g_CameraUpdateControlsEvent;
	Handles[7] = pServer->m_hTimer;
	Handles[8] = g_DisplayBitmapEvent;
	Handles[9] = g_DisplayOpenCVEvent;
	Handles[10] = g_SetTimerFrameRateEvent;
	Handles[11] = g_KillTimerEvent;
	Handles[12] = g_ResetTimerEvent;
	Handles[13] = g_CameraChangeSampleRateEvent;


	// this line is very important and must be present for each launched thread.
	// COM is initialized as Multi-Threaded Apartment Model
	// so all of the threads can access the data but I have to put
	// synchronization material manually
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	for (;;)
	{
		DWORD EventCaused = WaitForMultipleObjects(14,
			Handles,
			FALSE,
			INFINITE);

		if (EventCaused == WAIT_FAILED)
		{
			DWORD errorCode = GetLastError();
			TRACE("WaitForMultipleObjects failure at CameraDataReceiver",GetLastError());
			break;
		}

		else if (EventCaused == WAIT_OBJECT_0)
		{
			TRACE("CameraDataReceiver is shutting Down...");
			break;
		}

		//if (EventCaused == WAIT_FAILED || EventCaused == WAIT_OBJECT_0)
		//{
		//	if (EventCaused == WAIT_FAILED)
		//		TRACE("WaitForMultipleObjects failure at CameraDataReceiver",
		//		GetLastError());
		//	else
		//		TRACE("CameraDataReceiver is shutting Down...");
		//	break;
		//}

		else if (EventCaused == (WAIT_OBJECT_0 + 1)) // Start Data Reception From Camera
		{
			//ret_val = GetDiskFreeSpaceEx((LPCTSTR)LocalPrintFileDir,
			//	(PULARGE_INTEGER)&i64FreeBytesToCaller,
			//	(PULARGE_INTEGER)&i64TotalBytes,
			//	(PULARGE_INTEGER)&i64FreeBytes);
			//if (!ret_val)
			//{
			//	TRACE("\nCan not Get Disk Free Space\n\n");
			//}

			//ULONG diskFreeSpace;
			//// 1048576 = 1024 * 1024 = 1 MByte
			//diskFreeSpace = (ULONG)(i64FreeBytesToCaller.QuadPart / (ULONG)1048576);

			//// debugBN begin 
			//diskFreeSpace = CRITICAL_FREE_SPACE_AMOUNT - 2;
			//// debugBN end

			//// if critical amount of memory start purging 
			//if (diskFreeSpace < CRITICAL_FREE_SPACE_AMOUNT)
			//{
			//	// trigger the event for the next timer tick
			//	m_iSecCntExpiredFiles = (periodFilePurge*TimerNormalizer) - 1;

			//	TRACE("\nFreeDiskSpaceController Size Alert!!!!!\n\n\n\n");

			//	// do not set event here becaue of other controls at OnTimer
			//	//SetEvent(DeleteExpiredFilesEvent);
			//}

			// reset the event for further processing
			ResetEvent(g_CameraStartDataRecieveEvent);

			pServer->m_MyCamera.StartDataReception();

			EnterCriticalSection(&g_SomeHandlingCS);

			// enable the other database handling actions
			//isDatabaseHandlingInProgress = FALSE;

			LeaveCriticalSection(&g_SomeHandlingCS);
		}

		else if (EventCaused == (WAIT_OBJECT_0 + 2)) // Stop Data Reception From Camera
		{
			// reset the event for further processing
			ResetEvent(g_CameraStopDataRecieveEvent);

			pServer->m_MyCamera.StopDataReception();

			EnterCriticalSection(&g_SomeHandlingCS);

			// enable the other database handling actions
			//isDatabaseHandlingInProgress = FALSE;

			LeaveCriticalSection(&g_SomeHandlingCS);
		}

		else if (EventCaused == (WAIT_OBJECT_0 + 13)) // Change Camera Sample Rate
		{
			// reset the event for further processing
			ResetEvent(g_CameraChangeSampleRateEvent);

			//pServer->m_MyCamera.StopDataReception();

			EnterCriticalSection(&g_SomeHandlingCS);

			// enable the other database handling actions
			//isDatabaseHandlingInProgress = FALSE;

			LeaveCriticalSection(&g_SomeHandlingCS);
		}

		else if (EventCaused == (WAIT_OBJECT_0 + 3)) // Pause Data Reception From Camera
		{
			// reset the event for further processing
			ResetEvent(g_CameraPauseDataRecieveEvent);

			pServer->m_MyCamera.PauseDataReception();
		}

		////////////////////////////////////////
		else if (EventCaused == (WAIT_OBJECT_0 + 4)) // Load Camera Configuration File
		{
			// reset the event for further processing
			ResetEvent(g_CameraConfigFileChangeEvent);

			pServer->m_MyCamera.LoadConfiguationFile();

		}

		else if (EventCaused == (WAIT_OBJECT_0 + 5)) // Display Recording Selection Dialog
		{
			// reset the event for further processing
			ResetEvent(g_CameraSelectStreamFileEvent);
			pServer->m_MyCamera.DisplayRecordSelectDialog();
		}

		else if (EventCaused == (WAIT_OBJECT_0 + 6)) // update camera controls  
		{
			// reset the event for further processing
			ResetEvent(g_CameraUpdateControlsEvent);
			pServer->m_MyCamera.UpdateControls();
		}

		else if (EventCaused == (WAIT_OBJECT_0 + 7)) // timer event  
		{
			//::MessageBox(NULL, L("TIMER."), NULL, MB_OK);
			pServer->m_MyCamera.OnWaitableTimer();
		}

		else if (EventCaused == (WAIT_OBJECT_0 + 8)) // display bitmap window  
		{
			ResetEvent(g_DisplayBitmapEvent);
	}

		else if (EventCaused == (WAIT_OBJECT_0 + 9)) // display openCV window 
		{
			ResetEvent(g_DisplayOpenCVEvent);
		}

		else if (EventCaused == (WAIT_OBJECT_0 + 10)) // set timer frame rate 
		{
			ResetEvent(g_SetTimerFrameRateEvent);
			int tFrameTime = pServer->m_MyCamera.GetFrameTime();

			__int64 qwDueTime;
			LARGE_INTEGER   liDueTime;

			qwDueTime = -10;

			// Copy the relative time into a LARGE_INTEGER.
			liDueTime.LowPart = (DWORD)(qwDueTime & 0xFFFFFFFF);
			liDueTime.HighPart = (LONG)(qwDueTime >> 32);


			// Set a timer to wait for XX seconds.
			if (!SetWaitableTimer(pServer->m_hTimer, &liDueTime, tFrameTime, NULL, NULL, 0))
			{
				printf("SetWaitableTimer failed (%d)\n", GetLastError());
				return 2;
			}
		}

		else if (EventCaused == (WAIT_OBJECT_0 + 11)) // kill timer 
		{
			ResetEvent(g_KillTimerEvent);
			CancelWaitableTimer(pServer->m_hTimer);
			// send image data to Doc class
			pServer->m_MyCamera.GetCameraDataAsMat();
			//SetEvent(g_CameraDataReadyEvent);
		}

		else if (EventCaused == (WAIT_OBJECT_0 + 12)) // reset timer 
		{
			ResetEvent(g_ResetTimerEvent);
			//pServer->m_MyCamera.DisplayOpenCVWindow();
		}
		

	}
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CCameraDataReceiver::Start
// 
// DESCRIPTION: starts the CameraDataReceiver thread
// 
////////////////////////////////////////////////////////////////////////////////
bool CCameraDataReceiver::Start()
{
	TRACE("CameraDataReceiver Starting ...\n");

	ShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// Launch Add Print Job Thread
	ThreadCameraDataReceiver = (HANDLE)_beginthreadex(	NULL,
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