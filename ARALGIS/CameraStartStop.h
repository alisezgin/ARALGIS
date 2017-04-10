#pragma once


// CCameraStartStop dialog

class CCameraStartStop : public CDialogEx
{
	DECLARE_DYNAMIC(CCameraStartStop)

public:
	CCameraStartStop(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCameraStartStop();

// Dialog Data
	enum { IDD = IDD_CAMERASTR_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStartButton();
	afx_msg void OnBnClickedPauseButton();
	afx_msg void OnBnClickedStopButton();
	afx_msg void OnBnClickedShowBmpButton();
	afx_msg void OnBnClickedShowOpencvButton();
};
