#include "stdafx.h"
#include "VisitInfo.h"


CVisitInfo::CVisitInfo() :
m_DriverName(_T("")),
m_KeeperName(_T("")),
m_Gate(_T("")),
m_Division(_T("")),
m_VehicleType(_T(""))
{
}


CVisitInfo::~CVisitInfo()
{
}

void CVisitInfo::Prepare(CString _DriverName, CString _KeeperName, CString _Gate, CString _Division, CString _VehicleType)
{
	m_DriverName = _DriverName;
	m_KeeperName = _KeeperName;
	m_Gate = _Gate;
	m_Division = _Division;
	m_VehicleType = _VehicleType;
}

void CVisitInfo::Prepare(long _DriverID, long _UserID, long _GateID, long _DivisionID, long _VehicleTypeID)
{
	m_DriverID = _DriverID;
	m_UserID = _UserID;
	m_GateID = _GateID;
	m_DivisionID = _DivisionID;
	m_VehicleTypeID = _VehicleTypeID;
}