#pragma once
#include "afxwin.h"
#include "afxdtctl.h"


// CVehicleDlg dialog

class CVehicleDlg : public CDialog
{
	DECLARE_DYNAMIC(CVehicleDlg)

public:
	CVehicleDlg(CWnd* pParent = nullptr);   // standard constructor
	CVehicleDlg(const CTime&, const CString& = _T(""), const CString& = _T(""), 
		const int = BST_INDETERMINATE, const CString& = _T(""), CWnd* pParent = nullptr);
	virtual ~CVehicleDlg();

// Dialog Data
	enum { IDD = IDD_VEHICLE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	// checks whether the vehicle is in the black list or not.
	CButton m_dlgBlackList;
	// holds the driver name in the vehicle dialog
	CEdit m_dlgDriverName;
	// holds the license plate in the vehicle dialog
	CEdit m_dlgLicensePlate;
	//// holds the time and date of the current entry in the vehicle dialog
	//CDateTimeCtrl m_dlgDateTime;
	// holds the registration number of the current entry in the vehicle dialog
	CEdit m_dlgRegistrationNumber;

	virtual BOOL OnInitDialog();
//private:
	// the following are initialized by the constructor and hold the relevant info for dialog boxes
	// passed from the main view (ARALGISView)
	int m_transferredBL;
	CString m_transferredDN;
	CString m_transferredLP;
	//CTime m_transferredDT;
	CString m_transferredRN;

	CString m_dlgLicensePlateVal;
	int m_dlgBlackListVal;
	CString m_dlgDriverNameVal;
	//CTime m_dlgDateTimeVal;
	CString m_dlgRegistrationNumberVal;

	// returns LicensePlate
	CString getLP()
	{
		return m_dlgLicensePlateVal;
	}
	CString getDN()
	{
		return m_dlgDriverNameVal;
	}
	int getBL()
	{
		return m_dlgBlackListVal;
	}
	//CTime getDT()
	//{
	//	return m_dlgDateTimeVal;
	//}
	CString getRN()
	{
		return m_dlgRegistrationNumberVal;
	}
};
