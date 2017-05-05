// VehicleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ARALGIS.h"
#include "VehicleDlg.h"
#include "afxdialogex.h"


// CVehicleDlg dialog

IMPLEMENT_DYNAMIC(CVehicleDlg, CDialog)

CVehicleDlg::CVehicleDlg(CWnd* pParent /*=NULL*/)
: CDialog(CVehicleDlg::IDD, pParent),
m_dlgLicensePlateVal(_T("")), m_dlgBlackListVal{}, 
m_dlgDriverNameVal(_T("")), m_dlgRegistrationNumberVal(_T(""))
{

}

CVehicleDlg::CVehicleDlg(const CTime& _time, const CString& _DriverName, 
	const CString& _LicensePlate, int _BlackListFlag,  const CString& _RegistrationNumber, 
	CWnd* pParent)
	: CDialog(CVehicleDlg::IDD, pParent),
	m_transferredDN(_DriverName), m_transferredLP(_LicensePlate), 
	m_transferredBL(_BlackListFlag), m_transferredRN(_RegistrationNumber)
{
}

CVehicleDlg::~CVehicleDlg()
{
}

// the value variables are initialized upon initialization.
// they are modified if the user presses OK to close the dialog.
// after the doModal() call, the values are either the transferred ones or the new ones, but not a mixture of both.
void CVehicleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VEHICLE_BLACKLIST, m_dlgBlackList);
	DDX_Control(pDX, IDC_VEHICLE_DN, m_dlgDriverName);
	DDX_Control(pDX, IDC_VEHICLE_LP, m_dlgLicensePlate);
	//DDX_Control(pDX, IDC_VEHICLE_LASTVISIT, m_dlgDateTime);
	DDX_Control(pDX, IDC_VEHICLE_RN, m_dlgRegistrationNumber);
	// DoDataExchange is called on IDOK as well
	// this is a good place to finalize the transfer variables
	m_dlgBlackListVal = m_dlgBlackList.GetCheck();
	m_dlgDriverName.GetWindowText(m_dlgDriverNameVal);
	m_dlgLicensePlate.GetWindowText(m_dlgLicensePlateVal);
	//m_dlgDateTime.GetTime(m_dlgDateTimeVal);
	m_dlgRegistrationNumber.GetWindowText(m_dlgRegistrationNumberVal);
}


BEGIN_MESSAGE_MAP(CVehicleDlg, CDialog)
END_MESSAGE_MAP()


// CVehicleDlg message handlers



BOOL CVehicleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	// the caller must have set the transfer variables.
	// note that the initialization for the value variables could have been done here;
	// for the sake of uniformity, I have done that in the DoDataExchange routine
	// called by UpdateData().
	//m_dlgDateTime.SetTime(static_cast<const CTime*>(&m_transferredDT));
	m_dlgDriverName.SetWindowText(static_cast<LPCTSTR>(m_transferredDN));
	m_dlgLicensePlate.SetWindowText(static_cast<LPCTSTR>(m_transferredLP));
	m_dlgBlackList.SetCheck(m_transferredBL);
	m_dlgRegistrationNumber.SetWindowText(static_cast<LPCTSTR>(m_transferredRN));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
