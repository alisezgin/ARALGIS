#pragma once

#include "UserLogSet.h"
#include "UserSet.h"

// CUserLog

class CUserLog : public CWnd
{
	DECLARE_DYNAMIC(CUserLog)

protected:
	enum class LOG_STATE { IDLE, LOGGED };

public:
	CUserLog();
	virtual ~CUserLog();
	void Reset();
	BOOL Login();
	BOOL Logout();

protected:
	long m_lId;
	CString m_Username;
	CUserSet m_UserSet;
	CUserLogSet m_UserLogSet;
	LOG_STATE m_state;

protected:
	DECLARE_MESSAGE_MAP()
};


