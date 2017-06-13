#include "stdafx.h"
#include "VisitInfo.h"


CVisitInfo::CVisitInfo() :
m_DriverName(_T("")),
m_KeeperName(_T("")),
m_Gate(_T(""))
{
}


CVisitInfo::~CVisitInfo()
{
}

void CVisitInfo::Prepare(CString _DriverName, CString _KeeperName, CString _Gate)
{
	m_DriverName = _DriverName;
	m_KeeperName = _KeeperName;
	m_Gate = _Gate;
}