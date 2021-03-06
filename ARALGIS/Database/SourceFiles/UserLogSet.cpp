// C:\ali\github-home\ARALGIS\ARALGIS\Database\HeaderFiles\UserLog.h : Implementation of the CUserLogSet class



// CUserLogSet implementation

// code generated on 26 Temmuz 2017 Çarşamba, 14:57

#include "stdafx.h"
#include "UserLogSet.h"
IMPLEMENT_DYNAMIC(CUserLogSet, CRecordset)

CUserLogSet::CUserLogSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_UserID = 0;
	m_Nickname = L"";
	m_LoginDateTime;
	m_LogoffDateTime;
	m_nFields = 4;
	m_nDefaultType = dynaset;
}

// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CUserLogSet::GetDefaultConnect()
{
	return _T("DSN=ARALGISDB;Description=DB for ARALGISver0;Trusted_Connection=Yes;APP=Microsoft\x00ae Visual Studio\x00ae 2013;WSID=DESKTOP-5I084ND;DATABASE=ARALGISver0;");
}

CString CUserLogSet::GetDefaultSQL()
{
	return _T("[dbo].[UserLog]");
}

void CUserLogSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Int() are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long(pFX, _T("[UserID]"), m_UserID);
	RFX_Text(pFX, _T("[Nickname]"), m_Nickname);
	RFX_Date(pFX, _T("[LoginDateTime]"), m_LoginDateTime);
	RFX_Date(pFX, _T("[LogoffDateTime]"), m_LogoffDateTime);

}
/////////////////////////////////////////////////////////////////////////////
// CUserLogSet diagnostics

#ifdef _DEBUG
void CUserLogSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CUserLogSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG


