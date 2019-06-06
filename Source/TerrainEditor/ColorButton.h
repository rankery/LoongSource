#pragma once


// CColorButton

class CColorButton : public CButton
{
	DECLARE_DYNAMIC(CColorButton)

public:
	CColorButton();
	virtual ~CColorButton();

	void SetColor(COLORREF color);
	COLORREF GetColor();

protected:
	DECLARE_MESSAGE_MAP()
	COLORREF	m_color;
public:
	afx_msg void OnPaint();
	
};


