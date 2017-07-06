
// MainFrm.h : interface of the CMainFrame class
//

#pragma once


#include ".\\CameraInterface\\HeaderFiles\\CameraDataReceiver.h"
#include ".\\PTSCommunicatorISSD\\HeaderFiles\\PTSCommunicatorISSD.h"
#include ".\\PTSCommunicatorDVIT\\HeaderFiles\\PTSCommunicatorDVIT.h"
#include ".\\OdroidCommunicator\\HeaderFiles\\OdroidCommunicator.h"
#include ".\\Database\\HeaderFiles\\CameraDBServer.h"
#include ".\\ImageFiltering\\ImageFilterProcessing\\HeaderFiles\\ImageFilterProcessing.h"
#include ".\\miniIni\\Processing\\HeaderFiles\\IniFileProcessing.h"
#include ".\\VehicleDetection\\HeaderFiles\\VehicleDetection.h"
#include ".\\ChangeDetection\\HeaderFiles\\ChangeDetectController.h"
#include ".\\DiskSpaceController\\HeaderFiles\\FreeDiskSpaceController.h"




class CMainFrame : public CFrameWnd
{

protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

	// Attributes
public:

	CCameraDataReceiver      *m_CameraDataReceiver;
	CPTSCommunicatorISSD     *m_PTSCommunicatorISSD;
	CPTSCommunicatorDVIT     *m_PTSCommunicatorDVIT;
	COdroidCommunicator      *m_OdroidCommunicator;
	CCameraDBServer          *m_CameraDatabaseServer;
	CImageFilterProcessing   *m_ImageFilterProcessing;
	CVehicleDetection        *m_VehicleDetector;
	CChangeDetectController  *m_ChangeDetector;
	CFreeDiskSpaceController *m_DiskSpaceController;


protected:  // control bar embedded members
	CToolBar           m_wndToolBar;
	CStatusBar         m_wndStatusBar;
	HANDLE			   m_hSemaphore;
	IniFileProcessing  m_IniFile;

	bool m_IsFirstTime;

	// Operations
public:
	void Activate();
	HANDLE  CreateOneAppMutex(LPCTSTR lpName);
	ATOM    MyRegisterClass(HINSTANCE hInstance);


	// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
	// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
};


