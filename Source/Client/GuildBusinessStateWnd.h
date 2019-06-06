#pragma once
class GuildBusinessStateWnd : public GameFrame
{
public:
	GuildBusinessStateWnd();
	~GuildBusinessStateWnd();
	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();
	VOID Show() {if (m_pWnd) {m_pWnd->SetInvisible(FALSE);}}
	VOID Hide() {if (m_pWnd) {m_pWnd->SetInvisible(TRUE);}}
private:
	VOID LoadGUI();
	TObjRef<GUISystem>				m_pGUI;
	GUIStatic						*m_pWnd;		
};