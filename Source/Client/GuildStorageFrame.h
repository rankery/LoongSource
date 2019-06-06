#pragma once
#include "gameframe.h"
#include "..\WorldDefine\container_define.h"

class ItemButton;
class Item;
struct tagUpdateLPItemEvent;
struct tagItemPosChangeExEvent;

//! \class  GuildStorageFrame
//! \brief  帮派仓库界面
//! \author hyu
//! \date   2009-8-3
//! \last   2009-8-3
class GuildStorageFrame :
    public GameFrame
{
public:
    GuildStorageFrame(void);
    ~GuildStorageFrame(void);

    virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
    virtual BOOL Destroy();
    virtual BOOL ReloadUI();
    virtual VOID Update();
    virtual BOOL EscCancel();

    //! 设置NPC ID
    void SetNPCID(DWORD dwID) {m_dwNPCID = dwID;}

    //! 打开界面
    VOID ShowWnd(BOOL bVal);

private:
    //! receive the initial msg
    DWORD OnInitGuildWareEvent(tagGameEvent* pGameEvent);

    //! update game event
    DWORD OnEventUpdateLPItem(tagUpdateLPItemEvent* pGameEvent);

    //! close
    DWORD OnEventCloseGuildWare(tagGameEvent* pGameEvent);

    DWORD OnEventItemPosChange(tagItemPosChangeExEvent* pGameEvent);

    //! 显示当前页
    VOID ShowCurPage(INT16 page);

    //! 跟据位置获得物品
    Item* GetItem(INT16 index);

    //! 将位置上容器中的物品刷新到界面
    VOID RefreshItem(INT16 index);

    //! 是否在当前页
    BOOL IsCurPage(INT16 index){return ((SPACE_ONE_GUILDWARE * m_n16CurPage <= index) && (SPACE_ONE_GUILDWARE * (m_n16CurPage + 1) > index));}

    //! 处理物品移动
    //! \param n16PosSrc为移动的物品序号
    //! \param n16PosDst为移动至该位置，默认值为移动位置不指定
    VOID OnItemChange(INT16 n16PosSrc, INT16 n16PosDst = GT_INVALID);

    //! 处理物品移入仓库
    //! \param ePosSrc为物品原容器
    //! \param ePosDst为物品目的容器
    //! \param n16PosSrc为物品原序号
    //! \param n16PosDst为移动至该位置，默认值为移动位置不指定
    VOID OnItemChangeEx(EItemConType ePosSrc, EItemConType ePosDst, INT16 n16PosSrc, INT16 n16PosDst = GT_INVALID);

private:

    //GUI控件
    GUIWnd*         m_pWnd;
    GUIButton*      m_pBtClose;                                 //!< 关闭窗体
    GUIPushButton*  m_pPbt[MAX_GUILDWARE_NUM];                  //!< 分页按钮
    ItemButton*     m_pUnitItem[SPACE_ONE_GUILDWARE];			//!< 仓库单元格

    INT16           m_n16CurPage;                               //!< 当前仓库页

    DWORD           m_dwNPCID;                                  //!< 对话npc id

    DWORD           m_updateTimeFirst;
    DWORD           m_updateTimeDistance;
private:
    //! 处理GUI消息
    VOID EventHandler(tagGUIEvent* pEvent);

    TSFPTrunk<GuildStorageFrame>        m_Trunk;
    TObjRef<GUISystem>			        m_pGUI;
    TObjRef<Util>				        m_pUtil;
    TObjRef<NetCmdMgr>			        m_pCmdMgr;
    TObjRef<GameFrameMgr>		        m_pFrameMgr;
    TObjRef<NetSession>			        m_pSession;
};
