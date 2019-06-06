#pragma once
#include "..\WorldDefine\container_define.h"



class ItemButton;
class ItemFrame;
class Item;
class NetSession;
class ItemMenuButton;
struct tagUpdateLPItemEvent;
struct tagItemPosChangeExEvent;
struct tagRoleSilverChange;
struct tagRoleYuanbaoChange;

/**	\class StorageFrame
	\brief 仓库界面处理框架类
	\remarks 处理仓库中的物品金钱显示和物品操作 
*/
class StorageFrame :
	public GameFrame
{

public:

	StorageFrame(void);
	~StorageFrame(void);
	
	/** \GameFrame
	*/
	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
	virtual BOOL EscCancel();

    void SetWalkWareEnable(bool bEnabled) { m_bWalkWare = bEnabled; }
	bool GetWalkWareMode(){ return m_bWalkWare; }
private:
	/** \处理GUI消息
	*/
	DWORD EventHandler(tagGUIEvent* pEvent);
	/** \处理游戏事件
	*/
	DWORD OnEventUpdateLPItem(tagUpdateLPItemEvent* pGameEvent);
	DWORD OnEventItemPosChange(tagItemPosChangeExEvent* pGameEvent);
	DWORD OnEventRoleWareSilverChange(tagRoleSilverChange* pGameEvent);
	/*VOID OnEventRoleWareYuanbaoChange(tagRoleYuanbaoChange* pGameEvent);*/
	DWORD OnEventSaveMoneyToWare(tagMsgMoneyBoxEvent* pGameEvent);
	DWORD OnEventLoadMoneyFromWare(tagMsgMoneyBoxEvent* pGameEvent);
	DWORD OnEventWareExtend(tagGameEvent* pGameEvent);

	/** \显示当前页
	*/
	VOID ShowCurPage(INT16 page);
	

	/** \更新物品
		\param index为物品序号（即物品在容器中的序号）
	*/
	VOID UpdateItem(INT16 index);
	/** \更新货币（包括金钱和元宝）
	*/
	VOID UpdateMoney();

	
	/** \设置金钱数量
	*/
	VOID SetMoneyValue(INT64 n64Value);
	/** \设置元宝数量
	*/
	/*VOID SetTreasureValue(INT nValue);*/

	/** \获取当前位置上的物品;
		\param index为物品序号
	*/
	Item* GetItem(INT16 index);

	/** \刷新物品按钮
		\param index为物品序号
	*/
	VOID RefreshItem(INT16 index);


	/** \处理物品移动
		\param n16PosSrc为移动的物品序号
		\param n16PosDst为移动至该位置，默认值为移动位置不指定
	*/
	VOID OnItemChange(INT16 n16PosSrc, INT16 n16PosDst = GT_INVALID);
	/** \处理物品移入仓库
		\param ePosSrc为物品原容器
		\param ePosDst为物品目的容器
		\param n16PosSrc为物品原序号
		\param n16PosDst为移动至该位置，默认值为移动位置不指定
	*/
	VOID OnItemChangeEx(EItemConType ePosSrc, EItemConType ePosDst, INT16 n16PosSrc, INT16 n16PosDst = GT_INVALID);


	/** \是否在当前页
		\param index为物品序号
	*/
	BOOL IsCurPage(INT16 index){return ((SPACE_ONE_WARE * m_n16CurPage <= index) && (SPACE_ONE_WARE * (m_n16CurPage + 1) > index));}

	/** \销毁子窗体
	*/
	VOID DestroyChild();
	/** \消毁相关的对话框
	*/
	VOID DestroyMsgBox();

private:

	TSFPTrunk<StorageFrame>		m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<GameFrameMgr>		m_pFrameMgr;
	TObjRef<NetSession>		m_pSession;

	GUIWnd*						m_pWnd;
	GUIWnd*						m_pWndCaption;
	ItemMenuButton*				m_pPbnStorageIndex[MAX_WARE_NUM];		// 仓库选择按钮
	GUIStatic*					m_pStcStorage;							// 仓库页
	GUIButton*					m_pBtnSaving;							// 仓库存款按钮
	GUIButton*					m_pBtnLoading;							// 仓库取款按钮
	GUIButton*					m_pBtnSettle;							// 仓库整理按钮
	GUIButton*					m_pBtnExit;								// 退出按钮
	GUIStatic*					m_pStcGold;								// 金币显示框
	GUIStatic*					m_pStcSilver;							// 银币显示框
	//GUIStatic*					m_pStcTreasure;							// 元宝显示框

	ItemButton*					m_pUnitItem[SPACE_ONE_WARE];			// 仓库单元格

	INT16						m_n16CurPage;							// 为当前仓库页

	DWORD						m_dwNPCID;								// 仓库NPCID

    bool                        m_bWalkWare;                            // 是否是随身仓库
};
