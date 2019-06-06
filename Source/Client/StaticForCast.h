#pragma once
enum ESFCEvent
{
	EXUIE_SFCMouseClick = EGUIE_User + 1050,
	EXUIE_SFCMouseRClick,
};
class StaticForCast : public GUIStatic
{
public:
	StaticForCast(void);
	~StaticForCast(void);

	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Destroy();
	virtual BOOL OnInputMessage(tagGUIInputMsg* pMsg);
};