// C:\ali\github-home\ARALGIS\ARALGIS\GUI\SourceFiles\UserLog.cpp : implementation file
//

#include "stdafx.h"
#include "ARALGIS.h"
#include "UserLog.h"
#include "LoginDlg.h"


// CUserLog

IMPLEMENT_DYNAMIC(CUserLog, CWnd)

CUserLog::CUserLog()
{

}

CUserLog::~CUserLog()
{
}

BOOL CUserLog::Login()
{
	CLoginDlg dlgLogin;
	if (dlgLogin.DoModal() == IDOK)
	{
		CString _username = dlgLogin.getUsername();
		CString _password = dlgLogin.getPassword();

		CString strFilter = CString{ _T("[Nickname] = '") };
		strFilter += _username;
		strFilter += CString{ _T("' AND [Password] = '") };
		strFilter += _password;
		strFilter += CString{ _T("'") };
		m_UserSet.m_strFilter = strFilter;

		m_UserSet.Open(CRecordset::dynamic, nullptr, CRecordset::none);
		if (m_UserSet.IsEOF())
		{
			MessageBox(_T("Yanlis kullanici adi veya sifre girdiniz."));
			m_UserSet.Close();
			return FALSE;
		}
		m_Username = _username;
		MessageBox(_T("Hosgeldiniz ") + m_Username);
		m_UserLogSet.Open(CRecordset::dynamic, nullptr, CRecordset::appendOnly);
		m_UserLogSet.m_LoginDateTime = CTime::GetCurrentTime();
		m_UserLogSet.m_Nickname = m_Username;
		m_state = LOG_STATE::LOGGED;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CUserLog::Logout()
{
	if (m_state != LOG_STATE::LOGGED)
	{
		MessageBox(_T("Kullanici bagli degil."));
		return FALSE;
	}
	m_state = LOG_STATE::IDLE;

	// Update the logoff time for the current user.
	m_UserLogSet.Edit();
	if (!m_UserLogSet.CanUpdate())
	{
		MessageBox(_T("Kullanici Kayit tablosu guncellenemedi."));
	}
	else
	{
		CTime timeCurrent = CTime::GetCurrentTime();
		m_UserLogSet.m_LogoffDateTime = timeCurrent;
		m_UserLogSet.Update();
	}
	m_UserLogSet.Close();

	// nothing needs to be updated in the User Table
	m_UserSet.Close();
	return TRUE;
}

BEGIN_MESSAGE_MAP(CUserLog, CWnd)
END_MESSAGE_MAP()



// CUserLog message handlers


