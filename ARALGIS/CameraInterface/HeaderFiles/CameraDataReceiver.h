#pragma once

#include "ARALGIS.h"
#include "Constants.h"
#include "ReceiveCameraImage.h"

class CMainFrame;

class CCameraDataReceiver
{
public:
	CCameraDataReceiver();
	virtual ~CCameraDataReceiver();

public:
	CMainFrame*	  m_pFrame;

	bool Shutdown();
	bool Start(CMainFrame* pFrame);

	CReceiveCameraImage m_MyCamera;


private:
	static UINT	__stdcall	CameraDataReceiverThread(LPVOID);

	HANDLE			ThreadCameraDataReceiver;
	unsigned int	ThreadID;
	bool			m_bInit;
	HANDLE			ShutdownEvent;

	HANDLE			m_hTimer;

};

