#pragma once
enum REBForChatInput
{
	EXUIE_ContextChange = EGUIE_User + 1024,
	EXUIE_ShowItemOrEquip,
};
class RichEditBoxForChatInput :
	public GUIRichEditBox
{
public:
	RichEditBoxForChatInput(void);
	~RichEditBoxForChatInput(void);
	//处理释放的物品
	virtual VOID OnEvent(tagGUIEvent* pEvent); 

	virtual BOOL OnInputMessage(tagGUIInputMsg* pMsg);
};
