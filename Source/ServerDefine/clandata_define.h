//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: clandata_define.h
// author: 
// actor:
// data: 2009-02-19
// last:
// brief: 氏族信息数据库存储结构
//-----------------------------------------------------------------------------
#pragma once
#pragma pack(push,1)
#include "../WorldDefine/reputation.h"
struct tagDBReputeData 
{
	INT8			m_n8ActCount[ECLT_NUM];					// 剩余激活次数
	INT32			m_nReputation[ECLT_NUM];				// 声望值
	INT32			m_nContribution[ECLT_NUM];				// 贡献值
	tagDWORDTime	m_dwtLastResetTime;						// 上次声望重置时间
	UINT16			m_u16FameMask;							// 名人堂成员
	tagDBReputeData(){	ZeroMemory(this, sizeof(*this));}
};
#pragma pack(pop)