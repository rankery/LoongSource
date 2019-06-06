#pragma once

#include "..\WorldDefine\container_define.h"
class PlayerViewWindow;
class ItemButton;
class NetSession;
struct tagUpdateLPItemEvent;
struct tagItemPosChangeExEvent;
struct tagUpdataAvatarEvent;
struct tagFashionEvent;
struct tagUpdataAvatarDisplaySetEvent;

/** \class EquipmentFrame
	\brief 装备界面管理
*/
class EquipmentFrame :
	public GameFrame
{
public:
	EquipmentFrame(void);
	~EquipmentFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
	virtual VOID Render3D();
	//--Avatar刷新
	VOID UpdateEquipAvatar();

private:
	//--处理GUI事件
	DWORD EventHandler(tagGUIEvent* pEvent);
	
	//--处理物品刷新事件
	DWORD OnUpdateItemEvent(tagUpdateLPItemEvent* pGameEvent);
	//--处理物品换位事件
	DWORD OnChangeItemEvent(tagItemPosChangeExEvent* pGameEvent);
	//--处理Avatar变化事件
	DWORD OnUpdateAvatarEvent(tagUpdataAvatarEvent* pGameEvent);
	/*! 处理装备/时装模式改变事件 */
	DWORD OnChangDispPattern(tagFashionEvent *pGameEvent);

	DWORD OnKeySwapWeapon(DWORD dwID, BOOL bDown);
	
	//--装备刷新
	VOID UpdateEquipment(EEquipPos ePos);
	/*! \brief 刷新物品按钮
		\param bFashion 是否服装模式
	*/
	VOID UpdateAll();
	//--获取装备栏位
	EEquipPos GetEquipPos(INT nIndex);
	//--是否可移动
	BOOL IsEquipPosFix(EEquipPos eEquipPos, EEquipPos &eDstPos);
	
	//--穿装备
	VOID OnEquipment(INT16 nItemPos, EEquipPos ePos = EEP_End);
	//--装备切换
	VOID OnEquipSwap(EEquipPos ePosSrc, EEquipPos ePosDst);
	//--显示设置改变
	DWORD OnUpdataAvatarDisplaySetEvent(tagUpdataAvatarDisplaySetEvent* pEvent);

    // 打开时装模式下不需要的装备栏
    void TurnFashionPos(bool on);
private:
	TSFPTrunk<EquipmentFrame>	m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<GameFrameMgr>		m_pFrameMgr;
	TObjRef<NetSession>		    m_pSession;
	TObjRef<GameInputMap>		m_pKeyMap;

	GUIWnd*						m_pWnd;
	GUIWnd*						m_pWndFather;
	GUIStatic*					m_pStcEquipBack[EEP_MaxEquip];
	ItemButton*					m_pBtnEquip[EEP_MaxEquip];
	PlayerViewWindow*			m_pWndEquipView;

	BOOL						m_bIsFashionDisplay;	/*!< 是否是时装模式 */

};
