
// ARALGISView.h : interface of the CARALGISView class
//

#pragma once


#include "WndResizer.h"
#include "ColorFormView.h"
#include "ColourButton.h"
#include ".\\BitmapDisplay\\HeaderFiles\\PictureCtrl.h"
#include ".\\BitmapDisplay\\HeaderFiles\\PkMattoGDI.h"

#include "ImageDisplayTools\\ImageControl\\HeaderFiles\\ImageControl.h"
#include ".\\DrawRectangle\\HeaderFiles\\Rectangle.h"


#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include "afxwin.h"

#include "ColourButton.h"
#include "afxdtctl.h"
#include "atltime.h"

#include "VisitInfo.h"
#include <unordered_map>
#include "ImagePrep.h"

class CARALGISDoc;

class CARALGISView : public CColorFormView
{
protected:
	struct DriverInfo {
		CString _NatIdNo;
		CString _Type;
		CString _Division;
	};

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
	bool m_IsActive;

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
	void GetTestImageAsByte();

	// called to update the vehicle info boxes upon the retrieval of a new license plate number
	void OnLPUpdateInfo(CString);

	// clears the preview box of the chassis bottom
	void ClearPreviewBox();

	// clears the Preview, Test and Ref picture control boxes
	// uses the relevant pointers to fill associated backgrounds
	void ClearPictureBoxes();

	// prepares the image filename to be saved in the Cars/ChassisBottom directory
	/* std::string PrepareImageFilename();
	std::string PrepareImageFilename(CString const &, CTime const &);
	std::string PrepareImageFilename(CString const &, CString const &); */
	BOOL SaveImage(std::string const &);
	BOOL UpdateRefImage(CString const &);

	// inserts the current passage info into the VehiclePassage Table
	void UpdateVehiclePassage();

	// prepares m_DriverList to be used in the driver list combo box
	void PrepareDriverList();
	void PrepareDriverMap();
	void PreparePosDriverIdMap();
	// fills the drop down list with the contents of m_DriverList
	void FillDriverList();
	void FillDriverBox();

	// prepares m_UserList to be used in the keeper list combo box
	void PrepareUserList();
	void PrepareUserMap();
	void PreparePosUserIdMap();
	// fills the drop down list with the contents of m_UserList
	void FillUserList();
	void FillUserBox();

	// prepares m_GateList to be used in the gate list combo box
	void PrepareGateList();
	void PrepareGateMap();
	void PreparePosGateIdMap();
	// fills the drop down list with the contents of m_GateList
	void FillGateList();
	void FillGateBox();

	// prepares m_VehicleTypeList to be passed on to the search query
	void PrepareVehicleTypeList();
	void PrepareVehicleTypeMap();
	void PreparePosVehicleTypeIdMap();
	// fills the drop down list with the contents of the m_VehicleTypeList
	void FillVehicleTypeList();
//M	void FillVehicleTypeBox();

	// prepares m_DriverTypeList to be used by the CDriverDlg
	void PrepareDriverTypeList();
	void PrepareDriverTypeMap();
	void PreparePosDriverTypeIdMap();

	// prepares a disjunction (CString) for a given vector of ID's (of type long)
	CString GenDisjunctionOfVec(CString colName, const std::vector<long> & vec);

protected:
	CStatic    *m_PrevImgBMP;
	PkMatToGDI *m_MatToGDIPrev;
	cv::Mat     m_CVImagePrev;

	CImageControl m_MatToImageTest;
	CImageControl m_MatToImageRef;

	bool m_AutoFit;

	cv::Mat m_RefCVMat;
	cv::Mat	m_TestCVMat;
	cv::Mat m_PrevCVMat;

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


//	CString m_PlakaStr;
//	CEdit m_PlakaCtrl;

	CFont m_Font;

	CClrButton m_PTS_Status;
	CClrButton m_PeripheralStatus;
	CButton m_BarrierOpenBtn;
	CButton m_BarrierCloseBtn;
	CButton m_HeaterStartBtn;
	CButton m_HeterStopBtn;
	CButton m_AlarmStartBtn;
	CButton m_AlarmStop;

	// control variable for the combo box holding the most recent visits to the premises.
	CComboBox m_formCBoxVisitList;
	// an array holding the driver name, keeper name and entry gate per visit
	CVisitInfo m_VisitInfo[VISIT_LIST_LENGTH];

	// to be removed: enter licenseplates manually
	CString m_FormLPEntry;
	// ARALGISForm'da arac plaka numarasi kutusu icin
	CString m_FormELP;

	CString m_FormEUID;
	CString m_FormEGID;
	CString m_FormEDID;
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

	CButton m_ChangeDetectControl;

	// holds the ID of the passing vehicle
	long m_VID;
	// holds the ID of a driver
	long m_DID;
	// holds the gate number of vehicle entry
	long m_GID;
	// holds the ID of the current gate keeper
	long m_UID;

	// holds the absolute path to the 'Cars' directory
	//CString m_PathToCars;
	CImagePrep* m_ImagePrep;

	// controls the driver list combo box
	CComboBox m_FormCBDriverList;
	// holds the list of drivers in the database
	std::vector<std::pair<CString, long>> m_DriverList;
	std::unordered_map<long, CString> m_DriverMap;
	std::unordered_map<long, long> m_PosDriverIdMap;
	// maps driver ID to an information summary
	std::unordered_map<long, DriverInfo> m_DriverInfoMap;

	// controls the user list combo box
	CComboBox m_FormCBUserList;
	// holds the list of gate keepers in the database
	std::vector<std::pair<CString, long>> m_UserList;
	std::unordered_map<long, CString> m_UserMap;
	std::unordered_map<long, long> m_PosUserIdMap;

	// controls the gate list combo box
	CComboBox m_FormCBGateList;
	// holds the list of gate names in the database
	std::vector<std::pair<CString, long>> m_GateList;
	std::unordered_map<long, CString> m_GateMap;
	std::unordered_map<long, long> m_PosGateIdMap;

	// holds the list of vehicle types in the database
	std::vector<std::pair<CString, long>> m_VehicleTypeList;
	std::unordered_map<long, CString> m_VehicleTypeMap;
	std::unordered_map<long, long> m_PosVehicleTypeIdMap;

	// holds the list of driver types in the database
	std::vector<std::pair<CString, long>> m_DriverTypeList;
	std::unordered_map<long, CString> m_DriverTypeMap;
	std::unordered_map<long, long> m_PosDriverTypeIdMap;

	// keeps track of whether the displayed information 
	// has been inserted into the VehiclePassage Table
	BOOL m_bVehiclePassageUpdated;

	// to have a better control on the background color
	CBrush m_brush;

	////////////// mouse move /////////////
	// First point of the recorded element
	CPoint m_FirstPoint;

	// Second point of the object to be drawn
	CPoint m_SecondPoint;

	// pointer to the temporary drawing element
	CElement* m_pTempElement;

	// Cursor position
	CPoint m_CursorPos;

	// start position of move function
	CPoint m_FirstPos;

	CButton m_CropCheckBox;

	bool m_IsCropActive;
	int  m_CropWhichImage;
	int isInUVSSImages(CPoint aInPoint);
	///////////////////////////////////////////////


private:
	CWndResizer m_resizer;

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnCameraDataReady(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT GetTestImage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDBaseFileWritten(WPARAM wParam, LPARAM lParam);
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
	afx_msg LRESULT UpdatePeripheralStatusOK(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT UpdatePeripheralStatusNOK(WPARAM wParam, LPARAM lParam);
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
	afx_msg void OnBnClickedRadio1Origfilter();
	afx_msg void OnBnClickedRadioFilter1();
	afx_msg void OnBnClickedRadioFilter2();
	afx_msg void OnBnClickedRadioFilter3();
	afx_msg void OnBnClickedButtonChangeDetect();
	afx_msg void OnCbnSelchangeFormCboxVisitlist();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnReportSearch();
public:
	afx_msg void OnBnClickedCheckCrop();
	afx_msg LRESULT OnLButtonDownImage(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnLButtonUpImage(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseMoveImage(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseWheelImage(WPARAM wparam, LPARAM lparam);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCbnSelchangeFormCboxDriverlist();
	afx_msg void OnBnClickedFormBDriverinfo();
	afx_msg void OnDriverNew();
	afx_msg void OnUserNew();
};

#ifndef _DEBUG  // debug version in ARALGISView.cpp
inline CARALGISDoc* CARALGISView::GetDocument() const
{
	return reinterpret_cast<CARALGISDoc*>(m_pDocument);
}
#endif
