#pragma once
#include "gameframe.h"

struct tagNS_ExchangeReq;
struct tagNS_ExchangeReqRes;
struct tagNS_ExchangeCancel;
struct tagNS_ExchangeFinish;
struct tagFollowRoleEvent;
class NetSession;
/** \class TradeFrame
	\brief 交易系统模块，处理交易相关的内容 
*/
class TradeFrame :
	public GameFrame
{
public:
	TradeFrame(void);
	~TradeFrame(void);
	
	// GameFrame
	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual VOID Update();

	// 交易错误码显示
	VOID ShowExchangeErrorMsg(DWORD dwErrorCode, BOOL bNotMine = FALSE);

private:
	// 处理网络消息
	DWORD OnNetExchangeReq(tagNS_ExchangeReq* pNetCmd, DWORD);
	DWORD OnNetExchangeReqRes(tagNS_ExchangeReqRes* pNetCmd, DWORD);
	DWORD OnNetExchangeCancel(tagNS_ExchangeCancel* pNetCmd, DWORD);
	DWORD OnNetExchangeFinish(tagNS_ExchangeFinish* pNetCmd, DWORD);
	
	// 处理游戏事件
	DWORD OnEventOpenShop(tagGameEvent* pGameEvent);
	DWORD OnEventCloseShop(tagGameEvent* pGameEvent);
	DWORD OnEventCloseShopNow(tagGameEvent* pGameEvent);
	DWORD OnEventReqTrade(tagFollowRoleEvent* pGameEvent);
	DWORD OnEventReqReply(tagMsgBoxEvent* pGameEvent);

	// 打开玩家交易界面
	VOID OpenPlayerTrade(DWORD dwTargetID);
	// 关闭玩家交易界面
	VOID ClosePlayerTrade();
	// 解锁玩家交易界面
	VOID UnlockPlayerTrade();

private:
	TSFPTrunk<TradeFrame>	m_Trunk;
	TObjRef<NetCmdMgr>		m_pCmdMgr;
	TObjRef<NetSession>		m_pSession;

	DWORD					m_dwReqTime;  // 交易请求相应时间	

	DWORD					m_dwTargetID; // 交易对象ID
};
