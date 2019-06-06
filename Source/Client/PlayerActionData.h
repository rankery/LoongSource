#pragma once

#include ".\BaseData.h"

/** \玩家动作数据
*/
struct tagPlayerActionData
{
	DWORD dwAttackTime;  // 开始攻击的时间点
};

/** \class PlayerActionData
	\brief 玩家动作数据
*/
class PlayerActionData
	: public BaseData
{
public:
	PlayerActionData(void);
	~PlayerActionData(void);

	static PlayerActionData* Inst();
	/** \载入属性文件
	*/
	virtual void LoadFromFile();
	/** \查找特效数据
	*/
	const tagPlayerActionData* FindData(const TCHAR* szActName);
private:
	map<DWORD, tagPlayerActionData>		m_mapPlayerActionData;
};