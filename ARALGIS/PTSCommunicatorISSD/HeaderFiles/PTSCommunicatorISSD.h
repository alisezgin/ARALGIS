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

class CPTSCommunicatorISSD 
{
public:
	CPTSCommunicatorISSD();
	virtual ~CPTSCommunicatorISSD();


public:
	CMainFrame*	  m_pFrame;

	BOOL Start(CMainFrame* pFrame);
	BOOL Shutdown();

private:				
	static UINT	__stdcall	PTSCommunicatorThread(LPVOID);								
	static UINT __stdcall   CommThread(LPVOID);

	HANDLE					ThreadPTSCommunicator; 
	HANDLE					ThreadComm; 

	unsigned int		    ThreadID;					
	HANDLE				    ShutdownEvent;
	int						scPort;
	SOCKET					scSocket;
	bool				   	bRun;
	bool					m_bIsConnectedToServer;

	void SendImageData();
	void GetImageData(BYTE* imgDataIn, int dataLenIn);
	void GetPlakaData(BYTE* plakaDataIn);
	bool SendIDMessage();

	BOOL ControlMessage(BYTE *message);

};

