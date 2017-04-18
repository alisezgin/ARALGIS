#pragma once
#include "afxwin.h"


// CVehicleDlg dialog

class CVehicleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CVehicleDlg)

public:
	CVehicleDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVehicleDlg();

// Dialog Data
	enum { IDD = IDD_VEHICLE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

};
