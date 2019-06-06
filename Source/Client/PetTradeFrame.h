#pragma once
#include "gameframe.h"

struct tagNS_PetExchangeReq;
struct tagNS_PetExchangeReqRes;
struct tagNS_PetExchangeCancel;
struct tagNS_PetExchangeFinish;
struct tagFollowRoleEvent;
struct tagMsgBoxEvent;

//! \class   PetTradeFrame
//! \breif   宠物交易
//! \author  hyu
//! \date    2009-8-13
class PetTradeFrame :
    public GameFrame
{
public:
    PetTradeFrame(void);
    ~PetTradeFrame(void);

    virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
    virtual BOOL Destroy();
    virtual VOID Update();

    // 交易错误码显示
    VOID ShowExchangeErrorMsg(DWORD dwErrorCode, BOOL bNotMine = FALSE);

private:
    // 处理网络消息
    DWORD OnNetPetExchangeReq(tagNS_PetExchangeReq* pNetCmd, DWORD);
    DWORD OnNetPetExchangeReqRes(tagNS_PetExchangeReqRes* pNetCmd, DWORD);
    DWORD OnNetPetExchangeCancel(tagNS_PetExchangeCancel* pNetCmd, DWORD);
    DWORD OnNetPetExchangeFinish(tagNS_PetExchangeFinish* pNetCmd, DWORD);

    // 处理游戏事件
    DWORD OnEventReqTrade(tagFollowRoleEvent* pGameEvent);
    DWORD OnEventReqReply(tagMsgBoxEvent* pGameEvent);

    // 打开宠物交易界面
    VOID OpenPetTrade(DWORD dwTargetID);
    // 关闭宠物交易界面
    VOID ClosePetTrade();
    // 解锁宠物交易界面
    VOID UnlockPetTrade();

private:
    // 工具
    TSFPTrunk<PetTradeFrame>    m_Trunk;
    TObjRef<GUISystem>			m_pGUI;
    TObjRef<Util>				m_pUtil;
    TObjRef<NetCmdMgr>			m_pCmdMgr;
    TObjRef<GameFrameMgr>		m_pFrameMgr;
    TObjRef<NetSession>			m_pSession;

    // 变量
    DWORD					    m_dwReqTime;  // 交易请求相应时间	
    DWORD					    m_dwTargetID; // 交易对象ID
};
