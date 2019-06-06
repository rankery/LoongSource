//-----------------------------------------------------------------------------
//!\file gui_static.h
//!\author Lyp
//!
//!\date 2004-12-08
//! last 2004-12-10
//!
//!\brief 界面系统static控件
//-----------------------------------------------------------------------------
#pragma once


namespace vEngine {
//-----------------------------------------------------------------------------
// 界面系统static控件
//-----------------------------------------------------------------------------
class VENGINE_API GUIStatic : public GUIWnd
{
public:
	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Destroy();
	virtual VOID Render();
	virtual BOOL Resize(tagPoint& ptSize);
	virtual BOOL OnInputMessage(tagGUIInputMsg* pMsg);

	virtual VOID SetText(LPCTSTR szText){ m_strText = szText; SetRefresh(); }

	// 工具接口
	CONST tstring& GetText() { return m_strText; }
	VOID SetTextColor(DWORD dwColor) { m_dwTextColor = dwColor; }
	VOID SetShadeColor(DWORD dwColor) { m_dwShadeColor = dwColor; }
	VOID SetPicColor(DWORD dwColor) { m_dwPicColor = dwColor; }
	CONST tstring& GetTips() { return m_strTips; }
	VOID SetTips(LPCTSTR szText){ m_strTips = szText; SetRefresh(); }

	GUIStatic();

protected:
	tagGUIImage*		m_pPic;				// 图片
	bool				m_bPointInWnd;		// 鼠标是否在控件中

	//-----------------------------------------------------------------------------
	// 可编辑属性
	//-----------------------------------------------------------------------------
	tstring				m_strText;			// 文本
	tstring				m_strFont;			// 字体
	INT					m_nFontWidth;		// 字体宽度
	INT					m_nFontHeight;		// 字体高度
	INT					m_nFontWeight;		// 字体线宽
	tagRect				m_rcText;			// 文字区域
	DWORD				m_dwTextColor;		// 文本颜色
	DWORD				m_dwShadeColor;		// 文字阴影颜色
	BOOL				m_bNoAntiAliase;	// 文字是否关闭反走样
	BOOL				m_bTextWriteAlpha;	// 文字是否写Alpha通道
	EGUITextAlign		m_eTextAlign;		// 文字对齐方式

	tstring				m_strPic;			// 图片文件名
	tagRect				m_rcPic;			// 图片使用区域(图片坐标)
	DWORD				m_dwPicColor;		// 图片绘制颜色
	EGUIAlphaWrite		m_eAlphaWrite;		// Alpha通道写方式

	tstring				m_strTips;			// 工具提示

};





}	// namespace vEngine {
