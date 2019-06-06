#pragma once

/** \光标的状态
*/
enum ECursorState
{
	ECS_Idle,		// 空闲
	ECS_Active,		// 激活
	ECS_Sum
};

/** \光标激活的类型
*/
enum ECursorActiveName {
	ECAN_Null,

	ECAN_ItemStart	= 20,	// 特殊物品使用起始，为其他激活类型留出位置方便判断
	ECAN_Identify,			// 鉴定卷
	ECAN_Gem,				// 宝石
	ECAN_Brand,				// 烙印
	ECAN_Longfu,			// 龙附
	ECAN_Chisel,			// 开凿
	ECAN_Grind,				// 磨石
	ECAN_Key,				// 开宝箱 by xtian
    ECAN_Dye,               // 染色
	ECAN_AddPot,			// 提升装备潜力值
	ECAN_BeGoldStone		// 点金石
};

/** \光标空闲的类型
*/
enum ECursorIdleName
{
	ECIN_Default,           // 默认
	ECIN_Attack,            // 攻击
	ECIN_NPCTalk,           // NPC对话
	ECIN_NPCShop,           // 商店NPC
	ECIN_PickGroundItem,    // 拾取地面物品
	ECIN_Gather,            // 采集
	ECIN_Hyperlink,         // 超链接
	ECIN_DigBao,			// 挖宝
};

/** \光标显示的所有样式
*/
enum ECursorDisplayType
{
	ECDT_Default,           // 默认
	ECDT_Attack,            // 攻击
	ECDT_NPCTalk,           // NPC对话
	ECDT_PickGroundItem,    // 拾取地面物品
	ECDT_Gather,            // 采集
	ECDT_Hyperlink,         // 超链接
	ECDT_Identify,          // 鉴定
	ECDT_UseSpecilItem,     // 特殊物品使用
	ECDT_NPCShop,           // 商店NPC
	ECDT_DigBao,			// 挖宝
	ECDT_Num,
};

/** \
*/
struct tagActiveCursorEvent : public tagGameEvent
{
	ECursorActiveName	eCursor;
	DWORD				dwParam;

	tagActiveCursorEvent(LPCTSTR szEventName, GameFrame* pSenderFrame):tagGameEvent(szEventName, pSenderFrame)
	{
		eCursor = ECAN_Null;
		dwParam = GT_INVALID;
	}
};

/** \class CursorMgr, 游戏光标管理器
    \brief 用于加载、保存、更新游戏中的光标，并管理光标的状态
	\todo  暂时还不支持光标动画
*/
class CursorMgr
{
public:
	CursorMgr();
	~CursorMgr();

	static CursorMgr* Inst();

	void Init(HINSTANCE hInst);
	void Destroy();

	void Active(tagActiveCursorEvent* pGameEvent);
	void OnWindowsMsg(DWORD dwMsg, DWORD dwParam1, DWORD dwParam2);
	void SetIdleCursor( const ECursorIdleName eIn );

	ECursorState GetCursorState(){return m_eCursorState;}
	ECursorActiveName GetCursorActive(){return m_eActiveCursor;}
	ECursorIdleName GetCursorIdle(){return m_eIdleCursor;}
	DWORD GetCursorActiveParam(){return m_dwParam;}

	void Change2IdleState();
	const tagPoint& GetCursorPos() const { return m_ptCursor; }
protected:
	ECursorDisplayType GetCursorDisplay();
private:
	TSFPTrunk<CursorMgr>	m_Trunk;
	ECursorState			m_eCursorState;		// 当前光标状态
	ECursorActiveName		m_eActiveCursor;	// 当前光标激活状态的类型
	ECursorIdleName         m_eIdleCursor;      // 当前光标空闲状态的类型
	DWORD					m_dwParam;			// 参数m_dwParam是根据当前鼠标激活的类型来确定的
												// ECAN_Identify，ECAN_Gem，ECAN_Brand，ECAN_Longfu：m_dwParam为物品在背包中的位置

	HCURSOR                 m_hCursor[ECDT_Num];
	ECursorDisplayType      m_eCursorDisplay;   // 当前光标的显示样式
	tagPoint				m_ptCursor;         // 当前鼠标的窗口坐标
};



