#pragma once
#include "afxdtctl.h"
#include "afxwin.h"


// CSearchDlg dialog

using VectorType = std::vector<std::pair<CString, long>>;

class CSearchDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSearchDlg)

public:
	CSearchDlg(const std::vector<std::pair<CString, long>>& _driverList = VectorType{},
		const std::vector<std::pair<CString, long>>& _vehicleTypeList = VectorType{},
		const std::vector<std::pair<CString, long>>& _gateList = VectorType{},
		CWnd* pParent = NULL);   // standard constructor
	virtual ~CSearchDlg();
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	CString getFilter();
	void FillDriverList();
	void FillVehicleTypeList();
	void FillGateList();

// Dialog Data
	enum { IDD = IDD_DIALOG_SEARCH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	// holds the beginning date of the search; should be invisible if the user does not want to specify a beginning date
	CDateTimeCtrl m_DateBegin;
	// holds the ending date for the search dialog; should be invisible if the user wants to search until 'now'
	CDateTimeCtrl m_DateEnd;
	// holds the final string to be used as a filter in the sql query
	CString m_strFilter;
	// holds the license plate request; ignore in the search if it is null or invalid
	CString m_strLicensePlate;
	// the edit control for the license plate
	CEdit m_eLicensePlate;
	// holds the vehicle type; ignore if null or unselected
	CComboBox m_cVehicleType;
	// holds the driver name; ignore if null or empty string
	CComboBox m_cDriver;
	// holds the gate; ignore if null or empty string
	CComboBox m_cGate;

	// the database burden relievers are passed (by reference)
	const std::vector<std::pair<CString, long>>& m_DriverList;

	// holds the list of available vehicle types
	const std::vector<std::pair<CString, long>>& m_VehicleTypeList;

	// holds the list of gate names in the database
	const std::vector<std::pair<CString, long>>& m_GateList;

};
