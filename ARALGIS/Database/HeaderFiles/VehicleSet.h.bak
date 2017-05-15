// VehicleSet.h : Declaration of the CVehicleSet

#pragma once

// code generated on Tuesday, April 18, 2017, 3:02 PM

class CVehicleSet : public CRecordset
{
public:
	CVehicleSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CVehicleSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	LONG		m_VehicleID;
	CString		m_LicensePlate;
	CString		m_LicensePlateImage;
	CString		m_FrontViewImage;
	CString		m_ChassisBottomImageCurrent;
	CString		m_ChassisBottomImageRef;
	CTime		m_MostRecentVisitDate;
	CString		m_DriverName;
	BOOL		m_BlackList;

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


