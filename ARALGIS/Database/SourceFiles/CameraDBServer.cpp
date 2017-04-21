#include "stdafx.h"


#include "ARALGIS.h"
#include "..\HeaderFiles\CameraDBServer.h"

#include "MainFrm.h"
#include "ARALGISView.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CCameraDBServer::CCameraDBServer
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
CCameraDBServer::CCameraDBServer()
{
	bRun = FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CCameraDBServer::CCameraDBServer
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
CCameraDBServer::~CCameraDBServer()
{
	Shutdown();
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CCameraDBServer::CCameraDBServer
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
BOOL CCameraDBServer::Start(CMainFrame* pFrame)
{
	if (bRun)
	{
		TRACE("_beginthreadex(...) failure, for Launch Thread CameraDBServer::Start\n");
		return FALSE;
	}

	TRACE("Camera DB Server Thread Starting ...\n");

	m_pFrame = pFrame;

	ShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// Launch CameraDBServer Thread
	ThreadCameraDBServer = (HANDLE)_beginthreadex(  NULL,
													0,
													CameraDBServerThread,
													this,
													0,
													&ThreadID
												 );
	if (!ThreadCameraDBServer)
	{
		TRACE("_beginthreadex(...) failure, ThreadCameraDBServer::Start\n");
		return FALSE;
	}

	TRACE("ThreadCameraDBServer ThreadID %x ...\n", ThreadID);

	bRun = TRUE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CCameraDBServer::Shutdown
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
BOOL CCameraDBServer::Shutdown()
{
	if (!bRun)
		return FALSE;

	TRACE("Camera DB Server Shutting down ...\n");

	SetEvent(ShutdownEvent);

	// wait for the thread to stop
	WaitForSingleObject(ThreadCameraDBServer, INFINITE);

	CloseHandle(ShutdownEvent);
	CloseHandle(ThreadCameraDBServer);

	bRun = FALSE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CCameraDBServer::CameraDBServerThread
// 
// DESCRIPTION:	thread function for handling Database processing for car image 
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
////////////////////////////////////////////////////////////////////////////////
UINT __stdcall CCameraDBServer::CameraDBServerThread(LPVOID pParam)
{
	CCameraDBServer *pCameraDBServer = (CCameraDBServer*)pParam;

	TRACE("Camera DB Server Thread Started\n");

	HANDLE Handles[2];
	Handles[0] = pCameraDBServer->ShutdownEvent;
	Handles[1] = g_CameraDBServerPlakaDataReadyEvent;


	for (;;)
	{
		DWORD EventCaused = WaitForMultipleObjects(2,
												   Handles,
												   FALSE,
												   INFINITE);

		if (EventCaused == WAIT_FAILED || EventCaused == WAIT_OBJECT_0)
		{
			if (EventCaused == WAIT_FAILED)
				TRACE("WaitForMultipleObjects failure at CameraDBServerThread", GetLastError());
			else
				TRACE("CameraDBServerThread is shutting Down normally...\n");

			return THREADEXIT_SUCCESS;
		}

		// g_CameraDBServerPlakaDataReadyEvent is received
		else if (EventCaused == WAIT_OBJECT_0 + 1)
		{
			pCameraDBServer->SendDBMessage();
		}

	} // infite for loop

	return THREADEXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CCameraDBServer::SendDBMessage
// 
// DESCRIPTION:	Sends message to ARALGISView 
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
////////////////////////////////////////////////////////////////////////////////
void CCameraDBServer::SendDBMessage()
{
	CView * pView = m_pFrame->GetActiveView();

	if (!pView)
	{
		::MessageBox(NULL, _T("GetActiveView Not OK"), NULL, MB_OK);
	}

	// Fail if view is of wrong kind
	if (!pView->IsKindOf(RUNTIME_CLASS(CARALGISView)))
	{
		::MessageBox(NULL, _T("GetActiveView Not OK"), NULL, MB_OK);
	}

	LPARAM pLparam;
	pLparam = reinterpret_cast<LPARAM>("ARALGIS");

	pView->SendMessage(WM_DBASE_CAR_INFO_READY, 0, pLparam);
}