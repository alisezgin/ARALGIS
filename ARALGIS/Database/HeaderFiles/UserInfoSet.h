// UserInfoSet.h : Declaration of the CUserInfoSet

#pragma once

// code generated on Tuesday, May 9, 2017, 11:38 AM

class CUserInfoSet : public CRecordset
{
public:
	CUserInfoSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CUserInfoSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	long	m_dboUserID;
	CString	m_dboUserName;
	CString	m_dboUserLastName;
	long	m_dboUserType;
	CString	m_dboUserNickname;
	CTime	m_dboUserRegistrationDateTime;
	CString	m_dboUserPassword;
	CString	m_dboUserOldPassword;
	BOOL	m_dboUserIsActive;
	CTime	m_dboUserExpungeDateTime;
	long	m_dboUserLogUserID;
	CString	m_dboUserLogNickname;
	CTime	m_dboUserLogLoginDateTime;
	CTime	m_dboUserLogLogoffDateTime;
	long	m_dboUserTypeUTID;
	CString	m_dboUserTypeType;

// Overrides
	// Wizard generated virtual function overrides
	public:
	virtual CString GetDefaultConnect();	// Default connection string

	virtual CString GetDefaultSQL(); 	// default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);	// RFX support

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

};


