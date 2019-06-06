#pragma once

class ToolTipStatic;
class MouseIntoTipsFrame : public GameFrame
{
public:
	MouseIntoTipsFrame(void);
	~MouseIntoTipsFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual VOID Update(); 
	virtual BOOL ReloadUI();

	//在指定位置上显示信息
	VOID ShowTips(tagPoint ptPos, LPCTSTR szTip);

private:
	tstring GetMouseIntoRoleNameFont();
	tstring GetMouseIntoRoleOtherFont();

	tstring GetMouseIntoIsAttackColor(DWORD dwType);
	tstring GetMouseIntoMonsterColor(INT nIn);

	VOID ResetWindowPoint(tagPoint ptSize, tagPoint ptPos);

private:
	TSFPTrunk<MouseIntoTipsFrame>		m_Trunk;
	TObjRef<GUISystem>					m_pGUI;
	TObjRef<Util>						m_pUtil;
	TObjRef<GameFrameMgr>				m_pFrameMgr;
	TObjRef<GUIRender>					m_pRender;

	GUIWnd*								m_pWnd;						// Tips界面
	ToolTipStatic*						m_pStcTips;					// Tips内容显示控件
	DWORD								m_dwMouseIntoID;			//鼠标指向的id
	tagPoint							m_clientPoint;								
};