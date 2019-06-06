//-----------------------------------------------------------------------------
//!\file gui_button.h
//!\author Lyp
//!
//!\date 2004-12-08
//! last 2004-12-10
//!
//!\brief 界面系统button控件
//-----------------------------------------------------------------------------
#pragma once


namespace vEngine {
//-----------------------------------------------------------------------------
// 界面系统button控件
//-----------------------------------------------------------------------------
class VENGINE_API GUIButton : public GUIStatic
{
public:
	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Destroy();
	virtual VOID Render();
	virtual BOOL OnInputMessage(tagGUIInputMsg* pMsg);

	// 设置按钮是否有效
	virtual VOID SetEnable(bool b) { m_bDisable = !b; }
	virtual BOOL IsEnable() { return !m_bDisable; }

	// 得到按钮状态
	virtual EGUIButtonState	GetState() { return m_eState; }

	// 设置颜色
	virtual VOID SetMoveTextColor(DWORD dwColor) { m_dwMoveTextColor = dwColor; }
	virtual VOID SetPushTextColor(DWORD dwColor) { m_dwPushTextColor = dwColor; }

	GUIButton();

protected:

	tagGUIImage*	m_pPushPic;				// 按下图片handle
	tagGUIImage*	m_pMovePic;				// 经过图片handle
	tagGUIImage*	m_pDisablePic;			// 无效图片handle
	EGUIButtonState	m_eState;				// 按钮状态
	bool			m_bHaveMouseKeyDown;	// 是否曾经按下过鼠标

	//-----------------------------------------------------------------------------
	// 可编辑属性
	//-----------------------------------------------------------------------------
	BOOL			m_bDisable;				// 是否无效
	tstring			m_strPushPicFileName;	// 按下图片文件名
	tagRect			m_rcPushPic;			// 按下图片使用区域(图片坐标)
	DWORD			m_dwPushPicColor;		// 按下图片绘制颜色
	DWORD			m_dwPushTextColor;		// 按下文本颜色
	DWORD			m_dwPushShadeColor;		// 按下文字阴影颜色

	tstring			m_strMovePicFileName;	// 经过图片文件名
	tagRect			m_rcMovePic;			// 经过图片使用区域(图片坐标)
	DWORD			m_dwMovePicColor;		// 经过图片绘制颜色
	DWORD			m_dwMoveTextColor;		// 经过文本颜色
	DWORD			m_dwMoveShadeColor;		// 经过文字阴影颜色

	tstring			m_strDisablePicFileName;// 无效图片文件名
	tagRect			m_rcDisablePic;			// 无效图片使用区域(图片坐标)
	DWORD			m_dwDisablePicColor;	// 无效图片绘制颜色
	DWORD			m_dwDisableTextColor;	// 无效文本颜色
	DWORD			m_dwDisableShadeColor;	// 无效文字阴影颜色

	DWORD 			m_dwTopColor;			// 上部的颜色
	DWORD			m_dwBottomColor;		// 下部的颜色

};





}	// namespace vEngine {
