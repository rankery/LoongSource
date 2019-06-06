#pragma once
#include "..\WorldDefine\container_define.h"

class ItemButton;
class ItemFrame;
class Item;
class NetSession;
struct tagUpdateLPItemEvent;
struct tagRoleYuanbaoChange;
struct tagRoleGetNameEvent;
/**	\class StorageFrame
	\brief 百宝袋界面处理框架类
	\remarks 处理百宝袋中的物品操作 
*/
class ThroneFrame :
	public GameFrame
{

public:

	ThroneFrame(void);
	~ThroneFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();

	virtual BOOL EscCancel();

private:
	// 处理GUI消息
	DWORD EventHandler(tagGUIEvent* pEvent);
	// 处理游戏事件
	DWORD OnEventUpdateLPItem(tagUpdateLPItemEvent* pGameEvent);
	DWORD OnEventYuanbaoChange(tagRoleYuanbaoChange* pGameEvent);
	DWORD OnEventLoadYuanbao(tagMsgMoneyBoxEvent* pGameEvent);
	DWORD OnEventBaiBaoRecord(tagGameEvent* pGameEvent);
	DWORD OnEventRoleGetName(tagRoleGetNameEvent* pGameEvent);
	// 刷新百宝袋
	VOID ReflashThrone();

	// 更新元宝
	VOID UpdateYuanbao(INT nYuanbao);

	// 更新物品
	VOID UpdateItem(INT16 index);

	// 将物品放入其他容器中
	VOID OnItemPosChangeEx(INT16 n16Pos, EItemConType eDstCon = EICT_Bag, INT16 n16DstPos = GT_INVALID);

	// 获取当前位置上的物品;
	Item* GetItem(INT16 index);

	void DestroyMsgBox();

	// 刷新百宝袋记录
	void ReflashRecord();

private:

	TSFPTrunk<ThroneFrame>		m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<GameFrameMgr>		m_pFrameMgr;
	TObjRef<NetSession>			m_pSession;

	GUIWnd*						m_pWnd;
	GUIWnd*						m_pWndCaption;
	GUIWnd*						m_pWndList;								// 明细显示窗体
	GUIStatic*					m_pStcThrone;							// 百宝袋页
	GUIButton*					m_pBtnExit;								// 退出按钮
	GUIButton*					m_pBtnSwitch;							// 状态切换按钮
	GUIButton*					m_pBtnLoading;							// 取款
	GUIStatic*					m_pStcYuanbao;							// 元宝显示
	GUIStaticEx*				m_pStcList;								// 明细显示列表

	ItemButton*					m_pUnitItem[SPACE_BAIBAO_BAG];			// 百宝袋单元格

	std::set<DWORD>				m_setRecordRoleNameReq;					// 记录中角色ID获取缓存
};
