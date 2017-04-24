
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "ARALGIS.h"

#include "MainFrm.h"

#include "ARALGISView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_MOVING()
	ON_WM_NCLBUTTONDBLCLK()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_IsFirstTime = true;

	m_CameraDataReceiver = NULL;
	m_PTSCommunicator = NULL;
	m_OdroidCommunicator = NULL;
	m_CameraDatabaseServer = NULL;
	m_ImageFilterProcessing = NULL;
}

CMainFrame::~CMainFrame()
{
	if (m_CameraDataReceiver)
		delete m_CameraDataReceiver;

	if (m_PTSCommunicator)
		delete m_PTSCommunicator;

	if (m_OdroidCommunicator)
		delete m_OdroidCommunicator;

	if (m_CameraDatabaseServer)
		delete m_CameraDatabaseServer;

	if (m_ImageFilterProcessing)
		delete m_ImageFilterProcessing;

	// Don't forget to release the semaphore
	if (!ReleaseSemaphore(m_hSemaphore,  // handle to semaphore
						  1,           // increase count by one
						  NULL))      // not interested in previous count
	{
		TRACE("Failed to release semaphore\n");
	}
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	m_IsFirstTime = false;


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWnd::PreCreateWindow(cs))
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	CRect workArea;
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);

	cs.x = workArea.left;
	cs.y = workArea.top;

	cs.cx = workArea.right;
	cs.cy = workArea.bottom;

	// Create a window without min/max buttons or sizable border 
	cs.style = WS_OVERLAPPED | WS_SYSMENU | WS_BORDER;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CMainFrame::NotifyProcPTSComm
// 
// DESCRIPTION:	Handles the notification messages sent by PTSCommunicator.
//              Updates the GUI according to the received message 
//
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// BN              28032017	    1.0			Origin
// 
//////////////////////////////////////////////////////////////////////////////// 
void CMainFrame::NotifyProcPTSComm(LPVOID lpParam, UINT nCode)
{
	CMainFrame* pFrame = (CMainFrame*)lpParam;

	CARALGISView* pView = static_cast<CARALGISView*>(pFrame->GetActiveView());

	if (pView)
	{
		switch (nCode)
		{
		case PTS_DISPLAY_IMAGE:
			pView->DisplayPTSImage();
			break;

		case PTS_DELETE_IMAGE:
			pView->DeletePTSImage();
			break;

		case PTS_CONNECTION_LOST:
		{
			pView->UpdatePTSStatus(false);
			//::MessageBox(NULL,
			//	(LPCWSTR)L"PTS Ba�lant�s� Kayboldu",
			//	(LPCWSTR)WARNINGWINDOW_TITLE,
			//	MB_OK | MB_ICONERROR
			//	);
			break;
		}

		case PTS_CONNECTION_OK:
		{
			pView->UpdatePTSStatus(true);
			//::MessageBox(NULL,
			//	(LPCWSTR)L"PTS Ba�lant�s� Kuruldu",
			//	(LPCWSTR)WARNINGWINDOW_TITLE,
			//	MB_OK | MB_ICONERROR
			//	);
			break;
		}
		default:
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CMainFrame::NotifyProcOdroidComm
// 
// DESCRIPTION:	Handles the notification messages sent by OdroidCommunicator.
//              Updates the GUI according to the received message 
//
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// BN              28032017	    1.0			Origin
// 
//////////////////////////////////////////////////////////////////////////////// 
void CMainFrame::NotifyProcOdroidComm(LPVOID lpParam, UINT nCode)
{
	CMainFrame* pFrame = (CMainFrame*)lpParam;

	CARALGISView* pView = static_cast<CARALGISView*>(pFrame->GetActiveView());

	if (pView)
	{
		switch (nCode)
		{
		case ODROID_CONNECTION_LOST:
		{
			pView->UpdatePeripheralStatus(false);
		}
			break;

		case ODROID_CONNECTION_OK:
		{
			pView->UpdatePeripheralStatus(true);
		}
			break;			

		default:
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CMainFrame::NotifyProcCameraComm
// 
// DESCRIPTION:	Handles the notification messages sent by CameraDataReceiver.
//              Updates the GUI according to the received message 
//
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// BN              28032017	    1.0			Origin
// 
//////////////////////////////////////////////////////////////////////////////// 
void CMainFrame::NotifyProcCameraComm(LPVOID lpParam, UINT nCode)
{
	CMainFrame* pFrame = (CMainFrame*)lpParam;

	CARALGISView* pView = static_cast<CARALGISView*>(pFrame->GetActiveView());

	if (pView)
	{
		switch (nCode)
		{
		case SET_TIMER_PERIOD_CAMERA:
			//pView->SetTimerDisplay();
			break;

		case KILL_TIMER_CAMERA:
			//pView->KillTimerCamera();
			break;

		default:
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CMainFrame::NotifyProcImageFiltering
// 
// DESCRIPTION:	Handles the notification messages sent by ImageFilterProcessing.
//              Updates the GUI according to the received message 
//
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// BN              28032017	    1.0			Origin
// 
//////////////////////////////////////////////////////////////////////////////// 
void CMainFrame::NotifyProcImageFiltering(LPVOID lpParam, UINT nCode)
{
	CMainFrame* pFrame = (CMainFrame*)lpParam;

	CARALGISView* pView = static_cast<CARALGISView*>(pFrame->GetActiveView());

	if (pView)
	{
		switch (nCode)
		{
		case FILTER_PROCESS_FILTER1_READY:
			pView->FilterAvailable(1);
			break;

		case FILTER_PROCESS_FILTER2_READY:
			pView->FilterAvailable(2);
			break;

		case FILTER_PROCESS_FILTER3_READY:
			pView->FilterAvailable(3);
			break;

		default:
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CMainFrame::Activate
// 
// DESCRIPTION:	Starts all of the threads during window creation.
//
////////////////////////////////////////////////////////////////////////////////
void CMainFrame::Activate()
{
	HWND    hPrevWnd;

	TCHAR szTitle[100] = _T("ARALGIS");       // The title bar text
	TCHAR szWindowClass[100] = _T("ARALGIS");	    // The title bar text
	TCHAR szMutexName[100] = _T("ARALGISMutex");  // Mutex name

	//
	// Detect whether another instance of our application already exists.
	// 
	m_hSemaphore = CreateOneAppMutex(szMutexName);

	if (!m_hSemaphore)
	{
		// Another instance of our application already exists!

		::MessageBox(NULL, (LPCWSTR)L"ARALGIS Program Already Running.\nThis instance will shutdown",
			WARNINGWINDOW_TITLE, MB_ICONSTOP);

		// Get that instance window handle
		hPrevWnd = ::FindWindow(szWindowClass, szTitle);

		// Bring this window to front
		if (::IsIconic(hPrevWnd))
			::ShowWindow(hPrevWnd, SW_RESTORE);

		::SetForegroundWindow(hPrevWnd);

		g_isProgramStarted = FALSE;

		AfxGetMainWnd()->PostMessage(WM_CLOSE, 0, 0);
	}
	else
	{
		g_isProgramStarted = TRUE;

		MyRegisterClass(AfxGetInstanceHandle());

		//// initilize the local printer list 
		//CARALGISView* pView = static_cast<CARALGISView*>(GetActiveView());
		////pView->Initialize();

		// start the thread for receiving camera data
		m_CameraDataReceiver = new CCameraDataReceiver;
		if (!m_CameraDataReceiver->Start(NotifyProcCameraComm, this))
		{
			delete m_CameraDataReceiver;
			m_CameraDataReceiver = NULL;

			::MessageBox( NULL,
						 (LPCWSTR)L"Program Ba�lat�l�rken Hata Olu�tu: Kamera",
						 (LPCWSTR)WARNINGWINDOW_TITLE,
						 MB_OK | MB_ICONERROR
						);
		}

		// start the thread for PTS communication
		m_PTSCommunicator = new CPTSCommunicator;
		if (!m_PTSCommunicator->Start(NotifyProcPTSComm, this))
		{
			delete m_PTSCommunicator;
			m_PTSCommunicator = NULL;

			::MessageBox( NULL,
						 (LPCWSTR)L"Program Ba�lat�l�rken Hata Olu�tu: Plaka Tan�ma Sistemi",
						 (LPCWSTR)WARNINGWINDOW_TITLE,
						 MB_OK | MB_ICONERROR
						);
		}

		// start the thread for Odroid communication
		m_OdroidCommunicator = new COdroidCommunicator;
		if (!m_OdroidCommunicator->Start(NotifyProcOdroidComm, this))
		{
			delete m_OdroidCommunicator;
			m_OdroidCommunicator = NULL;

			::MessageBox( NULL,
						 (LPCWSTR)L"Program Ba�lat�l�rken Hata Olu�tu: �evre Birimler",
						 (LPCWSTR)WARNINGWINDOW_TITLE,
						 MB_OK | MB_ICONERROR
						);
		}

		// start the thread for Car Image - Database processing
		m_CameraDatabaseServer = new CCameraDBServer;
		if (!m_CameraDatabaseServer->Start(this))
		{
			delete m_CameraDatabaseServer;
			m_CameraDatabaseServer = NULL;

			::MessageBox( NULL,
						  (LPCWSTR)L"Program Ba�lat�l�rken Hata Olu�tu: VeriTaban�",
						  (LPCWSTR)WARNINGWINDOW_TITLE,
						  MB_OK | MB_ICONERROR
				        );
		}


		
		// start the thread for Imahe Giltering
		m_ImageFilterProcessing = new CImageFilterProcessing;
		if (!m_ImageFilterProcessing->Start(NotifyProcImageFiltering, this))
		{
			delete m_ImageFilterProcessing;
			m_ImageFilterProcessing = NULL;

			::MessageBox(NULL,
				(LPCWSTR)L"Program Ba�lat�l�rken Hata Olu�tu: Filtreleme",
				(LPCWSTR)WARNINGWINDOW_TITLE,
				MB_OK | MB_ICONERROR
				);
		}

	}
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CMainFrame::CreateOneAppMutex
// 
// DESCRIPTION: Creates a mutex only for one application
//              This function tries to create a mutex, but if specified mutex exits
//              this function returns NULL.
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// Name				Date		Version		Comments
// BN            26062003	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
HANDLE CMainFrame::CreateOneAppMutex(LPCTSTR lpName)
{
	HANDLE hSemaphore;

	// Create semaphore
	hSemaphore = CreateSemaphore(NULL, MAX_PROG_INSTANCE, MAX_PROG_INSTANCE, lpName);

	switch (GetLastError())
	{
	case ERROR_SUCCESS:
		//
		// Mutex created successfully. There is no instances running
		//
		break;

	case ERROR_ALREADY_EXISTS:
		//
		// Mutex already exists so there is a running instance of our app.
		//
		hSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, lpName);

		break;

	default:
		//
		// Failed to create mutex by unkniown reason
		//
		break;
	}

	if (hSemaphore)
	{
		DWORD dwWaitResult;

		// Try to enter the semaphore gate.

		dwWaitResult = WaitForSingleObject(hSemaphore,   // handle to semaphore
			0L);      // zero-second time-out interval

		switch (dwWaitResult)
		{
			// The semaphore object was signaled.
		case WAIT_OBJECT_0:
			// OK to start another application.
			return hSemaphore;
			break;

			// Semaphore was nonsignaled, so a time-out occurred.
		case WAIT_TIMEOUT:
			// Cannot start another application.
			return NULL;
			break;
		}
	}

	return hSemaphore;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	CMainFrame::MyRegisterClass
// 
// DESCRIPTION: Registers the window class.
//              The application will get 'well formed' small icons associated
//              with it.
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// Name				Date		Version		Comments
// BN            26062003	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
ATOM CMainFrame::MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	//wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)_T("ARALGIS"));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = (LPCTSTR)_T("ARALGIS");
	wcex.lpszClassName = _T("ARALGIS");
	//wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}



// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers



void CMainFrame::OnMoving(UINT fwSide, LPRECT pRect)
{
	CFrameWnd::OnMoving(fwSide, pRect);

	// TODO: Add your message handler code here

	CRect workArea;
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);

	pRect->left = workArea.left;
	pRect->top = workArea.top;
	pRect->right = workArea.right;
	pRect->bottom = workArea.bottom;
}


//void CMainFrame::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
//{
//	// TODO: Add your message handler code here and/or call default
//
//	if (nHitTest != HTCAPTION)
//	{
//		// invoke the super-class (default) behavior 
//		CFrameWnd::OnNcLButtonDblClk(nHitTest, point);
//	}
//}

