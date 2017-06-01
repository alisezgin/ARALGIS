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

	g_carsDetectedByPTSCnt = 0;

	InitializeCriticalSection(&g_IntermediateTestImgCS);

	g_CameraStartDataRecieveEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_CameraStopDataRecieveEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_CameraChangeSampleRateEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_SetTimerFrameRateEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_KillTimerEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_OdroidOpenBarrierEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_OdroidCloseBarrierEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_OdroidStartHeatingEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_OdroidStopHeatingEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_OdroidStartAlarmEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_OdroidStopAlarmEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	SendUVSSImageEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_PTSLostEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_CameraDBServerPlakaDataReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_ProcessFilter1Event = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_ProcessFilter2Event = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_ProcessFilter3Event = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_ProcessFilter4Event = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_ProcessFilter5Event = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_ProcessFilter6Event = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_IntermediateImageReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL); 
	g_StartChangeDetectEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	strncpy_s(g_PlakaNoChars, "          ", PLAKA_CHAR_LENGTH);

	InitializeCriticalSection(&RefImageCS);
	InitializeCriticalSection(&TestImageCS);
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	DeleteGlobalVariables
// 
// DESCRIPTION:	Deletes objects of the global variables.
////////////////////////////////////////////////////////////////////////////////
void DeleteGlobalVariables()
{
	DeleteCriticalSection(&g_IntermediateTestImgCS);

	CloseHandle(g_CameraStartDataRecieveEvent);
	CloseHandle(g_CameraStopDataRecieveEvent);
	CloseHandle(g_CameraChangeSampleRateEvent);
	CloseHandle(g_SetTimerFrameRateEvent);
	CloseHandle(g_KillTimerEvent);
	CloseHandle(g_OdroidOpenBarrierEvent);
	CloseHandle(g_OdroidCloseBarrierEvent);
	CloseHandle(g_OdroidStartHeatingEvent);
	CloseHandle(g_OdroidStopHeatingEvent);
	CloseHandle(g_OdroidStartAlarmEvent);
	CloseHandle(g_OdroidStopAlarmEvent);
	CloseHandle(SendUVSSImageEvent);
	CloseHandle(g_PTSLostEvent);
	CloseHandle(g_CameraDBServerPlakaDataReadyEvent);
	CloseHandle(g_ProcessFilter1Event);
	CloseHandle(g_ProcessFilter2Event);
	CloseHandle(g_ProcessFilter3Event);
	CloseHandle(g_ProcessFilter4Event);
	CloseHandle(g_ProcessFilter5Event);
	CloseHandle(g_ProcessFilter6Event);
	CloseHandle(g_IntermediateImageReadyEvent); 
	CloseHandle(g_StartChangeDetectEvent);

	DeleteCriticalSection(&RefImageCS);
	DeleteCriticalSection(&TestImageCS);
}
