#pragma once
#include "afxcmn.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>

#include ".\\BitmapDisplay\\HeaderFiles\\PkMattoGDI.h"



#include "VehiclePassageSet.h"
#include <unordered_map>
#include "ImagePrep.h"

// CReportDlg dialog

using MapTypeLStr = std::unordered_map<long, CString>;

class CReportDlg : public CDialogEx
{

#define	LICENSE_PLATE_POS 1
#define DRIVER_POS 2
#define ENTRY_DATE_POS 3
#define GATE_POS 4
#define USER_POS 5
#define VEHICLE_TYPE_POS 6
#define DIVISION_POS 7

	DECLARE_DYNAMIC(CReportDlg)

public:
	CReportDlg(CVehiclePassageSet & _vPassageSet, 
		MapTypeLStr& _driverMap,
		MapTypeLStr& _userMap,
		MapTypeLStr& _gateMap,
		MapTypeLStr& _vehicleTypeMap,
		CWnd* pParent = NULL);   // standard constructor
	virtual ~CReportDlg();
	virtual BOOL OnInitDialog() override;

// Dialog Data
	enum { IDD = IDD_DIALOG_REPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CVehiclePassageSet & m_VehiclePassageSet;
	MapTypeLStr& m_DriverMap;
	MapTypeLStr& m_UserMap;
	MapTypeLStr& m_GateMap;
	MapTypeLStr& m_VehicleTypeMap;

	int FindMaxLength(int colNo);
	CImagePrep* m_ImagePrep;

	
	cv::Mat m_CVChassis;
	cv::Mat m_CVFrontal;

	PkMatToGDI *m_ChassisMatToGDI;
	PkMatToGDI *m_FrontalMatToGDI;

	// handle to the chassis picture control box
	CStatic* m_ChassisPicCtrl;
	// handle to the picture control box displaying frontal image
	CStatic* m_FrontalPicCtrl;

	DECLARE_MESSAGE_MAP()
public:
	// multi-column tabular data each line of which contains a row from VehiclePassage Table
	CListCtrl m_ReportList;
	afx_msg void OnNMClickDlgLcbReport(NMHDR *pNMHDR, LRESULT *pResult);
};
