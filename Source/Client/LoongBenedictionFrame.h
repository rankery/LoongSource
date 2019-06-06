#pragma once
#include "gameframe.h"

/**
	\brief 神龙赐福 是新手登录游戏，以10分钟为单位向玩家赠送礼物
		   主要是为了防止新手的流失
    \auth  dyb
*/
class IconStatic;
class ServerTime;
struct tagNS_GetLoongBenediction;
struct tagNS_SetNextBenediction;

class LoongBenedictionFrame : public GameFrame
{
public:
	LoongBenedictionFrame(void);
	~LoongBenedictionFrame(void);

	virtual BOOL Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam );
	virtual BOOL Destroy();
	virtual VOID Update();
	virtual BOOL ReloadUI();

private:
	BOOL EventHandler( tagGUIEvent* pEvent );
	VOID SafeDestroyUI();

	/** 获得奖励
	*/
	VOID GetBenediction();
	
	/** 服务器针对获取奖励的返回事件
	*/
	DWORD OnNS_GetBenediction( tagNS_GetLoongBenediction* pMsg, DWORD dwParam );

	/** 服务器发送消息设置下一次赐福时间，以及物品信息
	*/
	DWORD OnNS_SetNextBenediction( tagNS_SetNextBenediction* pMsg, DWORD dwParam );

	/** 判断当前窗口是否是展开的
	*/
	BOOL IsWindowExpand();

	VOID ShowWnd( GUIWnd* pWnd, BOOL bShow );

	VOID EnableWnd( GUIButton* pBtn, BOOL bEnable );

private:
	TSFPTrunk<LoongBenedictionFrame>		m_Trunk;
	TObjRef<GUISystem>				m_pGUI;
	TObjRef<GameFrameMgr>			m_pFrameMgr;
	TObjRef<NetCmdMgr>				m_pCmdMgr;
	TObjRef<NetSession>				m_pSession;

	GUIWnd*							m_pWndExpand;
	GUIStatic*						m_pStcWaitTime;			//距离下一次神龙赐福的时间，倒计时
	GUIButton*						m_pBtnGetBenediction;	//领取
	GUIButton*						m_pBtnArrow;			//折叠
	GUIStatic*						m_pStcExprence;			//获得的经验

	IconStatic*						m_pStcItem;				//获得的道具

	GUIWnd*							m_pWndFold;					//折叠起来之后的界面
	GUIStatic*						m_pStcFoldWaitTime;			//距离下一次神龙赐福的时间，倒计时
	GUIButton*						m_pBtnFoldGetBenediction;	//领取
	GUIButton*						m_pBtnFoldArrow;			//折叠
	GUIWnd*							m_pWndCurDisplay;			//得到当前正在显示的控件

	DWORD							m_nItemTypeId;				//将要获得的道具的ID
	INT								m_nItemCount;				//奖励物品的个数
	//DWORD							m_dwExprence;				//将要获得的经验值

	DWORD							m_dwNeedTime;				//到下一次奖励需要的秒数
	float							m_dwStepTime;				//倒计时
	DWORD							m_dwStartTimer;				//计时器

	BOOL							m_bCanUpdate;				//能否更新，在没有收到服务器发来的 设置下一次赐福时间 之前是不能更新的
};
