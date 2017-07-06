// C:\ali\github-home\ARALGIS\ARALGIS\Database\HeaderFiles\UserSet.h : Implementation of the CUserSet class



// CUserSet implementation

// code generated on 12 Haziran 2017 Pazartesi, 15:47

#include "stdafx.h"
#include "UserSet.h"
IMPLEMENT_DYNAMIC(CUserSet, CRecordset)

CUserSet::CUserSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_ID = 0;
	m_Name = L"";
	m_LastName = L"";
	m_Type = 0;
	m_Nickname = L"";
	m_RegistrationDateTime;
	m_Password = L"";
	m_OldPassword = L"";
	m_IsActive = FALSE;
	m_ExpungeDateTime;
	m_nFields = 10;
	m_nDefaultType = dynaset;
}

//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CUserSet::GetDefaultConnect()
{
	return _T("DSN=ARALGISDB;Description=DB for ARALGISver0;Trusted_Connection=Yes;APP=Microsoft\x00ae Visual Studio\x00ae 2013;WSID=DESKTOP-5I084ND;DATABASE=ARALGISver0;");
}

CString CUserSet::GetDefaultSQL()
{
	return _T("[dbo].[User]");
}

void CUserSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Int() are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long(pFX, _T("[ID]"), m_ID);
	RFX_Text(pFX, _T("[Name]"), m_Name);
	RFX_Text(pFX, _T("[LastName]"), m_LastName);
	RFX_Long(pFX, _T("[Type]"), m_Type);
	RFX_Text(pFX, _T("[Nickname]"), m_Nickname);
	RFX_Date(pFX, _T("[RegistrationDateTime]"), m_RegistrationDateTime);
	RFX_Text(pFX, _T("[Password]"), m_Password);
	RFX_Text(pFX, _T("[OldPassword]"), m_OldPassword);
	RFX_Bool(pFX, _T("[IsActive]"), m_IsActive);
	RFX_Date(pFX, _T("[ExpungeDateTime]"), m_ExpungeDateTime);

}
/////////////////////////////////////////////////////////////////////////////
// CUserSet diagnostics

#ifdef _DEBUG
void CUserSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CUserSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG


