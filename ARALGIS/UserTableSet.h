// UserTable.h : Declaration of the CUserTable

#pragma once

// code generated on Monday, April 10, 2017, 3:42 PM

class CUserTableSet : public CRecordset
{
public:
	CUserTableSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CUserTableSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	// database table variables
	long		m_UserID;
	CStringA	m_FirstName;
	CStringA	m_LastName;
	long		m_GroupID;
	CStringA	m_UserName;
	CTime		m_RegDate;
	CTime		m_DelDate;
	CStringA	m_Password;
	CStringA	m_PasswordOld;
	BOOL		m_Status;

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


