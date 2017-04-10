
// ARALGISDoc.h : interface of the CARALGISDoc class
//


#pragma once

#include "BitmapDlg.h"
#include <array>
#include <map>

//#include <cv.h>    
#include "BitmapDlg.h"


#include "opencv2/core/core.hpp"
//#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/opencv.hpp"
//#include <opencv2/imgproc/imgproc.hpp>

#include ".\\BitmapDisplay\\HeaderFiles\\SapToGDI.h"



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
	/// captured image window management variables
	/// captured image windows "map" key identifiers
	typedef std::array<BOOL, MAX_BMP_WINDOW> MapKeyArray;
	MapKeyArray m_iMapKeyIdentifier;
	/// captured image window "map" key-modelessWindow association 
	typedef std::map<int, CBitmapDlg*> ModelessWindowMap;
	ModelessWindowMap m_BmpDlgMap;

	int FindEmptyIndex();

	//bool m_selList[MAX_BUFFER];

	//cv::Mat m_CVCroppedImage;

	//cv::Mat m_BN;

	//cv::Mat m_CVImage;
	//cv::Mat m_CVDest;
	//cv::Mat m_CVImageHistoCrop;

	cv::Mat* m_InImage;
	cv::Mat *m_InImageFlipped;


	CShowpic *m_LoadedImage{ NULL };
	//CShowpicCV *m_LoadedImage{ NULL };

	SapToGDI m_gdiPlus;
	void convertMattoBmp();

public:
	void DisplayAreaSelectWindow();
	void SetOriginalActiveWindow();
	void SetHistoActiveWindow();
	void SetAreaActiveWindow();
	void Rotate90CWActiveWindow();
	void Rotate90CCWActiveWindow();
	void Rotate180ActiveWindow();

	void DisplayBitmapWindow();

	void GetImageMat(cv::Mat& mImg);

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
