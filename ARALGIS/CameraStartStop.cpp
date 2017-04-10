// CameraStartStop.cpp : implementation file
//

#include "stdafx.h"
#include "ARALGIS.h"
#include "CameraStartStop.h"
#include "afxdialogex.h"


// CCameraStartStop dialog

IMPLEMENT_DYNAMIC(CCameraStartStop, CDialogEx)

CCameraStartStop::CCameraStartStop(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCameraStartStop::IDD, pParent)
{

}

CCameraStartStop::~CCameraStartStop()
{
}

void CCameraStartStop::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCameraStartStop, CDialogEx)
	ON_BN_CLICKED(IDC_START_BUTTON, &CCameraStartStop::OnBnClickedStartButton)
	ON_BN_CLICKED(IDC_PAUSE_BUTTON, &CCameraStartStop::OnBnClickedPauseButton)
	ON_BN_CLICKED(IDC_STOP_BUTTON, &CCameraStartStop::OnBnClickedStopButton)
	ON_BN_CLICKED(IDC_SHOW_BMP_BUTTON, &CCameraStartStop::OnBnClickedShowBmpButton)
	ON_BN_CLICKED(IDC_SHOW_OPENCV_BUTTON, &CCameraStartStop::OnBnClickedShowOpencvButton)
END_MESSAGE_MAP()


// CCameraStartStop message handlers


void CCameraStartStop::OnBnClickedStartButton()
{
	SetEvent(g_CameraStartDataRecieveEvent);
}


void CCameraStartStop::OnBnClickedPauseButton()
{
	SetEvent(g_CameraPauseDataRecieveEvent);
}


void CCameraStartStop::OnBnClickedStopButton()
{
	SetEvent(g_CameraStopDataRecieveEvent);
}


void CCameraStartStop::OnBnClickedShowBmpButton()
{
	// TODO: Add your control notification handler code here
}


void CCameraStartStop::OnBnClickedShowOpencvButton()
{
	// TODO: Add your control notification handler code here
}
