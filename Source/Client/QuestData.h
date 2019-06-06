/********************************************************************
	created:	2008/08/26
	created:	26:8:2008   13:55
	filename: 	d:\loong_client\Client\QuestData.h
	file path:	d:\loong_client\Client
	file base:	QuestData
	file ext:	h
	author:		leili
	
	purpose:	任务数据读取、存储，处理一些简单的查询
*********************************************************************/
#pragma once
#include "..\WorldDefine\QuestDef.h"

class QuestData
{
	friend class QuestMgr;

public:
	// 任务过滤
	enum EFilter
	{
		EF_DefFilter1,
		EF_DefFilter2,
		EF_CustomFilter1,
		EF_CustomFilter2,
		EF_NoFilter
	};
	typedef std::map<INT16, BOOL> QuestFilterConfigMap;		// 任务过滤方案
	typedef std::map<INT16, tstring> QuestCategoryNameMap;			// 任务大类别名称
	QuestData(void);
	~QuestData(void);

	VOID	Reset(void);

	BOOL	LoadQuestLocalData(LPCTSTR szPath, list<tstring>* pFileList);
	BOOL	LoadQuestData(LPCTSTR szPath, list<tstring>* pFileList);
	BOOL	LoadNPCAcceptQuestFile(LPCTSTR szFile);
	BOOL	LoadNPCCompleteQuestFile(LPCTSTR szFile);
	BOOL	LoadNPCUpdateQuestFile(LPCTSTR szFile);
	BOOL	LoadNPCScriptFile(LPCTSTR szMap);
	BOOL	LoadTriggerData(LPCTSTR szPath);
	BOOL	LoadPathfinding(LPCTSTR szFile);
	BOOL    LoadQuestCategory(LPCTSTR szFile);
	BOOL	LoadQuestType(LPCTSTR szFile);
	BOOL	LoadQuestDefaultFilterConfig(LPCTSTR szFile);
	

	const tagQuestProto* GetQuestProto(UINT16 questID)				const;
	const tagQuestDynamicTarget* GetQuestDynamicTarget(UINT16 questID)const;
	BOOL	GetQuestType(UINT16 questID,INT16& type)			const;
	BOOL	GetQuestTypeName(INT16 type, tstring& name)			const;
	BOOL	GetQuestName(UINT16 questID,tstring& name)				const;
	BOOL	GetQuestDesc(UINT16 questID,tstring& desc)				const;
	BOOL	GetQuestRewMoney(UINT16 questID,DWORD& money)			const;
	BOOL	GetQuestRewXP(UINT16 questID,DWORD& xp)					const;
	BOOL	GetQuestAutoComplete(UINT16 questID)					const;
	DWORD	GetCompleteReqNPC(UINT16 questID,int index)				const;
	BOOL	GetQuestRequiredItemCount(UINT16 questID,DWORD num,DWORD& id,UINT16& count)		const;
	BOOL	GetQuestRequiredCreatureCount(UINT16 questID,DWORD num,DWORD& id,UINT16& count)	const;
	BOOL	GetQuestItemQuality(UINT16 questID,INT index,INT16& quality)		const;
	BOOL	GetQuestChoiceItemQuality(UINT16 questID,INT index,INT16& quality)	const;
	INT		GetQuestRewItemNum(UINT16 questID)						const;
	BOOL	QuestRequireItemOrCreature(UINT16 questID)				const;
	BOOL	QuestCanDelete(UINT16 questID)							const;
	BOOL	ShowQuestAtOnce(UINT16 questID)							const;
	BOOL	QuestAddNext(UINT16 questID)							const;
	BOOL	GetNextQuest(UINT16 questID,UINT16& nextID)				const;
	BOOL	GetQuestDestPos(UINT16 questID,Vector3& pos)			const;
	BOOL	GetQuestTypeColor(UINT16 questID,DWORD& color)			const;
	DWORD	GetQuestLevelColor(UINT16 questID,INT roleLvl)			const;
	INT		GetQuestSrcItem(UINT16 questID)							const;
	INT		GetQuestLevel(UINT16 questID)							const;
	VOID	SetQuestDesc(UINT16 questID, LPCTSTR szDesc);
	VOID    SetQuestFilter(EFilter eFilter, INT16 n16Category, BOOL bSet);
	VOID	SetCurFilter(EFilter eFilter);
	VOID	SetLastFilter(EFilter eFilter);
	EFilter GetCurFilter()											const;
	EFilter GetLastFilter()											const;
	BOOL	IsFiltered(INT16 n16Category);
	INT16   GetQuestCategoryByQuestType(INT16 n16RType);
	INT16	GetQuestCategoryByQuestID(INT16 n16QuestID);
	const QuestFilterConfigMap& GetFilter(EFilter eFilter)			const;
	const QuestCategoryNameMap& GetCategoryName()					const;
	VOID    SetFilterByPlayerLevel(BOOL bSet);
	BOOL	IsFilterByPlayerLevel()									const;



	
	BOOL	IsNPCHasQuestByCurrentFilter(DWORD dwNPCID);
	BOOL	IsNPCHasQuestByCurrentFilterByPlayerLevel(DWORD dwNPCID);




	BOOL	NPCBeTalked(UINT16 questID,int index)					const;
	BOOL	BeTriggered(UINT16 questID,int index)					const;

	VOID	GetNPCAcceptQuests(DWORD npcID,vector<UINT16>& quests)	const;
	VOID	GetNPCCompleteQuests(DWORD npcID,vector<UINT16>& quests)const;
	VOID	GetNPCUpdateQuests(DWORD npcID,vector<UINT16>& quests)	const;
	BOOL	GetNPCScriptFile(DWORD npcID,tstring& script)			const;
	BOOL	GetTriggerPosition(LPCTSTR triggerName,Vector3& pos)	const;
	BOOL	GetTriggerName( DWORD id, tstring& name )				const;
	BOOL	GetPathPointMap(DWORD npcID,tstring& strMap)			const;

private:
	VOID	FindFilesInDir( LPCTSTR szDir,LPCTSTR szExt,vector<tstring>& findFiles );
	BOOL    LoadQuestLocal(LPCTSTR szFileName);
	BOOL	LoadQuestFromXml( tagQuestProto* pQuest,LPCTSTR szFileName );
	typedef map<string,string> XmlNodeMap;

	BOOL	LoadXmlNode(XmlElement* pNode, XmlNodeMap& xmlNodeMap);
	BOOL	SetQuestProto(tagQuestProto* pQuestProto, XmlNodeMap& xmlNodeMap);
	void	SetQuestDword(string str, DWORD& dwQuest,XmlNodeMap& xmlNodeMap);
	void	SetQuestDwordFlags(string str, DWORD& dwQuest, INT32 nMask, XmlNodeMap& xmlNodeMap);
	void	SetQuestUINT16(string str, UINT16& unQuest,XmlNodeMap& xmlNodeMap);
	void	SetQuestBYTE(string str, BYTE& byQuest,XmlNodeMap& xmlNodeMap);
	void	SetQuestINT16(string str, INT16& n16Quest,XmlNodeMap& xmlNodeMap);
	void	SetQuestINT32(string str, INT32& nQuest,XmlNodeMap& xmlNodeMap);
	void	SetQuestBool(string str, BOOL& bQuest,XmlNodeMap& xmlNodeMap);
	void	SetQuestFloat(string str, FLOAT& fQuest,XmlNodeMap& xmlNodeMap);

	BOOL	LoadCSV(LPCTSTR szPath);
	
private:
	typedef std::map<UINT16,tagQuestProto> QuestsMap;
	QuestsMap				m_quests;

	typedef std::multimap<DWORD,UINT16> NPCQuestMultiMap;
	NPCQuestMultiMap	m_npc2AcceptQuests;			//npc与任务的对应关系，接取任务
	NPCQuestMultiMap	m_npc2CompleteQuests;		//npc与任务的对应关系，完成任务
	NPCQuestMultiMap	m_npc2UpdateQuests;			//npc与任务的对应关系，更新任务

	typedef std::map<DWORD,tstring> NPCScriptMap;
	NPCScriptMap		m_npc2Script;				//npc与脚本的对应关系

	struct tagQuestState	// 任务的完成状况 
	{
		BOOL beTalked[QUEST_NPC_COUNT];
		BOOL beTriggered[QUEST_TRIGGERS_COUNT];

		tagQuestState(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};
	typedef std::map<UINT16, tagQuestState> QuestStateMap;
	QuestStateMap	m_questStates;					// 任务与任务状况的对应关系

	typedef std::map<UINT16, tagQuestDynamicTarget> QuestDynamicTargetMap;
	QuestDynamicTargetMap	m_questDynamicTarget;

	struct tagTriggerProto
	{
		tstring name;
		tstring map;
		DWORD	id;
		Vector3	pos;
	};

	typedef std::vector<tagTriggerProto> TriggerArray;
	TriggerArray		m_triggers;

	typedef std::multimap<DWORD,tstring> PathPointMultiMap;
	PathPointMultiMap	m_pathpoint2Map;			// 任务寻路点与地图的对应关系
	typedef std::map<INT16, pair<tstring, DWORD>> QuestTypeMap;
	QuestTypeMap		m_questtype;
	
	
	typedef std::multimap<INT16, INT16> QuestCategoryMap;		// 任务大类别和任务类别的对应关系
	QuestCategoryMap    m_questcategory;

	typedef std::map<INT16, INT16> QuestTypeToCategoryMap;		// 任务类别和大类别的对应关系
	QuestTypeToCategoryMap m_questtype2category;

	
	QuestCategoryNameMap m_questcategoryname;

	
	QuestFilterConfigMap m_deffilter1;
	QuestFilterConfigMap m_deffilter2;
	QuestFilterConfigMap m_customfilter1;
	QuestFilterConfigMap m_customfilter2;
	QuestFilterConfigMap m_nofilter;

	EFilter m_eCurFilter;
	EFilter m_eLastFilter;		// 上一次的

	BOOL m_bFilterByLevel;
};


