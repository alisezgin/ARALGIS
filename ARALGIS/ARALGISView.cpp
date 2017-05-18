
// ARALGISView.cpp : implementation of the CARALGISView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.

#include "afxbutton.h"

#ifndef SHARED_HANDLERS
#include "ARALGIS.h"
#endif

#include "MainFrm.h"

#include "ARALGISDoc.h"
#include "ARALGISView.h"
#include ".\\ChangeDetection\\HeaderFiles\\ChangeDetectController.h"
#include ".\ImageFiltering\HeaderFiles\pixkit-image.hpp"
#include ".\ImageFiltering\HeaderFiles\cvt.hpp"

#include "VehicleSet.h"
#include "GateSet.h"
#include "UserInfoSet.h"
//#include "VehicleView.h"

#include "VehiclePassageSet.h"
#include "VehicleDlg.h"

#include "DriverInfoSet.h"
#include "DriverSet.h"

#include "DBUpdateAckDlg.h"
#include "DriverAckDlg.h"

#include <sstream>


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
	//ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_FILTER1, &CARALGISView::OnBnClickedButtonFilter1)
	ON_BN_CLICKED(IDC_BUTTON_FILTER2, &CARALGISView::OnBnClickedButtonFilter2)
	ON_BN_CLICKED(IDC_BUTTON_FILTER3, &CARALGISView::OnBnClickedButtonFilter3)
	ON_LBN_SELCHANGE(IDC_FORM_LMRV, &CARALGISView::OnLbnSelchangeList2)
	ON_BN_CLICKED(IDC_FORM_BMODIFY, &CARALGISView::OnGuncelle)
	ON_BN_CLICKED(IDC_BUTTON_ORGINAL, &CARALGISView::OnBnClickedButtonOrginal)
	ON_BN_CLICKED(IDC_FORM_BDN_QUERY, &CARALGISView::OnBnClickedFormBdnQuery)
	ON_BN_CLICKED(IDC_FORM_BUPDATEDB, &CARALGISView::OnBnClickedFormBupdatedb)
	ON_EN_KILLFOCUS(IDC_FORM_EDID, &CARALGISView::OnEnKillfocusFormEdid)
	ON_BN_CLICKED(IDC_FORM_BARA, &CARALGISView::OnAdd)
END_MESSAGE_MAP()

// CARALGISView construction/destruction

CARALGISView::CARALGISView() : CColorFormView(CARALGISView::IDD)
, m_PlakaStr(_T(""))

, m_FormLPEntry(_T(""))
, m_FormELPI(_T(""))
, m_FormEFVI(_T(""))
//, m_FormECBCI(_T(""))
//, m_FormECBRI(_T(""))
, m_FormEDID(_T(""))
, m_FormEDT(0)
, m_FormEBL{}
, m_FormEGID(_T(""))
, m_FormEUID(_T(""))
{
	// TODO: add construction code here

	m_RefImgBMP = new CStatic;
	m_TestImgBMP = new CStatic;

	g_ByteImageTest = NULL;

	m_MatToGDITest = NULL;
	m_MatToGDIRef = NULL;

	m_iDisplayTestImageType = 0;


	strcpy_s(m_FilenameRef, "C:/Users/bora/Desktop/FUZYON-SW-Dev/SW-Projects/uvss-images/new/1600/car-1-handCropped.bmp\0");
	strcpy_s(m_FilenameTest, "C:/Users/bora/Desktop/FUZYON-SW-Dev/SW-Projects/uvss-images/new/1600/car-1Diff4-handCropped.bmp\0");
}

CARALGISView::~CARALGISView()
{
	if (m_RefImgBMP)
	{
		delete m_RefImgBMP;
	}

	if (m_TestImgBMP)
	{
		delete m_TestImgBMP;
	}

	if (m_MatToGDITest)
	{
		delete m_MatToGDITest;
	}

	if (m_MatToGDIRef)
	{
		delete m_MatToGDIRef;
	}

	if (g_CarPlakaImage)
	{
		delete[] g_CarPlakaImage;
	}

	if (g_ByteImageTest)
	{
		delete[] g_ByteImageTest;
	}
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
	DDX_Control(pDX, IDC_FORM_LMRV, m_FormListBox);
	DDX_Text(pDX, IDC_FORM_LPENTRY, m_FormLPEntry);
	DDX_Text(pDX, IDC_FORM_ELP, m_FormELP);
	DDX_Text(pDX, IDC_FORM_EDID, m_FormEDID);
	DDX_Text(pDX, IDC_FORM_EUID, m_FormEUID);
	DDX_Text(pDX, IDC_FORM_EGID, m_FormEGID);
	DDX_Control(pDX, IDC_FORM_EBL, m_FormEBL);
	DDX_Control(pDX, IDC_BUTTON_FILTER1, m_ButtonFilter1);
	DDX_Control(pDX, IDC_BUTTON_FILTER2, m_ButtonFilter2);
	DDX_Control(pDX, IDC_BUTTON_FILTER3, m_ButtonFilter3);
	DDX_Control(pDX, IDC_BUTTON_ORGINAL, m_ButtonOrgImage);
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

	m_ButtonOrgImage.EnableWindow(FALSE);
	m_ButtonFilter1.EnableWindow(FALSE);
	m_ButtonFilter2.EnableWindow(FALSE);
	m_ButtonFilter3.EnableWindow(FALSE); 

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

	bOk = m_resizer.CreateSplitContainer(_T("Splitter_H"), _T("Test_Panel"), _T("Ref_Panel"));
	ASSERT(bOk);

	bOk = m_resizer.SetShowSplitterGrip(_T("Splitter_H"), TRUE);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(_T("Splitter_H"), ANCHOR_ALL);
	ASSERT(bOk);

	CSize size(0, 200); // cy member will be ignored
	bOk = m_resizer.SetMinimumSize(_T("Test_Panel"), size);
	ASSERT(bOk);

	CSize size2(0, 200); // cy member will be ignored
	bOk = m_resizer.SetMinimumSize(_T("Ref_Panel"), size2);
	ASSERT(bOk);

	
	// ali: anchoring stuff

	arrID.RemoveAll();
	arrID.Add(IDC_FORM_GRPVEHICLE);
	/*arrID.Add(IDC_FORM_SLPI);
	arrID.Add(IDC_FORM_SFVI);*/
	arrID.Add(IDC_FORM_SMRV);
	arrID.Add(IDC_FORM_SLP);
	arrID.Add(IDC_FORM_SUID);
	arrID.Add(IDC_FORM_SGID);
	arrID.Add(IDC_FORM_SDID);
	/*arrID.Add(IDC_FORM_ELPI);
	arrID.Add(IDC_FORM_EFVI);*/
	arrID.Add(IDC_FORM_ELP);
	arrID.Add(IDC_FORM_EUID);
	arrID.Add(IDC_FORM_EGID);
	arrID.Add(IDC_FORM_EDID);
	arrID.Add(IDC_FORM_EBL);
	arrID.Add(IDC_FORM_LMRV);
	arrID.Add(IDC_FORM_BMODIFY);
	arrID.Add(IDC_FORM_BUPDATEDB);
	arrID.Add(IDC_FORM_BLP_QUERY);
	arrID.Add(IDC_FORM_BDN_QUERY);
	arrID.Add(IDC_FORM_BRN_QUERY);
	arrID.Add(IDC_FORM_BGN_QUERY);


	bOk = m_resizer.CreatePanel(_T("Info_Panel"), &arrID, TRUE);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(_T("Info_Panel"), ANCHOR_HORIZONTALLY_CENTERED);
	ASSERT(bOk);
	
	arrID.RemoveAll();

	arrID.Add(IDC_STATIC_FILTERS);
	arrID.Add(IDC_STATIC_PTS);
	arrID.Add(IDC_BUTTON_ORGINAL);
	arrID.Add(IDC_BUTTON_FILTER1);
	arrID.Add(IDC_BUTTON_FILTER2);
	arrID.Add(IDC_BUTTON_FILTER3);
	arrID.Add(IDC_BUTTON_PTS_STATUS);
	arrID.Add(IDC_BUTTON_PERIPHERAL_STATUS);
	arrID.Add(IDC_STATIC_BARRIER);
	arrID.Add(IDC_BUTTON_BARRIER_OPEN);
	arrID.Add(IDC_BUTTON_BARRIER_CLOSE);
	arrID.Add(IDC_BUTTON_HEATER_ON);
	arrID.Add(IDC_BUTTON_HEATER_OFF);
	arrID.Add(IDC_BUTTON_ALARM_ON);
	arrID.Add(IDC_BUTTON_ALARM_OFF);

	bOk = m_resizer.CreatePanel(_T("Camera_Panel"), &arrID, TRUE);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(_T("Camera_Panel"), ANCHOR_RIGHT);
	ASSERT(bOk);

	arrID.RemoveAll();

	arrID.RemoveAll();
	arrID.Add(IDC_STATIC_PLAKA);
	arrID.Add(IDC_FORM_LPENTRY); 
	arrID.Add(IDC_FORM_BARA); 

	bOk = m_resizer.CreatePanel(_T("View_Panel"), &arrID, TRUE);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(_T("View_Panel"), ANCHOR_LEFT);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(IDC_STATIC_PLAKA, ANCHOR_LEFT | ANCHOR_TOP);
	ASSERT(bOk);


	CRect workArea;
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
	// boraN code analysis
	// If the above function fails, check what happens to workArea
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


	cv::Mat dMat3;


	if (g_dEndIndex - g_dBeginIndex > 0)
	{
		cv::Rect region_of_interest = cv::Rect(0, g_dBeginIndex, g_CVImageTest.cols, g_dEndIndex - g_dBeginIndex);

		dMat3 = g_CVImageTest(region_of_interest);

		//cv::namedWindow("CAR", cv::WINDOW_NORMAL);
		//cv::imshow("CAR", dMat3);
		//cv::waitKey(1500);
	}

	dMat3.copyTo(g_CVImageTest);

	//GetTestImageAsByte();


	transpose(g_CVImageTest, dMat1);
	flip(dMat1, dMat2, 1); //transpose+flip(1)=CW
	dMat2.copyTo(g_CVImageTest);

	GetTestImageAsByte();



	if (m_MatToGDITest != NULL)
	{
		delete m_MatToGDITest;
		m_MatToGDITest = NULL;
	}

	m_MatToGDITest = new PkMatToGDI(m_TestImgBMP, false);
	m_MatToGDITest->DrawImg(g_CVImageTest);

	// ali: update the database with the current image
	// ali: this has to go to the VehiclePassage table

	// code to be deleted when dbase handler
	// thread is added
	// code deletion starts here
	LPARAM pLparam;
	pLparam = reinterpret_cast<LPARAM>("ARALGIS");
	SendMessage(WM_DBASE_CAR_INFO_READY, 0, pLparam);
	// code deletion ends here

	m_ButtonOrgImage.EnableWindow(TRUE);

	if (g_CVImageTest.cols > 0)
	{
		SetEvent(g_ProcessFilter1Event);
		SetEvent(g_ProcessFilter2Event);
		SetEvent(g_ProcessFilter3Event);

		m_ButtonFilter1.EnableWindow(FALSE);
		m_ButtonFilter2.EnableWindow(FALSE);
		m_ButtonFilter3.EnableWindow(FALSE);
	}



	// get Singleton ChangeDetectionController and start the change detection process
	//CChangeDetectController::getInstance()->process(m_FilenameRef, m_FilenameTest);

	return 0;
}


afx_msg LRESULT CARALGISView::OnDBaseCarInfoReady(WPARAM wParam, LPARAM lParam)
{
	// temporary modification to the g_ReferenceFilePath
	// IMPORTANT !!! 
	char strRefFilePathTemp[] = "C:\\ali\\github-home\\ARALGIS\\Cars\\ChassisBottom\\";
	strncpy_s(g_ReferenceFilePath, (size_t)(MAX_FILENAME_LENGTH + 1), strRefFilePathTemp, (size_t)(MAX_FILENAME_LENGTH));
	// IMPORTANT !!!

	strncpy_s(g_RefImageFileName, (size_t)(MAX_FILENAME_LENGTH + 1), g_ReferenceFilePath, (size_t)(MAX_FILENAME_LENGTH));
	strncat_s(g_RefImageFileName, (size_t)(MAX_FILENAME_LENGTH + 1), "arac1-colour.bmp", (size_t)(MAX_FILENAME_LENGTH));


	g_CVImageRef = cv::imread(g_RefImageFileName, cv::IMREAD_COLOR);

	//g_CVImageRef = cv::imread("C:/Users/bora/Desktop/FUZYON-SW-Dev/SW-Projects/uvss-images/new/1600/car-1-handCropped.bmp", cv::IMREAD_COLOR);

	// no need to transpose+flip
	// since image files are written landscape!!!
	// for the time being for testing purposes
	// these operations are needed.
	// delete below code for real system
	// code deletion starts here
	//cv::Mat dMat1, dMat2;
	//transpose(g_CVImageRef, dMat1);
	//flip(dMat1, dMat2, 1); //transpose+flip(1)=CW
	//dMat2.copyTo(g_CVImageRef);
	// code deletion ends here


	if (m_MatToGDIRef != NULL)
	{
		delete m_MatToGDIRef;
		m_MatToGDIRef = NULL;
	}

	m_MatToGDIRef = new PkMatToGDI(m_RefImgBMP, false);
	m_MatToGDIRef->DrawImg(g_CVImageRef);


	// TODO: Add your command handler code here
	CString aCString;
	CString bCString = CString(_T("         "));


	char AAA[9];
	for (int i = 0; i < 8; i++)
	{
		AAA[i] = g_PlakaNoChars[i];
	}
	AAA[8] = '\0';

	aCString = AAA;

	OnLPUpdateInfo(aCString);

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

void CARALGISView::DisplayPlakaNo()
{

	// TODO: Add your command handler code here
	CString aCString;
	CString bCString = CString(_T("         "));


	char AAA[9];
	for (int i = 0; i < 8; i++)
	{
		AAA[i] = g_PlakaNoChars[i];
	}
	AAA[8] = '\0';

	aCString = AAA;

	//OnLPUpdateInfo(aCString);





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
}


void CARALGISView::DisplayPTSImage()
{
	// TODO: Add your command handler code here
	m_CarPlakaImageStatic.Load(g_CarPlakaImage, (size_t)g_CarPlakaImageLenght);

	SetTimerDisplay();
}

void CARALGISView::DeletePTSImage()
{
	// TODO: Add your command handler code here
	//m_CarPlakaImageStatic.FreeData();
}

void CARALGISView::UpdatePTSStatus(int aStatus)
{
	if (aStatus == 2)
	{
		m_PTS_Status.SetColour(WHITE, GREEN);
	}
	else if (aStatus == 1)
	{
		m_PTS_Status.SetColour(WHITE, MAGENTA);
	}
	else if (aStatus == 0)
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

//void CARALGISView::OnTimer(UINT_PTR nIDEvent)
//{
//	if (nIDEvent == DISPLAY_TIMER_ID)
//	{
//		m_TimerSecondCounter++;
//
//		if (m_TimerSecondCounter == PLAKA_TIME_ON_SCREEN)
//		{
//			// do something
//			DeletePTSImage();
//			KillTimer(DISPLAY_TIMER_ID);
//		}
//	} // End if.
//	CColorFormView::OnTimer(nIDEvent);
//}



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

void CARALGISView::OnBnClickedButtonOrginal()
{
	m_iDisplayTestImageType = 0;
	m_MatToGDITest->DrawImg(g_CVImageTest);
}

void CARALGISView::OnBnClickedButtonFilter1()
{
	m_iDisplayTestImageType = 1;
	m_MatToGDITest->DrawImg(g_CVImageTestFilter1);
}


void CARALGISView::OnBnClickedButtonFilter2()
{
	m_iDisplayTestImageType = 2;
	m_MatToGDITest->DrawImg(g_CVImageTestFilter2);
}


void CARALGISView::OnBnClickedButtonFilter3()
{
	m_iDisplayTestImageType = 3;
	m_MatToGDITest->DrawImg(g_CVImageTestFilter3);	
}

void CARALGISView::FilterAvailable(int aNumFilter)
{
	if (aNumFilter == 1)
	{
		m_ButtonFilter1.EnableWindow(TRUE);
	}
	else if (aNumFilter == 2)
	{
		m_ButtonFilter2.EnableWindow(TRUE);
	}
	else if (aNumFilter == 3)
	{
		m_ButtonFilter3.EnableWindow(TRUE);
	}
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

	OnLPUpdateInfo(strLP);
}


void CARALGISView::OnGuncelle()
{
	// TODO: Add your control notification handler code here
	// it should open a dialog for row entry into the Vehicle database
//	CVehicleDlg vDlg{ m_FormEDT, m_FormEDN, m_FormELP, 
//		m_FormEBL.GetCheck(), m_FormERN };
//	if (vDlg.DoModal() == IDOK) {
//		m_FormEBL.SetCheck(vDlg.getBL());
//		m_FormEDN = vDlg.getDN();
//		//m_FormEDT = vDlg.getDT();
//		m_FormELP = vDlg.getLP();
//		m_FormERN = vDlg.getRN();
//
////		MessageBox(_T("You pressed OK; the database will be updated"));
//	}
//	else {
////		MessageBox(_T("You pressed CANCEL; the database will not be updated"));
//	}

	UpdateData(FALSE);
	//CVehicleSet vSet;
	//if (!vSet.Open()) {
	//	MessageBox(_T("Recordset is not open!"));
	//	return;
	//}

	//vSet.AddNew();

	/*vSet.m_VehicleID = 0;
	vSet.m_LicensePlate = m_FormELP;
	vSet.m_LicensePlateImage = m_FormELPI; // _T("some file");
	vSet.m_DriverName = m_FormEDN; // _T("Abuzer Negezer");
	vSet.m_ChassisBottomImageCurrent = m_FormECBCI; // _T("UpSkirtNow.jpg");
	vSet.m_ChassisBottomImageRef = m_FormECBRI; // _T("UpSkirtVintage.jpg");
	vSet.m_FrontViewImage = m_FormEFVI; // _T("Cleavage.jpg");
	vSet.m_MostRecentVisitDate = CTime::GetCurrentTime(); // CTime{ 2016, 12, 30, 22, 58, 0 };
	if (!vSet.Update()) {
		MessageBox(_T("Record not added"));
		return;
	}*/
}


// called to update the vehicle info boxes upon the retrieval of a new license plate number
void CARALGISView::OnLPUpdateInfo(CString strLP)
{
	m_FormListBox.ResetContent();

	CVehiclePassageSet vPassageSet;
	// find all visits of the given LP
	CString filter = CString{ _T("LicensePlate = '") } +strLP;
	filter += CString{ _T("'") };
	vPassageSet.m_strFilter = filter;
	vPassageSet.m_strSort = _T("EntryDateTime DESC");

	m_FormEDT = CTime::GetCurrentTime();

	// before executing the query;
	// it may be beneficial to bound the resulting record set
	// vPassageSet.SetRowsetSize() = RECENT_VISIT_COUNT;
	vPassageSet.Open(CRecordset::dynamic, nullptr, CRecordset::readOnly);
	if (vPassageSet.IsBOF()) {
		MessageBox(CString{ _T("Kayit bulunamadi; plaka no: ") } +strLP);
		m_FormListBox.AddString(LPCTSTR{ CString{ _T("Kayit yok") } });
		m_FormELP = _T("Kayit yok");
		m_FormEUID = _T("Kayit yok");
		m_FormEGID = _T("Kayit yok");
		m_FormEDID = _T("Kayit yok");
		m_FormEBL.SetCheck(BST_INDETERMINATE);
		m_VID = -1;
		m_DID = -1; // _T("Kayit yok");
		m_GID = -1; // _T("Kayit yok");
		m_UID = -1; // _T("Kayit yok");
		UpdateData(FALSE);
		return;
	}

	// these should belong to the most recent visit
	//	strLP.Remove(' ');
	strLP.MakeUpper();
	m_FormELP = strLP;
	m_FormEBL.SetCheck(!vPassageSet.m_VehiclePassagePermissionGranted);

	// since there is an existing record; m_VID >= 0
	m_VID = vPassageSet.m_VehiclePassageVehicleID;
	ASSERT(m_VID >= 0);

	// instead of the cryptic ID's, use the associated names
	// for each ID to name mapping, the corresponding Recordset has to be formed.
	// this is rather expensive operation.
	// for the final release version, modify the design of the database tables.
	//m_FormEDID = vPassageSet.m_VehiclePassageDriverID;
	CString tempStrofInt;
	CDriverInfoSet dInfoSet;
	CString driverFilter = CString{ _T("[dbo].[Driver].[Type] = [dbo].[DriverType].[DTID] AND [dbo].[Driver].[ID] = '") };
	tempStrofInt.Format(_T("%ld"), vPassageSet.m_VehiclePassageDriverID);
	driverFilter += tempStrofInt;
	driverFilter += CString{ _T("'") };
	dInfoSet.m_strFilter = driverFilter;
	dInfoSet.Open(CRecordset::dynamic, nullptr, CRecordset::readOnly);
	//ASSERT(dInfoSet.GetRecordCount() == 1);
	m_FormEDID = dInfoSet.m_dboDriverLastName;
	m_FormEDID += _T(", ");
	m_FormEDID += dInfoSet.m_dboDriverName;
	m_DID = dInfoSet.m_dboDriverID;


	//m_FormEGID = vPassageSet.m_VehiclePassageGateID;
	CGateSet gSet;
	CString gateFilter = CString{ _T("ID = '") };
	tempStrofInt.Format(_T("%ld"), vPassageSet.m_VehiclePassageGateID);
	gateFilter += tempStrofInt;
	gateFilter += CString{ _T("'") };
	gSet.m_strFilter = gateFilter;
	gSet.Open(CRecordset::dynamic, nullptr, CRecordset::readOnly);
	//ASSERT(gSet.GetRecordCount() == 1);
	m_FormEGID = gSet.m_GateType;
	m_GID = gSet.m_GateID;

	//m_FormEUID = _T("Veli, Ali");
	//m_FormEUID = vPassageSet.m_VehiclePassageUserID;
	CUserInfoSet uInfoSet;
	CString userFilter = CString{ _T("[User].[ID] = [UserLog].[UserID] AND [User].[Type] = [UserType].[UTID] AND [User].[ID] = '") };
	tempStrofInt.Format(_T("%ld"), vPassageSet.m_VehiclePassageUserID);
	userFilter += tempStrofInt;
	userFilter += CString{ _T("'") };
	uInfoSet.m_strFilter = userFilter;
	uInfoSet.Open(CRecordset::dynamic, nullptr, CRecordset::readOnly);
	//ASSERT(uInfoSet.GetRecordCount() == 1);
	m_FormEUID = uInfoSet.m_dboUserLastName;
	m_FormEUID += _T(",");
	m_FormEUID += uInfoSet.m_dboUserName;
	m_UID = uInfoSet.m_dboUserID;

	while (!vPassageSet.IsEOF()) {
		CTime visitDate = vPassageSet.m_VehiclePassageEntryDateTime;  // vSet.m_MostRecentVisitDate;
//		MessageBox(visitDate.Format("%d/%m/%Y %X"));
		m_FormListBox.AddString(visitDate.Format("%d/%m/%Y %X"));
		vPassageSet.MoveNext();
	}

	// textual information done.
	// now, update the relevant images: FrontalView, ChassisBottomRef, ChassisBottomCur
	CVehicleSet vSet;
	vSet.m_strFilter = filter;
	vSet.Open(CRecordset::dynamic, nullptr, CRecordset::readOnly);
	if (vSet.IsBOF()) {
		// this should never happen.
		// if a record for a license plate exists in the VehiclePassage table, 
		// then there must be (exactly) one entry in the Vehicle table.
		MessageBox(CString{ _T("Kayitli arac bulunamadi. Plaka: ") }+strLP);
	}
	// checking the assertion above: exactly one record must match the license plate
	ASSERT(vSet.GetRecordCount() == 1);

	// retrieve the reference filename to display the reference image
	// the other image files, frontal and test, are to be formed during the database update.
	CString strRefFilename = vSet.m_VehicleChassisBottomReferenceImageFile;
	
	// TODO: display the contents of strRefFilename
	// MessageBox(CString{ _T("The filename for Reference image: ") }+strRefFilename);


	UpdateData(FALSE);
	// m_FormListBox.AddString(strLP);

}


void CARALGISView::OnBnClickedFormBdnQuery()
{
	// TODO: Add your control notification handler code here
	CDriverInfoSet dInfoSet;
	/*CString strID; 
	strID.Format(_T("%ld"), m_FormEDID);
	CString filter = CString{ _T("[Driver].[ID] = '") } +strID;
	filter += CString{ _T("'") };

	dInfoSet.m_strFilter = filter;*/

	dInfoSet.Open(CRecordset::snapshot, nullptr, CRecordset::readOnly);
	CString message;
	if (dInfoSet.IsBOF()) {
		message.Format(_T("Kayit bulunamadi; Surucu: %s"), m_FormEDID);
		MessageBox(message);
		return;
	}

	message.Format(_T("Surucu adi: %s %s"), dInfoSet.m_dboDriverName, dInfoSet.m_dboDriverLastName);
	return;
}

void CARALGISView::GetTestImageAsByte()
{
	if (g_ByteImageTest)
	{
		delete[] g_ByteImageTest;
	}

	g_ByteImageSize = (unsigned int)((int)g_CVImageTest.step[0] * (int)g_CVImageTest.rows);

	g_ByteImageTest = new BYTE[g_ByteImageSize];

	memcpy(g_ByteImageTest, g_CVImageTest.data, g_ByteImageSize);

	SetEvent(SendUVSSImageEvent);

}


void CARALGISView::OnBnClickedFormBupdatedb()
{
	// TODO: Add your control notification handler code here
	// Things to do here:
	// 1. Copy the contents of the test image as a file into the relevant directory.
	// 2. Copy the contents of the frontal image as a file into the relevant directory.
	// 3. Read the contents of the value boxes and
	//  i) if a record with the given license plate does not exist, create a new record or,
	//  ii) if a record with the the given license plate does exist, update the existing record.
	// [Optional] Ask for a second time whether the update is to be made (an additional dialog box)
	// before starting all these modifications.
	
	CDBUpdateAckDlg dUpAck;

	if (dUpAck.DoModal() == IDOK) { // Update operations
		CVehiclePassageSet vPassageSet;
		vPassageSet.Open(CRecordset::dynamic, nullptr, CRecordset::none);

		// no modification of existing rows in the VehiclePassage table;
		// always add a new row.
		vPassageSet.AddNew();

		vPassageSet.m_VehiclePassageLicensePlate = m_FormELP;
		vPassageSet.m_VehiclePassageEntryDateTime = m_FormEDT;
		vPassageSet.m_VehiclePassageUserID = m_UID;
		vPassageSet.m_VehiclePassageGateID = m_GID;
		vPassageSet.m_VehiclePassageDriverID = m_DID;
		vPassageSet.m_VehiclePassagePermissionGranted = !m_FormEBL.GetCheck();

		CString msg = _T("These will be used by AddNew():");
		CString paramStr;
		paramStr.Format(_T("\nLicense Plate= %s,\nDriver ID= %ld,\nGate ID= %ld,\nGate Keeper ID= %ld,\nApproved= %d.\n"), m_FormELP, m_DID, m_GID, m_UID, !m_FormEBL.GetCheck());

		// MessageBox(msg + paramStr);

		vPassageSet.Update();
	}
	else {
		// nothing to do (yet)
		return;
	}
}



void CARALGISView::OnEnKillfocusFormEdid()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_FormEDID.Remove(' ');
	auto commaPos = m_FormEDID.Find(_T(","));
	CString strLastName = m_FormEDID.Left(commaPos);
	CString strFirstName = m_FormEDID.Right(m_FormEDID.GetLength() - commaPos - 1);

	if (strFirstName == _T("") && strLastName == _T("")) {
		return;
	} else if (strFirstName == _T("") || strLastName == _T("")) {
		MessageBox(_T("Isim Hatasi."));
		return;
	}

	//CDriverInfoSet dInfoSet;
	//CString driverFilter = CString{ _T("[dbo].[Driver].[Type] = [dbo].[DriverType].[DTID] AND [dbo].[Driver].[Name] = '") };
	//driverFilter += strFirstName;
	//driverFilter += CString{ _T("' AND [dbo].[Driver].[LastName] ='") };
	//driverFilter += strLastName;
	//driverFilter += CString{ _T("'") };
	//dInfoSet.m_strFilter = driverFilter;
	//dInfoSet.Open(CRecordset::dynamic, nullptr, CRecordset::readOnly);
	//ASSERT(dInfoSet.GetRecordCount() == 1);

	CDriverSet dSet;

	CString filter{ _T("[Name] = '") };
	filter += strFirstName;
	filter += _T("' AND [LastName] = '");
	filter += strLastName;
	filter += _T("'");

	dSet.m_strFilter = filter;

	dSet.Open(CRecordset::dynamic, nullptr, CRecordset::none);

	if (dSet.GetRecordCount() == 0) {
		MessageBox(_T("Bu sofor sistemde kayitli degil"));
	} 
	else // clean up spaces, reset driver ID
	{
		m_FormEDID = dSet.m_LastName;
		m_FormEDID += _T(", ");
		m_FormEDID += dSet.m_Name;
		m_DID = dSet.m_ID;
		return;
	}


	// new driver; for now, just insert name and last name
	// first make sure that this is the intention of the user
	CDriverAckDlg dAckDlg;
	if (dAckDlg.DoModal() == IDCANCEL)
		return;

	// ack'ed. now can modify the Driver table
	dSet.AddNew();

	dSet.m_Name = strFirstName;
	dSet.m_LastName = strLastName;
	dSet.m_Type = 1;

	dSet.Update();

	dSet.m_strFilter = filter;
	dSet.Requery();

	m_DID = dSet.m_ID;
}

