#pragma once
#include "stdafx.h"
#include <afxwin.h>
#include <WinGDI.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class CARALGISView;
class CReportDlg;


/// PkMatToGDI Class
/// This class provides a method to fast draw (fitting or stretching) an OpenCV Mat image directly into MFC Gui.
/// It's optimized to display a feed from video (cam or file) and it can be used to display a single cv::Mat.
/// 
/// It uses GDI to transfer the image from OpenCV memory to DC of a CStatic Control like a Picture or Static Text.
///
/// ThreadSafe:You can use this class also in a worker threads only if
/// two threads can't have DCs to the same window at same time
/// two threads can't try to manipulate the same DC at same time.
///
/// Only Grey,RGB, and RGBA images are supported (please note RGB and not BGR)
/// Windows GDI requires DWORD alignment for rows and continuous memory block for the source bitmap.
/// This means that the cv::Mat you will provide should be continuous (no ROI) and have columns %4.
/// The class checks the requirement and creates a right temporary image in case is needed (loosing a bit of time).
///
/// To improve general memory management is strongly suggested to use always images where cols %4 =0 but if
/// given image is not continuous the class will create the right image again.
///
/// This class isn't memory consuming because it uses an internal cv::Mat to recycle always it's possible,
/// in special case when you are rendering a video where all frames have almost same size.

class PkMatToGDI
{
	// ali: ARALGISView has to use BackgroundClear()
	friend CARALGISView;
	// ali: CReportDlg has to use BackgroundClear()
	friend CReportDlg;

public:
	//Standard constructor
	//Set the CStatic controls where the cv::Mat will be drawn
	//the cv::Mat will be fitted on width or height based on destination rectangle
	//the cv::Mat will be stretched into destination rectangle
	//In case you don't provide a valid ctrl at construction time you can use SetDestinationControl(...) later</remark>
	PkMatToGDI(CWnd* ctrl = NULL, bool autofit = true);


	///Standard de-constructor
	~PkMatToGDI(); 

	///Set the CStatic controls where the cv::Mat will be drawn
	///a valid CStatic object
	///set the autofit feature on/off (see SetAutofit() )
	bool SetDestination(CWnd* ctrl, bool autofit);


	///Set the CStatic controls where the cv::Mat will be drawn
	///a valid CStatic object
	///autofit feature will not change
	bool SetDestination(CWnd* ctrl);



	///Set autofit features on/off
	///true:the cv::Mat will be fitted on width or height based on destination rectangle
	///false:the cv::Mat will be stretched into destination rectangle
	void SetAutofit(bool autofit);

	//Draw a cv::Mat using the DC of current CStatic control.
	bool PkMatToGDI::DrawImg(const cv::Mat &img);

	void CvMatToWinControl(const cv::Mat& img, CStatic* WinCtrl);
	
private:
	///Repaint the rectangle using current brush
	void BackgroundClear();

	///Initialize members.
	///false if fail
	bool Init(CWnd* ctrl, bool autofit);


private:
	//Display mode. True=autofit, false=stretch
	bool m_autofit;

	// image used internally to reduce memory allocation due to DWORD padding requirement
	cv::Mat* m_internalImg;

	// the CStatic control where to show the image
	CWnd* m_WinCtrl;

	// Clientrect related to the m_WinCtrl
	RECT m_ctrlRectWin;

	// Utility: same as m_ctrlRectWin but using cv::Rect 
	cv::Rect m_ctrlRectCv;

	//Internal: ratio width/height for m_ctrlRectWin
	double m_ctrlRectRatio;

	//Internal:The image size into m_ctrlRectWin. This might smaller due to autofit
	cv::Rect m_destRectCv;

	//Bitmap header for standard color image
	BITMAPINFO* m_bmiColor;
	uchar _bmiColorBuffer[sizeof(BITMAPINFO)]; //extra space for grey color table

	//Bitmap header for grey scale image
	BITMAPINFO* m_bmiGrey;
	uchar _bmiGreyBuffer[sizeof(BITMAPINFO) + 256 * 4]; //extra space for grey color table
};

