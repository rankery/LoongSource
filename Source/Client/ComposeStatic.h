#pragma once
#include ".\GameClientDefine.h"

enum EOnEventSendDropGUI
{
	EOnEventSendDropGUI_Item = EGUIE_User + 1030,		
};


/*
	class ComposeStatic
	brief 生产强化static，只接收拖动按钮释放，不能拖动
*/
class ComposeStatic:
	public GUIStatic
{
public:
	ComposeStatic(void);
	~ComposeStatic(void);

	virtual BOOL Init(GUISystem* pSystem, GUIWnd* pFather, XmlElement* pXmlElement);
	virtual VOID Render();
	virtual VOID Update();
	virtual VOID Destroy();
	virtual BOOL OnInputMessage(tagGUIInputMsg* pMsg);
	virtual VOID OnEvent(tagGUIEvent* pEvent);

	//设置控件图片
	void SetPic(const tstring& strPicName);

protected:

	VOID DrawText();
	VOID DrawImg();

private:
	
};
