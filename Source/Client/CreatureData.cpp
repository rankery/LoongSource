#include "StdAfx.h"
#include "CreatureData.h"
#include "..\WorldDefine\move_define.h"

enum ETagNode
{
	ETN_Start,
	ETN_Face = ETN_Start,	// 面部
	ETN_Hat,				// 头部
	ETN_Back,				// 背部
	ETN_RSld,				// 右肩
	ETN_LSld,				// 左肩
	ETN_RHd,				// 右手
	ETN_LHd,				// 左手
	ETN_Wep1, 				// 右腰间
	ETN_Wep2,				// 左腰间
	ETN_Wep3,				// 腰后1
	ETN_Wep4,				// 腰后2
	ETN_Wep5,				// 背后
	ETN_Wep6,				// 右手腕
	ETN_Num,
};
const tstring g_strTagNodeName[ETN_Num] = 
{
	_T("tag_Face"),       // 面部
	_T("tag_Hat"),        // 头部
	_T("tag_Back"),       // 背部
	_T("tag_RSld"),       // 右肩
	_T("tag_LSld"),       // 左肩
	_T("tag_RHd"),        // 右手
	_T("tag_LHd"),        // 左手
	_T("tag_Wep1"),       // 右腰间
	_T("tag_Wep2"),       // 左腰间
	_T("tag_Wep3"),       // 腰后1
	_T("tag_Wep4"),       // 腰后2
	_T("tag_Wep5"),       // 背后
	_T("tag_Wep6"),       // 右手腕
};

CreatureData::CreatureData(void)
{
}
CreatureData::~CreatureData(void)
{
}

CreatureData g_creature;
CreatureData* CreatureData::Inst()
{
	return &g_creature;
}

void CreatureData::LoadFromFile()
{
	tstring strPath = g_strLocalPath + _T("\\attdata\\creature_name.xml");
	const TCHAR* const szCreatureName = strPath.c_str();
	const TCHAR* const szCreature = _T("data\\system\\attdata\\creature_proto.xml");

	list<tstring> CreNameFieldList;
	list<tstring> CreFieldList;
	list<tstring> SoundFieldList;
	list<tstring>::iterator iter;

	
	TObjRef<VarContainer> VarCre = CreateObj("CreatureProto", "VarContainer");
	if(!VarCre->Load("VFS_System", szCreature, "id", &CreFieldList))
		IMSG(_T("Load file creature_proto.xml failed\r\n"));
	
	for(iter = CreFieldList.begin(); iter != CreFieldList.end(); ++iter)
	{
		tagCreatureProto att;
		att.dwTypeID = VarCre->GetDword(_T("id"), (*iter).c_str());
		//eType
		att.eType = (ECreatureType)VarCre->GetDword(_T("type"), (*iter).c_str());
		att.nType2 = (INT)VarCre->GetDword(_T("type2"), (*iter).c_str());
		att.nType3 = (INT)VarCre->GetDword(_T("type3"), (*iter).c_str());
		att.eLite = (ERank)VarCre->GetDword(_T("elite"), (*iter).c_str(),ER_Null);
		//family
		att.nFamily = (INT)VarCre->GetDword(_T("family"), (*iter).c_str(), 0);
		//vSize（这三个字段已经废弃，直接是使用默认值，size_y+ID和其它字段CRC有重复）
		att.vSize.x = X_DEF_CREATURE_SIZE_X;
		att.vSize.y = X_DEF_CREATURE_SIZE_Y;
		att.vSize.z = X_DEF_CREATURE_SIZE_Z;

		att.eFunctionType = (EFunctionNPCType)VarCre->GetDword(_T("function_type"), (*iter).c_str(), EFNPCT_Null);
		att.uFunctionID.dwCommonID = VarCre->GetDword(_T("function_id"), (*iter).c_str(), GT_INVALID);
		att.eLite = (ERank)VarCre->GetDword(_T("elite"), (*iter).c_str(), ER_Null);
		att.bCanAttack = (BOOL)VarCre->GetDword(_T("can_attack"), (*iter).c_str(), FALSE);
        att.bCanBeAttack = (BOOL)VarCre->GetDword(_T("can_be_attack"), (*iter).c_str(), FALSE);
		
		INT nInspiration = VarCre->GetInt(_T("inspiration"), (*iter).c_str(), GT_INVALID);
		m_mapInspiration.insert(make_pair(att.dwTypeID, nInspiration));

		m_creatureMap.insert(make_pair(att.dwTypeID, att));
	}

	KillObj("CreatureProto");
	
	TObjRef<VarContainer> VarCreName = CreateObj("CreatureName", "VarContainer");
	if(!VarCreName->Load("VFS_System", szCreatureName, "id", &CreNameFieldList))
		IMSG(_T("Load file creature_name.xml failed\r\n"));

	for(iter = CreNameFieldList.begin(); iter != CreNameFieldList.end(); ++iter)
	{
		DWORD dwID = VarCreName->GetDword(_T("id"), (*iter).c_str());
		tagCreatureProto* pAtt = FindInMap(m_creatureMap, dwID);
		if(P_VALID(pAtt))
		{
			_tcscpy( pAtt->szName, VarCreName->GetString(_T("name"), (*iter).c_str(), _T("")) );
			_tcscpy ( pAtt->szModelName, VarCreName->GetString(_T("model"), (*iter).c_str(), _T("")) );
			_tcscpy( pAtt->szTitle, VarCreName->GetString(_T("title"), (*iter).c_str(), _T("")) );
			_tcscpy( pAtt->szTitleIcon, VarCreName->GetString(_T("title_icon"), (*iter).c_str(), _T("")) );
			_tcscpy( pAtt->szIdleSpeech, VarCreName->GetString(_T("idle_speech"), (*iter).c_str(), _T("")) );
			_tcscpy( pAtt->szCombatSpeech, VarCreName->GetString(_T("combat_speech"), (*iter).c_str(), _T("")) );
			_tcscpy( pAtt->szSkillSpeech, VarCreName->GetString(_T("skill_speech"), (*iter).c_str(), _T("")) );
			_tcscpy( pAtt->szDeathSpeech, VarCreName->GetString(_T("death_speech"), (*iter).c_str(), _T("")) );
			pAtt->eAnimationType = (EAniType)VarCreName->GetDword(_T("anitype"), (*iter).c_str(), 1);
			_tcscpy ( pAtt->szEffect, VarCreName->GetString(_T("effect"), (*iter).c_str(), _T("")) );
			_tcscpy( pAtt->szMonsterAI, VarCreName->GetString(_T("MonsterAI"), (*iter).c_str(), _T("")) );
		}
	}

	KillObj("CreatureName");


	TObjRef<Util> pUtil;

	list<tstring> MdlList;
	TObjRef<VarContainer> VarMdlData = CreateObj("CreatureMdlData", "VarContainer");
	tstring strMdlDataPath = g_strLocalPath + _T("\\attdata\\creature_mdl_data.xml");
	if(!VarMdlData->Load("VFS_System", strMdlDataPath.c_str(), "id", &MdlList))
		IMSG(_T("Load file creature_mdl_data.xml failed\r\n"));

	for(iter = MdlList.begin(); iter != MdlList.end(); ++iter)
	{
		// 骨骼名称
		for(int i = ETN_Start; i < ETN_Num; i++ )
		{
			tstring strBoneName = VarMdlData->GetString(g_strTagNodeName[i].c_str(),(*iter).c_str(),_T(""));
			if( !strBoneName.empty() )
			{
				DWORD dwKey = MakeNpcBoneNameKey( (*iter).c_str(), g_strTagNodeName[i].c_str() );
				m_npcBoneNameMap.insert(make_pair(dwKey, strBoneName));
			}
		}

		// 其它数据
		tagCreatureMdlData data;
		data.fBeAttackSfxScale			= VarMdlData->GetFloat( _T("txm_zoom"), (*iter).c_str(), 1.0f );
		data.bCanPitch					= VarMdlData->GetInt( _T("can_pitch"), (*iter).c_str(), FALSE ) == TRUE;
		data.szDeadSound				= VarMdlData->GetString( _T("sd_dead"), (*iter).c_str(), _T("") );
		data.szCombatSound				= VarMdlData->GetString( _T("sd_inter_combat"), (*iter).c_str(), _T("")) ;
		data.szRoarSound				= VarMdlData->GetString( _T("sd_roar"), (*iter).c_str(), _T("") );
		data.szHowlSound				= VarMdlData->GetString( _T("sd_howl"), (*iter).c_str(), _T("") );
		data.szFootSound				= VarMdlData->GetString( _T("sd_foot"), (*iter).c_str(), _T("") );
		data.szNormalAttackBeAtkSfx		= VarMdlData->GetString( _T("tx_natk"), (*iter).c_str(), _T("") );
		 
		const DWORD dwID = pUtil->Crc32( (*iter).c_str() );
		m_mdlData.insert( make_pair( dwID, data ) );
	}
	KillObj("CreatureMdlData");
}

tstring CreatureData::GetNpcMdlPath(const tagCreatureProto *pNpcAtt)
{
	tstringstream stream;

	stream << _T("data\\animation\\npc\\") << pNpcAtt->szModelName << _T("\\");
	switch( pNpcAtt->eAnimationType )
	{
	case EAT_None:
		stream << pNpcAtt->szModelName;
		stream << _T(".fsm");
		break;

	case EAT_KeyFrame:
		stream << pNpcAtt->szModelName;
		stream << _T(".fak");
		break;

	case EAT_Skeleton:
		stream << _T("std.fskel");
		break;
	}

	return stream.str();
}

tstring CreatureData::GetNpcSkinPath( const tagCreatureProto *pNpcAtt, int index )
{
	tstringstream stream;

	if( pNpcAtt->eAnimationType == EAT_Skeleton )
	{
		stream << _T("data\\animation\\npc\\") << pNpcAtt->szModelName << _T("\\") << pNpcAtt->szModelName
			<< _T("_obj") << index << _T(".fskin");
	}

	return stream.str();
}

DWORD CreatureData::MakeNpcBoneNameKey( const TCHAR* szMdlName, const TCHAR* szTagNodeName )
{
	ASSERT(NULL!=szMdlName);
	ASSERT(NULL!=szTagNodeName);
	if( _T('\0') == szMdlName[0] || _T('\0') == szTagNodeName[0] )
		return GT_INVALID;

	tstring strKey = szMdlName;
	strKey += _T(' ');
	strKey += szTagNodeName;
	return TObjRef<Util>()->Crc32(strKey.c_str());
}

const tstring CreatureData::FindNpcBoneName( const DWORD dwTypeID, const TCHAR* szTagNodeName )
{
	tagCreatureProto* pProto = FindInMap( m_creatureMap, dwTypeID );
	if( NULL == pProto )
		return _T("");
	return FindNpcBoneName( pProto->szModelName, szTagNodeName );
}

const tstring CreatureData::FindNpcBoneName( const TCHAR* szMdlName, const TCHAR* szTagNodeName )
{
	const DWORD dwKey = MakeNpcBoneNameKey( szMdlName, szTagNodeName );
	if( GT_INVALID == dwKey )
		return _T("");

	map<DWORD,tstring>::iterator itr = m_npcBoneNameMap.find( dwKey );
	if( itr == m_npcBoneNameMap.end() )
		return _T("");

	return itr->second;
}

const tagCreatureMdlData* CreatureData::FindMdlData(const TCHAR* szMdlName)
{
	ASSERT( NULL != szMdlName && szMdlName[0] != _T('\0') );
	const DWORD dwID = TObjRef<Util>()->Crc32(szMdlName);
	map<DWORD,tagCreatureMdlData>::iterator itr = m_mdlData.find( dwID );
	if( itr != m_mdlData.end() )
		return &itr->second;

	return NULL;
}

INT CreatureData::FindCreatureInspiration( DWORD dwTypeID )
{
	map<DWORD,INT>::iterator it = m_mapInspiration.find(dwTypeID);
	if( it!=m_mapInspiration.end() )
		return it->second;
	
	return 0;
}