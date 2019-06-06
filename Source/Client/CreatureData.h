#pragma once
#include "..\WorldDefine\creature_define.h"
#include ".\BaseData.h"

/** \生物模型相关数据
*/
struct tagCreatureMdlData
{
	float   fBeAttackSfxScale;		// 被攻击特效缩放系数
	bool    bCanPitch;				// 是否俯仰
	tstring szDeadSound;			// 死亡音效
	tstring szCombatSound;			// 进入战斗音效
	tstring szRoarSound;			// 进攻喊叫音效
	tstring szHowlSound;			// 挨打喊叫音效
	tstring szFootSound;			// 脚步声（前缀，后缀为表面类型）
	tstring szNormalAttackBeAtkSfx;	// 普通攻击对方被攻击特效
};

/** class	CreatureData
	brief	怪物，NPC，地物，宠物原始属性结构存储
*/
class CreatureData 
	: public BaseData
{
public:
	CreatureData(void);
	~CreatureData(void);
	
	static CreatureData* Inst();
	
	/** 载入属性文件
	*/
	virtual void LoadFromFile();
	/** 获取NPC模型文件路径
	*/
	static tstring GetNpcMdlPath(const tagCreatureProto *pNpcAtt);
	/**	获取NPC蒙皮文件路径
	*/
	static tstring GetNpcSkinPath(const tagCreatureProto *pNpcAtt, int index);
	/** 查找NPC的原始属性
	*/
	const tagCreatureProto* FindNpcAtt(DWORD attID)
	{
		return FindInMap(m_creatureMap,attID);
	}
	/** 获取所有的NPC原始属性
	*/
	void GetNpcMap(map<DWORD,tagCreatureProto>& npcMap)
	{
		npcMap = m_creatureMap;
	}
	/** 获取模型相关数据
	*/
	const tagCreatureMdlData* FindMdlData(const TCHAR* szMdlName);
	/** 获取骨骼名称
	*/
	const tstring FindNpcBoneName( const DWORD dwTypeID, const TCHAR* szTagNodeName );
	const tstring FindNpcBoneName( const TCHAR* szMdlName, const TCHAR* szTagNodeName );
	/**获取灵感值
	*/
	INT FindCreatureInspiration(DWORD dwTypeID);
private:
	DWORD MakeNpcBoneNameKey( const TCHAR* szMdlName, const TCHAR* szTagNodeName );
private:
	map<DWORD,tagCreatureProto>			m_creatureMap;
	map<DWORD,tagCreatureMdlData>		m_mdlData;
	map<DWORD,tstring>					m_npcBoneNameMap;
	map<DWORD,INT>						m_mapInspiration;
};
