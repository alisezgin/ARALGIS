
#include "stdafx.h"
#include "..\\HeaderFiles\\PkMattoGDI.h"



PkMatToGDI::PkMatToGDI(CWnd* ctrl, bool autofit)
{
	Init(ctrl, autofit);

	m_internalImg = NULL;
}

PkMatToGDI::~PkMatToGDI()
{
	if (m_internalImg != NULL)
	{
		delete m_internalImg;
		m_internalImg = NULL;
	}
}



///Set the CStatic controls where the cv::Mat will be drawn
///a valid CStatic object
///set the autofit feature on/off (see SetAutofit() )
bool PkMatToGDI::SetDestination(CWnd* ctrl, bool autofit)
{
	return Init(ctrl, autofit);
}

///Set the CStatic controls where the cv::Mat will be drawn</summary>
///a valid CStatic object
///autofit feature will not change
bool PkMatToGDI::SetDestination(CWnd* ctrl)
{
	return Init(ctrl, m_autofit);
}


///Set autofit features on/off
///true:the cv::Mat will be fitted on width or height based on destination rectangle
///false:the cv::Mat will be stretched into destination rectangle
void PkMatToGDI::SetAutofit(bool autofit)
{
	m_autofit = autofit;
}


//Draw a cv::Mat using the DC of current CStatic control.
bool PkMatToGDI::DrawImg(const cv::Mat &img)
{

	if (m_WinCtrl == NULL || img.empty())
		return false;

	/*
	CDC* pDC = m_WinCtrl->GetDC();
	if (pDC == NULL)
	return;
	HDC hDC = pDC->GetSafeHdc();
	*/
	CClientDC hDC(m_WinCtrl);
	int bpp =(int) (8 * img.elemSize());
	assert((bpp == 8 || bpp == 24 || bpp == 32));

	int img_w = img.cols;
	int img_h = img.rows;

	cv::Rect rr;
	if (m_autofit)
	{
		if (m_ctrlRectRatio > (1.0*img_w / img_h))
		{
			// fit height
			rr.height = m_ctrlRectCv.height;
			rr.width = (int)floor(1.0*img_w * m_ctrlRectCv.height / img_h);
			//align center
			rr.x = (int)floor((m_ctrlRectCv.width - rr.width) / 2.0);
			rr.y = m_ctrlRectCv.y;
		}
		else
		{
			// fit width
			rr.width = m_ctrlRectCv.width;
			rr.height = (int)floor(1.0*img_h * m_ctrlRectCv.width / img_w);
			//align middle
			rr.x = m_ctrlRectCv.x;
			rr.y = (int)floor((m_ctrlRectCv.height - rr.height) / 2);
		}
	}
	else
	{
		//stretch
		rr = m_ctrlRectCv;
	}


	// The image must be padded 4bytes and must be continuous
	//int stride = ((((img.cols * bpp) + 31) & ~31) >> 3);
	int padding = 0;

	// 32 bit image is always DWORD aligned because each pixel requires 4 bytes
	if (bpp == 32)
	{
		padding = 0;
	}
	else if ((img.cols % 4) > 0)
	{
		padding = 4 - (img.cols % 4);
	}
	else
	{
		padding = 0;
	}

	cv::Mat tempimg;
	if (padding > 0 || img.isContinuous() == false)
	{
		// Adding needed columns on the right (max 3 px)

		if (m_internalImg != NULL)
		{
			delete m_internalImg;
			m_internalImg = NULL;
		}

		m_internalImg = new cv::Mat;
		//*m_internalImg = cv::Mat::zeros(img.size(), CV_8U);
		*m_internalImg = cv::Mat::zeros(cv::Size(img.cols + padding, img.rows), img.type());

		// we use internal image to reuse the memory. Avoid to alloc new memory at each call due to img size changes rarely
		cv::copyMakeBorder(img, *m_internalImg, 0, 0, 0, padding, cv::BORDER_CONSTANT, 0);


		//m_internalImg->copyTo(tempimg);
		tempimg.create(img.size(), img.type());
		tempimg =  m_internalImg->clone();
		
		//tempimg = m_internalImg;
		// ignore (do not shows) the just added border
		//img_w = tempimg.cols;

		if (m_internalImg != NULL)
		{
			delete m_internalImg;
			m_internalImg = NULL;
		}
	}
	else
	{
		//tempimg = img;
		tempimg.create(img.size(), img.type());
		tempimg = img.clone();
	}



	BITMAPINFO* bmi;
	BITMAPINFOHEADER* bmih;
	if (bpp == 8)
	{
		bmi = m_bmiGrey;
	}
	else
	{
		bmi = m_bmiColor;
	}

	bmih = &(bmi->bmiHeader);
	bmih->biHeight = -tempimg.rows;
	bmih->biWidth = tempimg.cols;
	bmih->biBitCount = bpp;

	//------------------
	// DRAW THE IMAGE 

	//if source and destination are same size
	if (tempimg.size() == m_ctrlRectCv.size())
	{
		// tranfer memory block
		// NOTE: the padding border will be shown here. Anyway it will be max 3px width
		int numLines = SetDIBitsToDevice(hDC,
			m_ctrlRectCv.x, m_ctrlRectCv.y, m_ctrlRectCv.width, m_ctrlRectCv.height,
			0, 0, 0, tempimg.rows, tempimg.data, bmi, DIB_RGB_COLORS);
		if (numLines == 0)
			return false;

		m_destRectCv = m_ctrlRectCv;
		// all done
		return true;
	}

	//if destination rect is smaller of previous we need to clear the background
	if (m_destRectCv.width <= 0)
	{
		m_destRectCv = rr;
	}
	else if (rr != m_destRectCv)
	{
		BackgroundClear();
		m_destRectCv = rr;
	}
	//if destination width less than source width
	else if (m_destRectCv.width < img_w)
	{
		SetStretchBltMode(hDC, HALFTONE);
	}
	else
	{
		SetStretchBltMode(hDC, COLORONCOLOR);
	}

	//copy and stretch the image
	int numLines = StretchDIBits(hDC,
								 m_destRectCv.x, 
								 m_destRectCv.y, 
								 m_destRectCv.width, 
								 m_destRectCv.height,
								 0, 0, 
								 img_w, 
								 img_h,
								 tempimg.data, 
								 bmi, 
								 DIB_RGB_COLORS, 
								 SRCCOPY);

	if (numLines == 0)
		return false;

	//all done
	return true;
}



///Repaint the rectangle using current brush
void PkMatToGDI::BackgroundClear()
{
	CClientDC hDC(m_WinCtrl);
	//the rectangle is outlined by using the current pen and filled by using the current brush
	::Rectangle(hDC, m_ctrlRectWin.left, m_ctrlRectWin.top, m_ctrlRectWin.right, m_ctrlRectWin.bottom);

}

///Initialize members.
///false if fail
bool PkMatToGDI::Init(CWnd* ctrl, bool autofit)
{
	m_WinCtrl = ctrl;
	if (m_WinCtrl == NULL)
		return false;

	m_autofit = autofit;

	m_WinCtrl->GetClientRect(&m_ctrlRectWin);

	m_ctrlRectCv.x = m_ctrlRectWin.left;
	m_ctrlRectCv.y = m_ctrlRectWin.top;
	m_ctrlRectCv.width = m_ctrlRectWin.right - m_ctrlRectWin.left;
	m_ctrlRectCv.height = m_ctrlRectWin.bottom - m_ctrlRectWin.top;
	m_ctrlRectRatio = 1.0*m_ctrlRectCv.width / m_ctrlRectCv.height;
	m_destRectCv = m_ctrlRectCv;

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


void PkMatToGDI::CvMatToWinControl(const cv::Mat& img, CStatic* WinCtrl)
{
	if (WinCtrl == NULL || img.empty())
		return;

	int bpp = (int) (8 * img.elemSize());

	assert((bpp == 8 || bpp == 24 || bpp == 32));

	//Get DC of your win control
	CClientDC hDC(WinCtrl);

	// This is the rectangle where the control is defined
	// and where the image will appear
	RECT rr;
	WinCtrl->GetClientRect(&rr);

	//rr.top AND rr.left are always 0
	int rectWidth = rr.right;
	int rectHeight = rr.bottom;

	///------------------------------------
	/// DWORD ALIGNMENT AND CONTINOUS MEMORY
	/// The image must be padded 4bytes and must be continuous

	int border = 0;
	//32 bit image is always DWORD aligned because each pixel requires 4 bytes
	if (bpp < 32)
	{
		border = 4 - (img.cols % 4);
	}

	cv::Mat tmpImg;
	if (border > 0 || img.isContinuous() == false)
	{
		// Adding needed columns on the right (max 3 px)
		cv::copyMakeBorder(img, tmpImg, 0, 0, 0, border, cv::BORDER_CONSTANT, 0);
	}
	else
	{
		tmpImg = img;
	}

	///----------------------
	/// PREPARE BITMAP HEADER
	/// The header defines format and shape of the source bitmap in memory ... this will produce needed bmi

	uchar buffer[sizeof(BITMAPINFO) + 256 * 4];
	BITMAPINFO* bmi = (BITMAPINFO*)buffer;
	BITMAPINFOHEADER* bmih = &(bmi->bmiHeader);
	memset(bmih, 0, sizeof(*bmih));
	bmih->biSize = sizeof(BITMAPINFOHEADER);
	bmih->biWidth = tmpImg.cols;
	bmih->biHeight = -tmpImg.rows;// DIB are bottom ->top
	bmih->biPlanes = 1;
	bmih->biBitCount = bpp;
	bmih->biCompression = BI_RGB;

	//Sets the palette if image is grey scale
	if (bpp == 8)
	{
		RGBQUAD* palette = bmi->bmiColors;
		for (int i = 0; i < 256; i++)
		{
			palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
			palette[i].rgbReserved = 0;
		}
	}

	/// -----------
	/// Draw to DC

	if (tmpImg.cols == rectWidth  && tmpImg.rows == rectHeight)
	{
		// source and destination have same size
		// transfer memory block
		// NOTE: the padding border will be shown here. Anyway it will be max 3px width

		SetDIBitsToDevice(hDC,
		//destination rectangle
		0, 0, rectWidth, rectHeight,
		0, 0, 0, tmpImg.rows,
		tmpImg.data, bmi, DIB_RGB_COLORS);
	}
	else
	{
		// Image is bigger or smaller than into destination rectangle
		// we use stretch in full rect

		// destination rectangle
		int destx = 0, desty = 0;
		int destw = rectWidth;
		int desth = rectHeight;

		// rectangle defined on source bitmap
		// using imgWidth instead of tmpImg.cols will ignore the padding border
		int imgx = 0, imgy = 0;
		int imgWidth = tmpImg.cols - border;
		int imgHeight = tmpImg.rows;

		StretchDIBits(hDC,
		destx, desty, destw, desth,
		imgx, imgy, imgWidth, imgHeight,
		tmpImg.data, bmi, DIB_RGB_COLORS, SRCCOPY);
	}
} //end function


