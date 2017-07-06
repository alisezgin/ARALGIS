// FreeDiskSpaceController.cpp: implementation of the CFreeDiskSpaceController class.
//
/////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ARALGIS.h"
#include "..\\HeaderFiles\\FreeDiskSpaceController.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CFreeDiskSpaceController::CFreeDiskSpaceController
// 
// DESCRIPTION:	Constructor.
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// BN            26062003	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
CFreeDiskSpaceController::CFreeDiskSpaceController()
{
   m_bInit = FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CFreeDiskSpaceController::~CFreeDiskSpaceController
// 
// DESCRIPTION:	Destructor.
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// BN            26062003	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
CFreeDiskSpaceController::~CFreeDiskSpaceController()
{
   Shutdown();
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CFreeDiskSpaceController::Start
// 
// DESCRIPTION: starts the Purge expired files thread
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// Name				Date		Version		Comments
// BN            26062003	1.0			Origin 
// 
////////////////////////////////////////////////////////////////////////////////
bool CFreeDiskSpaceController::Start()
{
	TRACE("FreeDiskSpaceController Starting ...\n");

	ShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	// Launch Add Print Job Thread
	ThreadFreeDiskSpaceController = (HANDLE)_beginthreadex( NULL, 
		                                               0, 
															        FreeDiskSpaceControllerThread, 
															        this, 
															        0, 
															        &ThreadID
															      );
	if(!ThreadFreeDiskSpaceController)
	{
		TRACE("_beginthreadex(...) failure, FreeDiskSpaceController::Start\n");
		return FALSE;
	}
	TRACE("FreeDiskSpaceController ThreadID %x ...\n", ThreadID);

	m_bInit = TRUE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CFreeDiskSpaceController::Shutdown
// 
// DESCRIPTION: stops the Purge expired files thread
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// Name				Date		Version		Comments
// BN            26062003	1.0			Origin 
// 
////////////////////////////////////////////////////////////////////////////////
bool CFreeDiskSpaceController::Shutdown()
{
	if (m_bInit == FALSE)
		return FALSE;

	TRACE("FreeDiskSpaceController Shutting down ...\n");

	SetEvent(ShutdownEvent);

	// wait for the thread to stop
   WaitForSingleObject(ThreadFreeDiskSpaceController, INFINITE);

	CloseHandle(ShutdownEvent);
	CloseHandle(ThreadFreeDiskSpaceController);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CFreeDiskSpaceController::FreeDiskSpaceControllerThread
// 
// DESCRIPTION: thread function that control disk space, if less than a threshold 
//              delete oldest files to be on the safe side.	 
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// Name				Date		Version		Comments
// BN            26062003	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
UINT __stdcall CFreeDiskSpaceController::FreeDiskSpaceControllerThread(LPVOID pParam)
{
	CFreeDiskSpaceController *pServer = (CFreeDiskSpaceController*)pParam;

	ULARGE_INTEGER i64FreeBytesToCaller;
	ULARGE_INTEGER i64TotalBytes;
	ULARGE_INTEGER i64FreeBytes;
	BOOL           ret_val;

	HANDLE         Handles[2];
	Handles[0] = pServer->ShutdownEvent;
	Handles[1] = g_ControlHardDiskSpaceEvent;

	// this line is very important and must be present for each launched thread.
	// COM is initialized as Multi-Threaded Apartment Model
	// so all of the threads can access the data but I have to put
	// synchronization material manually
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	for (;;)
	{
		DWORD EventCaused = WaitForMultipleObjects(2,
			Handles,
			FALSE,
			INFINITE);

		if (EventCaused == WAIT_FAILED || EventCaused == WAIT_OBJECT_0)
		{
			if (EventCaused == WAIT_FAILED)
				TRACE("WaitForMultipleObjects failure at FreeDiskSpaceController",
				GetLastError());
			else
				TRACE("FreeDiskSpaceController is shutting Down normally...\n");
			break;

			return THREADEXIT_SUCCESS;
		}

		else if (EventCaused == (WAIT_OBJECT_0 + 1)) // ControlHardDiskSpace
		{
			ret_val = GetDiskFreeSpaceEx((LPCTSTR)NULL,//g_ReferenceFilePath,
										 (PULARGE_INTEGER)&i64FreeBytesToCaller,
										 (PULARGE_INTEGER)&i64TotalBytes,
										 (PULARGE_INTEGER)&i64FreeBytes);
			if (!ret_val)
			{
				DWORD dError = GetLastError();
				TRACE("\nCan not Get Disk Free Space. Error No: %ld \n\n", dError);
			}
			else
			{
				TRACE("Disk Free Space does not return error\n");
			}

			ULONG diskFreeSpace;
			// 1048576 = 1024 * 1024 = 1 MByte
			diskFreeSpace = (ULONG)(i64FreeBytesToCaller.QuadPart / (ULONG)1048576);

			// debugBN begin 
			//diskFreeSpace = CRITICAL_FREE_SPACE_AMOUNT - 2;
			// debugBN end

			// if critical amount of memory start purging 
			if (diskFreeSpace < CRITICAL_FREE_SPACE_AMOUNT)
			{
				TRACE("\nFreeDiskSpaceController Size Alert!!!!!\n\n\n\n");

				::MessageBox(NULL,
					(LPCWSTR)L"Disk Alaný Sorunu. Lütfen Sistem Yöneticisine Haber Veriniz",
					(LPCWSTR)WARNINGWINDOW_TITLE,
					MB_OK | MB_ICONERROR
					);

				// do not set event here becaue of other controls at OnTimer
				//SetEvent(DeleteExpiredFilesEvent);
			}
			// reset the event for further processing
			ResetEvent(g_ControlHardDiskSpaceEvent);
		}
	}
	return 1;
}