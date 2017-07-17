// C:\ali\github-home\ARALGIS\ARALGIS\Database\HeaderFiles\VehicleType.h : Implementation of the CVehicleType class



// CVehicleType implementation

// code generated on 14 Temmuz 2017 Cuma, 11:30

#include "stdafx.h"
#include "VehicleTypeSet.h"
IMPLEMENT_DYNAMIC(CVehicleTypeSet, CRecordset)

CVehicleTypeSet::CVehicleTypeSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_VTID = 0;
	m_Type = L"";
	m_nFields = 2;
	m_nDefaultType = dynaset;
}

// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CVehicleTypeSet::GetDefaultConnect()
{
	return _T("DSN=ARALGISDB;Description=DB for ARALGISver0;Trusted_Connection=Yes;APP=Microsoft\x00ae Visual Studio\x00ae 2013;WSID=DESKTOP-5I084ND;DATABASE=ARALGISver0;");
}

CString CVehicleTypeSet::GetDefaultSQL()
{
	return _T("[dbo].[VehicleType]");
}

void CVehicleTypeSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Int() are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long(pFX, _T("[VTID]"), m_VTID);
	RFX_Text(pFX, _T("[Type]"), m_Type);

}
/////////////////////////////////////////////////////////////////////////////
// CVehicleType diagnostics

#ifdef _DEBUG
void CVehicleTypeSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CVehicleTypeSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG


