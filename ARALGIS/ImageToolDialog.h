#pragma once

#include "ImageDisplayTools\\StandardLibrary\\HeaderFiles\\ResizableDlg.h"
#include "ImageDisplayTools\\ImageControl\\HeaderFiles\\ImageControl.h"
#include "ImageDisplayTools\\ImageControl\\HeaderFiles\\ResourceList.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "afxwin.h"

// CImageToolDialog dialog

class CImageToolDialog : public CResizableDlg
{
	DECLARE_DYNAMIC(CImageToolDialog)

public:
	CImageToolDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CImageToolDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_IMAGETOOLS };

private:
	//CResourceList m_rl;                  // Resource list.          
	//CArray<tResourceLCID*> *m_pRes;      // Array of resource "Locale Culture Identifier"-s from National Language Support Information.
	//CString m_resFilePath;               // Resource file path.
	//bool m_isOnEnChangeEditWidthHeight;  // Enabled/disabled OnEnChangeEditWidth() and OnEnChangeEditHeight() events.
	CImageControl m_image; 
	HICON m_hIcon;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()



public:
	CButton m_RealSizeButton;
	afx_msg void OnBnClickedRadioOrg();
	afx_msg void OnBnClickedRadioZoomfit();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	void SetCVMat(cv::Mat& aInMat);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	afx_msg LRESULT OnLButtonDownImage(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnLButtonUpImage(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseMoveImage(WPARAM wparam, LPARAM lparam);
};
