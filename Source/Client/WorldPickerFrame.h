#pragma once

struct tagGroundItem;
struct tagStall;
class GUIRender;

/** \拾取类型
*/
enum EWorldPickType
{
	EWPT_None,
	EWPT_LClick,    // 左键单击
	EWPT_RClick,    // 右键单击
};

/** \选中角色事件,dwRoleID == GT_INVALID为取消选中
*/
struct tagRolePickEvent : public tagGameEvent
{
	EWorldPickType eType;
	DWORD dwRoleID;
	tagRolePickEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : 
	tagGameEvent(szEventName,pSenderFrame), eType( EWPT_None ), dwRoleID( GT_INVALID ) {} 
};

/** \拾取场景事件
*/
struct tagScenePickEvent : public tagGameEvent
{
	EWorldPickType eType;
	Vector3 vPickPos;
	tagScenePickEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : 
	tagGameEvent(szEventName,pSenderFrame), eType( EWPT_None ), vPickPos( 0.0f, 0.0f, 0.0f ) {}
};

/** \拾取地面物品事件
*/
struct tagGroundItemPickEvent : public tagGameEvent
{
	EWorldPickType eType;
	tagGroundItem* pGroundItem;
	tagGroundItemPickEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : 
	tagGameEvent(szEventName,pSenderFrame), eType( EWPT_None ), pGroundItem( NULL ) {}
};

/** \显示场景贴花事件
*/
struct tagShowScenePickDecalEvent : public tagGameEvent
{
	BOOL bShow;
	tagShowScenePickDecalEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : 
	tagGameEvent(szEventName,pSenderFrame), bShow( false ) {}
};


/** \class  WorldPickerFrame
	\brief  场景拾取
	\remark 处理场景拾取检测及相关游戏事件发送
*/
class WorldPickerFrame : public GameFrame
{
	enum EMousePointType
	{
		EMPT_None = -1,
		EMPT_Role = 0,           // 角色
		EMPT_GroundItem,         // 地面物品
		EMPT_Scene,              // 场景
		EMPT_Water,              // 水面
		EMPT_Stall,				 // 摊位
		EMPT_Num
	};
	struct tagMousePointObject
	{
		bool            bValid;			// 是否有效
		Vector3         vPos;			// 对象坐标
		tagMousePointObject() : bValid( false ), vPos( FLOAT_MAX, FLOAT_MAX, FLOAT_MAX ) {}
	};
public:
	WorldPickerFrame(void);
	virtual ~WorldPickerFrame(void);

	// GameFrame
	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual VOID Update();

	/** \获取当前鼠标指向的角色ID,返回GT_INVALID表示没有指向角色
	*/
	DWORD GetCurMousePointRoleID() const;
	/** \获取当前鼠标指向的地面物品结构指针,返回NULL表示没有指向地面物品
	*/
	tagGroundItem* GetCurMousePointGroundItem() const;
	/** \获取当前选中的角色ID,返回GT_INVALID表示没有选中角色
	*/
	DWORD GetCurSelectedRoleID() const { return m_dwSelectedRoleID; }
private:
	VOID  UpdataMousePoint( const Ray& ray, const BOOL bIgnorePlayer );
	VOID  UpdataCursor(); 
	DWORD OnOpenMap( tagGameEvent* );
	DWORD OnCloseMap( tagGameEvent* );
	DWORD OnShowScenePickDecalEvent( tagShowScenePickDecalEvent* pEvent );
	DWORD OnRolePickEvent( tagRolePickEvent* pEvent );
	VOID  ReBuildScenePickDecal( const Vector3& pickPos, const Ray& ray, const bool bWater = false );
private:
	TSFPTrunk<WorldPickerFrame>		m_trunk;
	TObjRef<InputDX8>				m_pInput;
	TObjRef<GUISystem>				m_pGUI;
	TObjRef<GUIRender>				m_pRender;

	EMousePointType                 m_eMousePointType;      // 当前鼠标指向的类型
	DWORD                           m_dwMousePointRoleID;   // 当前鼠标指向的角色ID
	DWORD                           m_dwSelectedRoleID;     // 当前选中的角色ID
	Vector3                         m_vMousePointScenePos;  // 当前鼠标指向的场景坐标
	Vector3                         m_vMousePointWaterPos;  // 当前鼠标指向的水面坐标
	tagGroundItem*                  m_pGroundItem;          // 当前鼠标指向的掉落物品结构体指针
	tagStall*						m_pStall;				// 当前鼠标指向的摊位结构体指针
	SGEffectNode*                   m_pScenePickDecal;		// 场景贴花
	bool                            m_bDecalAttached;		// 贴花特效是否已绑定SG
};

inline WorldPickerFrame* GetWorldPickerFrm()
{
	return (WorldPickerFrame*)TObjRef<GameFrameMgr>()->GetFrame(_T("WorldPicker"));
}

inline DWORD GetCurMousePointRoleID()
{
	WorldPickerFrame* pWorldPicker = GetWorldPickerFrm();
	if( P_VALID(pWorldPicker) )
		return pWorldPicker->GetCurMousePointRoleID();
	else
		return GT_INVALID;
}