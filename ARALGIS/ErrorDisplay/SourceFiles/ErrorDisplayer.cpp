// ErrorDisplayer.cpp: implementation of the CErrorDisplayer class.
//
/////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ARALGIS.h"
#include "..\\HeaderFiles\\ErrorDisplayer.h"
#include ".\\ErrorDisplay\\ThreadSafeQueue\\HeaderFiles\\ThreadSafeQueue.h"

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
// FUNCTION:	CErrorDisplayer::CErrorDisplayer
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
CErrorDisplayer::CErrorDisplayer()
{
   m_bInit = FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CErrorDisplayer::~CErrorDisplayer
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
CErrorDisplayer::~CErrorDisplayer()
{
   Shutdown();
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CErrorDisplayer::Start
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
bool CErrorDisplayer::Start()
{
	TRACE("ErrorDisplayer Starting ...\n");

	ShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	// Launch Add Print Job Thread
	ThreadErrorDisplayer = (HANDLE)_beginthreadex(NULL,
		                                          0, 
												  ErrorDisplayThread,
												  this, 
												  0, 
												  &ThreadID
										          );
	if (!ThreadErrorDisplayer)
	{
		TRACE("_beginthreadex(...) failure, ErrorDisplayer::Start\n");
		return FALSE;
	}
	TRACE("ErrorDisplayer ThreadID %x ...\n", ThreadID);

	m_bInit = TRUE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CErrorDisplayer::Shutdown
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
bool CErrorDisplayer::Shutdown()
{
	if (m_bInit == FALSE)
		return FALSE;

	TRACE("ErrorDisplayer Shutting down ...\n");

	SetEvent(ShutdownEvent);

	// wait for the thread to stop
	WaitForSingleObject(ThreadErrorDisplayer, INFINITE);

	CloseHandle(ShutdownEvent);
	CloseHandle(ThreadErrorDisplayer);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CErrorDisplayer::ErrorDisplayThread
// 
// DESCRIPTION: thread function that displays error messages  
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
UINT __stdcall CErrorDisplayer::ErrorDisplayThread(LPVOID pParam)
{
	CErrorDisplayer *pServer = (CErrorDisplayer*)pParam;

	HANDLE         Handles[2];
	Handles[0] = pServer->ShutdownEvent;
	Handles[1] = g_ErrorOccurredEvent;

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
				TRACE("WaitForMultipleObjects failure at ErrorDisplayThread",
				GetLastError());
			else
				TRACE("ErrorDisplayThread is shutting Down normally...\n");

			break;
		}

		else if (EventCaused == (WAIT_OBJECT_0 + 1)) // ControlHardDiskSpace
		{
			// reset the event for further processing
			ResetEvent(g_ErrorOccurredEvent);

			_errorMessageType dTmp;
			CThreadSafeQueue<_errorMessageType>::getInstance().pop(dTmp);
			
			//char*  dZZ;
			//dZZ = new char[MAX_PATH];
			//strncpy_s(dZZ, (size_t)(MAX_PATH), dTmp._OdroidText, (size_t)(MAX_PATH));

			CString cstring(dTmp._OdroidText);
			LPCTSTR pszCharacterString = (LPCTSTR)cstring;

			
			::MessageBox(NULL,
				(LPCWSTR)pszCharacterString,
				(LPCWSTR)WARNINGWINDOW_TITLE,
				MB_OK | MB_ICONERROR | MB_TOPMOST
				);

			//delete[] dZZ;
		}
	}
	return THREADEXIT_SUCCESS;
}

//void CErrorDisplayer::GetErrorMessage(_errorMessageType* aInMess)
//{
//	_errorMessageType dTmp;
//	CThreadSafeQueue<_errorMessageType>::getInstance()->pop(dTmp);
//}