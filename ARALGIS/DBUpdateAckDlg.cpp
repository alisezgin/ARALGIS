// DBUpdateAckDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ARALGIS.h"
#include "DBUpdateAckDlg.h"
#include "afxdialogex.h"


// CDBUpdateAckDlg dialog

IMPLEMENT_DYNAMIC(CDBUpdateAckDlg, CDialogEx)

CDBUpdateAckDlg::CDBUpdateAckDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDBUpdateAckDlg::IDD, pParent)
{

}

CDBUpdateAckDlg::~CDBUpdateAckDlg()
{
}

void CDBUpdateAckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CDBUpdateAckDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// don't use SetFocus; the following is better
	GotoDlgCtrl(GetDlgItem(IDCANCEL));
	// returning FALSE is crucial; with TRUE, the default button is used
	return FALSE;
}


BEGIN_MESSAGE_MAP(CDBUpdateAckDlg, CDialogEx)
END_MESSAGE_MAP()


// CDBUpdateAckDlg message handlers
