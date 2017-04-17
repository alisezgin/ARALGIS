
// ARALGISView.cpp : implementation of the CARALGISView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.

#include "afxbutton.h"

//#include <gdiplus.h>
//#pragma comment(lib, "gdiplus.lib")

#ifndef SHARED_HANDLERS
#include "ARALGIS.h"
#endif

#include "MainFrm.h"

#include "ARALGISDoc.h"
#include "ARALGISView.h"
#include "CameraStartStop.h"
#include ".\\ChangeDetection\\HeaderFiles\\ChangeDetectController.h"

#include ".\ImageFiltering\HeaderFiles\pixkit-timer.hpp"
#include ".\ImageFiltering\HeaderFiles\pixkit-file.hpp"
#include ".\ImageFiltering\HeaderFiles\pixkit-ml.hpp"
#include ".\ImageFiltering\HeaderFiles\pixkit-image.hpp"
#include ".\ImageFiltering\HeaderFiles\pixkit-cv.hpp"
#include ".\ImageFiltering\HeaderFiles\cvt.hpp"


#ifdef _DEBUG
#define new DEBUG_NEW 
#endif


// CARALGISView

IMPLEMENT_DYNCREATE(CARALGISView, CFormView)

BEGIN_MESSAGE_MAP(CARALGISView, CFormView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_MESSAGE(WM_CAMERA_DATA_READY, &CARALGISView::OnCameraDataReady)
	ON_COMMAND(ID_KAMERA_KONFIG32771, &CARALGISView::OnCameraConfig)
	ON_COMMAND(ID_KAMERA_KAY32772, &CARALGISView::OnSelectRecording)
	ON_COMMAND(ID_RESIMG32774, &CARALGISView::OnDisplaySelectedBitmap)
	ON_COMMAND(ID_RESIMG32775, &CARALGISView::OnDisplayOpenCV)
	ON_COMMAND(ID_KAMERA_DURUDU, &CARALGISView::OnCameraStartStop)
	ON_BN_CLICKED(IDC_BUTTON_BARRIER_OPEN, &CARALGISView::OnBnClickedButtonBarrierOpen)
	ON_BN_CLICKED(IDC_BUTTON_BARRIER_CLOSE, &CARALGISView::OnBnClickedButtonBarrierClose)
	ON_BN_CLICKED(IDC_BUTTON_HEATER_ON, &CARALGISView::OnBnClickedButtonHeaterOn)
	ON_BN_CLICKED(IDC_BUTTON_HEATER_OFF, &CARALGISView::OnBnClickedButtonHeaterOff)
	ON_BN_CLICKED(IDC_BUTTON_ALARM_ON, &CARALGISView::OnBnClickedButtonAlarmOn)
	ON_BN_CLICKED(IDC_BUTTON_ALARM_OFF, &CARALGISView::OnBnClickedButtonAlarmOff)
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CARALGISView construction/destruction

CARALGISView::CARALGISView() : CColorFormView(CARALGISView::IDD)
, m_PlakaStr(_T(""))
{
	// TODO: add construction code here
	//for (int i = 0; i < MAX_BUFFER; i++)
	//{
	//	m_selList[i] = TRUE;
	//}

	m_RefImgBMP = new CShowpic;
	m_TestImgBMP = new CShowpic;

	strcpy_s(m_FilenameRef, "C:/Users/bora/Desktop/FUZYON-SW-Dev/SW-Projects/uvss-images/new/1600/car-1-handCropped.bmp\0");
	strcpy_s(m_FilenameTest, "C:/Users/bora/Desktop/FUZYON-SW-Dev/SW-Projects/uvss-images/new/1600/car-1Diff4-handCropped.bmp\0");
}

CARALGISView::~CARALGISView()
{
	if (m_RefImgBMP)
		delete m_RefImgBMP;

	if (m_TestImgBMP)
		delete m_TestImgBMP;
}

void CARALGISView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BMP_REFERENCE, *m_RefImgBMP);
	DDX_Control(pDX, IDC_BMP_TEST, *m_TestImgBMP);

	DDX_Text(pDX, IDC_EDIT_PLAKA, m_PlakaStr);
	DDX_Control(pDX, IDC_EDIT_PLAKA, m_PlakaCtrl);
	DDX_Control(pDX, IDC_STATIC_PLAKA, m_CarPlakaImageStatic);
}

BOOL CARALGISView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CColorFormView::PreCreateWindow(cs);
}

// CARALGISView drawing

void CARALGISView::OnDraw(CDC* /*pDC*/)
{
	CARALGISDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


void CARALGISView::OnInitialUpdate()
{
	CColorFormView::OnInitialUpdate();

	SetBackgroundColor(RGB(0, 128, 128));

	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	///// LOAD openCV --> bitmap picture

	// TODO: Add extra initialization here
	BOOL bOk = FALSE;
	bOk = m_resizer.Hook(this);
	ASSERT(bOk);

	CUIntArray arrID;
	arrID.Add(IDC_GRP_TEST);
	arrID.Add(IDC_BMP_TEST);

	bOk = m_resizer.CreatePanel(_T("Test_Panel"), &arrID, TRUE);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(IDC_GRP_TEST, ANCHOR_ALL);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(IDC_BMP_TEST, ANCHOR_ALL);
	ASSERT(bOk);

	arrID.RemoveAll();

	arrID.Add(IDC_GRP_REF);
	arrID.Add(IDC_BMP_REFERENCE);

	bOk = m_resizer.CreatePanel(_T("Ref_Panel"), &arrID, TRUE);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(IDC_GRP_REF, ANCHOR_ALL);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(IDC_BMP_REFERENCE, ANCHOR_ALL);
	ASSERT(bOk);

	bOk = m_resizer.CreateSplitContainer(_T("Splitter"), _T("Test_Panel"), _T("Ref_Panel"));
	ASSERT(bOk);

	bOk = m_resizer.SetShowSplitterGrip(_T("Splitter"), TRUE);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(_T("Splitter"), ANCHOR_ALL);
	ASSERT(bOk);

	CSize size(0, 200); // cy member will be ignored
	bOk = m_resizer.SetMinimumSize(_T("Test_Panel"), size);
	ASSERT(bOk);

	CSize size2(0, 200); // cy member will be ignored
	bOk = m_resizer.SetMinimumSize(_T("Ref_Panel"), size2);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(IDC_STATIC_PTS, ANCHOR_RIGHT);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(IDC_BUTTON_PTS_STATUS, ANCHOR_RIGHT);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(IDC_STATIC_BARRIER, ANCHOR_RIGHT);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(IDC_BUTTON_PERIPHERAL_STATUS, ANCHOR_RIGHT);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(IDC_BUTTON_BARRIER_OPEN, ANCHOR_RIGHT);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(IDC_BUTTON_BARRIER_CLOSE, ANCHOR_RIGHT);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(IDC_BUTTON_HEATER_ON, ANCHOR_RIGHT);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(IDC_BUTTON_HEATER_OFF, ANCHOR_RIGHT);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(IDC_BUTTON_ALARM_ON, ANCHOR_RIGHT);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(IDC_BUTTON_ALARM_OFF, ANCHOR_RIGHT);
	ASSERT(bOk);




	CRect workArea;
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
	AfxGetMainWnd()->MoveWindow(workArea.left, workArea.top, workArea.right, workArea.bottom);


	m_resizer.SetShowResizeGrip(TRUE);

	bOk = m_resizer.InvokeOnResized();
	ASSERT(bOk);

	CString sDebugInfo = m_resizer.GetDebugInfo();

	m_PeripheralStatus.SubclassDlgItem(IDC_BUTTON_PERIPHERAL_STATUS, this);
	m_PTS_Status.SubclassDlgItem(IDC_BUTTON_PTS_STATUS, this);

	m_PeripheralStatus.SetColour(WHITE, RED);
	m_PTS_Status.SetColour(WHITE, RED);
}


// CARALGISView printing

BOOL CARALGISView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CARALGISView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CARALGISView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CARALGISView diagnostics

#ifdef _DEBUG
void CARALGISView::AssertValid() const
{
	CView::AssertValid();
}

void CARALGISView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CARALGISDoc* CARALGISView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CARALGISDoc)));
	return (CARALGISDoc*)m_pDocument;
}
#endif //_DEBUG


// CARALGISView message handlers




afx_msg LRESULT CARALGISView::OnCameraDataReady(WPARAM wParam, LPARAM lParam)
{
	cv::Mat dMat1, dMat2;

	g_CVImageTest = cv::imread("C:/Users/bora/Desktop/FUZYON-SW-Dev/SW-Projects/uvss-images/new/1600/car-1-handCropped.bmp", cv::IMREAD_COLOR);

	transpose(g_CVImageTest, dMat1);
	flip(dMat1, dMat2, 1); //transpose+flip(1)=CW
	dMat2.copyTo(g_CVImageTest);

	convertMattoBmpTest();

	//m_TestImgBMP->GetcvImage(g_CVImageTest);

	cv::namedWindow("GörüntüXX", cv::WINDOW_NORMAL);
	cv::imshow("GörüntüXX", g_CVImageTest);
	cv::waitKey(1500);

	// get Singleton ChangeDetectionController and start the change detection process
	CChangeDetectController::getInstance()->process(m_FilenameRef, m_FilenameTest);

	return 0;
}

void CARALGISView::convertMattoBmpTest()
{
	m_TestImgBMP->m_un32ImageWidth = g_CVImageTest.cols;
	m_TestImgBMP->m_un32ImageHeight = g_CVImageTest.rows;
	m_TestImgBMP->m_un32ImageBits = CAM_BITS;
	m_TestImgBMP->m_iSizeImage = (DWORD)g_CVImageTest.elemSize() * m_TestImgBMP->m_un32ImageWidth * m_TestImgBMP->m_un32ImageHeight;

	if (m_TestImgBMP->m_pImageBytes)
	{
		delete m_TestImgBMP->m_pImageBytes;
		m_TestImgBMP->m_pImageBytes = NULL;
	}
	
	m_TestImgBMP->m_pImageBytes = new BYTE[m_TestImgBMP->m_iSizeImage];

	m_TestImgBMP->MakeBMPHeader();

	//cv::cvtColor(*m_InImage, *m_InImage, CV_BGRA2RGBA);
	m_gdiPlus.Init();
	Gdiplus::Bitmap* pBitmap;
	pBitmap = m_gdiPlus.CopyMatToBmp(g_CVImageTest);

	Gdiplus::BitmapData i_Data;
	Gdiplus::Rect k_Rect(0, 0, pBitmap->GetWidth(), pBitmap->GetHeight());

	Gdiplus::PixelFormat e_Format;

	switch (g_CVImageTest.channels())
	{
		case 1: 
			e_Format = PixelFormat8bppIndexed; 
			break;

		case 3: 
			e_Format = PixelFormat24bppRGB;    
			break;

		case 4: 
			e_Format = PixelFormat32bppARGB;   
			break;

		default: 
			throw L"Image format not supported.";
	}

	if (Gdiplus::Ok != pBitmap->LockBits(&k_Rect, Gdiplus::ImageLockModeWrite, e_Format, &i_Data))
	{
		delete pBitmap;
		throw L"Error locking Bitmap.";
	}

	m_TestImgBMP->ShowImage((BYTE*)i_Data.Scan0);
	GetDocument()->UpdateAllViews(nullptr, 0, 0);
	pBitmap->UnlockBits(&i_Data);

	delete pBitmap;
}



void CARALGISView::OnCameraConfig()
{
	SetEvent(g_CameraConfigFileChangeEvent);
}


void CARALGISView::OnSelectRecording()
{
	SetEvent(g_CameraSelectStreamFileEvent);
}


void CARALGISView::OnDisplaySelectedBitmap()
{
	SetEvent(g_DisplayBitmapEvent);
}


void CARALGISView::OnDisplayOpenCV()
{
	SetEvent(g_DisplayOpenCVEvent);
}


void CARALGISView::OnCameraStartStop()
{
	CCameraStartStop dDlg;

	dDlg.DoModal();
}



void CARALGISView::DisplayPTSImage()
{
	// TODO: Add your command handler code here
	m_CarPlakaImageStatic.Load(g_CarPlakaImage, (size_t)m_CarPlakaImageLenght);

	//CString aCString = CString(_T("A string"));


	CString aCString = CString(_T("A string"));

	aCString = g_PlakaNoChars;
	CFont myFont;
	myFont.CreateFont(-80, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("Verdana"));
	
	m_PlakaCtrl.SetFont(&myFont);

	m_PlakaCtrl.SetWindowTextW(aCString);
}

void CARALGISView::DeletePTSImage()
{
	// TODO: Add your command handler code here

}

void CARALGISView::UpdatePTSStatus(bool aStatus)
{
	if (aStatus == true)
	{
		m_PTS_Status.SetColour(WHITE, GREEN);
	}
	else
	{
		m_PTS_Status.SetColour(WHITE, RED);
	}
}

void CARALGISView::UpdatePeripheralStatus(bool aStatus)
{
	if (aStatus == true)
	{
		m_PeripheralStatus.SetColour(WHITE, GREEN);
	}
	else
	{
		m_PeripheralStatus.SetColour(WHITE, RED);
	}
}


void CARALGISView::OnBnClickedButtonBarrierOpen()
{
	// TODO: Add your control notification handler code here
	SetEvent(g_OdroidOpenBarrierEvent);
}


void CARALGISView::OnBnClickedButtonBarrierClose()
{
	// TODO: Add your control notification handler code here
	SetEvent(g_OdroidCloseBarrierEvent);
}


void CARALGISView::OnBnClickedButtonHeaterOn()
{
	// TODO: Add your control notification handler code here
	SetEvent(g_OdroidStartHeatingEvent);
}


void CARALGISView::OnBnClickedButtonHeaterOff()
{
	// TODO: Add your control notification handler code here
	SetEvent(g_OdroidStopHeatingEvent);
}

void CARALGISView::OnBnClickedButtonAlarmOn()
{
	// TODO: Add your control notification handler code here
	SetEvent(g_OdroidStartAlarmEvent);
}


void CARALGISView::OnBnClickedButtonAlarmOff()
{
	// TODO: Add your control notification handler code here
	SetEvent(g_OdroidStopAlarmEvent);

	SetEvent(g_KillTimerEvent);
}


void CARALGISView::SetTimerPeriodCamera()
{
	// added by bora, start 1 second interval timer
	SetTimer(CAMERA_TIMER_ID, g_iTimerPeriod, NULL);
}

void CARALGISView::KillTimerCamera()
{
	KillTimer(CAMERA_TIMER_ID);
}


void CARALGISView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == CAMERA_TIMER_ID)
	{
		SetEvent(g_mCameraTimerEvent);
	} // End if.
	CColorFormView::OnTimer(nIDEvent);
}

