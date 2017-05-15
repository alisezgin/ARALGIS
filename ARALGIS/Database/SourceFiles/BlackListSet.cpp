// BlackListSet.h : Implementation of the CBlackListSet class



// CBlackListSet implementation

// code generated on Tuesday, May 9, 2017, 11:35 AM

#include "stdafx.h"
#include "BlackListSet.h"
IMPLEMENT_DYNAMIC(CBlackListSet, CRecordset)

CBlackListSet::CBlackListSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_BlackListID = -1;
	m_BlackListLicensePlate = L"";
	m_BlackListInclusionDate;
	m_BlackListExclusionDate;
	m_BlackListExplanation = L"";
	m_nFields = 5;
	m_nDefaultType = snapshot;
}

// #error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CBlackListSet::GetDefaultConnect()
{
	return _T("DSN=AralgisDB;Description=Working DB for ARALGIS project;Trusted_Connection=Yes;APP=Microsoft\x00ae Visual Studio\x00ae 2013;WSID=FUZYON-SW;DATABASE=ARALGISver0;");
}

CString CBlackListSet::GetDefaultSQL()
{
	return _T("[dbo].[BlackList]");
}

void CBlackListSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Int() are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long(pFX, _T("[ID]"), m_BlackListID);
	RFX_Text(pFX, _T("[LicensePlate]"), m_BlackListLicensePlate);
	RFX_Date(pFX, _T("[InclusionDate]"), m_BlackListInclusionDate);
	RFX_Date(pFX, _T("[ExclusionDate]"), m_BlackListExclusionDate);
	RFX_Text(pFX, _T("[Explanation]"), m_BlackListExplanation);

}
/////////////////////////////////////////////////////////////////////////////
// CBlackListSet diagnostics

#ifdef _DEBUG
void CBlackListSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CBlackListSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG


