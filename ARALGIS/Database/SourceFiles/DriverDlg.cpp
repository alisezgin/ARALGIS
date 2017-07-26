// C:\ali\github-home\ARALGIS\ARALGIS\Database\SourceFiles\DriverDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ARALGIS.h"
#include "DriverDlg.h"
#include "afxdialogex.h"

#include <sstream>

#define DRIVER_LICENSE_AGE_LIMIT	18


// CDriverDlg dialog

IMPLEMENT_DYNAMIC(CDriverDlg, CDialogEx)

std::vector<const TCHAR*> CDriverDlg::m_CityList
{
	_T("Adana"), _T("Adiyaman"), _T("Afyon"), _T("Agri"), _T("Amasya"), _T("Ankara"), _T("Antalya"), _T("Artvin"), _T("Aydin")
	, _T("Balikesir"), _T("Bilecik"), _T("Bingol"), _T("Bitlis"), _T("Bolu"), _T("Burdur")
	, _T("Canakkale"), _T("Cankiri"), _T("Corum")
	, _T("Denizli"), _T("Diyarbakir") 
	, _T("Edirne"), _T("Elazig"), _T("Erzincan"), _T("Erzurum"), _T("Eskisehir")
	, _T("Gaziantep"), _T("Giresun"), _T("Gumushane")
	, _T("Hakkari"), _T("Hatay")
	, _T("Isparta")
	, _T("Icel"), _T("Istanbul"), _T("Izmir")
	, _T("Kars"), _T("Kastamonu"), _T("Kayseri"), _T("Kirklareli"), _T("Kirsehir"), _T("Kocaeli"), _T("Konya"), _T("Kutahya")
	, _T("Malatya"), _T("Manisa"), _T("Kahramanmaras"), _T("Mardin"), _T("Mugla"), _T("Mus")
	, _T("Nevsehir"), _T("Nigde")
	, _T("Ordu")
	, _T("Rize")
	, _T("Sakarya"), _T("Samsun"), _T("Siirt"), _T("Sinop"), _T("Sivas")
	, _T("Tekirdag"), _T("Tokat"), _T("Trabzon"), _T("Tunceli")
	, _T("Sanliurfa"), _T("Usak")
	, _T("Van")
	, _T("Yozgat")
	, _T("Zonguldak")
	, _T("Aksaray")
	, _T("Bayburt")
	, _T("Karaman"), _T("Kirikkale")
	, _T("Batman")
	, _T("Sirnak")
	, _T("Bartin")
	, _T("Ardahan")
	, _T("Igdir")
	, _T("Yalova")
	, _T("Karabuk"), _T("Kilis")
	, _T("Osmaniye")
	, _T("Duzce")
};

std::vector<const TCHAR*> CDriverDlg::m_BloodTypeList{
	_T("0 RH-"), _T("0 RH+")
	, _T("A RH-"), _T("A RH+")
	, _T("B RH-"), _T("B RH+")
	, _T("AB RH-"), _T("AB RH+")
};

CDriverDlg::CDriverDlg(std::vector<std::pair<CString, long>>& _DriverTypeList,
	std::unordered_map<long, long>& _PosDriverTypeIdMap,
	CWnd* pParent /*=NULL*/)
	: CDialogEx(CDriverDlg::IDD, pParent)
	, m_DriverTypeList(_DriverTypeList)
	, m_PosDriverTypeIdMap(_PosDriverTypeIdMap)
{
}

CDriverDlg::~CDriverDlg()
{
}

BOOL CDriverDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	for (auto pos = 0; pos < m_CityList.size(); ++pos)
	{
		m_DriverNatIdIssueCity.InsertString(pos, m_CityList[pos]);
		m_DriverLicenseIssueCity.InsertString(pos, m_CityList[pos]);
	}
	for (auto pos = 0; pos < m_BloodTypeList.size(); ++pos)
	{
		m_DriverBloodType.InsertString(pos, m_BloodTypeList[pos]);
	}

	for (auto pos = 0; pos < m_DriverTypeList.size(); ++pos)
	{
		m_DriverType.InsertString(pos, m_DriverTypeList[pos].first);
	}

	return FALSE;
}

void CDriverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DLG_C_DRIVER_NAME, m_DriverName);
	DDX_Control(pDX, IDC_DLG_C_DRIVER_LASTNAME, m_DriverLastName);
	DDX_Control(pDX, IDC_DLG_CB_DRIVER_TYPE, m_DriverType);
	DDX_Control(pDX, IDC_DLG_CB_DRIVER_LICENSEISSUECITY, m_DriverLicenseIssueCity);
	DDX_Control(pDX, IDC_DLG_C_DRIVER_LICENSEISSUEDISTRICT, m_DriverLicenseIssueDistrict);
	DDX_Control(pDX, IDC_DLG_C_DRIVER_NATIDNO, m_DriverNatIdNo);
	DDX_Control(pDX, IDC_DLG_DT_DRIVER_NATIDBIRTHDATE, m_DriverBirthDate);
	DDX_Control(pDX, IDC_DLG_C_DRIVER_NATIDBIRTHPLACE, m_DriverPlaceOfBirth);
	DDX_Control(pDX, IDC_DLG_CB_DRIVER_NATIDISSUECITY, m_DriverNatIdIssueCity);
	DDX_Control(pDX, IDC_DLG_C_DRIVER_NATIDISSUEDISTRICT, m_DriverNatIdIssueDistrict);
	DDX_Control(pDX, IDC_DLG_C_DRIVER_NATIDVOLNO, m_DriverNatIDVolNo);
	DDX_Control(pDX, IDC_DLG_C_DRIVER_NATIDROWNO, m_DriverNatIDRowNo);
	DDX_Control(pDX, IDC_DLG_C_DRIVER_NATIDLOGNO, m_DriverNatIDLogNo);
	DDX_Control(pDX, IDC_DLG_C_DRIVER_NATIDFATHER, m_DriverFatherName);
	DDX_Control(pDX, IDC_DLG_C_DRIVER_NATIDMOTHER, m_DriverMotherName);
	DDX_Control(pDX, IDC_DLG_CB_DRIVER_NATIDBLOODTYPE, m_DriverBloodType);
	DDX_Control(pDX, IDC_DLG_C_DRIVER_NATIDISSUEAUTHORITY, m_DriverNatIDIssueAuthority);
}

CString CDriverDlg::getName()
{
	return m_strDriverName;
}

CString CDriverDlg::getLastName()
{
	return m_strDriverLastName;
}

long CDriverDlg::getType()
{
	return m_lDriverType;
}

CString CDriverDlg::getDriverLicenseIssueCity()
{
	return m_strDriverLicenseIssueCity;
}

CString CDriverDlg::getDriverLicenseIssueDistrict()
{
	return m_strDriverLicenseIssueDistrict;
}

CString CDriverDlg::getNatIDNo()
{
	return m_strDriverNatIDNo;
}

COleDateTime CDriverDlg::getDateOfBirth()
{
	return m_dtDriverBirthDate;
}

CString CDriverDlg::getBirthPlace()
{
	return m_strDriverPlaceOfBirth;
}

CString CDriverDlg::getNatIDIssueCity()
{
	return m_strDriverNatIdIssueCity;
}

CString CDriverDlg::getNatIDIssueDistrict()
{
	return m_strDriverNatIdIssueDistrict;
}

long LongOfString(CString strLong)
{
	if (strLong == _T(""))
		return 0;
	std::basic_stringstream<TCHAR> ss;
	ss.str((LPCTSTR)strLong);
	long lLong;
	ss >> lLong;
	return lLong;
}

long CDriverDlg::getNatIDVolNo()
{
	return m_lDriverNatIDVolNo;
}

long CDriverDlg::getNatIDRowNo()
{
	return m_lDriverNatIDRowNo;
}

long CDriverDlg::getNatIDLogNo()
{
	return m_lDriverNatIDLogNo;
}

CString CDriverDlg::getFatherName()
{
	return m_strDriverFatherName;
}

CString CDriverDlg::getMotherName()
{
	return m_strDriverMotherName;
}

CString CDriverDlg::getBloodType()
{
	return m_strDriverBloodType;
}

CString CDriverDlg::getIssueAuthority()
{
	return m_strDriverNatIDIssueAuthority;
}

void CDriverDlg::CopyScreenToMembers()
{
		m_DriverName.GetWindowTextW(m_strDriverName);

		m_DriverLastName.GetWindowTextW(m_strDriverLastName);

		int posDriverType = m_DriverType.GetCurSel();
		if (posDriverType == CB_ERR)
			m_lDriverType = -1;
		else
			m_lDriverType = m_PosDriverTypeIdMap[posDriverType];
		
		int posDLIssueCity = m_DriverLicenseIssueCity.GetCurSel();
		if (posDLIssueCity == CB_ERR)
			m_strDriverLicenseIssueCity = _T("");
		else
			m_strDriverLicenseIssueCity = m_CityList[posDLIssueCity];

		m_DriverLicenseIssueDistrict.GetWindowTextW(m_strDriverLicenseIssueDistrict);

		m_DriverNatIdNo.GetWindowTextW(m_strDriverNatIDNo);

		DATETIMEPICKERINFO dtpi;
		dtpi.cbSize = sizeof(DATETIMEPICKERINFO);
		m_DriverBirthDate.GetDateTimePickerInfo(&dtpi);
		if (dtpi.stateCheck & STATE_SYSTEM_CHECKED)
		{
			m_DriverBirthDate.GetTime(m_dtDriverBirthDate);
			// we are forced to use datetime data type on the sql server side
			// by default reset all time components to 0.
			m_dtDriverBirthDate.SetDateTime(
				m_dtDriverBirthDate.GetYear(),
				m_dtDriverBirthDate.GetMonth(),
				m_dtDriverBirthDate.GetDay(),
				0, 0, 0);
			m_dtDriverBirthDate.SetStatus(COleDateTime::valid);
		}
		else
		{
			m_dtDriverBirthDate.SetStatus(COleDateTime::invalid);
		}


		m_DriverPlaceOfBirth.GetWindowTextW(m_strDriverPlaceOfBirth);

		int posNatIdIssueCity = m_DriverNatIdIssueCity.GetCurSel();
		if (posNatIdIssueCity == CB_ERR)
			m_strDriverNatIdIssueCity = _T("");
		else
			m_strDriverNatIdIssueCity = m_CityList[posNatIdIssueCity];

		m_DriverNatIdIssueDistrict.GetWindowTextW(m_strDriverNatIdIssueDistrict);

		CString strVolNo;
		m_DriverNatIDVolNo.GetWindowTextW(strVolNo);
		m_lDriverNatIDVolNo = LongOfString(strVolNo);

		CString strRowNo;
		m_DriverNatIDRowNo.GetWindowTextW(strRowNo);
		m_lDriverNatIDRowNo = LongOfString(strRowNo);

		CString strLogNo;
		m_DriverNatIDLogNo.GetWindowTextW(strLogNo);
		m_lDriverNatIDLogNo = LongOfString(strLogNo);

		m_DriverFatherName.GetWindowTextW(m_strDriverFatherName);

		m_DriverMotherName.GetWindowTextW(m_strDriverMotherName);

		int posBloodType = m_DriverBloodType.GetCurSel();
		if (posBloodType == CB_ERR)
			m_strDriverBloodType = _T("");
		else
			m_strDriverBloodType = m_BloodTypeList[m_DriverBloodType.GetCurSel()];

		m_DriverNatIDIssueAuthority.GetWindowTextW(m_strDriverNatIDIssueAuthority);
}

void CDriverDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	// even though name and lastname are not necessarily unique
	// leaving them as NULL on the database side results in ambiguous driver list
	CString strName;
	m_DriverName.GetWindowTextW(strName);
	if (strName == _T(""))
	{
		MessageBox(_T("Lutfen surucunun adini girin."));
		return;
	}
	CString strLastName;
	m_DriverLastName.GetWindowTextW(strLastName);
	if (strLastName == _T(""))
	{
		MessageBox(_T("Lutfen surucunun soyadini girin."));
		return;
	}

	// Check whether the birth date makes sense.
	DATETIMEPICKERINFO dtpi;
	dtpi.cbSize = sizeof(DATETIMEPICKERINFO);
	m_DriverBirthDate.GetDateTimePickerInfo(&dtpi);
	if (dtpi.stateCheck & STATE_SYSTEM_CHECKED)
	{
		COleDateTime dtCurr = COleDateTime::GetCurrentTime();
		COleDateTime dtBirth;
		m_DriverBirthDate.GetTime(dtBirth);
		COleDateTimeSpan dtDriverAge = dtCurr - dtBirth;
		int DriverAge = dtDriverAge.GetTotalDays() / 365;
		if (DriverAge < DRIVER_LICENSE_AGE_LIMIT)
		{
			MessageBox(_T("Surucu yasi 18'den kucuk."));
			return;
		}
	}

	// Check whether the national ID No has the proper length and 
	// is made up of numerals.
	CString strNatIdNo;
	m_DriverNatIdNo.GetWindowTextW(strNatIdNo);
	if (strNatIdNo.GetLength() != 11)
	{
		MessageBox(_T("TC Kimlik No 11 basamak olmali."));
		return;
	}
	for (auto c = 0; c < 11; ++c)
	{
		TCHAR tc = strNatIdNo.GetAt(c);
		if (tc < _T('0') || tc > _T('9'))
		{
			MessageBox(_T("TC Kimlik No sadece rakamlardan olusmali."));
			return;
		}
	}
	MessageBox(_T("Surucu Tablosu guncelleniyor."));
	CopyScreenToMembers();
	CDialogEx::OnOK();
}



BEGIN_MESSAGE_MAP(CDriverDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDriverDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CDriverDlg message handlers
