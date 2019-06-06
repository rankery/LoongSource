#pragma once
#include "gameframe.h"
#include "ItemButton.h"
#include "PetDef.h"
#include "../WorldDefine/pet_skill_define.h"
#include "PetEvent.h"

const int MAX_PICKUP_FUNC   =   6;

//! \class  PetSkillFrame
//! \brief  本地玩家宠物技能设置界面
//! \author hyu
//! \base   GameFrame
//! \sa     PetManager
class PetSkillFrame :
    public GameFrame
{
public:
    PetSkillFrame(void);
    ~PetSkillFrame(void);

    virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
    virtual BOOL Destroy();
    virtual BOOL ReloadUI();
    virtual VOID Update();
    virtual BOOL EscCancel();

private:
    ////! 保存设置
    //void SavePetSkillCfgToFile(EPetskillType ePetSkillType);
    ////! 读取设置
    //void LoadPetSkillCfgToFile(EPetskillType ePetSkillType);

    //! 初始化UI
    void InitalUI();
    //! resize combobox
    void ResizeCombobox(GUIComboBox* pComboBox, int rowCount);

    void SavePetSkillCfg(EPetskillType ePetSkillType);
    void LoadPetSkillCfg(EPetskillType ePetSkillType);
private:
    //! 处理窗体打开事件
    DWORD OnPetSkillCfgOpen(tagPetSkillCfgOpen* pEvent);

    //! 处理GUI消息
    DWORD EventHandlerForage(tagGUIEvent* pEvent);
    DWORD EventHandlerSupply(tagGUIEvent* pEvent);
    DWORD EventHandlerPickUp(tagGUIEvent* pEvent);
    DWORD EventHandlerTraining(tagGUIEvent* pEvent);

    //! 打开窗体
    VOID ShowWndForage(BOOL bVal);
    VOID ShowWndSupply(BOOL bVal);
    VOID ShowWndPickUp(BOOL bVal);
    VOID ShowWndTraining(BOOL bVal);

    //! 取得生产技能ID(不带等级的)
    DWORD GetComposeSkill(ESkillTypeEx2 eType);

    //! 是否人物当前状态可以使用红药
    bool IfCanUseHPItem(Item* pItem);
    //! 是否人物当前状态可以使用蓝药
    bool IfCanUseMPItem(Item* pItem);
    //! 是否人物当前状态可以使用持久力药
    bool IfCanUseToughnessItem(Item* pItem);


    DWORD   m_CurrPetID;        //!< 拥有此技能当前宠物
    DWORD   m_CurrSkillID;      //!< 当前技能ID

    //! 技能设置配置
    //tagPetSkillForage           m_ForageCfg;            //!< 觅食
    //bool                        m_bForageHasLoaded;
    //tagPetSkillPickup           m_PickupCfg;            //!< 分类拾取
    //bool                        m_bPickupHasLoaded;
    //tagPetSkillSupply           m_MedicineFeedCfg;      //!< 药童
    //bool                        m_bMedicineFeedHasLoaded;

    TSFPTrunk<PetSkillFrame>    m_Trunk;
    TObjRef<GUISystem>			m_pGUI;
    TObjRef<Util>				m_pUtil;
    TObjRef<GameFrameMgr>		m_pFrameMgr;
    TObjRef<NetSession>			m_pSession;

    //GUI控件
    GUIWnd*						m_pWndForage;               //!< 觅食
    GUIComboBox*                m_pCbCondition;             //!< 觅食条件
    GUIPushButton*              m_pPbtEnable;               //!< 开启觅食功能
    ItemButton*                 m_pBtFood;                  //!< 宠物食品
    GUIButton*                  m_pBtForageClose;           //!< 关闭

    GUIWnd*                     m_pWndSupply;               //!< 药童
    GUIComboBox*                m_pCbRedCondition;          //!< 红药条件
    GUIPushButton*              m_pPbtRedEnable;            //!< 红药功能开启
    ItemButton*                 m_pBtRed;                   //!< 红药
    GUIComboBox*                m_pCbBlueCondition;         //!< 蓝药条件
    GUIPushButton*              m_pPbtBlueEnable;           //!< 蓝药功能开启
    ItemButton*                 m_pBtBlue;                  //!< 蓝药
    GUIComboBox*                m_pCbToughnessCondition;    //!< 持久力条件
    GUIPushButton*              m_pPbtToughnessEnable;      //!< 持久力功能开启
    ItemButton*                 m_pBtToughness;             //!< 持久力药
    GUIButton*                  m_pBtSupplyClose;           //!< 关闭

    GUIWnd*                     m_pWndPickUp;               //!< 分类拾取
    GUIPushButton*              m_pBtFunc[MAX_PICKUP_FUNC]; //!< 功能开启
    GUIButton*                  m_pBtPickClose;             //!< 关闭

    GUIWnd*                     m_pWndTraining;             //!< 训练
    GUIPushButton*              m_pPbtComposeSkill[ESSTE_End]; //!< 生产技能复选框
    GUIButton*                  m_pBtTrainingConfirm;       //!< 确定
    GUIButton*                  m_pBtTrainingCanel;         //!< 取消
    GUIButton*                  m_pBtTrainingClose;         //!< 关闭

};
