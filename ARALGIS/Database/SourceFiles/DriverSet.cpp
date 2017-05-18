// DriveSet.h : Implementation of the CDriverSett class



// CDriverSett implementation

// code generated on 18 Mayýs 2017 Perþembe, 11:40

#include "stdafx.h"
#include "DriverSet.h"
IMPLEMENT_DYNAMIC(CDriverSet, CRecordset)

CDriverSet::CDriverSet(CDatabase* pdb)
: CRecordset(pdb)
{
	m_ID = 0;
	m_Name = L"";
	m_LastName = L"";
	m_Type = 0;
	m_DriverLicenseIssueCity = L"";
	m_DriverLicenseIssueDistrict = L"";
	m_NatIDNo = L"";
	m_DateofBirth;
	m_BirthPlace = L"";
	m_NatIDIssueCity = L"";
	m_NatIDIssueDistrict = L"";
	m_NatIDVolNo = 0;
	m_NatIDRowNo = 0;
	m_NatIDLogNo = 0;
	m_FatherName = L"";
	m_MotherName = L"";
	m_BloodType = L"";
	m_IssueAuthority = L"";
	m_nFields = 18;
	m_nDefaultType = snapshot;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CDriverSet::GetDefaultConnect()
{
	return _T("DSN=ARALGISDB;Description=DB for ARALGISver0;Trusted_Connection=Yes;APP=Microsoft\x00ae Visual Studio\x00ae 2013;WSID=DESKTOP-5I084ND;DATABASE=ARALGISver0;");
}

CString CDriverSet::GetDefaultSQL()
{
	return _T("[dbo].[Driver]");
}

void CDriverSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
	// Macros such as RFX_Text() and RFX_Int() are dependent on the
	// type of the member variable, not the type of the field in the database.
	// ODBC will try to automatically convert the column value to the requested type
	RFX_Long(pFX, _T("[ID]"), m_ID);
	RFX_Text(pFX, _T("[Name]"), m_Name);
	RFX_Text(pFX, _T("[LastName]"), m_LastName);
	RFX_Long(pFX, _T("[Type]"), m_Type);
	RFX_Text(pFX, _T("[DriverLicenseIssueCity]"), m_DriverLicenseIssueCity);
	RFX_Text(pFX, _T("[DriverLicenseIssueDistrict]"), m_DriverLicenseIssueDistrict);
	RFX_Text(pFX, _T("[NatIDNo]"), m_NatIDNo);
	RFX_Date(pFX, _T("[DateofBirth]"), m_DateofBirth);
	RFX_Text(pFX, _T("[BirthPlace]"), m_BirthPlace);
	RFX_Text(pFX, _T("[NatIDIssueCity]"), m_NatIDIssueCity);
	RFX_Text(pFX, _T("[NatIDIssueDistrict]"), m_NatIDIssueDistrict);
	RFX_Long(pFX, _T("[NatIDVolNo]"), m_NatIDVolNo);
	RFX_Long(pFX, _T("[NatIDRowNo]"), m_NatIDRowNo);
	RFX_Long(pFX, _T("[NatIDLogNo]"), m_NatIDLogNo);
	RFX_Text(pFX, _T("[FatherName]"), m_FatherName);
	RFX_Text(pFX, _T("[MotherName]"), m_MotherName);
	RFX_Text(pFX, _T("[BloodType]"), m_BloodType);
	RFX_Text(pFX, _T("[IssueAuthority]"), m_IssueAuthority);

}
/////////////////////////////////////////////////////////////////////////////
// CDriverSett diagnostics

#ifdef _DEBUG
void CDriverSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CDriverSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG


