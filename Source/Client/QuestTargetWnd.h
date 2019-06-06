#pragma once

class QuestTargetWnd : public GameFrame
{
public:
	QuestTargetWnd();
	~QuestTargetWnd();
	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();

	VOID Show() {if (m_pWnd) {m_pWnd->SetInvisible(FALSE);}}
	VOID Hide() {if (m_pWnd) {m_pWnd->SetInvisible(TRUE);}}
private:
	BOOL EventHandler(tagGUIEvent *pEvent);
	VOID OnInstanceRequestToLeaveBox(tagMsgBoxEvent *pEvent);

	TSFPTrunk<QuestTargetWnd>		m_trunk;
	TObjRef<GUISystem>				m_pGUI;
	TObjRef<GameFrameMgr>			m_pFrameMgr;
	
	GUIWnd							*m_pWnd;			
	GUIStatic						*m_pTargetDesc;		//任务描述
	GUIButton						*m_pBtnOut;			//离开按钮
};
