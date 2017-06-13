#pragma once
class CVisitInfo
{
public:
	CVisitInfo();
	~CVisitInfo();

	void Prepare(CString _DriverName, CString _KeeperName, CString _Gate);
	CString GetDriver() { return m_DriverName; }
	CString GetKeeper() { return m_KeeperName; }
	CString GetGate() { return m_Gate; }

private:
	CString m_DriverName;
	CString m_KeeperName;
	CString m_Gate;
};

