
// ARALGISDoc.h : interface of the CARALGISDoc class
//


#pragma once

//#include <array>
//#include <map>
//#include "opencv2/core/core.hpp"
//#include ".\\BitmapDisplay\\HeaderFiles\\SapToGDI.h"


//#include ".\\Database\\HeaderFiles\\VehicleSet.h"



class CARALGISDoc : public CDocument
{
protected: // create from serialization only
	CARALGISDoc();
	DECLARE_DYNCREATE(CARALGISDoc)

	// Attributes
public:

	// Operations
public:

	// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

	// Implementation
public:
	virtual ~CARALGISDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

public:




	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	// points to the Recordset object representing the Araclar tab
	//CVehicleSet* m_pVehicleSet;
	// used to link the Doc to the database; default connect of CVehicleSet opens the db connection
	//CVehicleSet m_VehicleSet;
};
