#pragma once
#include "..\vEngine\gui\gui_pushbutton.h"

/** \class ItemMenuButton
	\brief 支持物品拖放的背包仓库页按钮
*/
class ItemMenuButton :
	public GUIPushButton
{
public:
	ItemMenuButton(void);
	~ItemMenuButton(void);

	virtual VOID OnEvent(tagGUIEvent* pEvent);
};
