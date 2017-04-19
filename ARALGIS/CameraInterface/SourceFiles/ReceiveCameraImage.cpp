#include "stdafx.h"

#include "..\\HeaderFiles\\ReceiveCameraImage.h"
//#include "HighFrameRateDlg.h"
#include "float.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "ARALGISView.h"

CReceiveCameraImage::CReceiveCameraImage()
{
	m_FrameRate = 0.0f;
	m_MaxTime = 0.0f;
	m_MinTime = 0.0f;
	m_ActiveBuffer = 0;
	m_BufferCount = 0;
	m_Slider = 0;
	m_Msg = _T("");

	m_AcqDevice = NULL;
	m_Buffers = NULL;
	m_Xfer = NULL;
	//m_View = NULL;

	m_IsSignalDetected = TRUE;

	m_bRecordOn = FALSE;
	m_bPlayOn = FALSE;
	m_bPauseOn = FALSE;

	m_nFramesPerCallback = 1;
	m_nFramesLost = 0;

	m_frameTime = 0;

	InitSAPERA();
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
	SetEvent(g_CameraUpdateControlsEvent);
}

//==============================================================================
// Name      : CGigESeqGrabDemoDlg::XferCallback
// Purpose   : Callback function for the transfer.
// Parameters: None
//==============================================================================
void CReceiveCameraImage::XferCallback(SapXferCallbackInfo *pInfo)
{
	CReceiveCameraImage *pDlg = (CReceiveCameraImage *)pInfo->GetContext();

	// If grabbing in trash buffer, do not display the image, update the
	// appropriate number of frames on the status bar instead
	/*if (pInfo->IsTrash())
	{
	m_Msg.Format("Frames acquired in trash buffer: %d", pInfo->GetEventCount());
	}*/

	// Measure real frame time
	pDlg->UpdateFrameRate();

	// Check if last frame is reached
	pDlg->CheckForLastFrame();

	if (((pDlg->m_Buffers->GetIndex() + 1) % pDlg->m_nFramesPerCallback) == 0 ||
		pDlg->m_Buffers->GetIndex() == pDlg->m_Buffers->GetCount() - 1)
	{
		// Refresh view
		//pDlg->m_View->Show();

		// Refresh controls
		SetEvent(g_CameraUpdateControlsEvent);
	}

	//CString str;
	//str.Format(_T("Current Buffer Number: %d"), pDlg->m_Buffers->GetIndex());
	//pDlg->m_statusWnd.SetWindowText(str);

} // End of the CGigESeqGrabDemoDlg::XferCallback method.


//==============================================================================
// Name      : CReceiveCameraImage::InitSAPERA
// Purpose   : SAPERA initialization.
// Parameters: None
//==============================================================================
BOOL CReceiveCameraImage::InitSAPERA(void)
{
	// Are we operating on-line?
	CAcqConfigDlg dlg(NULL, CAcqConfigDlg::ServerAcqDevice); // boraN added NULL

	if (dlg.DoModal() == IDOK)
	{
		// Define objects
		m_AcqDevice = new SapAcqDevice(dlg.GetLocation(), NULL);

		CString configFile;
		dlg.GetConfigFile(configFile);


		char dummy[1000];
		size_t i = configFile.GetLength();
		strncpy_s(dummy, "\n", 1000);
		strncpy_s(dummy, CStringA(configFile).GetBuffer(), i);

		SapLocation loc = dlg.GetLocation();
		m_AcqDevice->SetLocation(loc);
		m_AcqDevice->SetConfigFile(dummy);

		m_Buffers = new SapBufferWithTrash(MAX_BUFFER, m_AcqDevice);
		m_Xfer = new SapAcqDeviceToBuf(m_AcqDevice, m_Buffers, XferCallback, this);
	}
	else
	{
		// Define off-line objects
		m_Buffers = new SapBuffer(MAX_BUFFER, 2048, IMAGE_HEIGHT, SapFormatRGB888, SapBuffer::TypeScatterGather); // MAX_BUFFER

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

	if (m_Buffers->GetIndex() == m_Buffers->GetCount() - 1)
	{
		if (m_bRecordOn)
		{
			m_bRecordOn = FALSE;
			//KillTimer(1);
			SetEvent(g_CameraStopDataRecieveEvent);  // g_KillTimerEvent
			ComputeFrameRate();
		}

		if (m_bPlayOn)
		{
			//m_bPlayOn = FALSE;
			//KillTimer(1);
			SetEvent(g_CameraStopDataRecieveEvent); // g_KillTimerEvent
		} // End if.

		SetEvent(g_CameraUpdateControlsEvent);

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

	// Resfresh display
	//m_View->Show();

	// Check if last frame is reached
	CheckForLastFrame();

	// Refresh controls
	SetEvent(g_CameraUpdateControlsEvent);

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
	SetEvent(g_CameraUpdateControlsEvent);

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
	m_Slider = m_Buffers->GetIndex();
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
	//m_nFramesLost = 0;

	//// Acquire all frames
	//m_Xfer->Init();

	//if (m_Xfer->Snap(m_Buffers->GetCount()))
	//{
	//	m_bRecordOn = TRUE;
	//}
	// online operation upto here

	///// delete these lines 
	CLoadSaveDlg dlg(NULL, m_Buffers, TRUE, TRUE);
	if (dlg.DoModal() == IDOK)
	{

	}
	/////////////  delete these lines
	
	// Initialize buffer index
	m_Buffers->SetIndex(0);

	//m_Buffers->ResetIndex();

	int AA = m_Buffers->GetIndex();
	int BB = m_Buffers->GetCount();

	// Start playback timer
	//int frameTime = (int)(1000.0 / m_Buffers->GetFrameRate());
	m_frameTime = (int)(1000.0 / m_Buffers->GetFrameRate());

	//SetTimer(1, frameTime, NULL);
	SetEvent(g_SetTimerFrameRateEvent);

	m_bPlayOn = TRUE;

	//SetEvent(g_CameraUpdateControlsEvent);
} // End of the CReceiveCameraImage::OnPlay method.

//==============================================================================
// Name      : CReceiveCameraImage::OnPause
// Purpose   : Pause the recording or playing of frames.
// Parameters: None
//==============================================================================
void CReceiveCameraImage::PauseDataReception(void)
{
	if (!m_bPauseOn)
	{
		// Check if recording or playing
		if (m_bRecordOn)
		{
			//KillTimer(1);
			SetEvent(g_KillTimerEvent);

			// Stop current acquisition
			if (!m_Xfer->Freeze())
				return;

			if (CAbortDlg(AfxGetApp()->m_pMainWnd, m_Xfer).DoModal() != IDOK)
				m_Xfer->Abort();
		}
		else if (m_bPlayOn)
		{
			// Stop playback timer
			//KillTimer(1);

			SetEvent(g_KillTimerEvent);
		} // End if, else if.
	}
	else
	{
		// Check if recording or playing
		if (m_bRecordOn)
		{
			//int frameTime = (int)(1000.0 / m_Buffers->GetFrameRate());
			m_frameTime = (int)(1000.0 / m_Buffers->GetFrameRate());

			//SetTimer(1, frameTime, NULL);
			SetEvent(g_SetTimerFrameRateEvent);

			// Acquire remaining frames
			if (!m_Xfer->Snap(m_Buffers->GetCount() - m_Buffers->GetIndex() - 1))
				return;
		}
		else if (m_bPlayOn)
		{
			// Restart playback timer
			//int frameTime = (int)(1000.0 / m_Buffers->GetFrameRate());
			m_frameTime = (int)(1000.0 / m_Buffers->GetFrameRate());

			//SetTimer(1, frameTime, NULL);
			SetEvent(g_SetTimerFrameRateEvent);

		} // End if, else if.
	} // End if, else.

	m_bPauseOn = !m_bPauseOn;

	SetEvent(g_CameraUpdateControlsEvent);
} // End of the CReceiveCameraImage::OnPause method.

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

		if (CAbortDlg(AfxGetApp()->m_pMainWnd, m_Xfer).DoModal() != IDOK) /// buraya SAP help'ten bak ve gerekli düzenlemeyi yap
			m_Xfer->Abort();

		m_bRecordOn = FALSE;
	}
	else if (m_bPlayOn)
	{
		// Stop playback timer
		//KillTimer(1);
		SetEvent(g_KillTimerEvent);

		m_bPlayOn = FALSE;
	} // End if, else if.

	m_bPauseOn = FALSE;

	//SetEvent(g_CameraUpdateControlsEvent);
} // End of the CReceiveCameraImage::OnStop method.

//*****************************************************************************************
//
//             General Options
//
//*****************************************************************************************

//==============================================================================
// Name      : CReceiveCameraImage::OnBufferOptions
// Purpose   : Change the number of buffers used.
// Parameters: None
//==============================================================================
//void CReceiveCameraImage::OnBufferOptions(void)
//{
	//CBufDlg dlg(this, m_Buffers, m_View->GetDisplay());
	//if (dlg.DoModal() == IDOK)
	//{
	//	CWaitCursor cur;

	//	// Destroy objects
	//	DestroyObjects();

	//	// Update buffer object
	//	SapBuffer buf = *m_Buffers;
	//	*m_Buffers = dlg.GetBuffer();

	//	// Recreate objects
	//	if (!CreateObjects())
	//	{
	//		*m_Buffers = buf;
	//		CreateObjects();
	//	} // End if.

	//	SetEvent(g_CameraUpdateControlsEvent);
	//} // End if.
//} // End of the CReceiveCameraImage::OnBufferOptions method.

//==============================================================================
// Name      : CReceiveCameraImage::OnLoadAcqConfig
// Purpose   : Select a new configuration file for the acquisition.
// Parameters: None
//==============================================================================
void CReceiveCameraImage::LoadConfiguationFile(void)
{
	// Set acquisition parameters
	CAcqConfigDlg dlg(NULL, CAcqConfigDlg::ServerAcqDevice); // boraN changed this --> NULL
	if (dlg.DoModal() == IDOK)
	{
		// Destroy objects
		DestroyObjects();

		// Backup
		SapLocation loc = m_AcqDevice->GetLocation();

		// Update object
		m_AcqDevice->SetLocation(dlg.GetLocation());

		//CString configFile = dlg.GetConfigFile();

		CString configFile;
		dlg.GetConfigFile(configFile);


		char dummy[1000];
		size_t i = configFile.GetLength();
		strncpy_s(dummy, "\n", 1000);
		strncpy_s(dummy, CStringA(configFile).GetBuffer(), i);
		m_AcqDevice->SetConfigFile(dummy);

		// Recreate objects
		if (!CreateObjects())
		{
			m_AcqDevice->SetLocation(loc);
			m_AcqDevice->SetConfigFile(dummy);
			CreateObjects();
		} // End if.

		SetEvent(g_CameraUpdateControlsEvent);
	} // End if, else.
} // End of the CReceiveCameraImage::OnLoadAcqConfig method.

//*****************************************************************************************
//
//             File Options
//
//*****************************************************************************************

//==============================================================================
// Name      : CReceiveCameraImage::OnHighFrameRate
// Purpose   :
// Parameters: None
//==============================================================================
//void CReceiveCameraImage::OnHighFrameRate()
//{
//	CHighFrameRateDlg dlg(this, m_nFramesPerCallback, m_Xfer);
//
//	if (dlg.DoModal() == IDOK)
//	{
//		CWaitCursor cursor;
//
//		m_nFramesPerCallback = dlg.GetNFramesPerCallback();
//
//		// Destroy transfer object
//		m_Xfer->Destroy();
//
//		// Recreate transfer object with new setting
//		CreateObjects();
//	}
//}// End of the CGigESeqGrabDemoDlg::OnHighFrameRate method.
//==============================================================================

void CReceiveCameraImage::DisplayRecordSelectDialog()
{
	if (m_Buffers->GetFormat() == SapFormatMono16)
	{
		::MessageBox(NULL, _T("Sequence images in AVI format are sampled at 8-bit pixel depth.\nYou cannot load a sequence in the current configuration."), NULL, MB_OK);

		return;
	}

	if ((m_Buffers->GetFormat() == SapFormatRGBR888))
	{
		::MessageBox(NULL, _T("Sequence images acquired in RGBR888 format (red first) were saved as RGB888 (blue first).\nYou cannot load a sequence in the current configuration."), NULL, MB_OK);

		return;
	}

	CLoadSaveDlg dlg(NULL, m_Buffers, TRUE, TRUE);
	if (dlg.DoModal() == IDOK)
	{
		SetEvent(g_CameraUpdateControlsEvent);
	} // End if.  SapFormatRGB888
}

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
	//int pixel_depth = m_Buffers->GetPixelDepth();
	int bytesPerPixel = m_Buffers->GetBytesPerPixel();

	int i = 0;
	int j = 0;
	int k = 0;
	int inumFrames = 0;
	int bufOffset = 0;

	int nNumBuffers = m_Buffers->GetCount();

	//for (i = 0; i < MAX_BUFFER; i++)
	//{
	//	inumFrames++;
	//}

	//if (inumFrames > nNumBuffers)
	//{
	//	inumFrames = nNumBuffers;
	//}

	inumFrames = m_Buffers->GetIndex();


	if (inumFrames > 0)
	{
		BYTE *pData = new BYTE[size * bytesPerPixel  * inumFrames];
		if (pData == NULL)
		{
			::MessageBox(NULL, _T("Can not allocate memory in OnBnClickedopenCV"), NULL, MB_OK);
		}
		BOOL success = FALSE;

		int iCount = m_Buffers->GetCount();
		int iIndex = m_Buffers->GetIndex();

		bufOffset = nNumBuffers - 1 - m_Buffers->GetIndex();
		for (i = 0; i < bufOffset; i++)
			m_Buffers->Next();

		for (i = 0; i < inumFrames; i++)  // MAX_BUFFER
		{
			//if (m_selList[i] == TRUE)
			//{
				j = k * size * bytesPerPixel;
				success = m_Buffers->Read(0, size, (pData + j));

				if (success == FALSE)
				{
					::MessageBox(NULL, _T("Can not read buffer in OnBnClickedopenCV"), NULL, MB_OK);
				}
				k++;
			//}
			m_Buffers->Next();
		}

		// Create a Mat pointing to external memory
		if (bytesPerPixel == 3)
		{
			cv::Mat src(iHeight*inumFrames, iWidth, CV_8UC3, pData);
			src.copyTo(g_CVImageTest);
		}
		else if (bytesPerPixel == 1)
		{
			cv::Mat src(iHeight*inumFrames, iWidth, CV_8UC1, pData);
			src.copyTo(g_CVImageTest);
		}

		delete[] pData;


		CFrameWnd *pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);

		if (!pFrame)
		{
			::MessageBox(NULL, _T("Frame Window Not OK in CReceiveCameraImage::GetCameraDataAsMat"), NULL, MB_OK);
		}


		CView * pView = pFrame->GetActiveView();

		if (!pView)
		{
			::MessageBox(NULL, _T("GetActiveView Not OK"), NULL, MB_OK);
		}

		// Fail if view is of wrong kind
		if (!pView->IsKindOf(RUNTIME_CLASS(CARALGISView)))
		{
			::MessageBox(NULL, _T("GetActiveView Not OK"), NULL, MB_OK);
		}

		LPARAM pLparam;
		pLparam = reinterpret_cast<LPARAM>("ARALGIS");
		// below must be send message
		// in sendmessage, message is immediately processed
		// so the pLparam, which is window title is still available 
		// when DestroyWindow is called within SketcherDoc class
		pView->SendMessage(WM_CAMERA_DATA_READY, 0, pLparam);
	}
}
