// Gate.h : Implementation of the CGate class



// CGate implementation

// code generated on Tuesday, May 9, 2017, 10:55 AM

#include "stdafx.h"
#include "GateSet.h"
IMPLEMENT_DYNAMIC(CGateSet, CRecordset)

CGateSet::CGateSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_GateID = 0;
	m_GateType = L"";
	m_GatePCName = L"";
	m_GateIP = L"";
	m_Notes = L"";
	m_nFields = 5;
	m_nDefaultType = snapshot;
}

//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CGateSet::GetDefaultConnect()
{
	return _T("DSN=AralgisDB;Description=Working DB for ARALGIS project;Trusted_Connection=Yes;APP=Microsoft\x00ae Visual Studio\x00ae 2013;WSID=FUZYON-SW;DATABASE=ARALGISver0;");
}

CString CGateSet::GetDefaultSQL()
{
	return _T("[dbo].[Gate]");
}

void CGateSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Int() are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long(pFX, _T("[ID]"), m_GateID);
	RFX_Text(pFX, _T("[Type]"), m_GateType);
	RFX_Text(pFX, _T("[PCName]"), m_GatePCName);
	RFX_Text(pFX, _T("[IP]"), m_GateIP);
	RFX_Text(pFX, _T("[Notes]"), m_Notes);

}
/////////////////////////////////////////////////////////////////////////////
// CGate diagnostics

#ifdef _DEBUG
void CGateSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CGateSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG


