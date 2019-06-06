#pragma once
#include "..\WorldDefine\container_define.h"

class ItemButton;
class ItemMenuButton;
class ItemFrame;
class Item;
class NetSession;
class ThroneFrame;
struct tagUpdateLPItemEvent;
struct tagItemPosChangeExEvent;
struct tagRoleSilverChange;
struct tagRoleYuanbaoChange;

/**	class	PocketFrame
	brief	行囊界面处理框架类
	remarks	处理行囊中的物品金钱显示和物品操作 
*/
class PocketFrame :
	public GameFrame
{

public:

	PocketFrame(void);
	~PocketFrame(void);
	
	/** \GameFrame
	*/
	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
	virtual BOOL EscCancel();
	
	/** \显示行囊界面
	*/
	VOID ShowWnd(BOOL bShow);

private:
	/** \处理GUI消息
	*/
	DWORD EventHandler(tagGUIEvent* pEvent);
	/** \处理游戏事件
	*/
	DWORD OnUpdateLPItem(tagUpdateLPItemEvent* pGameEvent);
	DWORD OnItemPosChangeEvent(tagItemPosChangeExEvent* pGameEvent);
	DWORD OnBagSilverChange(tagRoleSilverChange* pGameEvent);
	DWORD OnBagYuanbaoChange(tagRoleYuanbaoChange* pGameEvent);
	DWORD OnInputItemNumber(tagMsgInputBoxEvent* pGameEvent);
	DWORD OnUpdateEQDataChange(tagUpdateLPItemEvent* pGameEvent);
	DWORD OnUpdateItemCD(tagGameEvent* pGameEvent);
	DWORD OnEventBagExtend(tagGameEvent* pGameEvent);
	DWORD OnOpen_Encrypt(tagGameEvent* pGameEvent);

	DWORD OnEventSetPsdFirst(tagMsgInputBoxEvent* pGameEvent);
	DWORD OnEventSetPsdSecond(tagMsgInputBoxEvent* pGameEvent);
	DWORD OnEventModPsdFirst(tagMsgInputBoxEvent* pGameEvent);
	DWORD OnEventModPsdSecond(tagMsgInputBoxEvent* pGameEvent);
	DWORD OnEventModPsdThird(tagMsgInputBoxEvent* pGameEvent);
	DWORD OnEventDelPsdFirst(tagMsgInputBoxEvent* pGameEvent);
	DWORD OnEventOpenBagPsd(tagMsgInputBoxEvent* pGameEvent);
	
	/** \显示当前页
	*/
	VOID ShowCurPage(INT16 page);
	
	/** \更新最大页数
	*/
	VOID UpdateMaxPage();
	/** \更新物品
		\param index为物品序号（即物品容器序号）
	*/
	VOID UpdateItem(INT16 index);
	/** \更新货币(包括金钱和元宝)
	*/
	VOID UpdateMoney();

	/** \设置金钱数量
		\param n64Value为金钱数量
	*/
	VOID SetMoneyValue(INT64 n64Value);
	/** \设置元宝数量
		\param nValue为元宝数量
	*/
	VOID SetTreasureValue(INT nValue);
	
	/** \获取当前位置上的物品 
		\param index为物品序号
	*/
	Item* GetItem(INT16 index);

	/** \刷新物品按钮
		\param index为物品序号
	*/
	VOID RefreshItem(INT16 index);
	/** \刷新物品Tips
		\param pItem为物品指针
		\param pWnd为相应物品按钮指针
	*/
	VOID ReflashItemTips(Item* pItem, GUIWnd* pWnd);

	/** \处理物品移动
		\param n16PosSrc为移动的物品序号
		\param n16PosDst为移动至该位置，默认值为移动位置不指定
	*/
	VOID OnItemChange(INT16 n16PosSrc, INT16 n16PosDst = GT_INVALID, INT16 n16NumDst = 0);
	/** \处理物品移入背包
		\param ePosSrc为原物品容器
		\param n16PosSrc为原物品序号
		\param n16PosDst为移动至该位置，默认值为移动位置不指定
		\param n16NumDst为移入目标位置的数量，默认值为全部移入
	*/
	VOID OnItemChangeEx(EItemConType ePosSrc, INT16 n16PosSrc, INT16 n16PosDst = GT_INVALID);
	/** \处理商品购买
		\param nShelfPos为该商品在商店中的序号，方便查找原型
		\param dwTypeID为该商品的物品原型ID
		\param n16Num为购买的数量
	*/
	VOID OnBuyItem(INT nShelfPos, DWORD dwTypeID, INT16 n16Num = 1);
	/** \处理装备卸下
		\param ePos为物品在装备栏中的位置
		\param n16PosDst为移动至该位置，默认值为移动位置不指定
	*/
	VOID OnUnEquipment(EEquipPos ePos, INT16 n16PosDst = GT_INVALID);
    VOID OnPetUnEquipment(DWORD dwPetID, INT8 n8PebPos, INT16 n16PosDst = GT_INVALID);
	/** \处理鼠标右键点击物品事件
		\param index为物品序号
	*/
	VOID TryToUseItem(INT16 index);
	/** \处理鼠标左键点击拖动物品事件
		\param index为物品序号
		\param bShift为是否按住Shift键标志
	*/
	VOID TryToDragItem(INT16 index, bool bShift);
	/** \物品拖动
		\param pItem为物品拖动指针
		\param n16Num为要拖动的数量
	*/
	VOID OnItemDrag(Item* pItem, INT16 n16Num, bool bSplit = false);

		
	/** \计算是否在当前页
		\param index为物品序号
	*/
	BOOL IsCurPage(INT16 index){return ((SPACE_ONE_BAG * m_n16CurPage <= index) && (SPACE_ONE_BAG * (m_n16CurPage + 1) > index));}
	/** \销毁子窗体
	*/
	VOID DestroyChild();
	/** \消毁相关的对话框
	*/
	VOID DestroyMsgBox();

	/** \显示Tip
		\param n16Index为物品序号
	*/
	VOID ShowItemTip(GUIWnd* pWnd, INT16 n16Index);

private:

	TSFPTrunk<PocketFrame>		m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<GameFrameMgr>		m_pFrameMgr;
	TObjRef<NetSession>		m_pSession;

	GUIWnd*						m_pWnd;
	GUIWnd*						m_pWndCaption;
	GUIStatic*					m_pStcPocket;							// 背包页
	GUIButton*					m_pBtnThorne;							// 百宝箱按钮
	GUIButton*					m_pBtnSettle;							// 背包整理按钮
	GUIButton*					m_pBtnEncrypt;							// 行囊加密按钮
	GUIButton*					m_pBtnStall;							// 关闭按钮
	GUIButton*					m_pBtnExit;								// 退出按钮
	GUIStatic*					m_pStcGold;								// 金币显示框
	GUIStatic*					m_pStcSilver;							// 银币显示框
	GUIStatic*					m_pStcTreasure;							// 元宝显示框
	
	ItemMenuButton*				m_pPbnPocketIndex[MAX_BAG_NUM+1];		// 背包选择按钮需要加上任务物品栏
	ItemButton*					m_pUnitItem[SPACE_ONE_BAG];				// 背包单元格

	INT16						m_n16CurPage;							// 当前页

	Item*						m_pSliptItem;							// 要拆分的物品缓存


	DWORD							m_dwNewPsdCrc;		// 输入的新密码的CRC码
	DWORD							m_dwOldPsdCrc;		// 输入的旧密码的CRC码
};
