#pragma once
#include "gameframe.h"

class LockStatic;
struct tagNS_PetExchangeAdd_2Src;
struct tagNS_PetExchangeAdd_2Dst;
struct tagNS_PetExchangeDec;
struct tagNS_PetExchangeDec_2Dst;
struct tagNS_PetExchangeMoney;
struct tagNS_PetExchangeLock;
struct tagPetDispChange;

//! \class  PetTradeUIFrame
//! \brief  宠物交易界面
//! \author hyu
//! \date   2009-8-14
class PetTradeUIFrame :
    public GameFrame
{
public:
    PetTradeUIFrame(void);
    ~PetTradeUIFrame(void);

    virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
    virtual BOOL Destroy();
    virtual BOOL ReloadUI();
    virtual VOID Update();
    virtual BOOL EscCancel();

    VOID SetTargetPlayerID(DWORD dwTargetID);

    VOID LockSrcControl(BOOL bLock);
    VOID LockDstControl(BOOL bLock);

private:
    BOOL EventHandler(tagGUIEvent* pEvent);

    VOID SetExchangeEnable();

    // 接收网络消息
    DWORD OnNS_PetExchangeAdd_2Src(tagNS_PetExchangeAdd_2Src* pMsg, DWORD);
    DWORD OnNS_PetExchangeAdd_2Dst(tagNS_PetExchangeAdd_2Dst* pMsg, DWORD);
    DWORD OnNS_PetExchangeDec(tagNS_PetExchangeDec* pMsg, DWORD);
    DWORD OnNS_PetExchangeDec_2Dst(tagNS_PetExchangeDec_2Dst* pMsg, DWORD);
    DWORD OnNS_PetExchangeMoney(tagNS_PetExchangeMoney* pMsg, DWORD);
    DWORD OnNS_PetExchangeLock(tagNS_PetExchangeLock* pMsg, DWORD);

    // 发送网络消息
    VOID BeginNC_PetExchangeCancel();
    VOID BeginNC_PetExchangeVerify();
    VOID BeginNC_PetExchangeLock();
public:
    VOID BeginNC_PetExchangeAdd(DWORD dwPetID);
private:
    VOID BeginNC_PetExchangeDec(DWORD dwPetID);

private:
    // 外理游戏事件
    DWORD OnPetDispInfo(tagPetDispChange* e);
    DWORD OnEventExchangeMoney(tagMsgMoneyBoxEvent* pGameEvent);

    // 显示错误提示
    VOID ShowExchangeErrorMsg(DWORD dwErrorCode);
    // 销毁相关的对话框
    VOID DestroyMsgBox();

    // 刷新到界面
    VOID RefreshToUI();

    VOID SetMoneySrc(INT64 n64Silver);
    VOID SetMoneyDst(INT64 n64Silver);

private:
    
    typedef map<int, DWORD> _row2pet;
    _row2pet                    m_mapRow2PetTarget;
    _row2pet                    m_mapRow2PetMyself;
    //! 清除缓存
    void ClearBuffer() {m_mapRow2PetTarget.clear(); m_mapRow2PetMyself.clear();}
    //! 无效返回GT_INVALID
    DWORD FindPetByRow(_row2pet &row2pet, int rowIndex);
    //! 无效返回GT_INVALID
    int FindRowByPet(_row2pet &row2pet, DWORD petId);
    //! 删除
    void RemoveRowByPet(_row2pet &row2pet, DWORD petId);

    // 控件
    GUIWnd*                     m_pWnd;

    GUIButton*                  m_pBtTrade;
    GUIButton*                  m_pBtConfirm;
    GUIButton*                  m_pBtCancel;

    GUIButton*                  m_pBtClose;

    GUIStatic*                  m_pStcNameTarget;
    GUIListBox*                 m_pLbTarget;
    GUIStatic*                  m_pStcTargetGold;
    GUIStatic*                  m_pStcTargetSilver;
    LockStatic*                 m_pLstcTarget;

    GUIStatic*                  m_pStcNameMyself;
    GUIListBox*                 m_pLbMyself;
    GUIStatic*                  m_pStcMyGold;
    GUIStatic*                  m_pStcMySilver;
    LockStatic*                 m_pLstcMyself;



    // 工具
    TSFPTrunk<PetTradeUIFrame>  m_Trunk;
    TObjRef<GUISystem>			m_pGUI;
    TObjRef<Util>				m_pUtil;
    TObjRef<NetCmdMgr>			m_pCmdMgr;
    TObjRef<GameFrameMgr>		m_pFrameMgr;
    TObjRef<NetSession>			m_pSession;

    DWORD						m_dwTargetID;
};
