#pragma once

class RoleHeadTipsStatic:
	public GUIStatic
{
public:
	RoleHeadTipsStatic(void);
	//--
	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Destroy();
	virtual VOID Render();
	
	//¸Ä±ä×ÖÌå
	BOOL ResizeFont(const TCHAR* szInfo, INT nWidth, INT nHeight, INT nWeight=400);

};