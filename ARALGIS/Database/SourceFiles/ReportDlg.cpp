// C:\ali\github-home\ARALGIS\ARALGIS\Database\SourceFiles\ReportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ARALGIS.h"
#include "ReportDlg.h"
#include "afxdialogex.h"

// CReportDlg dialog

IMPLEMENT_DYNAMIC(CReportDlg, CDialogEx)

CReportDlg::CReportDlg(CVehiclePassageSet & _vPassageSet 
, MapTypeLStr& _driverMap
, MapTypeLStr& _userMap
, MapTypeLStr& _gateMap
, MapTypeLStr& _vehicleTypeMap
	, CWnd* pParent /*=NULL*/)
	: 
	CDialogEx(CReportDlg::IDD, pParent)
	, m_VehiclePassageSet(_vPassageSet)
	, m_DriverMap(_driverMap)
	, m_UserMap(_userMap)
	, m_GateMap(_gateMap)
	, m_VehicleTypeMap(_vehicleTypeMap)
	, m_ChassisPicCtrl(NULL)
	, m_FrontalPicCtrl(NULL)
{
	m_ImagePrep = CImagePrep::GetInstance();

	m_ChassisPicCtrl = new CStatic;
	m_FrontalPicCtrl = new CStatic;

	m_ChassisMatToGDI = nullptr;
	m_FrontalMatToGDI = nullptr;
}

CReportDlg::~CReportDlg()
{
}

void CReportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DLG_LCB_REPORT, m_ReportList);
	DDX_Control(pDX, IDC_REPORT_PCTRL_CHASSIS, *m_ChassisPicCtrl);
	DDX_Control(pDX, IDC_REPORT_PCTRL_FRONTAL, *m_FrontalPicCtrl);
}

int CReportDlg::FindMaxLength(int colNo)
{
	return m_ReportList.GetColumnWidth(colNo);
}

BOOL CReportDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	m_ReportList.InsertColumn(0, _T("Kayitlar"));

	m_ReportList.InsertColumn(LICENSE_PLATE_POS, _T("Plaka"));

	m_ReportList.InsertColumn(DRIVER_POS, _T("Surucu"));

	m_ReportList.InsertColumn(ENTRY_DATE_POS, _T("Giris Tarihi"));

	m_ReportList.InsertColumn(GATE_POS, _T("Giris Kapisi"));

	m_ReportList.InsertColumn(USER_POS, _T("Gorevli"));

	m_ReportList.InsertColumn(VEHICLE_TYPE_POS, _T("Arac Tipi"));

	m_ReportList.InsertColumn(DIVISION_POS, _T("Birligi"));

	m_ReportList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

	long rowIndex = 0;
	while (!m_VehiclePassageSet.IsEOF())
	{
		LVITEM lv;
		lv.iItem = rowIndex;
		lv.iSubItem = 0;
		CString strLabel{};
		strLabel.Format(_T("%ld. Kayit:"), rowIndex + 1);
		TCHAR* tcharLabel = new TCHAR[strLabel.GetLength() + 1]; 
		lstrcpy(tcharLabel, strLabel);
		//lv.pszText = (LPTSTR)&(strLabel); 
		lv.mask = LVIF_TEXT;
		lv.pszText = tcharLabel;
		m_ReportList.InsertItem(&lv);
		delete[] tcharLabel;

		lv.iSubItem++;
		lv.pszText = _T("");
		TCHAR tcharLP[15];
		lstrcpy(tcharLP, m_VehiclePassageSet.m_VehiclePassageLicensePlate);
		lv.pszText = tcharLP;
		m_ReportList.SetItem(&lv);

		CString strDefault{ _T("Bilgi yok") };
		LPTSTR ptrDefault{ _T("Bilgi yok.") };

		lv.iSubItem++;
		lv.pszText = _T("");
		long dID = m_VehiclePassageSet.m_VehiclePassageDriverID;
		if (dID < 0)
			lv.pszText = ptrDefault;
		else
		{
			TCHAR tcharDriver[200];
			lstrcpy(tcharDriver, m_DriverMap[dID]);
			lv.pszText = tcharDriver;
		}
		m_ReportList.SetItem(&lv);

		lv.iSubItem++;
		lv.pszText = _T("");
		TCHAR tcharDate[20];
		lstrcpy(tcharDate, m_VehiclePassageSet.m_VehiclePassageEntryDateTime.Format(_T("%Y/%m/%d %H:%M:%S")));
		lv.pszText = tcharDate;
		m_ReportList.SetItem(&lv);

		lv.iSubItem++;
		lv.pszText = _T("");
		long gID = m_VehiclePassageSet.m_VehiclePassageGateID;
		if (gID < 0)
			lv.pszText = ptrDefault;
		else
		{
			TCHAR tcharGate[50];
			lstrcpy(tcharGate, m_GateMap[gID]);
			lv.pszText = tcharGate;
		}
		m_ReportList.SetItem(&lv);

		lv.iSubItem++;
		lv.pszText = _T("");
		long uID = m_VehiclePassageSet.m_VehiclePassageUserID;
		if (uID < 0)
			lv.pszText = ptrDefault;
		else
		{
			TCHAR tcharUser[200];
			lstrcpy(tcharUser, m_UserMap[uID]);
			lv.pszText = tcharUser;
		}
		m_ReportList.SetItem(&lv);

		lv.iSubItem++;
		lv.pszText = _T("");
		long vID = m_VehiclePassageSet.m_VehiclePassageVehicleID;
		if (vID < 0)
			lv.pszText = ptrDefault;
		else
		{
			// lv.pszText = CT2W(m_VehicleTypeMap[vID]);
			// lv.pszText = CT2W(m_VehicleTypeMap[1]);
			TCHAR tcharVehicleType[100];
			lstrcpy(tcharVehicleType, m_VehicleTypeMap[1]);
			lv.pszText = tcharVehicleType;
		}
		m_ReportList.SetItem(&lv);

		// ali: to-do
		/*lv.iSubItem++;
		long divID = m_VehiclePassageSet.m_VehiclePassageDivisionID;
		lv.pszText = _T("");
		if (divID < 0)
			lv.pszText = CT2W(strDefault);
		else
			lv.pszText = CT2W(m_DivisionMap[divID]);
		m_ReportList.SetItem(&lv); */

		++rowIndex;

		m_VehiclePassageSet.MoveNext();
	}

	for (auto i = 0; i < 8; ++i)
		m_ReportList.SetColumnWidth(i, LVSCW_AUTOSIZE);

//	for (auto i = 0; i < 7; ++i)
//		m_ReportList.SetColumnWidth(i, FindMaxLength(i));

	return FALSE;
}

afx_msg void CReportDlg::OnNMClickDlgLcbReport(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	// clean the picture boxes
	if (m_ChassisMatToGDI != nullptr)
	{
		m_ChassisMatToGDI->BackgroundClear();
		delete m_ChassisMatToGDI;
		m_ChassisMatToGDI = nullptr;
	}
	if (m_FrontalMatToGDI != nullptr)
	{
		m_FrontalMatToGDI->BackgroundClear();
		delete m_FrontalMatToGDI;
		m_FrontalMatToGDI = nullptr;
	}

	int pos = m_ReportList.GetSelectionMark();
	/*CString strPos;
	strPos.Format(_T("Position is: %d"), pos);
	TCHAR tcharPos[50];
	lstrcpy(tcharPos, strPos);
	MessageBox(tcharPos);*/

	/*TCHAR tcharLP[15];
	m_ReportList.GetItemText(pos, 2, tcharLP, 15);*/
	CString strLP = m_ReportList.GetItemText(pos, LICENSE_PLATE_POS);
	CString strDate = m_ReportList.GetItemText(pos, ENTRY_DATE_POS);
	strDate.Replace(_T(' '), _T('_'));
	strDate.Remove(_T('/'));
	strDate.Remove(_T(':'));

	std::string fChassis = m_ImagePrep->PrepareChassisFilename(strLP, strDate + _T(".jpg"));
	std::string fFrontal = m_ImagePrep->PrepareFrontalFilename(strLP, strDate + _T(".jpg"));
	/* MessageBox(_T("Filename constructed: ") + CString{ fChassis.c_str() });
	MessageBox(_T("The license plate is: ") + strLP); */

	//std::string strFilename = PrepareImageFilename(m_FormELP, strVisitDateTime);
	DWORD dwAttribChassis = GetFileAttributes(CString{ fChassis.c_str() });

	if (dwAttribChassis == INVALID_FILE_ATTRIBUTES ||
		(dwAttribChassis & FILE_ATTRIBUTE_DIRECTORY))
	{ // if no such file exists, prompt a notification
		MessageBox(_T("File does not exist:"), +CString{ fChassis.c_str() });
	}
	else
	{
		if (m_CVChassis.rows != 0 || m_CVChassis.cols != 0)
		{
			m_CVChassis.release();
		}

		if (g_CameraPixelBits == 24)
		{
			m_CVChassis = cv::imread(fChassis, cv::IMREAD_COLOR);
		}
		else
		{
			m_CVChassis = cv::imread(fChassis, cv::IMREAD_GRAYSCALE);
		}

		if (m_ChassisMatToGDI != nullptr)
		{
			delete m_ChassisMatToGDI;
			m_ChassisMatToGDI = nullptr;
		}

		//m_PrevCVMat = m_CVChassis.clone();
		m_ChassisMatToGDI = new PkMatToGDI(m_ChassisPicCtrl, FALSE);
		m_ChassisMatToGDI->DrawImg(m_CVChassis);
	}


	DWORD dwAttribFrontal = GetFileAttributes(CString{ fFrontal.c_str() });

	if (dwAttribFrontal == INVALID_FILE_ATTRIBUTES ||
		(dwAttribFrontal & FILE_ATTRIBUTE_DIRECTORY))
	{ // if no such file exists, prompt a notification
		MessageBox(_T("File does not exist:"), +CString{ fFrontal.c_str() });
	}
	else
	{
		if (m_CVFrontal.rows != 0 || m_CVFrontal.cols != 0)
		{
			m_CVFrontal.release();
		}

		if (g_CameraPixelBits == 24)
		{
			m_CVFrontal = cv::imread(fFrontal, cv::IMREAD_COLOR);
		}
		else
		{
			m_CVFrontal = cv::imread(fFrontal, cv::IMREAD_GRAYSCALE);
		}

		if (m_FrontalMatToGDI != nullptr)
		{
			delete m_FrontalMatToGDI;
			m_FrontalMatToGDI = nullptr;
		}

		//m_PrevCVMat = m_CVChassis.clone();
		m_FrontalMatToGDI = new PkMatToGDI(m_FrontalPicCtrl, FALSE);
		m_FrontalMatToGDI->DrawImg(m_CVFrontal);
	}

	UpdateData(FALSE);

}

BEGIN_MESSAGE_MAP(CReportDlg, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_DLG_LCB_REPORT, &CReportDlg::OnNMClickDlgLcbReport)
END_MESSAGE_MAP()


// CReportDlg message handlers
