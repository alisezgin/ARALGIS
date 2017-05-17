// DriverInfoSet.h : Declaration of the CDriverInfoSet

#pragma once

// code generated on Tuesday, May 9, 2017, 11:44 AM

class CDriverInfoSet : public CRecordset
{
public:
	CDriverInfoSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CDriverInfoSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	long	m_dboDriverID;
	CString	m_dboDriverName;
	CString	m_dboDriverLastName;
	long  m_dboDriverType;
	CString	m_dboDriverDriverLicenseIssueCity;
	CString	m_dboDriverDriverLicenseIssueDistrict;
	CString	m_dboDriverNatIDNo;
	COleDateTime m_dboDriverDateofBirth;
	// CTime	m_dboDriverDateofBirth;
	CString	m_dboDriverBirthPlace;
	CString	m_dboDriverNatIDIssueCity;
	CString	m_dboDriverNatIDIssueDistrict;
	long	m_dboDriverNatIDVolNo;
	long	m_dboDriverNatIDRowNo;
	long	m_dboDriverNatIDLogNo;
	CString	m_dboDriverFatherName;
	CString	m_dboDriverMotherName;
	CString	m_dboDriverBloodType;
	CString	m_dboDriverIssueAuthority;
	long	m_dboDriverTypeDTID;
	CString	m_dboDriverTypeType;

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


