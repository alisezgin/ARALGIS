
// ARALGIS.h : main header file for the ARALGIS application
//
#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


#include "windows.h"
#include "opencv2/core/core.hpp"



// variable to control whether the program is started.
// if started, during exit the database  connections are closed
// eitherwise the connections are not tried to be closed
extern BOOL g_isProgramStarted;

// something like a counting semaphore for thread sync
extern int threadSyncCnt;

// used for calculating time using number of ticks generated by timer
extern int TimerNormalizer;

// event used for starting camera data reception
extern HANDLE g_CameraStartDataRecieveEvent;

// event used for stopping camera data reception
extern HANDLE g_CameraStopDataRecieveEvent;

// event used for changing camera sample rate
extern HANDLE g_CameraChangeSampleRateEvent;

// event used for pausing camera data reception
extern HANDLE g_CameraPauseDataRecieveEvent;

// event used for displaying camera configuration file selection dialog
extern HANDLE g_CameraConfigFileChangeEvent;

// event used for loading a recorded stream file 
extern HANDLE g_CameraSelectStreamFileEvent;

// event used for updating camera controls 
extern HANDLE g_CameraUpdateControlsEvent;

// event used for displaying BITMAP Window 
extern HANDLE g_DisplayBitmapEvent;

// event used for displaying OpenCV Window 
extern HANDLE g_DisplayOpenCVEvent;

// event used for setting timer frame rate 
extern HANDLE g_SetTimerFrameRateEvent;

// event used for killing timer 
extern HANDLE g_KillTimerEvent;

// event used for resetting timer 
extern HANDLE g_ResetTimerEvent;

// event used for resetting timer 
//extern HANDLE g_CameraDataReadyEvent;

// event used for updating camera data tith timer tick
extern HANDLE g_mCameraTimerEvent;

extern int g_iTimerPeriod;



// event used for sending open barrier message to odroid 
extern HANDLE g_OdroidOpenBarrierEvent;

// event used for sending close barrier message to odroid  
extern HANDLE g_OdroidCloseBarrierEvent;

// event used for sending start heating message to odroid 
extern HANDLE g_OdroidStartHeatingEvent;

// event used for sending stop heating message to odroid 
extern HANDLE g_OdroidStopHeatingEvent;

// event used for sending start alarm message to odroid 
extern HANDLE g_OdroidStartAlarmEvent;

// event used for sending stop alarm message to odroid 
extern HANDLE g_OdroidStopAlarmEvent;

// global variable to hold CV image from Camera
extern cv::Mat g_CVImageTest;

// event used for triggering PTS 
extern HANDLE g_PTSTriggerEvent;

// event used for triggering PTS 
extern HANDLE g_PTSLostEvent;

// PTS IP
extern char g_PtsIP[IP_ADDRESS_LEN + 1];

// PLAKA NUMBER
extern char g_PlakaNoChars[PLAKA_CHAR_LENGTH+1];

// CAR FRONTAL/PLAKA IMAGE 
extern BYTE* g_CarPlakaImage;
extern int m_CarPlakaImageLenght;

// Critical Section to protect isDatabaseHandlingInProgress
extern CRITICAL_SECTION g_SomeHandlingCS;


// CARALGISApp:
// See ARALGIS.cpp for the implementation of this class
//

class CARALGISApp : public CWinApp
{
public:
	CARALGISApp();


	// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CARALGISApp theApp;
