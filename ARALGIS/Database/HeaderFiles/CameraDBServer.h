#pragma once

#include "ARALGIS.h"
#include "Constants.h"

#define THREADEXIT_SUCCESS	  0x1234

class CMainFrame;

class CCameraDBServer
{
public:
	CCameraDBServer();
	virtual ~CCameraDBServer();


public:
	CMainFrame*	  m_pFrame;


	BOOL Start(CMainFrame* pFrame);
	BOOL Shutdown();

private:
	static UINT	__stdcall	CameraDBServerThread(LPVOID);

	HANDLE					ThreadCameraDBServer;

	unsigned int		    ThreadID;
	HANDLE				    ShutdownEvent;
	bool				   	bRun;

	void  SendDBMessage();
};

