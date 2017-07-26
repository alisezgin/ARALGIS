#pragma once
#include "afxwin.h"


// CLoginDlg dialog

class CLoginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLoginDlg)

public:
	CLoginDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoginDlg();

	CString getUsername();
	CString getPassword();

// Dialog Data
	enum { IDD = IDD_DIALOG_LOGIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	// holds the username for the login window
	CEdit m_Username;
	// holds the password in the login dialog
	CEdit m_Password;

	CString m_strUsername;
	CString m_strPassword;

public:
	afx_msg void OnBnClickedOk();
};
