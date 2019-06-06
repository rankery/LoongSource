/********************************************************************
	created:	2008/08/26
	created:	26:8:2008   14:08
	filename: 	d:\loong_client\Client\QuestData.cpp
	file path:	d:\loong_client\Client
	file base:	QuestData
	file ext:	cpp
	author:		leili
	
	purpose:	任务数据读取、存储，处理一些根据任务ID的查询
*********************************************************************/
#include "StdAfx.h"
#include "QuestData.h"
#include "QuestQuery.h"
#include "QuestMgr.h"
#include "RoleMgr.h"
#include "LocalPlayer.h"
#include "..\Cool3D\Util\CSVFile.h"

QuestData::QuestData( void )
{
	m_eCurFilter = EF_NoFilter;
	m_eLastFilter = EF_CustomFilter1;
	m_bFilterByLevel = FALSE;
}

QuestData::~QuestData( void )
{
	m_quests.clear();
	m_npc2AcceptQuests.clear();
	m_npc2CompleteQuests.clear();
	m_npc2UpdateQuests.clear();
	m_npc2Script.clear();
	m_questStates.clear();
	m_questtype.clear();
}

//-----------------------------------------------------------------------------
// 加载任务Local信息(名称, 条件, 概述)
//-----------------------------------------------------------------------------
BOOL QuestData::LoadQuestLocalData(LPCTSTR szPath, list<tstring>* pFileList)
{
	//路径为空
	if ( _tcscmp( szPath, _T("") ) == 0 || pFileList == NULL )
	{
		return FALSE;
	}

	//遍历任务文件集合，加载任务文件
	list<tstring>::iterator it = pFileList->begin();
	for ( ; it != pFileList->end(); ++it )
	{
		//得到任务文件相对路径
		tstring name = szPath;
		name += _T("\\") + (*it) + _T(".local");
		if ( LoadQuestLocal( name.c_str() ) != TRUE)
		{
			IMSG(_T("load local quest file %s failed!"), name.c_str());
			return FALSE;
		}
	}

	return TRUE;

}

//-----------------------------------------------------------------------------
// 加载任务Local文件
//-----------------------------------------------------------------------------
BOOL QuestData::LoadQuestLocal(LPCTSTR szFileName)
{
	if (szFileName == NULL || !_tcscmp(szFileName, _T(""))) 
		return FALSE;

	//读取xml
	//加载XML
	XmlDocument doc;
	if (doc.LoadFile("VFS_System", szFileName) == false) 
		return FALSE;

	XmlHandle hDoc(&doc);
	XmlHandle hRoot(0);

	//是否是任务Local文件
	XmlElement *pElem = hDoc.FirstChildElement("quest-local").Element();
	if (!pElem) 
		return FALSE;

	hRoot = XmlHandle( pElem );

	XmlElement *pNode = hRoot.FirstChild().Element();

	XmlAttribute* pAttrib = NULL;
	//ID
	pAttrib = pNode->FirstAttribute();
	if (pAttrib == NULL)
		return FALSE;

	INT16 id = atoi(pAttrib->Value());
	pNode = pNode->NextSiblingElement();

	//查找ID是否存在
	QuestsMap::iterator pos = m_quests.find(id);
	if (pos == m_quests.end())
		return FALSE;

	//Name
	if (pNode != NULL)
	{
		pAttrib = pNode->FirstAttribute();
		if(pAttrib!= NULL)
			_tcsncpy( pos->second.name, TObjRef<Util>()->Unicode8ToUnicode(pAttrib->Value()), X_SHORT_NAME );
		pNode = pNode->NextSiblingElement();
	}

	//condition
	if (pAttrib != NULL)
	{
		pAttrib = pNode->FirstAttribute();
		if(pAttrib!= NULL)
			_tcsncpy( pos->second.conditions, TObjRef<Util>()->Unicode8ToUnicode(pAttrib->Value()), X_SHORT_NAME );
		pNode = pNode->NextSiblingElement();
	}
	//overview
	if (pAttrib != NULL)
	{
		pAttrib = pNode->FirstAttribute();
		if(pAttrib!= NULL)
			_tcsncpy( pos->second.over_view, TObjRef<Util>()->Unicode8ToUnicode(pAttrib->Value()), QUEST_OVERVIEW );
	}
	return TRUE;
}

//-----------------------------------------------------------------------------
// 加载任务信息
//-----------------------------------------------------------------------------
BOOL QuestData::LoadQuestData( LPCTSTR szPath, list<tstring>* pFileList )
{
	//遍历任务文件集合，加载任务文件
	list<tstring>::iterator it = pFileList->begin();
	for ( ; it != pFileList->end(); ++it )
	{
		tagQuestProto quest;
		//得到任务文件相对路径
		tstring name = szPath;
		name += _T("\\") + (*it) + _T(".xml");
		if ( !LoadQuestFromXml(&quest, name.c_str()) )
		{
			IMSG(_T("load quest file %s failed!"), name.c_str());
			return FALSE;
		}
		else
		{
			m_quests.insert( make_pair( quest.id, quest ) );
			tagQuestState state;
			m_questStates.insert( make_pair( quest.id, state ) );
		}
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 加载NPC与可接任务的对应文件
//-----------------------------------------------------------------------------
BOOL QuestData::LoadNPCAcceptQuestFile( LPCTSTR szFile )
{
	m_npc2AcceptQuests.clear();

	//读入xml文件
	if ( szFile == NULL )
	{
		return FALSE;
	}

	XmlDocument doc;
	if ( !doc.LoadFile("VFS_System", szFile) )
	{
		return FALSE;
	}

	XmlHandle hDoc( &doc );

	//是否是任务关系文件
	XmlHandle eleHandle = hDoc.FirstChildElement("accept-quest-relation").FirstChildElement();
	XmlElement *pElem = eleHandle.Element();
	if (!pElem) return FALSE;

	while( P_VALID(pElem) )
	{
		DWORD dwNPCID;
		UINT16 uQuestID;
		const char *pChar = NULL;
		pChar = pElem->Attribute("NPCID");
		if (pChar) dwNPCID = atol(pChar);
		pChar = pElem->Attribute("QuestID");
		if (pChar) uQuestID = (UINT16)atol(pChar);

		m_npc2AcceptQuests.insert(make_pair(dwNPCID,uQuestID));
		pElem = pElem->NextSiblingElement();

	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 加载NPC与可交任务的对应文件
//-----------------------------------------------------------------------------
BOOL QuestData::LoadNPCCompleteQuestFile( LPCTSTR szFile )
{
	m_npc2CompleteQuests.clear();

	//读入xml文件
	if ( szFile == NULL )
	{
		return FALSE;
	}

	XmlDocument doc;
	if ( !doc.LoadFile("VFS_System", szFile) )
	{
		return FALSE;
	}

	XmlHandle hDoc( &doc );

	//是否是任务关系文件
	XmlHandle eleHandle = hDoc.FirstChildElement("complete-quest-relation").FirstChildElement();
	XmlElement *pElem = eleHandle.Element();
	if (!pElem) return FALSE;

	while( P_VALID(pElem) )
	{
		DWORD dwNPCID;
		UINT16 uQuestID;
		const char *pChar = NULL;
		pChar = pElem->Attribute("NPCID");
		if (pChar) dwNPCID = atol(pChar);
		pChar = pElem->Attribute("QuestID");
		if (pChar) uQuestID = (UINT16)atol(pChar);

		m_npc2CompleteQuests.insert(make_pair(dwNPCID,uQuestID));
		pElem = pElem->NextSiblingElement();

	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 加载NPC与可更新任务的对应文件
//-----------------------------------------------------------------------------
BOOL QuestData::LoadNPCUpdateQuestFile( LPCTSTR szFile )
{
	m_npc2UpdateQuests.clear();

	//读入xml文件
	if ( szFile == NULL )
	{
		return FALSE;
	}

	XmlDocument doc;
	if ( !doc.LoadFile("VFS_System", szFile) )
	{
		return FALSE;
	}

	XmlHandle hDoc( &doc );

	//是否是任务关系文件
	XmlHandle eleHandle = hDoc.FirstChildElement("quest-npc-relation").FirstChildElement();
	XmlElement *pElem = eleHandle.Element();
	if (!pElem) return FALSE;

	while( P_VALID(pElem) )
	{
		DWORD dwNPCID;
		UINT16 uQuestID;
		const char *pChar = NULL;
		pChar = pElem->Attribute("NPCID");
		if (pChar) dwNPCID = atol(pChar);
		pChar = pElem->Attribute("QuestID");
		if (pChar) uQuestID = (UINT16)atol(pChar);

		m_npc2UpdateQuests.insert(make_pair(dwNPCID,uQuestID));
		pElem = pElem->NextSiblingElement();
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 加载某张地图的NPC与脚本的对应文件
//-----------------------------------------------------------------------------
BOOL QuestData::LoadNPCScriptFile( LPCTSTR szMap )
{
	m_npc2Script.clear();

	// 读取本地图的csv
	TCHAR szPath[_MAX_PATH];
	_stprintf( szPath, _T("%s\\script\\%s.csv"), vEngine::g_strLocalPath.c_str(), szMap );
	if( !LoadCSV( szPath ) )
		return FALSE;

	// 读取common.csv
	_stprintf( szPath, _T("%s\\script\\common.csv"), vEngine::g_strLocalPath.c_str() );
	return LoadCSV( szPath );
}

BOOL QuestData::LoadCSV( LPCTSTR szPath )
{
	IFS* pIFS = (IFS*)(VOID*)TObjRef<VirtualFileSys>("VFS_System");

	CSVFile csv;
	if( !csv.OpenFile(pIFS, szPath) )
	{
		IMSG(_T("NPC脚本绑定文件打开失败,%s\r\n"), szPath);
		return FALSE;
	}

	csv.BeginReadLine();
	while( true )
	{
		const vector<tstring>& vec = csv.GetNextLine();
		if( vec.empty() )
			break;
		ASSERT( vec.size() == 2 );
		if( vec.size() != 2 )
			break;

		//十六进制:0x00000000
		DWORD npcID = _tcstoul(vec[0].c_str(),NULL,0);
		tstring szScript = vec[1];
		ASSERT(szScript.length()>0);
		//过滤掉结尾的回车换行符
		if( szScript[szScript.length()-1] == '\r'
			|| szScript[szScript.length()-1] == '\n')
			szScript[szScript.length()-1] = 0;
		m_npc2Script.insert( make_pair(npcID,szScript) );
	}

	csv.CloseFile();

	return TRUE;
}

//-----------------------------------------------------------------------------
// 加载任务寻路点对应地图的对应文件
//-----------------------------------------------------------------------------
BOOL QuestData::LoadPathfinding( LPCTSTR szFile )
{
	m_pathpoint2Map.clear();

	IFS* pIFS = (IFS*)(VOID*)TObjRef<VirtualFileSys>("VFS_System");

	CSVFile csv;
	if( !csv.OpenFile(pIFS, szFile) )
	{
		LOG(_T("寻路点地图对应文件打开失败,%s\r\n"), szFile);
		return FALSE;
	}

	csv.BeginReadLine();
	while( true )
	{
		const vector<tstring>& vec = csv.GetNextLine();
		if( vec.empty() )
			break;
		ASSERT( vec.size() > 1 );

		//十六进制:0x00000000
		DWORD npcID = _tcstoul(vec[0].c_str(),NULL,0);

		for( size_t i=1; i<vec.size(); i++ )
		{
			tstring szMap = vec[i];
			ASSERT(szMap.length()>0);
			//过滤掉结尾的回车换行符
			if( szMap[szMap.length()-1] == '\r'
				|| szMap[szMap.length()-1] == '\n')
				szMap[szMap.length()-1] = 0;
			m_pathpoint2Map.insert( make_pair(npcID,szMap) );
		}
	}

	csv.CloseFile();
	return TRUE;
}


//-----------------------------------------------------------------------------
// 加载大类型任务
//-----------------------------------------------------------------------------
BOOL QuestData::LoadQuestCategory( LPCTSTR szFile )
{
	std::list<tstring> QuestCategoryList;
	TObjRef<VarContainer> VarQuestCategory = CreateObj("QuestCategory", "VarContainer");
	if (!VarQuestCategory->Load("VFS_System", szFile, "id", &QuestCategoryList))
	{
		IMSG(_T("Load file RCategory.xml failed\r\n"));
		return FALSE;
	}
	
	std::list<tstring>::iterator end = QuestCategoryList.end();
	for (std::list<tstring>::iterator iter = QuestCategoryList.begin(); iter != end; ++iter)
	{
		INT16 n16ID = (INT16)VarQuestCategory->GetInt(_T("id"), iter->c_str(), 0);
		tstring strName = VarQuestCategory->GetString(_T("name"), iter->c_str(), _T(""));
		m_questcategoryname.insert(std::make_pair(n16ID, strName));
	}
	
	KillObj("QuestCategory");
	return TRUE;
}

//-----------------------------------------------------------------------------
// 加载任务类型(类型编号、名称、字体颜色)
//-----------------------------------------------------------------------------
BOOL QuestData::LoadQuestType( LPCTSTR szFile )
{
	std::list<tstring> QuestTypeList;
	std::list<tstring>::iterator iter;
	TObjRef<VarContainer> VarQuestType = CreateObj("QuestType", "VarContainer");
	if(!VarQuestType->Load("VFS_System", szFile, "id", &QuestTypeList))
	{
		IMSG(_T("Load file RType.xml failed\r\n"));
		return FALSE;
	}

	for(iter = QuestTypeList.begin(); iter != QuestTypeList.end(); ++iter)
	{
		INT16 n16ID		 = VarQuestType->GetInt(		_T("id"),		(*iter).c_str(), 0);
		tstring strName  = VarQuestType->GetString(	_T("name"),		(*iter).c_str(), _T(""));	
		tstring strColor = VarQuestType->GetString(	_T("color"),	(*iter).c_str(), _T("0xFFFFF7E0"));
		INT16 n16CategoryID = VarQuestType->GetInt(_T("category"), (*iter).c_str(), 0);
		DWORD dwColor	 = _tcstoul(strColor.c_str(), NULL, 16);
		m_questtype.insert(make_pair(n16ID, make_pair(strName, dwColor)));
		
		// 将任务类别归类到任务大类别里面去
		m_questcategory.insert(std::make_pair(n16CategoryID, n16ID));
		m_questtype2category.insert(std::make_pair(n16ID, n16CategoryID));
	}

	KillObj("QuestType");

	return TRUE;
}

//-----------------------------------------------------------------------------
// 默认过滤方案
//-----------------------------------------------------------------------------

BOOL QuestData::LoadQuestDefaultFilterConfig( LPCTSTR szFile )
{
	std::list<tstring> QuestDefFilterList;
	TObjRef<VarContainer> VarQuestDefFilter = CreateObj("QuestDefFilter", "VarContainer");
	if (!VarQuestDefFilter->Load("VFS_System", szFile, "id", &QuestDefFilterList))
	{
		IMSG(_T("Load file RDefaultCategory.xml failed\r\n"));
		return FALSE;
	}
	std::list<tstring>::iterator end = QuestDefFilterList.end();
	for (std::list<tstring>::iterator iter = QuestDefFilterList.begin(); iter != end; ++iter)
	{
		INT16 n16ID = VarQuestDefFilter->GetInt(_T("id"), iter->c_str(), 0);
		BOOL bConfig1 = (BOOL)VarQuestDefFilter->GetInt(_T("config1"), iter->c_str(), 0);
		BOOL bConfig2 = (BOOL)VarQuestDefFilter->GetInt(_T("config2"), iter->c_str(), 0);
		m_deffilter1.insert(std::make_pair(n16ID, bConfig1));
		m_deffilter2.insert(std::make_pair(n16ID, bConfig2));
	}
	
	// 自定义配置文件初始化时和默认配置相同
	QuestFilterConfigMap::iterator iter1 = m_deffilter1.begin();
	QuestFilterConfigMap::iterator end1 = m_deffilter1.end();
	for (; iter1 != end1; ++iter1)
	{
		m_customfilter1[iter1->first] = iter1->second;
	}
	
	QuestFilterConfigMap::iterator iter2 = m_deffilter2.begin();
	QuestFilterConfigMap::iterator end2 = m_deffilter2.end();
	for (; iter2 != end2; ++iter2)
	{
		m_customfilter2[iter2->first] = iter2->second;
	}
	

	// 不过滤的配置
	for (int i = 1; i <= 10; ++i)
	{
		m_nofilter[i] = TRUE;
	}

	KillObj("QuestDefFilter");
	return TRUE;
}


//-----------------------------------------------------------------------------
// 根据任务ID获得任务类型
//-----------------------------------------------------------------------------
BOOL QuestData::GetQuestType( UINT16 questID,INT16& type ) const
{
	QuestsMap::const_iterator iter = m_quests.find(questID);
	if( iter != m_quests.end() )
	{
		const tagQuestProto& quest = iter->second;
		type = quest.type;

		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 根据任务类型编号获得任务类型名称
//-----------------------------------------------------------------------------
BOOL QuestData::GetQuestTypeName( INT16 type, tstring& name ) const
{
	QuestTypeMap::const_iterator iter = m_questtype.find(type);
	if( iter != m_questtype.end() )
	{
		name = iter->second.first;

		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 根据任务ID获得任务名称
//-----------------------------------------------------------------------------
BOOL QuestData::GetQuestName( UINT16 questID,tstring& name ) const
{
	QuestsMap::const_iterator iter = m_quests.find(questID);
	if( iter != m_quests.end() )
	{
		const tagQuestProto& quest = iter->second;
		name = quest.name;

		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 根据任务ID获得任务描述
//-----------------------------------------------------------------------------
BOOL QuestData::GetQuestDesc( UINT16 questID,tstring& desc ) const
{
	QuestsMap::const_iterator iter = m_quests.find(questID);
	if( iter != m_quests.end() )
	{
		const tagQuestProto& quest = iter->second;
		desc = quest.over_view;

		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 根据任务ID获得任务奖励金钱
//-----------------------------------------------------------------------------
BOOL QuestData::GetQuestRewMoney( UINT16 questID,DWORD& money ) const
{
	QuestsMap::const_iterator iter = m_quests.find(questID);
	if( iter != m_quests.end() )
	{
		const tagQuestProto& quest = iter->second;
		money = quest.rew_money;

		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 根据任务ID获得任务奖励经验
//-----------------------------------------------------------------------------
BOOL QuestData::GetQuestRewXP( UINT16 questID,DWORD& xp ) const
{
	QuestsMap::const_iterator iter = m_quests.find(questID);
	if( iter != m_quests.end() )
	{
		const tagQuestProto& quest = iter->second;
		xp = quest.rew_xp;

		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 根据任务ID获得任务需要对话的NPCID
//-----------------------------------------------------------------------------
DWORD QuestData::GetCompleteReqNPC( UINT16 questID,int index ) const
{
	const tagQuestDynamicTarget *pDynamicTarget = GetQuestDynamicTarget(questID);
	if( P_VALID(pDynamicTarget) )//动态
	{
		if( index >= DYNAMIC_TARGET_COUNT )
		{
			return GT_INVALID;
		}

		return pDynamicTarget->dwTargetID[index];
	}
	else
	{
		if( index >= QUEST_NPC_COUNT )
		{
			return GT_INVALID;
		}

		QuestsMap::const_iterator iter = m_quests.find(questID);
		if( iter != m_quests.end() )
		{
			const tagQuestProto& quest = iter->second;

			return quest.complete_req_npc[index];
		}
	}

	return GT_INVALID;
}

//-----------------------------------------------------------------------------
// 根据任务ID获得任务是否自动完成属性
//-----------------------------------------------------------------------------
BOOL QuestData::GetQuestAutoComplete( UINT16 questID ) const
{
	QuestsMap::const_iterator iter = m_quests.find(questID);
	if( iter != m_quests.end() )
	{
		const tagQuestProto& quest = iter->second;		
		return (quest.complete_quest_npc[0] == 0);
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 获得NPC的可交任务列表
//-----------------------------------------------------------------------------
VOID QuestData::GetNPCAcceptQuests( DWORD npcID,vector<UINT16>& quests ) const
{
	quests.clear();

	NPCQuestMultiMap::const_iterator begin = m_npc2AcceptQuests.lower_bound(npcID);
	NPCQuestMultiMap::const_iterator end = m_npc2AcceptQuests.upper_bound(npcID);
	NPCQuestMultiMap::const_iterator iter;
	for( iter = begin; iter != end; ++iter )
	{
		quests.push_back(iter->second);
	}
}

//-----------------------------------------------------------------------------
// 获得NPC的可接任务列表
//-----------------------------------------------------------------------------
VOID QuestData::GetNPCCompleteQuests(DWORD npcID,vector<UINT16>& quests) const
{
	quests.clear();

	NPCQuestMultiMap::const_iterator begin = m_npc2CompleteQuests.lower_bound(npcID);
	NPCQuestMultiMap::const_iterator end = m_npc2CompleteQuests.upper_bound(npcID);
	NPCQuestMultiMap::const_iterator iter;
	for( iter = begin; iter != end; ++iter )
	{
		quests.push_back(iter->second);
	}
}

//-----------------------------------------------------------------------------
// 获得NPC的可更新任务列表
//-----------------------------------------------------------------------------
VOID QuestData::GetNPCUpdateQuests(DWORD npcID,vector<UINT16>& quests) const
{
	quests.clear();

	NPCQuestMultiMap::const_iterator begin = m_npc2UpdateQuests.lower_bound(npcID);
	NPCQuestMultiMap::const_iterator end = m_npc2UpdateQuests.upper_bound(npcID);
	NPCQuestMultiMap::const_iterator iter;
	for( iter = begin; iter != end; ++iter )
	{
		quests.push_back(iter->second);
	}
}

//-----------------------------------------------------------------------------
// 获得NPC的脚本
//-----------------------------------------------------------------------------
BOOL QuestData::GetNPCScriptFile( DWORD npcID,tstring& script ) const
{
	NPCScriptMap::const_iterator iter = m_npc2Script.find(npcID);
	if( iter != m_npc2Script.end() )
	{
		TCHAR szTmp[255];
		_stprintf(szTmp, _T("%s\\script\\%s.lua"), vEngine::g_strLocalPath.c_str(), iter->second.c_str() );
		script = szTmp;

		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 在指定目录查找指定后缀名的文件（搜索子目录、忽略大小写）
//-----------------------------------------------------------------------------
VOID QuestData::FindFilesInDir( LPCTSTR szDir, LPCTSTR szExt, vector<tstring>& findFiles )
{
	if ( szDir == NULL )
	{
		return;
	}

	WIN32_FIND_DATA fd;
	ZeroMemory( &fd, sizeof(WIN32_FIND_DATA) );

	HANDLE hFind = INVALID_HANDLE_VALUE;
	tstring strFilePathName;
	tstring strTmpPath;
	DWORD dwError=0;

	strFilePathName = szDir;

	strFilePathName += _T("\\*");

	hFind = ::FindFirstFile( strFilePathName.c_str(), &fd );

	do
	{
		//. 和 ..
		if ( _tcscmp(fd.cFileName, _T(".")) == 0 || _tcscmp(fd.cFileName, _T("..")) == 0 )
		{
			continue;
		}

		//目录
		if ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			strTmpPath = szDir;
			strTmpPath += _T("\\");
			strTmpPath +=fd.cFileName;
			//递归
			FindFilesInDir( strTmpPath.c_str(), szExt, findFiles );
		}
		//文件
		else
		{
			//加上文件路径全称
			tstring path = szDir;
			path += _T("\\");
			path += fd.cFileName;
			tstring::size_type pos = path.rfind(_T("."));
			if (pos != tstring::npos)
			{
				if ( _tcsicmp(path.substr(pos+1).c_str(), szExt) == 0 )
				{
					findFiles.push_back( path.c_str() );
				}
			}
		}

	}while( ::FindNextFile( hFind, &fd ) != 0 );
}

//-----------------------------------------------------------------------------
// 从XML中加载任务
//-----------------------------------------------------------------------------
BOOL QuestData::LoadQuestFromXml( tagQuestProto* pQuest, LPCTSTR szFileName )
{
	if (!pQuest) return FALSE;

	if (!szFileName) return FALSE;

	//加载XML
	XmlDocument doc;
	if (doc.LoadFile("VFS_System", szFileName) == false) return FALSE;

	XmlHandle hDoc(&doc);
	XmlHandle hRoot(0);

	//是否是任务文件
	XmlElement *pElem = hDoc.FirstChildElement("quest").Element();
	if (!pElem) return FALSE;

	hRoot = XmlHandle( pElem );

	XmlNodeMap xmlNode;

	XmlElement* pPropertyNode = hRoot.FirstChild( "property" ).FirstChild().Element();
	XmlElement* pAcceptConditionsNode = hRoot.FirstChild( "accept-conditions" ).FirstChild().Element();
	XmlElement* pCompleteConditionsNode = hRoot.FirstChild( "complete-conditions" ).FirstChild().Element();
	XmlElement* pRewNode = hRoot.FirstChild( "reward" ).FirstChild().Element();
	LoadXmlNode(pPropertyNode, xmlNode);
	LoadXmlNode(pAcceptConditionsNode, xmlNode);
	LoadXmlNode(pCompleteConditionsNode, xmlNode);
	LoadXmlNode(pRewNode, xmlNode);

	SetQuestProto(pQuest, xmlNode);
	return TRUE;	
}

BOOL QuestData::LoadXmlNode(XmlElement* pNode, XmlNodeMap& xmlNodeMap)
{
	//是否存在该节点
	if ( !pNode ) 
		return FALSE;

	const char* pChar = NULL;
	XmlAttribute* pAttrib = NULL;
	while( pNode )
	{
		pAttrib = pNode->FirstAttribute();
		if( !pAttrib )
		{
			pNode =  pNode->NextSiblingElement();
			continue;
		}

		string firstName = pAttrib->Name();
		while(pAttrib)
		{
			if( !pAttrib->Previous() )
			{
				xmlNodeMap.insert(make_pair(firstName, pAttrib->ValueStr()));
			}
			else
			{
				string tmp = firstName;
				tmp += "-";
				tmp += pAttrib->Name();
				xmlNodeMap.insert( make_pair(tmp, pAttrib->ValueStr()));
			}

			pAttrib = pAttrib->Next();
		}

		pNode =  pNode->NextSiblingElement();
	}
	return TRUE;
}


BOOL QuestData::SetQuestProto(tagQuestProto* pQuestProto, XmlNodeMap& xmlNodeMap)
{
	//property
	SetQuestUINT16("ID", pQuestProto->id, xmlNodeMap);
	SetQuestUINT16("PrevQuestID", pQuestProto->prev_quest_id, xmlNodeMap );
	SetQuestUINT16("PrevQuest2ID", pQuestProto->prev_quest2_id, xmlNodeMap );
	SetQuestINT32("PrevQuestRelation", pQuestProto->prev_quest_relation, xmlNodeMap);
	SetQuestUINT16("NextQuestID", pQuestProto->next_quest_id, xmlNodeMap );
	SetQuestUINT16("UncmpleteQuestID", pQuestProto->uncomplete_quest_id, xmlNodeMap);
	SetQuestBool("AutoAddQuest", pQuestProto->auto_add_quest, xmlNodeMap );

	SetQuestDwordFlags("QuestPlayerAccept", pQuestProto->accept_flags, Quest_Accept_Flag_Player, xmlNodeMap );
	SetQuestDwordFlags("QuestSystemRelease", pQuestProto->accept_flags, Quest_Accept_Flag_System, xmlNodeMap );
	SetQuestDwordFlags("QuestItemAccept", pQuestProto->accept_flags, Quest_Accept_Flag_Item, xmlNodeMap );

	SetQuestDword("QuestType", (DWORD&)pQuestProto->type, xmlNodeMap );
	SetQuestINT32("TargetMode", (INT32&)pQuestProto->target_mode, xmlNodeMap );
	SetQuestBYTE("SuggestedPlayers", pQuestProto->suggested_players, xmlNodeMap );
	SetQuestDword("LimitTime", pQuestProto->limit_time, xmlNodeMap );
	SetQuestBYTE("DoQuestTimes", pQuestProto->accept_times, xmlNodeMap );
	SetQuestBool("Periodic", pQuestProto->period, xmlNodeMap );
	SetQuestDword("QuestPeriodic", (DWORD&)pQuestProto->period_type, xmlNodeMap );
	SetQuestDword("Week", (DWORD&)pQuestProto->week, xmlNodeMap );
	SetQuestBool("Repeatable", pQuestProto->repeatable, xmlNodeMap );
	SetQuestBool("ShowQuestDialog", pQuestProto->show_quest_dialog, xmlNodeMap );
	SetQuestBool("NoDelete", pQuestProto->no_delete, xmlNodeMap );
	SetQuestFloat("Destination_X", pQuestProto->destination_x, xmlNodeMap);
	SetQuestFloat("Destination_Y", pQuestProto->destination_y, xmlNodeMap);
	SetQuestFloat("Destination_Z", pQuestProto->destination_z, xmlNodeMap);

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

	for( int i = 0; i < 6 ; ++i )
	{
		stringstream ss;
		ss << "AcceptQuestNPCID" << i + 1;
		SetQuestDword(ss.str().c_str(), pQuestProto->accept_quest_npc[i], xmlNodeMap );
	}

	for( int i = 0; i < 6 ; ++i )
	{
		stringstream ss;
		ss << "CompleteQuestNPCID" << i + 1;
		SetQuestDword(ss.str().c_str(), pQuestProto->complete_quest_npc[i], xmlNodeMap );
	}

	for( int i = 0; i < QUEST_ITEMS_COUNT ; ++i )
	{
		stringstream ss;
		ss << "InvestigateObjectID" << i + 1;
		SetQuestDword(ss.str().c_str(), pQuestProto->investigate_objects[i], xmlNodeMap );
	}

	SetQuestDword("SrcItemID", pQuestProto->src_item, xmlNodeMap );
	SetQuestINT16("SrcItemID-value", pQuestProto->src_item_num, xmlNodeMap );

	//accept-conditions
	SetQuestINT32("GetQuestRequiredMaxLevel", pQuestProto->accept_req_max_level, xmlNodeMap);
	SetQuestINT32("GetQuestRequiredMinLevel", pQuestProto->accept_req_min_level, xmlNodeMap);
	for( int i = 0; i < QUEST_REPUTATIONS_COUNT ; ++i )
	{
		stringstream ss;
		ss << "GetQuestRequiredRep" << i + 1;
		SetQuestINT32(ss.str().c_str(), pQuestProto->accept_req_rep[i], xmlNodeMap );
		string tmp = ss.str();
		tmp += "-max";
		SetQuestINT32(tmp, pQuestProto->accept_req_max_rep[i], xmlNodeMap );
		tmp = ss.str();
		tmp += "-min";
		SetQuestINT32(tmp, pQuestProto->accept_req_min_rep[i], xmlNodeMap );
	}
	for( int i = 0; i < QUEST_SKILLS_COUNT ; ++i )
	{
		stringstream ss;
		ss << "GetQuestRequiredSkill" << i + 1;
		SetQuestDword(ss.str().c_str(), pQuestProto->accept_req_skill[i], xmlNodeMap );
		string tmp = ss.str();
		tmp += "-value";
		SetQuestINT32(tmp, pQuestProto->accept_req_skill_val[i], xmlNodeMap );
	}
	SetQuestBYTE("GetQuestRequiredSex", pQuestProto->sex, xmlNodeMap);
	SetQuestDword("GetQuestRequiredClass", (DWORD&)pQuestProto->class_type, xmlNodeMap);
	SetQuestDword("GetQuestRequiredMapID", pQuestProto->accept_req_map, xmlNodeMap);
	for( int i = 0; i < QUEST_ITEMS_COUNT ; ++i )
	{
		stringstream ss;
		ss << "GetQuestRequiredItemID" << i + 1;
		SetQuestDword(ss.str().c_str(), pQuestProto->accept_req_item[i], xmlNodeMap );
		string tmp = ss.str();
		tmp += "-value";
		SetQuestINT16(tmp, pQuestProto->accept_req_item_num[i], xmlNodeMap );
	}
	for( int i = 0; i < QUEST_ATTS_COUNT ; ++i )
	{
		stringstream ss;
		ss << "GetQuestRequiredAtt" << i + 1;
		SetQuestINT32(ss.str().c_str(), pQuestProto->accept_req_att[i], xmlNodeMap );
		string tmp = ss.str();
		tmp += "-value";
		SetQuestINT16(tmp, pQuestProto->accept_req_att_val[i], xmlNodeMap );
	}
	SetQuestBool("DelRestrictedItem", pQuestProto->del_req_item, xmlNodeMap);
	for( int i = 0; i < QUEST_TRIGGERS_COUNT ; ++i )
	{
		stringstream ss;
		ss << "GetQuestReqTriggersID" << i + 1;
		SetQuestDword(ss.str().c_str(), pQuestProto->accept_req_trriger[i], xmlNodeMap );
	}
	SetQuestBool("GetQuestReqMarried", pQuestProto->married, xmlNodeMap);
	SetQuestBYTE("GetQuestReqLoverNum", pQuestProto->lover_num, xmlNodeMap);
	SetQuestINT32("GetQuestReqMoneyNum", pQuestProto->accept_req_money, xmlNodeMap );
	SetQuestBool("GetQuestDelMoneyNum", pQuestProto->accept_del_money, xmlNodeMap);

	//complete-conditions
	for( int i = 0; i < QUEST_ITEMS_COUNT; ++i )
	{
		stringstream ss;
		ss << "CompleteQuestRequiredItemID" << i + 1;
		SetQuestDword(ss.str().c_str(), pQuestProto->complete_req_item[i], xmlNodeMap );
		string tmp = ss.str();
		tmp += "-value";
		SetQuestINT16(tmp, pQuestProto->complete_req_item_num[i], xmlNodeMap );
	}
	SetQuestBool("RequiredOnlyOneItem", pQuestProto->only_one_item, xmlNodeMap);
	SetQuestBool("DeleteItem", pQuestProto->delete_item, xmlNodeMap);

	for( int i = 0; i < QUEST_CREATURES_COUNT; ++i )
	{
		stringstream ss;
		ss << "CompleteQuestRequiredCreatureID" << i + 1;
		SetQuestDword(ss.str().c_str(), pQuestProto->complete_req_creature[i*3], xmlNodeMap );
		string tmp = ss.str();
		tmp += "-id2";
		SetQuestDword(tmp, pQuestProto->complete_req_creature[i*3+1], xmlNodeMap );
		tmp = ss.str();
		tmp += "-id3";
		SetQuestDword(tmp, pQuestProto->complete_req_creature[i*3+2], xmlNodeMap );
		tmp = ss.str();
		tmp += "-value";
		SetQuestINT16(tmp, pQuestProto->complete_req_creature_num[i], xmlNodeMap );
	}

	SetQuestBool("RequiredCreatureLevel", pQuestProto->creature_level, xmlNodeMap);
	SetQuestBool("RequiredOnlyOneCreature", pQuestProto->only_one_creature, xmlNodeMap);

	for( int i = 0; i < QUEST_NPC_COUNT ; ++i )
	{
		stringstream ss;
		ss << "CompleteQuestRequiredNPC" << i + 1;
		SetQuestDword(ss.str().c_str(), pQuestProto->complete_req_npc[i], xmlNodeMap );
	}

	SetQuestBool("RequiredOnlyInOrder", pQuestProto->only_in_order, xmlNodeMap);
	SetQuestBool("RequiredOnlyOneNPC", pQuestProto->only_one_npc, xmlNodeMap);

	for( int i = 0; i < QUEST_TRIGGERS_COUNT; ++i )
	{
		stringstream ss;
		ss << "CompleteQuestReqTriggersID" << i + 1;
		SetQuestDword(ss.str().c_str(), pQuestProto->complete_req_trigger[i], xmlNodeMap );
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
		ss << "RewRequiredRep" << i + 1;
		SetQuestDword(ss.str().c_str(), pQuestProto->rew_rep[i], xmlNodeMap );
		string tmp = ss.str();
		tmp += "-value";
		SetQuestINT32(tmp, pQuestProto->rew_rep_val[i], xmlNodeMap );
	}	

	for( int i = 0; i < QUEST_REW_ITEM; ++i )
	{
		stringstream ss;
		ss << "RewItemID" << i + 1;
		SetQuestDword(ss.str().c_str(), pQuestProto->rew_item[i], xmlNodeMap );
		string tmp = ss.str();
		tmp += "-value";
		SetQuestINT16(tmp, pQuestProto->rew_item_num[i], xmlNodeMap );
		tmp = ss.str();
		tmp += "-quality";
		SetQuestINT16(tmp, pQuestProto->rew_item_quality[i], xmlNodeMap );
	}

	for( int i = 0; i < QUEST_REW_ITEM; ++i )
	{
		stringstream ss;
		ss << "RewChoicesItemID" << i + 1;
		SetQuestDword(ss.str().c_str(), pQuestProto->rew_choice_item[i], xmlNodeMap );
		string tmp = ss.str();
		tmp += "-value";
		SetQuestINT16(tmp, pQuestProto->rew_choice_item_num[i], xmlNodeMap );
		tmp = ss.str();
		tmp += "-quality";
		SetQuestINT16(tmp, pQuestProto->rew_choice_quality[i], xmlNodeMap );
	}

	for( int i = 0; i < QUEST_ATTS_COUNT; ++i )
	{
		stringstream ss;
		ss << "RewAtt" << i + 1;
		SetQuestINT32(ss.str().c_str(), pQuestProto->rew_att[i], xmlNodeMap );
		string tmp = ss.str();
		tmp += "-value";
		SetQuestINT16(tmp, pQuestProto->rew_att_val[i], xmlNodeMap );
	}

	for( int i = 0; i < QUEST_SKILLS_COUNT; ++i )
	{
		stringstream ss;
		ss << "RewSkill" << i + 1;
		SetQuestDword(ss.str().c_str(), pQuestProto->rew_skill[i], xmlNodeMap );
		string tmp = ss.str();
		tmp += "-value";
		SetQuestINT32(tmp, pQuestProto->rew_skill_val[i], xmlNodeMap );
	}

	for( int i = 0; i < QUEST_REW_BUFF; ++i )
	{
		stringstream ss;
		ss << "RewBuff" << i + 1l;
		SetQuestDword(ss.str().c_str(), pQuestProto->rew_buff[i], xmlNodeMap );
	}
	return TRUE;
}
// ----------------------------------------------------------------------------
// string ----> DWORD
// ----------------------------------------------------------------------------
void  QuestData::SetQuestDword(std::string str, DWORD& dwQuest, XmlNodeMap& xmlNodeMap)
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
void  QuestData::SetQuestDwordFlags(std::string str, DWORD& dwQuest, INT32 nMask, XmlNodeMap& xmlNodeMap)
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
void  QuestData::SetQuestUINT16(std::string str, UINT16& unQuest, XmlNodeMap& xmlNodeMap)
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
void  QuestData::SetQuestBYTE(std::string str, BYTE& byQuest, XmlNodeMap& xmlNodeMap)
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
void  QuestData::SetQuestINT16(std::string str, INT16& n16Quest, XmlNodeMap& xmlNodeMap)
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
void  QuestData::SetQuestINT32(std::string str, INT32& nQuest, XmlNodeMap& xmlNodeMap)
{
	XmlNodeMap::iterator pos = xmlNodeMap.find(str);
	if ( pos != xmlNodeMap.end() )
	{
		nQuest = atoi(pos->second.c_str());
		//xmlNodeMap.erase(pos);
	}
}

// ----------------------------------------------------------------------------
// string ----> BOOL
// ----------------------------------------------------------------------------
void  QuestData::SetQuestBool(std::string str, BOOL& bQuest, XmlNodeMap& xmlNodeMap)
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
void  QuestData::SetQuestFloat(std::string str, FLOAT& fQuest, XmlNodeMap& xmlNodeMap)
{
	XmlNodeMap::iterator pos = xmlNodeMap.find(str);
	if ( pos != xmlNodeMap.end() )
	{
		fQuest = (FLOAT)atof(pos->second.c_str());
		xmlNodeMap.erase(pos);
	}
}


const tagQuestProto* QuestData::GetQuestProto( UINT16 questID ) const
{
	QuestsMap::const_iterator iter = m_quests.find(questID);
	if( iter != m_quests.end() )
	{
		return (&iter->second);
	}

	return NULL;
}

const tagQuestDynamicTarget* QuestData::GetQuestDynamicTarget( UINT16 questID ) const
{
	QuestDynamicTargetMap::const_iterator iter = m_questDynamicTarget.find(questID);
	if( iter != m_questDynamicTarget.end() )
	{
		return (&iter->second);
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// 获得某个任务需要获得某个物品的数量
//-----------------------------------------------------------------------------
BOOL QuestData::GetQuestRequiredItemCount( UINT16 questID,DWORD num,DWORD& id,UINT16& count ) const
{
	const tagQuestDynamicTarget *pDynamicTarget = GetQuestDynamicTarget(questID);
	if( P_VALID(pDynamicTarget) )//动态
	{
		if( num >= DYNAMIC_TARGET_COUNT )
			return FALSE;

		if( pDynamicTarget->eTargetType == EQTT_Collect )
		{
			id = pDynamicTarget->dwTargetID[num];
			if( id == 0 )
				return FALSE;

			count = pDynamicTarget->nTargetNum[num];

			return TRUE;
		}
	}
	else
	{
		if( num >= QUEST_ITEMS_COUNT )
			return FALSE;

		QuestsMap::const_iterator iter = m_quests.find(questID);
		if( iter != m_quests.end() )
		{
			const tagQuestProto& quest = iter->second;
			id = quest.complete_req_item[num];
			if( id == 0 )
				return FALSE;

			count = quest.complete_req_item_num[num];

			return TRUE;
		}
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 获得某个任务需要击杀某个怪物的数量
//-----------------------------------------------------------------------------
BOOL QuestData::GetQuestRequiredCreatureCount( UINT16 questID,DWORD num,DWORD& id,UINT16& count ) const
{
	const tagQuestDynamicTarget *pDynamicTarget = GetQuestDynamicTarget(questID);
	if( P_VALID(pDynamicTarget) )//动态
	{
		if( num >= DYNAMIC_TARGET_COUNT )
			return FALSE;

		if( pDynamicTarget->eTargetType == EQTT_Kill )
		{
			id = pDynamicTarget->dwTargetID[num];
			if( id == 0 )
				return FALSE;

			count = pDynamicTarget->nTargetNum[num];

			return TRUE;
		}
	}
	else
	{
		if( num >= QUEST_CREATURES_COUNT )
			return FALSE;

		QuestsMap::const_iterator iter = m_quests.find(questID);
		if( iter != m_quests.end() )
		{
			const tagQuestProto& quest = iter->second;
			DWORD id1 = quest.complete_req_creature[num*3];
			DWORD id2 = quest.complete_req_creature[num*3+1];
			DWORD id3 = quest.complete_req_creature[num*3+2];
			if( id1 != 0 )
				id = id1;
			else if( id2 != 0 )
				id = id2;
			else if( id3 != 0 )
				id = id3;
			else
				return FALSE;

			count = quest.complete_req_creature_num[num];

			return TRUE;
		}
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 任务是否需要击杀怪物或收集物品
//-----------------------------------------------------------------------------
BOOL QuestData::QuestRequireItemOrCreature( UINT16 questID ) const
{
	const tagQuestDynamicTarget *pDynamicTarget = GetQuestDynamicTarget(questID);
	if( P_VALID(pDynamicTarget) )//动态
	{
		switch( pDynamicTarget->eTargetType )
		{
		case EQTT_Kill:
		case EQTT_Collect:
			{
				for( int i=0; i<DYNAMIC_TARGET_COUNT; i++ )
				{
					if( pDynamicTarget->dwTargetID[i] > 0 )
						return TRUE;
				}
			}
			break;
		}
	}
	else
	{
		QuestsMap::const_iterator iter = m_quests.find(questID);
		if( iter != m_quests.end() )
		{
			const tagQuestProto& quest = iter->second;

			for( int itemi=0; itemi<QUEST_ITEMS_COUNT; ++itemi )
			{
				if( quest.complete_req_item[itemi] > 0 )
					return TRUE;
			}

			for( int creaturei=0; creaturei<QUEST_CREATURES_COUNT; ++creaturei )
			{
				if( quest.complete_req_creature[creaturei] > 0 )
					return TRUE;
			}
		}
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 任务是否可被删除
//-----------------------------------------------------------------------------
BOOL QuestData::QuestCanDelete( UINT16 questID ) const
{
	QuestsMap::const_iterator iter = m_quests.find(questID);
	if( iter != m_quests.end() )
	{
		const tagQuestProto& quest = iter->second;
		return !quest.no_delete;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 是否立即弹出任务界面
//-----------------------------------------------------------------------------
BOOL QuestData::ShowQuestAtOnce( UINT16 questID ) const
{
	QuestsMap::const_iterator iter = m_quests.find(questID);
	if( iter != m_quests.end() )
	{
		const tagQuestProto& quest = iter->second;
		return quest.show_quest_dialog;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 是否自动添加下一个任务
//-----------------------------------------------------------------------------
BOOL QuestData::QuestAddNext( UINT16 questID ) const
{
	QuestsMap::const_iterator iter = m_quests.find(questID);
	if( iter != m_quests.end() )
	{
		const tagQuestProto& quest = iter->second;
		return quest.auto_add_quest;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 获得后续任务ID
//-----------------------------------------------------------------------------
BOOL QuestData::GetNextQuest( UINT16 questID,UINT16& nextID ) const
{
	QuestsMap::const_iterator iter = m_quests.find(questID);
	if( iter != m_quests.end() )
	{
		const tagQuestProto& quest = iter->second;
		nextID = quest.next_quest_id;

		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 获得目的地坐标
//-----------------------------------------------------------------------------
BOOL QuestData::GetQuestDestPos( UINT16 questID,Vector3& pos ) const
{
	QuestsMap::const_iterator iter = m_quests.find(questID);
	if( iter != m_quests.end() )
	{
		const tagQuestProto& quest = iter->second;
		pos.x = quest.destination_x;
		pos.y = quest.destination_y;
		pos.z = quest.destination_z;

		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 获得任务类型颜色
//-----------------------------------------------------------------------------
BOOL QuestData::GetQuestTypeColor( UINT16 questID,DWORD& color ) const
{
	QuestsMap::const_iterator iter = m_quests.find(questID);
	if( iter != m_quests.end() )
	{
		const tagQuestProto& quest = iter->second;

		color = 0xFFFFF7E0;
		/*switch( quest.type )
		{
		case EQT_Beginer:	color = RGB(255,247,224);	break;
		case EQT_Produce:	color = RGB(255,157,51);	break;
		case EQT_System:	color = RGB(246,235,120);	break;
		case EQT_Gut:		color = RGB(180,250,88);	break;
		case EQT_Profession:color = RGB(255,124,232);	break;
		case EQT_Lovers:	color = RGB(254,165,160);	break;
		case EQT_Guild:		color = RGB(71,187,255);	break;
		case EQT_Legend:	color = RGB(62,108,211);	break;
		case EQT_Wabao:		color = RGB(62,108,211);	break;
		case EQT_Spouse:	color = RGB(194,51,51);		break;
		case EQT_Challenge:	color = RGB(255,124,232);	break;
		}*/
		QuestTypeMap::const_iterator iter = m_questtype.find(quest.type);
		if( iter != m_questtype.end() )
		{
			color = iter->second.second;
		}

		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 获得任务等级颜色
//-----------------------------------------------------------------------------
DWORD QuestData::GetQuestLevelColor( UINT16 questID, INT roleLvl ) const
{
	DWORD color = 0xFFFFFFFF;

	QuestsMap::const_iterator iter = m_quests.find(questID);
	if( iter != m_quests.end() )
	{
		const tagQuestProto& quest = iter->second;
		INT questLvl = (INT)quest.destination_x;
		if( questLvl == 0 )
			return color;

		if( questLvl >= roleLvl-1 && questLvl <= roleLvl+1 )//白色
		{
			color = 0xFFFFFFFF;
		}
		else if( questLvl < roleLvl-1 && questLvl > roleLvl-5 )//绿色
		{
			color = 0xFF51DC3E;
		}
		else if( questLvl <= roleLvl-5 )//灰色
		{
			color = 0xFFAEAEAE;
		}
		else if( questLvl > roleLvl+1 && questLvl < roleLvl+5 )//黄色
		{
			color = 0xFFFFE400;
		}
		else if( questLvl >= roleLvl+5 )//红色
		{
			color = 0xFFFF4200;
		}
	}

	return color;
}

//-----------------------------------------------------------------------------
// 任务发布者是否给任务物品
//-----------------------------------------------------------------------------
INT QuestData::GetQuestSrcItem( UINT16 questID ) const
{
	QuestsMap::const_iterator iter = m_quests.find(questID);
	if( iter != m_quests.end() )
	{
		const tagQuestProto& quest = iter->second;

		return (quest.src_item != 0) ? 1 : 0;
	}

	return 0;
}

INT QuestData::GetQuestLevel( UINT16 questID ) const
{
	QuestsMap::const_iterator iter = m_quests.find(questID);
	if( iter != m_quests.end() )
	{
		const tagQuestProto& quest = iter->second;

		return (INT)quest.destination_x;
	}

	return 0;
}

VOID QuestData::SetQuestDesc( UINT16 questID, LPCTSTR szDesc )
{
	QuestsMap::iterator iter = m_quests.find(questID);
	if( iter != m_quests.end() )
	{
		tagQuestProto& quest = iter->second;
		_tcscpy_s( quest.over_view, QUEST_OVERVIEW, szDesc );
	}
}



VOID QuestData::SetQuestFilter( EFilter eFilter, INT16 n16Category, BOOL bSet )
{
	switch (eFilter)
	{
	case EF_CustomFilter1:
		{
			m_customfilter1[n16Category] = bSet;
		} break;
	case EF_CustomFilter2:
		{
			m_customfilter2[n16Category] = bSet;
		} break;
	default: break;
	}
}



VOID QuestData::SetCurFilter( EFilter eFilter )
{
	m_eCurFilter = eFilter;
}


VOID QuestData::SetLastFilter( EFilter eFilter )
{
	m_eLastFilter = eFilter;
}


QuestData::EFilter QuestData::GetLastFilter() const
{
	return m_eLastFilter;
}


QuestData::EFilter QuestData::GetCurFilter() const
{
	return m_eCurFilter;
}

BOOL QuestData::IsFiltered(INT16 n16Category)
{
	switch (m_eCurFilter)
	{
	case EF_DefFilter1:
		{
			QuestFilterConfigMap::iterator iter = m_deffilter1.find(n16Category);
			if (iter != m_deffilter1.end())
				return iter->second;
		} break;
	case EF_DefFilter2:
		{
			QuestFilterConfigMap::iterator iter = m_deffilter2.find(n16Category);
			if (iter != m_deffilter2.end())
				return iter->second;

		} break;
	case EF_CustomFilter1:
		{
			QuestFilterConfigMap::iterator iter = m_customfilter1.find(n16Category);
			if (iter != m_customfilter1.end())
				return iter->second;
		} break;
	case EF_CustomFilter2:
		{
			QuestFilterConfigMap::iterator iter = m_customfilter2.find(n16Category);
			if (iter != m_customfilter2.end())
				return iter->second;
		} break;
	case EF_NoFilter: break;
	default: break;
	}
	return TRUE;
}

INT16 QuestData::GetQuestCategoryByQuestType( INT16 n16RType )
{
	QuestTypeToCategoryMap::iterator iter = m_questtype2category.find(n16RType);
	if (iter != m_questtype2category.end())
		return iter->second;
	return 0;
}


INT16 QuestData::GetQuestCategoryByQuestID( INT16 n16QuestID )
{
	// 取得任务的TYPE
	INT16 n16Type;
	GetQuestType(n16QuestID, n16Type);
	INT16 n16Category = GetQuestCategoryByQuestType(n16Type);
	return n16Category;
}


const QuestData::QuestFilterConfigMap& QuestData::GetFilter( QuestData::EFilter eFilter ) const
{
	switch (eFilter)
	{
	case EF_DefFilter1:
		{
			return m_deffilter1;
		} 
	case EF_DefFilter2:
		{
			return m_deffilter2;	
		} 
	case EF_CustomFilter1:
		{
			return m_customfilter1;
		}
	case EF_CustomFilter2:
		{
			return m_customfilter2;
		} 
	case EF_NoFilter:
		{
			return m_nofilter;
		}
	}
	return m_nofilter;
}

const QuestData::QuestCategoryNameMap& QuestData::GetCategoryName() const
{
	return m_questcategoryname;
}

VOID QuestData::SetFilterByPlayerLevel( BOOL bSet )
{
	m_bFilterByLevel = bSet;
}

BOOL QuestData::IsFilterByPlayerLevel() const
{
	return m_bFilterByLevel;
}



BOOL QuestData::IsNPCHasQuestByCurrentFilter( DWORD dwNPCID )
{
	
	QuestQuery *pQQ = QuestMgr::Inst()->GetQuery();
	vector<UINT16> quests;
	pQQ->GetNPCAcceptQuests(dwNPCID, quests);
	
	BOOL bResult = FALSE;

	for (size_t i = 0; i < quests.size(); ++i)
	{
		INT16 n16CategoryID = GetQuestCategoryByQuestID(quests[i]);
		if (IsFiltered(n16CategoryID))
		{
			bResult = TRUE;
			break;
		}
	}
	return bResult;
}

BOOL QuestData::IsNPCHasQuestByCurrentFilterByPlayerLevel( DWORD dwNPCID )
{
	INT nLevel = RoleMgr::Inst()->GetLocalPlayer()->GetRoleLevel();

	QuestQuery *pQQ = QuestMgr::Inst()->GetQuery();
	vector<UINT16> quests;
	pQQ->GetNPCAcceptQuests(dwNPCID, quests);
	
	BOOL bResult = FALSE;

	for (size_t i = 0; i < quests.size(); ++i)
	{
		INT nQuestLevel = GetQuestLevel(quests[i]);
		if (nQuestLevel != 0)
		{
			if (nQuestLevel >= nLevel - 3)
			{
				bResult = TRUE;
				break;
			}
		}
		else
		{
			bResult = TRUE;
			break;
		}
	}
	return bResult;
}



//-----------------------------------------------------------------------------
// 某个任务中是否与指定次序的NPC对话
//-----------------------------------------------------------------------------
BOOL QuestData::NPCBeTalked( UINT16 questID,int index ) const
{
	if( index >= QUEST_NPC_COUNT )
	{
		return FALSE;
	}

	QuestStateMap::const_iterator iter = m_questStates.find( questID );
	if( iter != m_questStates.end() )
	{
		const tagQuestState& state = iter->second;

		return state.beTalked[index];
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 某个任务中是否触发某触发器
//-----------------------------------------------------------------------------
BOOL QuestData::BeTriggered( UINT16 questID,int index ) const
{
	if( index >= QUEST_TRIGGERS_COUNT )
	{
		return FALSE;
	}

	QuestStateMap::const_iterator iter = m_questStates.find( questID );
	if( iter != m_questStates.end() )
	{
		const tagQuestState& state = iter->second;

		return state.beTriggered[index];
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 加载触发器数据
//-----------------------------------------------------------------------------
BOOL QuestData::LoadTriggerData( LPCTSTR szPath )
{
	list<tstring> triggerList;
	list<tstring>::iterator iter;

	TObjRef<VarContainer> varTriggers = CreateObj( "TriggerProto", "VarContainer" );
	if(!varTriggers->Load("VFS_System", szPath, "name", &triggerList))
	{
		IMSG(_T("Load file trigger.xml failed\r\n"));
		return TRUE;
	}

	for(iter=triggerList.begin(); iter!=triggerList.end(); ++iter)
	{
		tagTriggerProto triggerProto;
		triggerProto.name	= varTriggers->GetString(_T("name"), (*iter).c_str(), _T(""));
		triggerProto.map	= varTriggers->GetString(_T("map"), (*iter).c_str(), _T(""));
		triggerProto.id		= varTriggers->GetInt(_T("id"), (*iter).c_str(), GT_INVALID);
		triggerProto.pos.x	= varTriggers->GetFloat(_T("x"), (*iter).c_str(), 0.0f);
		triggerProto.pos.y	= varTriggers->GetFloat(_T("y"), (*iter).c_str(), 0.0f);
		triggerProto.pos.z	= varTriggers->GetFloat(_T("z"), (*iter).c_str(), 0.0f);

		m_triggers.push_back(triggerProto);
	}
	
	KillObj("TriggerProto");
	return TRUE;
}

//-----------------------------------------------------------------------------
// 查找触发器位置
//-----------------------------------------------------------------------------
BOOL QuestData::GetTriggerPosition( LPCTSTR triggerName,Vector3& pos ) const
{
	tstring key = triggerName;

	for( int i=0; i<(int)m_triggers.size(); i++ )
	{
		const tagTriggerProto& triggerProto = m_triggers[i];
		if( triggerProto.name == key )
		{
			pos = triggerProto.pos;

			return TRUE;
		}
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 查找触发器名称
//-----------------------------------------------------------------------------
BOOL QuestData::GetTriggerName( DWORD id, tstring& name ) const
{
	for( int i=0; i<(int)m_triggers.size(); i++ )
	{
		const tagTriggerProto& triggerProto = m_triggers[i];
		if( triggerProto.id == id )
		{
			name = triggerProto.name;

			return TRUE;
		}
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 获得奖励物品品级
//-----------------------------------------------------------------------------
BOOL QuestData::GetQuestItemQuality( UINT16 questID,INT index,INT16& quality ) const
{
	if( index >= QUEST_REW_ITEM )
		return FALSE;

	QuestsMap::const_iterator iter = m_quests.find(questID);
	if( iter != m_quests.end() )
	{
		const tagQuestProto& quest = iter->second;
		quality = quest.rew_item_quality[index];

		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 获得可选奖励物品品级
//-----------------------------------------------------------------------------
BOOL QuestData::GetQuestChoiceItemQuality( UINT16 questID,INT index,INT16& quality ) const
{
	if( index >= QUEST_REW_ITEM )
		return FALSE;

	QuestsMap::const_iterator iter = m_quests.find(questID);
	if( iter != m_quests.end() )
	{
		const tagQuestProto& quest = iter->second;
		quality = quest.rew_choice_quality[index];

		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// 获得奖励物品数量（以种类统计）
//-----------------------------------------------------------------------------
INT QuestData::GetQuestRewItemNum( UINT16 questID ) const
{
	INT itemNum = 0;

	QuestsMap::const_iterator iter = m_quests.find(questID);
	if( iter != m_quests.end() )
	{
		const tagQuestProto& quest = iter->second;

		for( int i=0; i<QUEST_REW_ITEM; i++ )
		{
			if( quest.rew_item[i] != 0 )
			{
				itemNum++;
			}
			else
				break;
		}

		if( quest.rew_choice_item[0] != 0 )
			itemNum++;
	}

	return itemNum;
}

BOOL QuestData::GetPathPointMap( DWORD npcID,tstring& strMap ) const
{
	PathPointMultiMap::const_iterator iter = m_pathpoint2Map.find(npcID);
	if( iter != m_pathpoint2Map.end() )
	{
		strMap = iter->second;

		return TRUE;
	}

	return FALSE;
}

VOID QuestData::Reset( void )
{
	m_questDynamicTarget.clear();

	QuestStateMap::iterator iter;
	for( iter=m_questStates.begin(); iter!=m_questStates.end(); ++iter )
	{
		tagQuestState& state = iter->second;
		memset(&state,0,sizeof(tagQuestState));
	}
}



