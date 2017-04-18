// VehicleSet.h : Implementation of the CVehicleSet class



// CVehicleSet implementation

// code generated on Tuesday, April 18, 2017, 3:02 PM

#include "stdafx.h"
#include "VehicleSet.h"
IMPLEMENT_DYNAMIC(CVehicleSet, CRecordset)

CVehicleSet::CVehicleSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_VehicleID = 0;
	m_LicensePlate = L"";
	m_LicensePlateImage = L"";
	m_FrontViewImage = L"";
	m_ChassisBottomeImageCurrent = L"";
	m_ChassisBottomImageRef = L"";
	m_MostRecentVisitDate;
	m_nFields = 7;
	m_nDefaultType = dynaset;
}
// #error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CVehicleSet::GetDefaultConnect()
{
	return _T("DSN=AralgisDB;Description=Working DB for ARALGIS project;Trusted_Connection=Yes;APP=Microsoft\x00ae Visual Studio\x00ae 2013;WSID=FUZYON-SW;DATABASE=AliDummy;");
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
	RFX_BigInt(pFX, _T("[VehicleID]"), m_VehicleID);
	RFX_Text(pFX, _T("[LicensePlate]"), m_LicensePlate);
	RFX_Text(pFX, _T("[LicensePlateImage]"), m_LicensePlateImage);
	RFX_Text(pFX, _T("[FrontViewImage]"), m_FrontViewImage);
	RFX_Text(pFX, _T("[ChassisBottomeImageCurrent]"), m_ChassisBottomeImageCurrent);
	RFX_Text(pFX, _T("[ChassisBottomImageRef]"), m_ChassisBottomImageRef);
	RFX_Date(pFX, _T("[MostRecentVisitDate]"), m_MostRecentVisitDate);

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


