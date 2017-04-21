#pragma once

class CVehicleSet;
class CARALGISDoc;

class CVehicleView : public CRecordView
{
	DECLARE_DYNCREATE(CVehicleView)

protected:
	virtual ~CVehicleView() {}
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnInitialUpdate();

public:
	enum { IDD = IDD_VEHICLE_DIALOG };

	CVehicleSet* m_pSet;

	CARALGISDoc* GetDocument() const
	{
		return reinterpret_cast<CARALGISDoc*>(m_pDocument);
	}

	CVehicleSet* GetRecordSet();
	virtual CRecordset* OnGetRecordset();
	CVehicleView();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};