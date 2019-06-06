#pragma once

#include "..\WorldDefine\guild_define.h"

struct tagNS_GetGuildUpgradeInfo;
struct tagNS_UpdateFacilitiesInfo;
struct tagNS_GuildProcFailed;
struct tagUpdateLPItemEvent;
struct tagItemPosChangeExEvent;
class ActiveStatic;

/** \class GuildUpgradeFrame
	\brief 帮派设施升级界面
*/
class GuildUpgradeFrame : public GameFrame
{
public:
	GuildUpgradeFrame();
	~GuildUpgradeFrame();

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual BOOL EscCancel();

private:
	VOID SafeDestroyUI();
	BOOL GUIEventHandler(tagGUIEvent *pEvent);
	VOID UpdataFacilitiesInfoUI( const EFacilitiesType eType );
	VOID SetItemNum( const EFacilitiesType eType, const int nItemIndex, const int nItemNum );
	VOID HandInItems();
	VOID CheckLevelFull( const EFacilitiesType eType );
	/** \网络消息响应
	*/
	DWORD OnNS_GetGuildUpgradeInfo( tagNS_GetGuildUpgradeInfo* pCmd, DWORD );
	DWORD OnNS_UpdateFacilitiesInfo( tagNS_UpdateFacilitiesInfo* pCmd, DWORD );
	DWORD OnNS_GuildProcFailed( tagNS_GuildProcFailed* pCmd, DWORD );
	/** \游戏事件处理
	*/
	DWORD OnUpdateItemEvent( tagUpdateLPItemEvent* pEvent );
	DWORD OnClose_GuildUpgrade( tagGameEvent* pEvent );
	DWORD OnItemEvent_Move2GuildUpgrade( tagItemPosChangeExEvent* pEvent );
private:
	TSFPTrunk<GuildUpgradeFrame>	m_Trunk;
	TObjRef<GUISystem>				m_pGUI;
	TObjRef<NetCmdMgr>				m_pCmdMgr;
	TObjRef<NetSession>				m_pSession;

	tagGuildFacilitiesInfo	        m_sFacilitiesInfo[MAX_GUILD_FACILITIES_TYPE];  // 设施升级信息
	GUIWnd*							m_pWnd;
	GUIButton*						m_pBn_Close;
	GUIPushButton*                  m_pBn_Tab[MAX_GUILD_FACILITIES_TYPE];          // 设施标签
	GUIStatic*						m_pSt_LevelCaption;							   // 等级
	GUIStatic*						m_pSt_Level;
	GUIProgress*					m_pPro_Progress;							   // 进度
	ActiveStatic*                   m_pSt_Item_Icon[MAX_UPGRADE_NEED_ITEM_TYPE];   // 物品图标
	GUIStatic*						m_pSt_Item_Name[MAX_UPGRADE_NEED_ITEM_TYPE];
	GUIStatic*						m_pSt_Item_Num[MAX_UPGRADE_NEED_ITEM_TYPE];
	GUIStatic*						m_pSt_Fund;                                    // 所需金钱
	GUIStatic*						m_pSt_Material;                                // 所需资材
	GUIStatic*						m_pSt_Tips;									   // 说明
	GUIButton*                      m_pBn_HandInItems;                             // 上缴物品
	EFacilitiesType					m_eCurSelType;								   // 当前选定的设施类型

	// 放进界面的物品数量
	int                             m_nPutInedItemNum[MAX_GUILD_FACILITIES_TYPE][MAX_UPGRADE_NEED_ITEM_TYPE];
	bool                            m_bWaitingForHandInRet;                        // 是否正在等待上缴返回结果
	int                             m_bLevelFull[MAX_GUILD_FACILITIES_TYPE];	   // 是否已经达到等级上限
};



