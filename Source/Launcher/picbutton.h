//-----------------------------------------------------------------------------
// File: picbutton.h
// Time: 2006-6-18
// Auth: tidy
// Desc: picture button for mfc ui

// Copyright (c) 2006 Softstar Entertainment All rights reserved.
//-----------------------------------------------------------------------------

#pragma once


// CPicButtion

class CPicButtion : public CButton
{
	DECLARE_DYNAMIC(CPicButtion)

public:
	CPicButtion();
	virtual ~CPicButtion();

public:
	VOID SetSelBitmap(LPCTSTR szPicPath);		//设置选中状态下的位图
	VOID SetHoverBitmap(LPCTSTR szPicPath);
	VOID SetNormalBitmap(LPCTSTR szPicPath,bool autosize=false);
	VOID SetDisableBitmap(LPCTSTR szPicPath);

	//文字区域
	VOID SetTextRect(RECT& rect);
	//文字大小
	VOID SetTextSize(INT nSize);
	//文字是否居左
	VOID SetTextLeft(BOOL bTextLeft){ m_bTextLeft = bTextLeft; }

	//设置文字颜色
	VOID SetTextColor(DWORD dwTextColor);
	//文字描边的颜色
	VOID SetTextEfColor(DWORD dwTextEfColor) { m_dwTextEfColor = dwTextEfColor; }
	//鼠标经过时文字描边的颜色
	VOID SetTextHoverEfColor(DWORD dwTextEfColor) { m_dwTextHoverEfColor = dwTextEfColor; }
	//选中状态文字描边的颜色
	VOID SetTextSelEfColor(DWORD dwTextEfColor) { m_dwTextSelEfColor = dwTextEfColor; }
	//按钮无效时的文字颜色
	VOID SetDisableTextColor(DWORD dwTextColor){  m_dwDisableTextColor = dwTextColor;}
	//描边处理
	VOID RenderText(CDC* pDC, DWORD dwTextColor, CString &strText);

private:
	BOOL		m_bHover;			//鼠标是否在按钮上	
	BOOL		m_bTracking;

	CBitmap*	m_pBitmapSel;		//选择状态
	CBitmap*	m_pBitmapHover;		//鼠标经过状态
	CBitmap*	m_pBitmapNormal;	//正常状态
	CBitmap*	m_pBitmapDisable;	//不可用状态

	DWORD		m_dwTextColor;
	DWORD		m_dwTextEfColor;
	DWORD		m_dwTextHoverEfColor;
	DWORD		m_dwTextSelEfColor;

	DWORD		m_dwDisableTextColor;
	RECT		m_RectText;
	BOOL		m_bTextLeft;

	CFont*		m_pFont;

	HBITMAP		m_dwHandleBitmap[4];
	bool		m_bAutoSize;		//是否自动控制大小
	bool		m_bIsCreate;		//是否已经建立
public:
	char		Stretch;
	int			m_dwWidth;			//控件宽度
	int			m_dwHeight;			//控件高度
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);  //自绘
	virtual void PreSubclassWindow();	//创建之前进行初始化
	virtual BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	


protected:

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);	
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam);
//	afx_msg BOOL OnEraseBkgnd(CDC* pDC) { return TRUE; }

	DECLARE_MESSAGE_MAP()
};


