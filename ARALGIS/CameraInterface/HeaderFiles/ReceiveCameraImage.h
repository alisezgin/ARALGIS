#pragma once

#include "SapClassBasic.h"
#include "SapClassGUI.h"
#include "afxwin.h"
#include <iostream>
#include "ARALGIS.h"
#include "ARALGISView.h"


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

	void StartDataReception();
	void StopDataReception();
	//void PauseDataReception();
	//void LoadConfiguationFile();
	//void DisplayRecordSelectDialog();
	void UpdateControls();
	void OnWaitableTimer();
	void OnTimerCamera();
	int  GetFrameTime();
	void SetFrameTime(int aframeTime);

	void GetCameraDataAsMat();
	void GetCameraIntermediateDataAsMat();

	// Implementation
protected:
	SapAcqDevice		*m_AcqDevice;
	SapBuffer			*m_Buffers;
	SapTransfer			*m_Xfer;
	SapPerformance		m_FrameTimer;
	SapPerformance		m_RecordTimer;

	// Button flags
	BOOL           m_bRecordOn;      // TRUE if recording
	BOOL           m_bPlayOn;        // TRUE if playing back
	//BOOL           m_bPauseOn;       // TRUE if pausing
	int			   m_nFramesPerCallback;
	int            m_nFramesLost;
	int            m_frameTime;

	BOOL InitSAPERA(void);
	void OnDestroySAPERA(void);
	void OnBoraKillfocusFrameRate(void);

	CView * m_pView;
	LPARAM  m_pLparam;

	BOOL m_bServerAvailable;
};