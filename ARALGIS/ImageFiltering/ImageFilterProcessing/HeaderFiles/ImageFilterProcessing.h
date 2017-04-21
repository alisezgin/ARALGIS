#pragma once


//#include <stdlib.h>
//#include <stdio.h>


#include "ARALGIS.h"
#include "Constants.h"


#define THREADEXIT_SUCCESS	  0x1234

#define FILTER_PROCESS_FILTER1_READY    0x0001
#define FILTER_PROCESS_FILTER2_READY    0x0002
#define FILTER_PROCESS_FILTER3_READY    0x0003

typedef void (CALLBACK* SCNOTIFYPROC)(LPVOID, UINT nCode);

class CMainFrame;

class CImageFilterProcessing
{
public:
	CImageFilterProcessing();
	virtual ~CImageFilterProcessing();


public:
	SCNOTIFYPROC  m_pNotifyProc;
	CMainFrame*	  m_pFrame;

	BOOL Start(SCNOTIFYPROC pNotifyProc, CMainFrame* pFrame);
	BOOL Shutdown();

private:
	static UINT	__stdcall	Filter1ProcessingThread(LPVOID);
	static UINT	__stdcall	Filter2ProcessingThread(LPVOID);
	static UINT	__stdcall	Filter3ProcessingThread(LPVOID);

	HANDLE					ThreadFilter1Processing;
	HANDLE					ThreadFilter2Processing;
	HANDLE					ThreadFilter3Processing;

	unsigned int		    ThreadID1;
	unsigned int		    ThreadID2;
	unsigned int		    ThreadID3;

	HANDLE				    ShutdownEvent;
	bool				   	bRun;
};

