#include "stdafx.h"
#include "ARALGIS.h"
#include "..\HeaderFiles\PTSCommunicatorDVIT.h"
#include "MainFrm.h"
#include "..\HeaderFiles\PTSMessageDefinitionsDVIT.h"




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicatorDVIT::CPTSCommunicatorDVIT
// 
// DESCRIPTION:	C'tor initializes Winsock2 and miscelleanous events etc.
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
CPTSCommunicatorDVIT::CPTSCommunicatorDVIT()
{
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
	m_bClientedAccepted = false;

	WSADATA wsaData;

   int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
   if(result != 0)
	{
		TRACE("WSAStartup(...) failure CPTSCommunicatorDVIT::PTSCommunicatorThread\n");
	}

	bRun = FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicatorDVIT::CPTSCommunicatorDVIT
// 
// DESCRIPTION:	D'tor
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
CPTSCommunicatorDVIT::~CPTSCommunicatorDVIT()
{
	Shutdown();	
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicatorDVIT::Start
// 
// DESCRIPTION:	starts the PTSCommunicator Thread
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
BOOL CPTSCommunicatorDVIT::Start(CMainFrame* pFrame)
{
	if(bRun)
	{
		TRACE("_beginthreadex(...) failure, for Launch Thread CPTSCommunicatorDVIT::Start\n");
		return FALSE;
	}


	TRACE("PTS CommunicatorThread Starting ...\n");

	m_pFrame	    = pFrame;

	ShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// Launch PTSCommunicator Thread
	ThreadPTSCommunicator = (HANDLE)_beginthreadex( NULL,
													0, 
													PTSCommunicatorThread,
													this, 
													0, 
													&ThreadID
													);
	if (!ThreadPTSCommunicator)
	{
		TRACE("_beginthreadex(...) failure, ThreadPTSCommunicator::Start\n");
		return FALSE;
	}

	TRACE("ThreadPTSCommunicator ThreadID %x ...\n", ThreadID);


	// Launch Comm Thread
	ThreadComm = (HANDLE) _beginthreadex( NULL, 
		                                  0, 
										  CommThread, 
										  this, 
										  0, 
										  &ThreadID );
	if(!ThreadComm)
	{
		TRACE("_beginthreadex(...) failure, PTSCommunicator::Start PTSCommThread\n");
		return FALSE;
	}

	TRACE("PTSCommThread ThreadID %x ...\n", ThreadID);

	bRun = TRUE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicatorDVIT::Shutdown
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
BOOL CPTSCommunicatorDVIT::Shutdown()
{
   if(!bRun)
      return FALSE;

   TRACE("PTSCommunicator Shutting down ...\n");
	
   SetEvent(ShutdownEvent);

	// wait for the thread to stop
   WaitForSingleObject(ThreadPTSCommunicator, INFINITE);
   WaitForSingleObject(ThreadComm, INFINITE);

   CloseHandle(ShutdownEvent);
   CloseHandle(ThreadPTSCommunicator);
   CloseHandle(ThreadComm);

   closesocket(ListenSocket);
   closesocket(ClientSocket);

	WSACleanup();

	bRun = FALSE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicatorDVIT::PTSCommunicatorThread
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
UINT __stdcall CPTSCommunicatorDVIT::PTSCommunicatorThread(LPVOID pParam)
{
	CPTSCommunicatorDVIT *pPTSCommunicator = (CPTSCommunicatorDVIT*)pParam;

	WSANETWORKEVENTS	NetworkEvents;
	int result;

	CView * pView = pPTSCommunicator->m_pFrame->GetActiveView();
	LPARAM pLparam;
	pLparam = reinterpret_cast<LPARAM>("ARALGIS");

	struct addrinfo* resultAddr = NULL;
	struct addrinfo hints;
	int iResult;

	BYTE buffer[15];
	int dCamId = 0;
	PTSDataType ptsData;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the address and port
	iResult = getaddrinfo(NULL, g_PTSPort, &hints, &resultAddr);
	if ( iResult != 0 ) 
	{
		TRACE("getaddrinfo failed with error: %d failure CPTSCommunicatorDVIT::PTSCommunicatorThread\n", iResult);	
		WSACleanup();
		return THREADEXIT_SUCCESS;
	}

	// Create a SOCKET for connecting to server
	pPTSCommunicator->ListenSocket = socket(resultAddr->ai_family, resultAddr->ai_socktype, resultAddr->ai_protocol);
	if (pPTSCommunicator->ListenSocket == INVALID_SOCKET)
	{
		TRACE("socket failed with error: %d failure CPTSCommunicatorDVIT::PTSCommunicatorThread\n", WSAGetLastError());
		freeaddrinfo(resultAddr);
		WSACleanup();
		return THREADEXIT_SUCCESS;
	}

	// Setup the TCP listening socket
	iResult = bind(pPTSCommunicator->ListenSocket, resultAddr->ai_addr, (int)resultAddr->ai_addrlen);
	if (iResult == SOCKET_ERROR) 
	{
		TRACE("bind failed with error: %d failure CPTSCommunicatorDVIT::PTSCommunicatorThread\n", WSAGetLastError());
		freeaddrinfo(resultAddr);
		closesocket(pPTSCommunicator->ListenSocket);
		WSACleanup();
		return THREADEXIT_SUCCESS;
	}

	// Set the mode of the socket to be nonblocking
	u_long iMode = 1;
	iResult = ioctlsocket(pPTSCommunicator->ListenSocket, FIONBIO, &iMode);

	if (iResult == SOCKET_ERROR) 
	{
		TRACE("ioctlsocket failed with error: %d failure CPTSCommunicatorDVIT::PTSCommunicatorThread\n", WSAGetLastError());
		closesocket(pPTSCommunicator->ListenSocket);
		freeaddrinfo(resultAddr);
		WSACleanup();
		exit(1);
	}

	freeaddrinfo(resultAddr);

	iResult = listen(pPTSCommunicator->ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) 
	{
		TRACE("listen failed with error: %d failure CPTSCommunicatorDVIT::PTSCommunicatorThread\n", WSAGetLastError());
		closesocket(pPTSCommunicator->ListenSocket);
		WSACleanup();
		return THREADEXIT_SUCCESS;
	}


	// Create Notification Event for Listen Socket
	WSAEVENT NotificationEventListen = WSACreateEvent();
	if (NotificationEventListen == WSA_INVALID_EVENT)
	{
		TRACE("WSACreateEvent(ListenSocket) %d failure for Event CPTSCommunicatorDVIT::PTSCommunicatorThread\n",
			WSAGetLastError());
		closesocket(pPTSCommunicator->ListenSocket);
		return THREADEXIT_SUCCESS;
	}

	// selects the READ, WRITE, ACCEPT, CONNECT and CLOSE operations 
	result = WSAEventSelect(pPTSCommunicator->ListenSocket, NotificationEventListen, FD_ACCEPT);

	// No longer need server socket
	//closesocket(pPTSCommunicator->ListenSocket);

	WSAEVENT Handles[2];
	Handles[0] = pPTSCommunicator->ShutdownEvent;
	Handles[1] = NotificationEventListen;

	WSAEVENT HandlesClient[2];
	HandlesClient[0] = pPTSCommunicator->ShutdownEvent;

	// Create Notification Event for Client Socket
	WSAEVENT NotificationEventClient = WSACreateEvent();
	if (NotificationEventClient == WSA_INVALID_EVENT)
	{
		TRACE("WSACreateEvent(NotificationEventClient) %d failure for Event CPTSCommunicatorDVIT::PTSCommunicatorThread\n",
			WSAGetLastError());
		closesocket(pPTSCommunicator->ListenSocket);
		return THREADEXIT_SUCCESS;
	}


	for(;;)
	{
		DWORD EventCaused = WSAWaitForMultipleEvents( 2,
													  Handles,  
													  FALSE,                  
													  100,  //WSA_INFINITE, 
													  FALSE);

		if(EventCaused == WAIT_FAILED)
		{
			if(EventCaused == WAIT_FAILED)
				TRACE( "WaitForMultipleObjects(...) %d failure CPTSCommunicatorDVIT::PTSCommunicatorThread\n", 
				       GetLastError());
			TRACE( "PTSCommunicatorThread Shutting Down ...\n");
			closesocket(pPTSCommunicator->ListenSocket);
			return THREADEXIT_SUCCESS;
		}
		else
		{
			if(WaitForSingleObject(pPTSCommunicator->ShutdownEvent, 0) == WAIT_OBJECT_0)
			{
				TRACE( "PTSCommunicatorThread Shutting Down normally ...\n");
				closesocket(pPTSCommunicator->ListenSocket);
				return THREADEXIT_SUCCESS;
			}

			// Check if it is an event coming from Listen Socket
			if (EventCaused == WAIT_OBJECT_0 + 1)
			{
				result = WSAEnumNetworkEvents(pPTSCommunicator->ListenSocket,
											  NotificationEventListen,
											  &NetworkEvents);

				if (result == SOCKET_ERROR)
				{
					TRACE("WSAEnumNetworkEvents(...) %d failure CPTSCommunicatorDVIT::PTSCommunicatorThread\n",
						WSAGetLastError());
					closesocket(pPTSCommunicator->ListenSocket);
					return THREADEXIT_SUCCESS;
				}

				// data received from Server
				if (NetworkEvents.lNetworkEvents & FD_ACCEPT)
				{
					// Accept a client socket
					pPTSCommunicator->ClientSocket = accept(pPTSCommunicator->ListenSocket, NULL, NULL);
					if (pPTSCommunicator->ClientSocket == INVALID_SOCKET)
					{
						TRACE("accept failed with error: %d failure CPTSCommunicatorDVIT::PTSCommunicatorThread\n", WSAGetLastError());
						closesocket(pPTSCommunicator->ListenSocket);
						WSACleanup();
						return THREADEXIT_SUCCESS;
					}
					pPTSCommunicator->m_bClientedAccepted = true;
					pView->SendMessage(PTS_CONNECTION_OK, 0, pLparam);
					

					// selects the READ, CONNECT and CLOSE operations 
					result = WSAEventSelect(pPTSCommunicator->ClientSocket, NotificationEventClient,
						                    FD_READ | FD_CLOSE | FD_CONNECT);
					if (result == SOCKET_ERROR)
					{
						TRACE("WSAEventSelect(...) %d failure CPTSCommunicatorDVIT::PTSCommunicatorThread\n",
							WSAGetLastError());
						closesocket(pPTSCommunicator->ListenSocket);
						closesocket(pPTSCommunicator->ClientSocket);
						return THREADEXIT_SUCCESS;
					}

					HandlesClient[1] = NotificationEventClient;

				}


				while (pPTSCommunicator->m_bClientedAccepted)
				{

					DWORD EventCaused = WSAWaitForMultipleEvents(2,
																 HandlesClient,
																 FALSE,
																 100,  //WSA_INFINITE, 
																 FALSE);

					if (EventCaused == WAIT_FAILED)
					{
						if (EventCaused == WAIT_FAILED)
							TRACE("WaitForMultipleObjects(...) %d failure CPTSCommunicatorDVIT::PTSCommunicatorThread\n",
							GetLastError());
						TRACE("PTSCommunicatorThread Shutting Down ...\n");
						closesocket(pPTSCommunicator->ListenSocket);
						closesocket(pPTSCommunicator->ClientSocket);
						return THREADEXIT_SUCCESS;
					}
					else
					{
						if (WaitForSingleObject(pPTSCommunicator->ShutdownEvent, 0) == WAIT_OBJECT_0)
						{
							TRACE("PTSCommunicatorThread Shutting Down normally ...\n");
							closesocket(pPTSCommunicator->ListenSocket);
							closesocket(pPTSCommunicator->ClientSocket); 
							return THREADEXIT_SUCCESS;
						}

						// Check if it is an event coming from Listen Socket
						if (EventCaused == WAIT_OBJECT_0 + 1)
						{
							result = WSAEnumNetworkEvents(pPTSCommunicator->ClientSocket,
														  NotificationEventClient,
														  &NetworkEvents);

							if (result == SOCKET_ERROR)
							{
								TRACE("WSAEnumNetworkEvents(...) %d failure CPTSCommunicatorDVIT::PTSCommunicatorThread\n",
									WSAGetLastError());
								closesocket(pPTSCommunicator->ListenSocket);
								closesocket(pPTSCommunicator->ClientSocket);
								return THREADEXIT_SUCCESS;
							}

							// data received from Server
							if (NetworkEvents.lNetworkEvents & FD_READ)
							{
								int nBytes = recv(pPTSCommunicator->ClientSocket, (char*)buffer, 15, NULL);  //// 39321 is calcualted iaw Divit PTS

								if ((INVALID_SOCKET == nBytes) || (0 == nBytes))
								{
									TRACE("recvfrom(...) failure CPTSCommunicatorDVIT::PTSCommunicatorThread\n",
										WSAGetLastError());
									continue;
								}
								else
								{
									// control checksum, if failed do not process message
									if (pPTSCommunicator->ControlMessage(buffer) == FALSE)
										continue;


									// start processing the received message
									// PTS sent the new PTS Info
									if ((buffer[MESSAGEIDPOS] == 2) && (buffer[1] == 0))/// PTS Info message
									{
										dCamId = (int)buffer[2];

										// get the plaka no
										pPTSCommunicator->GetPlakaData(buffer + 3);

										int dImageSize;

										//BYTE a = buffer[IMAGE_SIZE_CALC_1];
										//BYTE b = buffer[IMAGE_SIZE_CALC_2];
										//BYTE c = buffer[IMAGE_SIZE_CALC_3];
										//BYTE d = buffer[IMAGE_SIZE_CALC_4];

										dImageSize = buffer[IMAGE_SIZE_CALC_1] * 16777216 +
											buffer[IMAGE_SIZE_CALC_2] * 65536 +
											buffer[IMAGE_SIZE_CALC_3] * 256 +
											buffer[IMAGE_SIZE_CALC_4];


										TRACE(L"\n\n\nPTS Image Size %d\n\n\n", dImageSize);

										// below 15 more bytes are allocated since
										// there is also some data after image.
										// This data can also be read within the loop.
										BYTE *bigBuffer = new BYTE[dImageSize + 15];

										///////////////////////////////////////////
										int dTotalBytesReceived = 0;
										int dLoopCntr = 0;
										int dReadSize = 1024;

										struct timeval timeout = { 10, 0 };

										fd_set fds;
										int buffer_fd;

										while (dTotalBytesReceived < dImageSize)
										{
											FD_ZERO(&fds);
											FD_SET(pPTSCommunicator->ClientSocket, &fds);

											buffer_fd = select(FD_SETSIZE, &fds, NULL, NULL, &timeout);

											if (buffer_fd < 0)
											{
												TRACE(L"error: bad file descriptor set.\n");
											}

											if (buffer_fd == 0)
											{
												TRACE(L"error: buffer read timeout expired.\n");
											}

											if (buffer_fd > 0)
											{
												nBytes = recv(pPTSCommunicator->ClientSocket, (char*)(bigBuffer + (dLoopCntr*dReadSize)), dReadSize, NULL);
												if ((INVALID_SOCKET == nBytes) || (0 == nBytes))
												{
													TRACE(L"recvfrom(...) receiving image %d failure CPTSCommunicatorDVIT::PTSCommunicatorThread\n",
														WSAGetLastError());
													continue;
												}

												TRACE(L"Packet size: %i\n", nBytes);

												if (nBytes < dReadSize)
												{
													TRACE(L"CPTSCommunicatorDVIT::PTSCommunicatorThread probably image receive from socket finished\n");
												}

												//Increment the total number of bytes read
												dTotalBytesReceived += nBytes;
												dLoopCntr++;
												TRACE(L"Total received image size: %i\n", dTotalBytesReceived);
											}
										}


										///////////////////////////////////////////////

										//nBytes = recv(pPTSCommunicator->ClientSocket, (char*)bigBuffer, dImageSize, NULL);
										//if ((INVALID_SOCKET == nBytes) || (0 == nBytes))
										//{
										//	TRACE("recvfrom(...) receiving image %d failure CPTSCommunicatorDVIT::PTSCommunicatorThread\n",
										//		WSAGetLastError());
										//	continue;
										//}
										//if (nBytes != dImageSize)
										//{
										//	TRACE("CPTSCommunicatorDVIT::PTSCommunicatorThread expected %d received %d\n", dImageSize, nBytes);
										//}
										pPTSCommunicator->GetImageData(bigBuffer, dImageSize);
										delete[] bigBuffer;

										int dTmpDiff = 0;
										dTmpDiff = dTotalBytesReceived - dImageSize;
										if (dTmpDiff == 15)
										{
											TRACE(L"CPTSCommunicatorDVIT::PTSCommunicatorThread readPTS image from socket: Remaining 15 bytes also read...\n");
											//ptsData.colourID = (unsigned short)bigBuffer[dTotalBytesReceived - 14];

											//ptsData.modelID = (unsigned short)bigBuffer[dTotalBytesReceived - 13];

											//ptsData.speedValue = (unsigned short)bigBuffer[dTotalBytesReceived - 12];

											//memcpy_s(&(ptsData.condfidenceVal), size_t(4), (bigBuffer+dTotalBytesReceived - 11), size_t(4));

											//memcpy_s(&(ptsData.timeStamp), size_t(8), (bigBuffer + dTotalBytesReceived - 7), size_t(8));
										}
										else if (dTmpDiff > 15)
										{
											TRACE(L"CPTSCommunicatorDVIT::PTSCommunicatorThread readPTS image from socket: some kind of error...\n");
											ptsData.cameraID = 0;
											ptsData.colourID = 0;
											ptsData.modelID = 0;
											ptsData.speedValue = 0;
											ptsData.condfidenceVal = 0;
											ptsData.timeStamp = 0;
										}
										else
										{
											int dRemainData = 15 - dTmpDiff;

											TRACE(L"CPTSCommunicatorDVIT::PTSCommunicatorThread readPTS image from socket: read remaining %d bytes...\n", dRemainData);

											nBytes = recv(pPTSCommunicator->ClientSocket, (char*)buffer, dRemainData, NULL);
											if ((INVALID_SOCKET == nBytes) || (0 == nBytes))
											{
												TRACE(L"recvfrom(...) receiving final part %d failure CPTSCommunicator::PTSCommunicatorThread\n",
													WSAGetLastError());
												continue;
											}

											/// some some handling here to fill the queue data
											/// TODO --- 
										}

										g_IsAnswerReceivedFromPTS = TRUE;


										if (g_PTS_Mode == PTS_MODE_CONTINUOUS)
										{
											SetEvent(g_CameraStartDataRecieveEvent);
											pView->SendMessage(PTS_DISPLAY_IMAGE, 0, pLparam);
										}
										else
										{
											if (g_IsOdroidStartReceived == TRUE)
											{
												pView->SendMessage(PTS_DISPLAY_IMAGE, 0, pLparam);
											}
										}

									}
								}
							}

							//// READ OPERATIONS UPTO HERE
							// we can handle close operations also
							else if (NetworkEvents.lNetworkEvents & FD_CLOSE)
							{
								TRACE("PTSCommunicator Thread Socket Closed\n");

								pPTSCommunicator->m_bClientedAccepted = false;
								pView->SendMessage(PTS_CONNECTION_LOST, 0, pLparam);
							} // FD_CLOSE
						} // network event
					}// client socket event
				} // while loop
			} // listen socket event
		} // wait failed
	} // infite for loop

	closesocket(pPTSCommunicator->ListenSocket);
	closesocket(pPTSCommunicator->ClientSocket);

	return THREADEXIT_SUCCESS; 
}	

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicatorDVIT::CommThread
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
UINT __stdcall CPTSCommunicatorDVIT::CommThread(LPVOID pParam)
{
	CPTSCommunicatorDVIT *pPTSCommunicator = (CPTSCommunicatorDVIT*)pParam;

	HANDLE Handles[2];
	Handles[0] = pPTSCommunicator->ShutdownEvent;
	Handles[1] = g_PTSTriggerEvent;

	
	for(;;)
	{
		DWORD EventCaused = WaitForMultipleObjects( 2,
													Handles,  
													FALSE,                  
													INFINITE);

		if(EventCaused == WAIT_FAILED || EventCaused == WAIT_OBJECT_0)
		{
			if(EventCaused == WAIT_FAILED)
				TRACE( "PTSCommThread Shutting Down %d ...\n",  GetLastError());
			else
				TRACE( "PTSCommThread Shutting Down Normally ...\n");
			return THREADEXIT_SUCCESS;
		}

		else if (EventCaused == WAIT_OBJECT_0 + 1) // Trigger PTS To make Licence Plate Recognition
		{
			TRACE("PTSCommThread PTSTriggerEvent Received\n");
			ResetEvent(g_PTSTriggerEvent);
			g_IsAnswerReceivedFromPTS = FALSE;
			pPTSCommunicator->SendTriggerMessage();
		}
	}

	return THREADEXIT_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicatorDVIT::ControlMessage
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
BOOL CPTSCommunicatorDVIT::ControlMessage(BYTE *message)
{


	return TRUE; /// boraN always return TRUE for the time being
}




////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicatorDVIT::SendTriggerMessage
// 
// DESCRIPTION:	Sends message 0xC0 database closed
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
void CPTSCommunicatorDVIT::SendTriggerMessage()
{
int                 iLen;
int                 iSent;
unsigned char       data[10];

	data[0] = 2;   // message ID                      
	data[1] = 0;   // reserved                   
	data[2] = 0;   // reserved                 
	data[3] = 1;   // camera number
	data[4] = 21;  // command number	
	data[5] = 0;   // reserved  	
	data[6] = 0;   // reserved  	
	data[7] = 3;   // end of packet  


	iLen = 8;

	iSent = send(ClientSocket, (char*)data, iLen, NULL);

	// check errors
	if ( iSent == SOCKET_ERROR )
	{
      LPVOID lpMsgBuf;
	   FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                     NULL,
                     GetLastError(),
                     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                     (LPTSTR) &lpMsgBuf,
                     0,
                     NULL );
         
      // Display the string.
      ::MessageBox( NULL,(LPTSTR)lpMsgBuf, (LPCWSTR)WARNINGWINDOW_TITLE, MB_OK|MB_ICONINFORMATION );
      // Free the buffer.
      LocalFree( lpMsgBuf );		   
	}
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicatorDVIT::GetImageData
// 
// DESCRIPTION:	Get the PTS image data and put it to global variable
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// BN              26082003   	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
void CPTSCommunicatorDVIT::GetImageData(BYTE* imgDataIn, int dataLenIn)
{
	if (g_CarPlakaImage)
	{
		delete[] g_CarPlakaImage;
	}

	g_CarPlakaImage = new BYTE[dataLenIn];

	memcpy_s(g_CarPlakaImage, dataLenIn, imgDataIn, dataLenIn);
	g_CarPlakaImageLenght = dataLenIn;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicatorDVIT::GetPlakaData
// 
// DESCRIPTION:	Get the PTS plaka data and put it to global variable
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// BN              26082003   	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
void CPTSCommunicatorDVIT::GetPlakaData(BYTE* plakaDataIn)
{
	//strcpy_s(g_PlakaNoChars, (int)PLAKA_CHAR_LENGTH, (char*)plakaDataIn);
	strncpy_s(g_PlakaNoChars, (char*)plakaDataIn, PLAKA_CHAR_LENGTH);

}





