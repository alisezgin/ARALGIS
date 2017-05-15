#include "stdafx.h"

#include "..\\HeaderFiles\\ReceiveCameraImage.h"
#include "float.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"


CReceiveCameraImage::CReceiveCameraImage()
{
	m_FrameRate = 0.0f;
	m_MaxTime = 0.0f;
	m_MinTime = 0.0f;
	m_ActiveBuffer = 0;
	m_BufferCount = 0;

	m_AcqDevice = NULL;
	m_Buffers = NULL;
	m_Xfer = NULL;

	m_bRecordOn = FALSE;
	m_bPlayOn = FALSE;
	//m_bPauseOn = FALSE;

	m_nFramesPerCallback = 1;
	m_nFramesLost = 0;

	m_frameTime = 0;

	m_bServerAvailable = FALSE;

	InitSAPERA();

	CFrameWnd *pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
	if (!pFrame)
	{
		::MessageBox(NULL,
			(LPCWSTR)L"CReceiveCameraImage::FrameNotFound",
			(LPCWSTR)WARNINGWINDOW_TITLE,
			MB_OK | MB_ICONERROR
			);
	}


	m_pView = pFrame->GetActiveView();
	if (!m_pView)
	{
		::MessageBox(NULL,
			(LPCWSTR)L"CReceiveCameraImage::GetActiveView",
			(LPCWSTR)WARNINGWINDOW_TITLE,
			MB_OK | MB_ICONERROR
			);
	}

	// Fail if view is of wrong kind
	if (!m_pView->IsKindOf(RUNTIME_CLASS(CARALGISView)))
	{
		::MessageBox(NULL,
			(LPCWSTR)L"CReceiveCameraImage::GetActiveView NOK",
			(LPCWSTR)WARNINGWINDOW_TITLE,
			MB_OK | MB_ICONERROR
			);
	}

	m_pLparam = reinterpret_cast<LPARAM>("ARALGIS");

}


CReceiveCameraImage::~CReceiveCameraImage()
{
	OnDestroySAPERA();
}




//==============================================================================
// Name      : CGigESeqGrabDemoDlg::AcqCallback
// Purpose   : Callback function for the acquisition.
// Parameters: None
//==============================================================================
void CReceiveCameraImage::AcqCallback(SapAcqCallbackInfo *pInfo)
{
	CReceiveCameraImage *pDlg = (CReceiveCameraImage *)pInfo->GetContext();

	pDlg->m_nFramesLost++;

	// Refresh controls
	//SetEvent(g_CameraUpdateControlsEvent);
}

//==============================================================================
// Name      : CGigESeqGrabDemoDlg::XferCallback
// Purpose   : Callback function for the transfer.
// Parameters: None
//==============================================================================
void CReceiveCameraImage::XferCallback(SapXferCallbackInfo *pInfo)
{
	CReceiveCameraImage *pDlg = (CReceiveCameraImage *)pInfo->GetContext();

	// Measure real frame time
	pDlg->UpdateFrameRate();

	// Check if last frame is reached
	pDlg->CheckForLastFrame();

	if (((pDlg->m_Buffers->GetIndex() + 1) % pDlg->m_nFramesPerCallback) == 0 ||
		pDlg->m_Buffers->GetIndex() == pDlg->m_Buffers->GetCount() - 1)
	{
		// Refresh controls
		//SetEvent(g_CameraUpdateControlsEvent);
	}
} // End of the CGigESeqGrabDemoDlg::XferCallback method.


//==============================================================================
// Name      : CReceiveCameraImage::InitSAPERA
// Purpose   : SAPERA initialization.
// Parameters: None
//==============================================================================
BOOL CReceiveCameraImage::InitSAPERA(void)
{
	SapManager::Open();

	int serverCount = SapManager::GetServerCount();

	if (serverCount == 0)
	{
		::MessageBox(NULL,
			(LPCWSTR)L"Baðlý Kamera Bulunamadý",
			(LPCWSTR)WARNINGWINDOW_TITLE,
			MB_OK | MB_ICONERROR
			);

		//return FALSE;
	}

	BOOL serverFound = FALSE;
	char serverName[CORSERVER_MAX_STRLEN];

	for (int serverIndex = 0; serverIndex < serverCount; serverIndex++)
	{
		if (SapManager::GetResourceCount(serverIndex, SapManager::ResourceAcqDevice) != 0)
			serverFound = TRUE;
	}

	bool dretVal = true;
	if (serverFound == TRUE)
	{

		for (int serverIndex = 0; serverIndex < serverCount; serverIndex++)
		{
			if (SapManager::GetResourceCount(serverIndex, SapManager::ResourceAcqDevice) != 0)
			{
				// Get Server Name Value
				SapManager::GetServerName(serverIndex, serverName, sizeof(serverName));

				m_AcqDevice = new SapAcqDevice(serverName);

				//CString configFile(_T("E:\\FuzyonSoft\\CamFiles\\default_withGain.ccf"));

				CString configFile(g_ConfigFilename);

				char dummy[1000];
				size_t i = configFile.GetLength();
				strncpy_s(dummy, "\n", 1000);
				strncpy_s(dummy, CStringA(configFile).GetBuffer(), i);

				BOOL retVal = FALSE;
				retVal = m_AcqDevice->SetConfigFile(dummy);
				if (retVal)
				{
					m_bServerAvailable = TRUE;
				}
			}
		}

		m_Buffers = new SapBufferWithTrash(g_CameraBufferSize, m_AcqDevice);
		m_Xfer = new SapAcqDeviceToBuf(m_AcqDevice, m_Buffers, XferCallback, this);
	}
	


	if (m_bServerAvailable == FALSE)
	{
		// Define off-line objects
		m_Buffers = new SapBuffer(g_CameraBufferSize, CAM_SIZE, g_CameraHeight, SapFormatRGB888, SapBuffer::TypeScatterGather); // MAX_BUFFER

	} // End if, else.

	// Create all objects
	if (!CreateObjects())
	{
		//EndDialog(TRUE); //boraN commentted out
		return FALSE;
	} // End if.

	return TRUE;  // return TRUE  unless you set the focus to a control
} // End of the CGigESeqGrabDemoDlg::OnInitDialog method.

//==============================================================================
// Name      : CReceiveCameraImage::CreateObjects
// Purpose   : Create sapera objects.
// Parameters: None
//==============================================================================
BOOL CReceiveCameraImage::CreateObjects(void)
{
	//Number of frames per callback retreived
	int nFramesPerCallback;

	// Create acquisition object
	if (m_AcqDevice && !*m_AcqDevice && !m_AcqDevice->Create())
	{
		DestroyObjects();
		return FALSE;
	}

	// Create buffer object
	if (m_Buffers && !*m_Buffers)
	{
		if (!m_Buffers->Create())
		{
			DestroyObjects();
			return FALSE;
		}
		// Clear all buffers
		m_Buffers->Clear();
	}

	if (m_Xfer && !*m_Xfer)
	{

		// Set number of frames per callback
		m_Xfer->GetPair(0)->SetFramesPerCallback(m_nFramesPerCallback);


		// If there is a large number of buffers, temporarily boost the command timeout value,
		// since the call to Create may take a long time to complete.
		// As a safe rule of thumb, use 100 milliseconds per buffer.
		int oldCommandTimeout = SapManager::GetCommandTimeout();
		int newCommandTimeout = 100 * m_Buffers->GetCount();

		if (newCommandTimeout < oldCommandTimeout)
			newCommandTimeout = oldCommandTimeout;

		SapManager::SetCommandTimeout(newCommandTimeout);

		// Create transfer object
		if (!m_Xfer->Create())
		{
			DestroyObjects();
			return FALSE;
		}

		// Restore original command timeout value
		SapManager::SetCommandTimeout(oldCommandTimeout);

		m_Xfer->Init(TRUE); // initialize tranfer object and reset source/destination index

		// Retrieve number of frames per callback
		// It may be less than what we have asked for.
		nFramesPerCallback = m_Xfer->GetPair(0)->GetFramesPerCallback();
		if (m_nFramesPerCallback > nFramesPerCallback)
		{
			m_nFramesPerCallback = nFramesPerCallback;
			AfxMessageBox(_T("No memory"));
		}

	}

	return TRUE;
} // End of the CReceiveCameraImage::CreateObjects method.

//==============================================================================
// Name      : CReceiveCameraImage::DestryObjects
// Purpose   : Destroy sapera objects.
// Parameters: None
//==============================================================================
BOOL CReceiveCameraImage::DestroyObjects(void)
{
	// Destroy transfer object
	if (m_Xfer && *m_Xfer)
	{
		m_Xfer->Destroy();
	}

	// Destroy buffer object
	if (m_Buffers && *m_Buffers)
	{
		m_Buffers->Destroy();
	}

	// Destroy acquisition object
	if (m_AcqDevice && *m_AcqDevice)
	{
		m_AcqDevice->Destroy();
	}

	return TRUE;
} // End of the CGigESeqGrabDemoDlg::DestryObjects method.


//==============================================================================
// Name      : CReceiveCameraImage::OnDestroySAPERA
// Purpose   : Handle the destroy message.
// Parameters: None
//==============================================================================
void CReceiveCameraImage::OnDestroySAPERA(void)
{

	// Destroy all objects
	DestroyObjects();

	// Delete all objects
	if (m_Xfer)
	{
		delete m_Xfer;
	}

	if (m_Buffers)
	{
		delete m_Buffers;
	}

	if (m_AcqDevice)
	{
		delete m_AcqDevice;
	}

	SapManager::Close();

} // End of the CGigESeqGrabDemoDlg::OnDestroySAPERA method.



//==============================================================================
// Name      : CReceiveCameraImage::OnKillfocusFrameRate
// Purpose   : Handle the
// Parameters: None
//==============================================================================
void CReceiveCameraImage::OnBoraKillfocusFrameRate(void)
{
	m_Buffers->SetFrameRate(m_FrameRate);
} // End of the CReceiveCameraImage::OnKillfocusFrameRate method.


//==============================================================================
// Name      : CReceiveCameraImage::UpdateFrameRate
// Purpose   : Calculate statistics on frame rate.
// Parameters: None
//==============================================================================
void CReceiveCameraImage::UpdateFrameRate(void)
{
	if (m_Buffers->GetIndex() > 0)
	{
		if ((m_Buffers->GetIndex() % m_nFramesPerCallback) != 0 ||
			m_Buffers->GetIndex() + m_nFramesPerCallback > m_Buffers->GetCount()) return;

		UINT32 counterUnit = UINT_MAX;
		BOOL isAvailable;

		//Genie
		m_AcqDevice->IsFeatureAvailable("TimestampCounter", &isAvailable);
		if (isAvailable)
		{
			m_AcqDevice->GetFeatureValue("TimestampCounter", &counterUnit);
		}
		else  //DFNC
		{
			m_AcqDevice->IsFeatureAvailable("timestampSource", &isAvailable);
			if (isAvailable)
			{
				m_AcqDevice->GetFeatureValue("timestampSource", &counterUnit);
			}
			else  //SFNC
			{
				m_AcqDevice->IsFeatureAvailable("GevTimestampValue", &isAvailable);
				if (isAvailable)
				{
					counterUnit = 0;
				}
			}
		}

		//get tick frequency
		UINT32 tickFrequency = 0;
		m_AcqDevice->IsFeatureAvailable("timestampTickFrequency", &isAvailable);
		if (isAvailable)
		{
			m_AcqDevice->GetFeatureValue("timestampTickFrequency", &tickFrequency);
		}
		else
		{
			m_AcqDevice->IsFeatureAvailable("GevTimestampTickFrequency", &isAvailable);
			if (isAvailable)
			{
				m_AcqDevice->GetFeatureValue("GevTimestampTickFrequency", &tickFrequency);
			}
			else
			{
				tickFrequency = 1000000;
			}
		}

		// A value of 0 means the counter unit is the camera's internal clock (1us resolution)
		// another possible unit is "trigger count" and is useless for frame rate calculation.
		// Warning: This value is valid for Genie cameras but isn't garanteed to stay the same
		// on other models.
		if (counterUnit == 0)
		{
			UINT32 counterStamp[2];

			// Calculate average frame rate
			m_Buffers->GetParameter(m_nFramesPerCallback - 1, CORBUFFER_PRM_COUNTER_STAMP, &counterStamp[0]);
			m_Buffers->GetParameter(m_Buffers->GetIndex(), CORBUFFER_PRM_COUNTER_STAMP, &counterStamp[1]);

			float timePerFrame = (float)(counterStamp[1] - counterStamp[0]);

			// Divide by the number of frames
			timePerFrame /= (float)(m_Buffers->GetIndex());

			m_Buffers->SetFrameRate(timePerFrame != 0 ? (float)tickFrequency / timePerFrame : 1);

			// Calculate minimum and maximum frame time interval
			// Only use buffers for which a callback has been registered.
			m_Buffers->GetParameter(m_Buffers->GetIndex() - m_nFramesPerCallback, CORBUFFER_PRM_COUNTER_STAMP, &counterStamp[0]);
			//m_Buffers->GetParameter( m_Buffers->GetIndex(), CORBUFFER_PRM_COUNTER_STAMP, &counterStamp[ 1]);

			// Calculate minimum and maximum frame time
			float diff = (counterStamp[1] - counterStamp[0]) / 1000.0f;		// frame time in milliseconcds
			diff /= m_nFramesPerCallback;
			if (diff < m_MinTime || m_MinTime == 0) m_MinTime = diff;
			if (diff > m_MaxTime) m_MaxTime = diff;
		}

		// For all other cases (including other GigE cameras), use the Sapera LT timer
		else
		{
			// Calculate average frame rate
			float timePerFrame = m_RecordTimer.GetTimeMicro(FALSE);
			timePerFrame /= (float)m_Buffers->GetIndex();
			m_Buffers->SetFrameRate(timePerFrame != 0 ? (float)tickFrequency / timePerFrame : 1);

			// Calculate minimum and maximum frame time
			float diff = m_FrameTimer.GetTimeMilli(TRUE) / m_nFramesPerCallback;
			if (diff < m_MinTime) m_MinTime = diff;
			if (diff > m_MaxTime) m_MaxTime = diff;
		}
	}
	else
	{
		// Reset timers
		m_FrameTimer.Reset();
		m_RecordTimer.Reset();
		m_MinTime = 0;
		m_MaxTime = 0;
	} // End if, else.
} // End of the CReceiveCameraImage::UpdateFrameRate method.

//==============================================================================
// Name      : CReceiveCameraImage::ComputeFrameRate
// Purpose   : Compute statistics on frame rate using counter stamp.
// Parameters: None
//========================================================================= a=====
void CReceiveCameraImage::ComputeFrameRate()
{
	UINT32 counterStamp[2];
	float  MinTime;
	float  MaxTime;

	// Must have at least 2 buffers
	if (m_Buffers->GetCount() == 1)
	{
		m_MinTime = 0;
		m_MaxTime = 0;
		return;
	}

	// Search for minimum and maximum frame time
	MinTime = FLT_MAX;
	MaxTime = -FLT_MAX;

	UINT32 counterUnit = UINT_MAX;
	BOOL isAvailable;

	//Genie
	m_AcqDevice->IsFeatureAvailable("TimestampCounter", &isAvailable);
	if (isAvailable)
	{
		m_AcqDevice->GetFeatureValue("TimestampCounter", &counterUnit);
	}
	else  //DFNC
	{
		m_AcqDevice->IsFeatureAvailable("timestampSource", &isAvailable);
		if (isAvailable)
		{
			m_AcqDevice->GetFeatureValue("timestampSource", &counterUnit);
		}
		else  //SFNC
		{
			m_AcqDevice->IsFeatureAvailable("GevTimestampValue", &isAvailable);
			if (isAvailable)
			{
				counterUnit = 0;
			}
		}
	}


	//get tick frequency
	UINT32 tickFrequency = 0;
	m_AcqDevice->IsFeatureAvailable("timestampTickFrequency", &isAvailable);
	if (isAvailable)
	{
		m_AcqDevice->GetFeatureValue("timestampTickFrequency", &tickFrequency);
	}
	else
	{
		m_AcqDevice->IsFeatureAvailable("GevTimestampTickFrequency", &isAvailable);
		if (isAvailable)
		{
			m_AcqDevice->GetFeatureValue("GevTimestampTickFrequency", &tickFrequency);
		}
		else
		{
			tickFrequency = 1000000;
		}
	}



	// A value of 0 means the counter unit is the camera's internal clock (1us resolution)
	// another possible unit is "trigger count" and is useless for frame rate calculation.
	// Warning: This value is valid for Genie cameras but isn't garanteed to stay the same
	// on other models.   
	if (counterUnit == 0)
	{
		// Only use buffers for which a callback has been registered.
		for (int i = m_nFramesPerCallback - 1; i < m_Buffers->GetCount() - m_nFramesPerCallback; i += m_nFramesPerCallback)
		{
			m_Buffers->GetParameter(i, CORBUFFER_PRM_COUNTER_STAMP, &counterStamp[0]);
			m_Buffers->GetParameter(i + m_nFramesPerCallback, CORBUFFER_PRM_COUNTER_STAMP, &counterStamp[1]);

			float diff = (counterStamp[1] - counterStamp[0]) / 1000.0f;		// frame time in milliseconcds
			diff /= m_nFramesPerCallback;
			if (diff < MinTime) MinTime = diff;
			if (diff > MaxTime) MaxTime = diff;
		}

		// Check minimum and maximum frame time
		// If both are zeroes, this means that this feature is not implemented in the transfer module
		// located on the current frame grabber
		if (MinTime == 0.0f && MaxTime == 0.0f)
			return;

		// Save minimum, maximum frame time
		m_MinTime = MinTime;
		m_MaxTime = MaxTime;

		// Compute frame rate
		// Get first and last counter stamp for buffers on which a callback has been registered.
		m_Buffers->GetParameter(m_nFramesPerCallback - 1, CORBUFFER_PRM_COUNTER_STAMP, &counterStamp[0]);
		m_Buffers->GetParameter(m_Buffers->GetCount() - 1, CORBUFFER_PRM_COUNTER_STAMP, &counterStamp[1]);

		// Compute total time 
		float timePerFrame = (float)(counterStamp[1] - counterStamp[0]);

		// Divide by the number of frames
		timePerFrame /= (float)(m_Buffers->GetCount() - m_nFramesPerCallback);

		// Save computed frame rate
		m_Buffers->SetFrameRate(timePerFrame != 0 ? (float)tickFrequency / timePerFrame : 1);
	}
}

//==============================================================================
// Name      : CReceiveCameraImage::CheckForLastFrame
// Purpose   : Check if the last frame needed has been acquired.
// Parameters: None
//==============================================================================
void CReceiveCameraImage::CheckForLastFrame(void)
{
	// Check for last frame
	int AA = m_Buffers->GetIndex();
	int BB = m_Buffers->GetCount();

	EnterCriticalSection(&g_IntermediateTestImgCS);
	GetCameraIntermediateDataAsMat();
	LeaveCriticalSection(&g_IntermediateTestImgCS);


	if (m_Buffers->GetIndex() == m_Buffers->GetCount() - 1)
	{
		if (m_bRecordOn)
		{
			//m_bRecordOn = FALSE;
			ComputeFrameRate();
			SetEvent(g_CameraStopDataRecieveEvent);  
		}

		if (m_bPlayOn)
		{
			//m_bPlayOn = FALSE;
			SetEvent(g_CameraStopDataRecieveEvent); 
		} // End if.

		//SetEvent(g_CameraUpdateControlsEvent);

	} // End if. 
} // End of the CReceiveCameraImage::CheckForLastFrame method.

//==============================================================================
// Name      : CReceiveCameraImage::OnTimer
// Purpose   :
// Parameters:
//    nIDEvent
//==============================================================================
void CReceiveCameraImage::OnWaitableTimer()
{
	// Increase buffer index
	m_Buffers->Next();

	// Check if last frame is reached
	CheckForLastFrame();

	// Refresh controls
	//SetEvent(g_CameraUpdateControlsEvent);

} // End of the CReceiveCameraImage::OnTimer method.

//==============================================================================
// Name      : CReceiveCameraImage::OnTimer
// Purpose   :
// Parameters:
//    nIDEvent
//==============================================================================
void CReceiveCameraImage::OnTimerCamera()
{
	// Increase buffer index
	m_Buffers->Next();

	// Check if last frame is reached
	CheckForLastFrame();

	// Refresh controls
	//SetEvent(g_CameraUpdateControlsEvent);

} // End of the CReceiveCameraImage::OnTimer method.

//==============================================================================
// Name      : CReceiveCameraImage::UpdateControls
// Purpose   : Update the control values.
// Parameters: None
//==============================================================================
void CReceiveCameraImage::UpdateControls()
{
	// Update edit controls
	m_ActiveBuffer = m_Buffers->GetIndex() + 1;
	m_BufferCount = m_Buffers->GetCount();
	m_FrameRate = m_Buffers->GetFrameRate();

	if (m_nFramesLost != 0)
	{
		TRACE(_T("Number of frames lost: %d"), m_nFramesLost);
	}
} // End of the CGigESeqGrabDemoDlg::UpdateControls method.

//==============================================================================
// Name      : CReceiveCameraImage::OnPlay
// Purpose   : Play back the frames.
// Parameters: None
//==============================================================================
void CReceiveCameraImage::StartDataReception(void)
{
	//////// boraN very important //////
	/////////////////////// 
	// ???????????????
	// ??????????????
	// ?????????

	// these lines are for online operation
	// starting from here
	m_nFramesLost = 0;

	if (m_bServerAvailable == TRUE)
	{
		// Acquire all frames
		m_Xfer->Init();

		if (m_Xfer->Snap(m_Buffers->GetCount()))
		{
			m_bRecordOn = TRUE;
		}
		m_Buffers->SetIndex(0);
	}
	// online operation upto here
	else
	{
		///// delete these lines 
		CLoadSaveDlg dlg(NULL, m_Buffers, TRUE, TRUE);
		if (dlg.DoModal() == IDOK)
		{
			m_bPlayOn = TRUE;
		}
	}
	/////////////  delete these lines
	
	// Initialize buffer index
	

	//m_Buffers->ResetIndex();

	int AA = m_Buffers->GetIndex();
	int BB = m_Buffers->GetCount();

	// Start playback timer
	//int frameTime = (int)(1000.0 / m_Buffers->GetFrameRate());
	m_frameTime = (int)(1000.0 / m_Buffers->GetFrameRate());

	if (m_bServerAvailable == FALSE)
	{
		SetEvent(g_SetTimerFrameRateEvent);
	}


	//SetEvent(g_CameraUpdateControlsEvent);
} // End of the CReceiveCameraImage::OnPlay method.

////==============================================================================
//// Name      : CReceiveCameraImage::OnPause
//// Purpose   : Pause the recording or playing of frames.
//// Parameters: None
////==============================================================================
//void CReceiveCameraImage::PauseDataReception(void)
//{
//	if (!m_bPauseOn)
//	{
//		// Check if recording or playing
//		if (m_bRecordOn)
//		{
//			//KillTimer(1);
//			SetEvent(g_KillTimerEvent);
//
//			// Stop current acquisition
//			if (!m_Xfer->Freeze())
//				return;
//
//			if (CAbortDlg(AfxGetApp()->m_pMainWnd, m_Xfer).DoModal() != IDOK)
//				m_Xfer->Abort();
//		}
//		else if (m_bPlayOn)
//		{
//			// Stop playback timer
//			//KillTimer(1);
//
//			SetEvent(g_KillTimerEvent);
//		} // End if, else if.
//	}
//	else
//	{
//		// Check if recording or playing
//		if (m_bRecordOn)
//		{
//			//int frameTime = (int)(1000.0 / m_Buffers->GetFrameRate());
//			m_frameTime = (int)(1000.0 / m_Buffers->GetFrameRate());
//
//			//SetTimer(1, frameTime, NULL);
//			SetEvent(g_SetTimerFrameRateEvent);
//
//			// Acquire remaining frames
//			if (!m_Xfer->Snap(m_Buffers->GetCount() - m_Buffers->GetIndex() - 1))
//				return;
//		}
//		else if (m_bPlayOn)
//		{
//			// Restart playback timer
//			//int frameTime = (int)(1000.0 / m_Buffers->GetFrameRate());
//			m_frameTime = (int)(1000.0 / m_Buffers->GetFrameRate());
//
//			//SetTimer(1, frameTime, NULL);
//			SetEvent(g_SetTimerFrameRateEvent);
//
//		} // End if, else if.
//	} // End if, else.
//
//	m_bPauseOn = !m_bPauseOn;
//
//	//SetEvent(g_CameraUpdateControlsEvent);
//} // End of the CReceiveCameraImage::OnPause method.

//==============================================================================
// Name      : CReceiveCameraImage::OnStop
// Purpose   : Stop the recording or playing of frames.
// Parameters: None
//==============================================================================
void CReceiveCameraImage::StopDataReception(void)
{
	// Check if recording or playing
	if (m_bRecordOn)
	{
		// Stop current acquisition
		if (!m_Xfer->Freeze())
			return;

		//if (CAbortDlg(AfxGetApp()->m_pMainWnd, m_Xfer).DoModal() != IDOK) /// buraya SAP help'ten bak ve gerekli düzenlemeyi yap
			m_Xfer->Abort();

		//SetEvent(g_KillTimerEvent);

		m_bRecordOn = FALSE;
	}
	else if (m_bPlayOn)
	{
		// Stop playback timer
		//SetEvent(g_KillTimerEvent);

		m_bPlayOn = FALSE;
	} // End if, else if.

	//m_bPauseOn = FALSE;

	//SetEvent(g_CameraUpdateControlsEvent);
} // End of the CReceiveCameraImage::OnStop method.

//*****************************************************************************************
//
//             General Options
//
//*****************************************************************************************
////==============================================================================
//// Name      : CReceiveCameraImage::OnLoadAcqConfig
//// Purpose   : Select a new configuration file for the acquisition.
//// Parameters: None
////==============================================================================
//void CReceiveCameraImage::LoadConfiguationFile(void)
//{
//	// Set acquisition parameters
//	CAcqConfigDlg dlg(NULL, CAcqConfigDlg::ServerAcqDevice); // boraN changed this --> NULL
//	if (dlg.DoModal() == IDOK)
//	{
//		// Destroy objects
//		DestroyObjects();
//
//		// Backup
//		SapLocation loc = m_AcqDevice->GetLocation();
//
//		// Update object
//		m_AcqDevice->SetLocation(dlg.GetLocation());
//
//		//CString configFile = dlg.GetConfigFile();
//
//		CString configFile;
//		dlg.GetConfigFile(configFile);
//
//
//		char dummy[1000];
//		size_t i = configFile.GetLength();
//		strncpy_s(dummy, "\n", 1000);
//		strncpy_s(dummy, CStringA(configFile).GetBuffer(), i);
//		m_AcqDevice->SetConfigFile(dummy);
//
//		// Recreate objects
//		if (!CreateObjects())
//		{
//			m_AcqDevice->SetLocation(loc);
//			m_AcqDevice->SetConfigFile(dummy);
//			CreateObjects();
//		} // End if.
//
//		SetEvent(g_CameraUpdateControlsEvent);
//	} // End if, else.
//} // End of the CReceiveCameraImage::OnLoadAcqConfig method.

//*****************************************************************************************
//
//             File Options
//
//*****************************************************************************************


//void CReceiveCameraImage::DisplayRecordSelectDialog()
//{
//	if (m_Buffers->GetFormat() == SapFormatMono16)
//	{
//		::MessageBox(NULL, _T("Sequence images in AVI format are sampled at 8-bit pixel depth.\nYou cannot load a sequence in the current configuration."), NULL, MB_OK);
//
//		return;
//	}
//
//	if ((m_Buffers->GetFormat() == SapFormatRGBR888))
//	{
//		::MessageBox(NULL, _T("Sequence images acquired in RGBR888 format (red first) were saved as RGB888 (blue first).\nYou cannot load a sequence in the current configuration."), NULL, MB_OK);
//
//		return;
//	}
//
//	CLoadSaveDlg dlg(NULL, m_Buffers, TRUE, TRUE);
//	if (dlg.DoModal() == IDOK)
//	{
//		SetEvent(g_CameraUpdateControlsEvent);
//	} // End if.  SapFormatRGB888
//}

int CReceiveCameraImage::GetFrameTime()
{
	return m_frameTime;
}

void CReceiveCameraImage::SetFrameTime(int aframeTime)
{
	m_frameTime = aframeTime;
}


void CReceiveCameraImage::GetCameraDataAsMat()
{
	int iWidth = m_Buffers->GetWidth();
	int iHeight = m_Buffers->GetHeight();

	int size = iWidth * iHeight;

	int bytesPerPixel = m_Buffers->GetBytesPerPixel();

	int i = 0;
	int j = 0;
	int k = 0;
	int inumFrames = 0;
	int bufOffset = 0;
	BOOL success = FALSE;


	int nNumBuffers = m_Buffers->GetCount();

	inumFrames = m_Buffers->GetIndex();


	if (inumFrames > 0)
	{
		BYTE *pData = new BYTE[size * bytesPerPixel  * (inumFrames+1)];
		if (pData == NULL)
		{
			::MessageBox(NULL, _T("Can not allocate memory in GetCameraDataAsMat"), NULL, MB_OK);
		}


		TRACE("GetCameraDataAsMat bufIndex %d \n", inumFrames);


		int QAZ = m_Buffers->GetIndex();


		bufOffset = nNumBuffers - 1- m_Buffers->GetIndex();
		for (i = 0; i < bufOffset; i++)
			m_Buffers->Next();

		int ZAQ = m_Buffers->GetIndex();

		for (i = 0; i < inumFrames+1; i++)  // g_CameraBufferSize
		{
			j = k * size * bytesPerPixel;

			TRACE("GetCameraDataAsMat In Loop bufIndex %d \n", m_Buffers->GetIndex());

			success = m_Buffers->Read(0, size, (pData + j));

			if (success == FALSE)
			{
				::MessageBox(NULL, _T("Can not read buffer in OnBnClickedopenCV"), NULL, MB_OK);
			}
			k++;
			m_Buffers->Next();
		}

		int ZZZ = m_Buffers->GetIndex();


		// Create a Mat pointing to external memory
		if (bytesPerPixel == 3)
		{
			cv::Mat src(iHeight*(inumFrames+1), iWidth, CV_8UC3, pData);
			src.copyTo(g_CVImageTest);
		}
		else if (bytesPerPixel == 1)
		{
			cv::Mat src(iHeight*(inumFrames+1), iWidth, CV_8UC1, pData);
			src.copyTo(g_CVImageTest);
		}

		delete[] pData;

		for (i = 0; i < bufOffset; i++)
			m_Buffers->Next();

		// below must be send message
		// in sendmessage, message is immediately processed
		// so the pLparam, which is window title is still available 
		m_pView->SendMessage(WM_CAMERA_DATA_READY, 0, m_pLparam);
	}
}


void CReceiveCameraImage::GetCameraIntermediateDataAsMat()
{
	int iWidth = m_Buffers->GetWidth();
	int iHeight = m_Buffers->GetHeight();

	int size = iWidth * iHeight;

	int bytesPerPixel = m_Buffers->GetBytesPerPixel();

	int i = 0;
	int offSet = 0;
	int k = 0;
	int inumFrames = 0;
	int bufOffset = 0;
	BOOL success = FALSE;



	inumFrames = m_Buffers->GetIndex();

	if ((inumFrames == 23) || (inumFrames == 24))
	{
		int ZORRO = 5;
	}


	//if (inumFrames > 0)
	//{
		BYTE *pData = new BYTE[size * bytesPerPixel  * 1];
		if (pData == NULL)
		{
			::MessageBox(NULL, _T("Can not allocate memory in GetCameraIntermediateDataAsMat"), NULL, MB_OK);
		}

		offSet = inumFrames * size * bytesPerPixel;

		int dPrevBufIndex = m_Buffers->GetIndex();
		TRACE("GetCameraIntermediateDataAsMat bufIndex %d \n", dPrevBufIndex);

		if (dPrevBufIndex == 0)
		{
			dPrevBufIndex = g_CameraBufferSize - 1;
		}
		else
		{
			dPrevBufIndex--;
		}
		m_Buffers->SetIndex(dPrevBufIndex);

		

		int QAZ = m_Buffers->GetIndex();

		success = m_Buffers->Read(0, size, pData);

		if (success == FALSE)
		{
			::MessageBox(NULL, _T("Can not read buffer in GetCameraIntermediateDataAsMat"), NULL, MB_OK);
		}

		m_Buffers->Next();

		int ZAQ = m_Buffers->GetIndex();

		// Create a Mat pointing to external memory
		if (bytesPerPixel == 3)
		{
			cv::Mat src(iHeight, iWidth, CV_8UC3, pData);
			src.copyTo(g_CVImageTestIntermediate[g_IntermediateCounter]);
		}
		else if (bytesPerPixel == 1)
		{
			cv::Mat src(iHeight, iWidth, CV_8UC1, pData);
			src.copyTo(g_CVImageTestIntermediate[g_IntermediateCounter]);
		}

		delete[] pData;

		g_IntermediateCounter++;

		SetEvent(g_IntermediateImageReadyEvent);
}