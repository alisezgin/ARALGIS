#include "stdafx.h"
#include "SapToGDI.h"

SapToGDI::SapToGDI(CWnd* ctrl, bool autofit)
{
	Init(ctrl, autofit);
	m_isColour = FALSE;
	m_ImageWidth = 0;
	m_ImageHeight = 0;
}

SapToGDI::~SapToGDI()
{
	if (m_pData != NULL)
		delete[] m_pData;
};


// Set the CStatic controls where the cv::Mat will be drawn
// a valid CStatic object
// set the autofit feature on/off (see SetAutofit() )
bool SapToGDI::SetDestination(CWnd* ctrl, bool autofit)
{
	return Init(ctrl, autofit);
}

// Set the CStatic controls where the cv::Mat will be drawn</summary>
// a valid CStatic object
// autofit feature will not change
bool SapToGDI::SetDestination(CWnd* ctrl)
{
	return Init(ctrl, m_autofit);
}


// Set autofit features on/off
// true:the cv::Mat will be fitted on width or height based on destination rectangle
// false:the cv::Mat will be stretched into destination rectangle
void SapToGDI::SetAutofit(bool autofit)
{
	m_autofit = autofit;
}

///Repaint the rectangle using current brush
void SapToGDI::BackgroundClear()
{
	CClientDC hDC(m_WinCtrl);
	//the rectangle is outlined by using the current pen and filled by using the current brush
	//::Rectangle(hDC, m_ctrlRectWin.left, m_ctrlRectWin.top, m_ctrlRectWin.right, m_ctrlRectWin.bottom);

	//CRect m_EnclosingRect = CRect{ m_ctrlRectWin.left, m_ctrlRectWin.top, m_ctrlRectWin.right, m_ctrlRectWin.bottom };
	//m_WinCtrl->InvalidateRect(&m_EnclosingRect, true);
}

///Initialize members.
///false if fail
bool SapToGDI::Init(CWnd* ctrl, bool autofit)
{
	m_pData = NULL;

	m_WinCtrl = ctrl;
	if (m_WinCtrl == NULL)
		return false;

	m_autofit = autofit;

	m_WinCtrl->GetClientRect(&m_ctrlRectWin);

	m_ctrlRectConv.x = m_ctrlRectWin.left;  // buralara biraz daha bak, yanlış olabilir left/right/top/buttom
	m_ctrlRectConv.y = m_ctrlRectWin.top;
	m_ctrlRectConv.width = m_ctrlRectWin.right - m_ctrlRectWin.left;
	m_ctrlRectConv.height = m_ctrlRectWin.bottom - m_ctrlRectWin.top;
	m_ctrlRectRatio = 1.0*m_ctrlRectConv.width / m_ctrlRectConv.height;
	m_destRectConv = m_ctrlRectConv;

	BITMAPINFOHEADER*	bmih;

	//standard colour bitmapinfo
	m_bmiColor = (BITMAPINFO*)_bmiColorBuffer;
	bmih = &(m_bmiColor->bmiHeader);
	memset(bmih, 0, sizeof(*bmih));
	bmih->biSize = sizeof(BITMAPINFOHEADER);
	bmih->biWidth = 0;
	bmih->biHeight = 0;
	bmih->biPlanes = 1;
	bmih->biBitCount = 0;
	bmih->biSizeImage = bmih->biWidth * bmih->biHeight * bmih->biBitCount / 8;
	bmih->biCompression = BI_RGB;

	//grey scale bitmapinfo
	m_bmiGrey = (BITMAPINFO*)_bmiGreyBuffer;
	bmih = &(m_bmiGrey->bmiHeader);
	memset(bmih, 0, sizeof(*bmih));
	bmih->biSize = sizeof(BITMAPINFOHEADER);
	bmih->biWidth = 0;
	bmih->biHeight = 0;
	bmih->biPlanes = 1;
	bmih->biBitCount = 8;
	bmih->biSizeImage = bmih->biWidth * bmih->biHeight * bmih->biBitCount / 8;
	bmih->biCompression = BI_RGB;

	//create a grey scale palette
	RGBQUAD* palette = m_bmiGrey->bmiColors;
	int i;
	for (i = 0; i < 256; i++)
	{
		palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
		palette[i].rgbReserved = 0;
	}
	return true;
}

void SapToGDI::SetImageWidth(LONG width)
{
	m_ImageWidth = width;
}
void SapToGDI::SetImageHeight(LONG height)
{
	m_ImageHeight = height;
}

void SapToGDI::ImageBytesPerPixel(int byPP)
{
	m_iBytesPerPixel = byPP;
}

// Copy image data to internal structure
bool SapToGDI::SetData(BYTE* img)
{
	//SapBuffer::State bufState;
	//img.GetState(&bufState);
	if (m_WinCtrl == NULL) // || (bufState == SapBuffer::StateEmpty))
		return false;

	//int iBytesPerPixel = (int)img.GetBytesPerPixel();
	int bpp = 8 * m_iBytesPerPixel;
	assert((bpp == 8 || bpp == 24 || bpp == 32));

	//m_ImageWidth = img.GetWidth();;
	//m_ImageHeight = img.GetHeight();

	if (m_autofit)
	{
		if (m_ctrlRectRatio > (1.0*m_ImageWidth / m_ImageHeight))
		{
			// fit height
			m_rr.height = m_ctrlRectConv.height;
			m_rr.width = (int)floor(1.0 * m_ImageWidth * m_ctrlRectConv.height / m_ImageHeight);
			//align center
			m_rr.x = (int)floor((m_ctrlRectConv.width - m_rr.width) / 2.0);
			m_rr.y = m_ctrlRectConv.y;
		}
		else
		{
			// fit width
			m_rr.width = m_ctrlRectConv.width;
			m_rr.height = (int)floor(1.0*m_ImageHeight * m_ctrlRectConv.width / m_ImageWidth);
			//align middle
			m_rr.x = m_ctrlRectConv.x;
			m_rr.y = (int)floor((double)(m_ctrlRectConv.height - m_rr.height) / 2.0);
		}
	}
	else
	{
		//stretch
		m_rr = m_ctrlRectConv;
	}


	// The image must be padded 4bytes and must be continuous
	//int stride = ((((img.cols * bpp) + 31) & ~31) >> 3);
	int padding = 0;

	// 32 bit image is always DWORD aligned because each pixel requires 4 bytes
	if (bpp == 32)
	{
		padding = 0;
	}
	else if ((m_ImageWidth % 4) > 0)
	{
		padding = 4 - (m_ImageWidth % 4);
	}
	else
	{
		padding = 0;
	}

	//SapBuffer* tempimg;
	if (padding > 0 ) // || img.isContinuous() == false) // boraN
	{
		// Adding needed columns on the right (max 3 px)

		// we use internal image to reuse the memory. Avoid to alloc new memory at each call due to img size changes rarely
		// XXXX boraN
		// XXXX boraN
		//cv::copyMakeBorder(img, m_internalImgSAP, 0, 0, 0, padding, cv::BORDER_CONSTANT, 0);
		//tempimg = &m_internalImgSAP;
		// ignore (do not shows) the just added border
		//img_w = tempimg.cols;
	}
	//else
	//{
	//	tempimg = &img;
	//}

	BITMAPINFO* bmi;
	BITMAPINFOHEADER* bmih;
	if (bpp == 8)
	{
		bmi = m_bmiGrey;
		m_isColour = FALSE;
	}
	else
	{
		bmi = m_bmiColor;
		m_isColour = TRUE;
	}

	bmih = &(bmi->bmiHeader);
	bmih->biHeight = -m_ImageHeight; // -1 is due to negative coordinate of windows
	bmih->biWidth = m_ImageWidth;
	bmih->biBitCount = (WORD)bpp;

	//------------------
	// DRAW THE IMAGE 

	int size = m_ImageHeight *  m_ImageWidth; 

	m_pData = new BYTE[size * m_iBytesPerPixel];

	//BOOL success = img.Read(0, size, m_pData);
	BOOL success = TRUE;
	memcpy(m_pData, img, size * m_iBytesPerPixel);
	if (success == FALSE)
	{
		::MessageBox(NULL, _T("Can not read Buffer in SapToGDI::SetData"), NULL, MB_OK);
		return FALSE;
	}

	//all done
	return true;
}


bool SapToGDI::PaintImage()
{
	CClientDC hDC(m_WinCtrl);

	BITMAPINFO* bmi;
	if (m_isColour == FALSE)
	{
		bmi = m_bmiGrey;
	}
	else
	{
		bmi = m_bmiColor;
	}

	//if source and destination are same size
	if (
		(m_ImageWidth == m_ctrlRectConv.width) &&
		(m_ImageHeight == m_ctrlRectConv.height)
		)
	{
		// transfer memory block
		// NOTE: the padding border will be shown here. Anyway it will be max 3px width
		int numLines = SetDIBitsToDevice(hDC,
			m_ctrlRectConv.x, m_ctrlRectConv.y, m_ctrlRectConv.width, m_ctrlRectConv.height,
			0, 0, 0, m_ImageHeight, m_pData, bmi, DIB_RGB_COLORS);

		if (numLines == 0)
			return false;

		m_destRectConv = m_ctrlRectConv;
		// all done
		return true;
	}

	//if destination rect is smaller of previous we need to clear the background
	if (m_destRectConv.width <= 0)
	{
		m_destRectConv = m_rr;
	}
	//else if (rr != m_destRectConv) // boraN
	else if (
		(m_rr.x != m_destRectConv.x) &&
		(m_rr.y != m_destRectConv.y) &&
		(m_rr.width != m_destRectConv.width) &&
		(m_rr.height != m_destRectConv.height)
		)
	{
		BackgroundClear();
		m_destRectConv = m_rr;
	}
	//if destination width less than source width
	else if (m_destRectConv.width < m_ImageWidth)
	{
		SetStretchBltMode(hDC, HALFTONE);
	}
	else
	{
		SetStretchBltMode(hDC, COLORONCOLOR);
	}

	//copy and stretch the image
	int numLines = StretchDIBits(hDC,
		m_destRectConv.x, m_destRectConv.y, m_destRectConv.width, m_destRectConv.height,
		0, 0, m_ImageWidth, m_ImageHeight,
		m_pData, bmi, DIB_RGB_COLORS, SRCCOPY);

	if (numLines == 0)
		return false;

	//CRect aRect = CRect{ m_destRectConv.x, m_destRectConv.y,
	//	m_destRectConv.x + m_destRectConv.width,
	//	m_destRectConv.y + m_destRectConv.height, };

	////m_WinCtrl->Invalidate(true);
	////m_WinCtrl->InvalidateRect(aRect);

	return true;
}
