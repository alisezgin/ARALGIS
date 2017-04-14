
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
	ON_COMMAND(ID_AYD32778, &CARALGISView::OnHistoEqualize)
	ON_COMMAND(ID_KAMERA_ALANSE32779, &CARALGISView::OnSelectArea)
	ON_COMMAND(ID_RESIMSE32781, &CARALGISView::OnSelectOriginal)
	ON_COMMAND(ID_RESIMSE32782, &CARALGISView::OnSelectFiltered)
	ON_COMMAND(ID_RESIMSE32799, &CARALGISView::OnImageSelectArea)
	ON_COMMAND(ID_RESMID32801, &CARALGISView::OnRotate90CW)
	ON_COMMAND(ID_RESMID32802, &CARALGISView::OnRotate90CCW)
	ON_COMMAND(ID_RESMID32803, &CARALGISView::OnRotate180)
	ON_BN_CLICKED(IDC_BUTTON_BARRIER_OPEN, &CARALGISView::OnBnClickedButtonBarrierOpen)
	ON_BN_CLICKED(IDC_BUTTON_BARRIER_CLOSE, &CARALGISView::OnBnClickedButtonBarrierClose)
	ON_BN_CLICKED(IDC_BUTTON_HEATER_ON, &CARALGISView::OnBnClickedButtonHeaterOn)
	ON_BN_CLICKED(IDC_BUTTON_HEATER_OFF, &CARALGISView::OnBnClickedButtonHeaterOff)
	ON_BN_CLICKED(IDC_BUTTON_ALARM_ON, &CARALGISView::OnBnClickedButtonAlarmOn)
	ON_BN_CLICKED(IDC_BUTTON_ALARM_OFF, &CARALGISView::OnBnClickedButtonAlarmOff)
END_MESSAGE_MAP()

// CARALGISView construction/destruction

CARALGISView::CARALGISView() : CColorFormView(CARALGISView::IDD)
, m_PlakaStr(_T(""))
{
	// TODO: add construction code here
	for (int i = 0; i < MAX_BUFFER; i++)
	{
		m_selList[i] = TRUE;
	}

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

	cv::namedWindow("GörüntüXX", cv::WINDOW_NORMAL);
	cv::imshow("GörüntüXX", g_CVImage);
	cv::waitKey(1500);

	// get Singleton ChangeDetectionController and start the change detection process
	CChangeDetectController::getInstance()->process(m_FilenameRef, m_FilenameTest);

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


void CARALGISView::OnCameraStartStop()
{
	CCameraStartStop dDlg;

	dDlg.DoModal();
}

////////////////// histogram equalization begins //////////////////////////
void CARALGISView::OnHistoEqualize()
{
	cv::Mat image_eq;

	image_eq.create(g_CVImage.rows, g_CVImage.cols, CV_8UC3);

	//separate channels, equalize histograms and them merge them
	std::vector<cv::Mat> channels, channels_eq;
	cv::split(g_CVImage, channels);
	for (int i = 0; i < channels.size(); i++)
	{
		cv::Mat eq;
		cv::equalizeHist(channels[i], eq);
		channels_eq.push_back(eq);
	}

	cv::merge(channels_eq, image_eq);

	/// bora Sil !!!!!
	image_eq.copyTo(m_CVImageHistoCrop);

	cv::namedWindow("Aydinlik", cv::WINDOW_NORMAL);
	cv::imshow("Aydinlik", image_eq);
	cv::waitKey(1500);
}
////////////////// histogram equalization ends //////////////////////////

////////////////// image cropping (Area Selection) begins //////////////////////////
// Global variables
// Flags updated according to left mouse button activity
bool ldown = false, lup = false;
// Original image
cv::Mat img;
cv::Mat imgToCopy;

// Starting and ending points of the user's selection
cv::Point corner1, corner2;
// ROI
cv::Rect box;
// Callback function for mouse events
static void mouse_callback(int event, int x, int y, int d, void* ptr)
{
	// When the left mouse button is pressed, 
	// record its position and save it in corner1
	if (event == cv::EVENT_LBUTTONDOWN)
	{
		ldown = true;
		corner1.x = x;
		corner1.y = y;
		//std::cout << "Corner 1 recorded at " << corner1 << std::endl;
	}

	// When the left mouse button is released, 
	//record its position and save it in corner2
	if (event == cv::EVENT_LBUTTONUP)
	{
		// Also check if user selection is bigger than 20 pixels (jut for fun!)
		if (abs(x - corner1.x) > 20 && abs(y - corner1.y) > 20)
		{
			lup = true;
			corner2.x = x;
			corner2.y = y;
			//std::cout << "Corner 2 recorded at " << corner2 << std::endl << std::endl;
		}
		else
		{
			//std::cout << "Please select a bigger region" << std::endl;
			ldown = false;
		}
	}

	// Update the box showing the selected region 
	// as the user drags the mouse
	if (ldown == true && lup == false)
	{
		cv::Point pt;
		pt.x = x;
		pt.y = y;
		cv::Mat local_img = img.clone();
		cv::rectangle(local_img, corner1, pt, cv::Scalar(0, 0, 255));
		cv::imshow("Alan Secme", local_img);
	}


	// Define ROI and crop it out when both corners have been selected
	if (ldown == true && lup == true)
	{
		box.width = abs(corner1.x - corner2.x);
		box.height = abs(corner1.y - corner2.y);
		box.x = cv::min(corner1.x, corner2.x);
		box.y = cv::min(corner1.y, corner2.y);
		// Make an image out of just the selected ROI and display it in a new window
		//cv::Mat crop(img, box);
		cv::Mat crop(imgToCopy, box);

		cv::Mat* m_BN = (cv::Mat*) ptr;
		crop.copyTo(*m_BN);

		cv::namedWindow("Secili Alan", cv::WINDOW_NORMAL);
		cv::imshow("Secili Alan", crop);


		//imwrite("C:\\SW-Projects\\uvss-images\\new\\2500\\car-1000-handCropped.jpg", crop);

		ldown = false;
		lup = false;
	}
}

bool isImageCropped = false;

void CARALGISView::OnSelectArea()
{
	// TODO: Add your command handler code here
	isImageCropped = false;

	g_CVImage.copyTo(imgToCopy);

	g_CVImage.copyTo(img);

	ldown = false;
	lup = false;

	cv::namedWindow("Alan Secme", cv::WINDOW_NORMAL);
	cv::imshow("Alan Secme", g_CVImage);
	cv::waitKey(1500);

	// Set the mouse event callback function
	cv::setMouseCallback("Alan Secme", mouse_callback, &m_CVCroppedImage);
	// Exit by pressing 'q'
	//cv::waitKey(1500);

	m_BN.copyTo(m_CVCroppedImage);

	isImageCropped = true;
}
////////////////// image cropping ends //////////////////////////



void CARALGISView::OnSelectOriginal()
{
	// TODO: Add your command handler code here
}


void CARALGISView::OnSelectFiltered()
{
	m_CVImageHistoCrop.copyTo(g_CVImage);
}


void CARALGISView::OnImageSelectArea()
{
	// TODO: Add your command handler code here
	if (isImageCropped == true)
	{
		m_CVCroppedImage.copyTo(g_CVImage);
	}
}



// rotate 90 degrees CW, CCW or 180
void CARALGISView::Rotate90(cv::Mat &matImage, int rotflag)
{
	//1=CW, 2=CCW, 3=180
	if (rotflag == 1)
	{
		transpose(matImage, matImage);
		flip(matImage, matImage, 1); //transpose+flip(1)=CW
	}
	else if (rotflag == 2)
	{
		transpose(matImage, matImage);
		flip(matImage, matImage, 0); //transpose+flip(0)=CCW     
	}
	else if (rotflag == 3)
	{
		flip(matImage, matImage, -1);    //flip(-1)=180          
	}
	else if (rotflag != 0)
	{ //if not 0,1,2,3:
		::MessageBox(NULL, _T("Rotation Error"), NULL, MB_OK);
	}
}

void CARALGISView::OnRotate90CW()
{
	// TODO: Add your control notification handler code here
	cv::Mat rotImg;
	g_CVImage.copyTo(rotImg);
	Rotate90(rotImg, 1);

	cv::namedWindow("Rotated 90 CW ", cv::WINDOW_NORMAL);
	imshow("Rotated 90 CW ", rotImg);
	cv::waitKey(1500);
}


void CARALGISView::OnRotate90CCW()
{
	// TODO: Add your command handler code here
	cv::Mat rotImg;
	g_CVImage.copyTo(rotImg);
	Rotate90(rotImg, 2);

	cv::namedWindow("Rotated 90 CCW", cv::WINDOW_NORMAL);
	imshow("Rotated 90 CCW", rotImg);
	cv::waitKey(1500);
}


void CARALGISView::OnRotate180()
{
	// TODO: Add your command handler code here
	cv::Mat rotImg;
	g_CVImage.copyTo(rotImg);
	Rotate90(rotImg, 3);

	cv::namedWindow("Rotated 180", cv::WINDOW_NORMAL);
	imshow("Rotated 180", rotImg);
	cv::waitKey(1500);
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
}
