// Showpic.cpp : implementation file
//

#include "stdafx.h"
#include "..\\..\\ARALGIS.h"
#include "..\\HeaderFiles\\Showpic.h"


// CShowpic

IMPLEMENT_DYNAMIC(CShowpic, CStatic)

CShowpic::CShowpic()
{
	m_un32ImageWidth = CAM_WIDTH;
	m_un32ImageHeight = CAM_HEIGHT;
	m_un32ImageBits = CAM_BITS;
	m_pImageBytes = NULL;
	m_pBmp = NULL;
	//MakeBMPHeader();

}

CShowpic::~CShowpic()
{
	if (m_pBmp)
		delete[]m_pBmp;
	m_pBmp = NULL;

	if (m_pImageBytes)
		delete[]m_pImageBytes;
	m_pImageBytes = NULL;
}


BEGIN_MESSAGE_MAP(CShowpic, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CShowpic message handlers

void CShowpic::MakeBMPHeader()
{

	DWORD  dwBitmapInfoSize;
	dwBitmapInfoSize = sizeof(BITMAPINFO);
	m_pBmp = (BITMAPINFO *)new BYTE[dwBitmapInfoSize];


	m_pBmp->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBmp->bmiHeader.biWidth = m_un32ImageWidth;
	m_pBmp->bmiHeader.biHeight = m_un32ImageHeight*-1;
	m_pBmp->bmiHeader.biPlanes = 1;
	m_pBmp->bmiHeader.biBitCount = (unsigned short)m_un32ImageBits;
	m_pBmp->bmiHeader.biCompression = BI_RGB;
	m_pBmp->bmiHeader.biSizeImage = 0;
	m_pBmp->bmiHeader.biXPelsPerMeter = 0;
	m_pBmp->bmiHeader.biYPelsPerMeter = 0;
	m_pBmp->bmiHeader.biClrUsed = 0;
	m_pBmp->bmiHeader.biClrImportant = 0;
}


void CShowpic::OnPaint()
{
	CRect rc;
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here
	if (m_pImageBytes != NULL)
	{
		GetClientRect(&rc);
		dc.RealizePalette();
		SetStretchBltMode(dc.GetSafeHdc(), COLORONCOLOR);


		//int m_un32ImageHeight;

		int nResult = ::StretchDIBits(
			dc.GetSafeHdc(),
			rc.right - rc.left,
			rc.top,
			-rc.right - rc.left,
			rc.bottom - rc.top,
			0,
			0,
			m_un32ImageWidth,
			m_un32ImageHeight,
			m_pImageBytes,
			m_pBmp,
			DIB_RGB_COLORS,
			SRCCOPY);
	}
}

void CShowpic::ShowImage(BYTE *pImageByte)
{
	if (pImageByte != NULL)
	{
		m_bImageCaptured = TRUE;
		memcpy(m_pImageBytes, pImageByte, m_iSizeImage);
		//Invalidate(FALSE);
	}
}

