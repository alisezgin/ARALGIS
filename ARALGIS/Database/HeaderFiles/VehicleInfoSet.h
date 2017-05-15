// VehicleInfoSet.h : Declaration of the CVehicleInfoSet

#pragma once

// code generated on Tuesday, May 9, 2017, 11:43 AM

class CVehicleInfoSet : public CRecordset
{
public:
	CVehicleInfoSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CVehicleInfoSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	long	m_dboVehicleID;
	CString	m_dboVehicleLicensePlate;
	long	m_dboVehicleType;
	CString	m_dboVehicleRegistrationName;
	CTime	m_dboVehicleRegistrationDate;
	CString	m_dboVehicleRegistrationNumber;
	CString	m_dboVehicleLicensePlateImageFile;
	CString	m_dboVehicleFrontViewImageFile;
	CString	m_dboVehicleChassisBottomReferenceImageFile;
	CString	m_dboVehicleChassisBottomLastImageFile;
	long	m_dboVehicleTypeVTID;
	CString	m_dboVehicleTypeType;

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


