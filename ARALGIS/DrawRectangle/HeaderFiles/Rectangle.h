#pragma once
#include "Element.h"
class CRectangle :
	public CElement
{
public:
	virtual ~CRectangle();
	virtual void Draw(CDC* pDC, std::shared_ptr<CElement> pElement = nullptr) override;  // function to display rectangle
	virtual void Move(const CSize& aSize) override; // function to move an element

	// constructor
	CRectangle(const CPoint& start, const CPoint& end, COLORREF colour, int penWidth);

protected:
	CPoint m_BottomRight;
	CRectangle();
};

