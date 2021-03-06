#include "stdafx.h"

#include "ARALGIS.h"
#include "MainFrm.h"
#include "..\\HeaderFiles\\OdroidCommunicator.h"
#include "..\HeaderFiles\OdroidMessageDefinitions.h"

#include ".\\ErrorDisplay\\ThreadSafeQueue\\HeaderFiles\\ThreadSafeQueue.h"


COdroidCommunicator::COdroidCommunicator()
{
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
	m_bClientedAccepted = false;

	WSADATA wsaData;

	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		TRACE("WSAStartup(...) failure COdroidCommunicator::OdroidCommunicatorThread\n");
	}

	///////////////////////////// TIMER ///////////////////////////////////////
	m_hTimer = NULL;

	// Create an unnamed waitable timer.
	m_hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	if (NULL == m_hTimer)
	{
		TRACE(L"CreateWaitableTimer failed in  COdroidCommunicator (%d)\n", GetLastError());
	}
	///////////////////////////// TIMER ///////////////////////////////////////

	bRun = FALSE;
}


COdroidCommunicator::~COdroidCommunicator()
{
	Shutdown();
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	COdroidCommunicator::Start
// 
// DESCRIPTION:	starts the OdroidCommunicator Thread
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// BN            26082003	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
BOOL COdroidCommunicator::Start(CMainFrame* pFrame)
{
	if (bRun)
	{
		TRACE("_beginthreadex(...) failure, for Launch Thread COdroidCommunicator::Start\n");
		return FALSE;
	}


	TRACE("Odroid CommunicatorThread Starting ...\n");

	m_pFrame = pFrame;

	bRun = TRUE;

	ShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// Launch OdroidCommunicator Thread
	ThreadOdroidCommunicator = (HANDLE)_beginthreadex  (NULL,
														0,
														OdroidCommunicatorThread,
														this,
														0,
														&ThreadID
														);
	if (!ThreadOdroidCommunicator)
	{
		TRACE("_beginthreadex(...) failure, ThreadOdroidCommunicator::Start\n");
		return FALSE;
	}

	TRACE("ThreadOdroidCommunicator ThreadID %x ...\n", ThreadID);


	// Launch Comm Thread
	ThreadComm = (HANDLE)_beginthreadex(NULL,
										0,
										CommThread,
										this,
										0,
										&ThreadID);
	if (!ThreadComm)
	{
		TRACE("_beginthreadex(...) failure, ThreadOdroidCommunicator::Start OdroidCommThread\n");
		return FALSE;
	}

	TRACE("OdroidCommThread ThreadID %x ...\n", ThreadID);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	COdroidCommunicator::Shutdown
// 
// DESCRIPTION:	stops the UDP multi-threaded Server
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// BN            26082003	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
BOOL COdroidCommunicator::Shutdown()
{
	if (!bRun)
		return FALSE;

	TRACE("OdroidCommunicator Shutting down ...\n");

	SetEvent(ShutdownEvent);

	// wait for the thread to stop
	WaitForSingleObject(ThreadOdroidCommunicator, INFINITE);
	WaitForSingleObject(ThreadComm, INFINITE);

	CloseHandle(ShutdownEvent);
	CloseHandle(ThreadOdroidCommunicator);
	CloseHandle(ThreadComm);

	closesocket(ClientSocket);
	closesocket(ListenSocket);

	WSACleanup();

	bRun = FALSE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	COdroidCommunicator::OdroidCommunicatorThread
// 
// DESCRIPTION:	thread function for communicating with server 
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// BN            26082003	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
UINT __stdcall COdroidCommunicator::OdroidCommunicatorThread(LPVOID pParam)
{
	COdroidCommunicator *pOdroidCommunicator = (COdroidCommunicator*)pParam;

	WSANETWORKEVENTS	NetworkEvents;
	int result;

	TRACE("Odroid Communicator Thread Started\n");

	CView * pView = pOdroidCommunicator->m_pFrame->GetActiveView();
	LPARAM pLparam;
	pLparam = reinterpret_cast<LPARAM>("ARALGIS");

	struct addrinfo* resultAddr = NULL;
	struct addrinfo hints;
	int iResult;

	BYTE buffer[MESSAGE_LENGTH];
	int dCamId = 0;


	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the address and port
	iResult = getaddrinfo(NULL, g_Odroid_Port, &hints, &resultAddr);
	if (iResult != 0)
	{
		TRACE("getaddrinfo failed with error: %d failure COdroidCommunicator::OdroidCommunicatorThread\n", iResult);
		WSACleanup();
		return THREADEXIT_SUCCESS;
	}

	// Create a SOCKET for connecting to server
	pOdroidCommunicator->ListenSocket = socket(resultAddr->ai_family, resultAddr->ai_socktype, resultAddr->ai_protocol);
	if (pOdroidCommunicator->ListenSocket == INVALID_SOCKET)
	{
		TRACE("socket failed with error: %d failure COdroidCommunicator::OdroidCommunicatorThread\n", WSAGetLastError());
		freeaddrinfo(resultAddr);
		WSACleanup();
		return THREADEXIT_SUCCESS;
	}

	// Setup the TCP listening socket
	iResult = bind(pOdroidCommunicator->ListenSocket, resultAddr->ai_addr, (int)resultAddr->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		TRACE("bind failed with error: %d failure COdroidCommunicator::OdroidCommunicatorThread\n", WSAGetLastError());
		freeaddrinfo(resultAddr);
		closesocket(pOdroidCommunicator->ListenSocket);
		WSACleanup();
		return THREADEXIT_SUCCESS;
	}

	// Set the mode of the socket to be nonblocking
	u_long iMode = 1;
	iResult = ioctlsocket(pOdroidCommunicator->ListenSocket, FIONBIO, &iMode);

	if (iResult == SOCKET_ERROR)
	{
		TRACE("ioctlsocket failed with error: %d failure COdroidCommunicator::OdroidCommunicatorThread\n", WSAGetLastError());
		closesocket(pOdroidCommunicator->ListenSocket);
		freeaddrinfo(resultAddr);
		WSACleanup();
		return THREADEXIT_SUCCESS;
	}

	freeaddrinfo(resultAddr);

	iResult = listen(pOdroidCommunicator->ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		TRACE("listen failed with error: %d failure COdroidCommunicator::OdroidCommunicatorThread\n", WSAGetLastError());
		closesocket(pOdroidCommunicator->ListenSocket);
		WSACleanup();
		return THREADEXIT_SUCCESS;
	}


	// Create Notification Event for Listen Socket
	WSAEVENT NotificationEventListen = WSACreateEvent();
	if (NotificationEventListen == WSA_INVALID_EVENT)
	{
		TRACE("WSACreateEvent(ListenSocket) %d failure for Event COdroidCommunicator::OdroidCommunicatorThread\n",
			WSAGetLastError());
		closesocket(pOdroidCommunicator->ListenSocket);
		return THREADEXIT_SUCCESS;
	}

	// selects the READ, WRITE, ACCEPT, CONNECT and CLOSE operations 
	result = WSAEventSelect(pOdroidCommunicator->ListenSocket, NotificationEventListen, FD_ACCEPT);

	// No longer need server socket
	//closesocket(pOdroidCommunicator->ListenSocket);

	WSAEVENT Handles[2];
	Handles[0] = pOdroidCommunicator->ShutdownEvent;
	Handles[1] = NotificationEventListen;

	WSAEVENT HandlesClient[3];
	HandlesClient[0] = pOdroidCommunicator->ShutdownEvent;
	// HandlesClient[1] is defined below
	HandlesClient[2] = pOdroidCommunicator->m_hTimer;

	// Create Notification Event for Client Socket
	WSAEVENT NotificationEventClient = WSACreateEvent();
	if (NotificationEventClient == WSA_INVALID_EVENT)
	{
		TRACE("WSACreateEvent(NotificationEventClient) %d failure for Event COdroidCommunicator::OdroidCommunicatorThread\n",
			WSAGetLastError());
		closesocket(pOdroidCommunicator->ListenSocket);
		return THREADEXIT_SUCCESS;
	}


	for (;;)
	{
		DWORD EventCaused = WSAWaitForMultipleEvents(2,
													 Handles,
													 FALSE,
													 100,  //WSA_INFINITE, 
													 FALSE);

		if (EventCaused == WAIT_FAILED || EventCaused == WAIT_OBJECT_0)
		{
			if (EventCaused == WAIT_FAILED)
				TRACE("WaitForMultipleObjects(...) %d failure COdroidCommunicator::OdroidCommunicatorThread\n",
				GetLastError());
			TRACE("OdroidCommunicatorThread Shutting Down ...\n");
			closesocket(pOdroidCommunicator->ListenSocket);
			return THREADEXIT_SUCCESS;
		}
		else
		{
			if (WaitForSingleObject(pOdroidCommunicator->ShutdownEvent, 0) == WAIT_OBJECT_0)
			{
				TRACE("OdroidCommunicatorThread Shutting Down normally ...\n");
				closesocket(pOdroidCommunicator->ListenSocket);
				return THREADEXIT_SUCCESS;
			}

			// Check if it is an event coming from Listen Socket
			if (EventCaused == WAIT_OBJECT_0 + 1)
			{
				result = WSAEnumNetworkEvents(pOdroidCommunicator->ListenSocket,
											  NotificationEventListen,
											  &NetworkEvents);

				if (result == SOCKET_ERROR)
				{
					TRACE("WSAEnumNetworkEvents(...) %d failure COdroidCommunicator::OdroidCommunicatorThread\n",
						WSAGetLastError());
					closesocket(pOdroidCommunicator->ListenSocket);
					return THREADEXIT_SUCCESS;
				}

				// data received from Server
				if (NetworkEvents.lNetworkEvents & FD_ACCEPT)
				{
					// Accept a client socket
					pOdroidCommunicator->ClientSocket = accept(pOdroidCommunicator->ListenSocket, NULL, NULL);
					if (pOdroidCommunicator->ClientSocket == INVALID_SOCKET)
					{
						TRACE("accept failed with error: %d failure COdroidCommunicator::OdroidCommunicatorThread\n", WSAGetLastError());
						closesocket(pOdroidCommunicator->ListenSocket);
						WSACleanup();
						return THREADEXIT_SUCCESS;
					}
					pOdroidCommunicator->m_bClientedAccepted = true;
					pView->SendMessage(ODROID_CONNECTION_OK, 0, pLparam);




					// selects the READ, CONNECT and CLOSE operations for Client Socket
					result = WSAEventSelect(pOdroidCommunicator->ClientSocket, NotificationEventClient,
											FD_READ | FD_CLOSE | FD_CONNECT);
					if (result == SOCKET_ERROR)
					{
						TRACE("WSAEventSelect(...) %d failure COdroidCommunicator::OdroidCommunicatorThread\n",
							WSAGetLastError());
						closesocket(pOdroidCommunicator->ClientSocket);
						closesocket(pOdroidCommunicator->ListenSocket);
						return THREADEXIT_SUCCESS;
					}

					HandlesClient[1] = NotificationEventClient;

				}


				while (pOdroidCommunicator->m_bClientedAccepted)
				{

					DWORD EventCaused = WSAWaitForMultipleEvents(3,
						HandlesClient,
						FALSE,
						100,  //WSA_INFINITE, 
						FALSE);

					if (EventCaused == WAIT_FAILED || EventCaused == WAIT_OBJECT_0)
					{
						if (EventCaused == WAIT_FAILED)
							TRACE("WaitForMultipleObjects(...) %d failure COdroidCommunicator::OdroidCommunicatorThread\n",
							GetLastError());
						TRACE("OdroidCommunicatorThread Shutting Down ...\n");
						closesocket(pOdroidCommunicator->ClientSocket);
						closesocket(pOdroidCommunicator->ListenSocket);
						return THREADEXIT_SUCCESS;
					}
					else
					{
						if (WaitForSingleObject(pOdroidCommunicator->ShutdownEvent, 0) == WAIT_OBJECT_0)
						{
							TRACE("OdroidCommunicatorThread Shutting Down normally ...\n");
							closesocket(pOdroidCommunicator->ClientSocket);
							closesocket(pOdroidCommunicator->ListenSocket);
							return THREADEXIT_SUCCESS;
						}

						// Check if it is an event coming from Listen Socket
						if (EventCaused == WAIT_OBJECT_0 + 1)
						{
							result = WSAEnumNetworkEvents(pOdroidCommunicator->ClientSocket,
														  NotificationEventClient,
														  &NetworkEvents);

							if (result == SOCKET_ERROR)
							{
								TRACE("WSAEnumNetworkEvents(...) %d failure COdroidCommunicator::OdroidCommunicatorThread\n",
									WSAGetLastError());
								closesocket(pOdroidCommunicator->ClientSocket);
								closesocket(pOdroidCommunicator->ListenSocket);
								return THREADEXIT_SUCCESS;
							}

							// data received from Server
							if (NetworkEvents.lNetworkEvents & FD_READ)
							{
								int nBytes = recv(pOdroidCommunicator->ClientSocket, (char*)buffer, MESSAGE_LENGTH, NULL);  //// 39321 is calcualted iaw Divit Odroid

								if ((INVALID_SOCKET == nBytes) || (0 == nBytes))
								{
									TRACE("recvfrom(...) failure COdroidCommunicator::OdroidCommunicatorThread\n",
										WSAGetLastError());
									continue;
								}
								else
								{
									// control checksum, if failed do not process message
									if (pOdroidCommunicator->ControlMessage(buffer) == FALSE)
										continue;

									// message is OK.	
									// start processing the received message
									if (buffer[MESSAGE_ID_POS] == MESSAGE_CAR_DETECTED_NO) 
									{
										if (g_PTS_Producer_ID == PTS_BY_DIVIT) 
										{
											g_IsOdroidStartReceived = TRUE;
											SetEvent(g_CameraStartDataRecieveEvent);
										}

										if ( (g_PTS_Producer_ID == PTS_BY_DIVIT) &&
											(g_PTS_Mode == PTS_MODE_TRIGGER))
										{
											SetEvent(g_PTSTriggerEvent);
										}
										
										/// start timer to check if PTS responds within 
										//  a prefeined period
										pOdroidCommunicator->m_iTimerTick = 0;
										__int64 qwDueTime;
										LARGE_INTEGER   liDueTime;
										int tFrameTime = TIMER_PERIOD_IN_MS_FOR_ODROID;

										qwDueTime = -10 * tFrameTime; // -10 is: negative since relative time
										//       : multiplied by 10, since it is in 100 nanoseconds 
										//         and  tFrameTime is in miliseconds

										liDueTime.QuadPart = (long long)qwDueTime;

										//Set a timer to wait for XX seconds.
										if (!SetWaitableTimer(pOdroidCommunicator->m_hTimer, &liDueTime, tFrameTime, NULL, NULL, 0))
										{
											TRACE(L"SetWaitableTimer failed in OdroidCommunicator (%d)\n", GetLastError());
											return 2;
										}
										TRACE(L"Timer Tick in OdroidCommunicator %d\n", pOdroidCommunicator->m_iTimerTick);
										//// end of timer init
									}

									if (buffer[MESSAGE_ID_POS] == MESSAGE_CAR_FINISHED_NO)
									{
										if (g_PTS_Producer_ID == PTS_BY_DIVIT)
										{
											g_IsOdroidStartReceived = FALSE;
											SetEvent(g_CameraStopDataRecieveEvent);
										}

										/// stop timer
										CancelWaitableTimer(pOdroidCommunicator->m_hTimer);
									}

									if (buffer[MESSAGE_ID_POS] == MESSAGE_CAR_SPEED_NO)
									{
										SetEvent(g_CameraChangeSampleRateEvent);
									}

								}
							}

							// we can handle close operations also
							else if (NetworkEvents.lNetworkEvents & FD_CLOSE)
							{ 
								TRACE("OdroidCommunicator Thread Socket Closed\n");

								pOdroidCommunicator->m_bClientedAccepted = false;

								pView->SendMessage(ODROID_CONNECTION_LOST, 0, pLparam);

							} // FD_CLOSE
						} // network event

						else if (EventCaused == WAIT_OBJECT_0 + 2) // time event
						{
							pOdroidCommunicator->m_iTimerTick++;
							TRACE(L"Timer Tick in OdroidCommunicator %d\n", pOdroidCommunicator->m_iTimerTick);

							////////////////////////////////////////////////////////
							if (pOdroidCommunicator->m_iTimerTick == TIMER_SECONDS_ERROR_FOR_PTS)
							{
								if (TIMER_SECONDS_ERROR_FOR_PTS > TIMER_SECONDS_ERROR_FOR_CAR_FINISHED)
								{
									CancelWaitableTimer(pOdroidCommunicator->m_hTimer);
									pOdroidCommunicator->m_iTimerTick = 0;

									SetEvent(g_CameraStopDataRecieveEvent);
									SetEvent(g_StopChangeDetectionEvent);
								}

								if (g_IsAnswerReceivedFromPTS == false)
								{
									ERRORMESSAGETYPE dTmp;
									char* cText = "Plaka Tan�ma Sistemi Tespit Yapamad�. L�tfen Plaka Tan�ma Sistemini Kontrol Ediniz";
									strncpy_s(dTmp._OdroidText, (size_t)(MAX_PATH), cText, (size_t)(MAX_PATH));
									CThreadSafeQueue<_errorMessageType>::getInstance().push(dTmp);
								}
							}
							////////////////////////////////////////////////////////

							if (pOdroidCommunicator->m_iTimerTick == TIMER_SECONDS_ERROR_FOR_CAR_FINISHED)
							{
								if (TIMER_SECONDS_ERROR_FOR_CAR_FINISHED >= TIMER_SECONDS_ERROR_FOR_PTS)
								{
									CancelWaitableTimer(pOdroidCommunicator->m_hTimer);
									pOdroidCommunicator->m_iTimerTick = 0;
								}

								SetEvent(g_CameraStopDataRecieveEvent);
								SetEvent(g_StopChangeDetectionEvent);

								ERRORMESSAGETYPE dTmp;
								char* cText = "Ara� Bitti Sinyali Gelmedi. L�tfen Loop Detekt�rlerini Kontrol Ediniz";
								strncpy_s(dTmp._OdroidText, (size_t)(MAX_PATH), cText, (size_t)(MAX_PATH));
								CThreadSafeQueue<_errorMessageType>::getInstance().push(dTmp);
							}
						}

					}// client socket event
				} // while loop
			} // listen socket event
		} // wait failed
	} // infite for loop

	closesocket(pOdroidCommunicator->ClientSocket);
	closesocket(pOdroidCommunicator->ListenSocket);

	return THREADEXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	COdroidCommunicator::CommThread
// 
// DESCRIPTION:	thread function for generating messages 
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// BN            26082003	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
UINT __stdcall COdroidCommunicator::CommThread(LPVOID pParam)
{
	COdroidCommunicator *pOdroidCommunicator = (COdroidCommunicator*)pParam;

	HANDLE Handles[7];
	Handles[0] = pOdroidCommunicator->ShutdownEvent;
	Handles[1] = g_OdroidOpenBarrierEvent;
	Handles[2] = g_OdroidCloseBarrierEvent;
	Handles[3] = g_OdroidStartHeatingEvent;
	Handles[4] = g_OdroidStopHeatingEvent;
	Handles[5] = g_OdroidStartAlarmEvent;
	Handles[6] = g_OdroidStopAlarmEvent;


	for (;;)
	{
		DWORD EventCaused = WaitForMultipleObjects(7,
			Handles,
			FALSE,
			INFINITE);

		if (EventCaused == WAIT_FAILED || EventCaused == WAIT_OBJECT_0)
		{
			if (EventCaused == WAIT_FAILED)
				TRACE("OdroidCommThread Shutting Down %d ...\n", GetLastError());
			else
				TRACE("OdroidCommThread Shutting Down Normally ...\n");
			return THREADEXIT_SUCCESS;
		}

		else if (EventCaused == WAIT_OBJECT_0 + 1) // send  Odroid open barrier message
		{
			TRACE("OdroidCommThread SendOdroidOpenBarrierMessage Received\n");
			ResetEvent(g_OdroidOpenBarrierEvent);
			if (pOdroidCommunicator->m_bClientedAccepted == true)
			{
				pOdroidCommunicator->SendOdroidOpenBarrierMessage();
			}
		}

		else if (EventCaused == WAIT_OBJECT_0 + 2) // send  Odroid close barrier message
		{
			TRACE("OdroidCommThread SendOdroidCloseBarrierMessage Received\n");
			ResetEvent(g_OdroidCloseBarrierEvent);
			if (pOdroidCommunicator->m_bClientedAccepted == true)
			{
				pOdroidCommunicator->SendOdroidCloseBarrierMessage();
			}
		}

		else if (EventCaused == WAIT_OBJECT_0 + 3) // send  Odroid start heating message
		{
			TRACE("OdroidCommThread SendOdroidStartHeatingMessage Received\n");
			ResetEvent(g_OdroidStartHeatingEvent);
			if (pOdroidCommunicator->m_bClientedAccepted == true)
			{
				pOdroidCommunicator->SendOdroidStartHeatingMessage();
			}
		} 

		else if (EventCaused == WAIT_OBJECT_0 + 4) // send  Odroid stop heating message
		{
			TRACE("OdroidCommThread SendOdroidStopHeatingMessage Received\n");
			ResetEvent(g_OdroidStopHeatingEvent);
			if (pOdroidCommunicator->m_bClientedAccepted == true)
			{
				pOdroidCommunicator->SendOdroidStopHeatingMessage();
			}
		}


		else if (EventCaused == WAIT_OBJECT_0 + 5) // send  Odroid start alarm message
		{
			TRACE("OdroidCommThread SendOdroidStartHeatingMessage Received\n");
			ResetEvent(g_OdroidStartAlarmEvent);
			if (pOdroidCommunicator->m_bClientedAccepted == true)
			{
				pOdroidCommunicator->SendOdroidStartAlarmMessage();
			}
		}

		else if (EventCaused == WAIT_OBJECT_0 + 6) // send  Odroid stop alarm message
		{
			TRACE("OdroidCommThread SendOdroidStopHeatingMessage Received\n");
			ResetEvent(g_OdroidStopAlarmEvent);
			if (pOdroidCommunicator->m_bClientedAccepted == true)
			{
				pOdroidCommunicator->SendOdroidStopAlarmMessage();
			}
		}
	}

	return THREADEXIT_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	COdroidCommunicator::ControlMessage
// 
// DESCRIPTION:	Controls checksum and magic number 
//                if they are not correct, discards the message
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// BN            26082003	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
BOOL COdroidCommunicator::ControlMessage(BYTE *message)
{
	BOOL retVal = FALSE;

	if ((int) message[MESSAGE_FROM_POS] != (int) ODROID_MESSAGE_INDICATOR)
		return FALSE;

	if ( ( (int) message[MESSAGE_ID_POS] == (int) MESSAGE_CAR_DETECTED_NO) ||
		 ( (int) message[MESSAGE_ID_POS] == (int) MESSAGE_CAR_FINISHED_NO) ||
		 ( (int) message[MESSAGE_ID_POS] == (int) MESSAGE_CAR_SPEED_NO) )
		 retVal = TRUE;
	else
		retVal = FALSE;

	if ( (int) message[MESSAGE_RESERVED1_POS] != (int) 0XFf)
		return FALSE;
	if ( (int) message[MESSAGE_RESERVED1_POS+1] != (int)0XFF)
		return FALSE;

	if ( ( (int) message[MESSAGE_ID_POS]      != (int) MESSAGE_CAR_SPEED_NO) &&
	     ( (int) message[MESSAGE_VAL_POS]     != (int) 0XFF) && 
	     ( (int) message[MESSAGE_VAL_POS + 1] != (int) 0XFF) )
		return FALSE;

	if ( (int) message[MESSAGE_RESERVED2_POS] != (int) 0X0)
		return FALSE;
	if ( (int) message[MESSAGE_RESERVED2_POS + 1] != (int) 0X0)
		return FALSE;

	//int a1, a2, a3, a4, a5, a6, a7, a8;
	//a1 = (int)message[TIME_POS];
	//a2 = (int)message[TIME_POS + 1];
	//a3 = (int)message[TIME_POS + 2];
	//a4 = (int)message[TIME_POS + 3];
	//a5 = (int)message[TIME_POS + 4];
	//a6 = (int)message[TIME_POS + 5];
	//a7 = (int)message[TIME_POS + 6];
	//a8 = (int)message[TIME_POS + 7];

	//int c = (int)message[MESSAGE_ID_POS];


	if (((int)message[MESSAGE_ID_POS] != (int)MESSAGE_CAR_SPEED_NO))
	{
		if (((int)message[TIME_POS] == (int)0XFF) &&
			((int)message[TIME_POS + 1] == (int)0XFF) &&
			((int)message[TIME_POS + 2] == (int)0XFF) &&
			((int)message[TIME_POS + 3] == (int)0XFF) &&
			((int)message[TIME_POS + 4] == (int)0XFF) &&
			((int)message[TIME_POS + 5] == (int)0XFF) &&
			((int)message[TIME_POS + 6] == (int)0XFF) &&
			((int)message[TIME_POS + 7] == (int)0XFF))
		{
			retVal = TRUE;
		}
		else
		{
			retVal = FALSE;
		}
	}

	return retVal; 
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	COdroidCommunicator::SendOdroidOpenBarrierMessage
// 
// DESCRIPTION:	Sends message open barrier
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// BN              10042017	   1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
void COdroidCommunicator::SendOdroidOpenBarrierMessage()
{
	int                 iSent;
	unsigned char       data[MESSAGE_LENGTH];
	MessageOdroid       messageToSend;

	messageToSend.messageFrom = ARALGIS_MESSAGE_INDICATOR;
	messageToSend.messageID = MESSAGE_OPEN_BARRIER_NO;
	messageToSend.reserved1[0] = 0xFF;
	messageToSend.reserved1[1] = 0xFF;
	messageToSend.valueContent[0] = 0xFF;
	messageToSend.valueContent[1] = 0xFF;
	messageToSend.reserved2[0] = 0x0;
	messageToSend.reserved2[1] = 0x0;

	for (int i = 0; i < TIME_SIZE; i++)
		messageToSend.messageTime[i] = 0xFF;

	memcpy_s(data, size_t(MESSAGE_LENGTH), &messageToSend, size_t(MESSAGE_LENGTH));
	
	iSent = send(ClientSocket, (char*)data, (int)MESSAGE_LENGTH, NULL);

	// check errors
	if (iSent == SOCKET_ERROR)
	{
		LPVOID lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR)&lpMsgBuf,
			0,
			NULL);

		// Display the string.
		::MessageBox(NULL, (LPTSTR)lpMsgBuf, (LPCWSTR)WARNINGWINDOW_TITLE, MB_OK | MB_ICONINFORMATION);
		// Free the buffer.
		LocalFree(lpMsgBuf);
	}
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	COdroidCommunicator::SendOdroidCloseBarrierMessage
// 
// DESCRIPTION:	Sends message close barrier
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// BN              10042017	   1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
void COdroidCommunicator::SendOdroidCloseBarrierMessage()
{
	int                 iSent;
	unsigned char       data[MESSAGE_LENGTH];
	MessageOdroid       messageToSend;

	messageToSend.messageFrom = ARALGIS_MESSAGE_INDICATOR;
	messageToSend.messageID = MESSAGE_CLOSE_BARRIRER_NO;
	messageToSend.reserved1[0] = 0xFF;
	messageToSend.reserved1[1] = 0xFF;
	messageToSend.valueContent[0] = 0xFF;
	messageToSend.valueContent[1] = 0xFF;
	messageToSend.reserved2[0] = 0x0;
	messageToSend.reserved2[1] = 0x0;

	for (int i = 0; i < TIME_SIZE; i++)
		messageToSend.messageTime[i] = 0xFF;

	memcpy_s(data, size_t(MESSAGE_LENGTH), &messageToSend, size_t(MESSAGE_LENGTH));

	iSent = send(ClientSocket, (char*)data, (int)MESSAGE_LENGTH, NULL);

	// check errors
	if (iSent == SOCKET_ERROR)
	{
		LPVOID lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR)&lpMsgBuf,
			0,
			NULL);

		// Display the string.
		::MessageBox(NULL, (LPTSTR)lpMsgBuf, (LPCWSTR)WARNINGWINDOW_TITLE, MB_OK | MB_ICONINFORMATION);
		// Free the buffer.
		LocalFree(lpMsgBuf);
	}
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	COdroidCommunicator::SendOdroidStartHeatingMessage
// 
// DESCRIPTION:	Sends message start heating
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// BN              10042017	   1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
void COdroidCommunicator::SendOdroidStartHeatingMessage()
{
	int                 iSent;
	unsigned char       data[MESSAGE_LENGTH];
	MessageOdroid       messageToSend;

	messageToSend.messageFrom = ARALGIS_MESSAGE_INDICATOR;
	messageToSend.messageID = MESSAGE_START_HEATING_NO;
	messageToSend.reserved1[0] = 0xFF;
	messageToSend.reserved1[1] = 0xFF;
	messageToSend.valueContent[0] = 0xFF;
	messageToSend.valueContent[1] = 0xFF;
	messageToSend.reserved2[0] = 0x0;
	messageToSend.reserved2[1] = 0x0;

	for (int i = 0; i < TIME_SIZE; i++)
		messageToSend.messageTime[i] = 0xFF;

	memcpy_s(data, size_t(MESSAGE_LENGTH), &messageToSend, size_t(MESSAGE_LENGTH));

	iSent = send(ClientSocket, (char*)data, (int)MESSAGE_LENGTH, NULL);

	// check errors
	if (iSent == SOCKET_ERROR)
	{
		LPVOID lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR)&lpMsgBuf,
			0,
			NULL);

		// Display the string.
		::MessageBox(NULL, (LPTSTR)lpMsgBuf, (LPCWSTR)WARNINGWINDOW_TITLE, MB_OK | MB_ICONINFORMATION);
		// Free the buffer.
		LocalFree(lpMsgBuf);
	}
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	COdroidCommunicator::SendOdroidStopHeatingMessage
// 
// DESCRIPTION:	Sends message stop heating
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// BN              10042017	   1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
void COdroidCommunicator::SendOdroidStopHeatingMessage()
{
	int                 iSent;
	unsigned char       data[MESSAGE_LENGTH];
	MessageOdroid       messageToSend;

	messageToSend.messageFrom = ARALGIS_MESSAGE_INDICATOR;
	messageToSend.messageID = MESSAGE_STOP_HEATING_NO;
	messageToSend.reserved1[0] = 0xFF;
	messageToSend.reserved1[1] = 0xFF;
	messageToSend.valueContent[0] = 0xFF;
	messageToSend.valueContent[1] = 0xFF;
	messageToSend.reserved2[0] = 0x0;
	messageToSend.reserved2[1] = 0x0;

	for (int i = 0; i < TIME_SIZE; i++)
		messageToSend.messageTime[i] = 0xFF;

	memcpy_s(data, size_t(MESSAGE_LENGTH), &messageToSend, size_t(MESSAGE_LENGTH));

	iSent = send(ClientSocket, (char*)data, (int)MESSAGE_LENGTH, NULL);

	// check errors
	if (iSent == SOCKET_ERROR)
	{
		LPVOID lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR)&lpMsgBuf,
			0,
			NULL);

		// Display the string.
		::MessageBox(NULL, (LPTSTR)lpMsgBuf, (LPCWSTR)WARNINGWINDOW_TITLE, MB_OK | MB_ICONINFORMATION);
		// Free the buffer.
		LocalFree(lpMsgBuf);
	}
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	COdroidCommunicator::SendOdroidStartAlarmMessage
// 
// DESCRIPTION:	Sends message start alarm
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// BN              10042017	   1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
void COdroidCommunicator::SendOdroidStartAlarmMessage()
{
	int                 iSent;
	unsigned char       data[MESSAGE_LENGTH];
	MessageOdroid       messageToSend;

	messageToSend.messageFrom = ARALGIS_MESSAGE_INDICATOR;
	messageToSend.messageID = MESSAGE_START_ALARM_NO;
	messageToSend.reserved1[0] = 0xFF;
	messageToSend.reserved1[1] = 0xFF;
	messageToSend.valueContent[0] = 0xFF;
	messageToSend.valueContent[1] = 0xFF;
	messageToSend.reserved2[0] = 0x0;
	messageToSend.reserved2[1] = 0x0;

	for (int i = 0; i < TIME_SIZE; i++)
		messageToSend.messageTime[i] = 0xFF;

	memcpy_s(data, size_t(MESSAGE_LENGTH), &messageToSend, size_t(MESSAGE_LENGTH));

	iSent = send(ClientSocket, (char*)data, (int)MESSAGE_LENGTH, NULL);

	// check errors
	if (iSent == SOCKET_ERROR)
	{
		LPVOID lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR)&lpMsgBuf,
			0,
			NULL);

		// Display the string.
		::MessageBox(NULL, (LPTSTR)lpMsgBuf, (LPCWSTR)WARNINGWINDOW_TITLE, MB_OK | MB_ICONINFORMATION);
		// Free the buffer.
		LocalFree(lpMsgBuf);
	}
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	COdroidCommunicator::SendOdroidStopAlarmMessage
// 
// DESCRIPTION:	Sends message stop heating
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// BN              10042017	   1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
void COdroidCommunicator::SendOdroidStopAlarmMessage()
{
	int                 iSent;
	unsigned char       data[MESSAGE_LENGTH];
	MessageOdroid       messageToSend;

	messageToSend.messageFrom = ARALGIS_MESSAGE_INDICATOR;
	messageToSend.messageID = MESSAGE_STOP_ALARM_NO;
	messageToSend.reserved1[0] = 0xFF;
	messageToSend.reserved1[1] = 0xFF;
	messageToSend.valueContent[0] = 0xFF;
	messageToSend.valueContent[1] = 0xFF;
	messageToSend.reserved2[0] = 0x0;
	messageToSend.reserved2[1] = 0x0;

	for (int i = 0; i < TIME_SIZE; i++)
		messageToSend.messageTime[i] = 0xFF;

	memcpy_s(data, size_t(MESSAGE_LENGTH), &messageToSend, size_t(MESSAGE_LENGTH));

	iSent = send(ClientSocket, (char*)data, (int)MESSAGE_LENGTH, NULL);

	// check errors
	if (iSent == SOCKET_ERROR)
	{
		LPVOID lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR)&lpMsgBuf,
			0,
			NULL);

		// Display the string.
		::MessageBox(NULL, (LPTSTR)lpMsgBuf, (LPCWSTR)WARNINGWINDOW_TITLE, MB_OK | MB_ICONINFORMATION);
		// Free the buffer.
		LocalFree(lpMsgBuf);
	}
}

