// VehicleInfoSet.h : Implementation of the CVehicleInfoSet class



// CVehicleInfoSet implementation

// code generated on Tuesday, May 9, 2017, 11:43 AM

#include "stdafx.h"
#include "VehicleInfoSet.h"
IMPLEMENT_DYNAMIC(CVehicleInfoSet, CRecordset)

CVehicleInfoSet::CVehicleInfoSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_dboVehicleID = -1;
	m_dboVehicleLicensePlate = L"";
	m_dboVehicleType = -1;
	m_dboVehicleRegistrationName = L"";
	m_dboVehicleRegistrationDate;
	m_dboVehicleRegistrationNumber = L"";
	m_dboVehicleLicensePlateImageFile = L"";
	m_dboVehicleFrontViewImageFile = L"";
	m_dboVehicleChassisBottomReferenceImageFile = L"";
	m_dboVehicleChassisBottomLastImageFile = L"";
	m_dboVehicleTypeVTID = -1;
	m_dboVehicleTypeType = L"";
	m_nFields = 12;
	m_nDefaultType = snapshot;
}

//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CVehicleInfoSet::GetDefaultConnect()
{
	return _T("DSN=AralgisDB;Description=Working DB for ARALGIS project;Trusted_Connection=Yes;APP=Microsoft\x00ae Visual Studio\x00ae 2013;WSID=FUZYON-SW;DATABASE=ARALGISver0;");
}

CString CVehicleInfoSet::GetDefaultSQL()
{
	return _T("[dbo].[Vehicle],[dbo].[VehicleType]");
}

void CVehicleInfoSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Int() are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long(pFX, _T("[dbo].[Vehicle].[ID]"), m_dboVehicleID);
	RFX_Text(pFX, _T("[dbo].[Vehicle].[LicensePlate]"), m_dboVehicleLicensePlate);
	RFX_Long(pFX, _T("[dbo].[Vehicle].[Type]"), m_dboVehicleType);
	RFX_Text(pFX, _T("[dbo].[Vehicle].[RegistrationName]"), m_dboVehicleRegistrationName);
	RFX_Date(pFX, _T("[dbo].[Vehicle].[RegistrationDate]"), m_dboVehicleRegistrationDate);
	RFX_Text(pFX, _T("[dbo].[Vehicle].[RegistrationNumber]"), m_dboVehicleRegistrationNumber);
	RFX_Text(pFX, _T("[dbo].[Vehicle].[LicensePlateImageFile]"), m_dboVehicleLicensePlateImageFile);
	RFX_Text(pFX, _T("[dbo].[Vehicle].[FrontViewImageFile]"), m_dboVehicleFrontViewImageFile);
	RFX_Text(pFX, _T("[dbo].[Vehicle].[ChassisBottomReferenceImageFile]"), m_dboVehicleChassisBottomReferenceImageFile);
	RFX_Text(pFX, _T("[dbo].[Vehicle].[ChassisBottomLastImageFile]"), m_dboVehicleChassisBottomLastImageFile);
	RFX_Long(pFX, _T("[dbo].[VehicleType].[VTID]"), m_dboVehicleTypeVTID);
	RFX_Text(pFX, _T("[dbo].[VehicleType].[Type]"), m_dboVehicleTypeType);

}
/////////////////////////////////////////////////////////////////////////////
// CVehicleInfoSet diagnostics

#ifdef _DEBUG
void CVehicleInfoSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CVehicleInfoSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG


