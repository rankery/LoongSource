#pragma once
#include "..\WorldDefine\creature_define.h"
#include "..\WorldDefine\msg_func_npc.h"
class NetSession;
/**
	class DakFrame
	brief 驿站
*/
class DakFrame :
	public GameFrame
{
public:
	DakFrame(void);
	~DakFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
	virtual BOOL EscCancel();

	void ShowWnd();
	void SetDakData(DWORD id, DWORD dwFunctionID);
	VOID SetCaption(LPCTSTR szCaption);

private:
	DWORD EventHandler(tagGUIEvent* pEvent);

	DWORD NetRecvNS_Dak(tagNS_Dak* pMsg, DWORD dwParam);

	//游戏事件
	DWORD OnUseDak(tagMsgBoxEvent* pGameEvent);

private:
	TSFPTrunk<DakFrame>			m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<GameFrameMgr>		m_pFrameMgr;
	TObjRef<NetSession>			m_pSession;
	TObjRef<NetCmdMgr>			m_pCmdMgr;

	GUIWnd						*m_pWnd;				//主界面
	GUIPatch					*m_pCaption;			//标题
	GUIStaticEx					*m_pStcCondition;		//对话内容窗口
	GUIListBox					*m_pListOptions;		//对话选项
	GUIButton					*m_pBtnCloseWnd1;		//关闭任务主界面按钮，上
	GUIButton					*m_pBtnCloseWnd2;		//关闭任务主界面按钮，下

	DWORD						m_dwNpcID;
	DWORD						m_dwFunctionID;
	DWORD						m_dwCurRow;
	DWORD						m_dwTotalRow;
	DWORD						m_escapeTime;
};