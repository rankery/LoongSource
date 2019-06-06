#pragma once
#define MAX_SPECIAL_BUTTON_NUM 8

class SpecialSkillButton;
struct tagRoleBuffChangeEvent;
struct tagRolePickEvent;
struct tagNS_Skill;

class SpecialSkillFrame : public GameFrame
{
public:
	SpecialSkillFrame(void);
	~SpecialSkillFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID OnEvent(tagGameEvent* pEvent);
	virtual VOID Update();

	void SetActiveState(BOOL bActive) { m_bActive = bActive; }

private:
	//控件事件处理函数
	DWORD EventHandler(tagGUIEvent* pEvent);

	//游戏事件
	DWORD OnRoleBuffChangeEvent(tagRoleBuffChangeEvent* pEvent);
	DWORD OnRolePickEvent(tagRolePickEvent* pEvent);

	//网络消息
	DWORD OnNS_Skill(tagNS_Skill* pMsg, DWORD dwParam);

	//移除所有连接技
	void RemoveAllSpecialSkill();

	//设置连接技
	void SetSelfSpecialSkill();
	void SetOtherSpecialSkill( DWORD dwRoleID );

	DWORD OnKey(DWORD dwID, BOOL bDown);
	DWORD OnKeyChange(DWORD dwID, DWORD key1, DWORD key2);

	//传入的技能ID是带等级的
	BOOL IsHaveSpecialSkill(DWORD dwSkillID);

	// 由DIK得到按键字符串名
	LPCTSTR GetKeyName(DIK key);

private:
	TSFPTrunk<SpecialSkillFrame>	m_Trunk;
	TObjRef<GUISystem>				m_pGUI;
	TObjRef<Util>					m_pUtil;
	TObjRef<GameInputMap>			m_pKeyMap;
	TObjRef<NetSession>				m_pSession;
	TObjRef<NetCmdMgr>				m_pCmdMgr;

	GUIWnd*							m_pWnd;
	GUIButton*						m_pCancel;
	SpecialSkillButton*				m_pBtn_Skill[MAX_SPECIAL_BUTTON_NUM];

	static TCHAR*					m_szAccArray[MAX_SPECIAL_BUTTON_NUM];
	static int						m_nAccArray[MAX_SPECIAL_BUTTON_NUM];
	BOOL							m_bActive;					//连接技能界面是否处于激活状态
	DWORD							m_nCurButton;
};

inline SpecialSkillFrame* GetSpecialSkillFrame()
{
	return (SpecialSkillFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("SpecialSkill"));
}