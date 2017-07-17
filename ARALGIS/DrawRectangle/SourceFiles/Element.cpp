// Element.cpp : implementation file
//

#include "stdafx.h"
#include "..\\..\\ARALGIS.h"
#include "..\\HeaderFiles\\Element.h"


// CElement

CElement::CElement()
{
}

CElement::~CElement()
{
}

/// alttaki her iki fonksiyon da olur - begins
/*
CElement::CElement(const CPoint& start, COLORREF colour, int penWidth) :
m_StartPoint{ start }, m_PenWidth{ penWidth }, m_Colour{ colour }{}
*/

CElement::CElement(const CPoint& start, COLORREF colour, int penWidth)
{
	m_StartPoint = start; 
	m_PenWidth = penWidth;
	m_Colour = colour;
}
/// alttaki her iki fonksiyon da olur - ends

// CElement member functions
