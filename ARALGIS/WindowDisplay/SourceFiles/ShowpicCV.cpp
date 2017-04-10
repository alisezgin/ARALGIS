// Showpic.cpp : implementation file
//

#include "stdafx.h"
#include "ShowpicCV.h"


// CShowpic

IMPLEMENT_DYNAMIC(CShowpicCV, CStatic)

CShowpicCV::CShowpicCV()
{

}

CShowpicCV::~CShowpicCV()
{
	delete m_MatToGDI;

	//m_SapImg->Destroy();
	//delete m_SapImg;
}


BEGIN_MESSAGE_MAP(CShowpicCV, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()


void CShowpicCV::OnPaint()
{
	m_MatToGDI->PaintImage();
}


bool CShowpicCV::GetSapImage(BYTE* SapImg, LONG width, LONG height, int bytePP)
{
	//m_SapImg = new SapBuffer(SapImg->GetCount(), SapImg->GetWidth(), SapImg->GetHeight(), 
	//	                     SapImg->GetFormat(), SapImg->GetType());
	//if (!m_SapImg->Create())
	//{
	//	::MessageBox(NULL, _T("Can not create Buffer in GetSapImage"), NULL, MB_OK);
	//	return FALSE;
	//}
	//m_SapImg->Copy(SapImg);
	m_MatToGDI = new SapToGDI(this, true);

	m_MatToGDI->SetImageWidth(width);
	m_MatToGDI->SetImageHeight(height);
	m_MatToGDI->ImageBytesPerPixel(bytePP);

	m_MatToGDI->SetData(SapImg);

	return TRUE;
}