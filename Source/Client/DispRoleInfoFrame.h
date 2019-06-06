#pragma once

#include "RoleInfoProtoData.h"
#include "..\WorldDefine\msg_role_vcard.h"
#include "HttpPicEx.h"
#include "InformationFrame.h"

struct tagRoleGetNameEvent;

struct tagOpenRoleInfoEvent : public tagGameEvent
{
    DWORD           dwRoleID;       //!< 角色id
    tagOpenRoleInfoEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) :
    tagGameEvent(szEventName, pSenderFrame) {}
};

/**
*class DispRoleInfoFrame
*brief 显示任意玩家名贴
*author hyu
*base GameFrame
*remark
*/
const DWORD TITLE_REFRESH_DELAY = 3000;     //!< 毫秒

class DispRoleInfoFrame :
	public GameFrame
{
public:
	DispRoleInfoFrame(void);
	~DispRoleInfoFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();

protected:
	VOID ShowWnd(BOOL bVal);

	//--处理GUI消息
	DWORD EventHandler(tagGUIEvent* pEvent);
	//--处理游戏事件
	DWORD OnOpenInfo(tagOpenRoleInfoEvent* pGameEvent);
	DWORD OnCloseInfo(tagGameEvent* pGameEvent);

    VOID SendGetRoleVCard();
    //处理获取角色名贴数据网络消息
    DWORD OnNS_GetRoleVCard(tagNS_GetRoleVCard* pMsg, DWORD dwParam);
    DWORD OnRoleGetNameEvent(tagRoleGetNameEvent* pGameEvent);

    //清除
    VOID ClearAll();

    DWORD       m_dwCurrRoleID;     // 当前请求名贴的roleID
    bool        m_bisRefreshing;    // 是否正在刷新
    DWORD       m_dwCurrMateID;     // 当前请求查询配偶名字的ID

	TSFPTrunk<DispRoleInfoFrame>	m_Trunk;
	TObjRef<GUISystem>				m_pGUI;
	TObjRef<Util>					m_pUtil;
	TObjRef<GameFrameMgr>			m_pFrameMgr;
    TObjRef<NetSession>				m_pSession;
    TObjRef<NetCmdMgr>				m_pCmdMgr;

	GUIWnd*							m_pWnd;

    GUIStatic*                      m_pStcName;             //名字
	GUIStatic*						m_pStcSpouse;			//配偶
	GUIStatic*						m_pStcGender;			//性别
	GUIStatic*						m_pStcConstellation;	//星座
	GUIStatic*						m_pStcCity;				//城市
	GUIStatic*						m_pStcChZodiac;			//生肖
	GUIEditBoxEx*					m_pEditBoxExPresent;	//个性签名
	GUIEditBoxEx*					m_pEditNameHistory;		//更名记录
	HttpPicEx*						m_pStcHeadPic;			//头像

	GUIButton*						m_pBtRefresh;			//刷新
	GUIButton*						m_pBtClose;				//关闭

	GUIButton*						m_pBtQuit;				//关闭窗口
};
