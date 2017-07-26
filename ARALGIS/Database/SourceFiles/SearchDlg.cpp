// C:\ali\github-home\ARALGIS\ARALGIS\database\SourceFiles\SearchDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ARALGIS.h"
#include "SearchDlg.h"
#include "afxdialogex.h"


// CSearchDlg dialog

IMPLEMENT_DYNAMIC(CSearchDlg, CDialogEx)

CSearchDlg::CSearchDlg(const VectorType& _driverList,
const VectorType& _vehicleTypeList,
const VectorType& _gateList,
MapType& _PosDriverIdMap,
std::unordered_map<long,long>& _PosVehicleTypeIdMap,
MapType& _PosGateIdMap,
CWnd* pParent /*=NULL*/)
	: CDialogEx(CSearchDlg::IDD, pParent)
	, m_DateBegin{}
	, m_DateEnd{}
	, m_strLicensePlate{ _T("") }
	, m_strFilter{ _T("") }
	, m_DriverList{ _driverList }
	, m_VehicleTypeList{ _vehicleTypeList }
	, m_GateList{ _gateList }
	, m_PosDriverIdMap{ _PosDriverIdMap }
	, m_PosVehicleTypeIdMap{ _PosVehicleTypeIdMap }
	, m_PosGateIdMap{ _PosGateIdMap }
{
}

CSearchDlg::~CSearchDlg()
{
}

void CSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SEARCH_DTP_DATEBEGIN, m_DateBegin);
	DDX_Control(pDX, IDC_SEARCH_DTP_DATEEND, m_DateEnd);
	DDX_Control(pDX, IDC_SEARCH_E_LICENSEPLATE, m_eLicensePlate);
	DDX_Control(pDX, IDC_SEARCH_C_VTYPE, m_cVehicleType);
	DDX_Control(pDX, IDC_SEARCH_C_DRIVER, m_cDriver);
	DDX_Control(pDX, IDC_SEARCH_C_GATE, m_cGate);
}

// reset the format of the date/time combo boxes
BOOL CSearchDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_DateBegin.SetFormat(L"dd-MM-yyyy HH:mm");
	m_DateEnd.SetFormat(L"dd-MM-yyyy HH:mm");
	FillDriverList();
	FillGateList();
	FillVehicleTypeList();
	return(FALSE);
}

// only when OK is pressed will it collect the field contents and form the filter string
void CSearchDlg::OnOK()
{
	m_strFilter = _T("1=1");
	DATETIMEPICKERINFO dtpi;
	dtpi.cbSize = sizeof(DATETIMEPICKERINFO);
	m_DateBegin.GetDateTimePickerInfo(&dtpi);
	if (dtpi.stateCheck & STATE_SYSTEM_CHECKED)
	{
		CTime tDateBegin{};
		m_DateBegin.GetTime(tDateBegin);
		m_strFilter += _T(" AND [EntryDateTime] >= '");
		m_strFilter += tDateBegin.Format("%Y-%m-%d %H:%M");
		m_strFilter += _T("'");
	}

	m_DateEnd.GetDateTimePickerInfo(&dtpi);
	if (dtpi.stateCheck & STATE_SYSTEM_CHECKED)
	{
		CTime tDateEnd{};
		m_DateEnd.GetTime(tDateEnd);
		m_strFilter += _T(" AND [EntryDateTime] <= '");
		m_strFilter += tDateEnd.Format("%Y-%m-%d %H:%M");
		m_strFilter += _T("'");
	}
	m_eLicensePlate.GetWindowText(m_strLicensePlate);
	m_strLicensePlate.Remove(_T(' '));
	m_strLicensePlate.MakeUpper();
	if (m_strLicensePlate != _T(""))
	{
		m_strFilter += _T(" AND [LicensePlate] = '");
		m_strFilter += m_strLicensePlate;
		m_strFilter += _T("'");
	}
	//MessageBox(_T("After license plate: ") + m_strFilter);

	long posVehicleType = m_cVehicleType.GetCurSel();
	// no selection and selecting "Butun Araclar" are equivalent.
	if (posVehicleType != CB_ERR && posVehicleType > 0) 
	{
		m_strFilter += _T(" AND [VehicleType] = '");
		CString strVTID{};
		// do not forget to decrement the position index 
		// to account for the first line "Butun Araclar".
		long id = m_PosVehicleTypeIdMap[posVehicleType - 1];
		strVTID.Format(_T("%ld"), id);
		/*auto iter = m_PosVehicleTypeIdMap.find(posVehicleType-1);
		strVTID.Format(_T("%ld"), *iter);*/
		m_strFilter += strVTID;
		m_strFilter += _T("'");
	}
	//MessageBox(_T("After Vehicle Type: ") + m_strFilter);

	long posDriver = m_cDriver.GetCurSel();
	// no selection and selecting "Butun Suruculer" are equivalent
	if (posDriver != CB_ERR && posDriver > 0)
	{
		m_strFilter += _T(" AND [DriverID] = '");
		CString strDriver{};
		// do not forget to decrement the position index 
		// to account for the first line "Butun Suruculer"
		long id = m_PosDriverIdMap[posDriver - 1];
		strDriver.Format(_T("%ld"), id);
		/*auto iter = m_PosDriverIdMap.find(posDriver);
		strDriver.Format(_T("%ld"), *iter);*/
		m_strFilter += strDriver;
		m_strFilter += _T("'");
	}
	//MessageBox(_T("After Driver: ") + m_strFilter);

	long posGate = m_cGate.GetCurSel();
	// no selection and selecting "Butun Kapilar" are equivalent
	if (posGate != CB_ERR && posGate > 0) 
	{
		m_strFilter += _T(" AND [GateID] = '");
		CString strGate;
		// do not foget to decrement the position index
		// to account for the first line "Butun Kapilar"
		long id = m_PosGateIdMap[posGate - 1];
		strGate.Format(_T("%ld"), id);
		/*auto iter = m_PosGateIdMap.find(posGate-1);
		strGate.Format(_T("%ld"), *iter);*/
		m_strFilter += strGate;
		m_strFilter += _T("'");
	}
	//MessageBox(_T("After Gate: ") + m_strFilter);

	CDialogEx::OnOK();
}

void CSearchDlg::FillDriverList()
{
	m_cDriver.ResetContent();
	m_cDriver.InsertString(0, CString{ _T("Butun Suruculer") });
	if (m_DriverList.size() > 0)
	{
		for (auto p : m_DriverList)
		{
			m_cDriver.InsertString(-1, p.first);
		}
	}
}

void CSearchDlg::FillVehicleTypeList()
{
	m_cVehicleType.ResetContent();
	m_cVehicleType.InsertString(0, CString{ _T("Butun Araclar") });
	if (m_VehicleTypeList.size() > 0)
	{
		for (const std::pair<CString, long>& p : m_VehicleTypeList)
		{
			m_cVehicleType.InsertString(-1, p.first);
		}
	}
}

void CSearchDlg::FillGateList()
{
	m_cGate.ResetContent();
	m_cGate.InsertString(0, CString{ _T("Butun Kapilar") });
	if (m_GateList.size() > 0)
	{
		for (auto p : m_GateList)
		{
			m_cGate.InsertString(-1, p.first);
		}
	}
}

CString CSearchDlg::getFilter()
{
	return m_strFilter;
}


BEGIN_MESSAGE_MAP(CSearchDlg, CDialogEx)
END_MESSAGE_MAP()


// CSearchDlg message handlers
