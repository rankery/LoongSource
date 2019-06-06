#pragma once
#include "gameframe.h"
#include "ItemButton.h"

struct tagPet;

const INT PETBAG_MAX_COL = 5;
const INT PETBAG_MAX_ROW = 4;
const INT PETBAG_MAX_UNIT = PETBAG_MAX_COL * PETBAG_MAX_ROW;
const SIZE PETBAG_UINT_SIZE = {36,36};
const SIZE PETBAG_SPACE_SIZE = {9,7};

//! \class  PetBagFrame
//! \brief  pet's bag
//! \datail pet's bag holds only the symbol links of items that's contained in role bag,
//!         and all of the links configurations is saved in local directory.
//! \author hyu
//! \date   2009-6-24
//! \last   2009-6-24
class PetBagFrame :
    public GameFrame
{
public:
    enum E_PET_BAG_SKILL_TYPE
    {
        EPBST_SELL,     // Ð¡··
    };

    PetBagFrame(void);
    ~PetBagFrame(void);

    virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
    virtual BOOL Destroy();
    virtual BOOL ReloadUI();
    virtual VOID Update();
    virtual BOOL EscCancel();

    VOID ShowWnd(BOOL bShow);
    void SetFocusPetID(DWORD petId);
    void RefreshItem();
    void SetUseType(E_PET_BAG_SKILL_TYPE eType) { m_eCurrType = eType; }
    E_PET_BAG_SKILL_TYPE GetUseType() { return m_eCurrType; }
    void SetCurrSkillID(DWORD dwSkillID) { m_dwCurrSkillID = dwSkillID; }
    DWORD GetCurrSkillID() { return m_dwCurrSkillID; }

private:

    DWORD EventHandler(tagGUIEvent* pEvent);

    TSFPTrunk<PetBagFrame>      m_Trunk;
    TObjRef<GUISystem>          m_pGUI;
    TObjRef<Util>               m_pUtil;
    TObjRef<GameFrameMgr>       m_pFrameMgr;
    TObjRef<NetSession>         m_pSession;

    GUIWnd*                     m_pWnd;
    ItemButton*                 m_pUnitItem[PETBAG_MAX_UNIT];				//!< item units

    GUIStatic*                  m_pStcUnitBack; //!< background of units
    GUIButton*                  m_pButton;      //!< button
    GUIButton*                  m_pBtClose;     //!< button to exit

    const tagPet*               m_focusPet;     //!< focus pet
    DWORD                       m_dwFocusPetID; //!< focus pet id
    int                         m_unitsAvailCnt;//!< available units;

    E_PET_BAG_SKILL_TYPE        m_eCurrType;    //!< current use type
    DWORD                       m_dwCurrSkillID;//!< current skill id

    DWORD                       m_updateTimeFirst; //!< start time
};
