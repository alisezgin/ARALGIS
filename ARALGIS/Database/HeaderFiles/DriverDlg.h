#pragma once
#include "afxwin.h"
#include "afxdtctl.h"

#include <unordered_map>


// CDriverDlg dialog

class CDriverDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDriverDlg)

public:
	CDriverDlg(std::vector<std::pair<CString, long>>& m_DriverTypeList,
		std::unordered_map<long, long>& m_PosDriverTypeIdMap,
		CWnd* pParent = NULL);   // standard constructor
	virtual ~CDriverDlg();

	virtual BOOL OnInitDialog() override;

// Dialog Data
	enum { IDD = IDD_DIALOG_DRIVER };

public:
	CString getName();
	CString getLastName();
	long getType();
	CString getDriverLicenseIssueCity();
	CString getDriverLicenseIssueDistrict();
	CString getNatIDNo();
	COleDateTime getDateOfBirth();
	CString getBirthPlace();
	CString getNatIDIssueCity();
	CString getNatIDIssueDistrict();
	long getNatIDVolNo();
	long getNatIDRowNo();
	long getNatIDLogNo();
	CString getFatherName();
	CString getMotherName();
	CString getBloodType();
	CString getIssueAuthority();



protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void CopyScreenToMembers();

	DECLARE_MESSAGE_MAP()

	// holds the name of the driver
	CEdit m_DriverName;
	CString m_strDriverName;
	// holds the last name of the driver
	CEdit m_DriverLastName;
	CString m_strDriverLastName;
	// holds the type of the driver (selected from a list)
	CComboBox m_DriverType;
	long m_lDriverType;
	// holds the city from where the driver's license was issued
	CComboBox m_DriverLicenseIssueCity;
	CString m_strDriverLicenseIssueCity;
	// holds the district from where the driver's license was issued
	CEdit m_DriverLicenseIssueDistrict;
	CString m_strDriverLicenseIssueDistrict;
	// holds the NatID number
	CEdit m_DriverNatIdNo;
	CString m_strDriverNatIDNo;
	// holds the date of birth (datetime control)
	CDateTimeCtrl m_DriverBirthDate;
	COleDateTime m_dtDriverBirthDate;
	// holds the place of birth
	CEdit m_DriverPlaceOfBirth;
	CString m_strDriverPlaceOfBirth;
	// holds the city from which the national ID was issued (combo box)
	CComboBox m_DriverNatIdIssueCity;
	CString m_strDriverNatIdIssueCity;
	// holds the district from where the national ID was issued
	CEdit m_DriverNatIdIssueDistrict;
	CString m_strDriverNatIdIssueDistrict;
	// holds the volume number of the national ID
	CEdit m_DriverNatIDVolNo;
	long m_lDriverNatIDVolNo;
	// holds the row number of the national ID
	CEdit m_DriverNatIDRowNo;
	long m_lDriverNatIDRowNo;
	// holds the log number of the national ID
	CEdit m_DriverNatIDLogNo;
	long m_lDriverNatIDLogNo;
	// holds the driver's father name
	CEdit m_DriverFatherName;
	CString m_strDriverFatherName;
	// holds the driver's mother's name
	CEdit m_DriverMotherName;
	CString m_strDriverMotherName;
	// holds the blood type of the driver
	CComboBox m_DriverBloodType;
	CString m_strDriverBloodType;
	// holds the national ID issuing authority
	CEdit m_DriverNatIDIssueAuthority;
	CString m_strDriverNatIDIssueAuthority;

	static std::vector<const TCHAR*> m_CityList;
	static std::vector<const TCHAR*> m_BloodTypeList;

	const std::vector<std::pair<CString, long>>& m_DriverTypeList;
	std::unordered_map<long, long>& m_PosDriverTypeIdMap;
public:
	afx_msg void OnBnClickedOk();
};
