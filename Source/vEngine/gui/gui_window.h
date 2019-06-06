//-----------------------------------------------------------------------------
//!\file gui_window.h
//!\author Lyp
//!
//!\date 2004-12-08
//! last 2007-06-15
//!
//!\brief 界面系统 window 控件
//-----------------------------------------------------------------------------
#pragma once


namespace vEngine {
//-----------------------------------------------------------------------------
// 界面系统 window 控件
//-----------------------------------------------------------------------------
class VENGINE_API GUIWindow : public GUIWnd
{
public:
	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Destroy();
	virtual VOID Render();
	virtual VOID Update();
	virtual BOOL OnInputMessage(tagGUIInputMsg* pMsg);
	virtual VOID SendEvent(tagGUIEvent* pEvent);
	
	virtual BOOL PointInWnd(tagPoint& pt, bool bCheckChild=true); // 指定点是否在本窗体内
	virtual BOOL Move(tagPoint& ptClient); // 移动控件,传入新坐标
	virtual VOID RefreshViewCoord(); // 刷新逻辑座标(view)

	virtual VOID AddChild(GUIWnd* pChild);
	virtual VOID DelChild(GUIWnd* pChild);
	virtual VOID SetInvisible(BOOL b);

	VOID SetZ(FLOAT z) { m_fZ = z; }

	VOID ReCalScroll();	// 重新计算Scroll坐标
	GUIButton* GetCloseButton() { return (GUIButton*)this->GetChild(m_strCloseButton.c_str()); }
	GUIWindow();

protected:
	tagGUIImage*			m_pNewCanvas;

	GUIScrollBar*			m_pScrollBar;
	INT						m_nMinY;
	INT						m_nMaxY;
	INT						m_nCurrentY;
	FLOAT					m_fZ;

	tagPoint				m_ptCurrentSize;		// 当前大小
	tagPoint				m_ptCurrentView;		// 当前位置
	FLOAT					m_fCurrentSizeRatio;	// 当前大小比例
	INT						m_nPopProgress;			// 是否处于放大状态 1放大 2缩小 0不变
	DWORD					m_dwPopStartTime;


	VOID ScrollChild(INT nY);	// 滚动子控件


	//-----------------------------------------------------------------------------
	// 可编辑属性
	//-----------------------------------------------------------------------------
	tstring			m_strCloseButton;		// 关闭按钮控件名
	BOOL			m_bRealDestroty;		// 是否真正关闭
	BOOL			m_bNotUseRenderTarget;	// 是否不使用渲染画布
	BOOL			m_bScroll;				// 是否支持滚动功能
	tstring			m_strPopupSound;		// 是否带弹出音效
	BOOL			m_bNoPopupEffect;		// 是否关闭弹出效果

};


}	// namespace vEngine {
