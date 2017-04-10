#pragma once

#include "ARALGIS.h"

#include ".\\WindowDisplay\\HeaderFiles\\ShowpicCV.h"


// CBitmapDlg dialog

class CBitmapDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CBitmapDlg)

public:
	CBitmapDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBitmapDlg();

	CShowpicCV *m_ImageToDisplay;


// Dialog Data
	enum { IDD = IDD_BITMAP_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonClose();
	virtual void PostNcDestroy();
};
