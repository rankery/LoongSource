#pragma once

#include ".\ChatSysDefine.h"

/**
	表情，从emotion.xml中读取表情转义符以及中文解释
*/
class GameFrame;
//聊天窗口的游戏事件
struct tagGameEventForChat:public tagGameEvent
{
	tstring		strEmotion;
	tagGameEventForChat(LPCTSTR szEventName, GameFrame* pSenderFrame, LPCTSTR szEmotion)
		:tagGameEvent(szEventName, pSenderFrame)
	{
		strEmotion = szEmotion;	
	}
};

class EmotionFrame:
	public GameFrame
{
public:
	EmotionFrame(void);
	~EmotionFrame(void);
	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();

	//从配置文件中读取表情的转义字符以及中文诠释
	VOID LoadEmotionFile(const tstring& szFile);
	
	//显示\隐藏窗口
	VOID SetInVisibleFrame(bool bVal); 
	BOOL IsInvisibleFrame() { return m_pWnd->IsInvisible(); }

	//得到表情的中文诠释
	const TCHAR* GetEmtionTran(int index);

	//
	const TCHAR* GetEmotionMean(int index);

private:

	//GUI事件处理函数
	BOOL EventHandler(tagGUIEvent* pEvent);

private:
	TSFPTrunk<EmotionFrame>		m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<GameFrameMgr>		m_pFrameMgr;

	GUIWnd*						m_pWnd;
	GUIWnd*						m_pWndCaption;
	GUIButton*					m_pBtnEmotion[MAX_EMOTION];		//表情按钮(60个)

	vector<tstring>				m_vecEmotionTran;		//表情的中文诠释
	vector<tstring>				m_vecEmotionMean;		//表情的转义符
};
