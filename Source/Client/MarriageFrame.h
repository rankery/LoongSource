#pragma once
#include "gameframe.h"

struct tagNS_GetMarriageReq;
struct tagNS_GetMarriageReqRes;

const int MIN_MARRIAGE_GOLD         = 66;           //!< 结婚需要的最小金数
const DWORD MAX_MARRIAGE_REQTIME	= 60 * 1000;	//!< 最大等待请求时间，单位：毫秒

//! \class   MarriageFrame
//! \breif   结婚过程
//! \author  hyu
//! \date    2009-9-23
//! \last    2009-10-23
class MarriageFrame :
    public GameFrame
{
public:
    MarriageFrame(void);
    ~MarriageFrame(void);

    virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
    virtual BOOL Destroy();
    virtual VOID Update();

private:
    // 判断本地玩家是否可结婚
    bool IfCanGetToMarry();
    // 判断与目标玩家是否可结婚
    bool ifObjCanGetToMarry(LPCTSTR szPlayerName);

    // 处理网络消息
    DWORD OnNS_GetMarriageReq       (tagNS_GetMarriageReq*      pMsg, DWORD);
    DWORD OnNS_GetMarriageReqRes    (tagNS_GetMarriageReqRes*   pMsg, DWORD);

    // 处理游戏事件
    DWORD OnOpenMarriageApply       (tagGameEvent* pEvent);
    DWORD OnMarriageInputNameCommit (tagMsgInputBoxEvent* pEvent);
    DWORD OnMarriageReqCheckEvent   (tagMsgBoxEvent* pEvent);

    // 工具
    TSFPTrunk<MarriageFrame>    m_Trunk;
    TObjRef<GUISystem>			m_pGUI;
    TObjRef<Util>				m_pUtil;
    TObjRef<NetCmdMgr>			m_pCmdMgr;
    TObjRef<GameFrameMgr>		m_pFrameMgr;
    TObjRef<NetSession>			m_pSession;

    // 变量
    DWORD					    m_dwReqTime;        //!< 结婚请求相应时间
    DWORD					    m_dwTargetID;       //!< 结婚对象ID
    tstring                     m_strTargetName;    //!< 结婚对象name
    DWORD                       m_dwNpcId;          //!< NPC标识

};
