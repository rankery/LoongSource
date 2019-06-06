#pragma once

struct tagUpdateLPItemEvent;
class ActiveStatic;
class EquipRecencyFrame : public GameFrame
{
public:
	enum EEquipReceny
	{
		EER_NULL = 0,
		EER_RecenyLess, 
		EER_RecenyOld,
		EER_RecenyBad,
	};
public:
	EquipRecencyFrame(void);
	~EquipRecencyFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();

private:
	// 处理GUI消息
	DWORD EventHandler(tagGUIEvent* pEvent);
	DWORD OnUpdateLPItemEvent(tagUpdateLPItemEvent* pGameEvent);

private:
	TSFPTrunk<EquipRecencyFrame>	m_Trunk;
	TObjRef<GUISystem>				m_pGUI;
	TObjRef<Util>					m_pUtil;
	TObjRef<GameFrameMgr>			m_pFrameMgr;

	GUIWnd*							m_pWnd;
	ActiveStatic*					m_pAS_Recency;
	EEquipReceny					m_eReceny;

};