
// ARALGISDoc.cpp : implementation of the CARALGISDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "ARALGIS.h"
#endif

#include "ARALGISDoc.h"
#include "InitGlobalVars.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CARALGISDoc

IMPLEMENT_DYNCREATE(CARALGISDoc, CDocument)

BEGIN_MESSAGE_MAP(CARALGISDoc, CDocument)
END_MESSAGE_MAP()


// CARALGISDoc construction/destruction

CARALGISDoc::CARALGISDoc()
{
	// TODO: add one-time construction code here
	// initialize the global variables
	InitializeGlobalVars();




	m_InImage = NULL;
	m_InImage = new cv::Mat;

	m_InImageFlipped = NULL;
	m_InImageFlipped = new cv::Mat;
}

CARALGISDoc::~CARALGISDoc()
{
	delete m_InImage;
	delete m_InImageFlipped;
}

BOOL CARALGISDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CARALGISDoc serialization

void CARALGISDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CARALGISDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CARALGISDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CARALGISDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

void CARALGISDoc::DisplayBitmapWindow()
{
	//int iMapIndex;

	//iMapIndex = FindEmptyIndex();
	//if (iMapIndex != -1)
	//{
	//	CBitmapDlg* wBMPDlg;
	//	wBMPDlg = new CBitmapDlg(AfxGetApp()->GetMainWnd()->GetDesktopWindow());
	//	wBMPDlg->Create(CBitmapDlg::IDD, AfxGetApp()->GetMainWnd()->GetDesktopWindow());

	//	m_BmpDlgMap[iMapIndex] = wBMPDlg;

	//	int iWidth = m_Buffers->GetWidth();

	//	int iHeight = m_Buffers->GetHeight();

	//	int size = iWidth * iHeight;
	//	int pixel_depth = m_Buffers->GetPixelDepth();
	//	int bytesPerPixel = m_Buffers->GetBytesPerPixel();


	//	int i = 0;
	//	int j = 0;
	//	int k = 0;
	//	int inumFrames = 0;
	//	int bufOffset = 0;

	//	int nNumBuffers;
	//	nNumBuffers = m_Buffers->GetCount();


	//	SapBuffer::State bufState;

	//	for (i = 0; i < MAX_BUFFER; i++)
	//	{
	//		if (m_selList[i] == TRUE)
	//			inumFrames++;
	//	}

	//	if (inumFrames > nNumBuffers)
	//	{
	//		inumFrames = nNumBuffers;
	//	}

	//	BYTE *pData = new BYTE[size * bytesPerPixel  * inumFrames];
	//	BOOL success = FALSE;

	//	int iCount = m_Buffers->GetCount();
	//	int iIndex = m_Buffers->GetIndex();

	//	bufOffset = m_Buffers->GetCount() - 1 - m_Buffers->GetIndex();
	//	for (i = 0; i < bufOffset; i++)
	//		m_Buffers->Next();

	//	for (i = 0; i < MAX_BUFFER; i++)  // MAX_BUFFER
	//	{
	//		//m_Buffers->GetState(&bufState);
	//		if (m_selList[i] == TRUE)
	//		{
	//			j = k * size * bytesPerPixel;
	//			success = m_Buffers->Read(0, size, (pData + j));
	//			k++;
	//		}
	//		m_Buffers->Next();
	//	}

	//	wBMPDlg->m_ImageToDisplay->GetSapImage(pData, iWidth, iHeight*inumFrames, bytesPerPixel);

	//	TCHAR str_winTitle[10];
	//	_itot_s(iMapIndex, str_winTitle, 10);

	//	wBMPDlg->SetWindowText(str_winTitle);
	//	wBMPDlg->ShowWindow(SW_SHOW);
	//}
}



//LRESULT  CARALGISDoc::OnCloseBMPDlg(WPARAM wParam, LPARAM lParam)
//{
//	int* nWindowNo = new int;
//
//	*nWindowNo = _ttoi((TCHAR*)lParam);
//
//	TCHAR *textToDisplay;
//	textToDisplay = new TCHAR[100];
//	//textToDisplay = (TCHAR *)_bstr_t(lParam);
//
//	//::MessageBox(NULL, reinterpret_cast<TCHAR*>(lParam), NULL, MB_OK);
//
//	delete[] textToDisplay;
//
//	CBitmapDlg* wBMPDlg;
//
//	wBMPDlg = m_BmpDlgMap[*nWindowNo];
//
//	m_iMapKeyIdentifier[*nWindowNo] = FALSE; // bunlarý destroyWindow sonrasýna mý koysan acaba
//
//	m_BmpDlgMap.erase(*nWindowNo); //bunlarý destroyWindow sonrasýna mý koysan acaba
//
//	delete nWindowNo; //bunlarý destroyWindow sonrasýna mý koysan acaba
//
//	wBMPDlg->DestroyWindow();
//
//	return 1;
//}
//
//LRESULT  CARALGISDoc::OnDestroyBMPDlg(WPARAM wParam, LPARAM lParam)
//{
//	return 1;
//}

int CARALGISDoc::FindEmptyIndex()
{
	int index = 0;


	for (int i = 0; i < (int)m_iMapKeyIdentifier.size(); i++)
	{
		if (m_iMapKeyIdentifier[i] == FALSE)
		{
			index = i;
			m_iMapKeyIdentifier[i] = TRUE;
			return index;
		}
	}
	::MessageBox(NULL, _T("Max Number of Windows Exceeded, Close Others To Continue"), NULL, MB_OK);
	return-1;
}


void CARALGISDoc::GetImageMat(cv::Mat& mImg)
{
	// boraN
	m_InImageFlipped->copyTo(mImg);
}





// CARALGISDoc diagnostics

#ifdef _DEBUG
void CARALGISDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CARALGISDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CARALGISDoc commands
