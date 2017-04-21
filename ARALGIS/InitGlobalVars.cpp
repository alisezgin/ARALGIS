// InitGlobalVars.cpp: Global Variable initilization and deletion
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ARALGIS.h"
#include "InitGlobalVars.h"
#include "Constants.h"


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	InitializeGlobalVars
// 
// DESCRIPTION:	Initializes the global variables.
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// BN            26062003	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
void InitializeGlobalVars()
{

	TimerNormalizer = (int)(1000 / TIMER_PERIOD_IN_MS);
	threadSyncCnt = 0;

	InitializeCriticalSection(&g_DBaseHandlingCS);

	g_CameraStartDataRecieveEvent =			CreateEvent(NULL, TRUE, FALSE, NULL);
	g_CameraStopDataRecieveEvent =			CreateEvent(NULL, TRUE, FALSE, NULL);
	g_CameraChangeSampleRateEvent =			 CreateEvent(NULL, TRUE, FALSE, NULL);
	g_CameraPauseDataRecieveEvent =			CreateEvent(NULL, TRUE, FALSE, NULL);
	g_CameraConfigFileChangeEvent =			CreateEvent(NULL, TRUE, FALSE, NULL);
	g_CameraSelectStreamFileEvent =			CreateEvent(NULL, TRUE, FALSE, NULL);
	g_CameraUpdateControlsEvent =			CreateEvent(NULL, TRUE, FALSE, NULL);
	g_DisplayBitmapEvent =					CreateEvent(NULL, TRUE, FALSE, NULL);
	g_DisplayOpenCVEvent =					CreateEvent(NULL, TRUE, FALSE, NULL);
	g_SetTimerFrameRateEvent =				CreateEvent(NULL, TRUE, FALSE, NULL);
	g_KillTimerEvent =						CreateEvent(NULL, TRUE, FALSE, NULL);
	g_ResetTimerEvent =						CreateEvent(NULL, TRUE, FALSE, NULL);
	g_OdroidOpenBarrierEvent =				CreateEvent(NULL, TRUE, FALSE, NULL);
	g_OdroidCloseBarrierEvent =				CreateEvent(NULL, TRUE, FALSE, NULL);
	g_OdroidStartHeatingEvent =				CreateEvent(NULL, TRUE, FALSE, NULL);
	g_OdroidStopHeatingEvent =				CreateEvent(NULL, TRUE, FALSE, NULL);
	g_OdroidStartAlarmEvent =				CreateEvent(NULL, TRUE, FALSE, NULL);
	g_OdroidStopAlarmEvent =				CreateEvent(NULL, TRUE, FALSE, NULL);
	g_PTSTriggerEvent =						CreateEvent(NULL, TRUE, FALSE, NULL);
	g_PTSLostEvent =						CreateEvent(NULL, TRUE, FALSE, NULL);
	g_CameraDBServerPlakaDataReadyEvent =	CreateEvent(NULL, TRUE, FALSE, NULL);
	g_ProcessFilter1Event =					CreateEvent(NULL, TRUE, FALSE, NULL);
	g_ProcessFilter2Event =					CreateEvent(NULL, TRUE, FALSE, NULL);
	g_ProcessFilter3Event =					CreateEvent(NULL, TRUE, FALSE, NULL);

	
	strncpy_s(g_PtsIP, PTS_IP_ADDRESS, IP_ADDRESS_LEN);
	strncpy_s(g_PlakaNoChars, "          ", PLAKA_CHAR_LENGTH);
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	DeleteGlobalVariables
// 
// DESCRIPTION:	Deletes objects of the global variables.
////////////////////////////////////////////////////////////////////////////////
void DeleteGlobalVariables()
{
	DeleteCriticalSection(&g_DBaseHandlingCS);

	CloseHandle(g_CameraStartDataRecieveEvent);
	CloseHandle(g_CameraStopDataRecieveEvent);
	CloseHandle(g_CameraChangeSampleRateEvent);
	CloseHandle(g_CameraPauseDataRecieveEvent); 
	CloseHandle(g_CameraConfigFileChangeEvent);
	CloseHandle(g_CameraSelectStreamFileEvent); 
	CloseHandle(g_CameraUpdateControlsEvent);
	CloseHandle(g_DisplayBitmapEvent);
	CloseHandle(g_DisplayOpenCVEvent);
	CloseHandle(g_SetTimerFrameRateEvent);
	CloseHandle(g_KillTimerEvent);
	CloseHandle(g_ResetTimerEvent);
	CloseHandle(g_OdroidOpenBarrierEvent);
	CloseHandle(g_OdroidCloseBarrierEvent);
	CloseHandle(g_OdroidStartHeatingEvent);
	CloseHandle(g_OdroidStopHeatingEvent);
	CloseHandle(g_OdroidStartAlarmEvent);
	CloseHandle(g_OdroidStopAlarmEvent);
	CloseHandle(g_PTSTriggerEvent);
	CloseHandle(g_PTSLostEvent);
	CloseHandle(g_CameraDBServerPlakaDataReadyEvent);
	CloseHandle(g_ProcessFilter1Event);
	CloseHandle(g_ProcessFilter2Event);
	CloseHandle(g_ProcessFilter3Event);
}
