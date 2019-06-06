//-----------------------------------------------------------------------------
//!\file gui_httppic.h
//!\author Lyp
//!
//!\date 2004-12-08
//! last 2008-04-24
//!
//!\brief 界面系统http picture控件
//-----------------------------------------------------------------------------
#pragma once


namespace vEngine {
class HttpDownload;
//-----------------------------------------------------------------------------
// 界面系统static控件
//-----------------------------------------------------------------------------
class VENGINE_API GUIHttpPic : public GUIStatic
{
public:
	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Destroy();
	virtual VOID Render();
	virtual VOID Update();

	GUIHttpPic();
	virtual ~GUIHttpPic();

protected:
	//-----------------------------------------------------------------------------
	// 可编辑属性
	//-----------------------------------------------------------------------------


	HttpDownload* m_pHttp;


	
};

}