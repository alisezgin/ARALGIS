#pragma once

#include "ARALGIS.h"
#include "Constants.h"
#include "ChangeDetector.h"

class CMainFrame;

class CChangeDetectController
{
public:
	CChangeDetectController();
	virtual ~CChangeDetectController();

public:
	CMainFrame*	  m_pFrame;

	bool Shutdown();
	bool Start(CMainFrame* pFrame);

	CChangeDetector m_MyChangeDetector;

private:
	static UINT	__stdcall	ChangeDetectorThread(LPVOID);
	HANDLE			ThreadChangeDetector;
	unsigned int	ThreadID;

	static UINT	__stdcall	ChangeDetectorCommThread(LPVOID);
	HANDLE			ThreadComm;
	unsigned int	ThreadIDComm;

	bool			m_bInit;
	HANDLE			ShutdownEvent;
};

