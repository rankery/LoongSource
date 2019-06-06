#pragma once
#include "..\vEngine\gui\gui_static.h"

/** \class LockStatic
	\brief 交易界面锁定控件，用于锁定交易框中的点击事件，但不阻挡鼠标经过事件
*/
class LockStatic :
	public GUIStatic
{
public:
	LockStatic(void);
	~LockStatic(void);

	virtual BOOL OnInputMessage(tagGUIInputMsg* pMsg);

private:

	GUIWnd*			m_pLastWnd;//上一个进入的控件
};
