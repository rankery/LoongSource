#pragma once

#include "StaticEx.h"
#include "..\WorldDefine\msg_fatigue_guarder.h"

//健康提示类型
enum EHealthyState
{
	EHS_Healthy			=	0,		//健康时间
	EHS_Fatigue			=	1,		//疲劳时间
	EHS_Unhealthy		=	2		//不健康时间
};

/**
*class HealthyControlFrame
*brief 防沉迷系统框架
*author hyu
*remark
*base GameFrame
*/
class HealthyControlFrame :
	public GameFrame
{
public:
	HealthyControlFrame(void);
	~HealthyControlFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();

    void ShowWnd(BOOL bShown);

private:
	DWORD OnGameEvent(tagGameEvent* pGameEvent);

	DWORD EventHandler(tagGUIEvent* pEvent);

	DWORD OnNS_GetFatigueInfo(tagNS_GetFatigueInfo* pMsg, DWORD dwParam);

    DWORD OnNS_FatigueLimit(tagNS_FatigueLimit* pMsg, DWORD dwParam);

	//设置防沉迷状态
	VOID SetHealthyState(EHealthyState evalue);

    // 显示提示信息
    VOID ShowMsg(LPCTSTR szMsg);

	TSFPTrunk<HealthyControlFrame>	m_Trunk;
	TObjRef<GUISystem>				m_pGUI;
	TObjRef<Util>					m_pUtil;
	TObjRef<GameFrameMgr>			m_pFrameMgr;
	TObjRef<NetCmdMgr>				m_pCmdMgr;
	TObjRef<NetSession>				m_pSession;

	GUIWnd*							m_pWnd;
	StaticEx*						m_pStcTips;

};
