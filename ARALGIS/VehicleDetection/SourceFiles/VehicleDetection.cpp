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
BOOL CVehicleDetection::Start(CMainFrame* pFrame)
{
	if (bRun)
	{
		TRACE("_beginthreadex(...) failure, for Launch ThreadVehicleDetection::Start\n");
		return FALSE;
	}

	TRACE("Vehicle Detection Thread Starting ...\n");

	m_pFrame = pFrame;

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

	TRACE(L"VehicleDetection Camera Interface Thread Starting ...\n");

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
		TRACE("_beginthreadex(...) failure, VehicleDetection-ThreadCameraInterface::Start\n");
		return FALSE;
	}

	TRACE("VehicleDetection-ThreadCameraInterface ThreadID %x ...\n", ThreadIDCI);

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
			if (g_AutoDetect_Type != NO_AUTO_VEHICLE_DETECT)
			{
				dFrmCntr = g_IntermediateCounter;
			}
			else
			{
				dFrmCntr = 1;
			}

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

			if (g_AutoDetect_Type != NO_AUTO_VEHICLE_DETECT)
			{
				if (pVehicleDetection->m_dFrameCounter - pVehicleDetection->m_dProcessedFrameCounter - 1 == 0)
				{
					TRACE(L"SetEvent(pVehicleDetection->ProcessDataEvent) \n");
					SetEvent(pVehicleDetection->ProcessDataEvent);
				}
			}
			else
			{
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

	CView * pView = pVehicleDetection->m_pFrame->GetActiveView();
	LPARAM pLparam;
	pLparam = reinterpret_cast<LPARAM>("ARALGIS");

	bool bIsProcessingFinished;
	bool bIsCarFound = false;

	cv::Mat inGray;
	cv::Mat inClr;
	int nRows;
	int nCols;
	uchar* dataPtr1;
	float* fMean;
	fMean = new float[100000];


//////////////////////////////////////////////
	int dAnalysisWindow = 10;

	float fMeanWindow;
	float fStdWindow;

	bool bBegIdxFound;
	bool bEndIdxFound;


	float fMeanWindowPrev, fMeanWindowCurr;
	float fStdWindowPrev, fStdWindowCurr;
	float fMeanWindowPrev2, fStdWindowPrev2;
	float fMeanWindowPrev3, fStdWindowPrev3;

	float fCoeffOfVar1, fCoeffOfVar2;

	//int k;

	fStdWindowPrev = fStdWindowCurr = 0.0;
	fMeanWindowPrev = fMeanWindowCurr = 0;
	fMeanWindowPrev2 = fStdWindowPrev2 = 0.0;

	float fMeanMeanStart1, fMeanMeanStart2;
	float fStdMeanStart1, fStdMeanStart2;
	int dMeanStartCtr1, dMeanStartCtr2;
	int dStartCancelCounter = 0;
	float fStartMean = 0.0;
	float fStartStd = 0.0;
	int dThresholdStartControl = 200;
	int dStartZeroStdCntr = 0;
	int dStartZeroStdThreshold = 5;

	int dStdSpikeCounterStart = 0;
	int dSpikeCtrTresholdStart = 30;

	fMeanMeanStart1 = fMeanMeanStart2 = 0.0;
	fStdMeanStart1 = fStdMeanStart2 = 0.0;
	dMeanStartCtr1 = 0;
	dMeanStartCtr2 = 0;


	float fMeanMeanEnd1, fMeanMeanEnd2;
	float fStdMeanEnd1, fStdMeanEnd2;
	int dMeanEndCtr1, dMeanEndCtr2;
	int dEndCancelCounter = 0;
	float fEndMean = 0.0;
	float fEndStd = 0.0;


	fMeanMeanEnd2 = 0.0;
	fStdMeanEnd2 = 0.0;

	fMeanMeanStart1 = fMeanMeanStart2 = (float)0.0;
	fStdMeanStart1 = fStdMeanStart2 = (float)0.0;
	dMeanEndCtr1 = dMeanEndCtr2 = 0;
	int dThresholdEndControl = 200;

	int dStdSpikeCounterEnd = 0;
	int dSpikeCtrTresholdEnd = 15;

	int dSmallStdCounterEnd = 0;
	int dSmallStdCounterTresholdEnd = 5;

	float fCVEnd;
	int   dCVEndCounter = 0;
	int   dCVEndCounterThreshold = 10;

	float dMagicMultiplier = (float)0.3;
	float dMagicMultiplier1 = (float)0.10;
	float dMagicMultiplier2 = (float) 0.05;
	float dMagicMultiplier3 = (float)0.01;

	bool bStartPassedTest = false;
	bool bEndPassedTest = false;

//////////////////////////////////////////////

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

			delete[] fMean;
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

				fStdWindowCurr  = (float)0.0;
				fStdWindowPrev  = (float)0.0;
				fStdWindowPrev2 = (float)0.0;
				fStdWindowPrev3 = (float)0.0;

				fMeanWindowCurr  = (float)0.0;
				fMeanWindowPrev  = (float)0.0;
				fMeanWindowPrev2 = (float)0.0;
				fMeanWindowPrev3 =(float)0.0;

				bEndIdxFound = false;
				bBegIdxFound = false;
				bIsCarFound = false;

				g_dCarDetectCount = 0;
				g_CarFound = FALSE;

				dMeanStartCtr1 = 0;
				dMeanStartCtr2 = 0;

			/////////
				dStartCancelCounter = 0;
				fStartMean =(float)0.0;
				fStartStd = (float)0.0;
				dThresholdStartControl = 200;
				dStartZeroStdCntr = 0;
				dStartZeroStdThreshold = 5;

				dStdSpikeCounterStart = 0;
				dSpikeCtrTresholdStart = 30;

				dEndCancelCounter = 0;
				fEndMean = 0.0;						
				fEndStd = 0.0;
					
				fMeanMeanEnd2 = (float)0.0;
				fStdMeanEnd2 = (float)0.0;

				fMeanMeanStart1 = fMeanMeanStart2 = (float)0.0;
				fStdMeanStart1 = fStdMeanStart2 = (float)0.0;
				dMeanEndCtr1 = 0;
				dMeanEndCtr2 = 0;
				dThresholdEndControl = 200;

				dStdSpikeCounterEnd = 0;
				dSpikeCtrTresholdEnd = 15;

				dSmallStdCounterEnd = 0;
				dSmallStdCounterTresholdEnd = 5;

				dCVEndCounter = 0;
				dCVEndCounterThreshold = 10;

				bStartPassedTest = false;
				bEndPassedTest = false;
			/////////
			}
			else
			{
				g_dEndIndex = g_dBeginIndex = 0;

				fStdWindowCurr = (float)0.0;
				fStdWindowPrev = (float)0.0;
				fStdWindowPrev2 = (float)0.0;
				fStdWindowPrev3 = (float)0.0;

				fMeanWindowCurr = (float)0.0;
				fMeanWindowPrev = (float)0.0;
				fMeanWindowPrev2 = (float)0.0;
				fMeanWindowPrev3 = (float)0.0;

				bEndIdxFound = false;
				bBegIdxFound = false;
				bIsCarFound = false;

				g_dCarDetectCount = 0;
				g_CarFound = FALSE;

				dMeanStartCtr1 = 0;
				dMeanStartCtr2 = 0;

				/////////
				dStartCancelCounter = 0;
				fStartMean = (float)0.0;
				fStartStd = (float)0.0;
				dThresholdStartControl = 200;
				dStartZeroStdCntr = 0;
				dStartZeroStdThreshold = 5;

				dStdSpikeCounterStart = 0;
				dSpikeCtrTresholdStart = 30;

				dEndCancelCounter = 0;
				fEndMean = 0.0;
				fEndStd = 0.0;

				fMeanMeanEnd2 = (float)0.0;
				fStdMeanEnd2 = (float)0.0;
				dMeanEndCtr2 = 0;

				fMeanMeanStart1 = fMeanMeanStart2 = (float)0.0;
				fStdMeanStart1 = fStdMeanStart2 = (float)0.0;
				dMeanEndCtr1 = 0;
				dMeanEndCtr2 = 0;
				dThresholdEndControl = 200;

				dStdSpikeCounterEnd = 0;
				dSpikeCtrTresholdEnd = 15;

				dSmallStdCounterEnd = 0;
				dSmallStdCounterTresholdEnd = 5;

				dCVEndCounter = 0;
				dCVEndCounterThreshold = 10;

				bStartPassedTest = false;
				bEndPassedTest = false;

				///TRACE("\n\n\n RESETTING THINGS !!!!!!!!!!!!!!!!!!!\n\n\n");
			}

			//TRACE("\n  STARTING WHILE \n");

			bIsProcessingFinished = false;

			while ((bIsProcessingFinished == false) )
			{
				TRACE("PRS FRM-CNT %d \n", pVehicleDetection->m_dProcessedFrameCounter);

				if (g_CameraPixelBits == 24)
				{
					inClr = g_CVImageTestIntermediate[pVehicleDetection->m_dProcessedFrameCounter].clone();
					cv::cvtColor(inClr, inGray, CV_RGB2GRAY);
				}
				else
				{
					if (g_AutoDetect_Type != NO_AUTO_VEHICLE_DETECT)
					{
						inGray.create(g_CVImageTestIntermediate[pVehicleDetection->m_dProcessedFrameCounter].size(),
							g_CVImageTestIntermediate[pVehicleDetection->m_dProcessedFrameCounter].type());
						inGray = g_CVImageTestIntermediate[pVehicleDetection->m_dProcessedFrameCounter].clone();
					}
					else
					{
						inGray.create(g_CVImageTest.size(),
							g_CVImageTest.type());
						inGray = g_CVImageTest.clone();
					}
				}

				nRows = inGray.rows;
				nCols = inGray.cols;

				fStdWindowPrev = fStdWindowCurr = 0.0;
				fMeanWindowPrev = fMeanWindowCurr = 0;
				fMeanWindowPrev2 = fStdWindowPrev2 = 0.0;
				fMeanWindowPrev3 = fStdWindowPrev3 = 0.0;


				{
					g_dEndIndex = g_dBeginIndex = 0;

					fStdWindowCurr = (float)0.0;
					fStdWindowPrev = (float)0.0;
					fStdWindowPrev2 = (float)0.0;
					fStdWindowPrev3 = (float)0.0;

					fMeanWindowCurr = (float)0.0;
					fMeanWindowPrev = (float)0.0;
					fMeanWindowPrev2 = (float)0.0;
					fMeanWindowPrev3 = (float)0.0;

					bEndIdxFound = false;
					bBegIdxFound = false;
					bIsCarFound = false;
					g_CarFound = FALSE;

					g_dCarDetectCount = 0;

					dMeanStartCtr1 = 0;
					dMeanStartCtr2 = 0;

					/////////
					dStartCancelCounter = 0;
					fStartMean = (float)0.0;
					fStartStd = (float)0.0;
					dThresholdStartControl = 200;
					dStartZeroStdCntr = 0;
					dStartZeroStdThreshold = 5;

					dStdSpikeCounterStart = 0;
					dSpikeCtrTresholdStart = 30;

					dEndCancelCounter = 0;
					fEndMean = 0.0;
					fEndStd = 0.0;

					fMeanMeanEnd2 = (float)0.0;
					fStdMeanEnd2 = (float)0.0;

					fMeanMeanStart1 = fMeanMeanStart2 = (float)0.0;
					fStdMeanStart1 = fStdMeanStart2 = (float)0.0;
					dThresholdEndControl = 200;
					dMeanEndCtr1 = 0;
					dMeanEndCtr2 = 0;

					dStdSpikeCounterEnd = 0;
					dSpikeCtrTresholdEnd = 15;

					dSmallStdCounterEnd = 0;
					dSmallStdCounterTresholdEnd = 5;

					dCVEndCounter = 0;
					dCVEndCounterThreshold = 10;

					bStartPassedTest = false;
					bEndPassedTest = false;

					//TRACE("\n\n\n RESETTING THINGS !!!!!!!!!!!!!!!!!!!\n\n\n");
				}

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

#ifdef DEBUG_BORA
				FILE* fp;
				fopen_s(&fp, "C:\\Users\\bora\\Desktop\\sil\\meanStd.txt", "w");
#endif

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


					if (bBegIdxFound == false)
					{
						fMeanMeanStart1 += fMeanWindow;
						fStdMeanStart1 += fStdWindow;
						dMeanStartCtr1++;
					}

					if (bBegIdxFound == true)
					{
						fMeanMeanStart2 += fMeanWindow;
						fStdMeanStart2 += fStdWindow;
						dMeanStartCtr2++;
					}

					if (bEndIdxFound == true)
					{
						fMeanMeanEnd2 += fMeanWindow;
						fStdMeanEnd2 += fStdWindow;
						dMeanEndCtr2++;
					}

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

					// check if begin index is still valid
					if ((dMeanStartCtr2 < dThresholdStartControl) && (dMeanStartCtr2 > 4))
					{
						//TRACE("\nChecking BEGIN for Index %d MeanCurr %f MeanBegin %f CurrentCV %f PrevCV %f  CurrentStd %f PrevStd %f  CntrlValue %d   \n",
						//	k, fMeanWindowCurr, fMeanMeanStart1, fCoeffOfVar2, fCoeffOfVar1, fStdWindowCurr, fStdWindowPrev, dMeanStartCtr2);

						{
							if (fStdWindowCurr < 0.1)
							{
								dStdSpikeCounterStart++;
							}
							else
							{
								dStdSpikeCounterStart = 0;
							}

							if (fStdWindowCurr == 0.0)
							{
								dStartZeroStdCntr++;
								//TRACE("\n\n\nSTART ZERO STD %d \n\n\n", dStartZeroStdCntr);
							}

							if ((fCoeffOfVar2 > 3000) || (fStdWindowCurr < 0.1))
							{

								if (
										(
										(fStdWindowCurr > (1.0 - dMagicMultiplier) * fStdMeanStart1) &&
										(fStdWindowCurr < (1.0 + dMagicMultiplier) * fStdMeanStart1) &&
										(fStdWindowPrev >(1.0 - dMagicMultiplier) * fStdMeanStart1) &&
										(fStdWindowPrev < (1.0 + dMagicMultiplier) * fStdMeanStart1) &&
										(fStdWindowPrev2 >(1.0 - dMagicMultiplier) * fStdMeanStart1) &&
										(fStdWindowPrev2 < (1.0 + dMagicMultiplier) * fStdMeanStart1) &&
										(fStdWindowPrev3 >(1.0 - dMagicMultiplier) * fStdMeanStart1) &&
										(fStdWindowPrev3 < (1.0 + dMagicMultiplier) * fStdMeanStart1)
										)
									||
										(dStdSpikeCounterStart > dSpikeCtrTresholdStart)
									||
										(dStartZeroStdCntr >= dStartZeroStdThreshold)
								   )
								{
									if ((fCoeffOfVar1 > 2000) || (dStartZeroStdCntr >= dStartZeroStdThreshold))
									{
										bBegIdxFound = FALSE;
										g_dBeginIndex = 0;

										fMeanMeanStart1 = fMeanMeanStart2;
										fStdMeanStart1 = fStdMeanStart2;
										dMeanStartCtr1 = dMeanStartCtr2;

										if (fStartMean != 0.0)
										{
											fMeanMeanStart1 += fStartMean;
											fMeanMeanStart1 /= 2.0;
											fStartMean = 0.0;

											fStdMeanStart1 += fStartStd;
											fStdMeanStart1 /= 2.0;
											fStartStd = 0.0;
										}

										fMeanMeanStart2 = 0.0;
										fStdMeanStart2 = 0.0;
										dMeanStartCtr2 = 0;

										dStartZeroStdCntr = 0;

										dStartCancelCounter++;
										TRACE("\nCancelling Start.... for %d time(s) \n", dStartCancelCounter);
									}
								}
							}
						}

						if ((fMeanWindowCurr > fMeanMeanStart1*(1.0 - dMagicMultiplier2)) && (dStdSpikeCounterStart == 0))
						{
							dThresholdStartControl = 100;
						}

						if ((fMeanWindowCurr > fMeanMeanStart1*(1.0 - dMagicMultiplier3)) && (dStdSpikeCounterStart == 0))
						{
							dThresholdStartControl = 20;
						}
					}

					if ((dMeanStartCtr2 > dThresholdStartControl) && (bBegIdxFound == true))
					{
						bStartPassedTest = true;
					}
					/// end of begin index control


					/// check if end is still valid
					if ((dMeanEndCtr2 < dThresholdEndControl) && (dMeanEndCtr2 > 4))
					{
						//TRACE("\nChecking end for Index %d MeanCurr %f MeanBegin %f CurrentCV %f PrevCV %f  CurrentStd %f PrevStd %f  CntrlValue %d THreshold %d  \n",
						//	k, fMeanWindowCurr, fMeanMeanStart1, fCoeffOfVar2, fCoeffOfVar1, fStdWindowCurr, fStdWindowPrev, dMeanEndCtr2, dThresholdEndControl);
					
						if ((fStdWindowCurr > fStdMeanStart1*1.3) && (fStdWindowCurr > 0.1))
						{
							dStdSpikeCounterEnd++;

							//TRACE("\n!!!!!!!Checking end for Index %d fStdMeanStart1 %f fStdMeanEnd2 %f  Counter %d CntrlValue %d   \n",
							//	k, fStdMeanStart1, fStdWindowCurr, dStdSpikeCounterEnd, dMeanEndCtr2);
						}

						if (fStdWindowCurr < 0.05)
						{
							dSmallStdCounterEnd++;
						}
						if (dSmallStdCounterEnd >= dSmallStdCounterTresholdEnd)
						{
							if (dThresholdEndControl > 20)
							{
								dThresholdEndControl = 20;

								dSmallStdCounterEnd = 0;
								dMeanEndCtr2 = 0;;
							}
						}

						fCVEnd = fMeanMeanEnd2 / fStdMeanEnd2;
						//TRACE("\nEND CV %f\n", fCVEnd);
						if (fCVEnd > (float) 1500.0)
						{
							dCVEndCounter++;
						}
						if (dCVEndCounter > dCVEndCounterThreshold)
						{
							if (dThresholdEndControl > 20)
							{
								dThresholdEndControl = 20;
								//TRACE("\n\n\nEND BIG CV Counter %d \n\n\n", dCVEndCounter);
								dCVEndCounter = 0;
								dMeanEndCtr2 = 0;;
							}
						}

						if (fMeanWindowCurr < fMeanMeanStart1*(1.0 - dMagicMultiplier1))
						{
							dThresholdEndControl++;

							if (
								(fCoeffOfVar2 > 4000) ||
								(fStdWindowCurr > 0.2)
								)
							{
								bEndIdxFound = false;
								g_dEndIndex = 0;

								fMeanMeanEnd1 = fMeanMeanEnd2;
								fStdMeanEnd1 = fStdMeanEnd2;
								dMeanEndCtr1 = dMeanEndCtr2;

								if (fEndMean != 0.0)
								{
									fMeanMeanEnd1 += fEndMean;
									fMeanMeanEnd1 /= 2.0;
									fEndMean = 0.0;

									fStdMeanEnd1 += fEndStd;
									fStdMeanEnd1 /= 2.0;
									fEndStd = 0.0;
								}

								fMeanMeanEnd2 = 0.0;
								fStdMeanEnd2 = 0.0;
								dMeanEndCtr2 = 0;

								dThresholdEndControl = 200;
								dEndCancelCounter++;

								dStdSpikeCounterEnd = 0;
								dSmallStdCounterEnd = 0;
								dCVEndCounter = 0;

								g_dCarDetectCount--;

								TRACE("\nCancelling 1st End.... for %d time(s) for index %d \n", dEndCancelCounter, k);
							}
						}
						else if (dStdSpikeCounterEnd > dSpikeCtrTresholdEnd)
						{
							bEndIdxFound = false;
							g_dEndIndex = 0;

							fMeanMeanEnd1 = fMeanMeanEnd2;
							fStdMeanEnd1 = fStdMeanEnd2;
							dMeanEndCtr1 = dMeanEndCtr2;

							if (fEndMean != 0.0)
							{
								fMeanMeanEnd1 += fEndMean;
								fMeanMeanEnd1 /= 2.0;
								fEndMean = 0.0;

								fStdMeanEnd1 += fEndStd;
								fStdMeanEnd1 /= 2.0;
								fEndStd = 0.0;
							}

							fMeanMeanEnd2 = 0.0;
							fStdMeanEnd2 = 0.0;
							dMeanEndCtr2 = 0;

							dThresholdEndControl = 200;
							dEndCancelCounter++;
							dStdSpikeCounterEnd = 0;
							dSmallStdCounterEnd = 0;
							dCVEndCounter = 0;

							g_dCarDetectCount--;

							TRACE("\nCancelling 2nd End.... for %d time(s) for index %d \n", dEndCancelCounter, k);
						} /// end of END index cancelling


						if ((fMeanWindowCurr > fMeanMeanStart1*(1.0 - dMagicMultiplier2)) && (dStdSpikeCounterEnd == 0))
						{
							dThresholdEndControl = 100;
						}

						if ((fMeanWindowCurr > fMeanMeanStart1*(1.0 - dMagicMultiplier3)) && (dStdSpikeCounterEnd == 0))
						{
							dThresholdEndControl = 20;
						}
					}

					if ((dMeanEndCtr2 > dThresholdEndControl) && (bEndIdxFound == true))
					{
						bEndPassedTest = true;
					}

#ifdef DEBUG_BORA
				fprintf_s(fp, "%lf %lf %lf %lf \n", fMeanWindow, fStdWindow, fCoeffOfVar1, fCoeffOfVar2);
#endif

			//////////////////////

						if (
							  (
								(bBegIdxFound == false) &&
								(fStdWindowCurr > 1.0) &&  // 2,5
								(fStdWindowPrev > 1.0) &&  // 1,0
								(fStdWindowPrev2 > 1.0) &&  // 0,5
								(fStdWindowPrev3 > 1.0) &&  // 0,5
								(k > 300) &&
								(fCoeffOfVar1 < 500.0) &&
								(fCoeffOfVar2 > 10.0)
							  )
						   )
						{
							bBegIdxFound = true;
							g_dBeginIndex = k;

							fMeanMeanStart1 /= dMeanStartCtr1;
							fStdMeanStart1 /= dMeanStartCtr1;
							fStartMean = fMeanMeanStart1;
							fStartStd = fStdMeanStart1;
							TRACE("\n Found Start at %d  ", g_dBeginIndex);
						}


						if (
						      (
								(bEndIdxFound == false) && (bBegIdxFound == true) &&
								(fStdWindowCurr < 0.5) &&
								(fStdWindowPrev < 0.5) &&
								(fStdWindowPrev2 < 0.5) &&
								(fStdWindowPrev3 < 0.5) &&
								(k - g_dBeginIndex > 2000) &&
								(fCoeffOfVar1 > 4000.0)
							  )
							||
							  (
								(bEndIdxFound == false) && (bBegIdxFound == true) &&
								(fStdWindowCurr < 0.1) &&
								(fStdWindowPrev < 0.1) &&
								(fStdWindowPrev2 < 0.1) &&
								(fStdWindowPrev3 < 0.1) &&
								(k - g_dBeginIndex > 2000)
							  )
						    )
						{
							bEndIdxFound = true;
							g_dEndIndex = k;

							g_dCarDetectCount++;

							fMeanMeanEnd1 = fMeanMeanStart2;
							fStdMeanEnd1 = fStdMeanStart2;
							dMeanEndCtr1 = dMeanStartCtr2;

							fMeanMeanEnd1 /= dMeanEndCtr1;
							fStdMeanEnd1 /= dMeanEndCtr1;
							fEndMean = fMeanMeanEnd1;
							fEndStd = fStdMeanEnd1;

							TRACE("\n Found End at %d  ", g_dEndIndex);
						}						
				} //// end of MAIN loop

#ifdef DEBUG_BORA
				fclose(fp);
#endif

				TRACE("\n  bStartPassedTest %d  bEndPassedTest %d \n", bStartPassedTest, bEndPassedTest);


				if ( 
					 (bBegIdxFound == true) && 
					 (bEndIdxFound == true) && 
					 (g_dCarDetectCount == 1) &&
					 (bStartPassedTest == true) &&
					 (bEndPassedTest == true) &&
					 (g_AutoDetect_Type != NO_AUTO_VEHICLE_DETECT)
				   )
				{
					bIsCarFound = true;
					g_CarFound = TRUE;

					TRACE("\n\n\nSTART %d END %d\n\n\n", g_dBeginIndex, g_dEndIndex);


					if (pVehicleDetection->m_isStopSignalled == false)
					{
						pVehicleDetection->m_isStopSignalled = true;
						TRACE("Signal STOPPPPPPPPPPPPPPPP\n");
						SetEvent(g_CameraStopDataRecieveEvent);
					}
				}

				if (g_AutoDetect_Type == NO_AUTO_VEHICLE_DETECT)
				{
					//if ((bBegIdxFound == false) || (bEndIdxFound == false))
					//{
					g_CarFound = FALSE;
					g_dBeginIndex = 0;
					g_dEndIndex = pVehicleDetection->m_dlineCounter - dAnalysisWindow;
					pVehicleDetection->m_isStopSignalled = true;
					//SetEvent(g_CameraStopDataRecieveEvent);
					pView->SendMessage(WM_VEHICLEDETECT_FINISHED, 0, pLparam);

					//}
				}
								
				TRACE("FRM_CNT %d PRS_CNT %d \n", pVehicleDetection->m_dFrameCounter, pVehicleDetection->m_dProcessedFrameCounter);

				pVehicleDetection->m_dProcessedFrameCounter++;


				if (
					   (bIsCarFound == true) ||
					   (pVehicleDetection->m_dFrameCounter - pVehicleDetection->m_dProcessedFrameCounter  == 0)
				   )
				{
					bIsProcessingFinished = true;
					ResetEvent(pVehicleDetection->ProcessDataEvent);  
					SetEvent(pVehicleDetection->ProcessingFinishedEvent);
				}

			}
		}
	} // infite for loop

	delete[] fMean;

	if (inClr.rows != 0 || inClr.cols != 0)
	{
		inClr.release();
	}

	if (inGray.rows != 0 || inGray.cols != 0)
	{
		inGray.release();
	}

	return THREADEXIT_SUCCESS;
}





