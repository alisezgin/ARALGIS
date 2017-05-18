#pragma once


// CDriverAckDlg dialog

class CDriverAckDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDriverAckDlg)

public:
	CDriverAckDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDriverAckDlg();

// Dialog Data
	enum { IDD = IDD_DB_MODIFY_ACK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog() override;

	DECLARE_MESSAGE_MAP()
};
