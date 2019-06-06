#pragma once

#include ".\BaseData.h"
#include "..\WorldDefine\trigger_define.h"
#include "..\WorldDefine\skill_define.h"
#include "..\WorldDefine\buff_define.h"

#define MAX_CONTINUE_SKILL_NUM 4
//连接技结构
struct tagContinuteSkill
{
	DWORD		dwTypeID;							//连接技表的typeid	
	INT			nTarget;							//自身或目标
	DWORD		dwSkill[MAX_CONTINUE_SKILL_NUM];	//对应buff表里的5位id
};

/** \class	SkillProtoData
	\brief	技能、状态、触发器原始属性结构存储
*/
class SkillProtoData
	: public BaseData
{
public:
	SkillProtoData(void);
	~SkillProtoData(void);

	static SkillProtoData* Inst();

	/** \载入属性文件
	*/
	virtual void LoadFromFile();
	/** \获取技能基础结构
	*/
	const tagSkillProtoClient* FindSkillProto(DWORD dwID)
	{
		return FindInMap(m_mapSkillProto, dwID);
	}
	/** \获取状态基础结构
	*/
	const tagBuffProtoClient* FindBuffProto(DWORD dwID)
	{
		return FindInMap(m_mapBuffProto, dwID);
	}
	/**	\获取触发器基础结构
	*/
	const tagTriggerProto* FindTriggerProto(DWORD dwID)
	{
		return FindInMap(m_mapTriggerProto, dwID);
	}
	/**	\获取连接技基础结构
	*/
	const tagContinuteSkill* FindContinuteSkillProto(DWORD dwID)
	{
		return FindInMap(m_mapConSkillProto, dwID);
	}

private:
	/** \增加技能属性
	*/
	VOID AddSkillRoleAtt(tagSkillProto& sSkill, ERoleAttribute eType, INT nValue);
	
private:
	map<DWORD, tagSkillProtoClient>		m_mapSkillProto;		// 技能原始结构
	map<DWORD, tagBuffProtoClient>		m_mapBuffProto;			// 状态原始结构
	map<DWORD, tagTriggerProto>			m_mapTriggerProto;		// 触发器原始结构
	map<DWORD, tagContinuteSkill>		m_mapConSkillProto;		// 连续技能原始结构
	
};