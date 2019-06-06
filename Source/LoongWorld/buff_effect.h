//-------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: buff_effect.h
// author: Aslan
// actor:
// data: 2008-11-26
// last:
// brief: 状态特殊效果
//-------------------------------------------------------------------------------
#pragma once


class Unit;
class BuffEffect;

typedef VOID (*BUFFEFFECTRUTINE)(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet, const tagBuffProto* pProto);

class BuffEffect
{
public:
	static VOID Init();

	//------------------------------------------------------------------------------
	// 注册Buff特殊效果处理函数
	//------------------------------------------------------------------------------
	static VOID RegisterBuffEffectRutine();

	//-------------------------------------------------------------------------------
	// 计算Buff效果
	//-------------------------------------------------------------------------------
	static VOID CalBuffEffect(Unit* pTarget, Unit* pSrc, EBuffEffectType eEffect, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);

private:
	//-----------------------------------------------------------------------------------------------------------------
	// 无
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectNull(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 瞬移
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectTeleport(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 打断起手
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectInterruptSpell(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 眩晕
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectDizzy(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 击退
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectRepel(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 冲锋
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectAssault(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 封技
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectNoSkill(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 昏睡
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectSpor(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 定身
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectTie(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 驱散
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectDispel(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 去除
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectCancel(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);

	//-----------------------------------------------------------------------------------------------------------------
	// 无敌
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectInvincible(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 即死
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectInstantDie(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 拖拽
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectPull(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 转魂
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectHPTransfer(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 转精
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectMPTransfer(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 复活
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectRevive(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 瞬间冷却
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectInstantCD(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 隐身
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectLurk(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 飞行
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectFly(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 改变仇恨
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectChangeEnmity(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 传送
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectTransmit(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 采集
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectGather(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 缴械
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectDisArm(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 交换位置
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectExchangePos(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 引爆
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectExplode(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 恐惧
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectFunk(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 追踪
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectPursue(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 无起手时间
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectNoPrepare(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 水上行走
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectOnWater(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	//-----------------------------------------------------------------------------------------------------------------
	// 移动掉血
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectMoveHPDmg(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);

	//-----------------------------------------------------------------------------------------------------------------
	// 无视护甲
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectIgnoreArmor(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);

	//-----------------------------------------------------------------------------------------------------------------
	// 嘲讽
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectSneer(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);

	//-----------------------------------------------------------------------------------------------------------------
	// 骑乘
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectMount(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);

	//-----------------------------------------------------------------------------------------------------------------
	// 邀请骑乘
	//-----------------------------------------------------------------------------------------------------------------
	static VOID BuffEffectMountInvite(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);


private:
	static VOID GatherEffect(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	static VOID InvesEffect(Unit* pTarget, Unit* pSrc, DWORD dwEffectMisc1, DWORD dwEffectMisc2, BOOL bSet=TRUE, const tagBuffProto* pProto=NULL);
	
	
	//  辅助函数
protected:
	// 释放完buffer怪物是否继续攻击玩家
	static BOOL EndbuffContinueIsAttack(Unit* pTarget, Unit* pSrc, BOOL bSet); 

private:
	static BUFFEFFECTRUTINE	m_Effect[EBET_End];
};