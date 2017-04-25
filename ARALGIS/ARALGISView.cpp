
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
#include ".\\ChangeDetection\\HeaderFiles\\ChangeDetectController.h"

#include ".\ImageFiltering\HeaderFiles\pixkit-timer.hpp"
#include ".\ImageFiltering\HeaderFiles\pixkit-file.hpp"
#include ".\ImageFiltering\HeaderFiles\pixkit-ml.hpp"
#include ".\ImageFiltering\HeaderFiles\pixkit-image.hpp"
#include ".\ImageFiltering\HeaderFiles\pixkit-cv.hpp"
#include ".\ImageFiltering\HeaderFiles\cvt.hpp"



//#include ".\\BitmapDisplay\\HeaderFiles\\PkMattoGDI.h"


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
	ON_MESSAGE(WM_DBASE_CAR_INFO_READY, &CARALGISView::OnDBaseCarInfoReady)
	ON_COMMAND(ID_KAMERA_KONFIG32771, &CARALGISView::OnCameraConfig)
	ON_COMMAND(ID_KAMERA_KAY32772, &CARALGISView::OnSelectRecording)
	ON_COMMAND(ID_RESIMG32774, &CARALGISView::OnDisplaySelectedBitmap)
	ON_COMMAND(ID_RESIMG32775, &CARALGISView::OnDisplayOpenCV)
	ON_BN_CLICKED(IDC_BUTTON_BARRIER_OPEN, &CARALGISView::OnBnClickedButtonBarrierOpen)
	ON_BN_CLICKED(IDC_BUTTON_BARRIER_CLOSE, &CARALGISView::OnBnClickedButtonBarrierClose)
	ON_BN_CLICKED(IDC_BUTTON_HEATER_ON, &CARALGISView::OnBnClickedButtonHeaterOn)
	ON_BN_CLICKED(IDC_BUTTON_HEATER_OFF, &CARALGISView::OnBnClickedButtonHeaterOff)
	ON_BN_CLICKED(IDC_BUTTON_ALARM_ON, &CARALGISView::OnBnClickedButtonAlarmOn)
	ON_BN_CLICKED(IDC_BUTTON_ALARM_OFF, &CARALGISView::OnBnClickedButtonAlarmOff)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_FILTER1, &CARALGISView::OnBnClickedButtonFilter1)
	ON_BN_CLICKED(IDC_BUTTON_FILTER2, &CARALGISView::OnBnClickedButtonFilter2)
	ON_BN_CLICKED(IDC_BUTTON_FILTER3, &CARALGISView::OnBnClickedButtonFilter3)
	ON_LBN_SELCHANGE(IDC_FORM_LISTBOX, &CARALGISView::OnLbnSelchangeList2)
	ON_BN_CLICKED(IDC_FORM_ADDCOMMAND, &CARALGISView::OnAdd)
	ON_BN_CLICKED(IDC_FORM_BMODIFY, &CARALGISView::OnGuncelle)
END_MESSAGE_MAP()

// CARALGISView construction/destruction

CARALGISView::CARALGISView() : CColorFormView(CARALGISView::IDD)
, m_PlakaStr(_T(""))

, m_FormLPEntry(_T(""))
, m_FormELPI(_T(""))
, m_FormEFVI(_T(""))
, m_FormECBCI(_T(""))
, m_FormECBRI(_T(""))
, m_FormDN(_T(""))
{
	// TODO: add construction code here

	m_RefImgBMP = new CStatic;
	m_TestImgBMP = new CStatic;

	m_MatToGDITest = NULL;
	m_MatToGDIRef = NULL;

	m_iDisplayTestImageType = 0;


	strcpy_s(m_FilenameRef, "C:/Users/bora/Desktop/FUZYON-SW-Dev/SW-Projects/uvss-images/new/1600/car-1-handCropped.bmp\0");
	strcpy_s(m_FilenameTest, "C:/Users/bora/Desktop/FUZYON-SW-Dev/SW-Projects/uvss-images/new/1600/car-1Diff4-handCropped.bmp\0");
}

CARALGISView::~CARALGISView()
{
	if (m_RefImgBMP)
		delete m_RefImgBMP;

	if (m_TestImgBMP)
		delete m_TestImgBMP;

	if (m_MatToGDITest)
		delete m_MatToGDITest;

	if (m_MatToGDIRef)
		delete m_MatToGDIRef;

	if (g_CarPlakaImage)
		delete g_CarPlakaImage;
}


void CARALGISView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BMP_REFERENCE, *m_RefImgBMP);
	DDX_Control(pDX, IDC_BMP_TEST, *m_TestImgBMP);
	DDX_Text(pDX, IDC_EDIT_PLAKA, m_PlakaStr);
	DDX_Control(pDX, IDC_EDIT_PLAKA, m_PlakaCtrl);
	DDX_Control(pDX, IDC_STATIC_PLAKA, m_CarPlakaImageStatic);
	DDX_Control(pDX, IDC_BUTTON_BARRIER_OPEN, m_BarrierOpenBtn);
	DDX_Control(pDX, IDC_BUTTON_BARRIER_CLOSE, m_BarrierCloseBtn);
	DDX_Control(pDX, IDC_BUTTON_HEATER_ON, m_HeaterStartBtn);
	DDX_Control(pDX, IDC_BUTTON_HEATER_OFF, m_HeterStopBtn);
	DDX_Control(pDX, IDC_BUTTON_ALARM_ON, m_AlarmStartBtn);
	DDX_Control(pDX, IDC_BUTTON_ALARM_OFF, m_AlarmStop);
	DDX_Control(pDX, IDC_FORM_LISTBOX, m_FormListBox);
	DDX_Text(pDX, IDC_FORM_LPENTRY, m_FormLPEntry);
	DDX_Text(pDX, IDC_FORM_ELP, m_FormELP);
	DDX_Text(pDX, IDC_FORM_ELPI, m_FormELPI);
	DDX_Text(pDX, IDC_FORM_EFVI, m_FormEFVI);
	DDX_Text(pDX, IDC_FORM_ECBCI, m_FormECBCI);
	DDX_Text(pDX, IDC_FORM_ECBRI, m_FormECBRI);
	DDX_Text(pDX, IDC_FORM_DN, m_FormDN);
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

	SetBackgroundColor(RGB(240, 240, 240));

	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

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

	m_BarrierOpenBtn.EnableWindow(FALSE);
	m_BarrierCloseBtn.EnableWindow(FALSE);
	m_HeaterStartBtn.EnableWindow(FALSE);
	m_HeterStopBtn.EnableWindow(FALSE);
	m_AlarmStartBtn.EnableWindow(FALSE);
	m_AlarmStop.EnableWindow(FALSE);
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

	//g_CVImageTest = cv::imread("C:/Users/bora/Desktop/FUZYON-SW-Dev/SW-Projects/uvss-images/new/1600/car-1-handCropped.bmp", cv::IMREAD_COLOR);

	transpose(g_CVImageTest, dMat1);
	flip(dMat1, dMat2, 1); //transpose+flip(1)=CW
	dMat2.copyTo(g_CVImageTest);


	if (m_MatToGDITest != NULL)
	{
		delete m_MatToGDITest;
		m_MatToGDITest = NULL;
	}

	m_iDisplayTestImageType = 0;

	m_MatToGDITest = new PkMatToGDI(m_TestImgBMP, false);
	m_MatToGDITest->DrawImg(g_CVImageTest);

    // code to be deleted when test image
	// display is stabilized.........
	// code deletion starts here
	cv::namedWindow("G�r�nt�XX", cv::WINDOW_NORMAL);
	cv::imshow("G�r�nt�XX", g_CVImageTest);
	cv::waitKey(150);
	// code deletion ends here

	// code to be deleted when dbase handler
	// thread is added
	// code deletion starts here
	LPARAM pLparam;
	pLparam = reinterpret_cast<LPARAM>("ARALGIS");
	SendMessage(WM_DBASE_CAR_INFO_READY, 0, pLparam);
	// code deletion ends here

	// get Singleton ChangeDetectionController and start the change detection process
	CChangeDetectController::getInstance()->process(m_FilenameRef, m_FilenameTest);

	return 0;
}


afx_msg LRESULT CARALGISView::OnDBaseCarInfoReady(WPARAM wParam, LPARAM lParam)
{
	//g_CVImageRef = cv::imread(g_RefImageFileName, cv::IMREAD_COLOR);

	g_CVImageRef = cv::imread("C:/Users/bora/Desktop/FUZYON-SW-Dev/SW-Projects/uvss-images/new/1600/car-1-handCropped.bmp", cv::IMREAD_COLOR);

	// no need to transpose+flip
	// since image files are written landscape!!!
	// for the time being for testing purposes
	// these operations are needed.
	// delete below code for real system
	// code deletion starts here
	cv::Mat dMat1, dMat2;
	transpose(g_CVImageRef, dMat1);
	flip(dMat1, dMat2, 1); //transpose+flip(1)=CW
	dMat2.copyTo(g_CVImageRef);
	// code deletion ends here


	if (m_MatToGDIRef != NULL)
	{
		delete m_MatToGDIRef;
		m_MatToGDIRef = NULL;
	}

	m_MatToGDIRef = new PkMatToGDI(m_RefImgBMP, false);
	m_MatToGDIRef->DrawImg(g_CVImageRef);

	return 0;
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




void CARALGISView::DisplayPTSImage()
{
	// TODO: Add your command handler code here
	m_CarPlakaImageStatic.Load(g_CarPlakaImage, (size_t)g_CarPlakaImageLenght);

	CString aCString;
	CString bCString = CString(_T("         "));

	aCString = g_PlakaNoChars;

	CFont font;
	VERIFY(font.CreateFont(
		40,                        // nHeight
		20,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Arial")));                 // lpszFacename



	// Done with the font.  Delete the font object.

	RECT r;
	CWnd* h = GetDlgItem(IDC_EDIT_PLAKA);
	h->GetWindowRect(&r); //get window rect of control relative to screen
	POINT pt = { r.left, r.top }; //new point object using rect x, y
	ScreenToClient(&pt); //convert screen co-ords to client based points

	// Do something with the font just created...
	CClientDC dc(this);
	CFont* def_font = dc.SelectObject(&font);
	dc.TextOut(pt.x, pt.y, bCString, 9);
	dc.TextOut(pt.x, pt.y, aCString, 8);
	dc.SelectObject(def_font);

	font.DeleteObject();

	SetTimerDisplay();
}

void CARALGISView::DeletePTSImage()
{
	// TODO: Add your command handler code here
	//m_CarPlakaImageStatic.FreeData();
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
		m_BarrierOpenBtn.EnableWindow(TRUE);
		m_BarrierCloseBtn.EnableWindow(TRUE);
		m_HeaterStartBtn.EnableWindow(TRUE);
		m_HeterStopBtn.EnableWindow(TRUE);
		m_AlarmStartBtn.EnableWindow(TRUE);
		m_AlarmStop.EnableWindow(TRUE);
	}
	else
	{
		m_PeripheralStatus.SetColour(WHITE, RED);
		m_BarrierOpenBtn.EnableWindow(FALSE);
		m_BarrierCloseBtn.EnableWindow(FALSE);
		m_HeaterStartBtn.EnableWindow(FALSE);
		m_HeterStopBtn.EnableWindow(FALSE);
		m_AlarmStartBtn.EnableWindow(FALSE);
		m_AlarmStop.EnableWindow(FALSE);
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

	//CColorDialog dlg;

	//INT_PTR nRet = dlg.DoModal();

	//if (IDOK == nRet)
	//{
	//	COLORREF crBackground = dlg.GetColor();
	//	// set dialog background color
	//	SetBackgroundColor(crBackground);

	//	// see action
	//	RedrawWindow();
	//}
}


void CARALGISView::SetTimerDisplay()
{
	// added by bora, start 1 second interval timer
	m_TimerSecondCounter = 0;
	KillTimer(DISPLAY_TIMER_ID);
	SetTimer(DISPLAY_TIMER_ID, TIMER_PERIOD_IN_MS, NULL);
}

void CARALGISView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == DISPLAY_TIMER_ID)
	{
		m_TimerSecondCounter++;

		if (m_TimerSecondCounter == PLAKA_TIME_ON_SCREEN)
		{
			// do something
			DeletePTSImage();
			KillTimer(DISPLAY_TIMER_ID);
		}
	} // End if.
	CColorFormView::OnTimer(nIDEvent);
}



void CARALGISView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CColorFormView::OnPaint() for painting messages

	if (m_MatToGDITest != NULL)
	{
		delete m_MatToGDITest;
		m_MatToGDITest = NULL;
		m_MatToGDITest = new PkMatToGDI(m_TestImgBMP, false);

		if (g_CVImageTest.rows != 0)
		{
			if (m_iDisplayTestImageType == 0)
			{
				m_MatToGDITest->DrawImg(g_CVImageTest);
			}
			else if (m_iDisplayTestImageType == 1)
			{
				m_MatToGDITest->DrawImg(g_CVImageTestFilter1);
			}
			else if (m_iDisplayTestImageType == 2)
			{
				m_MatToGDITest->DrawImg(g_CVImageTestFilter2);
			}
			else if (m_iDisplayTestImageType == 3)
			{
				m_MatToGDITest->DrawImg(g_CVImageTestFilter3);
			}
		}
	}



	if (m_MatToGDIRef != NULL)
	{
		delete m_MatToGDIRef;
		m_MatToGDIRef = NULL;

		m_MatToGDIRef = new PkMatToGDI(m_RefImgBMP, false);

		if (g_CVImageRef.rows != 0)
		{
			m_MatToGDIRef->DrawImg(g_CVImageRef);
		}
	}

}


BOOL CARALGISView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return CColorFormView::OnEraseBkgnd(pDC);
}


void CARALGISView::OnBnClickedButtonFilter1()
{
	if (g_CVImageTest.cols > 0)
	{
		m_iDisplayTestImageType = 1;
		pixkit::enhancement_local::MSRCP2014(g_CVImageTest, g_CVImageTestFilter1, 15, 127, 255, (float)0.1, (float)0.1);
		m_MatToGDITest->DrawImg(g_CVImageTestFilter1);
	}

	SetEvent(g_ProcessFilter1Event);
}


void CARALGISView::OnBnClickedButtonFilter2()
{
	if (g_CVImageTest.cols > 0)
	{
		m_iDisplayTestImageType = 2;

		cv::Mat inGray;
		cv::cvtColor(g_CVImageTest, inGray, CV_RGB2GRAY);

		pixkit::enhancement_global::GlobalHistogramEqualization1992(inGray, g_CVImageTestFilter2);
		m_MatToGDITest->DrawImg(g_CVImageTestFilter2);
	}
	SetEvent(g_ProcessFilter2Event);
}


void CARALGISView::OnBnClickedButtonFilter3()
{
	if (g_CVImageTest.cols > 0)
	{
		m_iDisplayTestImageType = 3;
		cv::Mat inGray;
		cv::cvtColor(g_CVImageTest, inGray, CV_RGB2GRAY);

		pixkit::enhancement_local::CLAHEnon1987(inGray, g_CVImageTestFilter3, cv::Size(8, 8));
		m_MatToGDITest->DrawImg(g_CVImageTestFilter3);
	}
	SetEvent(g_ProcessFilter3Event);
}


void CARALGISView::OnLbnSelchangeList2()
{
	// TODO: Add your control notification handler code here
	CString strLP;
	int index = m_FormListBox.GetCurSel();
	m_FormListBox.GetText(index, strLP);


	//MessageBox(strLP);
}


void CARALGISView::OnAdd()
{
	// TODO: Add your control notification handler code here
	CString strLP;

	UpdateData();

	strLP = m_FormLPEntry;
	m_FormListBox.ResetContent();

	// find all visits of the given LP
	CVehicleSet vSet;
	CString filter = CString{ "LicensePlate = '" } +strLP;
	filter += CString{ "'" };
	vSet.m_strFilter = filter;
	vSet.m_strSort = _T("MostRecentVisitDate DESC");

	vSet.Open(CRecordset::dynamic, nullptr, CRecordset::readOnly);
	if (vSet.IsBOF()) {
		MessageBox(CString{ "Kayit bulunamadi; plaka no: " } +strLP);
		m_FormListBox.AddString(LPCTSTR{ CString{ "Kayit yok" } });
		UpdateData(FALSE);
		return;
	}

	// these should belong to the most recent visit
	m_FormELPI = vSet.m_LicensePlateImage;
	m_FormEFVI = vSet.m_FrontViewImage;
	m_FormECBCI = vSet.m_ChassisBottomImageCurrent;
	m_FormECBRI = vSet.m_ChassisBottomImageRef;
	m_FormDN = vSet.m_DriverName;

	while (!vSet.IsEOF()) {
		CTime visitDate = vSet.m_MostRecentVisitDate;
		MessageBox(visitDate.Format("%d/%m/%Y %X"));
		m_FormListBox.AddString(visitDate.Format("%d/%m/%Y %X"));
		vSet.MoveNext();
	}

	UpdateData(FALSE);
	// m_FormListBox.AddString(strLP);
}


void CARALGISView::OnGuncelle()
{
	// TODO: Add your control notification handler code here
	// it should open a dialog for row entry into the Vehicle database
}
