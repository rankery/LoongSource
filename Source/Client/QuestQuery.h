/********************************************************************
	created:	2008/08/26
	created:	26:8:2008   15:04
	filename: 	d:\loong_client\Client\QuestQuery.h
	file path:	d:\loong_client\Client
	file base:	QuestQuery
	file ext:	h
	author:		leili
	
	purpose:	进行任务查询，可交接任务的NPC列表，已完成任务列表，玩家当前任务列表
*********************************************************************/
#pragma once
#include "ServerTime.h"

class QuestQuery
{
	friend class QuestMgr;

public:
	QuestQuery(void);
	~QuestQuery(void);

	VOID	Reset(void);

	struct tagQuestTrack 
	{
		bool	isTracked;
		float	lastUpdateTime;
		INT		itemCount[QUEST_ITEMS_COUNT];
		INT		creatureCount[QUEST_CREATURES_COUNT];
		bool	invesObj[QUEST_ITEMS_COUNT];

		tagQuestTrack(void)
		{
			memset(this,0,sizeof(tagQuestTrack));
		}
	};

	struct tagActiveQuest
	{
		EWeek	week;			// 星期几
		UINT16	questID;		// 任务ID
		tstring	questName;		// 任务名称
		INT32	minLevel;		// 最小等级
		INT32	maxLevel;		// 最大等级
		DWORD	acceptNPCID;	// 接取NPCID
		tstring	acceptNPCName;	// 接取NPC名称
		INT		complete;		// 已完成次数
		INT		all;			// 可完成次数
	};

	enum ESortType
	{
		EST_None	= 0,		// 不排序
		EST_Time	= 1,		// 按时间排序
		EST_Quest,				// 按任务名称排序
		EST_Level,				// 按等级限制排序
		EST_AcceptNPC,			// 按接取人排序
		EST_Complete			// 按完成次数排序
	};

	struct tagMapNPCInfo
	{
		DWORD	dwTypeID;		// 属性ID
		vector<Vector3>	vPos;	// 初始坐标
	};

	typedef std::set<DWORD> NPCSet;
	typedef std::map<UINT16,tagQuestTrack> QuestsMap;
	typedef std::pair<UINT16,tagQuestTrack> Quest_Pair;
	typedef std::map<DWORD,tagMapNPCInfo> MapNPCMap;
	typedef std::pair<DWORD,tagMapNPCInfo> MapNPC_Pair;
	typedef std::vector<tagActiveQuest> ActiveQuestArray;

	VOID			BuildMapQuestNPCs(void);
	VOID			BuildDynamicQuestNPCs(list<tstring>& strNPCs);
	VOID			BuildAcceptQuestNPCs(void);
	VOID			BuildCompleteQuestNPCs(void);
	VOID			BuildUpdateQuestNPCs(void);
	VOID			AddInvesObject(UINT16 questID);
	VOID			DeleteInvesObject(UINT16 questID);
	VOID			DeleteInvesObject( UINT16 questID,INT index );
	VOID			UpdateInvesObject(UINT16 questID);
	VOID			AddTriggerEffect(UINT16 questID);
	VOID			DeleteTriggerEffect(UINT16 questID);
	VOID			UpdateTriggerEffect(UINT16 questID);
	VOID			UpdateTriggerEffect(void);

	BOOL			IsInvesObject(DWORD typeID);

	const NPCSet&	GetAcceptQuestNPCs(void)		const				{ return m_acceptQuestNPCs; }
	const NPCSet&	GetCompleteQuestNPCs(void)		const				{ return m_completeQuestNPCs; }
	const NPCSet&	GetUpdateQuestNPCs(void)		const				{ return m_updateQuestNPCs; }
	const QuestsMap&GetCurrentQuests(void)			const				{ return m_currentQuests; }
	VOID			GetNPCAcceptQuests(DWORD npcID,vector<UINT16>& quests)	const;
	VOID			GetNPCCompleteQuests(DWORD npcID,vector<UINT16>& quests)const;
	VOID			GetNPCUpdateQuests(DWORD npcID,vector<UINT16>& quests)	const;
	BOOL			QuestCanAccept(UINT16 questID)	const;
	BOOL			QuestCanComplete(UINT16 questID)	const;
	BOOL			NPCCanUpdateQuest(DWORD npcID,UINT16 questID)	const;
	BOOL			QuestCanTrack(UINT16 questID)	const;
	BOOL			QuestIsTracked(UINT16 questID)	const;
	BOOL			GetQuestItemCount(UINT16 questID,DWORD num,UINT16& count)		const;
	BOOL			GetQuestCreatureCount(UINT16 questID,DWORD num,UINT16& count)	const;
	BOOL			ObjBeInves( UINT16 questID,DWORD num ) const;
	BOOL			SetQuestItemCount(UINT16 questID,DWORD index,UINT16 count);
	BOOL			SetQuestCreatureCount(UINT16 questID,DWORD index,UINT16 count);
	BOOL			SetQuestInvesObj(UINT16 questID,INT index);
	BOOL			UpdateQuestState(UINT16 questID);
	BOOL			GetNPCPosition(DWORD npcID,Vector3& pos,bool bRandom=true)		const;
	BOOL			GetNPCPosition(DWORD npcID,vector<Vector3>& posArray)			const;
	BOOL			IsAcceptNPC(DWORD npcID)		const;
	BOOL			IsCompleteNPC(DWORD npcID)		const;
	BOOL			IsUpdateNPC(DWORD npcID)		const;

	BOOL			QuestIsCompleted(UINT16 questID)	const;
	BOOL			QuestIsAccepted(UINT16 questID)		const;

	VOID			SortActiveQuest(ESortType eSort);
	const ActiveQuestArray& GetActiveQuestArray(void)	const	{ return m_activeQuests; }
	UINT16			GetActiveQuestID(DWORD index)		const;
	DWORD			GetActiveQuestNPCID(DWORD index)	const;

private:
	NPCSet				m_acceptQuestNPCs;			// 可接任务的npc列表
	NPCSet				m_dynAcceptQuestNPCs;		// 动态可接任务的npc列表
	NPCSet				m_completeQuestNPCs;		// 可交任务的npc列表
	NPCSet				m_updateQuestNPCs;			// 可更新任务的npc列表
	QuestsMap			m_currentQuests;			// 玩家当前任务列表
	MapNPCMap			m_mapNPCMap;				// 当前地图的NPC列表
	NPCSet				m_dynamicNPCs;				// 动态NPC列表
	ActiveQuestArray	m_activeQuests;				// 固定活动任务列表

	typedef std::map<DWORD,INT> InvesObjMap;
	InvesObjMap			m_invesObjects;				// 可调查地物列表

	struct tagTriggerEffect 
	{
		INT		refNum;
		DWORD	effectID;
	};
	typedef std::map<DWORD,tagTriggerEffect> TriggerEffectMap;
	TriggerEffectMap	m_triggers;					// 任务触发器列表

	typedef std::map<UINT16,INT> DoneQuestsMap;
	DoneQuestsMap		m_doneQuests;				// 已经完成的所有任务，后面为完成次数

	typedef std::map<UINT16,tagDWORDTime> DoneTimeMap;
	DoneTimeMap			m_doneTimes;				// 完成任务的时间戳
};