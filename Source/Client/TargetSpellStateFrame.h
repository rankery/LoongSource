#pragma once

class ActiveStatic;
class ActiveProgress;
struct tagRoleSpellStartEvent;
struct tagRoleSpellStopEvent;
struct tagRolePickEvent;

/** \class TargetSpellStateFrame
	\brief 目标起手状态界面
*/
class TargetSpellStateFrame : public GameFrame
{
public:
	TargetSpellStateFrame();
	~TargetSpellStateFrame();

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
private:
	VOID SafeDestroyUI();
	VOID Close();

	/** \游戏事件响应
	*/
	DWORD OnRoleSpellStartEvent(tagRoleSpellStartEvent* pEvent);
	DWORD OnRoleSpellStopEvent(tagRoleSpellStopEvent* pEvent);
	DWORD OnRolePickEvent(tagRolePickEvent* pEvent);
private:
	TSFPTrunk<TargetSpellStateFrame>	m_Trunk;
	TObjRef<GUISystem>					m_pGUI;
	GUIWnd*                             m_pWnd;
	ActiveStatic*						m_pIcon;			// 图标
	GUIStatic*							m_pName;			// 名称
	ActiveProgress*						m_pTimePro;			// 时间进度
	DWORD                               m_dwInterruptTime;  // 被打断的时间
	DWORD                               m_dwRoleID;         // 保存当前显示的角色ID
};