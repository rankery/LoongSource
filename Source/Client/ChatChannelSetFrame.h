#pragma once
#include "ChatSysDefine.h"

class ChatChannelSetFrame : public GameFrame
{
public: 
	ChatChannelSetFrame(void);
	~ChatChannelSetFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID OnEvent(tagGameEvent* pEvent);
	virtual VOID Update();
	virtual BOOL EscCancel();

	void CheckChatChannelSetParam(DWORD *dwParam);

private:
	//控件事件处理函数
	DWORD EventHandler(tagGUIEvent* pEvent);

	//当前频道的设置
	void CurrentChannelSet(INT nChannel);

	//设置当前频道
	void SetChannelParam();

	ERecvChatChannel TransChannelParam(INT nParam);

private:
	TSFPTrunk<ChatChannelSetFrame>		m_Trunk;
	TObjRef<GUISystem>					m_pGUI;
	TObjRef<Util>						m_pUtil;
	TObjRef<GameInputMap>				m_pKeyMap;
	TObjRef<NetSession>					m_pSession;
	TObjRef<NetCmdMgr>					m_pCmdMgr;

	GUIWnd*								m_pWnd;
	GUIButton*							m_pBtn_Exit;
	GUIButton*							m_pBtn_OK;
	GUIButton*							m_pBtn_Cancel;
	GUIPushButton*						m_pPBtn_Channel[NUM_CHAT_CHANNEL-1];
	GUIPushButton*						m_pPBtn_ChannelSet[NUM_CHAT_CHANNEL-1];
	DWORD								m_dwChannelSet[NUM_CHAT_CHANNEL-1];			//每个频道接收数据类型	
	INT									m_nCurChannel;
};