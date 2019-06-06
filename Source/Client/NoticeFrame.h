#pragma once

struct tagNS_RightNotice;
class NoticeFrame : public GameFrame
{	
public:
	NoticeFrame(void);
	~NoticeFrame(void);
	virtual BOOL Init(GameFrameMgr *pMgr, GameFrame *pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();

private:
	DWORD EventHandler(tagGUIEvent* pEvent);

	//--网络消息
	DWORD OnNS_RightNotice(tagNS_RightNotice* pMsg, DWORD dwParam);

private:
	TSFPTrunk<NoticeFrame>				m_Trunk;
	TObjRef<Util>						m_pUtil;
	TObjRef<NetCmdMgr>					m_pCmdMgr;
	TObjRef<NetSession>					m_pSession;
	TObjRef<GUISystem>					m_pGUI;
	TObjRef<GameFrameMgr>				m_pFrameMgr;

	GUIWnd*								m_pWnd;
	GUIButton*							m_pBtnClose;
	GUIStatic*							m_pStcTitle;
	GUIStaticEx*						m_pStcExContent;

	DWORD								m_dwCurTime;				//当前时间
	enum ENoticeState
	{
		ENoticeState_NULL		= 1,
		ENoticeState_Up,					//向上移动
		ENoticeState_Suspend,				//停留
		ENoticeState_Down,					//向下移动
	};
	ENoticeState						m_eNoticeState;
};