// C:\ali\github-home\ARALGIS\ARALGIS\Database\HeaderFiles\DriverTypeSet.h : Implementation of the CDriverTypeSet class



// CDriverTypeSet implementation

// code generated on 24 Temmuz 2017 Pazartesi, 16:04

#include "stdafx.h"
#include "DriverTypeSet.h"
IMPLEMENT_DYNAMIC(CDriverTypeSet, CRecordset)

CDriverTypeSet::CDriverTypeSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_DTID = 0;
	m_Type = L"";
	m_nFields = 2;
	m_nDefaultType = dynaset;
}

// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CDriverTypeSet::GetDefaultConnect()
{
	return _T("DSN=ARALGISDB;Description=DB for ARALGISver0;Trusted_Connection=Yes;APP=Microsoft\x00ae Visual Studio\x00ae 2013;WSID=DESKTOP-5I084ND;DATABASE=ARALGISver0;");
}

CString CDriverTypeSet::GetDefaultSQL()
{
	return _T("[dbo].[DriverType]");
}

void CDriverTypeSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Int() are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long(pFX, _T("[DTID]"), m_DTID);
	RFX_Text(pFX, _T("[Type]"), m_Type);

}
/////////////////////////////////////////////////////////////////////////////
// CDriverTypeSet diagnostics

#ifdef _DEBUG
void CDriverTypeSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CDriverTypeSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG


