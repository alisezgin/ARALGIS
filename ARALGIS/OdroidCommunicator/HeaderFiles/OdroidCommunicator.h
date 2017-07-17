#pragma once

#include <winsock2.h>
#include <process.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


#include "ARALGIS.h"
#include "Constants.h"




#define THREADEXIT_SUCCESS	  0x1234

class CMainFrame;

class COdroidCommunicator
{
public:
	COdroidCommunicator();
	~COdroidCommunicator();


public:
	CMainFrame*	  m_pFrame;

	BOOL Start(CMainFrame* pFrame);
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

	int						m_iTimerTick;
	HANDLE					m_hTimer;

	void SendOdroidOpenBarrierMessage();
	void SendOdroidCloseBarrierMessage();
	void SendOdroidStartHeatingMessage();
	void SendOdroidStopHeatingMessage();
	void SendOdroidStartAlarmMessage();
	void SendOdroidStopAlarmMessage();

	BOOL ControlMessage(BYTE *message);
};

