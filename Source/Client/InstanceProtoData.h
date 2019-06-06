#pragma once

#include "..\WorldDefine\MapAttDefine.h"

/*!
\brief 副本属性表部分属性，客户端通过这些属性进行一些预先判断 (待添加)
*/

#define INSTANCE_CONDITION_NUM 3		//副本难度级别数量

// 副本完成条件枚举
enum EInstanceComplteCondition
{
	EICC_Time		= 0,	// 坚持时间
	EICC_People		= 1,	// 坚持人数
	EICC_Monster	= 2,	// 清空怪物
	EICC_Boss		= 3,	// 击杀指定怪物
	EICC_Item		= 4		// 获得指定物品
};

// 副本完成条件结构,客户端屏显用
struct tagInstanceCompleteCondition
{
	EInstanceComplteCondition	eCondition;		// 完成条件
	DWORD						dwCompleteVal1; // 条件值1
	DWORD						dwCompleteVal2; // 条件值2
};

// 副本属性表（客户端所需部分）
struct tagInstanceProtoData
{
	DWORD				dwInstanceID;			// 副本ID
	tstring				strName;				// 副本名字
	EInstanceMapType	eType;					// 副本地图类型
	BOOL				bAskEnter;				// 是否询问进入
	BOOL				bSelectHard;			// 是否可选难度
	BOOL				bSelectNormal;			// 是否可选普通
	BOOL				bSelectElite;			// 是否可选精英
	BOOL				bSelectDevil;			// 是否可选魔王
	INT					nNumLowerLimit;			// 人数下限
	INT					nNumUpperLimit;			// 人数上限
	INT					nLevelLowerLimit;		// 等级下限
	INT					nLevelUpperLimit;		// 等级上限
	BOOL				bCombat;				// 能否战斗
	BOOL				bPK;					// 能否PK
	BOOL				bLoseSafeGuard;			// 是否保护模式失效

	tagInstanceCompleteCondition Condition[INSTANCE_CONDITION_NUM];
};

class InstanceProtoData
{
public:
	friend class InstanceMgr;
	BOOL LoadFromFile(LPCTSTR szPath);
	const tagInstanceProtoData* FindInstanceProtoData(DWORD dwInstanceID);
private:
	typedef map<DWORD, tagInstanceProtoData> InstanceProtoDataMap;
	InstanceProtoDataMap m_mapInstanceProtoData;
};

