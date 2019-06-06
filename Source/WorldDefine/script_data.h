//-----------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: script_data.h
// author: wjiang
// actor:
// data: 2009-04-6
// last:
// brief: 脚本通用数据管理类
//-----------------------------------------------------------------------------

#pragma once

#pragma pack(push, 1)

//-----------------------------------------------------------------------------
// 脚本模版Size
//-----------------------------------------------------------------------------
enum EScirptData
{
	ESD_Quest		=	10,		// 任务脚本数据个数
	ESD_Role		=	100,	// 玩家脚本数据个数
	ESD_Creature	=	8,		// 怪物脚本数据个数
	ESD_Activity	=	50,		// 活动脚本数据个数
	ESD_Map			=	40,		// 地图脚本数据个数
};

//-----------------------------------------------------------------------------
// 脚本数据模版
//-----------------------------------------------------------------------------
template<INT nSize>
struct tagScriptData
{
	DWORD	dwData[nSize];
	tagScriptData()
	{
		ZeroMemory(dwData, sizeof(DWORD)*nSize);
	}
};


template<INT nSize>
class ScriptData
{
public:
	ScriptData() {}

	inline VOID	ModScriptData(INT nIndex, DWORD dwValue)
	{
		ASSERT(nIndex >= 0 && nIndex < nSize);
		m_ScriptData.dwData[nIndex] += dwValue;
	}

	inline VOID	SetScriptData(INT nIndex, DWORD dwValue)
	{
		ASSERT(nIndex >= 0 && nIndex < nSize);
		m_ScriptData.dwData[nIndex] = dwValue;
	}

	inline DWORD GetScriptData(INT nIndex)
	{
		ASSERT(nIndex >= 0 && nIndex < nSize);
		return	m_ScriptData.dwData[nIndex];
	}

protected:
	tagScriptData<nSize>   m_ScriptData;
};


#pragma pack(pop)
