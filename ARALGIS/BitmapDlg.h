#pragma once

#include ".\\BitmapDisplay\\HeaderFiles\\Showpic.h"
#include "afxwin.h"

// CBitmapDlg dialog

class CBitmapDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CBitmapDlg)

public:
	CBitmapDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBitmapDlg();

// Dialog Data
	enum { IDD = IDD_BITMAP_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	//CShowpicCV * m_ImageToDisplay;
	CShowpic* m_ImageToDisplay;

public:
	afx_msg void OnBnClickedCancel();
	virtual void PostNcDestroy();
};
