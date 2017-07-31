#pragma once
class CVisitInfo
{
public:
	CVisitInfo();
	~CVisitInfo();

	void Prepare(CString _DriverName, CString _KeeperName, CString _Gate, CString _Division, CString _VehicleType);
	void Prepare(long _DriverID, long _UserID, long _GateID, long _DivisionID, long _VehicleTypeID);
	CString GetDriver() { return m_DriverName; }
	CString GetKeeper() { return m_KeeperName; }
	CString GetGate() { return m_Gate; }
	CString GetDivision() { return m_Division; }
	CString GetVehicleType() { return m_VehicleType; }
	long GetDriverID() { return m_DriverID; }
	long GetUserID() { return m_UserID; }
	long GetGateID() { return m_GateID; }
	long GetDivisionID() { return m_DivisionID; }
	long GetVehicleTypeID() { return m_VehicleTypeID; }

private:
	CString m_DriverName;
	CString m_KeeperName;
	CString m_Gate;
	CString m_Division;
	CString m_VehicleType;
	long m_DriverID;
	long m_UserID;
	long m_GateID;
	long m_DivisionID;
	long m_VehicleTypeID;
};

