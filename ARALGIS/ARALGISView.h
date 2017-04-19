
// ARALGISView.h : interface of the CARALGISView class
//

#pragma once


#include "WndResizer.h"
#include "ColorFormView.h"

//#include ".\\BitmapDisplay\\HeaderFiles\\ShowpicCV.h"
#include ".\\BitmapDisplay\\HeaderFiles\\PictureCtrl.h"
//#include ".\\BitmapDisplay\\HeaderFiles\\PkMattoGDI.h"
//#include ".\\BitmapDisplay\\HeaderFiles\\MatBitmapconvert.h"


#include ".\\BitmapDisplay\\HeaderFiles\\PkMattoGDI.h"



#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include "afxwin.h"

#include "ColourButton.h"


class CARALGISDoc;

class CARALGISView : public CColorFormView
{
protected: // create from serialization only
	CARALGISView();
	DECLARE_DYNCREATE(CARALGISView)

public:
	enum{ IDD = IDD_ARALGIS_FORM };

	// Attributes
public:
	CARALGISDoc* GetDocument() const;

	// Operations
public:

	// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	// Implementation
public:
	virtual ~CARALGISView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void DisplayPTSImage();
	void DeletePTSImage();
	void SetTimerDisplay();

	void UpdatePTSStatus(bool aStatus);
	void UpdatePeripheralStatus(bool aStatus);

protected:

	CStatic *m_RefImgBMP;
	CStatic *m_TestImgBMP;

	PkMatToGDI *m_MatToGDITest;
	PkMatToGDI *m_MatToGDIRef;

	CPictureCtrl m_CarPlakaImageStatic;

	int m_TimerSecondCounter;

	// 0: Original Test Image
	// 1: Filter-1 applied Test Image
	// 2: Filter-1 applied Test Image
	// 3: Filter-3 applied Test Image
	int m_iDisplayTestImageType;

private:
	CWndResizer m_resizer;



public:
	char m_FilenameRef[500];
	char m_FilenameTest[500];

	CString m_PlakaStr;
	CEdit m_PlakaCtrl;



	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

protected:
	afx_msg LRESULT OnCameraDataReady(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDBaseCarInfoReady(WPARAM wParam, LPARAM lParam); 

public:
	afx_msg void OnCameraConfig();
	afx_msg void OnSelectRecording();
	afx_msg void OnDisplaySelectedBitmap();
	afx_msg void OnDisplayOpenCV();


	virtual void OnInitialUpdate();

	afx_msg void OnBnClickedButtonBarrierOpen();
	afx_msg void OnBnClickedButtonBarrierClose();
	afx_msg void OnBnClickedButtonHeaterOn();
	afx_msg void OnBnClickedButtonHeaterOff();
	afx_msg void OnBnClickedButtonAlarmOn();
	afx_msg void OnBnClickedButtonAlarmOff();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

protected:
	CClrButton m_PTS_Status;
	CClrButton m_PeripheralStatus;


public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	CButton m_BarrierOpenBtn;
	CButton m_BarrierCloseBtn;
	CButton m_HeaterStartBtn;
	CButton m_HeterStopBtn;
	CButton m_AlarmStartBtn;
	CButton m_AlarmStop;
	afx_msg void OnBnClickedButtonFilter1();
	afx_msg void OnBnClickedButtonFilter2();
	afx_msg void OnBnClickedButtonFilter3();
};

#ifndef _DEBUG  // debug version in ARALGISView.cpp
inline CARALGISDoc* CARALGISView::GetDocument() const
{
	return reinterpret_cast<CARALGISDoc*>(m_pDocument);
}
#endif

