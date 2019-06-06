//------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: quest_mgr.h
// author: lh
// actor:
// data: 2008-7-24
// last:
// brief: 任务管理器
// remark: 每个角色身上一个任务管理器，来管理自身任务的加载，更新的操作
//------------------------------------------------------------------------------------------------------
#pragma once

struct tagQuestProto;
class Creature;

class QuestMgr
{
public:
	typedef TMap<UINT16, tagQuestProto*>	QuestMap;
	typedef TMap<DWORD, TList<UINT16>*>		QuestRelationMap;
	typedef TMap<UINT16, TList<DWORD>*>		NPCRelationMap;
	typedef map<string, string>				XmlNodeMap;

public:
	//--------------------------------------------------------------------
	// 初始化
	//--------------------------------------------------------------------
	BOOL Init();

	//--------------------------------------------------------------------
	// 销毁
	//--------------------------------------------------------------------
	VOID Destroy();

	//--------------------------------------------------------------------
	// 从指定目录读取任务结构
	//--------------------------------------------------------------------
	BOOL LoadQuest(LPCTSTR szPath);

	//--------------------------------------------------------------------
	// 是否可以从该NPC处得到这个任务
	//--------------------------------------------------------------------
	BOOL IsNPCAcceptQuest(Creature* pCreature, UINT16 u16QuestID);

	//--------------------------------------------------------------------
	// 是否可以从该NPC处交这个任务
	//--------------------------------------------------------------------
	BOOL IsNPCCompleteQuest(Creature* pCreature, UINT16 u16QuestID);

	//---------------------------------------------------------------------
	// 某个任务是否要通过NPC才能交
	//--------------------------------------------------------------------
	BOOL IsNeedNPCCompleteQuest(UINT16 u16QuestID);

	//---------------------------------------------------------------------
	// 某个任务是否要通过NPC才能接
	//--------------------------------------------------------------------
	BOOL IsNeedNPCAcceptQuest(UINT16 u16QuestID);

	//---------------------------------------------------------------------
	// 各种Get
	//---------------------------------------------------------------------
	const tagQuestProto* GetQuestProto(UINT16 u16QuestID) { return m_mapAllQuests.Peek(u16QuestID); }

private:
	DWORD	LoadQuestFromXml(tagQuestProto* pQuest, LPCTSTR szFileName);
	VOID	FindQuestFileInDir(LPCTSTR szPath, vector<tstring>& vecQuestXml);

	BOOL	LoadAcceptQuestNPCRelation(LPCTSTR szFileName);
	BOOL	LoadCompleteQuestNPCRelation(LPCTSTR szFileName);

	BOOL	LoadXmlNode(XmlElement* pNode, XmlNodeMap& xmlNodeMap);
	BOOL	SetQuestProto(tagQuestProto* pQuestProto, XmlNodeMap& xmlNodeMap);
	VOID	SetQuestDword(string str, DWORD& dwQuest,XmlNodeMap& xmlNodeMap);
	VOID	SetQuestDwordFlags(string str, DWORD& dwQuest, INT32 nMask, XmlNodeMap& xmlNodeMap);
	VOID	SetQuestUINT16(string str, UINT16& unQuest,XmlNodeMap& xmlNodeMap);
	VOID	SetQuestBYTE(string str, BYTE& byQuest,XmlNodeMap& xmlNodeMap);
	VOID	SetQuestINT16(string str, INT16& n16Quest,XmlNodeMap& xmlNodeMap);
	VOID	SetQuestINT32(string str, INT32& nQuest,XmlNodeMap& xmlNodeMap);
	VOID	SetQuestBool(string str, BOOL& bQuest,XmlNodeMap& xmlNodeMap);
	VOID	SetQuestFloat(string str, FLOAT& fQuest,XmlNodeMap& xmlNodeMap);	

private:
	TObjRef<VarContainer>	m_pVar;

	QuestMap			m_mapAllQuests;					// 所有任务集合
	QuestRelationMap	m_mapAcceptQuestNPCRelation;	// NPC与可接任务对应表
	QuestRelationMap	m_mapCompleteQuestNPCRelation;	// NPC与可交任务对应表
	NPCRelationMap		m_mapCompleteNPCRelation;		// 可交任务与NPC对应表
	NPCRelationMap		m_mapAcceptNPCRelation;			// 可接任务与NPC对应表

	tstring				m_strQuestPath;					// 任务文件存放路径
	tstring				m_strQuestFileList;				// 任务文件列表文件
	tstring				m_strAcceptQuestNPCRelation;	// 任务与接取NPC关系文件
	tstring				m_strCompleteQuestNPCRelation;	// 任务与完成NPC关系文件
};

extern QuestMgr g_questMgr;