#pragma once

#include "PkMattoGDI.h"

// CShowpic

class CShowpicCV : public CStatic
{
	DECLARE_DYNAMIC(CShowpicCV)

public:
	CShowpicCV();
	virtual ~CShowpicCV();

	cv::Mat m_cvImg;
	PkMatToGDI *m_MatToGDI;


	// Implementation
public:
	void GetcvImage(const cv::Mat &cvImg);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};


