#pragma once


//#include <stdlib.h>
//#include <stdio.h>


#include "ARALGIS.h"
#include "Constants.h"


#define THREADEXIT_SUCCESS	  0x1234

class CMainFrame;

class CVehicleDetection
{
public:
	CVehicleDetection();
	virtual ~CVehicleDetection();


public:
	CMainFrame*	  m_pFrame;

	BOOL Start(CMainFrame* pFrame);
	BOOL Shutdown();

private:
	static UINT	__stdcall	VehicleDetectionThread(LPVOID);
	static UINT	__stdcall	CameraInterfaceThread(LPVOID);

	HANDLE					ThreadVehicleDetection;
	HANDLE					ThreadCameraInterface;

	HANDLE					ProcessingFinishedEvent;
	HANDLE					ProcessDataEvent;


	unsigned int		    ThreadIDVD;
	unsigned int		    ThreadIDCI;


	HANDLE				    ShutdownEvent;
	bool				   	bRun;

	int						m_dFrameCounter;
	int						m_dProcessedFrameCounter;

	int						m_dlineCounter;
	bool					m_isStopSignalled;


};

