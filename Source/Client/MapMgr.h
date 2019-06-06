#pragma once

/** \class MapMgr
	\brief 游戏场景管理器
*/
class MapMgr
{
public:
	enum{MAX_VIEW_DIST=4};
public:
	MapMgr(void);
	~MapMgr(void);

	/** 初始化，进入游戏世界时调用
	*/
	void Init();
	/** 释放数据，离开游戏世界时调用
	*/
	void Destroy();

	void OpenMap(const TCHAR* szMapName,DWORD dwMapID,CameraBase* pBindCamera);
	void CloseMap();

	DWORD GetCurMapID(){ return m_curMapID;}
	const tstring& GetCurMapName() { return m_curMapName; }

	/** \远景视野
	*/
	void SetViewDist(int viewDist);
	int GetViewDist(){ return m_viewDist;}
	/** \环境细节
	*/
	void SetSceneDetailLevel(int level);
	int GetSceneDetailLevel(){ return m_sceneDetailLevel;}
	/** \太阳光晕
	*/
	void EnableSunFlare( const bool bEnable );

	void Update();
	DWORD OnGameEvent(tagGameEvent* pEvent);

	/** 同步更新可区域
	*/
	void SyncInvalidViewZone();

	SceneGraph* GetSceneGraph(){ return m_pSG;}
	GameMap& GetGameMap(){ return m_gameMap;}
	NavMap*  GetNavMap() { return &m_navMap; }

	int		GetMapAreaIndex( const Vector3& vPos, const Vector3& vSize, EMapArea eAreaType = EMA_Null );
	DWORD	GetCurTriggerID() const;
	BOOL	GetTriggerPos(DWORD questSerial,Vector3& pos,AABBox& box);
	BOOL	GetTriggerPos(DWORD triggerID, Vector3& pos);
	static MapMgr* Inst();
private:
	void SetViewZoneSize();
	void UpdateTriggers();
	void UpdateMapArea();
	void ResetSceneFog( const bool bFade );

private:
	int						m_viewDist;
	int                     m_sceneDetailLevel;
	bool                    m_bEnableSunFlare;
	HugeQuadTreeSG*			m_pSG;
	GameMap					m_gameMap;
	NavMap					m_navMap;
	DWORD					m_curMapID;
	tstring					m_curMapName;

	vector<HRGN>			m_triggerRgn;			//触发器区域
	vector<HRGN>			m_mapAreaRgn;			//地图区域
	bool					m_bInTrigger;			//玩家当前是否在触发器内
	int						m_curTriggerIndex;		//当前玩家所在触发器索引
	int						m_curMapAreaIndex;		//当前玩家所在地图区域索引

	float					m_lastUpdateMapAreaTime;//最后一次更新MapArea的时间

	TObjRef<NetSession>		m_pZoneSession;
	TObjRef<GameFrameMgr>	m_pGameFrameMgr;
	TSFPTrunk<MapMgr>		m_trunk;
};
