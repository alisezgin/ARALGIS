#pragma once

#include ".\\BitmapDisplay\\HeaderFiles\\PkMattoGDI.h"
#include "WndResizer.h"
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
private:
	CWndResizer m_resizer;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	cv::Mat m_cvMat;

	DECLARE_MESSAGE_MAP()
public:
	CStatic CBmpDisplayer;
	PkMatToGDI* m_MatToGDI;

	void SetCVMat(cv::Mat& aInMat);
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
};
