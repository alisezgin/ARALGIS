// C:\ali\github-home\ARALGIS\ARALGIS\Database\SourceFiles\DriverInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ARALGIS.h"
#include "C:\ali\github-home\ARALGIS\ARALGIS\Database\HeaderFiles\DriverInfoDlg.h"
#include "afxdialogex.h"


// CDriverInfoDlg dialog

IMPLEMENT_DYNAMIC(CDriverInfoDlg, CDialogEx)

CDriverInfoDlg::CDriverInfoDlg(
CString & _NatIdNo
, CString & _Type
, CString & _Division
, CWnd* pParent /*=NULL*/)
	: CDialogEx(CDriverInfoDlg::IDD, pParent)
	, m_NatIdNo(_NatIdNo)
	, m_Type(_Type)
	, m_Division(_Division)
{
}

CDriverInfoDlg::~CDriverInfoDlg()
{
}

BOOL CDriverInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_cNatIdNo.SetWindowTextW(m_NatIdNo);
	m_cType.SetWindowTextW(m_Type);
	m_cDivision.SetWindowTextW(m_Division);
	return FALSE;
}

void CDriverInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DLG_C_DRIVERINFO_NATIDNO, m_cNatIdNo);
	DDX_Control(pDX, IDC_DLG_C_DRIVERINFO_KATEGORI, m_cType);
	DDX_Control(pDX, IDC_DLG_C_DRIVERINFO_DIVISION, m_cDivision);
}


BEGIN_MESSAGE_MAP(CDriverInfoDlg, CDialogEx)
END_MESSAGE_MAP()


// CDriverInfoDlg message handlers
