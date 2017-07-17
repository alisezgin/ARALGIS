#pragma once

#include <memory>

static const COLORREF SELECT_COLOR{ RGB(255, 0, 180) };

// CElement command target

class CElement : public CObject
{
protected:
	CPoint m_StartPoint; // Element Position
	int m_PenWidth; // Pen width
	COLORREF m_Colour; // Colour of an element
	CRect m_EnclosingRect; // Rectangle enclosing an element

public:
	virtual ~CElement();
	virtual void Draw(CDC* pDC, std::shared_ptr<CElement> pElement = nullptr) {}  // {} provides serialazition of the CObject class, so this is not pure virtual
	virtual void Move(const CSize& aSize) {}; // {} provides serialazition of the CObject class, so this is not pure virtual
	// Get the enclosing rectangle
	const CRect& GetEnclosingRect() const { return m_EnclosingRect; }

protected:
	// constructors protected so that they can not be called outside of the class
	CElement();
	CElement(const CPoint& start, COLORREF colour, int penWidth = 1);

	// Create a pen
	// putting function in header file of a class makes it inline, compiler repeates the code
	// everywhere it sees draw, makes the program fast but bigger size
	void CreatePen(CPen& aPen, std::shared_ptr<CElement> pElement)
	{
		//if (!aPen.CreatePen(PS_SOLID, m_PenWidth, 
		//	this == pElement.get() ? SELECT_COLOR: m_Colour))

		if (!aPen.CreatePen(this == pElement.get() ? PS_DASH : PS_SOLID, m_PenWidth,
			this == pElement.get() ? SELECT_COLOR : m_Colour))
		{
			// pen creation failed
			AfxMessageBox(_T("Pen Creation Failed"), MB_OK);
			AfxAbort();
		}
	}
};


