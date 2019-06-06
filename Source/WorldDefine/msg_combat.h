#pragma once
#include "msg_common_errorcode.h"
#include "buff_define.h"
#include "move_define.h"

#pragma pack(push, 1)
//----------------------------------------------------------------------------
// 错误码枚举
//----------------------------------------------------------------------------
enum
{
	E_UseSkill_SkillNotExist		=	1,		// 技能不存在
	E_UseSkill_TargetInvalid		=	2,		// 目标非法
	E_UseSkill_PassiveSkill			=	3,		// 被动技能不能使用
	E_UseSkill_SkillTargetInvalid	=	4,		// 技能本身不是针对于战斗目标
	E_UseSkill_CoolDowning			=	5,		// 技能正在冷却
	E_UseSkill_Operating			=	6,		// 技能正在发动
	E_UseSkill_UseLimit				=	7,		// 发动条件不满足
	E_UseSkill_SexLimit				=	8,		// 性别不满足
	E_UseSkill_PosLimitFront		=	9,		// 位置不满足（必须在身前）
	E_UseSkill_PosLimitBack			=	10,		// 位置不满足（必须在身后）
	E_UseSkill_DistLimit			=	11,		// 距离不满足
	E_UseSkill_RayLimit				=	12,		// 射线检测不满足
	E_UseSkill_CostLimit			=	13,		// 消耗不够
	E_UseSkill_TargetLimit			=	14,		// 目标限制
	E_UseSkill_StallLimit			=	15,		// 摆摊状态下不可使用技能
	E_UseSkill_WeaponLimit			=	16,		// 武器限制
	E_UseSkill_SelfStateLimit		=	17,		// 自身状态限制
	E_UseSkill_TargetStateLimit		=	18,		// 目标状态限制
	E_UseSkill_SelfBuffLimit		=	19,		// 自身Buff限制
	E_UseSkill_TargetBuffLimit		=	20,		// 目标Buff限制
	E_UseSkill_Mount_NoPreparingPet	=	21,		// 没有预备宠物
	E_UseSkill_VocationLimit		=	22,		// 职业限制
	E_UseSkill_MapLimit				=	23,		// 无法在该地图内使用

	E_UseItem_ItemNotExist			=	30,		// 物品不存在
	E_UseItem_TargetInvalid			=	31,		// 目标非法
	E_UseItem_ItemCanNotUse			=	32,		// 物品不可使用
	E_UseItem_CoolDowning			=	33,		// 物品正在冷却
	E_UseItem_SexLimit				=	34,		// 性别不满足
	E_UseItem_LevelLimit			=   35,		// 等级限制
	E_UseItem_DistLimit				=	36,		// 距离不满足
	E_UseItem_RayLimit				=	37,		// 射线检测不满足
	E_UseItem_UseLimit				=	38,		// 人物以死亡,不能使用
	E_UseItem_Operating				=	39,		// 物品正在北使用
	E_UseItem_NotEnoughSpace		=   40,		// 背包空间不足
	E_UseItem_ReAcceptQuest			=	41,		// 您已经接取过该任务
	E_UseItem_ActivityLimit			=	42,		// 该道道具只可在固定活动中使用
	E_UseItem_PosLimit				=	43,		// 不可在该位置使用该物品
	E_UseItem_SelfStateLimit		=	44,		// 玩家自身状态限制
	E_UseItem_AcceptSimQuest		=	45,		// 您身上已经有同类任务
	E_UseItem_VocationLimit			=	46,		// 职业限制物品使用
	E_UseItem_MapLimit				=	47,		// 无法在该地图内使用
	E_UseItem_SpecFunError			=	48,		// 物品的特殊使用类型不匹配

	E_Revive_Unknown				=	50,		// 未知复活类型
	E_Revive_NotDead				=	51,		// 不在死亡状态
	E_Revive_CanNotTransmit			=	52,		// 回城复活时传送失败
	E_Revive_ItemLimit				=	53,		// 原地复活时缺少相应道具
	E_Revive_CanNotAccept			=	54,		// 接受复活时失败
	E_Revive_MapNotFound			=	55,		// 没有找到复活地图

    E_SkillMsg_NoMsg                =   999,    // 空提示
};

//----------------------------------------------------------------------------
// 角色HP变化原因
//----------------------------------------------------------------------------
enum ERoleHPChangeCause
{
	ERHPCC_SkillDamage,				//技能伤害引起
	ERHPCC_BuffDamage,				//Buff伤害引起
	ERHPCC_Other,					//其它原因
};

//----------------------------------------------------------------------------
// 角色死亡原因
//----------------------------------------------------------------------------
enum ERoleDeadCause
{
	ERDC_Skill,						//技能伤害引起
	ERDC_Buff,						//Buff伤害引起
	ERDC_Other,						//其它原因
};

//----------------------------------------------------------------------------
// 命中目标原因
//----------------------------------------------------------------------------
enum EHitTargetCause
{
	EHTC_Skill,						//使用技能
	EHTC_Item,						//使用物品
};

//----------------------------------------------------------------------------
// 场景特效类型
//----------------------------------------------------------------------------
enum ESceneEffectType
{
	ESET_ByObjID,					//使用ObjID进行播放
	ESET_ByPos,						//在指定位置播放
};

//----------------------------------------------------------------------------
// 场景音效类型
//----------------------------------------------------------------------------
enum ESceneMusicType
{
	ESMT_2D,						//2D音乐
	ESMT_3D,						//3D音效
};

//----------------------------------------------------------------------------
// 进入/离开战斗
//----------------------------------------------------------------------------
CMD_START(NC_EnterCombat)
CMD_END

CMD_START(NC_LeaveCombat)
CMD_END

//----------------------------------------------------------------------------
// 技能攻击
//----------------------------------------------------------------------------
CMD_START(NC_Skill)			
	Vector3		vSrcPos;			//攻击者当前坐标
	DWORD		dwTargetRoleID;		//目标ID
	DWORD		dwSkillID;			//技能ID
	DWORD		dwSerial;			//消息序列号
CMD_END

CMD_START(NS_Skill)			
	DWORD		dwSrcRoleID;		//攻击者ID
	DWORD		dwTarRoleID;		//目标ID
	DWORD		dwSkillID;			//技能ID
	DWORD		dwSerial;			//消息序列号
	INT			nSpellTime;			//吟唱时间，不带吟唱的技能为0
	DWORD		dwErrorCode;		//错误代码
CMD_END


//----------------------------------------------------------------------------
// 技能打断
//----------------------------------------------------------------------------
CMD_START(NC_SkillInterrupt)		//打断技能或吟唱	
	DWORD		dwSkillID;			//技能ID
CMD_END

CMD_START(NS_SkillInterrupt)		//打断技能或吟唱	
	DWORD		dwRoleID;			//发起者ID
	DWORD		dwSkillID;			//技能ID
CMD_END

//----------------------------------------------------------------------------
// 使用物品
//----------------------------------------------------------------------------
CMD_START(NC_UseItem)
	Vector3		vSrcPos;			//物品使用者当前坐标
	DWORD		dwTargetRoleID;		//目标ID
	INT64		n64ItemID;			//物品64位ID
	DWORD		dwSerial;			//消息序列号
CMD_END

CMD_START(NS_UseItem)			
	DWORD		dwSrcRoleID;		//物品使用者ID
	DWORD		dwTarRoleID;		//目标ID
	INT64		n64ItemID;			//物品64位ID
	DWORD		dwTypeID;			//物品TypeID
	DWORD		dwSerial;			//消息序列号
	INT			nSpellTime;			//吟唱时间，不带吟唱的物品为0
	DWORD		dwErrorCode;		//错误代码
	bool		bInmmediate;		//是否瞬发？
CMD_END

//----------------------------------------------------------------------------
// 使用物品打断
//----------------------------------------------------------------------------
CMD_START(NC_UseItemInterrupt)		//打断吟唱	
	INT64		n64ItemID;			//物品64位ID
CMD_END

CMD_START(NS_UseItemInterrupt)		//打断吟唱	
	DWORD		dwRoleID;			//发起者ID
	INT64		n64ItemID;			//物品64位ID
	DWORD		dwTypeID;			//物品TypeID
CMD_END


//----------------------------------------------------------------------------
// 使用技能或物品命中目标
//----------------------------------------------------------------------------
CMD_START(NS_HitTarget)
	DWORD		dwRoleID;			//角色ID
	DWORD		dwSrcRoleID;		//发起者ID
	EHitTargetCause	eCause;			//原因
	DWORD		dwMisc;				//如果是技能，则为技能ID；如果是使用物品，则为物品TypeID
	DWORD		dwSerial;			//如果是技能，则为技能消息序列号；如果是物品则为使用物品消息序列号
CMD_END


//----------------------------------------------------------------------------
// HP改变显示消息
//----------------------------------------------------------------------------
CMD_START(NS_RoleHPChange)
	DWORD		dwRoleID;			//角色ID
	ERoleHPChangeCause	eCause;		//HP变化原因
	bool		bMiss;				//是否Miss
	bool		bCrit;				//是否暴击
	bool		bBlocked;			//是否被格挡
	INT			nHPChange;			//HP变化值
	DWORD		dwSrcRoleID;		//发起者ID
	DWORD		dwMisc;				//根据原因来定义
	DWORD		dwMisc2;			//根据原因来定义
	DWORD		dwSerial;			//根据原因来定义，如果是技能，由为技能消息序列号；其它原因填GT_INVALID
CMD_END

//----------------------------------------------------------------------------
// 复活
//----------------------------------------------------------------------------
enum ERoleReviveType
{
	ERRT_Start			= 0,

	ERRT_ReturnCity		= 0,		// 回城复活
	ERRT_Locus			= 1,		// 原地复活
	ERRT_Accept			= 2,		// 接受复活
	ERRT_Prison			= 3,		// 狱中复活

	ERRT_None,						//无建议
	ERRT_End			= ERRT_None,
	
};

//----------------------------------------------------------------------------
// 角色死亡消息
//----------------------------------------------------------------------------
CMD_START(NS_RoleDead)
	DWORD		dwRoleID;			//角色ID
	ERoleDeadCause eCause;			//死亡原因
	DWORD		dwSrcRoleID;		//攻击者ID
	DWORD		dwMisc;				//如果是技能，则为技能ID；如果是buff，则为buff id
	DWORD		dwMisc2;			//如果是技能，则为管道序列号
	DWORD		dwSerial;			//如果是技能，则为技能消息序列号；其它原因填GT_INVALID
	ERoleReviveType eSuggestRevive;//复活方式，若为ERRT_None 则由玩家自己选择
CMD_END



CMD_START(NC_RoleRevive)
	ERoleReviveType	eType;
	INT64			n64ItemID;		// 原地复活所需的物品64位ID
CMD_END

CMD_START(NS_RoleRevive)			// 需要向周围玩家发送
	DWORD			dwErrorCode;
	DWORD			dwRoleID;
CMD_END

CMD_START(NS_RoleReviveNotify)
CMD_END

//----------------------------------------------------------------------
// 添加删除更新一个Buff
//----------------------------------------------------------------------
CMD_START(NS_AddRoleBuff)
	DWORD			dwRoleID;			// ID
	tagBuffMsgInfo	Buff;				// Buff内容
CMD_END

CMD_START(NS_RemoveRoleBuff)
	DWORD			dwRoleID;			// ID
	DWORD			dwBuffTypeID;		// Buff属性ID
CMD_END

CMD_START(NS_UpdateRoleBuff)
	DWORD			dwRoleID;			// ID
	tagBuffMsgInfo	Buff;				// Buff内容
CMD_END

//----------------------------------------------------------------------------
// 客户端取消一个Buff
//----------------------------------------------------------------------------
CMD_START(NC_CancelBuff)
	DWORD			dwBuffTypeID;
CMD_END

//----------------------------------------------------------------------------
// 客户端停止所有行为
//----------------------------------------------------------------------------
CMD_START(NS_StopAction)
	DWORD			dwRoleID;
	Vector3			curPos;
	Vector3			faceTo;
	bool			bSwim;
CMD_END


//----------------------------------------------------------------------------
// 怪物进入战斗，用于客户端播放音效
//----------------------------------------------------------------------------
CMD_START(NS_MonsterEnterCombat)
	DWORD			dwRoleID;
CMD_END

//----------------------------------------------------------------------------
// 特殊移动（击退，冲锋，瞬移等）
//----------------------------------------------------------------------------
CMD_START(NS_SpecialMove)
	ESpecialMoveType	eType;          // 移动类型
	DWORD				dwRoleID;       // 角色ID
	Vector3				vDestPos;       // 目标点
CMD_END


//----------------------------------------------------------------------------
// 怪物播放一个动作
//----------------------------------------------------------------------------
CMD_START(NS_MonsterPlayAction)
	DWORD			dwRoleID;				// 角色ID
	DWORD			dwActionFourCC;			// 动作FourCC
CMD_END


//----------------------------------------------------------------------------
// 怪物说话
//----------------------------------------------------------------------------
CMD_START(NS_MonsterSay)
	DWORD				dwRoleID;			// 角色ID
	DWORD				dwSayID;			// 说话内容ID，对应MonsterSay.xml中的一条记录
CMD_END


//----------------------------------------------------------------------------
// 播放一个场景特效
//----------------------------------------------------------------------------
CMD_START(NS_PlaySceneEffect)
	ESceneEffectType	eType;				// 场景特效类型
	DWORD				dwObjID;			// 特效ObjID
	Vector3				vPos;				// 播放坐标
	DWORD				dwEffectID;			// 特效ID，最终特效文件名格式：SCN+特效ID，场景特效存放于指定目录
CMD_END

//----------------------------------------------------------------------------
// 停止一个场景特效(仅限地图obj方式)
//----------------------------------------------------------------------------
CMD_START(NS_StopSceneEffect)
    DWORD           dwObjID;                // 特效objID
CMD_END

//----------------------------------------------------------------------------
// 播放一个场景音乐或音效
//----------------------------------------------------------------------------
CMD_START(NS_PlaySceneMusic)
	ESceneMusicType		eType;				// 场景音乐或音效类型
	Vector3				vPos;				// 3D音效坐标
	DWORD				dwMusicID;			// 音乐或音效ID，SCN+音效ID 对应SoundTable.xml中的一条记录
CMD_END


#pragma pack(pop)