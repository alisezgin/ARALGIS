// BitmapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BitmapDlg.h"
#include "afxdialogex.h"


// CBitmapDlg dialog

IMPLEMENT_DYNAMIC(CBitmapDlg, CDialogEx)

CBitmapDlg::CBitmapDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBitmapDlg::IDD, pParent)
{
	m_ImageToDisplay = new CShowpicCV;
}

CBitmapDlg::~CBitmapDlg()
{
}

void CBitmapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBitmapDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CBitmapDlg::OnBnClickedButtonClose)
END_MESSAGE_MAP()


// CBitmapDlg message handlers


void CBitmapDlg::OnBnClickedButtonClose()
{
	// TODO: Add your control notification handler code here
	this->ShowWindow(SW_HIDE);

	TCHAR* str_winTitle;
	str_winTitle = new TCHAR[10];

	this->GetWindowText(str_winTitle, 10);

	LPARAM pLparam;
	pLparam = reinterpret_cast<LPARAM>(str_winTitle);

	// below must be send message
	// in sendmessage, message is immediately processed
	// so the pLparam, which is window title is still available
	// when DestroyWindow is called within SketcherDoc class
	AfxGetApp()->m_pMainWnd->SendMessage(WM_BIMPDLG_CLOSED, 0, pLparam);

	delete[] str_winTitle;
}


void CBitmapDlg::PostNcDestroy()
{
	CDialogEx::PostNcDestroy();

	AfxGetApp()->m_pMainWnd->PostMessage(WM_BIMPDLG_DESTROYED, 0, 0);

	delete m_ImageToDisplay;

	delete this;
}
