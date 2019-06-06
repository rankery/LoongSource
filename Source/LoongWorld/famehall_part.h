//-----------------------------------------------------------------------------
//!\file famehall_part.h
//!\author xlguo
//!
//!\date 2009-02-23
//! last 
//!
//!\brief 名人堂相关数据定义
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <map>
#include <list>
#include <vector>
#include <set>
#include "mutex.h"
#include "TimeChecker.h"
#include "../ServerDefine/famehall_define.h"
#include "../WorldDefine/famehall_define.h"

// 声望排名更新时间
const INT32		REP_ORDER_UPDATE_INTERVAL_TICKS		= 10 * 60 * TICK_PER_SECOND;	

// 进入名人堂声望限制
const INT32		ENTER_FAMEHALL_REP_LIM				= 328800;

// 进入名人堂后激活次数奖励
const INT8		ENTER_ACTCOUNT_REWARD				= 1;				


//----------------------------------------------------------------------------
// 氏族珍宝原型
//----------------------------------------------------------------------------
struct tagClanTreasureProto
{
	DWORD		dwTypeID;
	DWORD		dwNpcID;
	DWORD		dwMapID;
	ECLanType	eClanType;
	INT32		nActClanConNeed;

};

//----------------------------------------------------------------------------
// 氏族珍宝
//----------------------------------------------------------------------------
struct tagClanTreasure
{
	ETReasureState	eState;						// 珍宝状态
	union {
		const tagClanTreasureProto*		pProto;			// 激活所需氏族贡献
		DWORD							dwNamePrefixID;	// 珍宝名称前缀
	};
}; 
//----------------------------------------------------------------------------
// 氏族珍宝激活列表
//----------------------------------------------------------------------------
class ClanTreasureActList
{
// 	typedef		std::list<tagClanTreasure>			ListClanTreasure;
// 	typedef		std::vector<DWORD>					VecTreasureID;
	typedef		std::map<UINT16, tagClanTreasure*>	MapU16ClanTreasure;
	typedef		std::pair<UINT16, tagClanTreasure*>	PairU16ClanTreasure;
public:// 地图线程调用
	// 珍宝数据获得
	VOID	GetActivatedTreasure(PVOID pData);

	// 珍宝数量获得
	INT16	GetActivatedTreasureNum();


public:// 主线程调用
	~ClanTreasureActList(){Destroy();}

	// 初始化读取所有氏族珍宝结构
	BOOL	Init(ECLanType eClantype);

	// 销毁
	VOID	Destroy();

	// 尝试激活珍宝（保留到主线程）
	DWORD	TryActiveTreasure(Role* pRole, UINT16 u16TreasureID, ECLanType eclantype);

	// 读取珍宝
	VOID	HandleLoadDBTreasure(tagTreasureData* pTreasureData, const INT32 nNum);
private:
	// 可否激活
	DWORD	CanActiveTreasure(Role* pRole, UINT16 u16TreasureID, ECLanType eclantype);

	// 激活
	DWORD	ActiveTreasure(Role* pRole, UINT16 u16TreasureID, ECLanType eclantype);

	// 发给玩家激活的物品
	VOID	GiveRoleItem( DWORD dwNameID, Role* pRole, DWORD dwTypeID );

	// 设置激活（用于初始化）
	VOID	SetAct(UINT16 u16TreasureID, DWORD dwNamePrefixID);
	
	// 重置所有
	VOID	ResetAll();
private:
	MapU16ClanTreasure		m_mapAllTreasure;
	INT						m_nActNum;
};


//----------------------------------------------------------------------------
// 名人堂角色跟踪
//----------------------------------------------------------------------------
class FameRoleTracker
{
	typedef std::vector<tagFameHallEnterSnap>	VecFameMember;
	typedef std::set<DWORD>						SetMemberID;
public:
	// 记录角色
	VOID	TrackRole(DWORD dwRoleID, ECLanType eClanType);

	// 角色是否被记录快照
	BOOL	IsSnapAdded(DWORD dwRoleID, DWORD dwNameID);

	// 获得快照前nNum名
	INT32	GetTopSnap(BYTE* pData, INT32 nNum = 50);// dwNameID

	// 获得成员数量
	INT32	GetTopSnapNum(INT32 nNum = 50) const;

	// 初始化加载成员进入快照
	VOID	LoadInitFameHallEnterSnap(tagFameHallEnterSnap* pFameMember, const INT32 nNum);

	// 是否是名人
	BOOL IsFameRole(DWORD dwRoleID, ECLanType eClanType);

private:
	// 添加角色快照
	VOID	AddRoleSnap(DWORD dwRoleID, ECLanType eClanType);

	// 快照已满
	BOOL	IsSnapFull();

	// 添加成员
	VOID MarkFameRole(DWORD dwRoleID, ECLanType eClanType);
private:
	VecFameMember	m_vecAllMembers;
	SetMemberID		m_setAllMemberIDs;
};


//----------------------------------------------------------------------------
// 氏族
//----------------------------------------------------------------------------
class ClanTrunk
{
	typedef		std::vector<tagRepRankData>	VecRepRank;

public:// 地图线程调用
	// 尝试进入名人堂
	BOOL TryEnterFameHall(Role* pRole);

	// 得到名人堂排名前50
	VOID GetFameHallTop50(BYTE* pData);

	// 得到名人堂当前个数
	INT32 GetFameHallTop50Num();

	// 得到进入快照更新时间
	tagDWORDTime GetEnterSnapUpdateTime() const {	return m_dwtRepResetTime;	}

	// 得到氏族声望排名
	VOID GetRepRank(PVOID pData);

	// 得到氏族声望排名人数
	INT32 GetRepRankNum();

	// 得到声望排名更新时间
	tagDWORDTime GetRepRankUpdateTime() const {	return m_dwtRepRankUpdateTime;	}

public:// 主线程调用
	// 更新
	VOID Update()
	{
		CHECK_TIME();
		UpdateEnter();
		UpdateRepRank();
	}

	// 更新声望排名
	VOID HandleUpdateRepRank(tagRepRankData* pRepOrderData, const INT32 nNum);

	// 初始化名人堂排名
	VOID HandleInitFameHallTop50(tagFameHallEnterSnap* pMember, const INT32 nNum);

	// 初始化氏族珍宝
	VOID HandleInitActTreasureList(tagTreasureData* pTreasure, const INT32 nNum);

	// 初始化声望重置时间
	VOID HandleInitRepRstTimeStamp(tagDWORDTime dwtRstTimeStamp) { m_dwtRepResetTime = dwtRstTimeStamp;}

	BOOL Init(ECLanType eClanType, INT32 nEnterLim, INT8 n8ActCount);
	VOID Destroy(){}

	// 激活氏族珍宝
	DWORD ActiveClanTreasure(Role* pRole, UINT16 u16TreasureID);

	// 获得氏族珍宝列表
	VOID GetActTreasureList(PVOID pData);

	// 获得氏族珍宝个数
	INT32 GetActTreasureNum();
	
	ClanTrunk()	:m_pRoleEnter( NULL ){}
	~ClanTrunk(){Destroy();}

	// tbc：生成nameid
	static DWORD GetNameID(DWORD dwRoleID);

private:// 主线程
	// 更新进入名人堂
	BOOL UpdateEnter();

	// 更新声望排名
	VOID UpdateRepRank();

	// 进入名人堂
	BOOL Enter();

	// 能否进入名人堂
	BOOL CanEnter();
private:
	ClanTreasureActList		m_ClanTreasure;					// 珍宝
	
	VecRepRank				m_vecRepRank;					// 声望容器				DBSession中初始化
	tagDWORDTime			m_dwtRepRankUpdateTime;			// 上次更新时间
	INT32					m_nRepRankUpdateTickCounter;	// 声望排名更新倒数计时	
	

	FameRoleTracker			m_FameRoleTracker;				// 名人堂容器				DBSession中初始化
	Role*					m_pRoleEnter;					// 进入角色					构造函数初始化
	Mutex					m_EnterLock;					// 进入锁（地图线程）
	INT32					m_nEnterFameHallRepLim;			// 进入名人堂声望限制		Init函数初始化
	INT8					m_n8ActCountReward;				// 奖励激活珍宝次数			Init函数初始化
	tagDWORDTime			m_dwtRepResetTime;				// 上次声望重置时间		DBSession中初始化

	ECLanType				m_eClanType;					// 氏族类型					Init函数初始化
};