#pragma once
#include "Singleton.h"

#include "..\ServerDefine\login_define.h"

//游戏世界
struct tagGameWorldInfo
{
	//游戏世界名称,world name
	tstring				world_name;
	//world id
	DWORD				id;
	//world ip
	std::string			world_ip;
	//db ip
	std::string			db_ip;
	//world 状态
	EWorldStatus		world_status;
	//DB 状态
	EWorldStatus		db_status;
	// 在线人数
	INT					nPlayerOnline;
	// 总人数上限
	INT					nMaxPlayerNum;	
};

//大区信息
struct tagSectionInfo
{
	//大区名字
	tstring				section_name;
	//名字Crc
	DWORD				id;
	//Login ip
	std::string			login_ip;
	//大区下游戏世界个数
	INT					num;
	//Login 状态
	EWorldStatus		login_status;

	std::map<DWORD, tagGameWorldInfo> game_world_infos;
};

struct tagWorld
{
	//大区名字
	tstring				section_name;
	DWORD				section_id;

	//游戏世界名称
	tstring				world_name;
	DWORD				world_id;

	//服务器ip
	std::string			ip;
	//服务器状态
	EWorldStatus		status;

	tagWorld()
	{
		section_id = 0;
		world_id = 0;
		section_name = _T("");
		world_name = _T("");
		ip = "";
		world_name = _T("");
		status = EWS_InitNotDone;
	}
};

struct tagSWId
{
	DWORD				section_id;
	DWORD				world_id;
};

struct tagSWIdEx
{
	DWORD				section_id;
	std::list<DWORD>	list_world_id;
};

class DataMgr : public Singleton<DataMgr>
{
public:
	DataMgr(void);
	~DataMgr(void);

	void Destroy();

	//--
	void AddWorldDataList(tagWorld* world);
	std::list<tagWorld*>* GetWorldDataList() { return &m_listWorld; }
	void UpdateWorldDataList(tagSectionInfo* section_info);
	void GetSectionAndWorldID(const tstring& name, tagSWId* id);
	bool IsSectionWorldValid(const tstring& szSectionName, const tstring szWorldName);
	bool IsSectionValid(const tstring& szSectionName);
	//--
	void AddSectionDataList(tagSectionInfo& info);	
	bool JudgeSingleSection(const DWORD& section_id, const std::list<DWORD>& list_world_id);
	EWorldStatus JudgeWorldListState(const DWORD& section_id, const DWORD& world_id);
	tstring GetWorldNameByWorldID(DWORD world_id);

private:
	std::list<tagWorld*>					m_listWorld;				
	std::list<tagSectionInfo>				m_listSection;
};

#define sDataMgrPtr DataMgr::GetSingletonPtr()