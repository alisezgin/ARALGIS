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

#define PTS_DISPLAY_IMAGE      0x0001
#define PTS_DELETE_IMAGE       0x0002
#define PTS_CONNECTION_LOST    0x0003
#define PTS_CONNECTION_OK	   0x0004



typedef void (CALLBACK* SCNOTIFYPROC)(LPVOID, UINT nCode);

class CMainFrame;

class CPTSCommunicator 
{
public:
	CPTSCommunicator();
	virtual ~CPTSCommunicator();


public:
	SCNOTIFYPROC  m_pNotifyProc;
	CMainFrame*	  m_pFrame;

	BOOL Start(SCNOTIFYPROC pNotifyProc, CMainFrame* pFrame);
	BOOL Shutdown();

private:				
	static UINT	__stdcall	PTSCommunicatorThread(LPVOID);								
	static UINT __stdcall   CommThread(LPVOID);

	HANDLE					ThreadPTSCommunicator; 
	HANDLE					ThreadComm; 

	unsigned int		    ThreadID;					
	HANDLE				    ShutdownEvent;
	int						scPort;
	SOCKET					ListenSocket;
	SOCKET					ClientSocket;
	bool				   	bRun;
	bool					m_bClientedAccepted;

	void SendTriggerMessage();
	void GetImageData(BYTE* imgDataIn, int dataLenIn);
	void GetPlakaData(BYTE* plakaDataIn);

	BOOL ControlMessage(BYTE *message);
};

