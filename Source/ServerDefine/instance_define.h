#include "../WorldDefine/RoleDefine.h"

#pragma once

#pragma pack(push, 1)

#define		RAND_CTEATUTE_NUM	3					// 随机副本导航点挂载的怪物数量
#define		RAND_INSTANCE_NUM	3					// 随机副本难度类型的数量

struct tagRandSpawnPointInfo						// 随机副本刷怪点
{
	DWORD		dwSpawnPointID;						// 刷怪点ID(大ID）
	INT			nLevel;
	DWORD		dwNormalID[RAND_CTEATUTE_NUM];		// 普通难度副本怪物ID		
	DWORD		dwEliteID[RAND_CTEATUTE_NUM];		// 精英难度副本怪物ID
	DWORD		dwDevilID[RAND_CTEATUTE_NUM];		// 魔王难度副本怪物ID
};

struct tagInstanceItem
{
	DWORD					dwMapID;
	TMap<DWORD, DWORD>		mapInstanceItem;
};

struct tagInstanceSkill
{
	DWORD					dwMapID;
	TMap<DWORD, DWORD>		mapInstanceSkill;
};

struct tagLevelMapping
{
	INT						nLevel;
	INT						nTransmitLevel;
};

#pragma pack(pop)