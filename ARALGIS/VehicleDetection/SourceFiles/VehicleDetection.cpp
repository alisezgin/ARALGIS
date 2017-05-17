#include "stdafx.h"


#include "ARALGIS.h"
#include "..\HeaderFiles\VehicleDetection.h"


#include "MainFrm.h"
#include "ARALGISView.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CVehicleDetection::CVehicleDetection
// 
// DESCRIPTION:	C'tor 
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
////////////////////////////////////////////////////////////////////////////////
CVehicleDetection::CVehicleDetection()
{
	bRun = FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CVehicleDetection::CVehicleDetection
// 
// DESCRIPTION:	D'tor
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
////////////////////////////////////////////////////////////////////////////////
CVehicleDetection::~CVehicleDetection()
{
	Shutdown();
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CVehicleDetection::CVehicleDetection
// 
// DESCRIPTION:	starts the CameraDBServer Thread
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
////////////////////////////////////////////////////////////////////////////////
BOOL CVehicleDetection::Start(SCNOTIFYPROC pNotifyProc, CMainFrame* pFrame)
{
	if (bRun)
	{
		TRACE("_beginthreadex(...) failure, for Launch ThreadVehicleDetection::Start\n");
		return FALSE;
	}

	TRACE("Vehicle Detection Thread Starting ...\n");

	m_pFrame = pFrame;
	m_pNotifyProc = pNotifyProc;

	bRun = TRUE;

	ShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	ProcessingFinishedEvent = CreateEvent(NULL, TRUE, TRUE, NULL); /// created as Set, 3rd argument is TRUE !!!!!!!!
	
	ProcessDataEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// Launch Vehicle Detection Thread
	ThreadVehicleDetection = (HANDLE)_beginthreadex(NULL,
													 0,
													 VehicleDetectionThread,
													 this,
													 0,
													 &ThreadIDVD
												    );
	if (!ThreadVehicleDetection)
	{
		TRACE("_beginthreadex(...) failure, ThreadVehicleDetection::Start\n");
		return FALSE;
	}

	TRACE("ThreadVehicleDetection ThreadID %x ...\n", ThreadIDVD);

	// Launch CameraInterface Thread
	ThreadCameraInterface = (HANDLE)_beginthreadex( NULL,
													0,
													CameraInterfaceThread,
													this,
													0,
													&ThreadIDCI
													);
	if (!ThreadCameraInterface)
	{
		TRACE("_beginthreadex(...) failure, ThreadCameraInterface::Start\n");
		return FALSE;
	}

	TRACE("ThreadCameraInterface ThreadID %x ...\n", ThreadIDCI);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CVehicleDetection::Shutdown
// 
// DESCRIPTION:	stops the CameraDBServer thread
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
////////////////////////////////////////////////////////////////////////////////
BOOL CVehicleDetection::Shutdown()
{
	if (!bRun)
		return FALSE;

	TRACE("ThreadVehicleDetection Shutting down ...\n");

	SetEvent(ShutdownEvent);

	WaitForSingleObject(ThreadVehicleDetection, INFINITE);
	WaitForSingleObject(ThreadCameraInterface, INFINITE);

	CloseHandle(ShutdownEvent);
	CloseHandle(ProcessingFinishedEvent);
	CloseHandle(ProcessDataEvent); 
	CloseHandle(ThreadVehicleDetection);
	CloseHandle(ThreadCameraInterface);


	bRun = FALSE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CVehicleDetection::CameraInterfaceThread
// 
// DESCRIPTION:	thread function for starting camera interface thread 
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
////////////////////////////////////////////////////////////////////////////////
UINT __stdcall CVehicleDetection::CameraInterfaceThread(LPVOID pParam)
{
	CVehicleDetection *pVehicleDetection = (CVehicleDetection*)pParam;

	int dFrmCntr;

	TRACE("CameraInterfaceThread Started\n");

	HANDLE Handles[2];
	Handles[0] = pVehicleDetection->ShutdownEvent;
	Handles[1] = g_IntermediateImageReadyEvent;

	for (;;)
	{
		DWORD EventCaused = WaitForMultipleObjects( 2,
													Handles,
													FALSE,
													INFINITE);

		if (EventCaused == WAIT_FAILED || EventCaused == WAIT_OBJECT_0)
		{
			if (EventCaused == WAIT_FAILED)
				TRACE("WaitForMultipleObjects failure at CameraInterfaceThread", GetLastError());
			else
				TRACE("CameraInterfaceThread is shutting Down normally...\n");

			return THREADEXIT_SUCCESS;
		}

		// g_IntermediateImageReadyEvent is received
		else if (EventCaused == WAIT_OBJECT_0 + 1)
		{
			TRACE("g_IntermediateImageReadyEvent received \n");
			dFrmCntr = g_IntermediateCounter;


			if (dFrmCntr == 1)
			{
				TRACE("waiting EventTTTTTTTT \n");
				WaitForSingleObject(pVehicleDetection->ProcessingFinishedEvent, INFINITE);
				ResetEvent(pVehicleDetection->ProcessingFinishedEvent);
				pVehicleDetection->m_dProcessedFrameCounter = 0;

				pVehicleDetection->m_dFrameCounter = dFrmCntr;
				pVehicleDetection->m_isStopSignalled = false;

				//SetEvent(pVehicleDetection->ProcessDataEvent);
			}
			pVehicleDetection->m_dFrameCounter = dFrmCntr;

			TRACE("THR_2 FRM_CNT %d PRS_CNT %d \n", pVehicleDetection->m_dFrameCounter, pVehicleDetection->m_dProcessedFrameCounter);


			if (pVehicleDetection->m_dFrameCounter - pVehicleDetection->m_dProcessedFrameCounter - 1 == 0)
			{
				TRACE("SetEvent(pVehicleDetection->ProcessDataEvent) \n");
				SetEvent(pVehicleDetection->ProcessDataEvent);
			}

			
			ResetEvent(g_IntermediateImageReadyEvent);

		}

	} // infite for loop

	return THREADEXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CVehicleDetection::VehicleDetectionThread
// 
// DESCRIPTION:	thread function for starting vehicle detection thread from test image 
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
////////////////////////////////////////////////////////////////////////////////
UINT __stdcall CVehicleDetection::VehicleDetectionThread(LPVOID pParam)
{
	CVehicleDetection *pVehicleDetection = (CVehicleDetection*)pParam;

	TRACE("VehicleDetectionThread Started\n");

	bool bIsProcessingFinished;
	bool bIsCarFound = false;

	cv::Mat inGray;
	cv::Mat inClr;
	int nRows;
	int nCols;
	uchar* dataPtr1;
	float fMean[50000];
	// boraN code analysis
	// decrease 50000. It uses all of the stack. put it to the heap.

	int dAnalysisWindow = 10;

	float fMeanWindow;
	float fStdWindow;

	bool bBegIdxFound = false;
	bool bEndIdxFound = false;

	float fMeanWindowPrev, fMeanWindowCurr;
	float fStdWindowPrev, fStdWindowCurr;
	float fMeanWindowPrev2, fStdWindowPrev2;
	float fMeanWindowPrev3, fStdWindowPrev3;

	float fCoeffOfVar1, fCoeffOfVar2;



	HANDLE Handles[2];
	Handles[0] = pVehicleDetection->ShutdownEvent;
	Handles[1] = pVehicleDetection->ProcessDataEvent;

	for (;;)
	{
		DWORD EventCaused = WaitForMultipleObjects( 2,
													Handles,
													FALSE,
													INFINITE);

		if (EventCaused == WAIT_FAILED || EventCaused == WAIT_OBJECT_0)
		{
			if (EventCaused == WAIT_FAILED)
				TRACE("WaitForMultipleObjects failure at VehicleDetectionThread", GetLastError());
			else
				TRACE("VehicleDetectionThread is shutting Down normally...\n");

			return THREADEXIT_SUCCESS;
		}

		// ProcessDataEvent is received
		else if (EventCaused == WAIT_OBJECT_0 + 1)
		{
			if (pVehicleDetection->m_dFrameCounter == 1)
			{
				TRACE("THR_1 Init\n");

				pVehicleDetection->m_dlineCounter = 0;

				g_dEndIndex = g_dBeginIndex = 0;

				fStdWindowPrev = fStdWindowCurr = 0.0;
				fMeanWindowPrev = fMeanWindowCurr = 0;
				fMeanWindowPrev2 = fStdWindowPrev2 = 0.0;
				fMeanWindowPrev3 = fStdWindowPrev3 = 0.0;
				bEndIdxFound = bBegIdxFound = false;


				bIsCarFound = false;
			}

			bIsProcessingFinished = false;

			while ((bIsProcessingFinished == false) )
			{
				TRACE("PRS FRM-CNT %d \n", pVehicleDetection->m_dProcessedFrameCounter);
				g_CVImageTestIntermediate[pVehicleDetection->m_dProcessedFrameCounter].copyTo(inClr);
				cv::cvtColor(inClr, inGray, CV_RGB2GRAY);

				nRows = inGray.rows;
				nCols = inGray.cols;

				fStdWindowPrev = fStdWindowCurr = 0.0;
				fMeanWindowPrev = fMeanWindowCurr = 0;
				fMeanWindowPrev2 = fStdWindowPrev2 = 0.0;
				fMeanWindowPrev3 = fStdWindowPrev3 = 0.0;

				TRACE("lineCounter-1 %d \n", pVehicleDetection->m_dlineCounter);


				for (int i = 0; i < nRows; i++)
				{
					dataPtr1 = inGray.ptr<uchar>(i);
					fMean[pVehicleDetection->m_dlineCounter] = 0.0;

					for (int j = 0; j < nCols; j++)
					{
						fMean[pVehicleDetection->m_dlineCounter] += (uchar)dataPtr1[j];
					}

					fMean[pVehicleDetection->m_dlineCounter] = (float)((float)fMean[pVehicleDetection->m_dlineCounter] / (float)nCols);
					pVehicleDetection->m_dlineCounter++;
				}
				TRACE("lineCounter-2 %d \n", pVehicleDetection->m_dlineCounter);


				for (int k = 0; k < pVehicleDetection->m_dlineCounter - dAnalysisWindow; k++)
				{
					fMeanWindow = 0;
					for (int i = k; i < (dAnalysisWindow + k); i++)
					{
						fMeanWindow += fMean[i];
					}
					fMeanWindow = fMeanWindow / (float)dAnalysisWindow;

					fStdWindow = 0;
					for (int i = k; i < (dAnalysisWindow + k); i++)
					{
						fStdWindow += ((fMeanWindow - fMean[i]) * (fMeanWindow - fMean[i]));
					}
					fStdWindow = fStdWindow / (float)dAnalysisWindow;
					fStdWindow = sqrtf(fStdWindow);


					fStdWindowPrev3 = fStdWindowPrev2;
					fMeanWindowPrev3 = fMeanWindowPrev2;

					fStdWindowPrev2 = fStdWindowPrev;
					fMeanWindowPrev2 = fMeanWindowPrev;

					fStdWindowPrev = fStdWindowCurr;
					fMeanWindowPrev = fMeanWindowCurr;

					fMeanWindowCurr = fMeanWindow;
					fStdWindowCurr = fStdWindow;

					fCoeffOfVar1 = fMeanWindowPrev / fStdWindowPrev;
					fCoeffOfVar2 = fMeanWindowCurr / fStdWindowCurr;


					//if (k >  nRows)
					//{

						if (
							(
							(bBegIdxFound == false) &&
							(fStdWindowCurr > 2.0) &&
							(fStdWindowPrev > 1.0) &&
							(fStdWindowPrev2 > 0.5) &&
							(fStdWindowPrev3 > 0.5)
							)
							&&
							(
							(fCoeffOfVar1 < 500.0) &&
							(bBegIdxFound == false)
							))
						{
							bBegIdxFound = true;
							g_dBeginIndex = k;
						}


						if (
							(
							(bEndIdxFound == false) && (bBegIdxFound == true) &&
							(fStdWindowCurr < 0.5) &&
							(fStdWindowPrev < 0.5) &&
							(fStdWindowPrev2 < 0.5) &&
							(fStdWindowPrev3 < 0.5) &&
							(k - g_dBeginIndex > 2000)
							)
							&&
							(
							(bEndIdxFound == false) && (bBegIdxFound == true) &&
							(k - g_dBeginIndex > 2000) &&
							(fCoeffOfVar1 > 2000.0)
							))
						{
							bEndIdxFound = true;
							g_dEndIndex = k;
						}
						
					//}

				}

				//pVehicleDetection->m_pNotifyProc((LPVOID)pVehicleDetection->m_pFrame, FILTER_PROCESS_FILTER1_READY);

				if ((bBegIdxFound == true) && (bEndIdxFound == true))
				{
					bIsCarFound = true;

					if (pVehicleDetection->m_isStopSignalled == false)
					{
						pVehicleDetection->m_isStopSignalled = true;
						//Sleep(1000);
						TRACE("Signal STOPPPPPPPPPPPPPPPP\n");
						SetEvent(g_CameraStopDataRecieveEvent);
					}
				}

				
				TRACE("FRM_CNT %d PRS_CNT %d \n", pVehicleDetection->m_dFrameCounter, pVehicleDetection->m_dProcessedFrameCounter);

				pVehicleDetection->m_dProcessedFrameCounter++;


				if (
					   (bIsCarFound == true) ||
					   (pVehicleDetection->m_dFrameCounter - pVehicleDetection->m_dProcessedFrameCounter  == 0)
				   )
				{
					bIsProcessingFinished = true;
					ResetEvent(pVehicleDetection->ProcessDataEvent);  ///???????????????
					SetEvent(pVehicleDetection->ProcessingFinishedEvent);
				}

			}
		}
	} // infite for loop

	return THREADEXIT_SUCCESS;
}





