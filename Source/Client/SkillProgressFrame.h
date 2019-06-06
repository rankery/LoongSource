#pragma once

class NetSession;
class IconStatic;
struct tagSkillProgressEvent;
/** class SkillProgressFrame
	brief 技能发动时进度条
*/
class SkillProgressFrame 
	: public GameFrame
{
public:
	enum ESkillProPhase
	{
		ESkillProPhase_NULL,
		ESkillProPhase_RecvStartEvent,
		ESkillProPhase_RecvCancelEvent,
		ESkillProPhase_Grade,					//渐隐
	};
public:
	SkillProgressFrame(void);
	~SkillProgressFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
	virtual VOID OnEvent(tagGameEvent* pEvent);

private:
	// true:显示  false:隐藏
	void ShowWnd(BOOL bVal) ;

	BOOL EventHandler(tagGUIEvent* pEvent);

	DWORD OnSkillProgressEvent(tagSkillProgressEvent* pEvent);

	BYTE CalSkillProGressLerp(float fCurTime, float fStartTime, float fHoldTime);

private:
	TSFPTrunk<SkillProgressFrame>		m_Trunk;
	TObjRef<GUISystem>					m_pGUI;
	TObjRef<Util>						m_pUtil;
	TObjRef<GameFrameMgr>				m_pFrameMgr;
	TObjRef<NetSession>					m_pSession;
	TObjRef<NetCmdMgr>					m_pCmdMgr;

	GUIWnd*								m_pWndMain;
	GUIStatic*							m_pStcHint;
	IconStatic*							m_pStcIcon;
	GUIStatic*							m_pProBack;
	GUIProgress*						m_pSkillPro;

	ESkillProPhase						m_eState;
	FLOAT								m_fStartTime;

};