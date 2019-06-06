#pragma once

#include "..\WorldDefine\reputation.h"
#include "TreasureMgr.h"
#include "FameHallFrame.h"
#include "ItemButton.h"
#include "StaticEx.h"

const INT MAX_TREASURE_PLACE = 10;        //窗体每页容纳最多物品数

//! \class ClanTreasureFrame
//! \brief 氏族珍宝界面
//! \author hyu
//! \base GameFrame
//! \remark
class ClanTreasureFrame :
    public GameFrame
{
public:
    ClanTreasureFrame(void);
    ~ClanTreasureFrame(void);

    virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
    virtual BOOL Destroy();
    virtual BOOL ReloadUI();
    virtual VOID Update();

private:
    VOID UpdateClanTreasure();

    DWORD OnGameEvent(tagGameEvent* pGameEvent);

    DWORD OnOpenClanTreasure(tagOpenClanTreasure* pGameEvent);
    DWORD OnCloseClanTreasure(tagGameEvent* pGameEvent);

    //! 服务器广播后的有新的珍宝被激活
    DWORD OnNewActivedTreasure(tagNewActivedTreasure* pGameEvent);
    //! 激活珍宝后的反馈
    DWORD OnActiveTreasureRet(tagActTreasureRezult* pGameEvent);

    //! 跟据当前珍宝数据初始化界面
    VOID InitTreasure();
    //! 将数据刷新到界面
    VOID RefreshToUI();
    //! 将剩余激活次数刷新到界面
    VOID RefreshActCntToUI();
    //! 点击上一页处理
    VOID PagePrev();
    //! 点击下一页处理
    VOID PageNext();
    //! 点击击活珍宝处理
    VOID BeginActive();

    //! 跟据页码和面板位置返回上面的珍宝,如没有则返回NULL
    const tagTreasureMgrData* GetTreaByPos(INT currPage, INT pos);

    DWORD EventHandler(tagGUIEvent* pEvent);

    VOID ShowWnd(bool bval);

    //! 点击选择某个面板
    VOID SelectItemPad(INT nIndex = GT_INVALID);

    TSFPTrunk<ClanTreasureFrame>    m_Trunk;
    TObjRef<GUISystem>              m_pGUI;
    TObjRef<Util>                   m_pUtil;
    TObjRef<GameFrameMgr>           m_pFrameMgr;
    TObjRef<NetSession>             m_pSession;

    // GUI控件
    GUIWnd*                         m_pWnd;

    StaticEx*                       m_pStcItemPad[MAX_TREASURE_PLACE];  //!< 物品板
    StaticEx*                       m_pStcIcon[MAX_TREASURE_PLACE];     //!< 物品图标
    ItemButton*                     m_pIbtTrea[MAX_TREASURE_PLACE];     //!< 物品
    GUIStatic*                      m_pStcName[MAX_TREASURE_PLACE];     //!< 物品名
    GUIStatic*                      m_pStcStus[MAX_TREASURE_PLACE];     //!< 激活状态

    GUIStatic*                      m_pStcRestnum;          //!< 剩余激次数

    GUIStatic*                      m_pStcPage;             //!< 页数
    GUIButton*                      m_pBtPrevPage;          //!< 前一页
    GUIButton*                      m_pBtNextPage;          //!< 下一页

    GUIButton*                      m_pBtActiveTreasure;    //!< 激活珍宝
    GUIButton*                      m_pBtClose;             //!< 关闭

    INT                             m_nSelected;            //!< 选中的物品板界面索引,GT_INVALID为不选任何面板

    ECLanType                       m_eClan;                //!< 当前选择的氏族

    INT                             m_nPageCount;           //!< 当前最大页数
    INT                             m_nCurrPage;            //!< 当前所处页数

    vector<tagTreasureMgrData*>     m_vecTreasure;          //!< 供随机访问的全部珍宝数据

    tstring                         strSelectIcon;          //!< 选择图标路径
    tstring                         strItemIcon;            //!< 默认图标路径
};

