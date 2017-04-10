#pragma once
#include "stdafx.h"
#include <afxwin.h>
#include <WinGDI.h>
#include "SapClassBasic.h"



///
// ThreadSafe:You can use this class also in a worker threads only if
// two threads can't have DCs to the same window at same time
// two threads can't try to manipulate the same DC at same time.
//
// Only Grey,RGB, and RGBA images are supported (please note RGB and not BGR)
// Windows GDI requires DWORD alignment for rows and continuous memory block for the source bitmap.
// This means that the cv::Mat you will provide should be continuous (no ROI) and have columns %4.
// The class checks the requirement and creates a right temporary image in case is needed (loosing a bit of time).
//
/// To improve general memory management is strongly suggested to use always images where cols %4 =0 but if
// given image is not continuous the class will create the right image again.
///


class SapToGDI
{
public:
	//Standard constructor
	//Set the CStatic controls where the Sapbuffer Image will be drawn
	//the Image will be fitted on width or height based on destination rectangle
	//the Image will be stretched into destination rectangle
	SapToGDI(CWnd* ctrl = NULL, bool autofit = true);

	///Standard de-constructor
	~SapToGDI();

	///a valid CStatic object
	///set the autofit feature on/off (see SetAutofit() )
	bool SetDestination(CWnd* ctrl, bool autofit);

	///a valid CStatic object
	///autofit feature will not change
	bool SetDestination(CWnd* ctrl);

	///Set autofit features on/off
	///true:the Image will be fitted on width or height based on destination rectangle
	///false:the Image will be stretched into destination rectangle
	void SetAutofit(bool autofit);

	// Copy image data to internal structure
	bool SetData(BYTE* img);

	//Draw a Image using the DC of current CStatic control.
	bool PaintImage();

	void SetImageWidth(LONG width);
	void SetImageHeight(LONG height);
	void ImageBytesPerPixel(int byPP);

		
	typedef struct 
	{
		LONG      x;
		LONG      y;
		LONG      width;
		LONG      height;
	} CONV_RECT;
	

private:
	// Repaint the rectangle using current brush
	void BackgroundClear();

	// Initialize members. false if fail
	bool Init(CWnd* ctrl, bool autofit);


private:
	//Display mode. True=autofit, false=stretch
	bool m_autofit;

	// the CStatic control where to show the image
	CWnd* m_WinCtrl;

	// Clientrect related to the m_WinCtrl (for CStatic/CShowpic)
	RECT m_ctrlRectWin;

	// Utility: same as m_ctrlRectWin (for CStatic/CShowpic)
	CONV_RECT m_ctrlRectConv;

	//Internal: ratio width/height for m_ctrlRectWin
	double m_ctrlRectRatio;

	// Internal:The image size into m_ctrlRectWin. This might smaller due to autofit
	CONV_RECT m_destRectConv;  

	// Bitmap header for standard color image
	BITMAPINFO* m_bmiColor;
	uchar _bmiColorBuffer[sizeof(BITMAPINFO)]; //extra space for grey color table

	// Bitmap header for grey scale image
	BITMAPINFO* m_bmiGrey;
	uchar _bmiGreyBuffer[sizeof(BITMAPINFO) + 256 * 4]; //extra space for grey color table

	// internal variable for data 
	BYTE *m_pData;

	// image type
	BOOL m_isColour;

	LONG m_ImageWidth;
	LONG m_ImageHeight;
	int m_iBytesPerPixel;
	CONV_RECT m_rr;
};

