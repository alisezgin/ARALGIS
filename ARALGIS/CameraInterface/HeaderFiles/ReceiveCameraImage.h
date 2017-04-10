#pragma once



#include "SapClassBasic.h"
#include "SapClassGui.h"
#include "afxwin.h"


#include <iostream>


#pragma once

#include "ARALGIS.h"


class CReceiveCameraImage
{
public:
	CReceiveCameraImage();
	virtual ~CReceiveCameraImage();

public:
	BOOL CreateObjects(void);
	BOOL DestroyObjects(void);
	void UpdateMenu(void);
	void UpdateFrameRate(void);
	void ComputeFrameRate(void);
	void CheckForLastFrame(void);

	static void AcqCallback(SapAcqCallbackInfo *pInfo);
	static void XferCallback(SapXferCallbackInfo *pInfo);

	float       m_FrameRate;
	float       m_MaxTime;
	float       m_MinTime;
	int         m_ActiveBuffer;
	int         m_BufferCount;
	int         m_Slider;
	CString		m_Msg;


	void StartDataReception();
	void StopDataReception();
	void PauseDataReception();
	void LoadConfiguationFile();
	void DisplayRecordSelectDialog();
	void UpdateControls();
	void OnWaitableTimer();
	int GetFrameTime();
	void SetFrameTime(int aframeTime);

	void GetCameraDataAsMat();



	// Implementation
protected:
	HICON          m_hIcon;
	CString        m_appTitle;

	SapAcqDevice		*m_AcqDevice;
	SapBuffer			*m_Buffers;
	SapTransfer			*m_Xfer;
	SapPerformance		m_FrameTimer;
	SapPerformance		m_RecordTimer;

	BOOL m_IsSignalDetected;   // TRUE if camera signal is detected

	// Button flags
	BOOL           m_bRecordOn;      // TRUE if recording
	BOOL           m_bPlayOn;        // TRUE if playing back
	BOOL           m_bPauseOn;       // TRUE if pausing

	int 		 m_nFramesPerCallback;
	int          m_nFramesLost;


	BOOL InitSAPERA(void);
	void OnDestroySAPERA(void);


	void OnBoraKillfocusFrameRate(void);

	int m_frameTime;

private:

};