//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: sspawnpoint_define.h
// author: xlguo
// actor:
// data: 2009-03-20
// last:
// brief: 服务器用非副本刷怪点原型
//-----------------------------------------------------------------------------

// 每个刷怪点怪物最多种类
const INT MAX_CREATURE_PER_SSPAWNPOINT	= 20;

//--------------------------------------------------------------------------------
// 服务器端刷怪点结构（普通地图）
//--------------------------------------------------------------------------------
struct tagSSpawnPointProto
{
	DWORD				dwSpawnPointID;								// 刷怪点id
	DWORD				dwTypeIDs[MAX_CREATURE_PER_SSPAWNPOINT];	// 怪物typeid

	tagSSpawnPointProto() { ZeroMemory(this, sizeof(*this)); }
};
