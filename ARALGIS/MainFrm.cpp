
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "ARALGIS.h"

#include "MainFrm.h"

#include "ARALGISView.h"

#include ".\\ErrorDisplay\\ThreadSafeQueue\\HeaderFiles\\ThreadSafeQueue.h"


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
	m_PTSCommunicatorISSD = NULL;
	m_PTSCommunicatorDVIT = NULL;
	m_OdroidCommunicator = NULL;
	m_CameraDatabaseServer = NULL;
	m_ImageFilterProcessing = NULL;
	m_VehicleDetector = NULL;
	m_ChangeDetector = NULL;
	m_DiskSpaceController = NULL;
	m_ErrorDisplayer = NULL;
}

CMainFrame::~CMainFrame()
{
	EnterCriticalSection(&g_ChangeDetectCS);

	if (m_CameraDataReceiver)
	{
		delete m_CameraDataReceiver;
	}

	if (m_PTSCommunicatorISSD)
	{
		delete m_PTSCommunicatorISSD;
	}

	if (m_PTSCommunicatorDVIT)
	{
		delete m_PTSCommunicatorDVIT;
	}

	if (m_OdroidCommunicator)
	{
		delete m_OdroidCommunicator;
	}

	if (m_CameraDatabaseServer)
	{
		delete m_CameraDatabaseServer;
	}

	if (m_ImageFilterProcessing)
	{
		delete m_ImageFilterProcessing;
	}

	if (m_VehicleDetector)
	{
		delete m_VehicleDetector;
	}

	if (m_ChangeDetector)
	{
		delete m_ChangeDetector;
	}

	if (m_DiskSpaceController)
	{
		delete m_DiskSpaceController;
	}

	if (m_ErrorDisplayer)
	{
		delete m_ErrorDisplayer;
	}

	//CThreadSafeQueue<_errorMessageType>::getInstance()->destroy();


	LeaveCriticalSection(&g_ChangeDetectCS);


	// Don't forget to release the semaphore
	if (!ReleaseSemaphore(m_hSemaphore,  // handle to semaphore
						  1,           // increase count by one
						  NULL))      // not interested in previous count
	{
		TRACE("Failed to release semaphore\n");
	}

	DeleteCriticalSection(&g_ChangeDetectCS);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	//{
	//	TRACE0("Failed to create toolbar\n");
	//	return -1;      // fail to create
	//}

	//if (!m_wndStatusBar.Create(this))
	//{
	//	TRACE0("Failed to create status bar\n");
	//	return -1;      // fail to create
	//}
	//m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));

	//// TODO: Delete these three lines if you don't want the toolbar to be dockable
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	//EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndToolBar);

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
	// boraN code analysis
	// If the above function fails, check what happens to workArea

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
		BOOL bIsSucessFul = m_IniFile.ReadIniFile();
		if (bIsSucessFul == TRUE)
		{
			g_isProgramStarted = TRUE;

			MyRegisterClass(AfxGetInstanceHandle());

			// initilize GUI 
			//CARALGISView* pView = static_cast<CARALGISView*>(GetActiveView());
			//pView->Initialize();

			// start the thread for receiving camera data
			m_CameraDataReceiver = new CCameraDataReceiver;
			if (!m_CameraDataReceiver->Start(this))
			{
				delete m_CameraDataReceiver;
				m_CameraDataReceiver = NULL;

				::MessageBox(NULL,
					(LPCWSTR)L"Program Baþlatýlýrken Hata Oluþtu: Kamera",
					(LPCWSTR)WARNINGWINDOW_TITLE,
					MB_OK | MB_ICONERROR
					);
			}

			// start the thread for PTS communication
			if (g_PTS_Producer_ID == PTS_BY_ISSD)
			{
				// start the thread for PTS communication
				m_PTSCommunicatorISSD = new CPTSCommunicatorISSD;
				if (!m_PTSCommunicatorISSD->Start(this))
				{
					delete m_PTSCommunicatorISSD;
					m_PTSCommunicatorISSD = NULL;

					::MessageBox(NULL,
						(LPCWSTR)L"Program Baþlatýlýrken Hata Oluþtu: Plaka Tanýma Sistemi",
						(LPCWSTR)WARNINGWINDOW_TITLE,
						MB_OK | MB_ICONERROR
						);
				}
			}
			else if (g_PTS_Producer_ID == PTS_BY_DIVIT)
			{
				// start the thread for PTS communication
				m_PTSCommunicatorDVIT = new CPTSCommunicatorDVIT;
				if (!m_PTSCommunicatorDVIT->Start(this))
				{
					delete m_PTSCommunicatorDVIT;
					m_PTSCommunicatorDVIT = NULL;

					::MessageBox(NULL,
						(LPCWSTR)L"Program Baþlatýlýrken Hata Oluþtu: Plaka Tanýma Sistemi",
						(LPCWSTR)WARNINGWINDOW_TITLE,
						MB_OK | MB_ICONERROR
						);
				}
			}

			// start the thread for Odroid communication
			m_OdroidCommunicator = new COdroidCommunicator;
			if (!m_OdroidCommunicator->Start(this))
			{
				delete m_OdroidCommunicator;
				m_OdroidCommunicator = NULL;

				::MessageBox(NULL,
					(LPCWSTR)L"Program Baþlatýlýrken Hata Oluþtu: Çevre Birimler",
					(LPCWSTR)WARNINGWINDOW_TITLE,
					MB_OK | MB_ICONERROR
					);
			}

			// start the threads for writing test image file to disk
			m_CameraDatabaseServer = new CCameraDBServer;
			if (!m_CameraDatabaseServer->Start(this))
			{
				delete m_CameraDatabaseServer;
				m_CameraDatabaseServer = NULL;

				::MessageBox(NULL,
					(LPCWSTR)L"Program Baþlatýlýrken Hata Oluþtu: VeriTabaný",
					(LPCWSTR)WARNINGWINDOW_TITLE,
					MB_OK | MB_ICONERROR
					);
			}

			// start the threads for Image Filtering
			m_ImageFilterProcessing = new CImageFilterProcessing;
			if (!m_ImageFilterProcessing->Start(this))
			{
				delete m_ImageFilterProcessing;
				m_ImageFilterProcessing = NULL;

				::MessageBox(NULL,
					(LPCWSTR)L"Program Baþlatýlýrken Hata Oluþtu: Filtreleme",
					(LPCWSTR)WARNINGWINDOW_TITLE,
					MB_OK | MB_ICONERROR
					);
			}

			// start the thread for VehicleDetection
			m_VehicleDetector = new CVehicleDetection;
			if (!m_VehicleDetector->Start(this))
			{
				delete m_VehicleDetector;
				m_VehicleDetector = NULL;

				::MessageBox(NULL,
					(LPCWSTR)L"Program Baþlatýlýrken Hata Oluþtu: Araç Tespiti",
					(LPCWSTR)WARNINGWINDOW_TITLE,
					MB_OK | MB_ICONERROR
					);
			}

			// start the thread for Change Detection
			m_ChangeDetector = new CChangeDetectController;
			if (!m_ChangeDetector->Start(this))
			{
				delete m_ChangeDetector;
				m_ChangeDetector = NULL;

				::MessageBox(NULL,
					(LPCWSTR)L"Program Baþlatýlýrken Hata Oluþtu: Deðiþiklik Tespiti",
					(LPCWSTR)WARNINGWINDOW_TITLE,
					MB_OK | MB_ICONERROR
					);
			}

			// start the threads for Disc Space Control
			m_DiskSpaceController = new CFreeDiskSpaceController;
			if (!m_DiskSpaceController->Start())
			{
				delete m_DiskSpaceController;
				m_DiskSpaceController = NULL;

				::MessageBox(NULL,
					(LPCWSTR)L"Program Baþlatýlýrken Hata Oluþtu: Boþ Disk Alaný Kontrolü",
					(LPCWSTR)WARNINGWINDOW_TITLE,
					MB_OK | MB_ICONERROR
					);
			}

			// start the threads for error display
			m_ErrorDisplayer = new CErrorDisplayer;
			if (!m_ErrorDisplayer->Start())
			{
				delete m_ErrorDisplayer;
				m_ErrorDisplayer = NULL;

				::MessageBox(NULL,
					(LPCWSTR)L"Program Baþlatýlýrken Hata Oluþtu: Hata Gösterimi",
					(LPCWSTR)WARNINGWINDOW_TITLE,
					MB_OK | MB_ICONERROR
					);
			}
		}
		else /// ini file fails
		{
			AfxGetMainWnd()->PostMessage(WM_CLOSE, 0, 0);
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

