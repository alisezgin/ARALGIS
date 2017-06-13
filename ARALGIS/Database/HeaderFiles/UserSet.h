// C:\ali\github-home\ARALGIS\ARALGIS\Database\HeaderFiles\UserSet.h : Declaration of the CUserSet

#pragma once

// code generated on 12 Haziran 2017 Pazartesi, 15:47

class CUserSet : public CRecordset
{
public:
	CUserSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CUserSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	long	m_ID;
	CString m_Name;
	CString m_LastName;
	long	m_Type;
	CString	m_Nickname;
	CTime	m_RegistrationDateTime;
	CString	m_Password;
	CString	m_OldPassword;
	BOOL	m_IsActive;
	CTime	m_ExpungeDateTime;

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


