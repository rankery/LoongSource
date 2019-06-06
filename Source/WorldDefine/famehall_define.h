#pragma once
#pragma pack(push, 1)

// 获得声望排名数量
const UINT16 MAX_REP_RANK_NUM				= 50;

// 氏族珍宝数量
const UINT16 CLAN_TREASURE_NUM				= 300;




//----------------------------------------------------------------------------
// 氏族珍宝
//----------------------------------------------------------------------------
struct tagTreasureData
{
	UINT16	n16TreasureID;			// 珍宝id
	DWORD	dwNamePrefixID;			// 珍宝名称前缀
}; 

//-----------------------------------------------------------------------------
// 声望排名数据
//-----------------------------------------------------------------------------
struct tagRepRankData
{
	DWORD	dwRoleID;				// 角色ID
	DWORD	dwReputValue;			// 声望值
};


#pragma pack(pop)