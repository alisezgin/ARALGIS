#pragma once

#include "..\\HeaderFiles\\PkMattoGDI.h"

// CShowpic

class CShowpic : public CStatic
{
	DECLARE_DYNAMIC(CShowpic)

public:
	CShowpic();
	virtual ~CShowpic();

	int m_un32ImageWidth;
	int m_un32ImageHeight;
	int m_un32ImageBits;
	BYTE *m_pImageBytes;

	BITMAPINFO* m_pBmp;

	DWORD m_iSizeImage;

	BOOL m_bImageCaptured{ FALSE };

	// Implementation
public:
	void ShowImage(BYTE *pImageByte);
	void MakeBMPHeader();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};


