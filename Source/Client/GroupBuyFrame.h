#pragma once
#include "GameClientDefine.h"
#include "..\WorldDefine\mall_define.h"
#include "..\WorldDefine\msg_mall.h"
#define Max_GroupBuy_Size 3

#define Max_GBInfo_PerPage 6
#define Max_Item_Pro 5

class MallButton;
class IconStatic;

/*
	class GroupBuyFrame
	brief 团购
*/
class GroupBuyFrame : public GameFrame
{
public:
	enum EGroupBuy
	{
		EGroupBuy_NULL			= 0,
		EGroupBuy_Purchase,
		EGroupBuy_Info,
	};
public:
	GroupBuyFrame(void);
	~GroupBuyFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
	virtual BOOL EscCancel();

	// 显示团购界面
	void ShowWnd(EGroupBuy eType=EGroupBuy_NULL);

	// 显示当前页的团购信息
	void ShowCurPageGroupBuyInfo(INT nPage);

	// 刷新团购信息
	void RefreshGroupBuyInfo();

private:
	DWORD EventHandler2GroupBuy(tagGUIEvent* pEvent);
	DWORD EventHandler2GroupInfo(tagGUIEvent* pEvent);

	//--网络消息
	DWORD OnNS_LaunchGroupPurchase(tagNS_LaunchGroupPurchase* pMsg, DWORD dwParam); 
	DWORD OnNS_Getparticipators(tagNS_Getparticipators* pMsg, DWORD dwParam);
	DWORD OnNS_RespondGroupPurchase(tagNS_RespondGroupPurchase* pMsg, DWORD dwParam);
	DWORD OnNS_RespondBroadCast(tagNS_RespondBroadCast* pMsg, DWORD dwParam);

	//--隐藏控件
	void HideTotalGroupBuyCtrl();
	void HideTotalGroupBuyInfoCtrl();

	// 发起团购
	void HandlerGroupLauncher();

	//--各属性判断
	bool JudgePromotePro(const tagMallItemProtoBase* pItem);
	bool JudgeGroupBuyPro(const tagMallItemProtoBase* pItem);
	bool JudgePresentPro(const tagMallItemProtoBase* pItem);
	bool JudgeNumPro(const tagMallItemProtoBase* pItem);
	bool JudgeExchangePro(const tagMallItemProtoBase* pItem);

	//--翻页
	void PageDown();
	void PageUp();

	//刷新所有团购商品的选择框
	void RefreshGroupBuyItemUI();

	// 显示商品的属性tips
	void ShowMallItemTips(int nPos, int nProPos);
	void ShowPromoteProTips(const tagMallItemProtoBase* pItem, int nGoodsPos, int nStatePos);
	void ShowGroupBuyProTips(const tagMallItemProtoBase* pItem, int nGoodsPos, int nStatePos);
	void ShowPresentProTips(const tagMallItemProtoBase* pItem, int nGoodsPos, int nStatePos);
	void ShowNumProTips(const tagMallItemProtoBase* pItem, int nGoodsPos, int nStatePos);
	void ShowExchangeProTips(const tagMallItemProtoBase* pItem, int nGoodsPos, int nStatePos);

	//响应团购判断
	bool JudgeResponseGroupBuy();

	//列出团购商品所有响应者
	void ListResponse();

private:
	TSFPTrunk<GroupBuyFrame>		m_Trunk;
	TObjRef<GUISystem>				m_pGUI;
	TObjRef<Util>					m_pUtil;
	TObjRef<GameFrameMgr>			m_pFrameMgr;
	TObjRef<NetSession>				m_pSession;
	TObjRef<NetCmdMgr>				m_pCmdMgr;

	//--发起团购
	GUIWnd*							m_pWndGBuy;
	GUIButton*						m_pBtnLauncher;
	GUIButton*						m_pBtnCancel;
	MallButton*						m_pStcItem;											//发起团购的商品
	GUIStatic*						m_pStcHoldTime;										//团购持续时间
	GUIStatic*						m_pStcGBBack[Max_GroupBuy_Size];					//规模背景
	GUIStatic*						m_pStcResponse[Max_GroupBuy_Size];					//响应人数
	GUIStatic*						m_pStcDiscount[Max_GroupBuy_Size];					//折扣摔
	GUIStatic*						m_pStcPrice[Max_GroupBuy_Size];						//价格
	GUIPushButton*					m_pPBtnScale[Max_GroupBuy_Size];					//团购规模
	GUIStatic*						m_pStcScaleBack[Max_GroupBuy_Size];					//团购规模背景
	
	//--团购信息
	GUIWnd*							m_pWndGInfo;
	GUIButton*						m_pBtnPageUp;
	GUIButton*						m_pBtnPageDown;
	GUIStatic*						m_pStcPage;											//当前页
	GUIButton*						m_pBtnResponse;										//响应
	GUIStatic*						m_pBtnGroupBuyExp;									//帮派团购指数
	GUIStatic*						m_pStcInfoBack[Max_GBInfo_PerPage];					//商品的背景
	IconStatic*						m_pStcInfoSele[Max_GBInfo_PerPage];					//团购商品的选择框
	MallButton*						m_pBtnBGItemInfo[Max_GBInfo_PerPage];				//每页显示的团购信息数量
	GUIStatic*						m_pStcItemName[Max_GBInfo_PerPage];					//商品名称
	GUIStatic*						m_pStcItemPrice[Max_GBInfo_PerPage];				//价格
	IconStatic*						m_pStcGBItemState[Max_GBInfo_PerPage][Max_Item_Pro];//每个商品的属性
	GUIListBox*						m_pLBoxResponse;									//帮派响应列表
	GUIStatic*						m_pStcResBack[Max_GBInfo_PerPage];					//响应背景
	GUIStatic*						m_pStcLauncherName[Max_GBInfo_PerPage];				//发起人
	GUIStatic*						m_pStcResCount[Max_GBInfo_PerPage];					//响应人数
	GUIStatic*						m_pStcResTime[Max_GBInfo_PerPage];					//响应剩余时间


	INT								m_nInfoCurPage;										//团购信息当前页
	INT								m_nInfoMaxPage;										//团购信息总页数
	EGPSCOPE						m_eScope;											//团购类型
	DWORD							m_dwGLauncherItemID;								//发起团购商品ID
		
	DWORD							m_dwUpdateTime;										//向服务器更新时间间隔
	INT								m_nCurPage;											//响应团购当前页
	INT								m_nMaxPage;											//响应团购总页数

	BtnData							m_seleDate;											//记录选择的团购商品

	TList<DWORD>					m_listRespons;										//响应者列表
};