/////////////////////////////////////////////////////////////////////////////
// Program:     loong任务编辑器
// Name:        QuestMgr.cpp
// Created:     2008-11-12
/////////////////////////////////////////////////////////////////////////////
#include "wx/wx.h"
#include "QuestMgr.h"
#include <sstream>
#include "..\WorldDefine\Questdef.h"


// ----------------------------------------------------------------------------
//构造函数
// ----------------------------------------------------------------------------
QuestMgr::QuestMgr() :m_pQuestTmp(NULL)
{

}

// ----------------------------------------------------------------------------
//析构函数
// ----------------------------------------------------------------------------
QuestMgr::~QuestMgr()
{
	//删除临时Quest
	if(m_pQuestTmp)
		delete m_pQuestTmp;

	//删除quest
	QuestMap::iterator it = m_mapQuest.begin();
	for (; it != m_mapQuest.end();)
	{
		delete it->second->pQuestProto;
		delete it->second;
		it = m_mapQuest.erase(it);
	}
}

// ----------------------------------------------------------------------------
// 加载任务文件
// ----------------------------------------------------------------------------
BOOL QuestMgr::LoadQuest(LPCTSTR pPath)
{
	if(pPath == NULL)
		return FALSE;

	TCHAR szType[512];
	_stprintf(szType,_T("%s\\RType.xml"),pPath);
	LoadQuestType(szType);

	//遍历指定目录下的所有任务文件，加入列表中
	FindFile(pPath, m_listQuestFile, m_listQuestLocalFile);
	
	//遍历任务文件集合
	list<wstring>::iterator it = m_listQuestFile.begin();
	for (; it != m_listQuestFile.end(); ++it)
	{
		LoadQuestFile((*it).c_str());
	}

	//遍历任务文件集合
	it = m_listQuestLocalFile.begin();
	for (; it != m_listQuestLocalFile.end(); ++it)
	{
		LoadQuestLocal((*it).c_str());
	}

	return TRUE;
}

// ----------------------------------------------------------------------------
// 保存任务文件
// ----------------------------------------------------------------------------
BOOL QuestMgr::SaveQuest(UINT16 id)
{
	QuestMap::iterator pos = m_mapQuest.find(id);
	if (pos == m_mapQuest.end())
		return FALSE;

	tagQuestProto *pQuestProto = pos->second->pQuestProto;
	wstring file = pos->second->file_path;

	if (file.empty() || pQuestProto == NULL)
		return FALSE;

	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "UTF-8", "" );
	doc.LinkEndChild( decl );

	TiXmlElement* root = new TiXmlElement( "quest" );
	doc.LinkEndChild( root );

	//block quest property
	{
		TiXmlElement* propertyNode = new TiXmlElement( "property" );
		root->LinkEndChild( propertyNode );

		//任务ID
		SaveEntry( propertyNode, "ID", pQuestProto->id );
		//前一个任务1ID
		SaveEntry( propertyNode, "PrevQuestID", pQuestProto->prev_quest_id );
		//前一个任务2ID
		SaveEntry( propertyNode, "PrevQuest2ID", pQuestProto->prev_quest2_id );
		//前序任务之间关系
		SaveEntry( propertyNode, "PrevQuestRelation", pQuestProto->prev_quest_relation );
		//后续任务ID
		SaveEntry( propertyNode, "NextQuestID", pQuestProto->next_quest_id );
		//后续任务ID
		SaveEntry( propertyNode, "UncmpleteQuestID", pQuestProto->uncomplete_quest_id );
		//当前任务完成后，后续任务是否自动加载到玩家身上
		SaveEntry( propertyNode, "AutoAddQuest", pQuestProto->auto_add_quest );
		//接取任务后是否弹出任务界面
		SaveEntry( propertyNode, "ShowQuestDialog", pQuestProto->show_quest_dialog );
		//任务不能被删除
		SaveEntry( propertyNode, "NoDelete", pQuestProto->no_delete );
		//任务接取类型--玩家主动接取
		SaveEntry( propertyNode, "QuestPlayerAccept", pQuestProto->accept_flags & Quest_Accept_Flag_Player );
		//任务接取类型--系统发布
		SaveEntry( propertyNode, "QuestSystemRelease", pQuestProto->accept_flags & Quest_Accept_Flag_System );
		//任务接取类型--从物品接取
		SaveEntry( propertyNode, "QuestItemAccept", pQuestProto->accept_flags & Quest_Accept_Flag_Item );
		//任务类型
		SaveEntry( propertyNode, "QuestType", pQuestProto->type );
		//任务目标类型
		SaveEntry( propertyNode, "TargetMode", pQuestProto->target_mode );
		//建议玩家个数
		SaveEntry( propertyNode, "SuggestedPlayers", pQuestProto->suggested_players );
		//任务完成的限制时间
		SaveEntry( propertyNode, "LimitTime", pQuestProto->limit_time );
		//每个玩家接取任务次数
		SaveEntry( propertyNode, "DoQuestTimes", pQuestProto->accept_times );
		//是否是周期性任
		SaveEntry( propertyNode, "Periodic", pQuestProto->period );
		//任务周期类别（每天 每月）
		SaveEntry( propertyNode, "QuestPeriodic", pQuestProto->period_type );
		//任务星期
		SaveEntry( propertyNode, "Week", pQuestProto->week );
		//任务是否可以重复
		SaveEntry( propertyNode, "Repeatable", pQuestProto->repeatable );
		
		//任务接取NPCID
		SaveEntry( propertyNode, "AcceptQuestNPCID1", pQuestProto->accept_quest_npc[0] );
		SaveEntry( propertyNode, "AcceptQuestNPCID2", pQuestProto->accept_quest_npc[1] );
		SaveEntry( propertyNode, "AcceptQuestNPCID3", pQuestProto->accept_quest_npc[2] );
		SaveEntry( propertyNode, "AcceptQuestNPCID4", pQuestProto->accept_quest_npc[3] );
		SaveEntry( propertyNode, "AcceptQuestNPCID5", pQuestProto->accept_quest_npc[4] );
		SaveEntry( propertyNode, "AcceptQuestNPCID6", pQuestProto->accept_quest_npc[5] );

		//任务完成NPCID
		SaveEntry( propertyNode, "CompleteQuestNPCID1", pQuestProto->complete_quest_npc[0] );
		SaveEntry( propertyNode, "CompleteQuestNPCID2", pQuestProto->complete_quest_npc[1] );
		SaveEntry( propertyNode, "CompleteQuestNPCID3", pQuestProto->complete_quest_npc[2] );
		SaveEntry( propertyNode, "CompleteQuestNPCID4", pQuestProto->complete_quest_npc[3] );
		SaveEntry( propertyNode, "CompleteQuestNPCID5", pQuestProto->complete_quest_npc[4] );
		SaveEntry( propertyNode, "CompleteQuestNPCID6", pQuestProto->complete_quest_npc[5] );
	
		//可调查地物ID
		SaveEntry( propertyNode, "InvestigateObjectID1", pQuestProto->investigate_objects[0] );
		SaveEntry( propertyNode, "InvestigateObjectID2", pQuestProto->investigate_objects[1] );
		SaveEntry( propertyNode, "InvestigateObjectID3", pQuestProto->investigate_objects[2] );
		SaveEntry( propertyNode, "InvestigateObjectID4", pQuestProto->investigate_objects[3] );
		SaveEntry( propertyNode, "InvestigateObjectID5", pQuestProto->investigate_objects[4] );
		SaveEntry( propertyNode, "InvestigateObjectID6", pQuestProto->investigate_objects[5] );
		SaveEntry( propertyNode, "InvestigateObjectID7", pQuestProto->investigate_objects[6] );
		SaveEntry( propertyNode, "InvestigateObjectID8", pQuestProto->investigate_objects[7] );

		//任务物品ID，数量
		SaveEntry( propertyNode, "SrcItemID", pQuestProto->src_item, pQuestProto->src_item_num );

		//任务目的地坐标
		//将float 转换成字符串
		wstringstream ss;
		wstring wstr;
		ss << pQuestProto->destination_x;
		SaveEntry( propertyNode, "Destination_X", ss.str().c_str() );

		ss.str(_T(""));
		ss << pQuestProto->destination_y;
		SaveEntry( propertyNode, "Destination_Y", ss.str().c_str() );
		
		ss.str(_T(""));
		ss << pQuestProto->destination_z;
		SaveEntry( propertyNode, "Destination_Z", ss.str().c_str() );
	}

	//block quest accept-conditions
	{
		TiXmlElement* acceptNode = new TiXmlElement( "accept-conditions" );
		root->LinkEndChild( acceptNode );

		//接取任务所需要的最大等级
		SaveEntry( acceptNode, "GetQuestRequiredMaxLevel", pQuestProto->accept_req_max_level );
		//接取任务所需要的最小等级
		SaveEntry( acceptNode, "GetQuestRequiredMinLevel", pQuestProto->accept_req_min_level );
		//接取任务所需要的声望,最小值,最大值
		for ( int i = 0; i < QUEST_REPUTATIONS_COUNT; ++i )
		{
			stringstream ss;
			string str;
			ss << "GetQuestRequiredRep" << i + 1 << endl;
			ss >> str;
			//SaveEntry( acceptNode, str.c_str(), pQuest->dwGetQuestRequiredRep[i] );
			//SaveEntry( acceptNode, "GetQuestRequiredMinRepValue", pQuest->dwGetQuestRequiredMinRepValue[i] );
			//SaveEntry( acceptNode, "GetQuestRequiredMaxRepValue", pQuest->dwGetQuestRequiredMaxRepValue[i] );
			SaveMaxEntry( acceptNode, str.c_str(), pQuestProto->accept_req_rep[i], pQuestProto->accept_req_max_rep[i], 
				pQuestProto->accept_req_min_rep[i] );
		}
		//接取任务所需要的性别
		SaveEntry( acceptNode, "GetQuestRequiredSex", pQuestProto->sex );
		//接取任务所需要的职业	
		SaveEntry( acceptNode, "GetQuestRequiredClass", pQuestProto->class_type );
		//接取任务所需要的地图
		SaveEntry( acceptNode, "GetQuestRequiredMapID", pQuestProto->accept_req_map );
		//接取任务所需要的技能,技能值
		for ( int i = 0; i < QUEST_SKILLS_COUNT; ++i )
		{
			stringstream ss;
			string str;
			ss << "GetQuestRequiredSkill" << i + 1 << endl;
			ss >> str;
			//SaveEntry( acceptNode, str.c_str(), pQuest->dwGetQuestRequiredSkill[i] );
			//SaveEntry( acceptNode, "GetQuestRequiredSkillValue", pQuest->dwGetQuestRequiredSkillValue[i] );
			SaveEntry( acceptNode, str.c_str(), pQuestProto->accept_req_skill[i], pQuestProto->accept_req_skill_val[i] );
		}
		//接取任务所需要的物品ID,数量
		for ( int i = 0; i < QUEST_ITEMS_COUNT; ++i )
		{
			stringstream ss;
			string str;
			ss << "GetQuestRequiredItemID" << i + 1 << endl;
			ss >> str;
			//SaveEntry( acceptNode, str.c_str(), pQuest->dwGetQuestRequiredItemID[i] );
			//SaveEntry( acceptNode, "GetQuestRequiredItemCount", pQuest->dwGetQuestRequiredItemCount[i] );
			SaveEntry( acceptNode, str.c_str(), pQuestProto->accept_req_item[i], pQuestProto->accept_req_item_num[i] );
		}
		//限制物品是否删除
		SaveEntry( acceptNode, "DelRestrictedItem", pQuestProto->del_req_item );

		//接取任务所需的人物属性,值
		for ( int i = 0; i < QUEST_ATTS_COUNT; ++i )
		{
			stringstream ss;
			string str;
			ss << "GetQuestRequiredAtt" << i + 1 << endl;
			ss >> str;	
			SaveEntry( acceptNode, str.c_str(), pQuestProto->accept_req_att[i], pQuestProto->accept_req_att_val[i] );
		}

		//接取任务所需触发器
		for ( int i = 0; i < QUEST_TRIGGERS_COUNT; ++i )
		{
			stringstream ss;
			string str;
			ss << "GetQuestReqTriggersID" << i + 1 << endl;
			ss >> str;	
			SaveEntry( acceptNode, str.c_str(), pQuestProto->accept_req_trriger[i] );
		}

		//是否是夫妻
		SaveEntry( acceptNode, "GetQuestReqMarried", pQuestProto->married );
		//情侣个数
		SaveEntry( acceptNode, "GetQuestReqLoverNum", pQuestProto->lover_num );
		//金钱数量
		SaveEntry( acceptNode, "GetQuestReqMoneyNum", pQuestProto->accept_req_money );
		//是否删除金钱
		SaveEntry( acceptNode, "GetQuestDelMoneyNum", pQuestProto->accept_del_money );
	}

	//block quest complete-conditions
	{
		TiXmlElement* completeNode = new TiXmlElement( "complete-conditions" );
		root->LinkEndChild( completeNode );

		//完成任务所需要的物品ID,数量
		for ( int i = 0; i < QUEST_ITEMS_COUNT; ++i )
		{
			stringstream ss;
			string str;
			ss << "CompleteQuestRequiredItemID" << i + 1 << endl;
			ss >> str;
			//SaveEntry( completeNode, str.c_str(), pQuest->dwCompleteQuestRequiredItemID[i] );
			//SaveEntry( completeNode, "CompleteQuestRequiredItemIDCount", pQuest->dwCompleteQuestRequiredItemIDCount[i] );
			SaveEntry( completeNode, str.c_str(), pQuestProto->complete_req_item[i], pQuestProto->complete_req_item_num[i] );
		}
		//是否搜集完其中一种就可完成
		SaveEntry( completeNode, "RequiredOnlyOneItem", pQuestProto->only_one_item );
		//完成任务后是否删除物品
		SaveEntry( completeNode, "DeleteItem", pQuestProto->delete_item );

		//完成任务所需要杀的怪,数量
		for ( int i = 0; i < QUEST_CREATURES_COUNT; ++i )
		{
			stringstream ss;
			string str;
			ss << "CompleteQuestRequiredCreatureID" << i + 1 << endl;
			ss >> str;
			//SaveEntry( completeNode, str.c_str(), pQuest->dwCompleteQuestRequiredCreatureID[i] );
			//SaveEntry( completeNode, "CompleteQuestRequiredCreatureCount", pQuest->dwCompleteQuestRequiredCreatureCount[i] );
			SaveCreatureEntry( completeNode, str.c_str(), pQuestProto->complete_req_creature[i*3], 
					pQuestProto->complete_req_creature[i*3+1], pQuestProto->complete_req_creature[i*3+2],
					pQuestProto->complete_req_creature_num[i] );
		}
		//是否击杀完指定数量的同一等级怪物就可完成
		SaveEntry( completeNode, "RequiredCreatureLevel", pQuestProto->creature_level );
		//是否击杀完指定数量的一种怪物就可完成
		SaveEntry( completeNode, "RequiredOnlyOneCreature", pQuestProto->only_one_creature );
		//完成任务所需要对话的NPC数量
		for ( int i = 0; i < QUEST_NPC_COUNT; ++i )
		{
			stringstream ss;
			string str;
			ss << "CompleteQuestRequiredNPC" << i + 1 << endl;
			ss >> str;
			SaveEntry( completeNode, str.c_str(), pQuestProto->complete_req_npc[i] );
		}
		//是否必须按顺序对话
		SaveEntry( completeNode, "RequiredOnlyInOrder", pQuestProto->only_in_order );
		//是否只和一个NPC对话后就可完成
		SaveEntry( completeNode, "RequiredOnlyOneNPC", pQuestProto->only_one_npc );
		//完成任务所需要的触发器ID
		for ( int i = 0; i < QUEST_TRIGGERS_COUNT; ++i )
		{
			stringstream ss;
			string str;
			ss << "CompleteQuestReqTriggersID" << i + 1 << endl;
			ss >> str;
			SaveEntry( completeNode, str.c_str(), pQuestProto->complete_req_trigger[i] );
		}
		//是否只触发一个就可完成
		SaveEntry( completeNode, "RequiredOnlyOneTrigger", pQuestProto->only_one_trigger );
		//完成任务所需要的金钱
		SaveEntry( completeNode, "CompleteQuestRequiredMoney", pQuestProto->complete_req_money );
		//完成任务是否删除所需要的金钱
		SaveEntry( completeNode, "CompleteQuestDelMoney", pQuestProto->complete_del_money );
		//完成任务所需要的地图ID
		SaveEntry( completeNode, "CompleteQuestRequiredMapID", pQuestProto->complete_req_map );
		//完成任务所需要的地图X坐标
		//将float 转换成字符串
		wstringstream ss;
		wstring wstr;
		ss << pQuestProto->complete_req_map_x << endl;
		ss >> wstr;
		SaveEntry( completeNode, "CompleteQuestRequiredMap_X", wstr.c_str() );
		//完成任务所需要的地图Y坐标
		ss.clear();
		ss << pQuestProto->complete_req_map_y << endl;
		ss >> wstr;
		SaveEntry( completeNode, "CompleteQuestRequiredMap_Y", wstr.c_str() );
		//完成任务所需要的地图Z坐标
		ss.clear();
		ss << pQuestProto->complete_req_map_z << endl;
		ss >> wstr;
		SaveEntry( completeNode, "CompleteQuestRequiredMap_Z", wstr.c_str() );
		//完成任务所需要的地图半径
		ss.clear();
		ss << pQuestProto->complete_req_map_radius << endl;
		ss >> wstr;
		SaveEntry( completeNode, "CompleteQuestRequiredMap_Radius", wstr.c_str() );
		//这个坐标是否随机生成
		SaveEntry( completeNode, "CompleteQuestRequiredMap_Random", pQuestProto->coordinate_random );
		//事件
		SaveEntry( completeNode, "CompleteQuestEventType", pQuestProto->event_type );
		SaveEntry( completeNode, "UseItemEvent_Item_ID", pQuestProto->use_item );
		SaveEntry( completeNode, "UseSkillEvent_Skill_ID", pQuestProto->use_skill );

		//是否以上所有条件完成一项即可完成整个任务
		SaveEntry( completeNode, "RequiredOnlyOneCondition", pQuestProto->only_one_condition );

		//是否是夫妻
		SaveEntry( completeNode, "CompleteQuestReqMarried", pQuestProto->complete_req_married );

		//情侣个数
		SaveEntry( completeNode, "CompleteQuestReqLoverNum", pQuestProto->complete_req_lover_num );

		//等级
		SaveEntry( completeNode, "CompleteQuestReqLevel", pQuestProto->complete_req_level );
	}

	//block quest reward
	{
		TiXmlElement* rewardNode = new TiXmlElement( "reward" );
		root->LinkEndChild( rewardNode );

		//任务完成后，奖励的金钱
		SaveEntry( rewardNode, "RewMoney", pQuestProto->rew_money );
		//任务完成后，奖励的经验值
		SaveEntry( rewardNode, "RewXP", pQuestProto->rew_xp );
		//任务奖励的声望,声望值
		for ( int i = 0; i < QUEST_REPUTATIONS_COUNT; ++i )
		{
			stringstream ss;
			string str;
			ss << "RewRequiredRep" << i + 1 << endl;
			ss >> str;
			//SaveEntry( rewardNode, str.c_str(), pQuest->dwRewRequiredRep[i] );
			//SaveEntry( rewardNode, "RewRequiredRepValue", pQuest->dwRewRequiredRepValue[i] );
			SaveEntry( rewardNode, str.c_str(), pQuestProto->rew_rep[i], pQuestProto->rew_rep_val[i] );
		}
		//任务奖励的贡献,贡献值
		for ( int i = 0; i < QUEST_CONTRIBUTION_COUNT; ++i )
		{
			stringstream ss;
			string str;
			ss << "RewRequiredContribution" << i + 1 << endl;
			ss >> str;
			SaveEntry( rewardNode, str.c_str(), pQuestProto->rew_contribution[i], pQuestProto->rew_contribution_val[i] );
		}
		//任务奖励的物品ID,数量
		for ( int i = 0; i < QUEST_REW_ITEM; ++i )
		{
			stringstream ss;
			string str;
			ss << "RewItemID" << i + 1 << endl;
			ss >> str;
			//SaveEntry( rewardNode, str.c_str(), pQuest->dwRewItemID[i] );
			//SaveEntry( rewardNode, "RewItemCount", pQuest->dwRewItemCount[i] );
			SaveItem( rewardNode, str.c_str(), pQuestProto->rew_item[i], pQuestProto->rew_item_num[i], pQuestProto->rew_item_quality[i] );
		}
		//任务奖励的可选物品ID,数量
		for ( int i = 0; i < QUEST_REW_ITEM; ++i )
		{
			stringstream ss;
			string str;
			ss << "RewChoicesItemID" << i + 1 << endl;
			ss >> str;
			//SaveEntry( rewardNode, str.c_str(), pQuest->dwRewChoicesItemID[i] );
			//SaveEntry( rewardNode, "RewChoicesItemCount", pQuest->dwRewChoicesItemCount[i] );
			SaveItem( rewardNode, str.c_str(), pQuestProto->rew_choice_item[i], pQuestProto->rew_choice_item_num[i], pQuestProto->rew_choice_quality[i] );
		}
		//任务奖励的属性,值
		for ( int i = 0; i < QUEST_ATTS_COUNT; ++i )
		{
			stringstream ss;
			string str;
			ss << "RewAtt" << i + 1 << endl;
			ss >> str;
			//SaveEntry( rewardNode, str.c_str(), pQuest->dwRewAtt[i] );
			//SaveEntry( rewardNode, "RewAttValue", pQuest->dwRewAttValue[i] );
			SaveEntry( rewardNode, str.c_str(), pQuestProto->rew_att[i], pQuestProto->rew_att_val[i] );
		}

		//任务奖励的技能,值
		for ( int i = 0; i < QUEST_SKILLS_COUNT; ++i )
		{
			stringstream ss;
			string str;
			ss << "RewSkill" << i + 1 << endl;
			ss >> str;
			//SaveEntry( rewardNode, str.c_str(), pQuest->dwRewSkill[i] );
			//SaveEntry( rewardNode, "RewSkillValue", pQuest->dwRewSkillValue[i] );
			SaveEntry( rewardNode, str.c_str(), pQuestProto->rew_skill[i], pQuestProto->rew_skill_val[i] );
		}
		//任务奖励的buff
		for ( int i = 0; i < QUEST_REW_BUFF; ++i )
		{
			stringstream ss;
			string str;
			ss << "RewBuff" << i + 1 << endl;
			ss >> str;
			SaveEntry( rewardNode, str.c_str(), pQuestProto->rew_buff[i] );
		}
	}

	string strFileName = ToAnsi( pos->second->file_path );
	doc.SaveFile( strFileName.c_str() );

	//将任务文件的文字部分另存为一个文件 Local
	TiXmlDocument doc_local;
	TiXmlDeclaration* decl_local = new TiXmlDeclaration( "1.0", "UTF-8", "" );
	doc_local.LinkEndChild( decl_local );

	TiXmlElement* root_local = new TiXmlElement( "quest-local" );
	doc_local.LinkEndChild( root_local );
	{
		//任务ID
		SaveEntry( root_local, "ID", pQuestProto->id );
		//任务名称
		SaveEntry( root_local, "Name", pQuestProto->name );
		//任务条件
		SaveEntry( root_local, "Conditions", pQuestProto->conditions );
		//任务概述
		SaveEntry( root_local, "Overview", pQuestProto->over_view );
	}
	size_t find = strFileName.rfind(".");
	if ( find == string::npos )
		return FALSE;
	string str = strFileName.substr(0,find);
	str += ".local";
	doc_local.SaveFile( str.c_str() );
	return TRUE;
}


// ----------------------------------------------------------------------------
// 删除任务文件
// ----------------------------------------------------------------------------
void  QuestMgr::DeleteQuest(UINT16 id)
{
	
}

// ----------------------------------------------------------------------------
// 遍历制定目录，将所有任务文件加入集合
// ----------------------------------------------------------------------------
void QuestMgr::FindFile(LPCTSTR pFolder, list<wstring>& xmlMap, list<wstring>& localMap)
{
	TCHAR szFullPathFileName[MAX_PATH];
	TCHAR szFileName[MAX_PATH];
	WIN32_FIND_DATA FileData = {0};
	BOOL bFinished = FALSE;

	_sntprintf(szFullPathFileName, MAX_PATH-1, _T("%s\\*.*"), pFolder);
	HANDLE hSearch = FindFirstFile(szFullPathFileName, &FileData);
	if (hSearch == INVALID_HANDLE_VALUE)
		return;

	while (!bFinished)
	{
		_sntprintf(szFileName, MAX_PATH-1, _T("%s\\%s"), pFolder, FileData.cFileName);
		if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (_tcscmp(FileData.cFileName, _T(".")) && _tcscmp(FileData.cFileName, _T("..")) && _tcscmp(FileData.cFileName, _T(".svn")))
			{
				m_listQuestDir.push_back(FileData.cFileName);
				FindFile(szFileName, xmlMap, localMap);
			}
		}
		else
		{
			//文件扩展名匹配
			if(IsExtensionOK(szFileName, _T("xml")))
				xmlMap.push_back(szFileName);
			else if (IsExtensionOK(szFileName, _T("local")))
				localMap.push_back(szFileName);
		}

		if (!FindNextFile(hSearch,&FileData))
		{
			if (GetLastError() == ERROR_NO_MORE_FILES)
				bFinished = TRUE;
		}
	}
}	

// ----------------------------------------------------------------------------
// 加载任务文件
// ----------------------------------------------------------------------------
void QuestMgr::LoadQuestFile(LPCTSTR pPath)
{
	if (pPath == NULL || !_tcscmp(pPath, _T(""))) 
		return;

	//读取xml
	//加载XML
	string strDoc = ToAnsi(wstring(pPath));
	TiXmlDocument doc(strDoc.c_str());
	if (doc.LoadFile() == false) 
		return;

	TiXmlHandle hDoc(&doc);
	TiXmlHandle hRoot(0);

	//是否是任务文件
	TiXmlElement *pElem = hDoc.FirstChildElement("quest").Element();
	if (!pElem) 
	   return;

	hRoot = TiXmlHandle( pElem );

	stringstream ss;
	string str;

	XmlNodeMap xmlNode;

	TiXmlElement* pPropertyNode = hRoot.FirstChild( "property" ).FirstChild().Element();
	TiXmlElement* pAcceptConditionsNode = hRoot.FirstChild( "accept-conditions" ).FirstChild().Element();
	TiXmlElement* pCompleteConditionsNode = hRoot.FirstChild( "complete-conditions" ).FirstChild().Element();
	TiXmlElement* pRewNode = hRoot.FirstChild( "reward" ).FirstChild().Element();
	LoadXmlNode(pPropertyNode, xmlNode);
	LoadXmlNode(pAcceptConditionsNode, xmlNode);
	LoadXmlNode(pCompleteConditionsNode, xmlNode);
	LoadXmlNode(pRewNode, xmlNode);

	tagQuest* pQuest = new tagQuest;
	pQuest->pQuestProto = new tagQuestProto;
	ZeroMemory(pQuest->pQuestProto, sizeof(tagQuestProto));

	//为任务赋值
	SetQuestProto(pQuest->pQuestProto, xmlNode);
	//取得任务文件目录
	pQuest->file_path = wstring(pPath);
	size_t index = pQuest->file_path.rfind(_T("\\"));
	if (index != wstring::npos)
	{
		pQuest->file_name = pQuest->file_path.substr(index+1);
		wstring tmp = pQuest->file_path.substr(0, index);
		index = tmp.rfind(_T("\\"));
		if (index != wstring::npos)
		{
			pQuest->file_dir = tmp.substr(index + 1);
		}
	}
	//加入到任务集合中
	//检查任务是否存在
	QuestMap::iterator pos = m_mapQuest.find(pQuest->pQuestProto->id);
	if (pos != m_mapQuest.end())
	{
		delete pQuest->pQuestProto;
		delete pQuest;
	}
	else
	{
		m_mapQuest.insert(make_pair(pQuest->pQuestProto->id,pQuest));
	}
}


// ----------------------------------------------------------------------------
// 加载任务Local文件
// ----------------------------------------------------------------------------
void QuestMgr::LoadQuestLocal(LPCTSTR pPath)
{
	if (pPath == NULL || !_tcscmp(pPath, _T(""))) 
		return;

	//读取xml
	//加载XML
	string strDoc = ToAnsi(wstring(pPath));
	TiXmlDocument doc(strDoc.c_str());
	if (doc.LoadFile() == false) 
		return;

	TiXmlHandle hDoc(&doc);
	TiXmlHandle hRoot(0);

	//是否是任务Local文件
	TiXmlElement *pElem = hDoc.FirstChildElement("quest-local").Element();
	if (!pElem) 
		return;

	hRoot = TiXmlHandle( pElem );

	TiXmlElement *pNode = hRoot.FirstChild().Element();

	TiXmlAttribute* pAttrib = NULL;
	//ID
	pAttrib = pNode->FirstAttribute();
	if (pAttrib == NULL)
		return;
	
	INT16 id = atoi(pAttrib->Value());
	pNode = pNode->NextSiblingElement();
	
	//查找ID是否存在
	QuestMap::iterator pos = m_mapQuest.find(id);
	if (pos == m_mapQuest.end())
		return;

	tagQuest *pQuest = pos->second;
	if (pQuest == NULL)
		return;
	if(pQuest->pQuestProto == NULL)
		return;

	//Name
	if (pNode != NULL)
	{
		pAttrib = pNode->FirstAttribute();
		if(pAttrib!= NULL)
			_tcsncpy( pQuest->pQuestProto->name, ToGBKW(pAttrib->Value()).c_str(), X_SHORT_NAME );
		pNode = pNode->NextSiblingElement();
	}

	//condition
	if (pAttrib != NULL)
	{
		pAttrib = pNode->FirstAttribute();
		if(pAttrib!= NULL)
			_tcsncpy( pQuest->pQuestProto->conditions, ToGBKW(pAttrib->Value()).c_str(), X_SHORT_NAME );
		pNode = pNode->NextSiblingElement();
	}
	//overview
	if (pAttrib != NULL)
	{
		pAttrib = pNode->FirstAttribute();
		if(pAttrib!= NULL)
			_tcsncpy( pQuest->pQuestProto->over_view, ToGBKW(pAttrib->Value()).c_str(), QUEST_OVERVIEW );
	}
	pQuest->local_path = pPath;
}

BOOL QuestMgr::LoadQuestType( LPCTSTR szPath )
{
	if (szPath == NULL || !_tcscmp(szPath, _T(""))) 
		return FALSE;

	//读取xml
	//加载XML
	string strDoc = ToAnsi(wstring(szPath));
	TiXmlDocument doc(strDoc.c_str());
	if (doc.LoadFile() == false)
		return FALSE;

	TiXmlHandle hDoc(&doc);
	TiXmlHandle hRoot(0);

	//是否是任务文件
	TiXmlElement *pElem = hDoc.FirstChildElement("root").Element();
	if (!pElem) 
		return FALSE;

	hRoot = TiXmlHandle( pElem );

	stringstream ss;
	string str;

	XmlNodeArray xmlNode;

	TiXmlElement* pPropertyNode = hRoot.FirstChild().Element();
	LoadXmlNode(pPropertyNode, xmlNode);

	//
	while( !xmlNode.empty() )
	{
		XmlNodeArray::iterator iter = xmlNode.begin();
		INT16 id = (INT16)atoi(iter->second.c_str());
		xmlNode.erase(iter);

		iter = xmlNode.begin();
		TCHAR name[X_SHORT_NAME];
		_tcsncpy( name, ToGBKW( iter->second.c_str() ).c_str(), X_SHORT_NAME );
		xmlNode.erase(iter);

		// color直接跳过不读
		iter = xmlNode.begin();
		if(iter != xmlNode.end() && iter->first == "id-color")
			xmlNode.erase(iter);

		//category跳过去不读
		iter = xmlNode.begin();
		if(iter != xmlNode.end() && iter->first == "id-category")
			xmlNode.erase(iter);

		m_questType.push_back(make_pair(id,name));

	}

	return TRUE;
}

// ----------------------------------------------------------------------------
// 读取xml节点
// ----------------------------------------------------------------------------
void QuestMgr::LoadXmlNode(TiXmlElement *pNode, XmlNodeMap &xmlNodeMap)
{
	//是否存在该节点
	if (! pNode) 
		return;

	const char* pChar = NULL;
	TiXmlAttribute* pAttrib = NULL;
	while( pNode)
	{
		if ( pNode)
		{
			pAttrib =  pNode->FirstAttribute();
			string firstName;
			if (pAttrib != NULL)
			{
				firstName = pAttrib->Name();
			}
			while(pAttrib)
			{
				if (pAttrib->Name() != NULL && pAttrib->Value() != NULL)
				{
					if (pAttrib->Previous() == NULL)
					{
						xmlNodeMap.insert(make_pair(firstName, string(pAttrib->Value())));
					}
					else
					{
						string tmp = firstName;
						tmp += "-";
						tmp += pAttrib->Name();
						xmlNodeMap.insert( make_pair(tmp, pAttrib->Value()));
					}

				}
				pAttrib = pAttrib->Next();
			}

			pNode =  pNode->NextSiblingElement();
		}
	}
}

void QuestMgr::LoadXmlNode( TiXmlElement* pNode, XmlNodeArray& xmlNodeArray )
{
	//是否存在该节点
	if (! pNode) 
		return;

	const char* pChar = NULL;
	TiXmlAttribute* pAttrib = NULL;
	while( pNode)
	{
		if ( pNode)
		{
			pAttrib =  pNode->FirstAttribute();
			string firstName;
			if (pAttrib != NULL)
			{
				firstName = pAttrib->Name();
			}
			while(pAttrib)
			{
				if (pAttrib->Name() != NULL && pAttrib->Value() != NULL)
				{
					if (pAttrib->Previous() == NULL)
					{
						xmlNodeArray.push_back(make_pair(firstName, string(pAttrib->Value())));
					}
					else
					{
						string tmp = firstName;
						tmp += "-";
						tmp += pAttrib->Name();
						xmlNodeArray.push_back( make_pair(tmp, pAttrib->Value()));
					}

				}
				pAttrib = pAttrib->Next();
			}

			pNode =  pNode->NextSiblingElement();
		}
	}
}

// ----------------------------------------------------------------------------
//设置任务数据
// ----------------------------------------------------------------------------
void QuestMgr::SetQuestProto(tagQuestProto *pQuestProto, XmlNodeMap &xmlNodeMap)
{
	//property
	SetQuestUINT16("ID", pQuestProto->id, xmlNodeMap);
	SetQuestString("Name", pQuestProto->name, X_SHORT_NAME, xmlNodeMap );
	SetQuestUINT16("PrevQuestID", pQuestProto->prev_quest_id, xmlNodeMap );
	SetQuestUINT16("PrevQuest2ID", pQuestProto->prev_quest2_id, xmlNodeMap );
	SetQuestINT32("PrevQuestRelation", pQuestProto->prev_quest_relation, xmlNodeMap);
	SetQuestUINT16("NextQuestID", pQuestProto->next_quest_id, xmlNodeMap );
	SetQuestUINT16("UncmpleteQuestID", pQuestProto->uncomplete_quest_id, xmlNodeMap );
	SetQuestBool("AutoAddQuest", pQuestProto->auto_add_quest, xmlNodeMap );
	SetQuestBool("ShowQuestDialog", pQuestProto->show_quest_dialog, xmlNodeMap );
	SetQuestBool("NoDelete", pQuestProto->no_delete, xmlNodeMap );

	SetQuestDwordFlags("QuestPlayerAccept", pQuestProto->accept_flags, Quest_Accept_Flag_Player, xmlNodeMap );
	SetQuestDwordFlags("QuestSystemRelease", pQuestProto->accept_flags, Quest_Accept_Flag_System, xmlNodeMap );
	SetQuestDwordFlags("QuestItemAccept", pQuestProto->accept_flags, Quest_Accept_Flag_Item, xmlNodeMap );
	SetQuestINT32("TargetMode", (INT32&)pQuestProto->target_mode, xmlNodeMap);

	SetQuestString("Conditions", pQuestProto->conditions, X_SHORT_NAME, xmlNodeMap );
	SetQuestString("Overview", pQuestProto->over_view, QUEST_OVERVIEW, xmlNodeMap );
	SetQuestDword("QuestType", (DWORD&)pQuestProto->type, xmlNodeMap );
	SetQuestBYTE("SuggestedPlayers", pQuestProto->suggested_players, xmlNodeMap );
	SetQuestDword("LimitTime", pQuestProto->limit_time, xmlNodeMap );
	SetQuestBYTE("DoQuestTimes", pQuestProto->accept_times, xmlNodeMap );
	SetQuestBool("Periodic", pQuestProto->period, xmlNodeMap );
	SetQuestDword("QuestPeriodic", (DWORD&)pQuestProto->period_type, xmlNodeMap );
	SetQuestDword("Week", (DWORD&)pQuestProto->week, xmlNodeMap );
	SetQuestBool("Repeatable", pQuestProto->repeatable, xmlNodeMap );

	for( int i = 0; i < 6 ; ++i )
	{
		stringstream ss;
		string str;
		ss << "AcceptQuestNPCID" << i + 1 << endl;
		ss >> str;	
		SetQuestDword(str, pQuestProto->accept_quest_npc[i], xmlNodeMap );
	}

	for( int i = 0; i < 6 ; ++i )
	{
		stringstream ss;
		string str;
		ss << "CompleteQuestNPCID" << i + 1 << endl;
		ss >> str;	
		SetQuestDword(str, pQuestProto->complete_quest_npc[i], xmlNodeMap );
	}

	for( int i = 0; i < QUEST_ITEMS_COUNT ; ++i )
	{
		stringstream ss;
		string str;
		ss << "InvestigateObjectID" << i + 1 << endl;
		ss >> str;	
		SetQuestDword(str, pQuestProto->investigate_objects[i], xmlNodeMap );
	}

	SetQuestDword("SrcItemID", pQuestProto->src_item, xmlNodeMap );
	SetQuestINT16("SrcItemID-value", pQuestProto->src_item_num, xmlNodeMap );
	SetQuestFloat("Destination_X", pQuestProto->destination_x, xmlNodeMap);
	SetQuestFloat("Destination_Y", pQuestProto->destination_y, xmlNodeMap);
	SetQuestFloat("Destination_Z", pQuestProto->destination_z, xmlNodeMap);

	//accept-conditions
	SetQuestINT32("GetQuestRequiredMaxLevel", pQuestProto->accept_req_max_level, xmlNodeMap);
	SetQuestINT32("GetQuestRequiredMinLevel", pQuestProto->accept_req_min_level, xmlNodeMap);
	for( int i = 0; i < QUEST_REPUTATIONS_COUNT ; ++i )
	{
		stringstream ss;
		string str;
		ss << "GetQuestRequiredRep" << i + 1 << endl;
		ss >> str;	
		SetQuestINT32(str, pQuestProto->accept_req_rep[i], xmlNodeMap );
		string tmp = str;
		tmp += "-max";
		SetQuestINT32(tmp, pQuestProto->accept_req_max_rep[i], xmlNodeMap );
		tmp = str;
		tmp += "-min";
		SetQuestINT32(tmp, pQuestProto->accept_req_min_rep[i], xmlNodeMap );
	}
	for( int i = 0; i < QUEST_SKILLS_COUNT ; ++i )
	{
		stringstream ss;
		string str;
		ss << "GetQuestRequiredSkill" << i + 1 << endl;
		ss >> str;	
		SetQuestDword(str, pQuestProto->accept_req_skill[i], xmlNodeMap );
		string tmp = str;
		tmp += "-value";
		SetQuestINT32(tmp, pQuestProto->accept_req_skill_val[i], xmlNodeMap );
	}
	SetQuestBYTE("GetQuestRequiredSex", pQuestProto->sex, xmlNodeMap);
	SetQuestDword("GetQuestRequiredClass", (DWORD&)pQuestProto->class_type, xmlNodeMap);
	SetQuestDword("GetQuestRequiredMapID", pQuestProto->accept_req_map, xmlNodeMap);
	for( int i = 0; i < QUEST_ITEMS_COUNT ; ++i )
	{
		stringstream ss;
		string str;
		ss << "GetQuestRequiredItemID" << i + 1 << endl;
		ss >> str;	
		SetQuestDword(str, pQuestProto->accept_req_item[i], xmlNodeMap );
		string tmp = str;
		tmp += "-value";
		SetQuestINT16(tmp, pQuestProto->accept_req_item_num[i], xmlNodeMap );
	}
	for( int i = 0; i < QUEST_ATTS_COUNT ; ++i )
	{
		stringstream ss;
		string str;
		ss << "GetQuestRequiredAtt" << i + 1 << endl;
		ss >> str;	
		SetQuestINT32(str, pQuestProto->accept_req_att[i], xmlNodeMap );
		string tmp = str;
		tmp += "-value";
		SetQuestINT16(tmp, pQuestProto->accept_req_att_val[i], xmlNodeMap );
	}
	SetQuestBool("DelRestrictedItem", pQuestProto->del_req_item, xmlNodeMap);
	for( int i = 0; i < QUEST_TRIGGERS_COUNT ; ++i )
	{
		stringstream ss;
		string str;
		ss << "GetQuestReqTriggersID" << i + 1 << endl;
		ss >> str;	
		SetQuestDword(str, pQuestProto->accept_req_trriger[i], xmlNodeMap );
	}
	SetQuestBool("GetQuestReqMarried", pQuestProto->married, xmlNodeMap);
	SetQuestBYTE("GetQuestReqLoverNum", pQuestProto->lover_num, xmlNodeMap);
	SetQuestINT32("GetQuestReqMoneyNum", pQuestProto->accept_req_money, xmlNodeMap );
	SetQuestBool("GetQuestDelMoneyNum", pQuestProto->accept_del_money, xmlNodeMap);
	
	//complete-conditions
	for( int i = 0; i < QUEST_ITEMS_COUNT; ++i )
	{
		stringstream ss;
		string str;
		ss << "CompleteQuestRequiredItemID" << i + 1 << endl;
		ss >> str;	
		SetQuestDword(str, pQuestProto->complete_req_item[i], xmlNodeMap );
		string tmp = str;
		tmp += "-value";
		SetQuestINT16(tmp, pQuestProto->complete_req_item_num[i], xmlNodeMap );
	}
	SetQuestBool("RequiredOnlyOneItem", pQuestProto->only_one_item, xmlNodeMap);
	SetQuestBool("DeleteItem", pQuestProto->delete_item, xmlNodeMap);

	for( int i = 0; i < QUEST_CREATURES_COUNT; ++i )
	{
		stringstream ss;
		string str;
		ss << "CompleteQuestRequiredCreatureID" << i + 1 << endl;
		ss >> str;	
		SetQuestDword(str, pQuestProto->complete_req_creature[i*3], xmlNodeMap );
		string tmp = str;
		tmp += "-id2";
		SetQuestDword(tmp, pQuestProto->complete_req_creature[i*3+1], xmlNodeMap );
		tmp = str;
		tmp += "-id3";
		SetQuestDword(tmp, pQuestProto->complete_req_creature[i*3+2], xmlNodeMap );
		tmp = str;
		tmp += "-value";
		SetQuestINT16(tmp, pQuestProto->complete_req_creature_num[i], xmlNodeMap );
	}

	SetQuestBool("RequiredCreatureLevel", pQuestProto->creature_level, xmlNodeMap);
	SetQuestBool("RequiredOnlyOneCreature", pQuestProto->only_one_creature, xmlNodeMap);

	for( int i = 0; i < QUEST_NPC_COUNT ; ++i )
	{
		stringstream ss;
		string str;
		ss << "CompleteQuestRequiredNPC" << i + 1 << endl;
		ss >> str;	
		SetQuestDword(str, pQuestProto->complete_req_npc[i], xmlNodeMap );
	}

	SetQuestBool("RequiredOnlyInOrder", pQuestProto->only_in_order, xmlNodeMap);
	SetQuestBool("RequiredOnlyOneNPC", pQuestProto->only_one_npc, xmlNodeMap);

	for( int i = 0; i < QUEST_TRIGGERS_COUNT; ++i )
	{
		stringstream ss;
		string str;
		ss << "CompleteQuestReqTriggersID" << i + 1 << endl;
		ss >> str;	
		SetQuestDword(str, pQuestProto->complete_req_trigger[i], xmlNodeMap );
	}
	SetQuestBool("RequiredOnlyOneTrigger", pQuestProto->only_one_trigger, xmlNodeMap);

	SetQuestINT32("CompleteQuestRequiredMoney", pQuestProto->complete_req_money, xmlNodeMap);
	SetQuestBool("CompleteQuestDelMoney", pQuestProto->complete_del_money, xmlNodeMap);
	SetQuestDword("CompleteQuestRequiredMapID", pQuestProto->complete_req_map, xmlNodeMap);
	SetQuestFloat("CompleteQuestRequiredMap_X", pQuestProto->complete_req_map_x, xmlNodeMap);
	SetQuestFloat("CompleteQuestRequiredMap_Y", pQuestProto->complete_req_map_y, xmlNodeMap);
	SetQuestFloat("CompleteQuestRequiredMap_Z", pQuestProto->complete_req_map_z, xmlNodeMap);
	SetQuestFloat("CompleteQuestRequiredMap_Radius", pQuestProto->complete_req_map_radius, xmlNodeMap);

	SetQuestBool("CompleteQuestRequiredMap_Random", pQuestProto->coordinate_random, xmlNodeMap);
	SetQuestBool("RequiredOnlyOneCondition", pQuestProto->only_one_condition, xmlNodeMap);

	SetQuestBool("CompleteQuestReqMarried", pQuestProto->complete_req_married, xmlNodeMap);
	SetQuestBYTE("CompleteQuestReqLoverNum", pQuestProto->complete_req_lover_num, xmlNodeMap);
	SetQuestINT32("CompleteQuestReqLevel", pQuestProto->complete_req_level, xmlNodeMap);

	SetQuestDword("CompleteQuestEventType", (DWORD&)pQuestProto->event_type, xmlNodeMap );
	SetQuestDword("UseItemEvent_Item_ID", pQuestProto->use_item, xmlNodeMap);
	SetQuestDword("UseSkillEvent_Skill_ID", pQuestProto->use_skill, xmlNodeMap);
	//Reword
	SetQuestINT32("RewMoney", pQuestProto->rew_money, xmlNodeMap);
	SetQuestINT32("RewXP", pQuestProto->rew_xp, xmlNodeMap);
	
	for( int i = 0; i < QUEST_REPUTATIONS_COUNT; ++i )
	{
		stringstream ss;
		string str;
		ss << "RewRequiredRep" << i + 1 << endl;
		ss >> str;	
		SetQuestDword(str, pQuestProto->rew_rep[i], xmlNodeMap );
		string tmp = str;
		tmp += "-value";
		SetQuestINT32(tmp, pQuestProto->rew_rep_val[i], xmlNodeMap );
	}

	//贡献
	for( int i = 0; i < QUEST_CONTRIBUTION_COUNT; ++i )
	{
		stringstream ss;
		string str;
		ss << "RewRequiredContribution" << i + 1 << endl;
		ss >> str;	
		SetQuestDword(str, pQuestProto->rew_contribution[i], xmlNodeMap );
		string tmp = str;
		tmp += "-value";
		SetQuestINT32(tmp, pQuestProto->rew_contribution_val[i], xmlNodeMap );
	}

	for( int i = 0; i < QUEST_REW_ITEM; ++i )
	{
		stringstream ss;
		string str;
		ss << "RewItemID" << i + 1 << endl;
		ss >> str;	
		SetQuestDword(str, pQuestProto->rew_item[i], xmlNodeMap );
		string tmp = str;
		tmp += "-value";
		SetQuestINT16(tmp, pQuestProto->rew_item_num[i], xmlNodeMap );
		tmp = str;
		tmp += "-quality";
		SetQuestINT16(tmp, pQuestProto->rew_item_quality[i], xmlNodeMap );
	}

	for( int i = 0; i < QUEST_REW_ITEM; ++i )
	{
		stringstream ss;
		string str;
		ss << "RewChoicesItemID" << i + 1 << endl;
		ss >> str;	
		SetQuestDword(str, pQuestProto->rew_choice_item[i], xmlNodeMap );
		string tmp = str;
		tmp += "-value";
		SetQuestINT16(tmp, pQuestProto->rew_choice_item_num[i], xmlNodeMap );
		tmp = str;
		tmp += "-quality";
		SetQuestINT16(tmp, pQuestProto->rew_choice_quality[i], xmlNodeMap );
	}

	for( int i = 0; i < QUEST_ATTS_COUNT; ++i )
	{
		stringstream ss;
		string str;
		ss << "RewAtt" << i + 1 << endl;
		ss >> str;	
		SetQuestINT32(str, pQuestProto->rew_att[i], xmlNodeMap );
		string tmp = str;
		tmp += "-value";
		SetQuestINT16(tmp, pQuestProto->rew_att_val[i], xmlNodeMap );
	}

	for( int i = 0; i < QUEST_SKILLS_COUNT; ++i )
	{
		stringstream ss;
		string str;
		ss << "RewSkill" << i + 1 << endl;
		ss >> str;	
		SetQuestDword(str, pQuestProto->rew_skill[i], xmlNodeMap );
		string tmp = str;
		tmp += "-value";
		SetQuestINT32(tmp, pQuestProto->rew_skill_val[i], xmlNodeMap );
	}

	for( int i = 0; i < QUEST_REW_BUFF; ++i )
	{
		stringstream ss;
		string str;
		ss << "RewBuff" << i + 1 << endl;
		ss >> str;	
		SetQuestDword(str, pQuestProto->rew_buff[i], xmlNodeMap );
	}
}

// ----------------------------------------------------------------------------
// string ----> DWORD
// ----------------------------------------------------------------------------
void  QuestMgr::SetQuestDword(std::string str, DWORD& dwQuest, XmlNodeMap& xmlNodeMap)
{
	XmlNodeMap::iterator pos = xmlNodeMap.find(str);
	if ( pos != xmlNodeMap.end() )
	{
		dwQuest = atol(pos->second.c_str());
		//xmlNodeMap.erase(pos);
	}
}

// ----------------------------------------------------------------------------
// string ----> DWORD FLAGs
// ----------------------------------------------------------------------------
void  QuestMgr::SetQuestDwordFlags(std::string str, DWORD& dwQuest, INT32 nMask, XmlNodeMap& xmlNodeMap)
{
	XmlNodeMap::iterator pos = xmlNodeMap.find(str);
	if ( pos != xmlNodeMap.end() )
	{
		DWORD n = atol(pos->second.c_str());
		if(n != 0)
			dwQuest = dwQuest | nMask;
		xmlNodeMap.erase(pos);
	}
}

// ----------------------------------------------------------------------------
// string ----> UINT16
// ----------------------------------------------------------------------------
void  QuestMgr::SetQuestUINT16(std::string str, UINT16& unQuest, XmlNodeMap& xmlNodeMap)
{
	XmlNodeMap::iterator pos = xmlNodeMap.find(str);
	if ( pos != xmlNodeMap.end() )
	{
		unQuest = (UINT16)atoi(pos->second.c_str());
		xmlNodeMap.erase(pos);
	}
}

// ----------------------------------------------------------------------------
// string ----> BYTE
// ----------------------------------------------------------------------------
void  QuestMgr::SetQuestBYTE(std::string str, BYTE& byQuest, XmlNodeMap& xmlNodeMap)
{
	XmlNodeMap::iterator pos = xmlNodeMap.find(str);
	if ( pos != xmlNodeMap.end() )
	{
		byQuest = (BYTE)atoi(pos->second.c_str());
		xmlNodeMap.erase(pos);
	}
}

// ----------------------------------------------------------------------------
// string ----> INT16
// ----------------------------------------------------------------------------
void  QuestMgr::SetQuestINT16(std::string str, INT16& n16Quest, XmlNodeMap& xmlNodeMap)
{
	XmlNodeMap::iterator pos = xmlNodeMap.find(str);
	if ( pos != xmlNodeMap.end() )
	{
		n16Quest = (INT16)atoi(pos->second.c_str());
		xmlNodeMap.erase(pos);
	}
}

// ----------------------------------------------------------------------------
// string ----> INT32
// ----------------------------------------------------------------------------
void  QuestMgr::SetQuestINT32(std::string str, INT32& nQuest, XmlNodeMap& xmlNodeMap)
{
	XmlNodeMap::iterator pos = xmlNodeMap.find(str);
	if ( pos != xmlNodeMap.end() )
	{
		nQuest = atoi(pos->second.c_str());
		//xmlNodeMap.erase(pos);
	}
}

// ----------------------------------------------------------------------------
// string ----> wstring
// ----------------------------------------------------------------------------
void QuestMgr::SetQuestString(std::string str,TCHAR* szQuest, INT32 nLen, XmlNodeMap& xmlNodeMap)
{
	XmlNodeMap::iterator pos = xmlNodeMap.find(str);
	if ( pos != xmlNodeMap.end() )
	{
		_tcsncpy( szQuest, ToGBKW( pos->second.c_str() ).c_str(), nLen );
		xmlNodeMap.erase(pos);
	}
}

// ----------------------------------------------------------------------------
// string ----> BOOL
// ----------------------------------------------------------------------------
void  QuestMgr::SetQuestBool(std::string str, BOOL& bQuest, XmlNodeMap& xmlNodeMap)
{
	XmlNodeMap::iterator pos = xmlNodeMap.find(str);
	if ( pos != xmlNodeMap.end() )
	{
		bQuest = atol(pos->second.c_str());
		xmlNodeMap.erase(pos);
	}
}

// ----------------------------------------------------------------------------
// string ----> Float
// ----------------------------------------------------------------------------
void  QuestMgr::SetQuestFloat(std::string str, FLOAT& fQuest, XmlNodeMap& xmlNodeMap)
{
	XmlNodeMap::iterator pos = xmlNodeMap.find(str);
	if ( pos != xmlNodeMap.end() )
	{
		fQuest = (FLOAT)atof(pos->second.c_str());
		xmlNodeMap.erase(pos);
	}
}

// ----------------------------------------------------------------------------
// wstring ---> string
// ----------------------------------------------------------------------------
string QuestMgr::ToAnsi(const wstring &widestring)
{
	// Determine the length necessary for the string in UTF8
	int utf8_length = WideCharToMultiByte(CP_ACP, 0, widestring.c_str(), -1, NULL, 0, NULL, NULL);

	if (utf8_length == 0) return string(""); 

	vector<char> result(utf8_length);
	int length = WideCharToMultiByte(CP_ACP, 0, widestring.c_str(), -1, &result[0], utf8_length, NULL, NULL);

	if (length != utf8_length) return string(""); 

	return string(&result[0]);
}

// ----------------------------------------------------------------------------
// wstring ---> string utf8
// ----------------------------------------------------------------------------
string QuestMgr::ToUtf8(const wstring &widestring)
{
	// Determine the length necessary for the string in UTF8
	int utf8_length = WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, NULL, 0, NULL, NULL);

	if (utf8_length == 0) return string(""); 

	vector<char> result(utf8_length);
	int length = WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, &result[0], utf8_length, NULL, NULL);

	if (length != utf8_length) return string(""); 

	return string(&result[0]);
}

// ----------------------------------------------------------------------------
// string ---> wstring
// ----------------------------------------------------------------------------
wstring QuestMgr::ToGBKW(const char *utf8)
{
	int len=MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL,0);
	unsigned short * wszGBK = new unsigned short[len+1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, (LPWSTR)wszGBK, len);
	wstring wstr = (LPCWSTR)wszGBK;
	delete[] wszGBK;
	wszGBK = NULL;
	return wstr;
}

void QuestMgr::SaveEntry( TiXmlElement *pEle, LPCSTR szName, LPCTSTR szValue )
{
	TiXmlElement* entry;
	entry = new TiXmlElement( "entry" );
	pEle->LinkEndChild( entry );
	//entry->SetAttribute( "name", szName );
	string str = ToUtf8( szValue );
	entry->SetAttribute( szName, str.c_str() );
}

void QuestMgr::SaveEntry( TiXmlElement *pEle, LPCSTR szName, DWORD dwValue )
{
	TiXmlElement* entry;
	entry = new TiXmlElement( "entry" );
	pEle->LinkEndChild( entry );
	//entry->SetAttribute( "name", szName );
	entry->SetAttribute( szName, dwValue );
}

void QuestMgr::SaveEntry( TiXmlElement* pEle, LPCSTR szName, DWORD dwValue1, DWORD dwValue2 )
{
	TiXmlElement* entry;
	entry = new TiXmlElement( "entry" );
	pEle->LinkEndChild( entry );
	entry->SetAttribute( szName, dwValue1 );
	entry->SetAttribute( "value", dwValue2 );
}

void QuestMgr::SaveItem( TiXmlElement* pEle, LPCSTR szName, DWORD dwValue1, DWORD dwValue2, INT nValue3 )
{
	TiXmlElement* entry;
	entry = new TiXmlElement( "entry" );
	pEle->LinkEndChild( entry );
	entry->SetAttribute( szName, dwValue1 );
	entry->SetAttribute( "value", dwValue2 );
	entry->SetAttribute( "quality", nValue3 );
}

void QuestMgr::SaveMaxEntry( TiXmlElement* pEle, LPCSTR szName, DWORD dwValue, DWORD dwMax, DWORD dwMin )
{
	TiXmlElement* entry;
	entry = new TiXmlElement( "entry" );
	pEle->LinkEndChild( entry );
	entry->SetAttribute( szName, dwValue );
	entry->SetAttribute( "max", dwMax );
	entry->SetAttribute( "min", dwMin );
}

void QuestMgr::SaveCreatureEntry( TiXmlElement* pEle, LPCSTR szName, DWORD dwValue, DWORD dwID2, DWORD dwID3, DWORD dwNum )
{
	TiXmlElement* entry;
	entry = new TiXmlElement( "entry" );
	pEle->LinkEndChild( entry );
	entry->SetAttribute( szName, dwValue );
	entry->SetAttribute( "id2", dwID2 );
	entry->SetAttribute( "id3", dwID3 );
	entry->SetAttribute( "value", dwNum );
}

void QuestMgr::AddQuestToMap()
{
	if (m_pQuestTmp == NULL)
		return;
	if (m_pQuestTmp->pQuestProto == NULL)
		return;

	m_mapQuest.insert(make_pair(m_pQuestTmp->pQuestProto->id, m_pQuestTmp));
	m_pQuestTmp = NULL;
}

void QuestMgr::SaveAcceptQuestRelation(LPCTSTR pPath)
{
	//key可以重复
	multimap< DWORD, DWORD > mapQuestAcceptRelation;
	DWORD npcID;

	QuestMap::iterator it = m_mapQuest.begin();
	for (; it != m_mapQuest.end(); ++it)
	{
		tagQuestProto* pQuestProto = it->second->pQuestProto;
		if (pQuestProto == NULL)
			continue;
		for ( int i = 0; i < QUEST_ACCEPT_NPC; ++i )
		{
			npcID = pQuestProto->accept_quest_npc[i];
			if ( npcID != 0 )
			{
				mapQuestAcceptRelation.insert( make_pair( npcID, pQuestProto->id ) );
			}
		}
	}

	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "UTF-8", "" );
	doc.LinkEndChild( decl );

	TiXmlElement* root = new TiXmlElement( "accept-quest-relation" );
	doc.LinkEndChild( root );

	multimap< DWORD, DWORD >::iterator it2 = mapQuestAcceptRelation.begin();
	for ( ; it2 != mapQuestAcceptRelation.end(); ++it2 )
	{
		TiXmlElement* entry;
		entry = new TiXmlElement( "entry" );
		root->LinkEndChild( entry );
		entry->SetAttribute( "NPCID",  it2->first );
		entry->SetAttribute( "QuestID",  it2->second );
	}
	wstring strPath =  pPath;
	strPath += _T("\\");
	strPath += _T("accept_quest_npc.data");
	string str = ToAnsi(strPath.c_str());
	doc.SaveFile( str.c_str() );
}

void QuestMgr::SaveCompleteQuestRelation(LPCTSTR pPath)
{
	//key可以重复
	multimap< DWORD, DWORD > mapQuestCompleteRelation;
	DWORD npcID;

	QuestMap::iterator it = m_mapQuest.begin();
	for (; it != m_mapQuest.end(); ++it)
	{
		tagQuestProto* pQuestProto = it->second->pQuestProto;
		if (pQuestProto == NULL)
			continue;
		for ( int i = 0; i < QUEST_COMPLETE_NPC; ++i )
		{
			npcID = pQuestProto->complete_quest_npc[i];
			if ( npcID != 0 )
			{
				mapQuestCompleteRelation.insert( make_pair( npcID, pQuestProto->id ) );
			}
		}
	}

	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "UTF-8", "" );
	doc.LinkEndChild( decl );

	TiXmlElement* root = new TiXmlElement( "complete-quest-relation" );
	doc.LinkEndChild( root );

	multimap< DWORD, DWORD >::iterator it2 = mapQuestCompleteRelation.begin();
	for ( ; it2 != mapQuestCompleteRelation.end(); ++it2 )
	{
		TiXmlElement* entry;
		entry = new TiXmlElement( "entry" );
		root->LinkEndChild( entry );
		entry->SetAttribute( "NPCID",  it2->first );
		entry->SetAttribute( "QuestID",  it2->second );
	}
	wstring strPath =  pPath;
	strPath += _T("\\");
	strPath += _T("complete_quest_npc.data");
	string str = ToAnsi(strPath.c_str());
	doc.SaveFile( str.c_str() );
}

// ----------------------------------------------------------------------------
// 导出任务完成选项中的8个NPC
// ----------------------------------------------------------------------------
void QuestMgr::SaveQuestNPCRelation(LPCTSTR pPath)
{
	//key可以重复
	multimap< DWORD, DWORD > mapQuestNPCRelation;
	DWORD npcID;

	QuestMap::iterator it = m_mapQuest.begin();
	for (; it != m_mapQuest.end(); ++it)
	{
		tagQuestProto* pQuestProto = it->second->pQuestProto;
		if (pQuestProto == NULL)
			continue;
	
		for ( int i = 0; i < QUEST_NPC_COUNT; ++i )
		{
			npcID = pQuestProto->complete_req_npc[i];
			if ( npcID != 0 )
			{
				mapQuestNPCRelation.insert( make_pair( npcID, pQuestProto->id ) );
			}
		}
	}

	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "UTF-8", "" );
	doc.LinkEndChild( decl );

	TiXmlElement* root = new TiXmlElement( "quest-npc-relation" );
	doc.LinkEndChild( root );

	multimap< DWORD, DWORD >::iterator it2 = mapQuestNPCRelation.begin();
	for ( ; it2 != mapQuestNPCRelation.end(); ++it2 )
	{
		TiXmlElement* entry;
		entry = new TiXmlElement( "entry" );
		root->LinkEndChild( entry );
		entry->SetAttribute( "NPCID",  it2->first );
		entry->SetAttribute( "QuestID",  it2->second );
	}
	wstring strPath =  pPath;
	strPath += _T("\\");
	strPath += _T("quest_npc.data");
	string str = ToAnsi(strPath.c_str());
	doc.SaveFile( str.c_str() );
}


// ----------------------------------------------------------------------------
// //检查文件扩展名是否匹配
// ----------------------------------------------------------------------------
bool QuestMgr::IsExtensionOK(LPCTSTR pFileName, LPCTSTR pExt)
{
	wstring file(pFileName);
	wstring ext(pExt);

	size_t pos = file.rfind(_T("."));
	if (pos != wstring::npos)
	{
		return file.substr(pos + 1) == ext;
	}
	else
	{
		return false;
	}
}