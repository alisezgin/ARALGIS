#pragma once


// CDBUpdateAckDlg dialog

class CDBUpdateAckDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDBUpdateAckDlg)

public:
	CDBUpdateAckDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDBUpdateAckDlg();

// Dialog Data
	enum { IDD = IDD_DB_MODIFY_ACK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	// to switch the default button manually, override OnInitDialog
	virtual BOOL OnInitDialog() override;

	DECLARE_MESSAGE_MAP()
};
