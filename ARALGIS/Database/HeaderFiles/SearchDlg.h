#pragma once
#include "afxdtctl.h"
#include "afxwin.h"
#include <unordered_map>


// CSearchDlg dialog

using VectorType = std::vector<std::pair<CString, long>>;
using MapType = std::unordered_map<long, long>;

class CSearchDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSearchDlg)

public:
	CSearchDlg(const VectorType& _driverList = VectorType{},
		const VectorType& _vehicleTypeList = VectorType{},
		const VectorType& _gateList = VectorType{},
		MapType& _PosDriverIdMap = MapType{},
		std::unordered_map<long,long>& _PosVehicleTypeIdMap = MapType{},
		MapType& _PosGateIdMap = MapType{},
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
	const VectorType& m_DriverList;
	MapType& m_PosDriverIdMap;

	// holds the list of available vehicle types
	const VectorType& m_VehicleTypeList;
	std::unordered_map<long,long>& m_PosVehicleTypeIdMap;

	// holds the list of gate names in the database
	const VectorType& m_GateList;
	MapType& m_PosGateIdMap;

};
