#pragma once

#include <winsock2.h>
#include <process.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


#include "ARALGIS.h"
#include "Constants.h"

#define ODROID_CONNECTION_LOST      0x0001
#define ODROID_CONNECTION_OK       0x0002


#define THREADEXIT_SUCCESS	  0x1234

typedef void (CALLBACK* SCNOTIFYPROC)(LPVOID, UINT nCode);

class CMainFrame;

class COdroidCommunicator
{
public:
	COdroidCommunicator();
	~COdroidCommunicator();


public:
	SCNOTIFYPROC  m_pNotifyProc;
	CMainFrame*	  m_pFrame;

	BOOL Start(SCNOTIFYPROC pNotifyProc, CMainFrame* pFrame);
	BOOL Shutdown();

private:
	static UINT	__stdcall	OdroidCommunicatorThread(LPVOID);
	static UINT __stdcall   CommThread(LPVOID);

	HANDLE					ThreadOdroidCommunicator;
	HANDLE					ThreadComm;

	unsigned int		    ThreadID;
	HANDLE				    ShutdownEvent;
	int						scPort;
	SOCKET					ListenSocket;
	SOCKET					ClientSocket;
	bool				   	bRun;
	bool					m_bClientedAccepted;

	void SendOdroidOpenBarrierMessage();
	void SendOdroidCloseBarrierMessage();
	void SendOdroidStartHeatingMessage();
	void SendOdroidStopHeatingMessage();
	void SendOdroidStartAlarmMessage();
	void SendOdroidStopAlarmMessage();

	BOOL ControlMessage(BYTE *message);
};

