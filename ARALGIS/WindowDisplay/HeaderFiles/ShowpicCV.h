#pragma once

#include "SapToGDI.h"

// CShowpic

class CShowpicCV : public CStatic
{
	DECLARE_DYNAMIC(CShowpicCV)

public:
	CShowpicCV();
	virtual ~CShowpicCV();

	SapBuffer *m_SapImg;
	SapToGDI *m_MatToGDI;


	// Implementation
public:
	bool GetSapImage(BYTE* SapImg, LONG width, LONG height, int bytePP);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};


