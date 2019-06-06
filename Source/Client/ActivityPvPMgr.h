#pragma once

//英雄乱武
struct tagPvPHero
{
	DWORD		dwRank;					//排行
	DWORD		dwRoleID;				//玩家ID
	tstring		strRoleName;			//玩家姓名
	INT			nScore;				//玩家积分
};

//群雄逐鹿
struct tagPvPMultiHero
{
	DWORD		dwRank;					//排行
	DWORD		dwTeamID;				//队伍编号
	DWORD		dwTeamLeaderID;			//队长ID
	tstring		strTeamLeaderName;		//队长姓名
	INT			nTeamScore;			//队伍积分
};

struct tagNS_MsgBlob;
struct tagRoleGetNameEvent;

class ActivityPvPMgr 
{
public:
	ActivityPvPMgr(void);
	~ActivityPvPMgr(void);

	static ActivityPvPMgr* Inst();

	void Init();
	void Destroy();
	
	//清空积分排行信息
	void ClearScoreInfo();

	//活动名
	void SetActivityName(const TCHAR* name, DWORD id);
	//剩余时间
	void SetActivityCD(DWORD dwCD);
	//当前积分
	void SetActivityScore(DWORD dwScore);
	//当前剩余复活次数
	void SetActivityRebornTime(DWORD dwRevive);
	//当前队伍ID
	void SetActivityTeamID(DWORD dwTeamID);
	//当前活动的类型(客户端专用)
	void SetActivityType(DWORD dwActivityType) { m_dwActivityType = dwActivityType; }

	//--获取活动相关信息
	tstring& GetActivityName()		{ return m_strActivityName; }
	DWORD GetActivityID()			{ return m_dwActivityID; }
	DWORD GetActivityScore()		{ return m_dwScore; }
	DWORD GetActivityRebornTime()	{ return m_dwRevive; }
	DWORD GetActivityTeamID()		{ return m_dwTeamID; }
	DWORD GetActivityType()			{ return m_dwActivityType; }
	DWORD GetPvPType();


	//--设置积分排行
	void SetPvPActivityScoreSort( DWORD dwRank, DWORD dwRoleID, DWORD dwScore );
	void SetPvPMultiActivityScoreSort( DWORD dwRank, DWORD dwRoleID, DWORD dwScore, DWORD dwTeamID );

	//--获取积分排行信息
	vector<tagPvPHero>& GetPvPActivityScoreSort()			{ return m_vecPvPHero; }
	vector<tagPvPMultiHero>& GetPvPMultiActivityScoreSort() { return m_vecPvPMhero; }

	void RefreshScoreSort();

	void ClearPvPUI();

private:
	//游戏事件
	DWORD OnRoleGetNameEvent(tagRoleGetNameEvent* pGameEvent);

private:
	TSFPTrunk<ActivityPvPMgr>		m_Trunk;
	TObjRef<NetCmdMgr>				m_pNetCmdMgr;
	TObjRef<GameFrameMgr>			m_pGameFrameMgr;
	TObjRef<NetSession>				m_pSession;

	vector<tagPvPHero>				m_vecPvPHero;
	vector<tagPvPMultiHero>			m_vecPvPMhero;

	vector<tagPvPHero>				m_vecPvPHeroTmp;			//没有姓名的缓存表
	vector<tagPvPMultiHero>			m_vecPvPMheroTmp;			//同上

	DWORD							m_dwActivityType;			//活动类型(客户端用)
	DWORD							m_dwActivityID;				//活动ID;
	tstring							m_strActivityName;			//活动名称
	DWORD							m_dwScore;					//活动积分(个人和队伍)
	DWORD							m_dwRevive;					//复活剩余次数
	DWORD							m_dwTeamID;					//队伍编号

};