#pragma once

#include "..\WorldDefine\skill_define.h"
#include "..\WorldDefine\talent_define.h"
#include "..\WorldDefine\role_att.h"
#include "..\WorldDefine\msg_talent.h"

#define MTransSkillTypeID(skill,lv) (DWORD)(skill*100+lv)
#define MTransSkillID(id) (DWORD)(id/100)
#define MTransSkillLevel(id) (INT)(id%100)
#define MTransProduceSkill(id) (DWORD)(id/10)

const int MAX_BATTLE_TALENT_COUNT = 2;

/** \struct tagSkillData
	\brief 技能数据结构（未完）
*/
struct tagSkillData
{
	DWORD					dwSkillID;		// 技能ID
	DWORD					dwTypeID;		// 技能模板ID
	INT						nCurCD;			// 当前冷却值（毫秒）
	INT						nLevel;			// 当前等级
	INT						nLearnLevel;	// 当前学习等级
	INT						nExp;			// 当前经验

	//--主动技能拥有的属性
	INT						nSkillDmg;		// 技能威力
	FLOAT					fOPDist;		// 作用距离（世界单位）
	INT						nPrepareTime;	// 起手时间（毫秒）
	INT						nCooldown;		// 冷却时间（毫秒）
	DWORD					dwStartTime;	// 冷却计算起始时间
	INT						nCost[ESCT_End];// 消耗
						
	tagSkillProtoClient*	pProto;
	
	tagSkillData(){ZeroMemory(this, sizeof(tagSkillData));}
	tagSkillData(DWORD dwID){ZeroMemory(this, sizeof(tagSkillData)); dwSkillID = dwID;}
};

class NetSession;
/** \class SkillMgr
	\brief 技能管理器
	\remarks 管理客户端本地玩家所学会的技能
*/
class SkillMgr
{
public:
	SkillMgr(void);
	~SkillMgr(void);

	static SkillMgr* Inst();
	
	void Init();
	void Destroy();
	
	/** \根据技能ID获得技能数据
	*/
	const tagSkillData* GetSkillData(DWORD dwID);
	/** \根据技能TypeID获得技能数据
	*/
	const tagSkillData* GetSkillDataByTypeID(DWORD dwTypeID);

	/** \获取普通攻击ID
	*/
	DWORD GetNormalAttackID();
	/** \获取普通攻击的TypeID
	*/
	DWORD GetNormalAttackTypeID();
	/** \获取开采类技能的TypeID
	*/
	DWORD GetGatherSkillTypeID(INT nType);

	/** \使用技能
	*/
	VOID OnUseSkill(DWORD dwTypeID);
	
	
	/** \获取技能列表
	*/
	void GetSkillList(TList<DWORD>& List);
	
	/** \获取技能图片
	*/
	tstring GetSkillIconPath(DWORD dwTypeID);
	/** \是否在可投天资范畴
	*/
	BOOL IsInsideOfMaxTalent(ETalentType eTalent);
	/** \获取天资点数
	*/
	INT GetTalentPoint(ETalentType eTalent);

	/** \是否有该技能
	*/
	BOOL IsHaveSkillByTypeID(DWORD dwTypeID);
	BOOL IsHaveProduceSkillByTypeID(DWORD dwTypeID);

	/** \根据模板ID获取技能是否冷却
	*/
	BOOL IsSkillCDByTypeID(DWORD dwTypeID);
	/** \获取技能CD
	*/
	VOID GetSkillCDTime(DWORD dwID, FLOAT& fCDCount, FLOAT& fRestoreRatio);
	/** \技能是否可学习
		\param dwTypeID为该级技能的原型
		\param n8Type学习类型
	*/
	DWORD IsSkillCanLearn(DWORD dwTypeID);

	/** \网络消息错误提示
	*/
	void ShowNetError(INT nErrorCode);

    /** \获得当前玩家战斗系天资个数
    */
    int GetBattleTalentCount();
    /** \是否获得此天资
    */
    BOOL IsTalentAquired(ETalentType eType);

private:

	
	/** \增加新技能
	*/
	void AddSkill(tagSkillMsgInfo data);
	/** \删除一个已有技能
	*/
	void RemoveSkill(DWORD dwID);
	/** \更新一个技能
	*/
	void UpdateSkill(tagSkillMsgInfo data);
	
	/** \设置天资点数
	*/
	void SetTalentPoint(tagRoleTalent Talent);

	/** \初始化玩家技能
	*/
	DWORD NetInitLocalPlayerSkill(tagNS_GetRoleInitState_Skill* pNetCmd, DWORD dwParam);

	/** \网络消息的处理
	*/
	DWORD NetAddSkill(tagNS_AddSkill* pNetCmd, DWORD dwParam);
	DWORD NetUpdateSkill(tagNS_UpdateSkill* pNetCmd, DWORD dwParam);
	DWORD NetRemoveSkill(tagNS_RemoveSkill* pNetCmd, DWORD dwParam);
	DWORD NetLearnSkill(tagNS_LearnSkill* pNetCmd, DWORD dwParam);
	DWORD NetLevelUpSkill(tagNS_LevelUpSkill* pNetCmd, DWORD dwParam);
	DWORD NetForgetSkill(tagNS_ForgetSkill* pNetCmd, DWORD dwParam);
	DWORD NetClearTalent(tagNS_ClearTalent* pNetCmd, DWORD dwParam);
	DWORD NetAddTalent(tagNS_AddTalent* pNetCmd, DWORD dwParam);
	DWORD NetRemoveTalent(tagNS_RemoveTalent* pNetCmd, DWORD dwParam);
	DWORD NetUpdateTalent(tagNS_UpdateTalent* pNetCmd, DWORD dwParam);
	DWORD NetUpdateSkillCD(tagNS_UpdateSkillCoolDown* pNetCmd, DWORD dwParam);

	/** \释放技能Map
	*/
	void FreeMap(); 
private:
	TSFPTrunk<SkillMgr>			m_Trunk;
	TObjRef<Util>				m_pUtil;
	TObjRef<NetCmdMgr>			m_pCmdMgr;
	TObjRef<NetSession>		m_pSession;
	TObjRef<GameFrameMgr>		m_pFrameMgr;
	
	TMap<DWORD, tagSkillData*>	m_skillMap;
	tagRoleTalent				m_talentPoint[X_MAX_TALENT_PER_ROLE];
};
