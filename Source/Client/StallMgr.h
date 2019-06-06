#pragma once

class TemporaryContainer;
class Item;
class LocalPlayer;
struct tagTemporaryItem;
struct tagRolePickEvent;
struct tagVIPStall;
struct tagNS_StallStart;
struct tagNS_StallClose;
struct tagNS_StallGetTitle;
struct tagNS_StallBuyRefresh;
struct tagNS_GetAllVIPStallInfo;
struct tagNS_UpdateVIPStallInfo;
struct tagNS_StallSetFinish;
struct tagRoleGetNameEvent;

struct tagStallAppearEvent : public tagGameEvent
{
	DWORD	dwRoleID;
	Vector3 vPos;
	FLOAT	fYaw;
	BYTE    byStallLevel;

	tagStallAppearEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName, pSenderFrame){}
};

struct tagStallUpdateItemEvent : public tagGameEvent
{
	INT16	n16Pos;

	tagStallUpdateItemEvent(LPCTSTR szEventName, GameFrame* pSenderFrame) : tagGameEvent(szEventName, pSenderFrame){}
};


struct tagStallStarEvent : public tagGameEvent
{
	INT nLevel;
	tagStallStarEvent(LPCTSTR szEventName, GameFrame *pSenderFrame) : tagGameEvent(szEventName, pSenderFrame){}

};

struct tagStall
{
	DWORD			dwRoleID;		// 谁的摊位
	BYTE			byLevel;		// 摊位等级
	BYTE			byType;			// 招牌类型
	BYTE			byDummy[2];		
	ObjectSpace		space;			// 摊位3D位置
	EntityNode*		pNode;			// 摊位模型
	TCHAR			szName[X_SHORT_NAME]; // 摊位名称
    bool            bCustomName;    // 是否自定义摊位名称

    tagStall() :
        dwRoleID(GT_INVALID),
        byLevel(0),
        byType(0),
        pNode(0),
        bCustomName(false)
    {
        byDummy[0]  = 0;
        byDummy[1]  = 0;
        szName[0] = 0;
    }
};

struct tagVipStallEvent : public tagGameEvent
{
	DWORD			dwRoleID;		// vip的摊位
	BYTE			byIndex;		// vip摊位编号

	tagVipStallEvent(LPCTSTR szEventName, GameFrame* pSenderFrame)
		: tagGameEvent(szEventName, pSenderFrame)
	{
		dwRoleID = GT_INVALID;
		byIndex	 = GT_INVALID;
	}
};

class StallMgr
{
public:
	enum EOperateStallType
	{
		EOperateStallType_NULL		= -1,
		EOperateStallType_Normal,				//正常打开方式
		EOperateStallType_Vip,					//VIP打开方式
	};

	struct tagStallModelConfig
	{
		int nStallLevel;	// 摊位等级
		int nModel;			// 摊位模型
		int nSignBoard;		// 招牌模型
	};

	typedef std::map<int, tagStallModelConfig> StallModelConfigMap;

public:
	StallMgr(void);
	~StallMgr(void);

	VOID Init();
	VOID Destroy();

	tagStall* PickStall(const Ray& rayMouse);

	BOOL AddItem(Item* pItem, INT16 &nIndex, INT64 n64Price = 0);
	BOOL DelItem(INT16 nIndex, BOOL bUnlock = TRUE);
	tagTemporaryItem* GetItem(INT16 nIndex);

	tstring GetStallStars(INT64 nTotalTax);
	
	VOID ShowStallErrorMsg(DWORD dwErrorCode);

	BOOL IsStallState();

	BOOL IsOutValidDist();
	
	// 是否可以摆摊
	BOOL CanStall(BYTE byLevel);

	// 设置/获取摊位名称
	VOID SetStallName(DWORD dwRoleID, LPCTSTR szName, bool bCustom);
	LPCTSTR GetStallName(DWORD dwRoleID);

    // 请求获得NameTable角色名字事件返回处理
    DWORD OnGetPlayerName(tagRoleGetNameEvent* pGameEvent);

	// 设置/获取本地玩家广告标志位
	VOID SetStallAdFlag(BOOL bFlag){m_bSetAdFlag = bFlag;}
	BOOL GetStallAdFlag(){return m_bSetAdFlag;}

	// 获取摊位列表
	TMap<DWORD,tagStall*>& GetStallMap() { return m_mapStall; }

	//设置打开摊位方式
	void SetOperateStallType(EOperateStallType eType=EOperateStallType_Normal){ m_eOperateStallType = eType; }
	EOperateStallType GetOperateStallType() { return m_eOperateStallType; }

	//--检查vip摊位状态
	BOOL IsCanCheckVipStall(BYTE byIndex);
	BOOL IsCanRentVipStall(BYTE byIndex);

	//检查自己有没有vip摊位
	BOOL IsSelfVipStall();

	//刷新VIP摊们显示
	void RefreshVipStallState(DWORD dwRoleID);

	// 获取vip摊位信息
	map<BYTE, tagVIPStall>& GetVipStallInfo() { return m_mapVipStall; }
	tagVIPStall* FinVipStallInfo(BYTE byKey);

	static StallMgr* Inst();

	// 根据摊位等级取得摊位模型路径
	tstring GetStallModelPath(int nStallLevel);

	// 根据摊位等级取得招牌路径
	tstring GetStallSignBoardPath(int nStallLevel);

private:

	DWORD OnEventStallAppear(tagStallAppearEvent* pGameEvent);
	DWORD OnEventStallDisappear(tagStallAppearEvent* pGameEvent);
	DWORD OnEventPickStall(tagRolePickEvent*	pGameEvent);
	DWORD OnEventOpenStall(tagGameEvent* pGameEvent);
	DWORD OnEventCloseStall(tagGameEvent* pGameEvent);
	DWORD OnEventCloseVend(tagGameEvent* pGameEvent);
	DWORD OnEventOpenVipStall(tagGameEvent* pGameEvent);
	DWORD OnCheckVipStallEvent(tagVipStallEvent* pGameEvent);

	DWORD OnNetStallStart(tagNS_StallStart* pNetCmd, DWORD);
	DWORD OnNetStallClose(tagNS_StallClose* pNetCmd, DWORD);
	DWORD OnNetStallGetTitle(tagNS_StallGetTitle* pNetCmd, DWORD);
	DWORD OnNetStallBuyRefresh(tagNS_StallBuyRefresh* pNetCmd, DWORD);
	DWORD OnNetGetAllVIPStallInfo(tagNS_GetAllVIPStallInfo* pNetCmd, DWORD);
	DWORD OnNetUpdateVIPStallInfo(tagNS_UpdateVIPStallInfo* pNetCmd, DWORD);
	DWORD OnNetStallSetFinish( tagNS_StallSetFinish* pNetCmd, DWORD );

	BOOL LoadStallModelConfig(tstring strConfigPath);


	VOID SendToGetStallTitle(DWORD dwRoleID);

	// 初始化摊位容器
	VOID InitStallContainer();	
	// 存储链表，清理容器内容
	VOID RecordStallList();	

	VOID OpenStallShelf();
	VOID CloseStallShelf();
	VOID OpenStallVend();
	VOID CloseStallVend();

	BOOL ShowStall(tagStall* pStall);
	VOID HideStall(tagStall* pStall);

	BOOL IsAABBCollideStall(const AABBox &box);

	BOOL IsAABBCollideGround(const AABBox &box, Vector3 &wpt);
	
	// 根据摊位等级获取一个摊位相应的AABBox
	VOID GetStallAABBox(AABBox& out, Vector3& wpt, BYTE byLevel);

private:
	TSFPTrunk<StallMgr>			m_Trunk;
	TObjRef<Util>				m_pUtil;
	TObjRef<NetCmdMgr>			m_pCmdMgr;
	TObjRef<NetSession>			m_pSession;
	TObjRef<GameFrameMgr>		m_pFrameMgr;

	TMap<DWORD, tagStall*>		m_mapStall;	
	TemporaryContainer*			m_pContainerStall;
	TMap<INT64, INT64>			m_mapStallItem;
	LocalPlayer*				m_pLp;
	DWORD						m_dwStallRoleID;
	tstring						m_strLPStallName;
	BOOL						m_bSetAdFlag;

	//--VIP摊位
	EOperateStallType			m_eOperateStallType;
	map<BYTE, tagVIPStall>		m_mapVipStall;
	DWORD						m_dwVipTime;
	
	StallModelConfigMap			m_mapStallModelConfigs;
};
