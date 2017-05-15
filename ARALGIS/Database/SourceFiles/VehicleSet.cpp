// VehicleSet.h : Implementation of the CVehicleSet class



// CVehicleSet implementation

// code generated on Tuesday, May 9, 2017, 11:08 AM

#include "stdafx.h"
#include "VehicleSet.h"
IMPLEMENT_DYNAMIC(CVehicleSet, CRecordset)

CVehicleSet::CVehicleSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_VehicleID = -1;
	m_VehicleLicensePlate = L"";
	m_VehicleType = -1;
	m_VehicleRegistrationName = L"";
	m_VehicleRegistrationDate;
	m_VehicleRegistrationNumber = L"";
	m_VehicleLicensePlateImageFile = L"";
	m_VehicleFrontViewImageFile = L"";
	m_VehicleChassisBottomReferenceImageFile = L"";
	m_VehicleChassisBottomLastImageFile = L"";
	m_nFields = 10;
	m_nDefaultType = snapshot;
}

//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CVehicleSet::GetDefaultConnect()
{
	return _T("DSN=AralgisDB;Description=Working DB for ARALGIS project;Trusted_Connection=Yes;APP=Microsoft\x00ae Visual Studio\x00ae 2013;WSID=FUZYON-SW;DATABASE=ARALGISver0;");
}

CString CVehicleSet::GetDefaultSQL()
{
	return _T("[dbo].[Vehicle]");
}

void CVehicleSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Int() are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long(pFX, _T("[ID]"), m_VehicleID);
	RFX_Text(pFX, _T("[LicensePlate]"), m_VehicleLicensePlate);
	RFX_Long(pFX, _T("[Type]"), m_VehicleType);
	RFX_Text(pFX, _T("[RegistrationName]"), m_VehicleRegistrationName);
	RFX_Date(pFX, _T("[RegistrationDate]"), m_VehicleRegistrationDate);
	RFX_Text(pFX, _T("[RegistrationNumber]"), m_VehicleRegistrationNumber);
	RFX_Text(pFX, _T("[LicensePlateImageFile]"), m_VehicleLicensePlateImageFile);
	RFX_Text(pFX, _T("[FrontViewImageFile]"), m_VehicleFrontViewImageFile);
	RFX_Text(pFX, _T("[ChassisBottomReferenceImageFile]"), m_VehicleChassisBottomReferenceImageFile);
	RFX_Text(pFX, _T("[ChassisBottomLastImageFile]"), m_VehicleChassisBottomLastImageFile);

}
/////////////////////////////////////////////////////////////////////////////
// CVehicleSet diagnostics

#ifdef _DEBUG
void CVehicleSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CVehicleSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG


