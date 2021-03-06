// VehicleSet.h : Declaration of the CVehicleSet

#pragma once

// code generated on Tuesday, May 9, 2017, 2:22 PM

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

	long	m_VehicleID;
	CString	m_VehicleLicensePlate;
	long	m_VehicleType;
	CString	m_VehicleRegistrationName;
	CTime	m_VehicleRegistrationDate;
	CString	m_VehicleRegistrationNumber;
	CString	m_VehicleLicensePlateImageFile;
	CString	m_VehicleFrontViewImageFile;
	CString	m_VehicleChassisBottomReferenceImageFile;
	CString	m_VehicleChassisBottomLastImageFile;

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


