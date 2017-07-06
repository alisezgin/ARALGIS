#include "stdafx.h"
#include "ARALGIS.h"
#include "..\HeaderFiles\PTSCommunicatorISSD.h"
#include "MainFrm.h"
#include "..\HeaderFiles\PTSMessageDefinitionsISSD.h"




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicatorISSD::CPTSCommunicatorISSD
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
CPTSCommunicatorISSD::CPTSCommunicatorISSD()
{
	SOCKET scSocket = INVALID_SOCKET;

	m_bIsConnectedToServer = false;

	WSADATA wsaData;

   int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
   if(result != 0)
	{
		TRACE("WSAStartup(...) failure CPTSCommunicatorISSD::PTSCommunicatorThread\n");
	}

	bRun = FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicatorISSD::CPTSCommunicatorISSD
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
CPTSCommunicatorISSD::~CPTSCommunicatorISSD()
{
	Shutdown();	
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicatorISSD::Start
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
BOOL CPTSCommunicatorISSD::Start(CMainFrame* pFrame)
{
	if(bRun)
	{
		TRACE("_beginthreadex(...) failure, for Launch Thread CPTSCommunicatorISSD::Start\n");
		return FALSE;
	}
	
	TRACE("PTS CommunicatorThread Starting ...\n");

	m_pFrame	    = pFrame;

	bRun = TRUE;

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

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicatorISSD::Shutdown
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
BOOL CPTSCommunicatorISSD::Shutdown()
{
   if(!bRun)
      return FALSE;

   TRACE("PTSCommunicator Shutting down ...\n");

   bRun = FALSE;
	
   SetEvent(ShutdownEvent);

	// wait for the thread to stop
   WaitForSingleObject(ThreadPTSCommunicator, INFINITE);
   WaitForSingleObject(ThreadComm, INFINITE);

   CloseHandle(ShutdownEvent);
   CloseHandle(ThreadPTSCommunicator);
   CloseHandle(ThreadComm);

   closesocket(scSocket);

	WSACleanup();

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicatorISSD::PTSCommunicatorThread
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
UINT __stdcall CPTSCommunicatorISSD::PTSCommunicatorThread(LPVOID pParam)
{
	CPTSCommunicatorISSD *pPTSCommunicator = (CPTSCommunicatorISSD*)pParam;

//////////////////////////////////
	WSANETWORKEVENTS	NetworkEvents;
	int iResult;

	struct addrinfo* result = NULL;
	struct addrinfo* ptr = NULL;
	struct addrinfo hints;

	CView* pView = pPTSCommunicator->m_pFrame->GetActiveView();
	LPARAM pLparam;
	pLparam = reinterpret_cast<LPARAM>("ARALGIS");

	// Create Notification Event
	WSAEVENT NotificationEvent = WSACreateEvent();
	if (NotificationEvent == WSA_INVALID_EVENT)
	{
		TRACE("WSACreateEvent(...) failure for Event CPTSCommunicatorISSD::PTSCommunicatorThread",
			WSAGetLastError());
		//closesocket(pPTSCommunicator->scSocket);
		WSACleanup();
		return THREADEXIT_SUCCESS;
	}


	// set address info
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;  //TCP connection!!!

	//resolve server address and port 
	iResult = getaddrinfo(g_PTSIP, g_PTSPort, &hints, &result);
	if (iResult != 0)
	{
		TRACE("CPTSCommunicatorISSD::PTSCommunicatorThread getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return THREADEXIT_SUCCESS;
	}

	WSAEVENT Handles[2];

	Handles[0] = pPTSCommunicator->ShutdownEvent;
	Handles[1] = NotificationEvent;

	while (pPTSCommunicator->bRun)
	{
		// Attempt to connect to an address until one succeeds
		for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
		{

			// Create a SOCKET for connecting to server
			pPTSCommunicator->scSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

			if (pPTSCommunicator->scSocket == INVALID_SOCKET)
			{
				TRACE("socket failed with error: %ld\n", WSAGetLastError());
			}

			// Connect to server.
			iResult = connect(pPTSCommunicator->scSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (iResult == SOCKET_ERROR)
			{
				pPTSCommunicator->scSocket = INVALID_SOCKET;
				TRACE("The server is down... did not connect\n");
			}
			else
			{
				pPTSCommunicator->m_bIsConnectedToServer = true;
				bool bretVal = pPTSCommunicator->SendIDMessage();

				if (bretVal)
				{
					pView->SendMessage(PTS_CONNECTION_OK, 0, pLparam);
				}
				else
				{
					pView->SendMessage(PTS_CONNECTION_NOK, 0, pLparam);
				}
			}
		}


		// if connection failed
		if (pPTSCommunicator->scSocket == INVALID_SOCKET)
		{
			TRACE("Unable to connect to server!\n");
			Sleep(100);
		}

		while (pPTSCommunicator->m_bIsConnectedToServer == true)
		{
			// selects the READ, WRITE, ACCEPT, CONNECT and CLOSE operations 
			iResult = WSAEventSelect(pPTSCommunicator->scSocket, NotificationEvent,
				FD_READ | FD_CLOSE);

			if (iResult == SOCKET_ERROR)
			{
				TRACE("WSAEventSelect(...) failure CPTSCommunicatorISSD::PTSCommunicatorThread",
					WSAGetLastError());
				closesocket(pPTSCommunicator->scSocket);
				WSACleanup();
				return THREADEXIT_SUCCESS;
			}

			DWORD EventCaused = WSAWaitForMultipleEvents(2,
														 Handles,
														 FALSE,
														 100,  //WSA_INFINITE, 
														 FALSE);

			if (EventCaused == WAIT_FAILED)
			{
				if (EventCaused == WAIT_FAILED)
					TRACE("WSAWaitForMultipleEvents(...) failure CPTSCommunicatorISSD::PTSCommunicatorThread",
					GetLastError());
				TRACE("PTSCommunicatorThread Shutting Down ...\n");
				closesocket(pPTSCommunicator->scSocket);
				WSACleanup();
				return THREADEXIT_SUCCESS;
			}
			else
			{
				if (WaitForSingleObject(pPTSCommunicator->ShutdownEvent, 0) == WAIT_OBJECT_0)
				{
					TRACE("PTSCommunicatorThread Shutting Down normally...\n");
					closesocket(pPTSCommunicator->scSocket);
					WSACleanup(); 
					return THREADEXIT_SUCCESS;
				}

				iResult = WSAEnumNetworkEvents(pPTSCommunicator->scSocket,
					NotificationEvent,
					&NetworkEvents);

				if (iResult == SOCKET_ERROR)
				{
					TRACE("WSAEnumNetworkEvents(...) failure CPTSCommunicatorISSD::PTSCommunicatorThread",
						WSAGetLastError());
					closesocket(pPTSCommunicator->scSocket);
					WSACleanup();
					return THREADEXIT_SUCCESS;
				}

				// data received from Server
				if (NetworkEvents.lNetworkEvents & FD_READ)
				{
					BYTE buffer[1024];
					int nBytes = recv(pPTSCommunicator->scSocket, (char*)buffer, 5, NULL);

					if ((INVALID_SOCKET == nBytes) || (0 == nBytes))
					{
						TRACE("recvfrom(...) failure CPTSCommunicatorISSD::PTSCommunicatorThread",
							WSAGetLastError());
						continue;
					}
					else
					{
						// control checksum, if failed do not process message
						if (pPTSCommunicator->ControlMessage(buffer) == FALSE)
							continue;

						// message is OK.	
						// start processing the received message
						if (buffer[MESSAGE_TYPE_POS] == PLAKA_NO_MESSAGE)
						{

							int dPlakaSize = 0;

							dPlakaSize = buffer[IMAGE_SIZE_CALC_1] * 16777216 +
										 buffer[IMAGE_SIZE_CALC_2] * 65536 +
										 buffer[IMAGE_SIZE_CALC_3] * 256 +
										 buffer[IMAGE_SIZE_CALC_4];

							nBytes = recv(pPTSCommunicator->scSocket, (char*)buffer, dPlakaSize, NULL);
							if ((INVALID_SOCKET == nBytes) || (0 == nBytes))
							{
								TRACE("recvfrom(...) receiving final part %d failure CPTSCommunicatorISSD::PTSCommunicatorThread\n",
									WSAGetLastError());
								continue;
							}
							else
							{
								if (g_AutoDetect_Type != NO_AUTO_VEHICLE_DETECT)
								{
									::InterlockedIncrement((long*)&g_carsDetectedByPTSCnt);
									TRACE(L"inc g_carsDetectedByPTSCnt %d \n", g_carsDetectedByPTSCnt);
									SetEvent(g_CameraStartDataRecieveEvent);
								}

								strncpy_s(g_PlakaNoChars, (char*)buffer, dPlakaSize);
								pView->SendMessage(PTS_DISPLAY_PLAKA, 0, pLparam);
							}
						}

						else if (buffer[MESSAGE_TYPE_POS] == CAR_IMAGE_MESSAGE)
						{

							int dPlakaImageSize = 0;
							memcpy(&dPlakaImageSize, buffer, 4);

							BYTE *bigBuffer = new BYTE[dPlakaImageSize];

							int dTotalBytesReceived = 0;
							int dLoopCntr = 0;
							int dReadSize = 1024;

							struct timeval timeout = { 10, 0 };

							fd_set fds;
							int buffer_fd;

							while (dTotalBytesReceived < dPlakaImageSize)
							{
								FD_ZERO(&fds);
								FD_SET(pPTSCommunicator->scSocket, &fds);

								buffer_fd = select(FD_SETSIZE, &fds, NULL, NULL, &timeout);

								if (buffer_fd < 0)
								{
									TRACE("error: bad file descriptor set.\n");
								}

								if (buffer_fd == 0)
								{
									TRACE("error: buffer read timeout expired.\n");
								}

								if (buffer_fd > 0)
								{
									nBytes = recv(pPTSCommunicator->scSocket, (char*)(bigBuffer + (dLoopCntr*dReadSize)), dReadSize, NULL);
									if ((INVALID_SOCKET == nBytes) || (0 == nBytes))
									{
										TRACE("recvfrom(...) receiving image %d failure CPTSCommunicatorISSD::PTSCommunicatorThread\n",
											WSAGetLastError());
										continue;
									}

									TRACE("Packet size: %i\n", nBytes);

									if (nBytes < dReadSize)
									{
										TRACE("CPTSCommunicatorISSD::PTSCommunicatorThread probably image receive from socket finished\n");
									}

									//Increment the total number of bytes read
									dTotalBytesReceived += nBytes;
									dLoopCntr++;
									TRACE("Total received image size: %i\n", dTotalBytesReceived);
								}
							}

							pPTSCommunicator->GetImageData(bigBuffer, dPlakaImageSize);
							delete[] bigBuffer;
							pView->SendMessage(PTS_DISPLAY_IMAGE, 0, pLparam);
						}

					}
				}

				// handle ACCEPT operations 
				else if (NetworkEvents.lNetworkEvents & FD_ACCEPT)
				{
					TRACE("CPTSCommunicatorISSD Thread Socket ACCEPT\n");
				}

				// handle WRITE operations 
				else if (NetworkEvents.lNetworkEvents & FD_WRITE)
				{
					TRACE("CPTSCommunicatorISSD Thread Socket WRITE\n");
				}
				//// READ OPERATIONS UPTO HERE
				// we can handle close operations also
				else if (NetworkEvents.lNetworkEvents & FD_CLOSE)
				{
					pPTSCommunicator->m_bIsConnectedToServer = false;
					TRACE("CPTSCommunicatorISSD Thread Server Socket Closed\n");
					pView->SendMessage(PTS_CONNECTION_LOST, 0, pLparam);
				}
			}
		}
	}

	freeaddrinfo(result);
	closesocket(pPTSCommunicator->scSocket);
	WSACleanup();

	return THREADEXIT_SUCCESS; 
}	

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicatorISSD::CommThread
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
UINT __stdcall CPTSCommunicatorISSD::CommThread(LPVOID pParam)
{
	CPTSCommunicatorISSD *pPTSCommunicator = (CPTSCommunicatorISSD*)pParam;

	HANDLE Handles[2];
	Handles[0] = pPTSCommunicator->ShutdownEvent;
	Handles[1] = g_SendUVSSImageEvent;
	 
	
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
			ResetEvent(g_SendUVSSImageEvent);
			pPTSCommunicator->SendImageData();
		}
	}

	return THREADEXIT_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicatorISSD::ControlMessage
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
BOOL CPTSCommunicatorISSD::ControlMessage(BYTE *message)
{


	return TRUE; /// boraN always return TRUE for the time being
}




////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicatorISSD::SendImageData
// 
// DESCRIPTION:	Sends UVSS Image to PTS
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
void CPTSCommunicatorISSD::SendImageData()
{
int  iLen;
int   iSent;
BYTE* data;

	iLen = 5 + g_ByteImageSize;

	data = new BYTE[iLen];

	// boraN code analysis
	// chck if data[4] can be written, i.e., iLen is greater than 4
	data[4] = UVSS_IMAGE;

	memcpy(data, &g_ByteImageSize, 4);


	memcpy(data+5, g_ByteImageTest, g_ByteImageSize);

	/////////////////////////////////////////////////
	//int dTotalBytesSent = 0;
	//int dLoopCntr = 0;
	//int dSendSize = 1024;

	//struct timeval timeout = { 10, 0 };

	//fd_set fds;
	//int buffer_fd;

	//while (dTotalBytesSent < iLen)
	//{
	//	FD_ZERO(&fds);
	//	FD_SET(scSocket, &fds);

	//	buffer_fd = select(FD_SETSIZE, NULL, &fds, NULL, &timeout);

	//	if (buffer_fd < 0)
	//	{
	//		TRACE("error: bad file descriptor set.\n");
	//	}

	//	if (buffer_fd == 0)
	//	{
	//		TRACE("error: buffer read timeout expired.\n");
	//	}

	//	if (buffer_fd > 0)
	//	{
	//		iSent = send(scSocket, (char*)(data + (dLoopCntr*dSendSize)), dSendSize, NULL);

	//		if ((INVALID_SOCKET == iSent) || (0 == iSent))
	//		{
	//			TRACE("sendTo(...) sending image %d failure CPTSCommunicatorISSD::PTSCommunicatorThread\n",
	//				WSAGetLastError());
	//			continue;
	//		}

	//		TRACE("Packet size: %i\n", iSent);

	//		//Increment the total number of bytes read
	//		dTotalBytesSent += iSent;
	//		dLoopCntr++;
	//		TRACE("Total sent image size: %i\n", dTotalBytesSent);
	//	}
	//}
	//////////////////////////////////////////////
	iSent = send(scSocket, (char*)data, iLen, NULL);

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
	
	delete[] data;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicatorISSD::GetImageData
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
void CPTSCommunicatorISSD::GetImageData(BYTE* imgDataIn, int dataLenIn)
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
// FUNCTION:	CPTSCommunicatorISSD::GetPlakaData
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
void CPTSCommunicatorISSD::GetPlakaData(BYTE* plakaDataIn)
{
	//strcpy_s(g_PlakaNoChars, (int)PLAKA_CHAR_LENGTH, (char*)plakaDataIn);
	strncpy_s(g_PlakaNoChars, (char*)plakaDataIn, PLAKA_CHAR_LENGTH);

}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicatorISSD::SendIDMessage
// 
// DESCRIPTION:	Send the ID Message to PTS
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
bool CPTSCommunicatorISSD::SendIDMessage()
{
	int                 iSent;
	unsigned char       data[MESSAGE_LENGTH];
	MessageAralgisID    messageToSend;

	messageToSend.DataLenght[0] = (unsigned char)0;
	messageToSend.DataLenght[1] = (unsigned char)0;
	messageToSend.DataLenght[2] = (unsigned char)0;
	messageToSend.DataLenght[3] = (unsigned char)1;
	messageToSend.DataLenght[4] = (unsigned char)REGISTRATION_CONSTANT;
	messageToSend.DataLenght[5] = (unsigned char)1;


	memcpy_s(data, size_t(MESSAGE_LENGTH), &messageToSend, size_t(MESSAGE_LENGTH));

	iSent = send(scSocket, (char*)data, (int)MESSAGE_LENGTH, NULL);

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
		return false;
	}

 	return true;
}
