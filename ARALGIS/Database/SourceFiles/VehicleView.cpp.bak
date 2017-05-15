
#include "stdafx.h"
#include "ARALGIS.h"
#include "VehicleView.h"
#include "VehicleSet.h"
#include "ARALGISDoc.h"

IMPLEMENT_DYNCREATE(CVehicleView, CRecordView)

CVehicleView::CVehicleView()
: CRecordView(CVehicleView::IDD), m_pSet(nullptr)
{
}

CVehicleView::~CVehicleView()
{
	if (m_pSet != nullptr)
		delete m_pSet;
}

#ifdef _DEBUG
void CVehicleView::AssertValid() const
{
	CRecordView::AssertValid();
}

void CVehicleView::Dump(CDumpContext& dc) const
{
	CRecordView::Dump(dc);
}
#endif _DEBUG

void CVehicleView::DoDataExchange(CDataExchange* pDX)
{
	CRecordView::DoDataExchange(pDX);
	DDX_FieldText(pDX, IDC_VEHICLE_LP, m_pSet->m_LicensePlate, m_pSet);
	DDX_FieldText(pDX, IDC_VEHICLE_LPI, m_pSet->m_LicensePlateImage, m_pSet);
	DDX_FieldText(pDX, IDC_VEHICLE_CBCI, m_pSet->m_ChassisBottomImageCurrent, m_pSet);
	DDX_FieldText(pDX, IDC_VEHICLE_CBRI, m_pSet->m_ChassisBottomImageRef, m_pSet);
	DDX_FieldText(pDX, IDC_VEHICLE_FVI, m_pSet->m_FrontViewImage, m_pSet);
	DDX_FieldText(pDX, IDC_VEHICLE_DN, m_pSet->m_DriverName, m_pSet);
	//DDX_FieldText(pDX, IDC_VEHICLE_BL, m_pSet->m_BlackList, m_pSet);
	//DDX_DateTimeCtrl(pDX, IDC_VEHICLE_LASTVISIT, m_pSet->m_MostRecentVisitDate);
}

CVehicleSet* CVehicleView::GetRecordSet()
{
	ASSERT(m_pSet != nullptr);
	return m_pSet;
}

CRecordset* CVehicleView::OnGetRecordset()
{
	return m_pSet;
}

void CVehicleView::OnInitialUpdate()
{
	// normally, it should be the following
	BeginWaitCursor();
	m_pSet = &GetDocument()->m_VehicleSet;

	if (m_pSet->IsOpen()) {
		GetDocument()->SetTitle(_T("All entries"));
	}
	CRecordView::OnInitialUpdate();
	EndWaitCursor();
	
	// the following is used in case the link in doc does not have the correct Recordset object
	//CARALGISDoc* pDoc = static_cast<CARALGISDoc*>(GetDocument());
	//m_pSet = &pDoc->m_pVehicleSet;

	//m_pSet->m_pDatabase = pDoc->m_VehicleSet.m_pDatabase;
	//// parameter stuff
	//// m_pSet->{param-holder} = pDoc->m_VehicleSet.{column-member}:
	//// filter stuff
	//// m_pSet->m_strFilter = {filter string};

	//CRecordView::OnInitialUpdate();
}