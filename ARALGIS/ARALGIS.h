
// ARALGIS.h : main header file for the ARALGIS application
//
#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


#include "windows.h"
#include "opencv2/core/core.hpp"
#include <queue>


/////////// Type Definitions start here /////////
typedef struct _PTSDataType {
	unsigned short   cameraID;
	unsigned short   colourID;
	unsigned short   modelID;
	unsigned short   speedValue;
	unsigned int     condfidenceVal;
	long long        timeStamp;
} PTSDataType;

typedef struct _errorMessageType{
	char _OdroidText[MAX_PATH];
} ERRORMESSAGETYPE;

///////// GLOBAL EVENTS start here ////////////////
///////////////////////////////////////////////////

// event used for starting camera data reception
extern HANDLE g_CameraStartDataRecieveEvent;

// event used for stopping camera data reception
extern HANDLE g_CameraStopDataRecieveEvent;

// event used for changing camera sample rate
extern HANDLE g_CameraChangeSampleRateEvent;

// event used for setting timer frame rate 
extern HANDLE g_SetTimerFrameRateEvent;

// event used for killing timer 
extern HANDLE g_KillTimerEvent;

// event used for triggering PTS 
extern HANDLE g_PTSTriggerEvent;

// event used for sending UVSS image to PTS 
extern HANDLE g_SendUVSSImageEvent;

// event used for triggering PTS 
extern HANDLE g_PTSLostEvent;

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

// event used for sending critical disk space message to odroid 
extern HANDLE g_OdroidDiskSpaceEvent;

// event used for sending file written message to odroid 
extern HANDLE g_OdroidPTSFileWrittenEvent;

// event used for sending file written message to odroid 
extern HANDLE g_OdroidImageFileWrittenEvent;

// event used for sending error occured message to odroid 
extern HANDLE g_OdroidErrorOccuredEvent;

// event used for triggering CameraDBServer
extern HANDLE g_CameraDBServerWriteFileEvent;

// event used for triggering Filter-1 processing of test image
extern HANDLE g_ProcessFilter1Event;

// event used for triggering Filter-2 processing of test image
extern HANDLE g_ProcessFilter2Event;

// event used for triggering Filter-3 processing of test image
extern HANDLE g_ProcessFilter3Event;

// event used for triggering Filter-1 processing of test image
extern HANDLE g_ProcessFilter4Event;

// event used for triggering Filter-2 processing of test image
extern HANDLE g_ProcessFilter5Event;

// event used for triggering Filter-3 processing of test image
extern HANDLE g_ProcessFilter6Event;

// event used for triggering intermediate test image ready
extern HANDLE g_IntermediateImageReadyEvent;

// event used for start change detection 
extern HANDLE g_StartChangeDetectEvent;

// event used for starting free disc space control 
extern HANDLE g_ControlHardDiskSpaceEvent;

// event used for interrupting change detection thread 
extern HANDLE g_StopChangeDetectionEvent;

// event for error message processing
extern HANDLE g_ErrorOccurredEvent;
///////////////////////////////////////////////////
////////// GLOBAL EVENTS ends here ////////////////


////////// GLOBAL CRITICAL SECTIONs start here ////////////
///////////////////////////////////////////////////////////

// Critical Section to protect intermediate test Images
extern CRITICAL_SECTION g_IntermediateTestImgCS;

// Critical Section to protect RefImage 
extern CRITICAL_SECTION   RefImageCS;

// Critical Section to protect TestImage 
extern CRITICAL_SECTION   TestImageCS;

// Critical Section to shutdown after change detection is finished
extern CRITICAL_SECTION g_ChangeDetectCS;

// Critical Section to protect intermediate test Images
extern CRITICAL_SECTION g_DBFileWriteCS;

// Critical Section to protect Error Messages Queue
extern CRITICAL_SECTION g_QueueErrorMessageCS;
///////////////////////////////////////////////////////////
////////// GLOBAL CRITICAL SECTIONs start here ////////////

//// GLOBAL DATA VARIABLES start here////////////////
/////////////////////////////////////////////////////

// global variable to hold openCV test image from Camera
extern cv::Mat g_CVImageTest;

// global variable to hold filtered #1 openCV test image 
extern cv::Mat g_CVImageTestFilter1;

// global variable to hold filtered #2 openCV test image 
extern cv::Mat g_CVImageTestFilter2;

// global variable to hold filtered #3 openCV test image 
extern cv::Mat g_CVImageTestFilter3;

// global variable to hold the image in byte array to be sent to PTS
extern BYTE* g_ByteImageTest;
extern unsigned int g_ByteImageSize;

// global variable to hold openCV reference image from Camera
extern cv::Mat g_CVImageRef;

// global variable to hold filtered #1 openCV ref image 
extern cv::Mat g_CVImageRefFilter1;

// global variable to hold filtered #2 openCV ref image 
extern cv::Mat g_CVImageRefFilter2;

// global variable to hold filtered #3 openCV ref image 
extern cv::Mat g_CVImageRefFilter3;

// global variable to hold openCV intermediate test image from Camera
extern cv::Mat g_CVImageTestIntermediate[NUM_INTERMEDIATE_TEST_IMAGE+1];

// global variable for change detection result
extern cv::Mat g_CVTestChangeDetect;

// global variable for perspective wrapped test image
extern cv::Mat g_CVTestWrapped;

// global variable to hold counter of  openCV intermediate test image from Camera
extern int g_IntermediateCounter;

// global variable to hold car PLAKA NUMBER
extern char g_PlakaNoChars[PLAKA_CHAR_LENGTH + 1];

// global variable to hold car FRONTAL/PLAKA IMAGE 
extern BYTE* g_CarPlakaImage;
extern int  g_CarPlakaImageLenght;

// global variable to hold stored image files directory path
extern char g_ImageFilesDirPath[MAX_DIR_PATH_LENGTH];

// global variable to hold test image filename (includes directory path)
//extern char g_TestImageFileName[MAX_DIR_PATH_LENGTH];

// global variable to hold reference image filename (includes directory path)
extern char g_RefImageFileName[MAX_DIR_PATH_LENGTH];

// test image indices
extern int g_dBeginIndex;
extern int g_dEndIndex;
extern int g_dCarDetectCount;

extern int  g_CameraWidth;
extern int  g_CameraHeight;
extern int  g_CameraPixelBits;
extern int  g_CameraBufferSize;
extern char g_Odroid_Port[PORT_BYTE_LEN + 1];
extern char g_PTSPort[PORT_BYTE_LEN + 1];
extern char g_PTSIP[IP_BYTE_LEN + 1];
extern char g_ReferenceFilePath[MAX_DIR_PATH_LENGTH + 1];
extern char g_CameraConfigFilename[MAX_FILENAME_LENGTH + 1];

extern bool g_ChangeDetectActive;

extern cv::Mat g_CVRefBigWindow;
extern cv::Mat g_CVTestBigWindow;

extern std::string g_TestImageFileName;
/////////////////////////////////////////////////////
//// GLOBAL DATA VARIABLES end here////////////////

////// GLOBAL CONTROL VARIABLES start here /////////////////////////
////////////////////////////////////////////////////////////////////

// variable to control whether the program is started.
// if started, during exit the database  connections are closed
// eitherwise the connections are not tried to be closed
extern BOOL g_isProgramStarted;

// something like a counting semaphore for thread sync
extern int g_carsDetectedByPTSCnt;

// defines what type of code will be used for PTS comm
extern int g_PTS_Producer_ID;

// defines operation mode of PTS (trigger/continuous)
extern int g_PTS_Mode;

// defines is automatic vehicle will be used
extern bool g_Use_Auto_VehicleDetect;

// defines if PTS is DIVIT and g_Use_Auto_VehicleDetect is TRUE
// use full camera data for vehicle detection
extern int g_AutoDetect_Type;

extern BOOL g_CarFound;

extern BOOL g_IsOdroidStartReceived;
extern BOOL g_IsAnswerReceivedFromPTS;
////////////////////////////////////////////////////////////////////
////// GLOBAL CONTROL VARIABLES end here /////////////////////////


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
