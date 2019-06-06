#pragma once
#include "afxcmn.h"

//---------------------------------------------------------------
#ifndef _MEMDC_H_
//////////////////////////////////////////////////
// CMemDC - memory DC
//
// Author: Keith Rule
// Email: keithr@europa.com
// Copyright 1996-1997, Keith Rule
//
// You may freely use or modify this code provided this
// Copyright is included in all derived versions.
//
// History - 10/3/97 Fixed scrolling bug.
// Added print support.
//
// This class implements a memory Device Context
class CMemDC : public CDC
{
public:

	// constructor sets up the memory DC
	CMemDC(CDC* pDC) : CDC()
	{
		ASSERT(pDC != NULL);

		m_pDC = pDC;
		m_pOldBitmap = NULL;
		m_bMemDC = !pDC->IsPrinting();

		if (m_bMemDC) // Create a Memory DC
		{
			pDC->GetClipBox(&m_rect);
			CreateCompatibleDC(pDC);
			m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
			m_pOldBitmap = SelectObject(&m_bitmap);
			SetWindowOrg(m_rect.left, m_rect.top);
		}
		else // Make a copy of the relevent parts of the current DC for printing
		{
			m_bPrinting = pDC->m_bPrinting;
			m_hDC = pDC->m_hDC;
			m_hAttribDC = pDC->m_hAttribDC;
		}
	}

	// Destructor copies the contents of the mem DC to the original DC
	~CMemDC()
	{
		if (m_bMemDC) 
		{ 
			// Copy the offscreen bitmap onto the screen.
			m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
				this, m_rect.left, m_rect.top, SRCCOPY);

			//Swap back the original bitmap.
			SelectObject(m_pOldBitmap);
		} else {
			// All we need to do is replace the DC with an illegal value,
			// this keeps us from accidently deleting the handles associated with
			// the CDC that was passed to the constructor.
			m_hDC = m_hAttribDC = NULL;
		}
	}

	// Allow usage as a pointer
	CMemDC* operator->() {return this;}

	// Allow usage as a pointer
	operator CMemDC*() {return this;}

private:
	CBitmap m_bitmap; // Offscreen bitmap
	CBitmap* m_pOldBitmap; // bitmap originally found in CMemDC
	CDC* m_pDC; // Saves CDC passed in constructor
	CRect m_rect; // Rectangle of drawing area.
	BOOL m_bMemDC; // TRUE if CDC really is a Memory DC.
};

#endif

//------------------------------------------------------------
class CTextProgress :
	public CProgressCtrl
{
public:
	CTextProgress(void);
	virtual ~CTextProgress(void);

public:
	void		SetForeColour(COLORREF col);
	void		SetBkColour(COLORREF col);
	void		SetTextForeColour(COLORREF col);
	void		SetTextBkColour(COLORREF col);
	COLORREF	GetForeColour();
	COLORREF	GetBkColour();
	COLORREF	GetTextForeColour();
	COLORREF	GetTextBkColour();

	void		SetShowText(BOOL bShow);

	//{{AFX_MSG(CTextProgressCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	int			m_nPos; 
	INT			m_nStepSize;
	INT			m_nMax;
	INT			m_nMin;
	CString		m_strText;
	BOOL		m_bShowText;
	int			m_nBarWidth;
	COLORREF	m_colFore;
	COLORREF	m_colBk;
	COLORREF	m_colTextFore;
	COLORREF	m_colTextBk;

};
