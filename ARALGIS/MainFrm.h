
// MainFrm.h : interface of the CMainFrame class
//

#pragma once


#include ".\\CameraInterface\\HeaderFiles\\CameraDataReceiver.h"
#include ".\\PTSCommunicator\\HeaderFiles\\PTSCommunicator.h"
#include ".\\OdroidCommunicator\\HeaderFiles\\OdroidCommunicator.h"



class CMainFrame : public CFrameWnd
{

protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

	// Attributes
public:

	CCameraDataReceiver *m_CameraDataReceiver;
	CPTSCommunicator  *m_PTSCommunicator;
	COdroidCommunicator *m_OdroidCommunicator;

protected:
	bool m_IsFirstTime;

	// Operations
public:
	static void CALLBACK NotifyProcPTSComm(LPVOID lpParam, UINT nCode);
	static void CALLBACK NotifyProcOdroidComm(LPVOID lpParam, UINT nCode);

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

protected:  // control bar embedded members
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;

	HANDLE			m_hMutex;

	// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);

};


