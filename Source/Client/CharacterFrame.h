#pragma once

class StateFrame;
class EquipmentFrame;
class ReputeFrame;
class RoleTitleFrame;
class InformationFrame;

class CharacterFrame :
	public GameFrame
{
public:
	CharacterFrame(void);
	~CharacterFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();

	tstring GetGUIFullName()
	{
		if(P_VALID(m_pWndCaption))
			return m_pWndCaption->GetFullName();
		else 
			return _T("");
	};

    tstring GetStateWndFullName()
    {
        if (P_VALID(m_pWnd))
            return m_pWnd->GetFullName();
        else
            return _T("");
    }

protected:

	VOID ShowWnd(BOOL bVal);

	//--处理GUI消息
	DWORD EventHandler(tagGUIEvent* pEvent);
	//--处理游戏事件
	DWORD OnOpenCharacter(tagGameEvent* pGameEvent);
	DWORD OnCloseCharacter(tagGameEvent* pGameEvent);

    DWORD OnOpenEquipState(tagGameEvent* pEvent);
    DWORD OnOpenInfomation(tagGameEvent* pEvent);
    DWORD OnOpenReputation(tagGameEvent* pEvent);
    DWORD OnOpenTitle(tagGameEvent* pEvent);
private:
	TSFPTrunk<CharacterFrame>	m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<GameFrameMgr>		m_pFrameMgr;

	StateFrame*					m_pState;
	EquipmentFrame*				m_pEquipment;
	ReputeFrame*				m_pRepute;
	RoleTitleFrame*				m_pRoleTitle;
	InformationFrame*			m_pInformation;

	GUIWnd*						m_pWnd;
	GUIWnd*						m_pWndCaption;
	GUIWnd*						m_pWndState;
	GUIWnd*						m_pWndCareer;
	GUIWnd*						m_pWndRepute;
	GUIWnd*						m_pWndTitle;
	GUIWnd*						m_pWndRoleTitle;
	GUIPushButton*				m_pPbnState;
	GUIPushButton*				m_pPbnCareer;
	GUIPushButton*				m_pPbnRepute;
	GUIPushButton*				m_pPbnRoleTitle;

	GUIButton*					m_pBtnClose;
};
