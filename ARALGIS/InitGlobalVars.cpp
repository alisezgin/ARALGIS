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
	int i;

	for (i = 0; i <= MAX_BORA; i++)
	{
		//AccountDataReceivedEvent[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
		//ResponseReceivedFromTerminal[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
		//PrintingThreadsTimerEvent[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
		//CopyThreadsTimerEvent[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
		//TerminalIsAliveAgain[i] = CreateEvent(NULL, TRUE, FALSE, NULL);

		//TerminalFirmwareBlockReadyForDownloadEvent[i] =
		//	CreateEvent(NULL, TRUE, FALSE, NULL);
		//TerminalFirmwareEraseChipEvent[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
		//TerminalFirmwareBlockDownloadedEvent[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
		//TerminalFirmwareDownloadFinishedEvent[i] = CreateEvent(NULL, TRUE, FALSE, NULL);

		//PrinterDeletedEvent[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
		//DeleteTerminalEvent[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
		//FirmwareDownloadTimeoutEvent[i] = CreateEvent(NULL, TRUE, FALSE, NULL);

		//SendMailEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

		//ActiveJobInfo[i].userID = 0;
	}






	TimerNormalizer = (int)(1000 / TIMER_PERIOD_IN_MS);



	InitializeCriticalSection(&g_SomeHandlingCS);


	g_CameraStartDataRecieveEvent =		CreateEvent(NULL, TRUE, FALSE, NULL);
	g_CameraStopDataRecieveEvent =		CreateEvent(NULL, TRUE, FALSE, NULL);
	g_CameraChangeSampleRateEvent =     CreateEvent(NULL, TRUE, FALSE, NULL);
	g_CameraPauseDataRecieveEvent =		CreateEvent(NULL, TRUE, FALSE, NULL);
	g_CameraConfigFileChangeEvent =		CreateEvent(NULL, TRUE, FALSE, NULL);
	g_CameraSelectStreamFileEvent =		CreateEvent(NULL, TRUE, FALSE, NULL);
	g_CameraUpdateControlsEvent =		CreateEvent(NULL, TRUE, FALSE, NULL);
	g_DisplayBitmapEvent =				CreateEvent(NULL, TRUE, FALSE, NULL);
	g_DisplayOpenCVEvent =				CreateEvent(NULL, TRUE, FALSE, NULL);
	g_SetTimerFrameRateEvent =			CreateEvent(NULL, TRUE, FALSE, NULL);
	g_KillTimerEvent =					CreateEvent(NULL, TRUE, FALSE, NULL);
	g_ResetTimerEvent =					CreateEvent(NULL, TRUE, FALSE, NULL);
	g_OdroidOpenBarrierEvent =			CreateEvent(NULL, TRUE, FALSE, NULL);
	g_OdroidCloseBarrierEvent =			CreateEvent(NULL, TRUE, FALSE, NULL);
	g_OdroidStartHeatingEvent =			CreateEvent(NULL, TRUE, FALSE, NULL);
	g_OdroidStopHeatingEvent =			CreateEvent(NULL, TRUE, FALSE, NULL);


	//g_CameraDataReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);


	threadSyncCnt = 0;


	//isDataBaseDirSet = FALSE;
	//periodFilePurge = (UINT)((int)(24 / NUM_PURGE_PER_DAY) * 60);
	//minDiskSpace = CRITICAL_FREE_SPACE_AMOUNT;
	//periodDiskSpaceControl = TIME_TO_CONTROL_HARDDISK;
	//dbaseCapacityThreshold = CRITICAL_DBASE_SIZE_PERCENT;
	//periodDatabaseCapacity = PERIOD_DATABASE_CONTROL;
	//dbaseCompactHour = PRINTJOB_DBASE_COMPACT_HOUR;
	//dbaseCompactMinute = PRINTJOB_DBASE_COMPACT_MINUTE;
	//dbaseAccountCompactHour = ACCOUNT_DBASE_COMPACT_HOUR;
	//dbaseAccountCompactMinute = ACCOUNT_DBASE_COMPACT_MINUTE;


	g_PTSTriggerEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_PTSLostEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

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
	DeleteCriticalSection(&g_SomeHandlingCS);

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
	//CloseHandle(g_CameraDataReadyEvent);
	CloseHandle(g_OdroidOpenBarrierEvent);
	CloseHandle(g_OdroidCloseBarrierEvent);
	CloseHandle(g_OdroidStartHeatingEvent);
	CloseHandle(g_OdroidStopHeatingEvent);

	CloseHandle(g_PTSTriggerEvent);
	CloseHandle(g_PTSLostEvent);


}



