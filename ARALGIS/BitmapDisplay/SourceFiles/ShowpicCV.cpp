// Showpic.cpp : implementation file
//

#include "stdafx.h"
#include "..\\..\\ARALGIS.h"
#include "..\\HeaderFiles\\ShowpicCV.h"




// CShowpic

IMPLEMENT_DYNAMIC(CShowpicCV, CStatic)

CShowpicCV::CShowpicCV()
{

}

CShowpicCV::~CShowpicCV()
{
	delete m_MatToGDI;
}


BEGIN_MESSAGE_MAP(CShowpicCV, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()


void CShowpicCV::OnPaint()
{
	if (m_cvImg.rows != 0)
	{
		m_MatToGDI->DrawImg(m_cvImg);
	}
}


void CShowpicCV::GetcvImage(const cv::Mat &cvImg)
{
	cvImg.copyTo(m_cvImg);
	m_MatToGDI = new PkMatToGDI(this, true);
}