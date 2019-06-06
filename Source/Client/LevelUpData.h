#pragma once
#include "BaseData.h"
#include "..\WorldDefine\skill_define.h"
struct tagExperience
{
	INT			nLevel;					//等级
	INT			nExp;					//下一级需要的经验
	INT			nAtt[X_ERA_ATTA_NUM];	//一级属性
	INT			nCost[ESCT_End];		//可消耗属性的最大值
	tagExperience(void)
	{
		memset(this, 0x0, sizeof(tagExperience));
	}
};

class LevelUpData :
	public BaseData
{
public:
	LevelUpData(void);
	~LevelUpData(void);

	static LevelUpData* Inst();

	/** 载入角色升级经验属性文件
	*/
	virtual void LoadFromFile();

	const tagExperience* FindExpData(DWORD dwLevel)
	{
		return FindInMap(m_mapExp, dwLevel);
	}

private:
	map<DWORD, tagExperience>			m_mapExp;
};