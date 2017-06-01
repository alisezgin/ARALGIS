#pragma once


//#include <stdlib.h>
//#include <stdio.h>


#include "ARALGIS.h"
#include "Constants.h"

#define THREADEXIT_SUCCESS	  0x1234

class CMainFrame;

class CImageFilterProcessing
{
public:
	CImageFilterProcessing();
	virtual ~CImageFilterProcessing();


public:
	CMainFrame*	  m_pFrame;

	BOOL Start(CMainFrame* pFrame);
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

	static UINT	__stdcall	RefFilter1ProcessingThread(LPVOID);
	static UINT	__stdcall	RefFilter2ProcessingThread(LPVOID);
	static UINT	__stdcall	RefFilter3ProcessingThread(LPVOID);

	HANDLE					ThreadRefFilter1Processing;
	HANDLE					ThreadRefFilter2Processing;
	HANDLE					ThreadRefFilter3Processing;

	unsigned int		    ThreadRefID1;
	unsigned int		    ThreadRefID2;
	unsigned int		    ThreadRefID3;

	HANDLE				    ShutdownEvent;
	bool				   	bRun;
};

