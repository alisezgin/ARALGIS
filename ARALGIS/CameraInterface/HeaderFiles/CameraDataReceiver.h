#pragma once

#include "ARALGIS.h"

#include "ReceiveCameraImage.h"



class CCameraDataReceiver
{
public:
	bool Shutdown();
	bool Start();

	CCameraDataReceiver();
	virtual ~CCameraDataReceiver();


	CReceiveCameraImage m_MyCamera;

private:
	static UINT	__stdcall	CameraDataReceiverThread(LPVOID);

	HANDLE			ThreadCameraDataReceiver;
	unsigned int	ThreadID;
	bool			m_bInit;
	HANDLE			ShutdownEvent;
	
	HANDLE			m_hTimer;
};

