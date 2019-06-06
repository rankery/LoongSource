//-----------------------------------------------------------------------------
//!\file gui_staticex.h
//!\author Lyp
//!
//!\date 2004-12-08
//! last 2008-06-17
//!
//!\brief 界面系统static ex控件
//-----------------------------------------------------------------------------
#pragma once


namespace vEngine {
//-----------------------------------------------------------------------------
// 界面系统 static ex 控件
// 支持各种转意字符
// <font=type,width,height,weight>:设置字体，例如<font=Arial,0,16,400>
// <pic=filename,left,top,right,bottom>:图片文件名，使用图片的哪个区域来显示
// <color=0xFFFFFFFF>:设置文字或者图片颜色
// <link=id,LineColor>:显示一个超链接，用户点击后会发送带id信息的事件,
// 用<link=0xffffffff,0>标志链接结束
//-----------------------------------------------------------------------------
class VENGINE_API GUIStaticEx : public GUIStatic
{
public:
	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Destroy();
	virtual VOID Render();
	virtual BOOL OnInputMessage(tagGUIInputMsg* pMsg);
	virtual BOOL Resize(tagPoint& ptSize);

	virtual VOID SetText(LPCTSTR szText){ m_strText = szText; ParseText(); SetRefresh(); }

	// 得到内容的总高度
	INT	GetTextTotalHeight() { return m_nTotalHeight; }	

	GUIStaticEx();

protected:
	struct tagStaticExItem
	{
		tagGUIFont*		pFont;
		tagGUIImage*	pImage;
		tstring			strText;
		DWORD			dwColor;
		DWORD			dwLinkID;
		DWORD			dwLinkColor;
		tagPoint		ptPos;
		tagPoint		ptSize;

		tagStaticExItem() { pFont = NULL; pImage = NULL; dwLinkID = GT_INVALID; }
	};

	std::list<tagStaticExItem*>	m_listItems;
	std::list<tagStaticExItem*>::iterator m_itCurrent;

	TList<tagGUIFont*>		m_listFont;
	TList<tagGUIImage*>		m_listImage;

	INT						m_nTotalHeight;

	// 把字符串解析成items
	VOID ParseText();

	// 把字符串解析成关键字
	BOOL ParseKey(tstring& strKey, tstring& strValue, tagStaticExItem* pItem);

	// 清空所有资源
	VOID ClearResource();


	//-----------------------------------------------------------------------------
	// 可编辑属性
	//-----------------------------------------------------------------------------
	INT			m_nRowHeight;					// 行高
};





}	// namespace vEngine {
