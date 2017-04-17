#pragma once

#include "ARALGIS.h"
#include "Constants.h"
#include "ReceiveCameraImage.h"

#define SET_TIMER_PERIOD_CAMERA      0x0001
#define KILL_TIMER_CAMERA            0x0002

typedef void (CALLBACK* SCNOTIFYPROC)(LPVOID, UINT nCode);

class CMainFrame;

class CCameraDataReceiver
{
public:
	CCameraDataReceiver();
	virtual ~CCameraDataReceiver();

public:
	SCNOTIFYPROC  m_pNotifyProc;
	CMainFrame*	  m_pFrame;

	bool Shutdown();
	bool Start(SCNOTIFYPROC pNotifyProc, CMainFrame* pFrame);

	CReceiveCameraImage m_MyCamera;


private:
	static UINT	__stdcall	CameraDataReceiverThread(LPVOID);

	HANDLE			ThreadCameraDataReceiver;
	unsigned int	ThreadID;
	bool			m_bInit;
	HANDLE			ShutdownEvent;

	HANDLE			m_hTimer;

};

