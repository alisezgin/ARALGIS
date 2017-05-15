// UserTable.h : Implementation of the CUserTable class



// CUserTable implementation

// code generated on Monday, April 10, 2017, 3:42 PM

#include "stdafx.h"
#include "UserTableSet.h"

IMPLEMENT_DYNAMIC(CUserTableSet, CRecordset)

CUserTableSet::CUserTableSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_UserID = 0;
	m_FirstName = "";
	m_LastName = "";
	m_GroupID = 0;
	m_UserName = "";
	m_RegDate;
	m_DelDate;
	m_Password = "";
	m_PasswordOld = "";
	m_Status = FALSE;
	m_nFields = 10;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CUserTableSet::GetDefaultConnect()
{
	return _T("DSN=AralgisDB;Description=Working DB for ARALGIS project;Trusted_Connection=Yes;APP=Microsoft\x00ae Visual Studio\x00ae 2013;WSID=FUZYON-SW;DATABASE=AliDummy;");
}

CString CUserTableSet::GetDefaultSQL()
{
	return _T("[dbo].[UserTable]");
}

void CUserTableSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Int() are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long(pFX, _T("[UserID]"), m_UserID);
	RFX_Text(pFX, _T("[FirstName]"), m_FirstName);
	RFX_Text(pFX, _T("[LastName]"), m_LastName);
	RFX_Long(pFX, _T("[GroupID]"), m_GroupID);
	RFX_Text(pFX, _T("[UserName]"), m_UserName);
	RFX_Date(pFX, _T("[RegDate]"), m_RegDate);
	RFX_Date(pFX, _T("[DelDate]"), m_DelDate);
	RFX_Text(pFX, _T("[Password]"), m_Password);
	RFX_Text(pFX, _T("[PasswordOld]"), m_PasswordOld);
	RFX_Bool(pFX, _T("[Status]"), m_Status);

}
/////////////////////////////////////////////////////////////////////////////
// CUserTable diagnostics

#ifdef _DEBUG
void CUserTableSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CUserTableSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG


