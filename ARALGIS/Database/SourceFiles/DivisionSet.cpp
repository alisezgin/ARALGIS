// C:\ali\github-home\ARALGIS\ARALGIS\Database\HeaderFiles\DivisionSet.h : Implementation of the CDivisionSet class



// CDivisionSet implementation

// code generated on 30 Temmuz 2017 Pazar, 15:36

#include "stdafx.h"
#include "DivisionSet.h"
IMPLEMENT_DYNAMIC(CDivisionSet, CRecordset)

CDivisionSet::CDivisionSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_DivID = 0;
	m_Type = L"";
	m_nFields = 2;
	m_nDefaultType = dynaset;
}

// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CDivisionSet::GetDefaultConnect()
{
	return _T("DSN=ARALGISDB;Description=DB for ARALGISver0;Trusted_Connection=Yes;APP=Microsoft\x00ae Visual Studio\x00ae 2013;WSID=DESKTOP-5I084ND;DATABASE=ARALGISver0;");
}

CString CDivisionSet::GetDefaultSQL()
{
	return _T("[dbo].[DivisionType]");
}

void CDivisionSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Int() are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long(pFX, _T("[DivID]"), m_DivID);
	RFX_Text(pFX, _T("[Type]"), m_Type);

}
/////////////////////////////////////////////////////////////////////////////
// CDivisionSet diagnostics

#ifdef _DEBUG
void CDivisionSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CDivisionSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG


