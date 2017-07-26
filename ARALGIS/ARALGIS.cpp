
// ARALGIS.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "ARALGIS.h"
#include "MainFrm.h"

#include "ARALGISDoc.h"
#include "ARALGISView.h"
#include "afxwin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


///////// GLOBAL EVENTS start here ////////////////
///////////////////////////////////////////////////

// event used for starting camera data reception
HANDLE g_CameraStartDataRecieveEvent;

// event used for stopping camera data reception
HANDLE g_CameraStopDataRecieveEvent;

// event used for changing camera sample rate
HANDLE g_CameraChangeSampleRateEvent;

// event used for setting timer frame rate 
HANDLE g_SetTimerFrameRateEvent;

// event used for killing timer 
HANDLE g_KillTimerEvent;

// event used for triggering PTS 
HANDLE g_PTSTriggerEvent;

// event used for sending UVSS image to PTS 
HANDLE g_SendUVSSImageEvent;

// event used for triggering PTS 
HANDLE g_PTSLostEvent;

// event used for sending open barrier message to odroid 
HANDLE g_OdroidOpenBarrierEvent;

// event used for sending close barrier message to odroid  
HANDLE g_OdroidCloseBarrierEvent;

// event used for sending start heating message to odroid 
HANDLE g_OdroidStartHeatingEvent;

// event used for sending stop heating message to odroid 
HANDLE g_OdroidStopHeatingEvent;

// event used for sending start alarm message to odroid 
HANDLE g_OdroidStartAlarmEvent;

// event used for sending stop alarm message to odroid 
HANDLE g_OdroidStopAlarmEvent;

// event used for sending critical disk space message to odroid 
HANDLE g_OdroidDiskSpaceEvent;

// event used for sending PTS file written message to odroid 
HANDLE g_OdroidPTSFileWrittenEvent;

// event used for sending Image file written message to odroid 
HANDLE g_OdroidImageFileWrittenEvent;

// event used for sending error occured message to odroid 
HANDLE g_OdroidErrorOccuredEvent;

// event used for triggering CameraDBServer
HANDLE g_CameraDBServerWriteFileEvent;

// event used for triggering Filter-1 processing of test image
HANDLE g_ProcessFilter1Event;

// event used for triggering Filter-2 processing of test image
HANDLE g_ProcessFilter2Event;

// event used for triggering Filter-3 processing of test image
HANDLE g_ProcessFilter3Event;

// event used for triggering Filter-1 processing of test image
HANDLE g_ProcessFilter4Event;

// event used for triggering Filter-2 processing of test image
HANDLE g_ProcessFilter5Event;

// event used for triggering Filter-3 processing of test image
HANDLE g_ProcessFilter6Event;

// event used for triggering intermediate test image ready
HANDLE g_IntermediateImageReadyEvent;

// event used for starting change detection 
HANDLE g_StartChangeDetectEvent;

// event used for starting free disc space control 
HANDLE g_ControlHardDiskSpaceEvent;

// event used for interrupting change detection thread 
HANDLE g_StopChangeDetectionEvent;

// event for error message processing
HANDLE g_ErrorOccurredEvent;
///////////////////////////////////////////////////
////////// GLOBAL EVENTS ends here ////////////////


////////// GLOBAL CRITICAL SECTIONs start here ////////////
///////////////////////////////////////////////////////////

// Critical Section to protect isDatabaseHandlingInProgress
CRITICAL_SECTION g_IntermediateTestImgCS;

// Critical Section to protect RefImage 
CRITICAL_SECTION   RefImageCS;

// Critical Section to protect TestImage 
CRITICAL_SECTION   TestImageCS;

// Critical Section to shutdown after change detection is finished
CRITICAL_SECTION g_ChangeDetectCS;

// Critical Section to protect intermediate test Images
CRITICAL_SECTION g_DBFileWriteCS;

// Critical Section to protect Error Messages Queue
CRITICAL_SECTION g_QueueErrorMessageCS;
///////////////////////////////////////////////////////////
////////// GLOBAL CRITICAL SECTIONs start here ////////////

//// GLOBAL DATA VARIABLES start here////////////////
/////////////////////////////////////////////////////

// global variable to hold openCV test image from Camera
cv::Mat g_CVImageTest;

// global variable to hold filtered #1 openCV test image 
cv::Mat g_CVImageTestFilter1;

// global variable to hold filtered #2 openCV test image 
cv::Mat g_CVImageTestFilter2;

// global variable to hold filtered #3 openCV test image 
cv::Mat g_CVImageTestFilter3;

// global variable to hold the image in byte array to be sent to PTS
BYTE* g_ByteImageTest;
int unsigned g_ByteImageSize;

// global variable to hold openCV reference image from Camera
cv::Mat g_CVImageRef;

// global variable to hold filtered #1 openCV ref image 
cv::Mat g_CVImageRefFilter1;

// global variable to hold filtered #2 openCV ref image 
cv::Mat g_CVImageRefFilter2;

// global variable to hold filtered #3 openCV ref image 
cv::Mat g_CVImageRefFilter3;

// global variable for change detection result
cv::Mat g_CVTestChangeDetect;

// global variable for perspective wrapped test image
cv::Mat g_CVTestWrapped;

// global variable to hold openCV intermediate test image from Camera
cv::Mat g_CVImageTestIntermediate[NUM_INTERMEDIATE_TEST_IMAGE+1];

// global variable to hold counter of  openCV intermediate test image from Camera
int g_IntermediateCounter;

// global variable to hold car PLAKA NUMBER
char g_PlakaNoChars[PLAKA_CHAR_LENGTH+1];

// global variable to hold car FRONTAL/PLAKA IMAGE 
BYTE* g_CarPlakaImage;
int  g_CarPlakaImageLenght;

// global variable to hold stored image files directory path
char g_ImageFilesDirPath[MAX_DIR_PATH_LENGTH];

// global variable to hold test image filename (includes directory path)
//char g_TestImageFileName[MAX_DIR_PATH_LENGTH];

// global variable to hold reference image filename (includes directory path)
char g_RefImageFileName[MAX_DIR_PATH_LENGTH];

// test image indices
int g_dBeginIndex;
int g_dEndIndex;
int g_dCarDetectCount;

// configuration file (INI file) variables starts here
int  g_CameraWidth;
int  g_CameraHeight;
int  g_CameraPixelBits;
int  g_CameraBufferSize;
char g_Odroid_Port[PORT_BYTE_LEN + 1];
char g_PTSPort[PORT_BYTE_LEN + 1];
char g_PTSIP[IP_BYTE_LEN + 1];
char g_ReferenceFilePath[MAX_DIR_PATH_LENGTH + 1];
char g_CameraConfigFilename[MAX_FILENAME_LENGTH + 1];

bool g_ChangeDetectActive;
// boraN INI ends

cv::Mat g_CVRefBigWindow;
cv::Mat g_CVTestBigWindow;
std::string g_TestImageFileName;
/////////////////////////////////////////////////////
//// GLOBAL DATA VARIABLES end here////////////////

////// GLOBAL CONTROL VARIABLES start here /////////////////////////
////////////////////////////////////////////////////////////////////

// variable to control whether the program is started.
// if started, during exit the database  connections are closed
// eitherwise the connections are not tried to be closed
BOOL g_isProgramStarted;

// something like a counting semaphore for thread sync
int g_carsDetectedByPTSCnt;

// defines what type of code will be used for PTS comm
int g_PTS_Producer_ID;

// defines operation mode of PTS (trigger/continuous)
int g_PTS_Mode;

// defines is automatic vehicle will be used
bool g_Use_Auto_VehicleDetect;

// defines if PTS is DIVIT and g_Use_Auto_VehicleDetect is TRUE
// use full camera data for vehicle detection
int g_AutoDetect_Type;

BOOL g_CarFound;

BOOL g_IsOdroidStartReceived;
BOOL g_IsAnswerReceivedFromPTS;
////////////////////////////////////////////////////////////////////
////// GLOBAL CONTROL VARIABLES end here /////////////////////////



// CARALGISApp

BEGIN_MESSAGE_MAP(CARALGISApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CARALGISApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CARALGISApp construction

CARALGISApp::CARALGISApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("ARALGIS.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CARALGISApp object

CARALGISApp theApp;


BOOL CARALGISApp::InitInstance()
{
	/// memory leak detection begins
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	/// memory leak detection ends

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Utarit ARALGIS"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)


	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CARALGISDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CARALGISView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	CFrameWnd *pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
	CView * pView = pFrame->GetActiveView();

	// added by bora  --->not a debug debugBN
	//((CMainFrame*)m_pMainWnd)->Activate();
	((CMainFrame*)m_pMainWnd)->Activate();


	return TRUE;
}

int CARALGISApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);

	_CrtDumpMemoryLeaks();


	return CWinApp::ExitInstance();
}

// CARALGISApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CARALGISApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CARALGISApp message handlers


