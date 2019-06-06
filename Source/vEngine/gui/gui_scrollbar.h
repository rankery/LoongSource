//-----------------------------------------------------------------------------
//!\file gui_scrollbar.h
//!\author Lyp
//!
//!\date 2004-12-08
//! last 2007-04-28
//!
//!\brief 界面系统 scroll bar 控件
//-----------------------------------------------------------------------------
#pragma once


namespace vEngine {
//-----------------------------------------------------------------------------
// 界面系统scrollbar控件
//-----------------------------------------------------------------------------
class VENGINE_API GUIScrollBar : public GUIStatic
{
public:
	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Destroy();
	virtual VOID Render();
	virtual BOOL OnInputMessage(tagGUIInputMsg* pMsg);
	virtual VOID OnEvent(tagGUIEvent* pEvent);
	virtual VOID SendEvent(tagGUIEvent* pEvent);

	virtual VOID AddChild(GUIWnd* pChild);	// 添加子窗口
	virtual VOID DelChild(GUIWnd* pChild);	// 删除子窗口

	VOID SetValue(INT nTotal, INT nVisible, INT nCurrent)
	{ 
		m_nTotalValue = nTotal;
		m_nVisibleValue = nVisible;
		m_nCurrentValue = nCurrent;
		CalStick();
	}
	
	VOID SetValue(INT nCurrent) { m_nCurrentValue = nCurrent; CalStick(); }
	INT GetValue() { return m_nCurrentValue; }

	GUIScrollBar();

protected:

	GUIWnd*		m_pUpButton;
	GUIWnd*		m_pDownButton;
	INT			m_nStickStartPos;			// 滚动棒起始位置
	INT			m_nStickEndPos;				// 滚动棒起始位置+滚滚动棒长度

	tagGUIImage*	m_pTopPic;				// 上部图片handle
	tagGUIImage*	m_pMidPic;				// 中部图片handle
	tagGUIImage*	m_pBelowPic;			// 下部图片handle

	VOID		CalStick();					// 计算滚动棒的长度起始位置等

	//-----------------------------------------------------------------------------
	// 可编辑属性
	//-----------------------------------------------------------------------------
	BOOL			m_bHorizon;				// 是否水平
	INT				m_nTotalValue;			// 总量
	INT				m_nVisibleValue;		// 可显示的量
	INT				m_nCurrentValue;		// 当前量

	tstring			m_strTopPic;			// 上部图片文件名
	tagRect			m_rcTopPic;				// 上部图片使用区域(图片坐标)
	DWORD			m_dwTopPicColor;		// 上部图片绘制颜色
	tstring			m_strMidPic;			// 中部图片文件名
	tagRect			m_rcMidPic;				// 中部图片使用区域(图片坐标)
	DWORD			m_dwMidPicColor;		// 中部图片绘制颜色
	tstring			m_strBelowPic;			// 下部图片文件名
	tagRect			m_rcBelowPic;			// 下部图片使用区域(图片坐标)
	DWORD			m_dwBelowPicColor;		// 下部图片绘制颜色

	DWORD 			m_dwTopColor;			// 上部的颜色
	DWORD			m_dwBottomColor;		// 下部的颜色


};

}	// namespace vEngine {
