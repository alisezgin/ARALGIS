#pragma once
#include "afxwin.h"


// CDriverInfoDlg dialog

class CDriverInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDriverInfoDlg)

public:
	CDriverInfoDlg(CString & _NatIdNo,
		CString & _Type,
		CString & _Division,
		CWnd* pParent = NULL);   // standard constructor
	virtual ~CDriverInfoDlg();
	virtual BOOL OnInitDialog() override;

// Dialog Data
	enum { IDD = IDD_DIALOG_DRIVERINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	// holds the national Id No of the driver
	CString m_NatIdNo;
	// holds the driver type
	CString m_Type;
	// holds the division of the driver
	CString m_Division;

protected:
	CEdit m_cNatIdNo;
	CEdit m_cType;
	CEdit m_cDivision;
};
