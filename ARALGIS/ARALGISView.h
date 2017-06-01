
// ARALGISView.h : interface of the CARALGISView class
//

#pragma once


#include "WndResizer.h"
#include "ColorFormView.h"
#include "ColourButton.h"
#include ".\\BitmapDisplay\\HeaderFiles\\PictureCtrl.h"
#include ".\\BitmapDisplay\\HeaderFiles\\PkMattoGDI.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include "afxwin.h"

#include "ColourButton.h"
#include "afxdtctl.h"
#include "atltime.h"

class CARALGISDoc;

class CARALGISView : public CColorFormView
{
protected: // create from serialization only
	CARALGISView();
	virtual ~CARALGISView();
	DECLARE_DYNCREATE(CARALGISView)

public:
	enum{ IDD = IDD_ARALGIS_FORM };

	// Attributes
public:
	CARALGISDoc* GetDocument() const;

	// Operations
public:

	// Overrides
protected:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate();
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);


#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Implementation
protected:
	void DeletePTSImage();
	void SetTimerDisplay();
	void GetTestImageAsByte();

	// called to update the vehicle info boxes upon the retrieval of a new license plate number
	void OnLPUpdateInfo(CString);

protected:

	CStatic *m_RefImgBMP;
	CStatic *m_TestImgBMP;

	PkMatToGDI *m_MatToGDITest;
	PkMatToGDI *m_MatToGDIRef;

	bool m_AutoFit;

	cv::Mat    m_RefCVMat;
	cv::Mat	   m_TestCVMat;

	CPictureCtrl m_CarPlakaImageStatic;
	CStatic m_PlakaDisplayControl;

	int m_TimerSecondCounter;

	// 0: Original Test Image
	// 1: Filter-1 applied Test Image
	// 2: Filter-1 applied Test Image
	// 3: Filter-3 applied Test Image
	int m_iDisplayTestImageFilterType;
	int m_iDisplayTestImageColourType;

	int m_iDisplayRefImageFilterType;
	int m_iDisplayRefImageColourType;
	BOOL m_Ref1FilterOK;
	BOOL m_Ref2FilterOK;
	BOOL m_Ref3FilterOK;


	CString m_PlakaStr;
	CEdit m_PlakaCtrl;

	CFont m_Font;

	CClrButton m_PTS_Status;
	CClrButton m_PeripheralStatus;
	CButton m_BarrierOpenBtn;
	CButton m_BarrierCloseBtn;
	CButton m_HeaterStartBtn;
	CButton m_HeterStopBtn;
	CButton m_AlarmStartBtn;
	CButton m_AlarmStop;

	CListBox m_FormListBox;
	// to be removed: enter licenseplates manually
	CString m_FormLPEntry;
	// ARALGISForm'da arac plaka numarasi kutusu icin
	CString m_FormELP;
	CString m_FormEUID;
	CString m_FormEGID;
	CString m_FormEDID;
	// controls the edit box for License Plate Image; eventually will be of type MyPic
	CString m_FormELPI;
	// controls the edit box of frontal view image in ARALGISForm; eventually will be of type MyPic
	CString m_FormEFVI;
	//// controls the edit box of car chassis bottom image (current) in ARALGISForm; eventually will be of type MyPic
	//CString m_FormECBCI;
	//// controls the edit box for chassis bottom reference image in ARALGISForm; eventually will be of type MyPic
	//CString m_FormECBRI;
	// holds the driver name for the current record
	// CString m_FormEDN;
	// holds the registration number for the current record
	// CString m_FormERN;

	// snaps the current time and date for the current vehicle database entry
	CTime m_FormEDT;
	// checks whether the current vehicle is in the black list
	CButton m_FormEBL;

	CButton m_ColourRadioOriginal;
	CButton m_FilterRadioOriginal;

	// holds the ID of the passing vehicle
	long m_VID;
	// holds the ID of a driver
	long m_DID;
	// holds the gate number of vehicle entry
	long m_GID;
	// holds the ID of the current gate keeper
	long m_UID;

private:
	CWndResizer m_resizer;

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnCameraDataReady(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDBaseCarInfoReady(WPARAM wParam, LPARAM lParam); 
	afx_msg LRESULT Filter1Available(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT Filter2Available(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT Filter3Available(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT Filter4Available(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT Filter5Available(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT Filter6Available(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT DisplayPTSImage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT DisplayPlakaNo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT UpdatePTSStatusLost(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT UpdatePTSStatusNOK(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT UpdatePTSStatusOK(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT  UpdatePeripheralStatusOK(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT  UpdatePeripheralStatusNOK(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT ChangeDetectionFinished(WPARAM wParam, LPARAM lParam);

	afx_msg void OnBnClickedButtonBarrierOpen();
	afx_msg void OnBnClickedButtonBarrierClose();
	afx_msg void OnBnClickedButtonHeaterOn();
	afx_msg void OnBnClickedButtonHeaterOff();
	afx_msg void OnBnClickedButtonAlarmOn();
	afx_msg void OnBnClickedButtonAlarmOff();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLbnSelchangeList2();
	afx_msg void OnAdd();
	afx_msg void OnGuncelle();
	afx_msg void OnBnClickedFormBdnQuery();
	afx_msg void OnBnClickedFormBupdatedb();
	afx_msg void OnEnKillfocusFormEdid();
	afx_msg void OnBnClickedRadioColour1();
	afx_msg void OnBnClickedRadioColour2();
	afx_msg void OnBnClickedRadioColour3();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
public:
	afx_msg void OnBnClickedRadio1Origfilter();
	afx_msg void OnBnClickedRadioFilter1();
	afx_msg void OnBnClickedRadioFilter2();
	afx_msg void OnBnClickedRadioFilter3();
	CButton m_ChangeDetectControl;
	afx_msg void OnBnClickedButtonChangeDetect();
};

#ifndef _DEBUG  // debug version in ARALGISView.cpp
inline CARALGISDoc* CARALGISView::GetDocument() const
{
	return reinterpret_cast<CARALGISDoc*>(m_pDocument);
}
#endif
