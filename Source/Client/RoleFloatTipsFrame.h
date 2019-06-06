#pragma once

#include "Render2D.h"
#include "ClientCamera.h"
#include "..\WorldDefine\msg_combat.h"

struct tagShowHPChangeEvent;
struct tagUpdateLPLevelExp;
struct tagUpdateLPLevel;
struct tagNS_LearnSkill;
struct tagPctNExpAchieve;
struct tagLRoleCombatState;
struct tagAddTeammateEvent;
struct tagLeaveTeamEvent;
struct tagReputeChange;
struct tagReputeLvlChange;
struct tagUpdateLPAttEvent;

enum EChangeType
{
	EChange_NULL = 0,	
	EChange_HPInc,			// HP增加
	EChange_HPDec,			// HP减少
	EChange_MPInc,			// MP增加
	EChange_MPDec,			// MP减少
	EChange_Miss,			// MISS
	EChange_Crit,			// 暴击
	EChange_Blocked,		// 格挡
	EChange_EnterSafe,		// 进入安全区
	EChange_ExitSafe,		// 离开安全区
	EChange_Exp,			// 经验变化
	EChange_NExp,			// N%经验达成
	EChange_Skill,			// 使用XX技能
	EChange_StudySkill,		// 学会XX技能
	EChange_EnterCombat,	// 进入战斗
	EChange_ExitCombat,		// 离开战斗
	EChange_EnterTeam,		// 进入队伍
	EChange_ExitTeam,		// 离开队伍
	EChange_Reputation,		// 声望变化
	EChange_ReputaLel,		// 声望等级
	EChange_HPPer10,		// 10%hp提示
	EChange_MPPer10,		// 10%mp提示
};

// 飘浮信息
struct tagFloatInfo
{
	DWORD		roleID;			// 哪个角色的变化
	EChangeType eTypeChange;	// 变化类型
	float		startTime;		// 什么时候产生
	tagPoint	ptTxtSize;		// 信息的宽高
	float		stayTime;		// 停留时间
	TCHAR		szInfo[64];		// 变化的值
	POINT		ptOffset;		// 偏移（防止重叠)
	Vector3		worldPos;		// 世界座标
	Vector3		srcPos;
	POINT		pt;				// 屏幕坐标位置
	Color4ub	fontColor;
	int			nWidth;
	int			nHeight;
	bool		bFrist;

	tagFloatInfo()
	{
		memset(this, 0, sizeof(*this));
		eTypeChange = EChange_NULL;
		stayTime = 1.0f;
	}

	tagFloatInfo(DWORD dwRoleID)
	{
		memset(this, 0, sizeof(*this));
		roleID = dwRoleID;
		startTime = Kernel::Inst()->GetAccumTime();		//记录开始时间
		stayTime = 1.0f;
	}
};

/**class RoleFloatTips
   brief 角色头顶飘浮的tips信息
*/
class RoleFloatTipsFrame :
	public GameFrame
{
public:
	RoleFloatTipsFrame(void);
	~RoleFloatTipsFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual VOID Render3D();
	virtual VOID OnEvent(tagGameEvent* pEvent);
	virtual VOID Update();
	virtual BOOL ReloadUI();

	//--游戏事件
	DWORD OnShowHPChangeEvent(tagShowHPChangeEvent* pEvent);
	DWORD OnUpdateLPLevelExp(tagUpdateLPLevelExp* pEvent);
	DWORD OnPctNExpAchieve(tagPctNExpAchieve* pEvent);
	DWORD OnLRoleCombatState(tagLRoleCombatState* pEvent);
	DWORD OnAddTeammateEvent(tagAddTeammateEvent* pEvent);
	DWORD OnLeaveTeamEvent(tagLeaveTeamEvent* pEvent);
	DWORD OnReputeChange(tagReputeChange* pEvent);
	DWORD OnReputeLvlChange(tagReputeLvlChange* pEvent);
	DWORD OnUpdateLPAttEvent(tagUpdateLPAttEvent* pEvent);

	//--网络消息
	DWORD OnNS_Skill(tagNS_Skill* pMsg, DWORD dwParam);
	DWORD OnNS_LearnSkill(tagNS_LearnSkill* pMsg, DWORD dwParam);

private:
	// 添加飘HP信息
	void PushHPInfo(DWORD roleID, int damage, bool bMiss=false, bool bCrit=false, bool bBlocked=false);
	// 添加飘MP信息
	void PushMPInfo(DWORD roleID, int damage);
	// 进入进出安全区
	void PushEnterOrExitSafe(DWORD roleID, bool bEnter=true);
	// 经验变化
	void PushUpdateLPLevelExp(INT nExpInc);
	// 使用技能
	void PushLocalRoleUseSkill(DWORD dwSkill);
	// 学会技能
	void PushStudySkill(DWORD dwSkillID);


	//创建飘浮信息
	void CreateFloatInfo(tagFloatInfo& info);
	//绘制角色头顶飘浮的信息
	void DrawFloatInfo();
	//计算字产生的位置防止位置重叠
	void CalCreateFontPos(tagFloatInfo& info);
	//计算字的位置
	void CalFontPos(const tagFloatInfo& info,int nChange, POINT& pt);
	//计算字的颜色
	void CalFontColor(DWORD dwRoleID, EChangeType eType, Color4ub &fontColor);

	//10%hp / 10%mp提示
	void OnHPorMpExcessPer10();

private:
	TSFPTrunk<RoleFloatTipsFrame>		m_Trunk;
	TObjRef<GUIRender>					m_pRender;
	TObjRef<GUISystem>					m_pSystem;
	TObjRef<NetCmdMgr>					m_pCmdMgr;
	TObjRef<Util>						m_pUtil;
	vector<tagFloatInfo>				m_vFloatInfoArray;	

	PicFontEx							m_AttackOtherPicFont;			//图片字
	PicFontEx							m_AttackOwnPicFont;
	IDepthFont							*m_pFont;
	INT									m_nMP;

	BOOL								m_bHPPer10;
	BOOL								m_bMPPer10;
};
