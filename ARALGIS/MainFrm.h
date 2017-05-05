
// MainFrm.h : interface of the CMainFrame class
//

#pragma once


#include ".\\CameraInterface\\HeaderFiles\\CameraDataReceiver.h"
#include ".\\PTSCommunicator\\HeaderFiles\\PTSCommunicator.h"
#include ".\\OdroidCommunicator\\HeaderFiles\\OdroidCommunicator.h"
#include ".\\Database\\HeaderFiles\\CameraDBServer.h"
#include ".\\ImageFiltering\\ImageFilterProcessing\\HeaderFiles\\ImageFilterProcessing.h"
#include ".\\miniIni\\Processing\\HeaderFiles\\IniFileProcessing.h"

class CMainFrame : public CFrameWnd
{

protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

	// Attributes
public:

	CCameraDataReceiver    *m_CameraDataReceiver;
	CPTSCommunicator       *m_PTSCommunicator;
	COdroidCommunicator    *m_OdroidCommunicator;
	CCameraDBServer        *m_CameraDatabaseServer;
	CImageFilterProcessing *m_ImageFilterProcessing;

protected:  // control bar embedded members
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;
	HANDLE			m_hSemaphore;
	IniFileProcessing  m_IniFile;

	bool m_IsFirstTime;

	// Operations
public:
	static void CALLBACK NotifyProcPTSComm(LPVOID lpParam, UINT nCode);
	static void CALLBACK NotifyProcOdroidComm(LPVOID lpParam, UINT nCode);
	static void CALLBACK NotifyProcCameraComm(LPVOID lpParam, UINT nCode);
	static void CALLBACK NotifyProcImageFiltering(LPVOID lpParam, UINT nCode);

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


