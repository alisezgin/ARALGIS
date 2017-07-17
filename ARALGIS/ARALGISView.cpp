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
#include "UserSet.h"
//#include "VehicleView.h"

#include "VehiclePassageSet.h"
#include "VehicleDlg.h"

#include "DriverInfoSet.h"
#include "DriverSet.h"

#include "DBUpdateAckDlg.h"
#include "DriverAckDlg.h"

#include "BitmapDlg.h"
#include "ImageToolDialog.h"

#include "VisitInfo.h"

#include ".\\ErrorDisplay\\ThreadSafeQueue\\HeaderFiles\\ThreadSafeQueue.h"

#include "resource.h"


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
	ON_MESSAGE(WM_VEHICLEDETECT_FINISHED, &CARALGISView::GetTestImage)
	ON_MESSAGE(WM_DBASE_FILE_WRITTEN, &CARALGISView::OnDBaseFileWritten)
	ON_MESSAGE(WM_FILTER1_READY, &CARALGISView::Filter1Available)
	ON_MESSAGE(WM_FILTER2_READY, &CARALGISView::Filter2Available)
	ON_MESSAGE(WM_FILTER3_READY, &CARALGISView::Filter3Available)
	ON_MESSAGE(WM_FILTER4_READY, &CARALGISView::Filter4Available)
	ON_MESSAGE(WM_FILTER5_READY, &CARALGISView::Filter5Available)
	ON_MESSAGE(WM_FILTER6_READY, &CARALGISView::Filter6Available)
	ON_MESSAGE(WM_CHANGEDETECT_FINISHED, &CARALGISView::ChangeDetectionFinished)
	ON_MESSAGE(PTS_DISPLAY_IMAGE, &CARALGISView::DisplayPTSImage)
	ON_MESSAGE(PTS_DISPLAY_PLAKA, &CARALGISView::DisplayPlakaNo)
	ON_MESSAGE(PTS_CONNECTION_LOST, &CARALGISView::UpdatePTSStatusLost)
	ON_MESSAGE(PTS_CONNECTION_NOK, &CARALGISView::UpdatePTSStatusNOK)
	ON_MESSAGE(PTS_CONNECTION_OK, &CARALGISView::UpdatePTSStatusOK)
	ON_MESSAGE(ODROID_CONNECTION_OK, &CARALGISView::UpdatePeripheralStatusOK)
	ON_MESSAGE(ODROID_CONNECTION_LOST, &CARALGISView::UpdatePeripheralStatusNOK)
	ON_MESSAGE(WM_LMOUSE_DOWN, &CARALGISView::OnLButtonDownImage)
	ON_MESSAGE(WM_LMOUSE_UP, &CARALGISView::OnLButtonUpImage)
	ON_MESSAGE(WM_LMOUSE_MOVE, &CARALGISView::OnMouseMoveImage)
	ON_MESSAGE(WM_MOUSE_WHEEL, &CARALGISView::OnMouseWheelImage)
	//ON_MESSAGE(WM_LMOUSE_DBLCLK, &CARALGISView::OnLButtonDblClkImage)
	ON_BN_CLICKED(IDC_BUTTON_BARRIER_OPEN, &CARALGISView::OnBnClickedButtonBarrierOpen)
	ON_BN_CLICKED(IDC_BUTTON_BARRIER_CLOSE, &CARALGISView::OnBnClickedButtonBarrierClose)
	ON_BN_CLICKED(IDC_BUTTON_HEATER_ON, &CARALGISView::OnBnClickedButtonHeaterOn)
	ON_BN_CLICKED(IDC_BUTTON_HEATER_OFF, &CARALGISView::OnBnClickedButtonHeaterOff)
	ON_BN_CLICKED(IDC_BUTTON_ALARM_ON, &CARALGISView::OnBnClickedButtonAlarmOn)
	ON_BN_CLICKED(IDC_BUTTON_ALARM_OFF, &CARALGISView::OnBnClickedButtonAlarmOff)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_LBN_SELCHANGE(IDC_FORM_LMRV, &CARALGISView::OnLbnSelchangeList2)
	ON_BN_CLICKED(IDC_FORM_BMODIFY, &CARALGISView::OnGuncelle)
	ON_BN_CLICKED(IDC_FORM_BDN_QUERY, &CARALGISView::OnBnClickedFormBdnQuery)
	ON_BN_CLICKED(IDC_FORM_BUPDATEDB, &CARALGISView::OnBnClickedFormBupdatedb)
	ON_EN_KILLFOCUS(IDC_FORM_EDID, &CARALGISView::OnEnKillfocusFormEdid)
	ON_BN_CLICKED(IDC_FORM_BARA, &CARALGISView::OnAdd)
	ON_BN_CLICKED(IDC_RADIO_COLOUR1, &CARALGISView::OnBnClickedRadioColour1)
	ON_BN_CLICKED(IDC_RADIO_COLOUR2, &CARALGISView::OnBnClickedRadioColour2)
	ON_BN_CLICKED(IDC_RADIO_COLOUR3, &CARALGISView::OnBnClickedRadioColour3)
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_RADIO1_ORIGFILTER, &CARALGISView::OnBnClickedRadio1Origfilter)
	ON_BN_CLICKED(IDC_RADIO_FILTER1, &CARALGISView::OnBnClickedRadioFilter1)
	ON_BN_CLICKED(IDC_RADIO_FILTER2, &CARALGISView::OnBnClickedRadioFilter2)
	ON_BN_CLICKED(IDC_RADIO_FILTER3, &CARALGISView::OnBnClickedRadioFilter3)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_DETECT, &CARALGISView::OnBnClickedButtonChangeDetect)
	ON_CBN_SELCHANGE(IDC_FORM_CBOX_VISITLIST, &CARALGISView::OnCbnSelchangeFormCboxVisitlist)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CARALGISView::OnBnClickedButtonExit)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_CHECK_CROP, &CARALGISView::OnBnClickedCheckCrop)
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

// CARALGISView construction/destruction

CARALGISView::CARALGISView() : CColorFormView(CARALGISView::IDD)
//, m_PlakaStr(_T(""))

, m_FormLPEntry(_T(""))
, m_FormELP(_T(""))
, m_FormEFVI(_T(""))
//, m_FormECBCI(_T(""))
//, m_FormECBRI(_T(""))
, m_FormEDID(_T(""))
, m_FormEDT(0)
, m_FormEBL{}
, m_FormEGID(_T(""))
, m_FormEUID(_T(""))
, m_FormCBDriverList{}
, m_bVehiclePassageUpdated( TRUE )
{
	// TODO: add construction code here

	m_PrevImgBMP = new CStatic;

	g_ByteImageTest = NULL;

	m_MatToGDIPrev = nullptr;

	m_MatToImageTest.setPanMode(TRUE);
	m_MatToImageTest.setZoomMode(TRUE);
	m_MatToImageTest.setSizeType(1);
	m_MatToImageTest.setMaintainAspectRatio(false);

	m_MatToImageRef.setPanMode(TRUE);
	m_MatToImageRef.setZoomMode(TRUE);
	m_MatToImageRef.setSizeType(1);
	m_MatToImageRef.setMaintainAspectRatio(false);

	m_iDisplayTestImageFilterType = (int)0;
	m_iDisplayRefImageFilterType = (int)0;

	m_iDisplayTestImageColourType = (int)0;
	m_iDisplayRefImageColourType = (int)0;

	m_AutoFit = false;
	g_dCarDetectCount = 0;

	// ali : ARALGISCarsDir is an environment variable which should be set
	// (at the time of the installation of ARALGIS)
	// pointing to the directory containing the Image directories
	// LPWSTR tmpPathToCars = (LPWSTR)malloc(MAX_FILENAME_LENGTH*sizeof(TCHAR));
	LPWSTR tmpPathToCars = m_PathToCars.GetBufferSetLength(MAX_FILENAME_LENGTH);
	GetEnvironmentVariable(_T("ARALGISCarsDir"), tmpPathToCars, MAX_FILENAME_LENGTH);
	// because the contents were modified via the buffer, it must be released
	// before further modification to the contents of the CString variable.
	m_PathToCars.ReleaseBuffer();
	
	/// bora2ali bug-fix begins
	// the environment variable does not contain the trailing directory separator
	//m_PathToCars += _T("\\");
	// currently we have two image subdirectories:
	// FrontalView and ChassisBottom
	// we may very well end up removing the former.
	//m_PathToCars += _T("ChassisBottom\\");
	//strcpy(m_PathToCars, "\\");
	/// bora2ali bug-fix ends

	// Prepare the driver list to be displayed in the Driver List Combo box
	PrepareDriverList();

	// set the background color variable
	m_brush.CreateSolidBrush(RGB(COLOUR_RED, COLOUR_GREEN, COLOUR_BLUE));

	m_IsCropActive = false;
	m_pTempElement = NULL;
	m_CropWhichImage = -1;

	g_IsOdroidStartReceived = FALSE;
	g_IsAnswerReceivedFromPTS = FALSE;
	m_IsActive = true;
}

CARALGISView::~CARALGISView()
{
	m_IsActive = false;

	EnterCriticalSection(&g_ChangeDetectCS);

	// ali: mimic above for Preview
	if (m_PrevImgBMP)
	{
		delete m_PrevImgBMP;
	}

	if (m_MatToGDIPrev != nullptr)
	{
		delete m_MatToGDIPrev;
	}

	if (g_CarPlakaImage)
	{
		delete[] g_CarPlakaImage;
	}

	if (g_ByteImageTest)
	{
		delete[] g_ByteImageTest;
	}

	if (g_CVImageRef.rows != 0 || g_CVImageRef.cols != 0)
	{
		g_CVImageRef.release();
	}

	// ali: for the Preview Image; mimic above
	if (m_CVImagePrev.rows != 0 || m_CVImagePrev.cols != 0)
	{
		m_CVImagePrev.release();
	}

	if (g_CVImageRefFilter1.rows != 0 || g_CVImageRefFilter1.cols != 0)
	{
		g_CVImageRefFilter1.release();
	}

	if (g_CVImageRefFilter2.rows != 0 || g_CVImageRefFilter2.cols != 0)
	{
		g_CVImageRefFilter2.release();
	}

	if (g_CVImageRefFilter3.rows != 0 || g_CVImageRefFilter3.cols != 0)
	{
		g_CVImageRefFilter3.release();
	}

	if (g_CVImageTest.rows != 0 || g_CVImageTest.cols != 0)
	{
		g_CVImageTest.release();
	}

	if (g_CVImageTestFilter1.rows != 0 || g_CVImageTestFilter1.cols != 0)
	{
		g_CVImageTestFilter1.release();
	}

	if (g_CVImageTestFilter2.rows != 0 || g_CVImageTestFilter2.cols != 0)
	{
		g_CVImageTestFilter2.release();
	}

	if (g_CVImageTestFilter3.rows != 0 || g_CVImageTestFilter3.cols != 0)
	{
		g_CVImageTestFilter3.release();
	}

	if (m_TestCVMat.rows != 0 || m_TestCVMat.cols != 0)
	{
		m_TestCVMat.release();
	}

	if (m_RefCVMat.rows != 0 || m_RefCVMat.cols != 0)
	{
		m_RefCVMat.release();
	}
	
	if (g_CVTestChangeDetect.rows != 0 || g_CVTestChangeDetect.cols != 0)
	{
		g_CVTestChangeDetect.release();
	}
	
	if (g_CVTestWrapped.rows != 0 || g_CVTestWrapped.cols != 0)
	{
		g_CVTestWrapped.release();
	}

	
	if (g_CVRefBigWindow.rows != 0 || g_CVRefBigWindow.cols != 0)
	{
		g_CVRefBigWindow.release();
	}

	if (g_CVTestBigWindow.rows != 0 || g_CVTestBigWindow.cols != 0)
	{
		g_CVTestBigWindow.release();
	}

	for (int i= 0; i<=NUM_INTERMEDIATE_TEST_IMAGE; i++)
	{
		if (g_CVImageTestIntermediate[i].rows != 0 || g_CVImageTestIntermediate[i].cols != 0)
		{
			g_CVImageTestIntermediate[i].release();
		}
	}

	m_Font.DeleteObject();

	LeaveCriticalSection(&g_ChangeDetectCS);
}


void CARALGISView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_FORM_PICTCTRL_CBPREVIEW, *m_PrevImgBMP);
	//DDX_Text(pDX, IDC_EDIT_PLAKA, m_PlakaStr);
	//DDX_Control(pDX, IDC_EDIT_PLAKA, m_PlakaCtrl);
	DDX_Control(pDX, IDC_STATIC_PLAKA, m_CarPlakaImageStatic);
	DDX_Control(pDX, IDC_BUTTON_BARRIER_OPEN, m_BarrierOpenBtn);
	DDX_Control(pDX, IDC_BUTTON_BARRIER_CLOSE, m_BarrierCloseBtn);
	DDX_Control(pDX, IDC_BUTTON_HEATER_ON, m_HeaterStartBtn);
	DDX_Control(pDX, IDC_BUTTON_HEATER_OFF, m_HeterStopBtn);
	DDX_Control(pDX, IDC_BUTTON_ALARM_ON, m_AlarmStartBtn);
	DDX_Control(pDX, IDC_BUTTON_ALARM_OFF, m_AlarmStop);
	DDX_Text(pDX, IDC_FORM_LPENTRY, m_FormLPEntry);
	DDX_Text(pDX, IDC_FORM_ELP, m_FormELP);
	DDX_Text(pDX, IDC_FORM_EDID, m_FormEDID);
	DDX_Text(pDX, IDC_FORM_EUID, m_FormEUID);
	DDX_Text(pDX, IDC_FORM_EGID, m_FormEGID);
	DDX_Control(pDX, IDC_FORM_EBL, m_FormEBL);
	DDX_Control(pDX, IDC_STATIC_PLAKA_STR, m_PlakaDisplayControl);
	DDX_Control(pDX, IDC_RADIO_COLOUR1, m_ColourRadioOriginal);
	DDX_Control(pDX, IDC_RADIO1_ORIGFILTER, m_FilterRadioOriginal);
	DDX_Control(pDX, IDC_BUTTON_CHANGE_DETECT, m_ChangeDetectControl);
	DDX_Control(pDX, IDC_FORM_CBOX_VISITLIST, m_formCBoxVisitList);
	DDX_Control(pDX, IDC_FORM_CBOX_DRIVERLIST, m_FormCBDriverList);
	DDX_Control(pDX, IDC_CHECK_CROP, m_CropCheckBox);
	DDX_Control(pDX, IDC_STATIC_TESTBMP, m_MatToImageTest);
	DDX_Control(pDX, IDC_STATIC_REFBMP, m_MatToImageRef);
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

	SetBackgroundColor(RGB(COLOUR_RED, COLOUR_GREEN, COLOUR_BLUE));

	m_FilterRadioOriginal.EnableWindow(FALSE);
	m_FilterRadioOriginal.SetCheck(1);

	CButton* dRadioB;
	dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_FILTER1));
	dRadioB->SetCheck(0);
	dRadioB->EnableWindow(FALSE);

	dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_FILTER2));
	dRadioB->EnableWindow(FALSE);
	dRadioB->SetCheck(0);

	dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_FILTER3));
	dRadioB->EnableWindow(FALSE);
	dRadioB->SetCheck(0);

	m_ColourRadioOriginal.EnableWindow(FALSE);
	m_ColourRadioOriginal.SetCheck(1);

	m_iDisplayTestImageFilterType = (int)0;
	m_iDisplayRefImageFilterType = (int)0;
	m_iDisplayRefImageColourType = (int)0;
	m_iDisplayTestImageColourType = (int)0;

	dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_COLOUR2));
	dRadioB->SetCheck(0);
	dRadioB->EnableWindow(FALSE);

	dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_COLOUR3));
	dRadioB->EnableWindow(FALSE);
	dRadioB->SetCheck(0);

	dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_BUTTON_CHANGE_DETECT));
	dRadioB->EnableWindow(FALSE);

	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	// TODO: Add extra initialization here
	BOOL bOk = FALSE;
	bOk = m_resizer.Hook(this);
	ASSERT(bOk);

	CUIntArray arrID;
	arrID.Add(IDC_GRP_TEST);
	arrID.Add(IDC_STATIC_TESTBMP);

	bOk = m_resizer.CreatePanel(_T("Test_Panel"), &arrID, TRUE);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(IDC_GRP_TEST, ANCHOR_ALL);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(IDC_STATIC_TESTBMP, ANCHOR_ALL);
	ASSERT(bOk);

	arrID.RemoveAll();

	arrID.Add(IDC_GRP_REF);
	arrID.Add(IDC_STATIC_REFBMP);

	bOk = m_resizer.CreatePanel(_T("Ref_Panel"), &arrID, TRUE);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(IDC_GRP_REF, ANCHOR_ALL);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(IDC_STATIC_REFBMP, ANCHOR_ALL);
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
	arrID.Add(IDC_FORM_CBOX_VISITLIST);
	arrID.Add(IDC_FORM_BMODIFY);
	arrID.Add(IDC_FORM_BUPDATEDB);
	arrID.Add(IDC_FORM_BLP_QUERY);
	arrID.Add(IDC_FORM_BDN_QUERY);
	arrID.Add(IDC_FORM_BRN_QUERY);
	arrID.Add(IDC_FORM_BGN_QUERY);


	bOk = m_resizer.CreatePanel(_T("Info_Panel"), &arrID, TRUE);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(_T("Info_Panel"), ANCHOR_LEFT);  // ANCHOR_HORIZONTALLY_CENTERED
	ASSERT(bOk);
	
	arrID.RemoveAll();

	arrID.Add(IDC_STATIC_FILTERS);
	arrID.Add(IDC_RADIO1_ORIGFILTER);
	arrID.Add(IDC_RADIO_FILTER1);
	arrID.Add(IDC_RADIO_FILTER2);
	arrID.Add(IDC_RADIO_FILTER3);

	arrID.Add(IDC_STATIC_COLOURING);
	arrID.Add(IDC_RADIO_COLOUR1);
	arrID.Add(IDC_RADIO_COLOUR2);
	arrID.Add(IDC_RADIO_COLOUR3); 
	arrID.Add(IDC_BUTTON_CHANGE_DETECT); 


	bOk = m_resizer.CreatePanel(_T("Image_Panel"), &arrID, TRUE);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(_T("Image_Panel"), ANCHOR_RIGHT);  // ANCHOR_HORIZONTALLY_CENTERED
	ASSERT(bOk);

	arrID.RemoveAll();

	arrID.Add(IDC_BUTTON_PERIPHERAL_STATUS);
	arrID.Add(IDC_STATIC_BARRIER);
	arrID.Add(IDC_BUTTON_BARRIER_OPEN);
	arrID.Add(IDC_BUTTON_BARRIER_CLOSE);
	arrID.Add(IDC_BUTTON_HEATER_ON);
	arrID.Add(IDC_BUTTON_HEATER_OFF);
	arrID.Add(IDC_BUTTON_ALARM_ON);
	arrID.Add(IDC_BUTTON_ALARM_OFF);

	bOk = m_resizer.CreatePanel(_T("Peripheral_Panel"), &arrID, TRUE);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(_T("Peripheral_Panel"), ANCHOR_LEFT |ANCHOR_BOTTOM);
	ASSERT(bOk);

	arrID.RemoveAll();

	arrID.RemoveAll();
	arrID.Add(IDC_STATIC_PLAKA);
	arrID.Add(IDC_STATIC_PLAKA_STR);
	arrID.Add(IDC_FORM_LPENTRY); 
	arrID.Add(IDC_FORM_BARA); 
	arrID.Add(IDC_STATIC_PTS);
	arrID.Add(IDC_BUTTON_PTS_STATUS);


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
		
	VERIFY(m_Font.CreateFont(40,                        // nHeight
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

	m_PlakaDisplayControl.SetFont(&m_Font);

	m_CropCheckBox.SetBitmap(::LoadBitmap(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP_CROP)));

	// start 1 second interval timer
	SetTimer(DISPLAY_TIMER_ID, TIMER_PERIOD_IN_MS, NULL);
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

	cv::Mat dMat3;

	if (g_AutoDetect_Type != NO_AUTO_VEHICLE_DETECT)
	{
		if (g_dEndIndex - g_dBeginIndex > 0)
		{
			cv::Rect region_of_interest = cv::Rect(0, g_dBeginIndex, g_CVImageTest.cols, g_dEndIndex - g_dBeginIndex);

			dMat3 = g_CVImageTest(region_of_interest).clone();

#ifdef DEBUG_BORA
			cv::imwrite("C:\\Users\\bora\\Desktop\\sil\\boraDeneme.bmp", dMat3);
#endif

			//transpose(dMat3, dMat1);
			//flip(dMat1, dMat2, 1); //transpose+flip(1)=CW
			//g_CVImageTest = dMat2.clone();

			g_CVImageTest = dMat3.clone();

			GetTestImageAsByte();

			if (m_TestCVMat.rows != 0 || m_TestCVMat.cols != 0)
			{
				m_TestCVMat.release();
			}

			m_iDisplayTestImageFilterType = (int)0;
			m_iDisplayRefImageFilterType = (int)0;
			m_iDisplayRefImageColourType = (int)0;
			m_iDisplayTestImageColourType = (int)0;

			m_MatToImageRef.SetFocus();

			m_TestCVMat = g_CVImageTest.clone();

			m_MatToImageTest.load(m_TestCVMat);
			//Invalidate(); UpdateWindow();

			std::string strFilename = PrepareImageFilename(m_FormELP, m_FormEDT);
			g_TestImageFileName = strFilename;
			SetEvent(g_CameraDBServerWriteFileEvent);


			// ali: update the database with the current image
			// ali: this has to go to the VehiclePassage table

			if (g_CVImageTest.cols > 0)
			{
				if (g_CVImageTestFilter1.rows != 0 || g_CVImageTestFilter1.cols != 0)
				{
					g_CVImageTestFilter1.release();
				}

				if (g_CVImageTestFilter2.rows != 0 || g_CVImageTestFilter2.cols != 0)
				{
					g_CVImageTestFilter2.release();
				}

				if (g_CVImageTestFilter3.rows != 0 || g_CVImageTestFilter3.cols != 0)
				{
					g_CVImageTestFilter3.release();
				}

				m_FilterRadioOriginal.EnableWindow(TRUE);
				m_FilterRadioOriginal.SetCheck(1);

				CButton* dRadioB;
				dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_FILTER1));
				dRadioB->SetCheck(0);
				dRadioB->EnableWindow(FALSE);

				dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_FILTER2));
				dRadioB->EnableWindow(FALSE);
				dRadioB->SetCheck(0);

				dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_FILTER3));
				dRadioB->EnableWindow(FALSE);
				dRadioB->SetCheck(0);

				m_ColourRadioOriginal.EnableWindow(FALSE);
				m_ColourRadioOriginal.SetCheck(1);

				dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_COLOUR2));
				dRadioB->SetCheck(0);
				dRadioB->EnableWindow(FALSE);

				dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_COLOUR3));
				dRadioB->EnableWindow(FALSE);
				dRadioB->SetCheck(0);

				SetEvent(g_ProcessFilter1Event);
				SetEvent(g_ProcessFilter2Event);
				SetEvent(g_ProcessFilter3Event);
			}

			if (g_ChangeDetectActive == 1)
			{
				if (g_CVTestChangeDetect.rows != 0 || g_CVTestChangeDetect.cols != 0)
				{
					g_CVTestChangeDetect.release();
				}
				g_CVTestChangeDetect.create(g_CVImageTest.size(), g_CVImageTest.type());

				CButton* dRadioB;
				dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_BUTTON_CHANGE_DETECT));
				dRadioB->EnableWindow(FALSE);

				SetEvent(g_StartChangeDetectEvent);
			}
		}
		else
		{
			ERRORMESSAGETYPE dTmp;
			char* cText = "Yeni araç altý görüntüsü elde edilemedi. Lütfen tekrar deneyiniz";
			strncpy_s(dTmp._OdroidText, (size_t)(MAX_PATH), cText, (size_t)(MAX_PATH));
			CThreadSafeQueue<_errorMessageType>::getInstance().push(dTmp);
			TRACE(L"\n\nOnCameraDataReady NO NEW IMAGE ACQUIRED\n\n");
		}
	}
	else
	{
		TRACE(L"OnCameraDataReady SetEvent(g_IntermediateImageReadyEvent)\n");
		SetEvent(g_IntermediateImageReadyEvent);
	}
	return 0;
}

afx_msg LRESULT CARALGISView::GetTestImage(WPARAM wParam, LPARAM lParam)
{
	cv::Mat dMat1, dMat2;

	cv::Mat dMat3;
	if (g_AutoDetect_Type == NO_AUTO_VEHICLE_DETECT)
	{
		if (g_dEndIndex - g_dBeginIndex > 0)
		{
			cv::Rect region_of_interest = cv::Rect(0, g_dBeginIndex, g_CVImageTest.cols, g_dEndIndex - g_dBeginIndex);

			dMat3 = g_CVImageTest(region_of_interest).clone();


			//transpose(dMat3, dMat1);
			//flip(dMat1, dMat2, 1); //transpose+flip(1)=CW
			//g_CVImageTest = dMat2.clone();

			g_CVImageTest = dMat3.clone();


			GetTestImageAsByte();

			if (m_TestCVMat.rows != 0 || m_TestCVMat.cols != 0)
			{
				m_TestCVMat.release();
			}

			m_iDisplayTestImageFilterType = (int)0;
			m_iDisplayRefImageFilterType = (int)0;
			m_iDisplayRefImageColourType = (int)0;
			m_iDisplayTestImageColourType = (int)0;

			m_MatToImageRef.SetFocus();
			
			m_TestCVMat = g_CVImageTest.clone();

			m_MatToImageTest.load(m_TestCVMat);
			//Invalidate(); UpdateWindow();

			if (g_CVImageTest.cols > 0)
			{
				if (g_CVImageTestFilter1.rows != 0 || g_CVImageTestFilter1.cols != 0)
				{
					g_CVImageTestFilter1.release();
				}

				if (g_CVImageTestFilter2.rows != 0 || g_CVImageTestFilter2.cols != 0)
				{
					g_CVImageTestFilter2.release();
				}

				if (g_CVImageTestFilter3.rows != 0 || g_CVImageTestFilter3.cols != 0)
				{
					g_CVImageTestFilter3.release();
				}

				m_FilterRadioOriginal.EnableWindow(TRUE);
				m_FilterRadioOriginal.SetCheck(1);

				CButton* dRadioB;
				dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_FILTER1));
				dRadioB->SetCheck(0);
				dRadioB->EnableWindow(FALSE);

				dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_FILTER2));
				dRadioB->EnableWindow(FALSE);
				dRadioB->SetCheck(0);

				dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_FILTER3));
				dRadioB->EnableWindow(FALSE);
				dRadioB->SetCheck(0);

				m_ColourRadioOriginal.EnableWindow(FALSE);
				m_ColourRadioOriginal.SetCheck(1);

				dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_COLOUR2));
				dRadioB->SetCheck(0);
				dRadioB->EnableWindow(FALSE);

				dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_COLOUR3));
				dRadioB->EnableWindow(FALSE);
				dRadioB->SetCheck(0);

				SetEvent(g_ProcessFilter1Event);
				SetEvent(g_ProcessFilter2Event);
				SetEvent(g_ProcessFilter3Event);
			}

			CButton* dRadioB;
			dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_BUTTON_CHANGE_DETECT));
			dRadioB->EnableWindow(FALSE);

			// start the change detection process
			if (g_CarFound == TRUE)
			{
				if (g_ChangeDetectActive == 1)
				{
					if (g_CVTestChangeDetect.rows != 0 || g_CVTestChangeDetect.cols != 0)
					{
						g_CVTestChangeDetect.release();
					}
					g_CVTestChangeDetect.create(g_CVImageTest.size(), g_CVImageTest.type());

					SetEvent(g_StartChangeDetectEvent);
				}
			}
		}
		else
		{
			ERRORMESSAGETYPE dTmp;
			char* cText = "Yeni araç altý görüntüsü elde edilemedi. Lütfen tekrar deneyiniz";
			strncpy_s(dTmp._OdroidText, (size_t)(MAX_PATH), cText, (size_t)(MAX_PATH));
			CThreadSafeQueue<_errorMessageType>::getInstance().push(dTmp);
		}
	}
	else
	{
		ERRORMESSAGETYPE dTmp;
		char* cText = "Yeni araç altý görüntüsü elde edilemedi. Lütfen tekrar deneyiniz";
		strncpy_s(dTmp._OdroidText, (size_t)(MAX_PATH), cText, (size_t)(MAX_PATH));
		CThreadSafeQueue<_errorMessageType>::getInstance().push(dTmp);
		TRACE(L"\n\n GetTestImage NO NEW IMAGE ACQUIRED\n\n");
	}

	return 0;
}


afx_msg LRESULT CARALGISView::OnDBaseFileWritten(WPARAM wParam, LPARAM lParam)
{
	TRACE("Test Image File Written\n");
	return 0;
}


afx_msg LRESULT CARALGISView::DisplayPlakaNo(WPARAM wParam, LPARAM lParam)
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

	aCString.MakeUpper();

	m_PlakaDisplayControl.SetWindowTextW(aCString);

	if (g_CVImageRefFilter1.rows != 0 || g_CVImageRefFilter1.cols != 0)
	{
		g_CVImageRefFilter1.release();
	}

	if (g_CVImageRefFilter2.rows != 0 || g_CVImageRefFilter2.cols != 0)
	{
		g_CVImageRefFilter2.release();
	}

	if (g_CVImageRefFilter3.rows != 0 || g_CVImageRefFilter3.cols != 0)
	{
		g_CVImageRefFilter3.release();
	}
	
	m_Ref1FilterOK = FALSE;
	m_Ref2FilterOK = FALSE;
	m_Ref3FilterOK = FALSE;

	m_FilterRadioOriginal.EnableWindow(FALSE);
	m_FilterRadioOriginal.SetCheck(1);

	CButton* dRadioB;
	dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_FILTER1));
	dRadioB->SetCheck(0);
	dRadioB->EnableWindow(FALSE);

	dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_FILTER2));
	dRadioB->EnableWindow(FALSE);
	dRadioB->SetCheck(0);

	dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_FILTER3));
	dRadioB->EnableWindow(FALSE);
	dRadioB->SetCheck(0);

	m_ColourRadioOriginal.EnableWindow(FALSE);

	m_iDisplayTestImageFilterType = (int)0;
	m_iDisplayRefImageFilterType = (int)0;
	m_iDisplayRefImageColourType = (int)0;
	m_iDisplayTestImageColourType = (int)0;

	m_MatToImageRef.SetFocus();
	
	dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_COLOUR2));
	dRadioB->SetCheck(0);
	dRadioB->EnableWindow(FALSE);

	dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_COLOUR3));
	dRadioB->EnableWindow(FALSE);
	dRadioB->SetCheck(0);

	OnLPUpdateInfo(aCString);

	SetEvent(g_ProcessFilter4Event);
	SetEvent(g_ProcessFilter5Event);
	SetEvent(g_ProcessFilter6Event);

	return 0;
}

afx_msg LRESULT CARALGISView::DisplayPTSImage(WPARAM wParam, LPARAM lParam)
{
	if (g_CVImageRef.rows != 0 || g_CVImageRef.cols != 0)
	{
		g_CVImageRef.release();
	}

	// TODO: Add your command handler code here
	m_CarPlakaImageStatic.FreeImage();
	m_CarPlakaImageStatic.Load(g_CarPlakaImage, (size_t)g_CarPlakaImageLenght);

	if (g_PTS_Producer_ID == PTS_BY_DIVIT)
	{
		//WPARAM wParam; 
		LPARAM pLparam;
		pLparam = reinterpret_cast<LPARAM>("ARALGIS");
		DisplayPlakaNo(0, pLparam);
	}

	return 0;
}


afx_msg LRESULT  CARALGISView::UpdatePTSStatusLost(WPARAM wParam, LPARAM lParam)
{
	m_PTS_Status.SetColour(WHITE, RED);
	return 0;
}

afx_msg LRESULT CARALGISView::UpdatePTSStatusNOK(WPARAM wParam, LPARAM lParam)
{
	m_PTS_Status.SetColour(WHITE, MAGENTA);
	return 0;
}

afx_msg LRESULT CARALGISView::UpdatePTSStatusOK(WPARAM wParam, LPARAM lParam)
{
	m_PTS_Status.SetColour(WHITE, GREEN);
	return 0;
}

afx_msg LRESULT CARALGISView::UpdatePeripheralStatusOK(WPARAM wParam, LPARAM lParam)
{
	m_PeripheralStatus.SetColour(WHITE, GREEN);
	m_BarrierOpenBtn.EnableWindow(TRUE);
	m_BarrierCloseBtn.EnableWindow(TRUE);
	m_HeaterStartBtn.EnableWindow(TRUE);
	m_HeterStopBtn.EnableWindow(TRUE);
	m_AlarmStartBtn.EnableWindow(TRUE);
	m_AlarmStop.EnableWindow(TRUE);

	return 0;
}

afx_msg LRESULT  CARALGISView::UpdatePeripheralStatusNOK(WPARAM wParam, LPARAM lParam)
{
	m_PeripheralStatus.SetColour(WHITE, RED);
	m_BarrierOpenBtn.EnableWindow(FALSE);
	m_BarrierCloseBtn.EnableWindow(FALSE);
	m_HeaterStartBtn.EnableWindow(FALSE);
	m_HeterStopBtn.EnableWindow(FALSE);
	m_AlarmStartBtn.EnableWindow(FALSE);
	m_AlarmStop.EnableWindow(FALSE);

	return 0;
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
}

void CARALGISView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CColorFormView::OnPaint() for painting messages



	if (g_CVImageTest.rows != 0)
	{
		LPDRAWITEMSTRUCT lpDrawItemStruct = NULL;
		m_MatToImageTest.DrawItem(lpDrawItemStruct);
	}

	if (g_CVImageRef.rows != 0)
	{
		LPDRAWITEMSTRUCT lpDrawItemStruct = NULL;
		m_MatToImageRef.DrawItem(lpDrawItemStruct);
	}

	// ali: mimic what is done above for the Preview image
	if (m_MatToGDIPrev != nullptr)
	{
		delete m_MatToGDIPrev;
		m_MatToGDIPrev = nullptr;

		m_MatToGDIPrev = new PkMatToGDI(m_PrevImgBMP, false);

		if (m_CVImagePrev.rows != 0)
		{
			m_MatToGDIPrev->DrawImg(m_PrevCVMat);
		}
	}
}


BOOL CARALGISView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return CColorFormView::OnEraseBkgnd(pDC);
}


afx_msg LRESULT CARALGISView::Filter1Available(WPARAM wParam, LPARAM lParam)
{
	if (m_Ref1FilterOK)
	{
		CButton* dRadioB;
		dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_FILTER1));
		dRadioB->SetCheck(0);
		dRadioB->EnableWindow(TRUE);

		m_ColourRadioOriginal.EnableWindow(TRUE);
		dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_COLOUR2));
		dRadioB->EnableWindow(TRUE);
		dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_COLOUR3));
		dRadioB->EnableWindow(TRUE);
	}
	return 0;
}

afx_msg LRESULT CARALGISView::Filter2Available(WPARAM wParam, LPARAM lParam)
{
	if (m_Ref2FilterOK)
	{
		//m_ButtonFilter2.EnableWindow(TRUE);

		CButton* dRadioB;
		dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_FILTER2));
		dRadioB->EnableWindow(TRUE);

		m_ColourRadioOriginal.EnableWindow(TRUE);
		dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_COLOUR2));
		dRadioB->EnableWindow(TRUE);
		dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_COLOUR3));
		dRadioB->EnableWindow(TRUE);
	}	return 0;
}

afx_msg LRESULT CARALGISView::Filter3Available(WPARAM wParam, LPARAM lParam)
{
	if (m_Ref3FilterOK)
	{
		CButton* dRadioB;
		dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_FILTER3));
		//dRadioB->SetCheck(0);
		dRadioB->EnableWindow(TRUE);

		m_ColourRadioOriginal.EnableWindow(TRUE);
		dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_COLOUR2));
		dRadioB->EnableWindow(TRUE);
		dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_COLOUR3));
		dRadioB->EnableWindow(TRUE);
	}
	return 0;
}

afx_msg LRESULT CARALGISView::Filter4Available(WPARAM wParam, LPARAM lParam)
{
	m_Ref1FilterOK = TRUE;

	return 0;
}

afx_msg LRESULT CARALGISView::Filter5Available(WPARAM wParam, LPARAM lParam)
{
	m_Ref2FilterOK = TRUE;
	return 0;
}

afx_msg LRESULT CARALGISView::Filter6Available(WPARAM wParam, LPARAM lParam)
{
	m_Ref3FilterOK = TRUE;

	return 0;
}


afx_msg LRESULT CARALGISView::ChangeDetectionFinished(WPARAM wParam, LPARAM lParam)
{
	CButton* dRadioB;
	dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_BUTTON_CHANGE_DETECT));
	dRadioB->EnableWindow(TRUE);
	return 0;
}

void CARALGISView::OnLbnSelchangeList2()
{
	// TODO: Add your control notification handler code here
	CString strLP;

	int index = m_formCBoxVisitList.GetCurSel();
	m_formCBoxVisitList.GetLBText(index, strLP);


	MessageBox(strLP);
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



// ali - UTILITY FUNCTION - begin

CString DriverNameFromID(long _id) 
{
	CString tempStrofInt;
	CDriverInfoSet dInfoSet;
	CString driverFilter = CString{ _T("[dbo].[Driver].[Type] = [dbo].[DriverType].[DTID] AND [dbo].[Driver].[ID] = '") };
	tempStrofInt.Format(_T("%ld"), _id);
	driverFilter += tempStrofInt;
	driverFilter += CString{ _T("'") };
	dInfoSet.m_strFilter = driverFilter;
	dInfoSet.Open(CRecordset::dynamic, nullptr, CRecordset::readOnly);
	//ASSERT(dInfoSet.GetRecordCount() == 1);
	CString strDriverName = dInfoSet.m_dboDriverLastName;
	strDriverName += _T(", ");
	strDriverName += dInfoSet.m_dboDriverName;
	return strDriverName;
}

CString GateFromID(long _id)
{
	CString tempStrofInt;
	CGateSet gSet;
	CString gateFilter = CString{ _T("ID = '") };
	tempStrofInt.Format(_T("%ld"), _id);
	gateFilter += tempStrofInt;
	gateFilter += CString{ _T("'") };
	gSet.m_strFilter = gateFilter;
	gSet.Open(CRecordset::dynamic, nullptr, CRecordset::readOnly);
	//ASSERT(gSet.GetRecordCount() == 1);
	CString strGate = gSet.m_GateType;
	return strGate;
}

CString KeeperNameFromID(long _id)
{
	CString tempStrofInt;
	CUserSet uSet;
	CString userFilter = CString{ _T("[User].[ID] = '") };
	tempStrofInt.Format(_T("%ld"), _id);
	userFilter += tempStrofInt;
	userFilter += CString{ _T("'") };
	uSet.m_strFilter = userFilter;
	uSet.Open(CRecordset::dynamic, nullptr, CRecordset::readOnly);
	CString strKeeperName = uSet.m_LastName;
	strKeeperName += _T(",");
	strKeeperName += uSet.m_Name;
	return strKeeperName;

	/* CString tempStrofInt;
	CUserInfoSet uInfoSet;
	CString userFilter = CString{ _T("[User].[ID] = [UserLog].[UserID] AND [User].[Type] = [UserType].[UTID] AND [User].[ID] = '") };
	tempStrofInt.Format(_T("%ld"), _id);
	userFilter += tempStrofInt;
	userFilter += CString{ _T("'") };
	uInfoSet.m_strFilter = userFilter;
	uInfoSet.Open(CRecordset::dynamic, nullptr, CRecordset::readOnly);
	//ASSERT(uInfoSet.GetRecordCount() == 1);
	CString strKeeperName = uInfoSet.m_dboUserLastName;
	strKeeperName += _T(",");
	strKeeperName += uInfoSet.m_dboUserName;
	return strKeeperName; */
}

void CARALGISView::ClearPreviewBox()
{
	CPaintDC dc{ this };
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	CStatic* pBox = static_cast<CStatic*>(GetDlgItem(IDC_FORM_PICTCTRL_CBPREVIEW));	
	CRect rect;
	pBox->GetClientRect(&rect);
	pBox->ClientToScreen(&rect);
	
}

void CARALGISView::ClearPictureBoxes()
{
	//if (m_MatToGDITest != nullptr)
	//{
	//	m_MatToGDITest->BackgroundClear();
	//	delete m_MatToGDITest;
	//	m_MatToGDITest = nullptr;
	//}

	//if (m_MatToGDIRef != nullptr)
	//{
	//	m_MatToGDIRef->BackgroundClear();
	//	delete m_MatToGDIRef;
	//	m_MatToGDIRef = nullptr;
	//}

	m_MatToImageTest.erase();
	m_MatToImageRef.erase();

	if (m_MatToGDIPrev != nullptr)
	{
		m_MatToGDIPrev->BackgroundClear();
		delete m_MatToGDIPrev;
		m_MatToGDIPrev = nullptr;
	}

}


void CARALGISView::PrepareDriverList()
{
	CDriverSet dSet;
	dSet.Open(CRecordset::dynamic, nullptr, CRecordset::readOnly);
	m_DriverList.clear();
	while (!dSet.IsEOF())
	{
		CString strDisplayName = dSet.m_Name + CString{ _T(" ") }+dSet.m_LastName;
		long lID = dSet.m_ID;
		m_DriverList.push_back(std::make_pair(strDisplayName, lID));
		dSet.MoveNext();
	}
	std::sort(m_DriverList.begin(), m_DriverList.end(),
		[](std::pair<CString, long> p1, std::pair<CString, long> p2) {
		if (p1 == p2) return p1.second < p2.second;
		else return p1 < p2;
	});
}

void CARALGISView::FillDriverList()
{
	m_FormCBDriverList.ResetContent();
	m_FormCBDriverList.InsertString(0,CString{ _T("Bilinmeyen Surucu") });
	m_FormCBDriverList.InsertString(1,CString{ _T("Yeni Surucu Ekle") });
	for(auto p : m_DriverList)
	{
		m_FormCBDriverList.InsertString(-1,p.first);
	}
}
// ali - UTILITY FUNCTION - end



// called to update the vehicle info boxes upon the retrieval of a new license plate number
void CARALGISView::OnLPUpdateInfo(CString strLP)
{
	if (g_CVImageRef.rows != 0 || g_CVImageRef.cols != 0)
	{
		g_CVImageRef.release();
	}

	m_iDisplayTestImageFilterType = (int)0;
	m_iDisplayRefImageFilterType = (int)0;
	m_iDisplayRefImageColourType = (int)0;
	m_iDisplayTestImageColourType = (int)0;

	m_MatToImageRef.SetFocus();

	if (m_TestCVMat.rows != 0 || m_TestCVMat.cols != 0)
	{
		m_TestCVMat.release();
	}

	m_Ref1FilterOK = FALSE;
	m_Ref2FilterOK = FALSE;
	m_Ref3FilterOK = FALSE;

	m_ColourRadioOriginal.EnableWindow(FALSE);
	m_ColourRadioOriginal.SetCheck(1);

	CButton* dRadioB;
	dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_COLOUR2));
	dRadioB->SetCheck(0);
	dRadioB->EnableWindow(FALSE);

	dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_COLOUR3));
	dRadioB->EnableWindow(FALSE);
	dRadioB->SetCheck(0);

	strLP.Remove(_T(' '));

	// new entry; check whether the previous car has been inserted
	if (!m_bVehiclePassageUpdated)
		UpdateVehiclePassage();
	m_bVehiclePassageUpdated = FALSE;

	// clear picture boxes
	ClearPictureBoxes();
	// m_MatToGDIPrev, m_MatToGDITest and m_MatToGDIRef
	// are set to nullptr at this point.

	m_formCBoxVisitList.ResetContent();
	FillDriverList();

	CVehiclePassageSet vPassageSet;
	// find all visits of the given LP
	CString filter = CString{ _T("LicensePlate = '") } +strLP;
	filter += CString{ _T("'") };
	vPassageSet.m_strFilter = filter;
	vPassageSet.m_strSort = _T("EntryDateTime DESC");

	m_FormEDT = CTime::GetCurrentTime();
	strLP.MakeUpper();
	m_FormELP = strLP;

	///////////////////////////// sils il sil sil sil 
	CString sil{ m_FormELP };
	int silmin = m_FormEDT.GetMinute();
	TRACE("sil: %s\nsilmin: %d", sil, silmin);
	///////////////////////////// sils il sil sil sil 


	// before executing the query;
	// it may be beneficial to bound the resulting record set
	// vPassageSet.SetRowsetSize() = RECENT_VISIT_COUNT;
	vPassageSet.Open(CRecordset::dynamic, nullptr, CRecordset::readOnly);
	if (vPassageSet.IsBOF()) {
		MessageBox(CString{ _T("Kayit bulunamadi; plaka no: ") } +strLP);
		m_formCBoxVisitList.AddString(LPCTSTR{ CString{ _T("Kayit yok") } });
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
	m_FormEBL.SetCheck(!vPassageSet.m_VehiclePassagePermissionGranted);

	// since there is an existing record; m_VID >= 0
	m_VID = vPassageSet.m_VehiclePassageVehicleID;
	ASSERT(m_VID >= 0);

	m_FormEDID = DriverNameFromID(vPassageSet.m_VehiclePassageDriverID);
	m_DID = vPassageSet.m_VehiclePassageDriverID;


	m_FormEGID = GateFromID(vPassageSet.m_VehiclePassageGateID);
	m_GID = vPassageSet.m_VehiclePassageGateID;

	m_FormEUID = KeeperNameFromID(vPassageSet.m_VehiclePassageUserID);
	m_UID = vPassageSet.m_VehiclePassageUserID;

	auto VisitListIndex = 0;

	// careful with looping: do not go out of bounds for m_VisitInfo array
	while (!vPassageSet.IsEOF() && VisitListIndex < VISIT_LIST_LENGTH) {
		CTime visitDate = vPassageSet.m_VehiclePassageEntryDateTime;  
		m_formCBoxVisitList.AddString(visitDate.Format("%d/%m/%Y %X"));
		m_VisitInfo[VisitListIndex++].Prepare(
			DriverNameFromID(vPassageSet.m_VehiclePassageDriverID),
			KeeperNameFromID(vPassageSet.m_VehiclePassageUserID),
			GateFromID(vPassageSet.m_VehiclePassageGateID));

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

	// prepare the reference image filename: <license_plate>_ref.jpg
	std::string strRefFilename = PrepareImageFilename(strLP, CString{ _T("ref.jpg") });

	//if (g_CVImageRef.rows != 0 || g_CVImageRef.cols != 0)
	//{
	//	g_CVImageRef.release();
	//}

	cv::Mat pTemp;

	if (g_CameraPixelBits == 24)
	{
		g_CVImageRef = cv::imread(strRefFilename, cv::IMREAD_COLOR);
	}
	else
	{
		g_CVImageRef = cv::imread(strRefFilename, cv::IMREAD_GRAYSCALE);
	}

	if (m_RefCVMat.rows != 0 || m_RefCVMat.cols != 0)
	{
		m_RefCVMat.release();
	}

	m_RefCVMat = g_CVImageRef.clone();

	m_MatToImageRef.load(m_RefCVMat);
	//Invalidate(); UpdateWindow();

	UpdateData(FALSE);
	// m_formCBoxVisitList.AddString(strLP);
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
	if (g_PTS_Producer_ID == PTS_BY_ISSD)
	{
		if (g_ByteImageTest)
		{
			delete[] g_ByteImageTest;
		}

		g_ByteImageSize = (unsigned int)((int)g_CVImageTest.step[0] * (int)g_CVImageTest.rows);

		g_ByteImageTest = new BYTE[g_ByteImageSize];

		memcpy(g_ByteImageTest, g_CVImageTest.data, g_ByteImageSize);

		SetEvent(g_SendUVSSImageEvent);
	}
}

// returns the full path for the default image
std::string CARALGISView::PrepareImageFilename()
{
	CString strFilename = m_PathToCars + CString{ _T("default.jpg") };
	LPCTSTR  cpImgFilename = LPCTSTR(strFilename);

	std::basic_string<TCHAR, std::char_traits<TCHAR>> strStdFilename{ cpImgFilename };
	std::string strFinalFilename{ strStdFilename.begin(), strStdFilename.end() };

	return strFinalFilename;
}

// returns the full path for the car with license plate _lp and date/time _datetime
// the format is: <m_PathToCars><_lp>_<DDMMYYYY_hhmmss>.jpg
std::string CARALGISView::PrepareImageFilename(CString const & _lp, CTime const & _datetime)
{
	return PrepareImageFilename(_lp, _datetime.Format(_T("%d%m%Y_%H%M%S.jpg")));
}

// returns the full path for the car with license plate _lp and string _suffix
// the format is: <m_PathToCars><_lp>_<_suffix>
// note: _suffix should end with .jpg
std::string CARALGISView::PrepareImageFilename(CString const & _lp, CString const & _suffix)
{
	CString strPrevTag = _lp + _T("_") + _suffix;
	CString strFilename = m_PathToCars + strPrevTag;
	LPCTSTR cpImgFilename = LPCTSTR(strFilename);

	std::basic_string<TCHAR, std::char_traits<TCHAR>> strStdFilename{ cpImgFilename };
	std::string strFinalFilename{ strStdFilename.begin(), strStdFilename.end() };
	
	return strFinalFilename;
}

BOOL CARALGISView::SaveImage(std::string const & _filename)
{
	cv::Mat MatToSave{};
	BOOL bSuccess = TRUE;

	if (m_TestCVMat.empty())
	{
		bSuccess = FALSE;
		// in the debug code, if there is a valid preview image
		// that will be saved as the current test image
		// - this is to artificially populate the set of saved images
		// the default image is used, if the preview image box is also empty.
#ifdef _DEBUG  
		if (m_PrevCVMat.empty())
		{
			//MessageBox(_T("m_TestCVMat is empty!\nWriting default image."));
			//m_TestCVMat = cv::imread("c:\\ali\\github-home\\ARALGIS\\Cars\\ChassisBottom\\default.jpg", cv::IMREAD_COLOR);
			//m_TestCVMat = cv::imread(PrepareImageFilename(), cv::IMREAD_COLOR);
			MatToSave = cv::imread(PrepareImageFilename(), cv::IMREAD_COLOR);
		}
		else
		{
			// m_TestCVMat = m_PrevCVMat.clone();
			// MatToSave = m_PrevCVMat.clone();
			// should be safe to copy assign to an automatic variable
			// so avoid the expensive clone operator
			MatToSave = m_PrevCVMat;
		}
#endif

		// in the release code, the default image is saved in the absence of a valid test image
		// currently, this operation is redundant because if there is no matching image file for
		// a given datetime, the default image is selected.
		// however, in future versions this will give flexibility to distinguish between
		// an entry which did not produce a proper image vs.
		// a visit datetime for which there was no file saved (may signal an abnormal database operation).
		// ali - addendum
		// if the current chassis bottom image is not present, do not modify the reference image file!
		// so do not write a new image.
//#ifndef _DEBUG 
//		MatToSave = cv::imread(PrepareImageFilename(), cv::IMREAD_COLOR);
//#endif
	}

	// ali - addendum
	// In the RELEASE version: 
	// if the current chassis bottom image is not present, do not modify the reference image file!
	// so do not write a new image.
#ifndef _DEBUG
	if (bSuccess)
		cv::imwrite(_filename, static_cast<cv::InputArray>(MatToSave));
	// in debug version, modify the reference file regardless.
#else
	cv::imwrite(_filename, static_cast<cv::InputArray>(MatToSave));
#endif

	return bSuccess;
}

// As the database is being updated with the new entry information,
// update the reference image as well.
// For now, the reference image is simply set to be the current entry image.
// The image is contained in the member variable m_CVImageTest which is
// used in the SaveImage member method.
BOOL CARALGISView::UpdateRefImage(CString const & _lp)
{
	std::string strRefFilename = PrepareImageFilename(_lp, CString{ _T("ref.jpg") });
	return SaveImage(strRefFilename);
}

// updates the VehiclePassageTable with the on-screen contents
void CARALGISView::UpdateVehiclePassage()
{
	// begin by saving the current image in m_CVImageTest
	std::string strFilename = PrepareImageFilename(m_FormELP, m_FormEDT);
	SaveImage(strFilename);

	// update the reference image
	UpdateRefImage(m_FormELP);

	CVehiclePassageSet vPassageSet;
	if (m_bVehiclePassageUpdated)
	{
		CString filter{ _T("[EntryDateTime] = '") };
		filter += m_FormEDT.Format(_T("%Y-%m-%d %X"));
		/*filter += _T("' AND [LastName] = '");
		filter += strLastName;
		*/
		filter += _T("'");

		vPassageSet.m_strFilter = filter;

	}
	else
	{
		//this is unnecessary; vPassageSet is automatic
		//and cannot have residue from previous call
		//vPassageSet.m_strFilter = _T("");
	}

	vPassageSet.Open(CRecordset::dynamic, nullptr, CRecordset::none);

	// no modification of existing rows in the VehiclePassage table;
	// always add a new row.
	if (m_bVehiclePassageUpdated)
	{
		vPassageSet.Edit();
	}
	else
	{
		vPassageSet.AddNew();
	}

	vPassageSet.m_VehiclePassageLicensePlate = m_FormELP;
	vPassageSet.m_VehiclePassageEntryDateTime = m_FormEDT;
	vPassageSet.m_VehiclePassageUserID = m_UID;
	vPassageSet.m_VehiclePassageGateID = m_GID;
	vPassageSet.m_VehiclePassageDriverID = m_DID;
	vPassageSet.m_VehiclePassagePermissionGranted = !m_FormEBL.GetCheck();

	/*CString msg = _T("These will be used by AddNew():");
	CString paramStr;
	paramStr.Format(_T("\nLicense Plate= %s,\nDriver ID= %ld,\nGate ID= %ld,\nGate Keeper ID= %ld,\nApproved= %d.\n"), m_FormELP, m_DID, m_GID, m_UID, !m_FormEBL.GetCheck());

	MessageBox(msg + paramStr); */

	vPassageSet.Update();
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

	if (dUpAck.DoModal() == IDOK) 
	{ // Update operations
		UpdateVehiclePassage();
		m_bVehiclePassageUpdated = TRUE;
	}
	else 
	{ // nothing to do (yet)
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



void CARALGISView::OnBnClickedRadioColour1()
{
	// TODO: Add your control notification handler code here
	m_iDisplayTestImageColourType = (int)0;

	if ((m_TestCVMat.rows != 0) && (m_TestCVMat.cols != 0))
	{
		m_TestCVMat.release();

		if (m_iDisplayTestImageFilterType == (int)0)
		{
			m_TestCVMat = g_CVImageTest.clone();
		}
		else if (m_iDisplayTestImageFilterType == (int)1)
		{
			m_TestCVMat = g_CVImageTestFilter1.clone();
		}
		else if (m_iDisplayTestImageFilterType == (int)2)
		{
			m_TestCVMat = g_CVImageTestFilter2.clone();
		}
		else if (m_iDisplayTestImageFilterType == (int)3)
		{
			m_TestCVMat = g_CVImageTestFilter3.clone();
		}

		m_MatToImageTest.load(m_TestCVMat);
		//Invalidate(); //UpdateWindow();
	}

	m_iDisplayRefImageColourType = (int)0;

	if ((m_RefCVMat.rows != 0) && (m_RefCVMat.cols != 0))
	{
		m_RefCVMat.release();

		if (m_iDisplayRefImageFilterType == (int)0)
		{
			m_RefCVMat = g_CVImageRef.clone();
		}
		else if (m_iDisplayRefImageFilterType == (int)1)
		{
			m_RefCVMat = g_CVImageRefFilter1.clone();
		}
		else if (m_iDisplayRefImageFilterType == (int)2)
		{
			m_RefCVMat = g_CVImageRefFilter2.clone();
		}
		else if (m_iDisplayRefImageFilterType == (int)3)
		{
			m_RefCVMat = g_CVImageRefFilter3.clone();
		}

		m_MatToImageRef.load(m_RefCVMat);
		//Invalidate(); //UpdateWindow();
	}
}


void CARALGISView::OnBnClickedRadioColour2()
{
	// TODO: Add your control notification handler code here
	cv::Mat dTempCV;
	m_iDisplayTestImageColourType = (int)1;

	if ((m_TestCVMat.rows != 0) && (m_TestCVMat.cols != 0))
	{
		if (m_iDisplayTestImageFilterType == (int)0)
		{
			cv::applyColorMap(g_CVImageTest, dTempCV, cv::COLORMAP_RAINBOW);
		}
		else if (m_iDisplayTestImageFilterType == (int)1)
		{
			cv::applyColorMap(g_CVImageTestFilter1, dTempCV, cv::COLORMAP_RAINBOW);
		}
		else if (m_iDisplayTestImageFilterType == (int)2)
		{
			cv::applyColorMap(g_CVImageTestFilter2, dTempCV, cv::COLORMAP_RAINBOW);
		}
		else if (m_iDisplayTestImageFilterType == (int)3)
		{
			cv::applyColorMap(g_CVImageTestFilter3, dTempCV, cv::COLORMAP_RAINBOW);
		}

		m_TestCVMat.release();
		m_TestCVMat = dTempCV.clone();

		m_MatToImageTest.load(m_TestCVMat);
		//Invalidate(); //UpdateWindow();
	}

	m_iDisplayRefImageColourType = (int)1;

	if ((m_RefCVMat.rows != 0) && (m_RefCVMat.cols != 0))
	{
		if (m_iDisplayRefImageFilterType == (int)0)
		{
			cv::applyColorMap(g_CVImageRef, dTempCV, cv::COLORMAP_RAINBOW);
		}
		else if (m_iDisplayRefImageFilterType == (int)1)
		{
			cv::applyColorMap(g_CVImageRefFilter1, dTempCV, cv::COLORMAP_RAINBOW);
		}
		else if (m_iDisplayRefImageFilterType == (int)2)
		{
			cv::applyColorMap(g_CVImageRefFilter2, dTempCV, cv::COLORMAP_RAINBOW);
		}
		else if (m_iDisplayRefImageFilterType == (int)3)
		{
			cv::applyColorMap(g_CVImageRefFilter3, dTempCV, cv::COLORMAP_RAINBOW);
		}

		m_RefCVMat.release();
		m_RefCVMat = dTempCV.clone();

		m_MatToImageRef.load(m_RefCVMat);
		//Invalidate(); //UpdateWindow();
	}
}


void CARALGISView::OnBnClickedRadioColour3()
{
	// TODO: Add your control notification handler code here
	cv::Mat dTempCV;
	m_iDisplayTestImageColourType = (int)2;


	if ((m_TestCVMat.rows != 0) && (m_TestCVMat.cols != 0))
	{
		if (m_iDisplayTestImageFilterType == (int)0)
		{
			cv::applyColorMap(g_CVImageTest, dTempCV, cv::COLORMAP_JET);
		}
		else if (m_iDisplayTestImageFilterType == (int)1)
		{
			cv::applyColorMap(g_CVImageTestFilter1, dTempCV, cv::COLORMAP_JET);
		}
		else if (m_iDisplayTestImageFilterType == (int)2)
		{
			cv::applyColorMap(g_CVImageTestFilter2, dTempCV, cv::COLORMAP_JET);
		}
		else if (m_iDisplayTestImageFilterType == (int)3)
		{
			cv::applyColorMap(g_CVImageTestFilter3, dTempCV, cv::COLORMAP_JET);
		}

		m_TestCVMat.release();
		m_TestCVMat = dTempCV.clone();

		m_MatToImageTest.load(m_TestCVMat);
		//Invalidate(); UpdateWindow();
	}

	m_iDisplayRefImageColourType = (int)2;

	if ((m_RefCVMat.rows != 0) && (m_RefCVMat.cols != 0))
	{
		if (m_iDisplayRefImageFilterType == (int)0)
		{
			cv::applyColorMap(g_CVImageRef, dTempCV, cv::COLORMAP_JET);
		}
		else if (m_iDisplayRefImageFilterType == (int)1)
		{
			cv::applyColorMap(g_CVImageRefFilter1, dTempCV, cv::COLORMAP_JET);
		}
		else if (m_iDisplayRefImageFilterType == (int)2)
		{
			cv::applyColorMap(g_CVImageRefFilter2, dTempCV, cv::COLORMAP_JET);
		}
		else if (m_iDisplayRefImageFilterType == (int)3)
		{
			cv::applyColorMap(g_CVImageRefFilter3, dTempCV, cv::COLORMAP_JET);
		}

		m_RefCVMat.release();
		m_RefCVMat = dTempCV.clone();

		m_MatToImageRef.load(m_RefCVMat);
		//Invalidate(); //UpdateWindow();
	}
}

void CARALGISView::OnBnClickedRadio1Origfilter()
{
	// TODO: Add your control notification handler code here
	m_iDisplayTestImageFilterType = (int)0;
	m_iDisplayRefImageFilterType = (int)0;

	if (m_iDisplayRefImageColourType == (int)0)
	{
		OnBnClickedRadioColour1();
	}
	else if (m_iDisplayRefImageColourType == (int)1)
	{
		OnBnClickedRadioColour2();
	}
	else if (m_iDisplayRefImageColourType == (int)2)
	{
		OnBnClickedRadioColour3();
	}
}


void CARALGISView::OnBnClickedRadioFilter1()
{
	// TODO: Add your control notification handler code here
	m_iDisplayTestImageFilterType = (int)1;
	m_iDisplayRefImageFilterType = (int)1;

	if (m_iDisplayRefImageColourType == (int)0)
	{
		OnBnClickedRadioColour1();
	}
	else if (m_iDisplayRefImageColourType == (int)1)
	{
		OnBnClickedRadioColour2();
	}
	else if (m_iDisplayRefImageColourType == (int)2)
	{
		OnBnClickedRadioColour3();
	}
}


void CARALGISView::OnBnClickedRadioFilter2()
{
	// TODO: Add your control notification handler code here
	m_iDisplayTestImageFilterType = (int)2;
	m_iDisplayRefImageFilterType = (int)2;

	if (m_iDisplayRefImageColourType == (int)0)
	{
		OnBnClickedRadioColour1();
	}
	else if (m_iDisplayRefImageColourType == (int)1)
	{
		OnBnClickedRadioColour2();
	}
	else if (m_iDisplayRefImageColourType == (int)2)
	{
		OnBnClickedRadioColour3();
	}
}


void CARALGISView::OnBnClickedRadioFilter3()
{
	// TODO: Add your control notification handler code here
	m_iDisplayTestImageFilterType = (int)3;
	m_iDisplayRefImageFilterType = (int)3;

	if (m_iDisplayRefImageColourType == (int)0)
	{
		OnBnClickedRadioColour1();
	}
	else if (m_iDisplayRefImageColourType == (int)1)
	{
		OnBnClickedRadioColour2();
	}
	else if (m_iDisplayRefImageColourType == (int)2)
	{
		OnBnClickedRadioColour3();
	}
}


void CARALGISView::OnBnClickedButtonChangeDetect()
{
	// TODO: Add your control notification handler code here
	cv::Mat dTestImage;
	cv::Mat dMat1, dMat2, dMat3, dMat4;

	cv::Mat AA1, AA2, AA3, AA4, AA5, AA6;

	if (g_CVTestWrapped.rows != 0 && g_CVTestWrapped.cols != 0)
	{
		//transpose(g_CVTestWrapped, AA1);
		//flip(AA1, AA2, 1); //transpose+flip(1)=CW
		//AA2.copyTo(AA3);
		g_CVTestWrapped.copyTo(AA3);

		//transpose(g_CVTestChangeDetect, AA4);
		//flip(AA4, AA5, 1); //transpose+flip(1)=CW
		//AA5.copyTo(AA6);
		g_CVTestChangeDetect.copyTo(AA6);

		dTestImage.create(AA3.size(), AA3.type());
		dTestImage = AA3.clone();

		// 1. create wrapped image first as grey since it is calculated with false colour
		// 2. change it to colour (it is grey in fact) so that it has 3 channels again
		// it will be ready for blending  
		cvtColor(AA3, dMat3, CV_BGR2GRAY);

		cvtColor(dMat3, dMat1, CV_GRAY2BGR);
		////// end of wrapped image preparation

		// create detected changes as 
		// red colour image with 3 channels
		dMat2.create(AA6.size(), CV_8UC3);

		std::vector<cv::Mat> bgr_planes(3);
		cv::split(dMat2, bgr_planes);

		bgr_planes[2] = AA6.clone();
		cv::merge(bgr_planes, dMat2);
		// red colour image creation ends

		float fBlend = 0.75;
		dMat4 = fBlend*dMat1 + (1.0 - fBlend)*dMat2;


		//cv::namedWindow("g_CVTestWrapped", cv::WINDOW_NORMAL);
		//cv::imshow("g_CVTestWrapped", AA3);
		//cv::waitKey(0);

		//cv::namedWindow("dTestImage", cv::WINDOW_NORMAL);
		//cv::imshow("dTestImage", dTestImage);
		//cv::waitKey(0);

		//cv::namedWindow("dMat3", cv::WINDOW_NORMAL);
		//cv::imshow("dMat3", dMat3);
		//cv::waitKey(0);

		//cv::namedWindow("dMat1", cv::WINDOW_NORMAL);
		//cv::imshow("dMat1", dMat1);
		//cv::waitKey(0);

		//cv::namedWindow("g_CVTestChangeDetect", cv::WINDOW_NORMAL);
		//cv::imshow("g_CVTestChangeDetect", AA6);
		//cv::waitKey(0);

		//cv::namedWindow("dMat2", cv::WINDOW_NORMAL);
		//cv::imshow("dMat2", dMat2);
		//cv::waitKey(0);

		//cv::namedWindow("dMat4", cv::WINDOW_NORMAL);
		//cv::imshow("dMat4", dMat4);
		//cv::waitKey(0);

		//CBitmapDlg dlgBitmap;
		//dlgBitmap.SetCVMat(dMat4);
		//dlgBitmap.DoModal();

		CImageToolDialog dlgBitmap;
		dlgBitmap.SetCVMat(dMat4);
		dlgBitmap.DoModal();
	}
}


/** Called every time the highlighted previous date is altered.
	Read the associated image filename and publish the image in the
	small preview box (IDC_FORM_PICCTRL_CBPREVIEW). 
	*/
void CARALGISView::OnCbnSelchangeFormCboxVisitlist()
{
	// TODO: Add your control notification handler code here
	CString strVisitDateTime;

	int index = m_formCBoxVisitList.GetCurSel();
	m_formCBoxVisitList.GetLBText(index, strVisitDateTime);

	// prepare the associated image filename
	strVisitDateTime.Replace(' ', '_');
	strVisitDateTime.Remove('/');
	strVisitDateTime.Remove(':');
	strVisitDateTime += _T(".jpg");
	// m_FormELP must have been set at the time of the License Plate reception
	std::string strFilename = PrepareImageFilename(m_FormELP, strVisitDateTime);
	//CString strPrevTag = m_FormELP + _T("_") + strVisitDateTime;
	//std::string strFilename = PrepareImageFilename(m_FormELP, strVisitDateTime);
	
	//CString strFilename = m_PathToCars + strPrevTag;
	//strFilename += _T(".img");

	DWORD dwAttrib = GetFileAttributes(CString{ strFilename.c_str() });
	//char charFilename[MAX_FILENAME_LENGTH];

	if (dwAttrib == INVALID_FILE_ATTRIBUTES ||
		(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) 
	{ // if no such file exists, use the default image
		strFilename = PrepareImageFilename();
	}

	if (m_CVImagePrev.rows != 0 || m_CVImagePrev.cols != 0)
	{
		m_CVImagePrev.release();
	}

	if (g_CameraPixelBits == 24)
	{
		m_CVImagePrev = cv::imread(strFilename, cv::IMREAD_COLOR);
	}
	else
	{
		m_CVImagePrev = cv::imread(strFilename, cv::IMREAD_GRAYSCALE);
	}

	if (m_MatToGDIPrev != nullptr)
	{
		delete m_MatToGDIPrev;
		m_MatToGDIPrev = nullptr;
	}

	m_PrevCVMat = m_CVImagePrev.clone();
	m_MatToGDIPrev = new PkMatToGDI(m_PrevImgBMP, false);
	m_MatToGDIPrev->DrawImg(m_CVImagePrev);

	// fill in the rest of visit information boxes.
	// careful: index is in reverse
	auto visitInfoIndex = m_formCBoxVisitList.GetCount() - index - 1;
	m_FormEDID = m_VisitInfo[visitInfoIndex].GetDriver();
	m_FormEUID = m_VisitInfo[visitInfoIndex].GetKeeper();
	m_FormEGID = m_VisitInfo[visitInfoIndex].GetGate();

	UpdateData(FALSE);

	// MessageBox(strVisitDateTime);
	
}


HBRUSH CARALGISView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	// HBRUSH hbr = CColorFormView::OnCtlColor(pDC, pWnd, nCtlColor);
	CColorFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	//if (pWnd->GetDlgCtrlID() == IDC_FORM_PICTCTRL_CBPREVIEW)
	//{
	//	pDC->SetBkColor(RGB(COLOUR_RED, COLOUR_GREEN, COLOUR_BLUE));
	//	pDC->SetDCBrushColor(RGB(COLOUR_RED, COLOUR_GREEN, COLOUR_BLUE));
	//	tmpBrush.CreateSolidBrush(RGB(COLOUR_RED, COLOUR_GREEN, COLOUR_BLUE));
	//}

	// TODO:  Change any attributes of the DC here

	// TODO:  Return a different brush if the default is not desired
	return (HBRUSH)m_brush;
}


void CARALGISView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == DISPLAY_TIMER_ID)
	{
		SetEvent(g_ControlHardDiskSpaceEvent);
	} // End if.
	CColorFormView::OnTimer(nIDEvent);
}


void CARALGISView::OnBnClickedButtonExit()
{
	if (::MessageBox(GetSafeHwnd(),
		(LPCWSTR)L"Do you want to close the application?",
		(LPCWSTR)WARNINGWINDOW_TITLE,
		MB_OKCANCEL | MB_ICONQUESTION)
		== IDCANCEL)
		return;

	AfxGetMainWnd()->PostMessage(WM_CLOSE, 0, 0);
}


void CARALGISView::OnLButtonDown(UINT nFlags, CPoint point)
{
	//TRACE("OnLButtonDown");

	CFrameWnd* dFrm;
	dFrm = GetTopLevelFrame();
	CView* pView = dFrm->GetActiveView();
	//SendMessage(WM_LMOUSE_DOWN, 0, 0);

	if (m_IsCropActive) 
	{
		if (isInUVSSImages(point) == 1)
		{
			CClientDC aDC{ this };
			OnPrepareDC(&aDC);
			aDC.DPtoLP(&point); // convert to logical coordinates
			// all points are stored in logical coordinates

			m_FirstPoint = point;

			// capture subsequent mouse message
			SetCapture();
		}
	}
	CColorFormView::OnLButtonDown(nFlags, point);
}

void CARALGISView::OnLButtonUp(UINT nFlags, CPoint point)
{
	//TRACE("OnLButtonUp");

	if ((m_IsCropActive) && isInUVSSImages(point) == 1)
	{
		if (this == GetCapture())
			ReleaseCapture();

		if (m_pTempElement)
		{
			// get enclosing rectangle
			CRect aRect{ m_pTempElement->GetEnclosingRect() };

			// Convert Rectangle coordinates
			CClientDC aDC{ this };
			OnPrepareDC(&aDC);
			aDC.LPtoDP(aRect);

			//aDC.SetROP2(R2_MERGEPENNOT); // boraN

			InvalidateRect(aRect); // this information  is passedd to WM_PAINT message additevely

			delete m_pTempElement;
			m_pTempElement = NULL;
			m_CropWhichImage = -1;
		}
	}

	CColorFormView::OnLButtonUp(nFlags, point);
}


void CARALGISView::OnMouseMove(UINT nFlags, CPoint point)
{
	if ((m_IsCropActive) && isInUVSSImages(point) == 1)
	{
		CClientDC aDC{ this };
		OnPrepareDC(&aDC);
		aDC.DPtoLP(&point);

		if ((nFlags & MK_LBUTTON) && (this == GetCapture()))
		{
			m_SecondPoint = point;

			if (m_pTempElement)
			{
				// not a curve so delete the previously drawn element
				aDC.SetROP2(R2_NOTXORPEN);
				m_pTempElement->Draw(&aDC);
			}

			delete m_pTempElement;
			m_pTempElement = NULL;
			// create a new temp element
			m_pTempElement = new CRectangle(m_FirstPoint, m_SecondPoint, RGB(255, 0, 0), 1);
			m_pTempElement->Draw(&aDC);

			//TRACE("IMAGE No %d\n\n", m_CropWhichImage);
		}
	}

	CColorFormView::OnMouseMove(nFlags, point);
}


void CARALGISView::OnBnClickedCheckCrop()
{
	bool oldVal = m_IsCropActive;
	
	if (oldVal)
		m_IsCropActive = false;
	else
		m_IsCropActive = true;

	m_CropWhichImage = -1;
}
int CARALGISView::isInUVSSImages(CPoint aInPoint)
{
	RECT r;
	CWnd* h = GetDlgItem(IDC_STATIC_TESTBMP);

	h->GetWindowRect(&r); //get window rect of control relative to screen
	ScreenToClient(&r); //optional step - see below

	if ((aInPoint.y > r.top) &&
		(aInPoint.y < r.bottom) &&
		(aInPoint.x > r.left) &&
		(aInPoint.x < r.right) &&
		(m_TestCVMat.rows != 0) &&
		(m_TestCVMat.cols != 0))
	{
		if (m_CropWhichImage == -1)
		{
			m_CropWhichImage = 0;
			return 1;
		}
		else if (m_CropWhichImage == 0)
		{
			return 1;
		}
		else
		{
			//TRACE("Test return false for %d\n", m_CropWhichImage);
			return 0;
		}
	}
	else
	{
		h = GetDlgItem(IDC_STATIC_REFBMP);
		h->GetWindowRect(&r); //get window rect of control relative to screen
		ScreenToClient(&r); //optional step - see below

		if ((aInPoint.y > r.top) &&
			(aInPoint.y < r.bottom) &&
			(aInPoint.x > r.left) &&
			(aInPoint.x < r.right) &&
			(m_RefCVMat.rows != 0) &&
			(m_RefCVMat.cols != 0))		
		{
			if (m_CropWhichImage == -1)
			{
				m_CropWhichImage = 1;
				return 2;
			}
			else if (m_CropWhichImage == 1)
			{
				return 2;
			}
			else
			{
				//TRACE("Reference return false f0r %d\n", m_CropWhichImage);
				return 0;
			}		
		}
	}	

	//TRACE("returm false fpr %d\n\n", m_CropWhichImage);
	return 0;
}


afx_msg LRESULT CARALGISView::OnLButtonDownImage(WPARAM wparam, LPARAM lparam)
{
	//TRACE("OnLButtonDownImage\n");
	CRect rect; 
	GetDlgItem(IDC_STATIC_PLAKA)->GetWindowRect(&rect);
	InvalidateRect(rect, FALSE);
	UpdateWindow();
	return 1;
}

afx_msg LRESULT CARALGISView::OnLButtonUpImage(WPARAM wparam, LPARAM lparam)
{
	//TRACE("OnLButtonDownImage\n");
	CRect rect;
	GetDlgItem(IDC_STATIC_PLAKA)->GetWindowRect(&rect);
	InvalidateRect(rect, FALSE);
	UpdateWindow();
	return 1;
}


afx_msg LRESULT CARALGISView::OnMouseMoveImage(WPARAM wparam, LPARAM lparam)
{
	//TRACE("OnMouseMoveImage\n");
	CRect rect; 
	GetDlgItem(IDC_STATIC_PLAKA)->GetWindowRect(&rect);
	InvalidateRect(rect, FALSE);
	UpdateWindow();
	return 1;
}

afx_msg LRESULT CARALGISView::OnMouseWheelImage(WPARAM wparam, LPARAM lparam)
{
	//TRACE("OnMouseWheel\n");
	CRect rect;
	GetDlgItem(IDC_STATIC_PLAKA)->GetWindowRect(&rect);
	InvalidateRect(rect, FALSE);
	UpdateWindow();
	return 1;
}


BOOL CARALGISView::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
		RECT r;
		CWnd* h = GetDlgItem(IDC_STATIC_TESTBMP);
	
		cv::Mat dMat1, dMat2;
	
		h->GetWindowRect(&r); //get window rect of control relative to screen
		ScreenToClient(&r); //optional step - see below
	
		if ((point.y > r.top) &&
			(point.y < r.bottom) &&
			(point.x > r.left) &&
			(point.x < r.right))
		{
			if ((m_TestCVMat.rows != 0) && (m_TestCVMat.cols != 0))
			{
				m_MatToImageTest.bMouseWheel(nFlags, zDelta, point);
			}
		}
		else
		{
			h = GetDlgItem(IDC_STATIC_REFBMP);
			h->GetWindowRect(&r); //get window rect of control relative to screen
			ScreenToClient(&r); //optional step - see below
	
			if ((point.y > r.top) &&
				(point.y < r.bottom) &&
				(point.x > r.left) &&
				(point.x < r.right))
			{
				//CBitmapDlg dlgBitmap;
				if ((m_RefCVMat.rows != 0) && (m_RefCVMat.cols != 0))
				{
					m_MatToImageRef.bMouseWheel(nFlags, zDelta, point);
				}
			}
		}

		return CColorFormView::OnMouseWheel(nFlags, zDelta, point);
}

void CARALGISView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	cv::Mat dMat1, dMat2;

	// TODO: Add your message handler code here and/or call default
	RECT r;
	CWnd* h = GetDlgItem(IDC_FORM_PICTCTRL_CBPREVIEW);

	h->GetWindowRect(&r); //get window rect of control relative to screen
	ScreenToClient(&r); //optional step - see below

	if ((point.y > r.top) &&
		(point.y < r.bottom) &&
		(point.x > r.left) &&
		(point.x < r.right))
	{
		//CBitmapDlg dlgBitmap;
		if ((m_TestCVMat.rows != 0) && (m_TestCVMat.cols != 0))
		{

			transpose(m_CVImagePrev, dMat1);
			flip(dMat1, dMat2, 1); //transpose+flip(1)=CW

			CImageToolDialog dlgBitmap;
			dlgBitmap.SetCVMat(dMat2);
			dlgBitmap.DoModal();
		}
	}

	CColorFormView::OnLButtonDblClk(nFlags, point);
}


BOOL CARALGISView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	cv::Mat dMat1, dMat2;

	WORD dControl = LOWORD(wParam);

	if (HIWORD(wParam) == STN_CLICKED)
	{
		int AA = 0;
	}
	else if (HIWORD(wParam) == STN_DBLCLK)
	{
		if (dControl == IDC_STATIC_TESTBMP)
		{
			if ((m_TestCVMat.rows != 0) && (m_TestCVMat.cols != 0))
			{
				if (g_CVTestBigWindow.rows != 0 || g_CVTestBigWindow.cols != 0)
				{
					g_CVTestBigWindow.release();
				}

				CImageToolDialog dlgBitmap;

				transpose(m_TestCVMat, dMat1);
				flip(dMat1, dMat2, 1); //transpose+flip(1)=CW
				g_CVTestBigWindow = dMat2.clone();

				dlgBitmap.SetCVMat(g_CVTestBigWindow);
				dlgBitmap.DoModal();
			}
		}
		else if (dControl == IDC_STATIC_REFBMP)
		{
			if ((m_RefCVMat.rows != 0) && (m_RefCVMat.cols != 0))
			{
				if (g_CVRefBigWindow.rows != 0 || g_CVRefBigWindow.cols != 0)
				{
					g_CVRefBigWindow.release();
				}
				CImageToolDialog dlgBitmap;

				transpose(m_RefCVMat, dMat1);
				flip(dMat1, dMat2, 1); //transpose+flip(1)=CW
				g_CVRefBigWindow = dMat2.clone();

				dlgBitmap.SetCVMat(g_CVRefBigWindow);
				dlgBitmap.DoModal();
			}
		}
	}

	return CColorFormView::OnCommand(wParam, lParam);
}
