
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

#include "BitmapDlg.h"


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

	m_iDisplayTestImageFilterType = (int)0;
	m_iDisplayRefImageFilterType = (int)0;

	m_iDisplayTestImageColourType = (int)0;
	m_iDisplayRefImageColourType = (int)0;

	m_AutoFit = false;
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

	if (g_CVImageRef.rows != 0 || g_CVImageRef.cols != 0)
	{
		g_CVImageRef.release();
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
	
	if (g_CVTestChangeDetect.rows != 0 || g_CVTestChangeDetect.cols != 0)
	{
		g_CVTestChangeDetect.release();
	}
	
	if (g_CVTestWrapped.rows != 0 || g_CVTestWrapped.cols != 0)
	{
		g_CVTestWrapped.release();
	}

	for (int i= 0; i<=NUM_INTERMEDIATE_TEST_IMAGE; i++)
	{
			if (g_CVImageTestIntermediate[i].rows != 0 || g_CVImageTestIntermediate[i].cols != 0)
			{
				g_CVImageTestIntermediate[i].release();
			}
	}

	m_Font.DeleteObject();
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
	DDX_Control(pDX, IDC_STATIC_PLAKA_STR, m_PlakaDisplayControl);
	DDX_Control(pDX, IDC_RADIO_COLOUR1, m_ColourRadioOriginal);
	DDX_Control(pDX, IDC_RADIO1_ORIGFILTER, m_FilterRadioOriginal);
	DDX_Control(pDX, IDC_BUTTON_CHANGE_DETECT, m_ChangeDetectControl);
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

	//m_ButtonOrgImage.EnableWindow(FALSE);
	//m_ButtonFilter1.EnableWindow(FALSE);
	//m_ButtonFilter2.EnableWindow(FALSE);
	//m_ButtonFilter3.EnableWindow(FALSE); 

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


	if (g_dEndIndex - g_dBeginIndex > 0)
	{
		cv::Rect region_of_interest = cv::Rect(0, g_dBeginIndex, g_CVImageTest.cols, g_dEndIndex - g_dBeginIndex);

		dMat3 = g_CVImageTest(region_of_interest).clone();
	}

#ifdef DEBUG_BORA
	cv::imwrite("C:\\Users\\bora\\Desktop\\sil\\boraDeneme.bmp", dMat3);
#endif

	transpose(dMat3, dMat1);
	flip(dMat1, dMat2, 1); //transpose+flip(1)=CW
	//dMat2.copyTo(g_CVImageTest);
	g_CVImageTest = dMat2.clone();

	GetTestImageAsByte();

	if (m_MatToGDITest != NULL)
	{
		delete m_MatToGDITest;
		m_MatToGDITest = NULL;
	}

	if (m_TestCVMat.rows != 0 || m_TestCVMat.cols != 0)
	{
		m_TestCVMat.release();
	}

	m_TestCVMat = g_CVImageTest.clone();
	m_MatToGDITest = new PkMatToGDI(m_TestImgBMP, m_AutoFit);
	m_MatToGDITest->DrawImg(m_TestCVMat);

	//dMat1.release();
	//dMat2.release();
	//dMat3.release();

	// ali: update the database with the current image
	// ali: this has to go to the VehiclePassage table

	//// code to be deleted when dbase handler
	//// thread is added
	//// code deletion starts here
	//LPARAM pLparam;
	//pLparam = reinterpret_cast<LPARAM>("ARALGIS");
	//SendMessage(WM_DBASE_CAR_INFO_READY, 0, pLparam);
	//// code deletion ends here

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

		m_iDisplayTestImageFilterType = (int)0;
		m_iDisplayRefImageFilterType = (int)0;
		m_iDisplayRefImageColourType = (int)0;
		m_iDisplayTestImageColourType = (int)0;

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


	
	if (g_CVTestChangeDetect.rows != 0 || g_CVTestChangeDetect.cols != 0)
	{
		g_CVTestChangeDetect.release();
	}
	g_CVTestChangeDetect.create(g_CVImageTest.size(), g_CVImageTest.type());

	CButton* dRadioB;
	dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_BUTTON_CHANGE_DETECT));
	dRadioB->EnableWindow(FALSE);

	// get Singleton ChangeDetectionController and start the change detection process
	//CChangeDetectController::getInstance()->process(m_FilenameRef, m_FilenameTest);
	SetEvent(g_StartChangeDetectEvent);
	//CChangeDetectController::getInstance()->process(m_RefCVMat, m_TestCVMat);

	return 0;
}


afx_msg LRESULT CARALGISView::OnDBaseCarInfoReady(WPARAM wParam, LPARAM lParam)
{
	//// temporary modification to the g_ReferenceFilePath
	//// IMPORTANT !!! 
	//char strRefFilePathTemp[] = "C:\\Users\\bora\\Desktop\\ARALGIS-GitHub\\Cars\\ChassisBottom\\";
	//strncpy_s(g_ReferenceFilePath, (size_t)(MAX_FILENAME_LENGTH + 1), strRefFilePathTemp, (size_t)(MAX_FILENAME_LENGTH));
	//// IMPORTANT !!!

	//strncpy_s(g_RefImageFileName, (size_t)(MAX_FILENAME_LENGTH + 1), g_ReferenceFilePath, (size_t)(MAX_FILENAME_LENGTH));
	//strncat_s(g_RefImageFileName, (size_t)(MAX_FILENAME_LENGTH + 1), "arac1-colour.bmp", (size_t)(MAX_FILENAME_LENGTH));


	//g_CVImageRef = cv::imread(g_RefImageFileName, cv::IMREAD_COLOR);

	////g_CVImageRef = cv::imread("C:/Users/bora/Desktop/FUZYON-SW-Dev/SW-Projects/uvss-images/new/1600/car-1-handCropped.bmp", cv::IMREAD_COLOR);

	//// no need to transpose+flip
	//// since image files are written landscape!!!
	//// for the time being for testing purposes
	//// these operations are needed.
	//// delete below code for real system
	//// code deletion starts here
	////cv::Mat dMat1, dMat2;
	////transpose(g_CVImageRef, dMat1);
	////flip(dMat1, dMat2, 1); //transpose+flip(1)=CW
	////dMat2.copyTo(g_CVImageRef);
	//// code deletion ends here


	//if (m_MatToGDIRef != NULL)
	//{
	//	delete m_MatToGDIRef;
	//	m_MatToGDIRef = NULL;
	//}

	//m_MatToGDIRef = new PkMatToGDI(m_RefImgBMP, m_AutoFit);
	//m_MatToGDIRef->DrawImg(g_CVImageRef);


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

	OnLPUpdateInfo(aCString);

	m_PlakaDisplayControl.SetWindowTextW(aCString);

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

	dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_COLOUR2));
	dRadioB->SetCheck(0);
	dRadioB->EnableWindow(FALSE);

	dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_COLOUR3));
	dRadioB->EnableWindow(FALSE);
	dRadioB->SetCheck(0);

	SetEvent(g_ProcessFilter4Event);
	SetEvent(g_ProcessFilter5Event);
	SetEvent(g_ProcessFilter6Event);

	//CFont font;
	//VERIFY(font.CreateFont(
	//	40,                        // nHeight
	//	20,                         // nWidth
	//	0,                         // nEscapement
	//	0,                         // nOrientation
	//	FW_NORMAL,                 // nWeight
	//	FALSE,                     // bItalic
	//	FALSE,                     // bUnderline
	//	0,                         // cStrikeOut
	//	ANSI_CHARSET,              // nCharSet
	//	OUT_DEFAULT_PRECIS,        // nOutPrecision
	//	CLIP_DEFAULT_PRECIS,       // nClipPrecision
	//	DEFAULT_QUALITY,           // nQuality
	//	DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
	//	_T("Arial")));                 // lpszFacename



	//// Done with the font.  Delete the font object.

	//RECT r;
	//CWnd* h = GetDlgItem(IDC_STATIC_PLAKA_STR);
	//h->GetWindowRect(&r); //get window rect of control relative to screen
	//POINT pt = { r.left, r.top }; //new point object using rect x, y
	//ScreenToClient(&pt); //convert screen co-ords to client based points

	//// Do something with the font just created...
	//CClientDC dc(this);
	//CFont* def_font = dc.SelectObject(&font);
	//dc.TextOut(pt.x, pt.y, bCString, 9);
	//dc.TextOut(pt.x, pt.y, aCString, 8);
	//dc.SelectObject(def_font);

	//font.DeleteObject();

	return 0;
}

afx_msg LRESULT CARALGISView::DisplayPTSImage(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your command handler code here
	m_CarPlakaImageStatic.Load(g_CarPlakaImage, (size_t)g_CarPlakaImageLenght);

	SetTimerDisplay();
	return 0;
}

void CARALGISView::DeletePTSImage()
{
	// TODO: Add your command handler code here
	//m_CarPlakaImageStatic.FreeData();
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


void CARALGISView::SetTimerDisplay()
{
	// added by bora, start 1 second interval timer
	m_TimerSecondCounter = 0;
	KillTimer(DISPLAY_TIMER_ID);
	SetTimer(DISPLAY_TIMER_ID, TIMER_PERIOD_IN_MS, NULL);
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
		m_MatToGDITest = new PkMatToGDI(m_TestImgBMP, m_AutoFit);

		if (g_CVImageTest.rows != 0)
		{
			m_MatToGDITest->DrawImg(m_TestCVMat);
		}
	}

	if (m_MatToGDIRef != NULL)
	{
		delete m_MatToGDIRef;
		m_MatToGDIRef = NULL;

		m_MatToGDIRef = new PkMatToGDI(m_RefImgBMP, m_AutoFit);

		if (g_CVImageRef.rows != 0)
		{
			m_MatToGDIRef->DrawImg(m_RefCVMat);

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

	CString fullPathName = CString{ _T("C:\\Users\\bora\\Desktop\\ARALGIS-GitHub-HTR\\Cars\\ChassisBottom\\") } +strRefFilename;

	
	const size_t newsizew = (fullPathName.GetLength() + 1) * 2;
	char *charfilename = new char[newsizew];
	size_t convertedCharsw = 0;
	wcstombs_s(&convertedCharsw, charfilename, newsizew, fullPathName, newsizew);
	
	// TODO: display the contents of strRefFilename
	// MessageBox(CString{ _T("The filename for Reference image: ") }+strRefFilename);

	if (g_CVImageRef.rows != 0 || g_CVImageRef.cols != 0)
	{
		g_CVImageRef.release();
	}

	cv::Mat pTemp;

	if (g_CameraPixelBits == 24)
	{
		g_CVImageRef = cv::imread(charfilename, cv::IMREAD_COLOR);
	}
	else
	{
		g_CVImageRef = cv::imread(charfilename, cv::IMREAD_GRAYSCALE);
	}

	//g_CVImageRef.create(pTemp.size(), pTemp.type());
	
	if (m_MatToGDIRef != nullptr)
	{
		delete m_MatToGDIRef;
		m_MatToGDIRef = nullptr;
	}
	m_RefCVMat = g_CVImageRef.clone();
	m_MatToGDIRef = new PkMatToGDI(m_RefImgBMP, m_AutoFit);

	EnterCriticalSection(&RefImageCS);
	m_MatToGDIRef->DrawImg(g_CVImageRef);
	LeaveCriticalSection(&RefImageCS);

	m_Ref1FilterOK = FALSE;
	m_Ref2FilterOK = FALSE;
	m_Ref3FilterOK = FALSE;

	m_ColourRadioOriginal.EnableWindow(FALSE);
	m_ColourRadioOriginal.SetCheck(1);

	m_iDisplayTestImageFilterType = (int)0;
	m_iDisplayRefImageFilterType = (int)0;
	m_iDisplayRefImageColourType = (int)0;
	m_iDisplayTestImageColourType = (int)0;

	CButton* dRadioB;
	dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_COLOUR2));
	dRadioB->SetCheck(0);
	dRadioB->EnableWindow(FALSE);

	dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_COLOUR3));
	dRadioB->EnableWindow(FALSE);
	dRadioB->SetCheck(0);
	
	UpdateData(FALSE);
	// m_FormListBox.AddString(strLP);

	delete[] charfilename;
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



void CARALGISView::OnBnClickedRadioColour1()
{
	// TODO: Add your control notification handler code here
	m_iDisplayTestImageColourType = (int)0;

	if (m_iDisplayTestImageFilterType == (int)0)
	{
		//g_CVImageTest.copyTo(m_TestCVMat);
		m_TestCVMat = g_CVImageTest.clone();
	}
	else if (m_iDisplayTestImageFilterType == (int)1)
	{
		//g_CVImageTestFilter1.copyTo(m_TestCVMat);
		m_TestCVMat = g_CVImageTestFilter1.clone();
	}
	else if (m_iDisplayTestImageFilterType == (int)2)
	{
		//g_CVImageTestFilter2.copyTo(m_TestCVMat);
		m_TestCVMat = g_CVImageTestFilter2.clone();
	}
	else if (m_iDisplayTestImageFilterType == (int)3)
	{
		//g_CVImageTestFilter3.copyTo(m_TestCVMat);
		m_TestCVMat = g_CVImageTestFilter3.clone();
	}

	m_MatToGDITest->DrawImg(m_TestCVMat);
///////////////////////////////////////////////////////
	m_iDisplayRefImageColourType = (int)0;

	if (m_iDisplayRefImageFilterType == (int)0)
	{
		//g_CVImageRef.copyTo(m_RefCVMat);
		m_RefCVMat = g_CVImageRef.clone();
	}
	else if (m_iDisplayRefImageFilterType == (int)1)
	{
		//g_CVImageRefFilter1.copyTo(m_RefCVMat);
		m_RefCVMat = g_CVImageRefFilter1.clone();
	}
	else if (m_iDisplayRefImageFilterType == (int)2)
	{
		//g_CVImageRefFilter2.copyTo(m_RefCVMat);
		m_RefCVMat = g_CVImageRefFilter2.clone();
	}
	else if (m_iDisplayRefImageFilterType == (int)3)
	{
		//g_CVImageRefFilter3.copyTo(m_RefCVMat);
		m_RefCVMat = g_CVImageRefFilter3.clone();
	}

	m_MatToGDIRef->DrawImg(m_RefCVMat);

}


void CARALGISView::OnBnClickedRadioColour2()
{
	// TODO: Add your control notification handler code here
	cv::Mat dTempCV;
	m_iDisplayTestImageColourType = (int)1;

	if (m_iDisplayTestImageFilterType == (int)0)
	{
		m_TestCVMat = g_CVImageTest.clone();
		cv::applyColorMap(g_CVImageTest, dTempCV, cv::COLORMAP_RAINBOW);
	}
	else if (m_iDisplayTestImageFilterType == (int)1)
	{
		m_TestCVMat = g_CVImageTestFilter1.clone();
		cv::applyColorMap(g_CVImageTestFilter1, dTempCV, cv::COLORMAP_RAINBOW);
	}
	else if (m_iDisplayTestImageFilterType == (int)2)
	{
		m_TestCVMat = g_CVImageTestFilter2.clone();
		cv::applyColorMap(g_CVImageTestFilter2, dTempCV, cv::COLORMAP_RAINBOW);
	}
	else if (m_iDisplayTestImageFilterType == (int)3)
	{
		m_TestCVMat = g_CVImageTestFilter3.clone();
		cv::applyColorMap(g_CVImageTestFilter3, dTempCV, cv::COLORMAP_RAINBOW);
	}

	//dTempCV.copyTo(m_TestCVMat);
	m_TestCVMat = dTempCV.clone();

	m_MatToGDITest->DrawImg(m_TestCVMat);

	////////////////////////////////////////////////////////

	m_iDisplayRefImageColourType = (int)1;

	if (m_iDisplayRefImageFilterType == (int)0)
	{
		m_RefCVMat = g_CVImageRef.clone();
		cv::applyColorMap(g_CVImageRef, dTempCV, cv::COLORMAP_RAINBOW);
	}
	else if (m_iDisplayRefImageFilterType == (int)1)
	{
		m_RefCVMat = g_CVImageRefFilter1.clone();
		cv::applyColorMap(g_CVImageRefFilter1, dTempCV, cv::COLORMAP_RAINBOW);
	}
	else if (m_iDisplayRefImageFilterType == (int)2)
	{
		m_RefCVMat = g_CVImageRefFilter2.clone();
		cv::applyColorMap(g_CVImageRefFilter2, dTempCV, cv::COLORMAP_RAINBOW);
	}
	else if (m_iDisplayRefImageFilterType == (int)3)
	{
		m_RefCVMat = g_CVImageRefFilter3.clone();
		cv::applyColorMap(g_CVImageRefFilter3, dTempCV, cv::COLORMAP_RAINBOW);
	}

	m_RefCVMat = dTempCV.clone();

	m_MatToGDIRef->DrawImg(m_RefCVMat);

}


void CARALGISView::OnBnClickedRadioColour3()
{
	// TODO: Add your control notification handler code here
	cv::Mat dTempCV;
	m_iDisplayTestImageColourType = (int)2;


	if (m_iDisplayTestImageFilterType == (int)0)
	{
		m_TestCVMat = g_CVImageTest.clone();
		cv::applyColorMap(g_CVImageTest, dTempCV, cv::COLORMAP_JET);
	}
	else if (m_iDisplayTestImageFilterType == (int)1)
	{
		m_TestCVMat = g_CVImageTestFilter1.clone();
		cv::applyColorMap(g_CVImageTestFilter1, dTempCV, cv::COLORMAP_JET);
	}
	else if (m_iDisplayTestImageFilterType == (int)2)
	{
		m_TestCVMat = g_CVImageTestFilter2.clone();
		cv::applyColorMap(g_CVImageTestFilter2, dTempCV, cv::COLORMAP_JET);
	}
	else if (m_iDisplayTestImageFilterType == (int)3)
	{
		m_TestCVMat = g_CVImageTestFilter3.clone();
		cv::applyColorMap(g_CVImageTestFilter3, dTempCV, cv::COLORMAP_JET);
	}

	//dTempCV.copyTo(m_TestCVMat);
	m_TestCVMat = dTempCV.clone();

	m_MatToGDITest->DrawImg(m_TestCVMat);

	////////////////////////////////////////////////////////

	m_iDisplayRefImageColourType = (int)2;

	if (m_iDisplayRefImageFilterType == (int)0)
	{
		m_RefCVMat = g_CVImageRef.clone();
		cv::applyColorMap(g_CVImageRef, dTempCV, cv::COLORMAP_JET);
	}
	else if (m_iDisplayRefImageFilterType == (int)1)
	{
		m_RefCVMat = g_CVImageRefFilter1.clone();
		cv::applyColorMap(g_CVImageRefFilter1, dTempCV, cv::COLORMAP_JET);
	}
	else if (m_iDisplayRefImageFilterType == (int)2)
	{
		m_RefCVMat = g_CVImageRefFilter2.clone();
		cv::applyColorMap(g_CVImageRefFilter2, dTempCV, cv::COLORMAP_JET);
	}
	else if (m_iDisplayRefImageFilterType == (int)3)
	{
		m_RefCVMat = g_CVImageRefFilter3.clone();
		cv::applyColorMap(g_CVImageRefFilter3, dTempCV, cv::COLORMAP_JET);
	}

	m_RefCVMat = dTempCV.clone();

	m_MatToGDIRef->DrawImg(m_RefCVMat);
}


void CARALGISView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	RECT r;
	CWnd* h = GetDlgItem(IDC_BMP_TEST);
	h->GetWindowRect(&r); //get window rect of control relative to screen
	ScreenToClient(&r); //optional step - see below

	if ((point.y > r.top) &&
		(point.y < r.bottom) &&
		(point.x > r.left) &&
		(point.x < r.right))
	{
		CBitmapDlg dlgBitmap;
		dlgBitmap.SetCVMat(m_TestCVMat);
		dlgBitmap.DoModal();
	}
	else
	{
		h = GetDlgItem(IDC_BMP_REFERENCE);
		h->GetWindowRect(&r); //get window rect of control relative to screen
		ScreenToClient(&r); //optional step - see below

		if ((point.y > r.top) &&
			(point.y < r.bottom) &&
			(point.x > r.left) &&
			(point.x < r.right))
		{
			CBitmapDlg dlgBitmap;
			dlgBitmap.SetCVMat(m_RefCVMat);
			dlgBitmap.DoModal();
		}
	}


	CColorFormView::OnLButtonDblClk(nFlags, point);
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

	transpose(g_CVTestWrapped, AA1);
	flip(AA1, AA2, 1); //transpose+flip(1)=CW
	AA2.copyTo(AA3);

	transpose(g_CVTestChangeDetect, AA4);
	flip(AA4, AA5, 1); //transpose+flip(1)=CW
	AA5.copyTo(AA6);

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

	CBitmapDlg dlgBitmap;
	dlgBitmap.SetCVMat(dMat4);
	dlgBitmap.DoModal();
}

//void CARALGISView::OnBnClickedButtonChangeDetect()
//{
//	// TODO: Add your control notification handler code here
//	cv::Mat dMat1, dMat2, dMat3, dMat4;
//
//	cv::Mat AA1, AA2, AA3, AA4, AA5, AA6;
//	cv::Mat BB1, BB2, BB3;
//
//	transpose(g_CVTestWrapped, BB1);
//	flip(BB1, BB2, 1); 
//	BB2.copyTo(BB3);
//
//	cv::resize(g_CVImageTest, AA3, BB3.size(), (0, 0), (0, 0), cv::INTER_CUBIC);
//
//	cv::transpose(g_CVTestChangeDetect, AA4);
//	cv::flip(AA4, AA5, 1); 
//	AA5.copyTo(AA6);
//
//	// 1. create wrapped image first as grey since it is calculated with false colour
//	// 2. change it to colour (it is grey in fact) so that it has 3 channels again
//	// it will be ready for blending  
//	//cvtColor(AA3, dMat3, CV_BGR2GRAY);
//
//	cvtColor(AA3, dMat1, CV_GRAY2BGR);
//	////// end of wrapped image preparation
//
//	// create detected changes as 
//	// red colour image with 3 channels
//	dMat2.create(AA6.size(), CV_8UC3);
//
//	std::vector<cv::Mat> bgr_planes(3);
//	cv::split(dMat2, bgr_planes);
//
//	bgr_planes[2] = AA6.clone(); // 2 is red channel
//	cv::merge(bgr_planes, dMat2);
//	// red colour image creation ends
//
//	float fBlend = 0.75;
//	dMat4 = fBlend*dMat1 + (1.0 - fBlend)*dMat2;
//
//
//	cv::namedWindow("g_CVTestWrapped", cv::WINDOW_NORMAL);
//	cv::imshow("g_CVTestWrapped", AA3);
//	cv::waitKey(0);
//
//	//cv::namedWindow("dMat3", cv::WINDOW_NORMAL);
//	//cv::imshow("dMat3", dMat3);
//	//cv::waitKey(0);
//
//	cv::namedWindow("dMat1", cv::WINDOW_NORMAL);
//	cv::imshow("dMat1", dMat1);
//	cv::waitKey(0);
//
//	cv::namedWindow("g_CVTestChangeDetect", cv::WINDOW_NORMAL);
//	cv::imshow("g_CVTestChangeDetect", AA6);
//	cv::waitKey(0);
//
//	cv::namedWindow("dMat2", cv::WINDOW_NORMAL);
//	cv::imshow("dMat2", dMat2);
//	cv::waitKey(0);
//
//	cv::namedWindow("dMat4", cv::WINDOW_NORMAL);
//	cv::imshow("dMat4", dMat4);
//	cv::waitKey(0);
//
//	CBitmapDlg dlgBitmap;
//	dlgBitmap.SetCVMat(dMat4);
//	dlgBitmap.DoModal();
//}