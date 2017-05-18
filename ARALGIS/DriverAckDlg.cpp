// DriverAckDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ARALGIS.h"
#include "DriverAckDlg.h"
#include "afxdialogex.h"


// CDriverAckDlg dialog

IMPLEMENT_DYNAMIC(CDriverAckDlg, CDialogEx)

CDriverAckDlg::CDriverAckDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDriverAckDlg::IDD, pParent)
{

}

CDriverAckDlg::~CDriverAckDlg()
{
}

void CDriverAckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CDriverAckDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// don't use SetFocus; the following is better
	GotoDlgCtrl(GetDlgItem(IDCANCEL));
	// returning FALSE is crucial; with TRUE, the default button is used
	return FALSE;
}


BEGIN_MESSAGE_MAP(CDriverAckDlg, CDialogEx)
END_MESSAGE_MAP()


// CDriverAckDlg message handlers
