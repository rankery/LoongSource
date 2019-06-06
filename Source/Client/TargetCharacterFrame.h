#pragma once
#include "gameframe.h"
#include "SocialEvent.h"
#include "ReputeMgr.h"
#include "RoleTitleProtoData.h"
#include "RoleTitleFrame.h"
#include "TreeEx.h"
class RemoteRoleStateFrame;
#include "..\WorldDefine\msg_role_title.h"
#include "..\WorldDefine\msg_reputation.h"

class TargetCharacterFrame :
    public GameFrame
{
public:
    TargetCharacterFrame(void);
    ~TargetCharacterFrame(void);

    virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
    virtual BOOL Destroy();
    virtual BOOL ReloadUI();
    virtual VOID Update();
	virtual BOOL EscCancel();

    tstring GetGUIFullName()
    {
        if(P_VALID(m_pWnd))
            return m_pWnd->GetFullName();
        else 
            return _T("");
    };

private:
    //! 处理GUI消息
    DWORD EventHandler(tagGUIEvent* pEvent);

    //! 显示目标玩家装备,声望,称号界面
    DWORD OnDispPlayerState(tagDispPlayerState* pMsg);

    //! 显示窗体
    VOID ShowWnd(BOOL bVal);

    //! 重置声望显示
    VOID ResetRepute();
    //! 发送获得声望请求
    VOID BeginNC_GetReputation(DWORD dwRoleID);
    //! 返回声望结果
    DWORD OnNS_GetReputation(tagNS_GetReputation* pMsg, DWORD);

    //! 重置称号显示
    VOID ResetTitle();
    //! 发送称号获得请求
    VOID BeginNC_GetRoleTitles(DWORD dwRoleID);
    //! 返回称号结果
    DWORD OnNS_GetRoleTitles(tagNS_GetRoleTitles* pMsg, DWORD);
    //! 初始化所有称号xml数据到控件
    VOID InitTree();
    //! 跟据称号变化刷新控件
    VOID UpdateTree();
    //! 选中某一称号
    VOID SelectRoleTitle();

	// 重置装备
	VOID ResetRemoteRoleEquip();

    map<DWORD, tagTitleList>		m_mapOwnedTitles;		/*!< 保存角色已获得称号的容器 */
    map<DWORD, tagTitleTypeActivedCount> m_mapActivedCount; /*!< 此类title获得的数量 */

    TSFPTrunk<TargetCharacterFrame> m_Trunk;
    TObjRef<GUISystem>			    m_pGUI;
    TObjRef<Util>				    m_pUtil;
    TObjRef<GameFrameMgr>		    m_pFrameMgr;
    TObjRef<NetCmdMgr>				m_pCmdMgr;
    TObjRef<NetSession>				m_pSession;

    // 主窗体

    GUIWnd*						    m_pWnd;             //!< 主窗体
    GUIWnd*                         m_pEquip;           //!< 装备子窗体
    GUIWnd*                         m_pRepute;          //!< 声望窗体
    GUIWnd*                         m_pTitle;           //!< 称号窗体
    
    GUIPushButton*                  m_pBtEquip;         //!< 进入装备
    GUIPushButton*                  m_pBtReput;         //!< 进入声望 
    GUIPushButton*                  m_pBtTitle;         //!< 进入称号

    GUIButton*                      m_pBtClose;         //!< 关闭

    // 声望相关
    ProgressEx*	    	            m_pPrgrssReputeValue[ECLT_NUM];
    GUIStatic*                      m_pStcReputeLevel[ECLT_NUM];
    GUIStatic*                      m_pStcContrib[ECLT_NUM];
    GUIStatic*                      m_pStcHero[ECLT_NUM];

    // 称号相关
    TreeEx*                         m_pTreeTitleList;
    GUIStaticEx*                    m_pStcexDescription;

    DWORD                           m_dwCurrPlayerID;    //!< 当前查看玩家ID
	RemoteRoleStateFrame*			m_pRRSF;

    bool                            m_bneedRefreshRepute;
    DWORD                           m_timeFirst;
};
