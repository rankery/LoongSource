#pragma once
#include "gameframe.h"
#include "ItemButton.h"
#include "PetSkillButton.h"
#include "StaticEx.h"
#include "../WorldDefine/msg_pet.h"
#include "PetEvent.h"

const INT MAX_PET_EQUIP     =   4;
const INT MAX_PET_SKILL     =   12;

class PetShowWindow;
class PetBagFrame;

//! \class  PetFrame
//! \brief  本地玩家宠物管理界面
//! \author hyu
//! \base   GameFrame
//! \sa     PetManager
class PetFrame :
    public GameFrame
{
public:
    PetFrame(void);
    ~PetFrame(void);

    virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
    virtual BOOL Destroy();
    virtual BOOL ReloadUI();
    virtual VOID Update();
    virtual VOID Render3D();
	virtual BOOL EscCancel();

    bool IsWindowVisible();

private:
    //! 处理GUI消息
    DWORD EventHandler(tagGUIEvent* pEvent);

    //! 打开/关闭宠物界面游戏事件
    DWORD OnOpen_Pet(tagGameEvent* pMsg);
	DWORD OnClose_Pet(tagGameEvent* pMsg);

    //! 打开界面
    VOID ShowWnd(BOOL bVal);

    //! 刷新宠物列表
    VOID RefreshPetList();
    //! 刷新界面数据
    VOID RefreshToUI();
    //! 刷新当前装备
    VOID RefreshEquip();
    //! 刷新当前技能
    VOID RefreshSkill();
    //! 刷新当前模型
    VOID RefreshPetShow();

    //! 获得等级文字
    tstring GetLevelText(int nLevel);
    //! 获得品级文字
    LPCTSTR GetQualityText(int nQuality);

    //! 重置界面
    VOID ClearUI();

    //! 确定删除宠物
    DWORD OnCommitDeletePet(tagMsgBoxEvent* pEvent);

    //! 确定灌注经验
    DWORD OnCommitPourExp(tagMsgBoxEvent* pEvent);

    //! 处理宠物穿装备事件
    DWORD OnUsePetEquipHandler(tagUsePetEquip* pEvent);

    //! 宠物属性发生变化
    DWORD OnPetAttrChange(tagPetAttrChange* pEvent);

    //! 支持流的类型互相转换
    template<typename T>
    tstring TtoString(T nSrc);

    DWORD                       m_dwCurrSelectPetID;    //!< 当前选中的宠物ID(如无选择则为GT_INVALID)

    map<INT, DWORD>             m_mapRow2Pet;           //!< 列表控件行和宠物id对照表

    TSFPTrunk<PetFrame>         m_Trunk;
    TObjRef<GUISystem>			m_pGUI;
    TObjRef<Util>				m_pUtil;
    TObjRef<NetCmdMgr>			m_pCmdMgr;
    TObjRef<GameFrameMgr>		m_pFrameMgr;
    TObjRef<NetSession>			m_pSession;

    //GUI控件
    GUIWnd*						m_pWnd;

    GUIStatic*                  m_pStcState;                    //!< 状态
    PetShowWindow*              m_pStcPetShow;                  //!< 宠物Avata

    GUIListBox*                 m_pLbPetList;                   //!< 宠物列表

    GUIButton*                  m_pBtCall;                      //!< 召唤
    GUIButton*                  m_pBtSetFree;                   //!< 放生
    GUIButton*                  m_pBtPerfuse;                   //!< 灌注
    GUIButton*                  m_pBtRide;                      //!< 骑乘
    GUIButton*                  m_pBtTrade;                     //!< 交易

    ItemButton*                 m_pBtEquipA[MAX_PET_EQUIP];     //!< 宠物装备
    PetSkillButton*             m_pBtSkillA[MAX_PET_SKILL];     //!< 宠物技能

    GUIStatic*                  m_pStcQuality;                  //!< 宠物品质
    GUIStatic*                  m_pStcMatureTalent;             //!< 成长资质
    GUIStatic*                  m_pStcPotential;                //!< 潜能
    GUIStatic*                  m_pStcMagic;                    //!< 仙灵之力(原五行力)
    GUIStatic*                  m_pStcEnergy;                   //!< 精气(原灵力)
    GUIStatic*                  m_pStcRidingSpeed;              //!< 骑乘速度

    GUIStatic*                  m_pStcNick;                     //!< 宠物昵称
    GUIStatic*                  m_pStcLevel;                    //!< 级别
    GUIStatic*                  m_pStcCurrExp;                  //!< 当前经验
    GUIStatic*                  m_pStcLevelUpNeed;              //!< 升级经验

    GUIButton*                  m_pBtClose;                     //!< 关闭

    PetBagFrame*                m_pPetBagFrame;                 //!< pet bag frame
};

template<typename T>
tstring PetFrame::TtoString( T nSrc )
{
    tstringstream sstream;
    sstream << nSrc;
    tstring ret;
    sstream >> ret;
    return ret;
}