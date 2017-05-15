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

class CVehicleDetection
{
public:
	CVehicleDetection();
	virtual ~CVehicleDetection();


public:
	SCNOTIFYPROC  m_pNotifyProc;
	CMainFrame*	  m_pFrame;

	BOOL Start(SCNOTIFYPROC pNotifyProc, CMainFrame* pFrame);
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

