// UserInfoSet.h : Implementation of the CVehicleInfoSet class



// CUserInfoSet implementation

// code generated on Tuesday, May 9, 2017, 11:43 AM

#include "stdafx.h"
#include "UserInfoSet.h"
IMPLEMENT_DYNAMIC(CUserInfoSet, CRecordset)

CUserInfoSet::CUserInfoSet(CDatabase* pdb)
: CRecordset(pdb)
{
	m_dboUserID = -1;
	m_dboUserName = _T("");
	m_dboUserLastName = _T("");
	m_dboUserType = -1;
	m_dboUserNickname = _T("");
	m_dboUserPassword = _T("");
	m_dboUserOldPassword = _T("");
	m_dboUserIsActive = FALSE;
	m_dboUserLogUserID = -1;
	m_dboUserLogNickname = _T("");
	m_dboUserTypeUTID = -1;
	m_dboUserTypeType = _T("");
	m_nFields = 16;
	m_nDefaultType = snapshot;
}

//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CUserInfoSet::GetDefaultConnect()
{
	return _T("DSN=AralgisDB;Description=Working DB for ARALGIS project;Trusted_Connection=Yes;APP=Microsoft\x00ae Visual Studio\x00ae 2013;WSID=FUZYON-SW;DATABASE=ARALGISver0;");
}

CString CUserInfoSet::GetDefaultSQL()
{
	return _T("[dbo].[User],[dbo].[UserLog],[dbo].[UserType]");
}

void CUserInfoSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
	// Macros such as RFX_Text() and RFX_Int() are dependent on the
	// type of the member variable, not the type of the field in the database.
	// ODBC will try to automatically convert the column value to the requested type
	RFX_Long(pFX, _T("[User].[ID]"), m_dboUserID);
	RFX_Text(pFX, _T("[User].[Name]"), m_dboUserName);
	RFX_Text(pFX, _T("[User].[LastName]"), m_dboUserLastName);
	RFX_Long(pFX, _T("[User].[Type]"), m_dboUserType);
	RFX_Text(pFX, _T("[User].[Nickname]"), m_dboUserNickname);
	RFX_Date(pFX, _T("[User].[RegistrationDateTime]"), m_dboUserRegistrationDateTime);
	RFX_Text(pFX, _T("[User].[Password]"), m_dboUserPassword);
	RFX_Text(pFX, _T("[User].[OldPassword]"), m_dboUserOldPassword);
	RFX_Bool(pFX, _T("[User].[IsActive]"), m_dboUserIsActive);
	RFX_Date(pFX, _T("[User].[ExpungeDateTime]"), m_dboUserExpungeDateTime);
	RFX_Long(pFX, _T("[UserLog].[UserID]"), m_dboUserLogUserID);
	RFX_Text(pFX, _T("[UserLog].[Nickname]"), m_dboUserLogNickname);
	RFX_Date(pFX, _T("[UserLog].[LoginDateTime]"), m_dboUserLogLoginDateTime);
	RFX_Date(pFX, _T("[UserLog].[LogoffDateTime]"), m_dboUserLogLogoffDateTime);
	RFX_Long(pFX, _T("[UserType].[UTID]"), m_dboUserTypeUTID);
	RFX_Text(pFX, _T("[UserType].[Type]"), m_dboUserTypeType);
}
/////////////////////////////////////////////////////////////////////////////
// CVehicleInfoSet diagnostics

#ifdef _DEBUG
void CUserInfoSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CUserInfoSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG

