// ImageToolDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ARALGIS.h"
#include "ImageToolDialog.h"
#include "afxdialogex.h"


// CImageToolDialog dialog

IMPLEMENT_DYNAMIC(CImageToolDialog, CResizableDlg)

CImageToolDialog::CImageToolDialog(CWnd* pParent /*=NULL*/)
	//: CResizableDlg(CImageToolDialog::IDD, 400, 490, pParent), m_pRes(NULL), m_isOnEnChangeEditWidthHeight(true)
	: CResizableDlg(CImageToolDialog::IDD, 400, 490, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME); 
	//m_rl.setResourceType(CResourceList::resourceType::RESOURCE_IMAGE); 
	// m_rl.setResourceType((UINT)RT_GROUP_ICON);

	m_image.setPanMode(TRUE);
	m_image.setZoomMode(TRUE);
	m_image.setSizeType(1);
	m_image.setMaintainAspectRatio(false);
}


CImageToolDialog::~CImageToolDialog()
{
}

void CImageToolDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_IMAGE, m_image);
	DDX_Control(pDX, IDC_RADIO_ORG, m_RealSizeButton);
}


BEGIN_MESSAGE_MAP(CImageToolDialog, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO_ORG, &CImageToolDialog::OnBnClickedRadioOrg)
	ON_BN_CLICKED(IDC_RADIO_ZOOMFIT, &CImageToolDialog::OnBnClickedRadioZoomfit)
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_MESSAGE(WM_LMOUSE_DOWN, &CImageToolDialog::OnLButtonDownImage)
	ON_MESSAGE(WM_LMOUSE_UP, &CImageToolDialog::OnLButtonUpImage)
	ON_MESSAGE(WM_LMOUSE_MOVE, &CImageToolDialog::OnMouseMoveImage)
END_MESSAGE_MAP()


// CImageToolDialog message handlers


void CImageToolDialog::OnBnClickedRadioOrg()
{
	m_image.setSizeType(0);
	m_image.update();
}


void CImageToolDialog::OnBnClickedRadioZoomfit()
{
	CRect rect;

	m_image.setSizeType(1);
	m_image.update();

	if (!m_image.isImageShown())
	{
		GetDlgItem(IDC_STATIC_IMAGE)->GetClientRect(rect); 
		m_image.setWidth(rect.Width()); 
		m_image.setHeight(rect.Height());
	}
}


BOOL CImageToolDialog::OnInitDialog()
{
	moveResizeControl(IDC_STATIC_IMAGE, tControl::anchor::TOP_LEFT, 0, 0, 1, 1);
	moveResizeControl(IDC_RADIO_ORG, tControl::anchor::TOP_LEFT, 0, 1, 0, 0);
	moveResizeControl(IDC_RADIO_ZOOMFIT, tControl::anchor::TOP_LEFT, 0, 1, 0, 0);
	moveResizeControl(IDC_STATIC, tControl::anchor::TOP_LEFT, 0, 1, 0, 0, true);
	moveResizeControl(IDCANCEL, tControl::anchor::TOP_LEFT, 1, 1, 0, 0);

	CButton* dRadioB;
	dRadioB = reinterpret_cast<CButton*>(GetDlgItem(IDC_RADIO_ZOOMFIT));
	dRadioB->SetCheck(1);

	CResizableDlg::OnInitDialog();

	SetIcon(m_hIcon, TRUE); SetIcon(m_hIcon, FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CImageToolDialog::OnSize(UINT nType, int cx, int cy)
{
	CResizableDlg::OnSize(nType, cx, cy);

	if (m_image.isImageShown())
	{
		m_image.update();
	}
}


void CImageToolDialog::SetCVMat(cv::Mat& aInMat)
{
	m_image.load2(aInMat);

	//CString szFilePath = CString(_T("C:\\Users\\bora\\Desktop\\ARALGIS-GitHub\\ARALGIS\\Cars\\ChassisBottom\\06zdk89_ref.jpg"));
	//m_image.load(szFilePath);
}

BOOL CImageToolDialog::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	TRACE("CImageToolDialog::OnMouseWheel\n");

	m_image.bMouseWheel(nFlags, zDelta, pt);

	return CResizableDlg::OnMouseWheel(nFlags, zDelta, pt);
}

afx_msg LRESULT CImageToolDialog::OnLButtonDownImage(WPARAM wparam, LPARAM lparam)
{
	TRACE("CImageToolDialog::OnLButtonDownImage\n");

	CRect rect;
	GetDlgItem(IDC_STATIC_IMAGE)->GetWindowRect(&rect);

	InvalidateRect(rect, FALSE);
	UpdateWindow();
	return 1;
}

afx_msg LRESULT CImageToolDialog::OnLButtonUpImage(WPARAM wparam, LPARAM lparam)
{
	TRACE("CImageToolDialog::OnLButtonDownImage\n");
	CRect rect;
	GetDlgItem(IDC_STATIC_IMAGE)->GetWindowRect(&rect);
	InvalidateRect(rect, FALSE);

	UpdateWindow();
	return 1;
}


afx_msg LRESULT CImageToolDialog::OnMouseMoveImage(WPARAM wparam, LPARAM lparam)
{
	TRACE("CImageToolDialog::OnMouseMoveImage\n");
	CRect rect;
	GetDlgItem(IDC_STATIC_IMAGE)->GetWindowRect(&rect);
	InvalidateRect(rect, FALSE);
	UpdateWindow();
	return 1;
}

