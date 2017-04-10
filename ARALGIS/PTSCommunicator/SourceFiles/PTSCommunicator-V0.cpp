#include "stdafx.h"
#include "ARALGIS.h"
#include "..\HeaderFiles\PTSCommunicator.h"
#include "MainFrm.h"
#include "..\HeaderFiles\MessageDefinitions.h"




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicator::CPTSCommunicator
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
CPTSCommunicator::CPTSCommunicator()
{
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	WSADATA wsaData;

   int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
   if(result != 0)
	{
		TRACE("WSAStartup(...) failure CPTSCommunicator::PTSCommunicatorThread");
	}

	bRun = FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicator::CPTSCommunicator
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
CPTSCommunicator::~CPTSCommunicator()
{
	Shutdown();	
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicator::Start
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
BOOL CPTSCommunicator::Start(SCNOTIFYPROC pNotifyProc, CMainFrame* pFrame)
{
	if(bRun)
	{
		TRACE("_beginthreadex(...) failure, for Launch Thread CPTSCommunicator::Start");
		return FALSE;
	}


	TRACE("PTS Communicator Starting ...\n");

	m_pNotifyProc	= pNotifyProc;
	m_pFrame	    = pFrame;

	ShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// Launch PTSCommunicator Thread
	ThreadPTSCommunicator = (HANDLE) _beginthreadex( NULL, 
		                                             0, 
													 PTSCommunicatorThread, 
												 	 this, 
													 0, 
													 &ThreadID
													);
	if (!ThreadPTSCommunicator)
	{
		TRACE("_beginthreadex(...) failure, PTSCommunicator::Start");
		return FALSE;
	}

	TRACE("PTSCommunicator ThreadID %x ...\n", ThreadID);


	// Launch Comm Thread
	ThreadComm = (HANDLE) _beginthreadex( NULL, 
		                                  0, 
										  CommThread, 
										  this, 
										  0, 
										  &ThreadID );
	if(!ThreadComm)
	{
		TRACE("_beginthreadex(...) failure, PTSCommunicator::Start CommThread");
		return FALSE;
	}

	TRACE("CommThread ThreadID %x ...\n", ThreadID);

	bRun = TRUE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicator::Shutdown
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
BOOL CPTSCommunicator::Shutdown()
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


	WSACleanup();

	bRun = FALSE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicator::PTSCommunicatorThread
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
UINT __stdcall CPTSCommunicator::PTSCommunicatorThread(LPVOID pParam)
{
	CPTSCommunicator *pPTSCommunicator = (CPTSCommunicator*)pParam;

	WSANETWORKEVENTS	NetworkEvents;
	int result;

	TRACE("\n\n PTS Communicator Thread Started\n");

	struct addrinfo* resultAddr = NULL;
	struct addrinfo hints;
	int iResult;

	BYTE buffer[15];
	int dCamId = 0;


	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the address and port
	iResult = getaddrinfo(NULL, ARALGIS_PTS_TCP_PORT_CHAR, &hints, &resultAddr);
	if ( iResult != 0 ) 
	{
		TRACE("getaddrinfo failed with error: %d failure CPTSCommunicator::PTSCommunicatorThread", iResult);	
		WSACleanup();
		return THREADEXIT_SUCCESS;
	}

	// Create a SOCKET for connecting to server
	pPTSCommunicator->ListenSocket = socket(resultAddr->ai_family, resultAddr->ai_socktype, resultAddr->ai_protocol);
	if (pPTSCommunicator->ListenSocket == INVALID_SOCKET)
	{
		TRACE("socket failed with error: %d failure CPTSCommunicator::PTSCommunicatorThread", WSAGetLastError());
		freeaddrinfo(resultAddr);
		WSACleanup();
		return THREADEXIT_SUCCESS;
	}

	// Setup the TCP listening socket
	iResult = bind(pPTSCommunicator->ListenSocket, resultAddr->ai_addr, (int)resultAddr->ai_addrlen);
	if (iResult == SOCKET_ERROR) 
	{
		TRACE("bind failed with error: %d failure CPTSCommunicator::PTSCommunicatorThread", WSAGetLastError());
		freeaddrinfo(resultAddr);
		closesocket(pPTSCommunicator->ListenSocket);
		WSACleanup();
		return THREADEXIT_SUCCESS;
	}

	freeaddrinfo(resultAddr);

	iResult = listen(pPTSCommunicator->ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) 
	{
		TRACE("listen failed with error: %d failure CPTSCommunicator::PTSCommunicatorThread", WSAGetLastError());
		closesocket(pPTSCommunicator->ListenSocket);
		WSACleanup();
		return THREADEXIT_SUCCESS;
	}

	// Accept a client socket
	pPTSCommunicator->ClientSocket = accept(pPTSCommunicator->ListenSocket, NULL, NULL);
	if (pPTSCommunicator->ClientSocket == INVALID_SOCKET) 
	{
		TRACE("accept failed with error: %d failure CPTSCommunicator::PTSCommunicatorThread", WSAGetLastError());
		closesocket(pPTSCommunicator->ListenSocket);
		WSACleanup();
		return THREADEXIT_SUCCESS;
	}

	// No longer need server socket
	//closesocket(pPTSCommunicator->ListenSocket);

	// Create Notification Event
	WSAEVENT NotificationEvent = WSACreateEvent();
	if (NotificationEvent == WSA_INVALID_EVENT)
	{
		TRACE( "WSACreateEvent(...) failure for Event CPTSCommunicator::PTSCommunicatorThread",
			    WSAGetLastError());
		closesocket(pPTSCommunicator->ClientSocket);
		return THREADEXIT_SUCCESS;
	}	


// selects the READ, WRITE, ACCEPT, CONNECT and CLOSE operations 
	result = WSAEventSelect(pPTSCommunicator->ClientSocket, NotificationEvent,
							FD_READ | FD_CLOSE | FD_CONNECT | FD_WRITE | FD_ACCEPT);

	if(result == SOCKET_ERROR)
	{
		TRACE( "WSAEventSelect(...) failure CPTSCommunicator::PTSCommunicatorThread", 
			    WSAGetLastError());
		closesocket(pPTSCommunicator->ClientSocket);
		return THREADEXIT_SUCCESS;
	}


	WSAEVENT Handles[2];

	Handles[0] = pPTSCommunicator->ShutdownEvent;
	Handles[1] = NotificationEvent;

	for(;;)
	{
		DWORD EventCaused = WSAWaitForMultipleEvents( 2,
													  Handles,  
													  FALSE,                  
													  100,  //WSA_INFINITE, 
													  FALSE);

		if(EventCaused == WAIT_FAILED || EventCaused == WAIT_OBJECT_0)
		{
			if(EventCaused == WAIT_FAILED)
				TRACE( "WaitForMultipleObjects(...) failure CPTSCommunicator::PTSCommunicatorThread", 
				       GetLastError());
			TRACE( "PTSCommunicatorThread Shutting Down ...\n");
			closesocket(pPTSCommunicator->ClientSocket);
			return THREADEXIT_SUCCESS;
		}
		else
		{
			if(WaitForSingleObject(pPTSCommunicator->ShutdownEvent, 0) == WAIT_OBJECT_0)
			{
				TRACE( "PTSCommunicatorThread Shutting Down normally ...\n");
				return THREADEXIT_SUCCESS;
			}



			////// else data received and handled below
			result = WSAEnumNetworkEvents(pPTSCommunicator->ClientSocket,
										   NotificationEvent,              
										   &NetworkEvents );
			
			if(result == SOCKET_ERROR)						 
			{
				TRACE( "WSAEnumNetworkEvents(...) failure CPTSCommunicator::PTSCommunicatorThread", 
						 WSAGetLastError());
				closesocket(pPTSCommunicator->ClientSocket);
				return THREADEXIT_SUCCESS;
			}

			// data received from Server
			if(NetworkEvents.lNetworkEvents & FD_READ)
			{
				int nBytes = recv(pPTSCommunicator->ClientSocket, (char*)buffer, 15, NULL);  //// 39321 is calcualted iaw Divit PTS

				if( (INVALID_SOCKET == nBytes ) || (0 == nBytes) )
				{
					TRACE( "recvfrom(...) failure CPTSCommunicator::PTSCommunicatorThread" , 
							 WSAGetLastError());
					continue; 
				}
				else
				{
					// control checksum, if failed do not process message
					if( pPTSCommunicator->ControlMessage(buffer) == FALSE )
						continue;

					
					// start processing the received message
					// PTS sent the new PTS Info
					if ((buffer[MESSAGEIDPOS] == 2) && (buffer[1] == 0))/// PTS Info message
					{
						dCamId = (int)buffer[2];

						// get the plaka no
						pPTSCommunicator->GetPlakaData(buffer + 3);

						int dImageSize;

						BYTE a = buffer[IMAGE_SIZE_CALC_1];
						BYTE b = buffer[IMAGE_SIZE_CALC_2];
						BYTE c = buffer[IMAGE_SIZE_CALC_3];
						BYTE d = buffer[IMAGE_SIZE_CALC_4];

						dImageSize = buffer[IMAGE_SIZE_CALC_1] * 16777216 + 
									 buffer[IMAGE_SIZE_CALC_2] * 65536 +
									 buffer[IMAGE_SIZE_CALC_3] * 256 +
									 buffer[IMAGE_SIZE_CALC_4];



						BYTE *bigBuffer = new BYTE[dImageSize];

						nBytes = recv(pPTSCommunicator->ClientSocket, (char*)bigBuffer, dImageSize, NULL);
						if ((INVALID_SOCKET == nBytes) || (0 == nBytes))
						{
							TRACE("recvfrom(...) receiving image failure CPTSCommunicator::PTSCommunicatorThread\n",
								WSAGetLastError());
							continue;
						}
						pPTSCommunicator->GetImageData(bigBuffer, dImageSize);
						delete bigBuffer;

						nBytes = recv(pPTSCommunicator->ClientSocket, (char*)buffer, 15, NULL);
						if ((INVALID_SOCKET == nBytes) || (0 == nBytes))
						{
							TRACE("recvfrom(...) receiving final part failure CPTSCommunicator::PTSCommunicatorThread\n",
								WSAGetLastError());
							continue;
						}

///////////////////////////////////////////////////////////
						//struct timeval timeout = { 10, 0 };

						//fd_set fds;
						//int buffer_fd, buffer_out;
						//int recv_size = 0;

						//while (recv_size < dImageSize + 15)  /// 15 from PTS message
						//{
						//	FD_ZERO(&fds);
						//	FD_SET(pPTSCommunicator->ClientSocket, &fds);

						//	buffer_fd = select(FD_SETSIZE, &fds, NULL, NULL, &timeout);

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
						//		do
						//		{
						//			nBytes = recv(pPTSCommunicator->ClientSocket, &dImage, 1024, NULL); // recv(pPTSCommunicator->ClientSocket, (char*)buffer, 15, NULL);
						//		} while (nBytes < 0);

						//		TRACE("Packet size: %i\n", nBytes);
						//		
						//		//Increment the total number of bytes read
						//		recv_size += nBytes;
						//		TRACE("Total received image size: %i\n", recv_size);
						//	}
						//}

//////////////////////////////////////////////////////////

						//SetEvent(g_PTSInfoReceivedEvent);


						pPTSCommunicator->m_pNotifyProc((LPVOID)pPTSCommunicator->m_pFrame, PTS_DISPLAY_IMAGE);
					}
				}
			}

			///
			// handle ACCEPT operations 
			else if(NetworkEvents.lNetworkEvents & FD_ACCEPT)
			{
				TRACE("PTSCommunicator Thread Socket ACCEPT\n");
			}

			// handle WRITE operations 
			else if(NetworkEvents.lNetworkEvents & FD_WRITE)
			{
				TRACE("PTSCommunicator Thread Socket WRITE\n");
			}
			//// READ OPERATIONS UPTO HERE
			// we can handle close operations also
			else if(NetworkEvents.lNetworkEvents & FD_CLOSE)
			{
				TRACE("PTSCommunicator Thread Socket Closed\n");

				//isServerFound = FALSE;
				//isActiveUserPresent = FALSE;

				CMainFrame* pMFrame = 
	                    static_cast<CMainFrame*>(pPTSCommunicator->m_pFrame);

				pMFrame->PostMessage(WM_PTS_LOST, 0, 0);

				::MessageBox(NULL, (LPCWSTR)".......", (LPCWSTR)WARNINGWINDOW_TITLE, MB_OK | MB_ICONERROR);
			}
		}
	}

	return THREADEXIT_SUCCESS; 
}	

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicator::CommThread
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
UINT __stdcall CPTSCommunicator::CommThread(LPVOID pParam)
{
	CPTSCommunicator *pPTSCommunicator = (CPTSCommunicator*)pParam;

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
				TRACE( "CommThread Shutting Down %d ...\n",  GetLastError());
			else
				TRACE( "CommThread Shutting Down Normally ...\n");
			return THREADEXIT_SUCCESS;
		}

		else if (EventCaused == WAIT_OBJECT_0 + 1) // Trigger PTS To make Licence Plate Recognition
		{
			TRACE("CommThread PTSTriggerEvent Received\n");
			ResetEvent(g_PTSTriggerEvent);
			pPTSCommunicator->SendTriggerMessage();
		}
	}

	return THREADEXIT_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicator::ControlMessage
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
BOOL CPTSCommunicator::ControlMessage(BYTE *message)
{
	//int countData = (int)message[NUMBYTEPOS];
	//BYTE checksum = 0x00;

	//checksum = message[NUMBYTEPOS] ^ message[MESSAGEIDPOS] ^ message[MAGICNUMPOS];
	//for(int i=DATAPOS; i<countData; i++)
	//	checksum ^= message[i];

	//if (checksum !=  message[countData])
	//	return FALSE;

	//if (MAGICNUMBER !=  message[MAGICNUMPOS])
	//	return FALSE;

	return TRUE; /// boraN always return TRUE for the time being
}




////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicator::SendTriggerMessage
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
void CPTSCommunicator::SendTriggerMessage()
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
// FUNCTION:	CPTSCommunicator::GetImageData
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
void CPTSCommunicator::GetImageData(BYTE* imgDataIn, int dataLenIn)
{
	if (g_CarPlakaImage)
	{
		delete g_CarPlakaImage;
	}

	g_CarPlakaImage = new BYTE[dataLenIn];

	memcpy_s(g_CarPlakaImage, dataLenIn, imgDataIn, dataLenIn);
	m_CarPlakaImageLenght = dataLenIn;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CPTSCommunicator::GetPlakaData
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
void CPTSCommunicator::GetPlakaData(BYTE* plakaDataIn)
{
	//strcpy_s(g_PlakaNoChars, (int)PLAKA_CHAR_LENGTH, (char*)plakaDataIn);
	strncpy_s(g_PlakaNoChars, (char*)plakaDataIn, PLAKA_CHAR_LENGTH);

}





