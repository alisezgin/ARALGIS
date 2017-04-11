#include "afxwin.h"


const COLORREF CLOUDBLUE = RGB(128, 184, 223);
const COLORREF WHITE = RGB(255, 255, 255);
const COLORREF BLACK = RGB(1, 1, 1);
const COLORREF DKGRAY = RGB(128, 128, 128);
const COLORREF LLTGRAY = RGB(220, 220, 220);
const COLORREF LTGRAY = RGB(192, 192, 192);
const COLORREF YELLOW = RGB(255, 255, 0);
const COLORREF DKYELLOW = RGB(128, 128, 0);
const COLORREF RED = RGB(255, 0, 0);
const COLORREF DKRED = RGB(128, 0, 0);
const COLORREF BLUE = RGB(0, 0, 255);
const COLORREF LBLUE = RGB(192, 192, 255);
const COLORREF DKBLUE = RGB(0, 0, 128);
const COLORREF CYAN = RGB(0, 255, 255);
const COLORREF DKCYAN = RGB(0, 128, 128);
const COLORREF GREEN = RGB(0, 255, 0);
const COLORREF DKGREEN = RGB(0, 128, 0);
const COLORREF MAGENTA = RGB(255, 0, 255);
const COLORREF DKMAGENTA = RGB(128, 0, 128);


class CClrButton : public CButton
{
	// Construction
public:
	  CClrButton();

	// Attributes
public:

	// Operations
public:
	// UK version
	void SetColour(COLORREF text_colour,
		           COLORREF background_colour);
	
	// US version
	void SetColor(COLORREF text_colour,
		          COLORREF background_colour)
	{
		SetColour(text_colour, background_colour);
	};

	// UK version
	void SetColour(COLORREF text_colour,
				   COLORREF background_colour,
				   COLORREF disabled_background_colour);

	// US version
	void SetColor(COLORREF text_colour,
				  COLORREF background_colour,
				  COLORREF disabled_background_colour)
	{
		SetColour(text_colour,
		          background_colour,
			      disabled_background_colour);
	};

	// UK version
	void ResetColour();

	// US version
	void ResetColor()
	{
	   ResetColour();
	}

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClrButton)
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CClrButton();

private:
	COLORREF text_colour, background_colour, disabled_background_colour;
	COLORREF light, highlight, shadow, dark_shadow;

	// Generated message map functions
protected:
	//{{AFX_MSG(CClrButton)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	void DrawFrame(CDC *dc, CRect r, int state);
	void DrawFilledRect(CDC *dc, CRect r, COLORREF colour);
	void DrawLine(CDC *dc, long sx, long sy, long ex, long ey, COLORREF colour);
	void DrawButtonText(CDC *dc, CRect r, TCHAR *buf, COLORREF text_colour);
	DECLARE_MESSAGE_MAP()
};

