#pragma once
#include "Render2D.h"
#include "StaticNoInput.h"
#include "RoleEvent.h"
#define MAX_SCENE_CENTER_INFO 3

struct tagCenterInfo
{
	tstring		strText;				//要显示的信息
	DWORD		dwTime;					//接收显示信息的时间
	tagPoint	txtSize;				//显示信息的大小
	DWORD		dwColor;				//信息的颜色
	int			nNum;

	tagCenterInfo(void)
	{
		strText		= _T("");
		dwTime		= GT_INVALID;
		dwColor		= 0x0;
	}
};
struct tagEnterMapAreaEvent;
class SceneCenterInfoFrame :
	public GameFrame
{
public:
	SceneCenterInfoFrame(void);
	~SceneCenterInfoFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
	virtual VOID OnEvent(tagGameEvent* pEvent);

	void AddCenterinfo(const TCHAR* szInfo);

	void AddCenterinfoEx(const TCHAR* szInfo);

	DWORD OnUpdateLPLevel(tagUpdateLPLevel* pEvent);
	DWORD OnEnterMapAreaEvent(tagEnterMapAreaEvent* pEvent);

private:
	void SortSceneCenterInfo();
	void SortSceneCenterInfoEx();

	void CreateAreaWnd();

private:
	TSFPTrunk<SceneCenterInfoFrame>		m_Trunk;
	TObjRef<GUISystem>					m_pGUI;
	TObjRef<Util>						m_pUtil;
	TObjRef<GameFrameMgr>				m_pFrameMgr;
	TObjRef<NetSession>					m_pSession;
	TObjRef<NetCmdMgr>					m_pCmdMgr;
	TObjRef<GUIRender>					m_pRender;

	vector<tagCenterInfo>				m_vecCenterInfo;
	GUIStaticEx*						m_pWnd;
	GUIStaticEx*						m_pStcInfo[MAX_SCENE_CENTER_INFO];

	GUIStaticEx*						m_pWndEx;
	GUIStaticEx*						m_pStcInfoEx[MAX_SCENE_CENTER_INFO];

	GUIStaticEx*						m_pWndArea;
	DWORD								m_dwAreaTime;

	//StaticNoInput*					m_pStaInfoEx;
	//tagCenterInfo						m_CenterInfoEx;
	vector<tagCenterInfo>				m_vecCenterInfoEx;
};