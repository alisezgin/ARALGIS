// DriverSet.h : Declaration of the CDriverSet

#pragma once

// code generated on 18 Mayıs 2017 Perşembe, 11:35

class CDriverSet : public CRecordset
{
public:
	CDriverSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CDriverSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	long	m_ID;
	CString	m_Name;
	CString	m_LastName;
	long	m_Type;
	CString	m_DriverLicenseIssueCity;
	CString	m_DriverLicenseIssueDistrict;
	CString	m_NatIDNo;
	COleDateTime	m_DateofBirth;
	CString	m_BirthPlace;
	CString	m_NatIDIssueCity;
	CString	m_NatIDIssueDistrict;
	long	m_NatIDVolNo;
	long	m_NatIDRowNo;
	long	m_NatIDLogNo;
	CString	m_FatherName;
	CString	m_MotherName;
	CString	m_BloodType;
	CString	m_IssueAuthority;

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


