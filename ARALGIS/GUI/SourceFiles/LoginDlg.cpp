// C:\ali\github-home\ARALGIS\ARALGIS\GUI\SourceFiles\LoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ARALGIS.h"
#include "LoginDlg.h"
#include "afxdialogex.h"


// CLoginDlg dialog

IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLoginDlg::IDD, pParent)
	, m_strUsername(_T(""))
	, m_strPassword(_T(""))
{
}

CLoginDlg::~CLoginDlg()
{
}

CString CLoginDlg::getUsername()
{
	return m_strUsername;
}

CString CLoginDlg::getPassword()
{
	return m_strPassword;
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DLG_C_LOGIN_USERNAME, m_Username);
	DDX_Control(pDX, IDC_DLG_C_LOGIN_PASSWORD, m_Password);
}



void CLoginDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	m_Username.GetWindowTextW(m_strUsername);
	m_Password.GetWindowTextW(m_strPassword);

	CDialogEx::OnOK();
}

BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CLoginDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CLoginDlg message handlers
