#pragma once
#include "iconstaticex.h"

class IconStaticHL : public IconStatic
{
public:
	IconStaticHL(void);
	~IconStaticHL(void);

	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual BOOL OnInputMessage( tagGUIInputMsg* pMsg );
	virtual VOID Render();
	virtual VOID Destroy();

	void SetHighLightPic( LPCTSTR szPath );
	
	bool IsHighLight(){ return P_VALID(m_pHighLightPic); }

protected:
	void DrawHighLight();

protected:
	tagGUIImage*		m_pHighLightPic;
};
