// BitmapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ARALGIS.h"
#include "BitmapDlg.h"
#include "afxdialogex.h"


// CBitmapDlg dialog

IMPLEMENT_DYNAMIC(CBitmapDlg, CDialogEx)

CBitmapDlg::CBitmapDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBitmapDlg::IDD, pParent)
{
	//CBmpDisplayer = new CStatic;
	m_MatToGDI = NULL;
}

CBitmapDlg::~CBitmapDlg()
{
	if (m_MatToGDI)
	{
		delete m_MatToGDI;
	}

	//if (CBmpDisplayer)
	//{
	//	delete CBmpDisplayer;
	//}
}

void CBitmapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BITMAP, CBmpDisplayer);
}


BEGIN_MESSAGE_MAP(CBitmapDlg, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CBitmapDlg message handlers

void CBitmapDlg::SetCVMat(cv::Mat& aInMat)
{
	//aInMat.copyTo(m_cvMat);
	m_cvMat = aInMat.clone();
}


void CBitmapDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialogEx::OnPaint() for painting messages

	if (m_cvMat.rows != 0)
	{
		if (m_MatToGDI != NULL)
		{
			delete m_MatToGDI;

		}
		m_MatToGDI = new PkMatToGDI(&CBmpDisplayer, false);
		m_MatToGDI->DrawImg(m_cvMat);
	}
}


BOOL CBitmapDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	BOOL bOk = FALSE;
	bOk = m_resizer.Hook(this);
	ASSERT(bOk);

	//CUIntArray arrID;
	//arrID.Add(IDC_BITMAP);

	//bOk = m_resizer.CreatePanel(_T("BMP_Panel"), &arrID, TRUE);
	//ASSERT(bOk);

	bOk = m_resizer.SetAnchor(IDC_BITMAP, ANCHOR_ALL);
	ASSERT(bOk);

	bOk = m_resizer.SetAnchor(IDCANCEL, ANCHOR_BOTTOM | ANCHOR_HORIZONTALLY_CENTERED);
	ASSERT(bOk);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
