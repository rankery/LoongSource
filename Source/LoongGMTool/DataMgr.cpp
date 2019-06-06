#include "stdafx.h"
#include "..\ServerDefine\msg_rt_c.h"
#include "DataMgr.h"

DataMgr::DataMgr( void )
{

}

DataMgr::~DataMgr( void )
{
	this->Destroy();
}

void DataMgr::Destroy()
{
	std::list<tagWorld*>::iterator it = m_listWorld.begin();
	for( ; it!=m_listWorld.end(); it++ )
	{
		delete (*it);
	}
	m_listWorld.clear();
	m_listSection.clear();
}

void DataMgr::AddWorldDataList( tagWorld* world )
{
	m_listWorld.push_back(world);
}

void DataMgr::UpdateWorldDataList( tagSectionInfo* section_info )
{
	if( (int)m_listWorld.size() <= 0 )
		return;

	std::map<DWORD, tagGameWorldInfo>& info = section_info->game_world_infos;
	std::map<DWORD, tagGameWorldInfo>::iterator it2;
	std::list<tagWorld*>::iterator it = m_listWorld.begin();
	for( ; it!=m_listWorld.end(); it++ )
	{
		tagWorld* world = *it;
		if( world->section_id == section_info->id )
		{
			//更新World状态
			it2 = info.find(world->world_id);
			if (it2 != info.end())
			{
				//找到相同world,更新world状态
				world->status = it2->second.world_status;
			}
		}
	}
}

void DataMgr::GetSectionAndWorldID( const tstring& name, tagSWId* id )
{
	std::list<tagWorld*>::iterator it = m_listWorld.begin();
	for( ; it!=m_listWorld.end(); it++ )
	{
		if( (*it)->world_name == name )
		{
			id->section_id = (*it)->section_id;
			id->world_id = (*it)->world_id;
			return;
		}
	}
	id->section_id = GT_INVALID;
	id->world_id = GT_INVALID;
}

bool DataMgr::IsSectionWorldValid(const tstring& szSectionName, const tstring szWorldName)
{
	std::list<tagWorld*>::iterator it = m_listWorld.begin();
	for( ; it!=m_listWorld.end(); it++ )
	{
		if ( (_tcscmp((*it)->section_name.c_str(), szSectionName.c_str()) == 0 ) && (_tcscmp((*it)->world_name.c_str(), szWorldName.c_str()) == 0) )
		{
			return true;
		}

	}

	return false;
}

bool DataMgr::IsSectionValid(const tstring& szSectionName)
{
	std::list<tagWorld*>::iterator it = m_listWorld.begin();
	for( ; it!=m_listWorld.end(); it++ )
	{
		if ( _tcscmp((*it)->section_name.c_str(), szSectionName.c_str()) == 0)
		{
			return true;
		}

	}

	return false;
}

void DataMgr::AddSectionDataList( tagSectionInfo& info)
{
	m_listSection.push_back(info);
}

bool DataMgr::JudgeSingleSection( const DWORD& section_id, const std::list<DWORD>& list_world_id )
{
	if( (int)list_world_id.size() == 0 )
		return false;

	bool bEqual = true;
	std::list<tagSectionInfo>::iterator it = m_listSection.begin();
	for( ; it!=m_listSection.end(); it++ )
	{
		if( it->id == section_id )
		{
			if( list_world_id.size()!=it->game_world_infos.size() )
			{
				//没有选择大区下所有游戏世界
				return false;
			}
			else 
			{
				
				int i=0; 
				std::map<DWORD, tagGameWorldInfo>::iterator itInfo;
				std::map<DWORD, tagGameWorldInfo>& info = it->game_world_infos;
				for( std::list<DWORD>::const_iterator itList=list_world_id.begin(); 
					itList!=list_world_id.end(); itList++)
				{
					itInfo = info.find(*itList);
					if(itInfo==info.end())
					{
						bEqual = false;
						break;
					}
				}
			}
		}
	}

	if( bEqual )
		return true;

	return false;
}

EWorldStatus DataMgr::JudgeWorldListState( const DWORD& section_id, const DWORD& world_id )
{
	std::list<tagWorld*>::iterator it = m_listWorld.begin();
	for( ; it!=m_listWorld.end(); it++ )
	{
		if(section_id==(*it)->section_id && world_id==(*it)->world_id)
		{
			return (*it)->status ;
		}
	}
	
	return EWS_InitNotDone;
}

tstring DataMgr::GetWorldNameByWorldID( DWORD world_id )
{
	std::list<tagWorld*>::iterator it = m_listWorld.begin();
	for( ; it!=m_listWorld.end(); it++ )
	{
		if(world_id==(*it)->world_id)
		{
			return (*it)->world_name ;
		}
	}
	return _T("");
}