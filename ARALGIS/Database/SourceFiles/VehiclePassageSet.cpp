// VehiclePassageSet.h : Implementation of the CVehiclePassageSet class



// CVehicleSet implementation

// code generated on Tuesday, May 9, 2017, 11:08 AM

#include "stdafx.h"
#include "VehiclePassageSet.h"
IMPLEMENT_DYNAMIC(CVehiclePassageSet, CRecordset)

CVehiclePassageSet::CVehiclePassageSet(CDatabase* pdb)
: CRecordset(pdb)
{
	m_VehiclePassageVehicleID = -1;
	m_VehiclePassageLicensePlate = _T("");
	m_VehiclePassageUserID = -1;
	m_VehiclePassageGateID = -1;
	m_VehiclePassageDriverID = -1;
	m_VehiclePassagePermissionGranted = FALSE;
	m_nFields = 8;
	m_nDefaultType = snapshot;
}

//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CVehiclePassageSet::GetDefaultConnect()
{
	return _T("DSN=AralgisDB;Description=Working DB for ARALGIS project;Trusted_Connection=Yes;APP=Microsoft\x00ae Visual Studio\x00ae 2013;WSID=FUZYON-SW;DATABASE=ARALGISver0;");
}

CString CVehiclePassageSet::GetDefaultSQL()
{
	return _T("[dbo].[VehiclePassage]");
}

void CVehiclePassageSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
	// Macros such as RFX_Text() and RFX_Int() are dependent on the
	// type of the member variable, not the type of the field in the database.
	// ODBC will try to automatically convert the column value to the requested type
	RFX_Long(pFX, _T("[VehicleID]"), m_VehiclePassageVehicleID);
	RFX_Text(pFX, _T("[LicensePlate]"), m_VehiclePassageLicensePlate);
	RFX_Date(pFX, _T("[EntryDateTime]"), m_VehiclePassageEntryDateTime);
	RFX_Date(pFX, _T("[ExitDateTime]"), m_VehiclePassageExitDateTime);
	RFX_Long(pFX, _T("[UserID]"), m_VehiclePassageUserID);
	RFX_Long(pFX, _T("[GateID]"), m_VehiclePassageGateID);
	RFX_Long(pFX, _T("[DriverID]"), m_VehiclePassageDriverID);
	RFX_Bool(pFX, _T("[PermissionGranted]"), m_VehiclePassagePermissionGranted);

}
/////////////////////////////////////////////////////////////////////////////
// CVehicleSet diagnostics

#ifdef _DEBUG
void CVehiclePassageSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CVehiclePassageSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG


