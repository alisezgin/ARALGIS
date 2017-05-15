// DriverInfoSet.h : Implementation of the CVehicleInfoSet class



// CDriverInfoSet implementation

// code generated on Tuesday, May 9, 2017, 11:43 AM

#include "stdafx.h"
#include "DriverInfoSet.h"
IMPLEMENT_DYNAMIC(CDriverInfoSet, CRecordset)

CDriverInfoSet::CDriverInfoSet(CDatabase* pdb)
: CRecordset(pdb)
{
	m_dboDriverID = -1;
	m_dboDriverName = _T("");
	m_dboDriverLastName = _T("");
	m_dboDriverType = -1;
	m_dboDriverDriverLicenseIssueCity = _T("");
	m_dboDriverDriverLicenseIssueDistrict = _T("");
	m_dboDriverNatIDNo = _T("");
	m_dboDriverBirthPlace = _T("");
	m_dboDriverNatIDIssueCity = _T("");
	m_dboDriverNatIDIssueDistrict = _T("");
	m_dboDriverNatIDVolNo = -1;
	m_dboDriverNatIDRowNo = -1;
	m_dboDriverNatIDLogNo = -1;
	m_dboDriverFatherName = _T("");
	m_dboDriverMotherName = _T("");
	m_dboDriverBloodType = _T("");
	m_dboDriverIssueAuthority = _T("");
	m_dboDriverTypeDTID = -1;
	m_dboDriverTypeType = _T("");
	m_nFields = 20;
	m_nDefaultType = snapshot;
}

//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CDriverInfoSet::GetDefaultConnect()
{
	return _T("DSN=AralgisDB;Description=Working DB for ARALGIS project;Trusted_Connection=Yes;APP=Microsoft\x00ae Visual Studio\x00ae 2013;WSID=FUZYON-SW;DATABASE=ARALGISver0;");
}

CString CDriverInfoSet::GetDefaultSQL()
{
	return _T("[dbo].[Driver],[dbo].[DriverType]");
}

void CDriverInfoSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
	// Macros such as RFX_Text() and RFX_Int() are dependent on the
	// type of the member variable, not the type of the field in the database.
	// ODBC will try to automatically convert the column value to the requested type
	RFX_Long(pFX, _T("[dbo].[Driver].[ID]"), m_dboDriverID);
	RFX_Text(pFX, _T("[dbo].[Driver].[Name]"), m_dboDriverName);
	RFX_Text(pFX, _T("[dbo].[Driver].[LastName]"), m_dboDriverLastName);
	RFX_Long(pFX, _T("[dbo].[Driver].[Type]"), m_dboDriverType);
	RFX_Text(pFX, _T("[dbo].[Driver].[DriverLicenseIssueCity]"), m_dboDriverDriverLicenseIssueCity);
	RFX_Text(pFX, _T("[dbo].[Driver].[DriverLicenseIssueDistrict]"), m_dboDriverDriverLicenseIssueDistrict);
	RFX_Text(pFX, _T("[dbo].[Driver].[NatIDNo]"), m_dboDriverNatIDNo);
	RFX_Date(pFX, _T("[dbo].[Driver].[DateofBirth]"), m_dboDriverDateofBirth);
	RFX_Text(pFX, _T("[dbo].[Driver].[BirthPlace]"), m_dboDriverBirthPlace);
	RFX_Text(pFX, _T("[dbo].[Driver].[NatIDIssueCity]"), m_dboDriverNatIDIssueCity);
	RFX_Text(pFX, _T("[dbo].[Driver].[NatIDIssueDistrict]"), m_dboDriverNatIDIssueDistrict);
	RFX_Long(pFX, _T("[dbo].[Driver].[NatIDVolNo]"), m_dboDriverNatIDVolNo);
	RFX_Long(pFX, _T("[dbo].[Driver].[NatIDRowNo]"), m_dboDriverNatIDRowNo);
	RFX_Long(pFX, _T("[dbo].[Driver].[NatIDLogNo]"), m_dboDriverNatIDLogNo);
	RFX_Text(pFX, _T("[dbo].[Driver].[FatherName]"), m_dboDriverFatherName);
	RFX_Text(pFX, _T("[dbo].[Driver].[MotherName]"), m_dboDriverMotherName);
	RFX_Text(pFX, _T("[dbo].[Driver].[BloodType]"), m_dboDriverBloodType);
	RFX_Text(pFX, _T("[dbo].[Driver].[IssueAuthority]"), m_dboDriverIssueAuthority);
	RFX_Long(pFX, _T("[dbo].[DriverType].[DTID]"), m_dboDriverTypeDTID);
	RFX_Text(pFX, _T("[dbo].[DriverType].[Type]"), m_dboDriverTypeType);
}
/////////////////////////////////////////////////////////////////////////////
// CVehicleInfoSet diagnostics

#ifdef _DEBUG
void CDriverInfoSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CDriverInfoSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG

