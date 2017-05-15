// VehiclePassageSet.h : Declaration of the CVehiclePassageSet

#pragma once

// code generated on Tuesday, May 9, 2017, 11:56 AM

class CVehiclePassageSet : public CRecordset
{
public:
	CVehiclePassageSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CVehiclePassageSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	long	m_VehiclePassageVehicleID;
	CString	m_VehiclePassageLicensePlate;
	CTime	m_VehiclePassageEntryDateTime;
	CTime	m_VehiclePassageExitDateTime;
	long	m_VehiclePassageUserID;
	long	m_VehiclePassageGateID;
	long	m_VehiclePassageDriverID;
	BOOL	m_VehiclePassagePermissionGranted;

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


