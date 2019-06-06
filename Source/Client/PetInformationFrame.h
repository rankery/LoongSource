#pragma once
#include "gameframe.h"
#include "PetFrame.h"

struct  tagGetPetAttrEvent;
class   PetShowWindow;
class   ItemButton;
class   PetSkillButton;
struct  tagPet;

//! \class  PetInformationFrame
//! \brief  宠物信息查询界面
//! \author hyu
//! \base   GameFrame
class PetInformationFrame :
    public GameFrame
{
public:
    PetInformationFrame(void);
    ~PetInformationFrame(void);

    virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
    virtual BOOL Destroy();
    virtual BOOL ReloadUI();
    virtual VOID Update();
    virtual VOID Render3D();
    virtual BOOL EscCancel();

    //! 发送查询信息请求
    VOID GetPetReq(DWORD dwRoleID, DWORD dwPetID);

private:
    //! 打开界面
    VOID ShowWnd(BOOL bVal);

    //! 处理GUI消息
    DWORD EventHandler(tagGUIEvent* pEvent);

    //! 清空ui
    VOID ClearUI();

    //! 刷新界面数据
    VOID RefreshToUI();
    //! 刷新当前装备
    VOID RefreshEquip();
    //! 刷新当前技能
    VOID RefreshSkill();
    //! 刷新当前模型
    VOID RefreshPetShow();

    // 游戏事件
public:
    DWORD OnGetPetAttrEvent(tagNS_GetPetAttr* pMsg);
private:
    DWORD OnPetAttrChange( tagPetAttrChange* pEvent );

    void SetPet(tagPet* pPet);

    //! 获得等级文字
    tstring GetLevelText(int nLevel);

    DWORD                       m_currRoleId;
    tagPet*                     m_pCurrPet;

    // GUI控件
    GUIWnd*						m_pWnd;

    GUIButton*                  m_pBtClose;                     //!< 关闭

    PetShowWindow*              m_pStcPetShow;                  //!< 宠物Avata

    ItemButton*                 m_pBtEquipA[MAX_PET_EQUIP];     //!< 宠物装备
    PetSkillButton*             m_pBtSkillA[MAX_PET_SKILL];     //!< 宠物技能

    GUIStatic*                  m_pStcNick;                     //!< 宠物昵称
    GUIStatic*                  m_pStcLevel;                    //!< 级别
    GUIStatic*                  m_pStcCurrExp;                  //!< 当前经验
    GUIStatic*                  m_pStcLevelUpNeed;              //!< 升级经验

    GUIStatic*                  m_pStcQuality;                  //!< 宠物品质
    GUIStatic*                  m_pStcMatureTalent;             //!< 成长资质
    GUIStatic*                  m_pStcPotential;                //!< 潜能
    GUIStatic*                  m_pStcMagic;                    //!< 仙灵之力(原五行力)
    GUIStatic*                  m_pStcEnergy;                   //!< 精气(原灵力)
    GUIStatic*                  m_pStcRidingSpeed;              //!< 骑乘速度

    // 工具
    TSFPTrunk<PetInformationFrame>  m_Trunk;
    TObjRef<GUISystem>			m_pGUI;
    TObjRef<Util>				m_pUtil;
    TObjRef<NetCmdMgr>			m_pCmdMgr;
    TObjRef<GameFrameMgr>		m_pFrameMgr;
    TObjRef<NetSession>			m_pSession;

    //! 支持流的类型互相转换
    template<typename T>
    tstring TtoString(T nSrc);
};

template<typename T>
tstring PetInformationFrame::TtoString( T nSrc )
{
    tstringstream sstream;
    sstream << nSrc;
    tstring ret;
    sstream >> ret;
    return ret;
}