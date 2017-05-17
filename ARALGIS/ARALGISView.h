
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

	void UpdatePTSStatus(int aStatus);
	void UpdatePeripheralStatus(bool aStatus);
	void GetTestImageAsByte();

	void FilterAvailable(int aNumFilter);

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
	afx_msg void OnLbnSelchangeList2();
	CListBox m_FormListBox;
	// to be removed: enter licenseplates manually
	CString m_FormLPEntry;
	afx_msg void OnAdd();
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
	afx_msg void OnGuncelle();
	// holds the driver name for the current record
	// CString m_FormEDN;
	// holds the registration number for the current record
	// CString m_FormERN;
protected:
	// called to update the vehicle info boxes upon the retrieval of a new license plate number
	void OnLPUpdateInfo(CString);
public:
	// snaps the current time and date for the current vehicle database entry
	CTime m_FormEDT;
	// checks whether the current vehicle is in the black list
	CButton m_FormEBL;
	CButton m_ButtonFilter1;
	CButton m_ButtonFilter2;
	CButton m_ButtonFilter3;
	CButton m_ButtonOrgImage;
	afx_msg void OnBnClickedButtonOrginal();
	// holds the ID of the passing vehicle
	long m_VID;
	// holds the ID of a driver
	long m_DID;
	// holds the gate number of vehicle entry
	long m_GID;
	// holds the ID of the current gate keeper
	long m_UID;
	afx_msg void OnBnClickedFormBdnQuery();
	afx_msg void OnBnClickedFormBupdatedb();

};

#ifndef _DEBUG  // debug version in ARALGISView.cpp
inline CARALGISDoc* CARALGISView::GetDocument() const
{
	return reinterpret_cast<CARALGISDoc*>(m_pDocument);
}
#endif
