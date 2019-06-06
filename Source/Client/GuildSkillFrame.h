#pragma once

class IconStatic;
struct tagGuildSkill;

#define GUILD_SKILL_ITEM_NUM_PERPAGE 12
class GuildSkillFrame : public GameFrame
{
public:
	GuildSkillFrame();
	~GuildSkillFrame();
	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();
	void Show(bool bShow) { m_pPatch->SetInvisible((BOOL)(!bShow)); }
	void Refresh();
	void SetAllDefaultItemIcon();
private:
	BOOL EventHandler(tagGUIEvent *pEvent);
	
	
private:
	TSFPTrunk<GuildSkillFrame> m_Trunk;
	TObjRef<GUISystem> m_pGUI;
	TObjRef<GameFrameMgr> m_pGameFrameMgr;

	GUIWnd *m_pWndGuild;			// ¸¸´°Ìå



};




