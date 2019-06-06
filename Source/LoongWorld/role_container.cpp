//--------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: role_container.h
// author: Sxg
// actor:
// data: 2008-8-1
// last:
// brief: 人物装备栏、行囊、仓库等处理方法
//--------------------------------------------------------------------------------------------
#include "StdAfx.h"
#include "role.h"
#include "creature.h"
#include "player_session.h"
#include "../WorldDefine/msg_item.h"
#include "../WorldDefine/base_define.h"
#include "../ServerDefine/log_cmdid_define.h"
#include "map.h"


//****************************** 换装 ******************************************

//--------------------------------------------------------------------------------------------
// 检查是否能够换上
//--------------------------------------------------------------------------------------------
DWORD Role::CanEquip(tagEquip *pEquip, EEquipPos ePosDst)
{
	// 判断装备是否鉴定
	/*if(!pEquip->bIdentified)*/
	if(!MIsIdentified(pEquip))
	{
		return E_Equip_NotIdentify;
	}

	// 判断人物等级是否符合
	if(m_nLevel < pEquip->equipSpec.byMinUseLevel 
		|| m_nLevel > pEquip->equipSpec.byMaxUseLevel)
	{
		return E_Item_LevelLimit;
	}

	const tagEquipProto *pEquipProto = pEquip->pEquipProto;

	// 判断职业限制
	if ( E_Success != VocationCheck( pEquipProto->dwVocationLimitWear ))
		return E_Equip_VocationLimit;

	/*INT nTmpClass =  1 << ( m_eClass - 1 );
	INT nTmpClassEx = 0;
	
	if ( (INT)m_eClassEx != (INT)EHV_Base )
	{
		nTmpClassEx = 1 << ( m_eClassEx + 8 );
	}


	if (!(( nTmpClass + nTmpClassEx) & pEquipProto->dwVocationLimitWear))
		return E_Equip_VocationLimit;*/
	
	// 判断角色性别
	if(pEquipProto->eSexLimit != ESL_Null 
		&& pEquipProto->eSexLimit != m_Avatar.bySex)
	{
		return E_Item_SexLimit;
	}

	FLOAT fFactor = 1 + pEquip->equipSpec.n16AttALimModPct / 10000.0f;

	// 判断属性限制 -- 原始一级属性即只计算玩家属性中的初始属性，升级自动加点和手动加点部分
	for(INT32 i=0; i<X_ERA_ATTA_NUM; ++i)
	{
		if(GetBaseAttValue(i) < (INT)(pEquipProto->n16AttALimit[i] * fFactor + pEquip->equipSpec.n16AttALimMod))
		{
			return E_Item_AttA_Limit;
		}
	}

	// 判断职业 //?? 临时注掉
	//if(!(pEquipProto->byClassLimit[m_eClass] & m_eClassEx))
	//{
	//	return E_Item_ClassLimit;
	//}

	// 判断氏族声望
	if(pEquipProto->eClanRepute >= ERT_BEGIN && pEquipProto->eClanRepute < ERT_END)
	{
		ECLanType eClanType = MTRANS_ERT2ECLT(pEquipProto->eClanRepute);
		if(GetClanData().RepGetVal(eClanType) < pEquipProto->nClanReputeVal)
		{
			return E_Item_ClanRepLimit;
		}
	}

	//// 其他声望类型值
	//if( 0 )
	//{
	//	return E_Item_OtherClanRepLimit;
	//}

	//// 武器挂载位置是否重复
	//if(MIsEquipment(pEquip->dwTypeID))
	//{
	//	tagEquip *pOtherWeapon = GetEquipBar().GetItem((INT16)(EEP_RightHand == ePosDst ? EEP_LeftHand : EEP_RightHand));
	//	if(!P_VALID(pOtherWeapon))
	//	{
	//		return E_Success;
	//	}

	//	MTRANS_POINTER(pEquipProto, pEquip->pProtoType, tagEquipProto);
	//	MTRANS_POINTER(pOtherProto, pOtherWeapon->pProtoType, tagEquipProto);

	//	if((EWP_WaistBack == pEquipProto->eWeaponPos || EWP_Back == pEquip->pEquipProto->eWeaponPos)
	//		&& pEquip->pEquipProto->eWeaponPos == pOtherProto->eWeaponPos)
	//	{
	//		return E_Equip_WeaponPos_Overlap;
	//	}
	//}

	return E_Success;
}

//--------------------------------------------------------------------------------------------
// 换上
//--------------------------------------------------------------------------------------------
DWORD Role::Equip(INT64 n64Serial, EEquipPos ePosDst)
{
	// 获得待穿装备
	tagItem *pItem = GetItemMgr().GetBagItem(n64Serial);
	if(!P_VALID(pItem))
	{
		return E_Item_NotFound;
	}

	// 判断欲装备物品是否为装备
	if(!MIsEquipment(pItem->dwTypeID))
	{
		return E_Item_NotEquipment;
	}

	tagEquip *pEquip = (tagEquip *)pItem;

	// 检查是否符合装备条件
	DWORD dwErrorCode = CanEquip(pEquip, ePosDst);
	if(dwErrorCode != E_Success)
	{
		return dwErrorCode;
	}

	// 获得当前装备位置的装备
	tagEquip *pEquipOld = GetItemMgr().GetEquipBarEquip((INT16)ePosDst);

	// 换装
	dwErrorCode = GetItemMgr().Equip(n64Serial, ePosDst);
	if(E_Success == dwErrorCode)
	{
		ProcEquipEffect(pEquip, pEquipOld, (INT16)ePosDst);
	}
	else
	{
		return dwErrorCode;
	}

	// 任务触发
	OnQuestEvent(EQE_Item, pItem->dwTypeID, 1, FALSE);

	return dwErrorCode;
}

//--------------------------------------------------------------------------------------------
// 脱下 -- 目标位置应为空，不空客户端应发换上装备消息
//--------------------------------------------------------------------------------------------
DWORD Role::Unequip(INT64 n64Serial, INT16 n16IndexDst)
{
	// 检查目标位置是否为空，不空就直接返回
	if(n16IndexDst != GT_INVALID && !GetItemMgr().IsBagOneSpaceFree(n16IndexDst))
	{
		return GT_INVALID;
	}

	// 获得待脱装备
	tagEquip *pEquipOld = GetItemMgr().GetEquipBarEquip(n64Serial);
	if(!P_VALID(pEquipOld))
	{
		return E_Item_NotFound;
	}

	INT16 n16IndexOld = pEquipOld->n16Index;

	// 换装
	if(GT_INVALID == n16IndexDst)
	{
		n16IndexDst = GetItemMgr().GetBagOneFreeSpace();
		if(GT_INVALID == n16IndexDst)
		{
			return E_Bag_NotEnoughSpace;
		}
	}

	DWORD dwErrorCode = GetItemMgr().Unequip(n64Serial, n16IndexDst);
	if(dwErrorCode != E_Success)
	{
		return dwErrorCode;
	}

	ProcEquipEffect(NULL, pEquipOld, n16IndexOld);

	// 任务触发
	OnQuestEvent(EQE_Item, pEquipOld->dwTypeID, 1, TRUE);

	return dwErrorCode;
}

//--------------------------------------------------------------------------------------------
// 主副手对换
//--------------------------------------------------------------------------------------------
DWORD Role::SwapWeapon()
{
	tagEquip *pEquipRight	= GetItemMgr().GetEquipBarEquip((INT16)EEP_RightHand);
	tagEquip *pEquipLeft	= GetItemMgr().GetEquipBarEquip((INT16)EEP_LeftHand);

	DWORD dwErrorCode = GetItemMgr().SwapWeapon();
	if(E_Success != dwErrorCode)
	{
		return dwErrorCode;
	}

	// 将装备位置信息发送给客户端
	ProcEquipEffectPos(pEquipRight, pEquipLeft, EICT_Equip, EICT_Equip);

	// 重新计算武器攻击 -- 先脱下，后换上
	if(P_VALID(pEquipRight))	// 有主手武器
	{
		ResetWeaponDmg(*pEquipRight, FALSE);
	}

	if(P_VALID(pEquipLeft))
	{
		ResetWeaponDmg(*pEquipLeft, TRUE);
	}

	// 龙魂能力相关重新计算 -- 先脱后穿：内部处理时，部分装备存在重复计算龙魂能力问题
	//m_LongHun.Remove(n16OldPos1);
	//m_LongHun.Remove(n16OldPos2);
	//m_LongHun.Add(pRing1, n16PosDst, TRUE);
	//m_LongHun.Add(pRing2, n16OldPos1, TRUE);
	m_LongHun.Swap(pEquipRight, pEquipLeft, (INT16)EEP_RightHand, (INT16)EEP_LeftHand);

	// 重新计算受影响的人物当前状态
	RecalAtt();

	// 将外观发给周围玩家
	ProcEquipEffectAvatar(pEquipLeft, EEP_RightHand);
	ProcEquipEffectAvatar(pEquipRight, EEP_LeftHand);

	return E_Success;
}

//--------------------------------------------------------------------------------------------
// 换戒指位置
//--------------------------------------------------------------------------------------------
DWORD Role::MoveRing(INT64 n64SerialSrc, INT16 n16PosDst)
{
	tagEquip *pRing1 = GetItemMgr().GetEquipBarEquip(n64SerialSrc);
	tagEquip *pRing2 = GetItemMgr().GetEquipBarEquip(n16PosDst);

	INT16 n16OldPos1 = EEP_Null;
	//INT16 n16OldPos2 = EEP_Null;

	if(P_VALID(pRing1))
	{
		n16OldPos1 = pRing1->n16Index;
	}

	//if(P_VALID(pRing2))
	//{
	//	n16OldPos2 = pRing2->n16Index;
	//}

	DWORD dwErrorCode = GetItemMgr().MoveRing(n64SerialSrc, n16PosDst);
	if(E_Success != dwErrorCode)
	{
		return dwErrorCode;
	}

	// 龙魂能力相关重新计算 -- 先脱后穿：内部处理时，部分装备存在重复计算龙魂能力问题
	//m_LongHun.Remove(n16OldPos1);
	//m_LongHun.Remove(n16OldPos2);
	//m_LongHun.Add(pRing1, n16PosDst, TRUE);
	//m_LongHun.Add(pRing2, n16OldPos1, TRUE);
	m_LongHun.Swap(pRing1, pRing2, n16OldPos1, n16PosDst);

	// 将装备位置信息发送给客户端
	ProcEquipEffectPos(pRing1, pRing2, EICT_Equip, EICT_Equip);

	return E_Success;
}

//--------------------------------------------------------------------------------------------
// 计算换装影响, 并发送到客户端
//--------------------------------------------------------------------------------------------
VOID Role::ProcEquipEffect(tagEquip *pNewEquip, tagEquip *pOldEquip, INT16 n16IndexOld, BOOL bDiscard/* = FALSE*/)
{
	// 将装备位置信息发送给客户端
	if(!bDiscard)
	{
		ProcEquipEffectPos(pNewEquip, pOldEquip, EICT_Equip, EICT_Bag);
	}

	// 更新装备影响人物属性 -- 注意要先脱再穿
	if(P_VALID(pOldEquip))
	{
		ProcEquipEffectAtt(pOldEquip, FALSE, n16IndexOld);
		m_Suit.Remove(pOldEquip, n16IndexOld);
		m_LongHun.Remove(n16IndexOld);
	}

	if(P_VALID(pNewEquip))
	{
		ProcEquipEffectAtt(pNewEquip, TRUE, n16IndexOld);
		m_Suit.Add(pNewEquip, n16IndexOld);
		m_LongHun.Add(pNewEquip, n16IndexOld, TRUE);
	}

	// 重新计算受影响的人物当前状态
	RecalAtt();

	// 将外观发给周围玩家
	ProcEquipEffectAvatar(pNewEquip, n16IndexOld);
}

//--------------------------------------------------------------------------------------------
// 计算换装影响, 将装备位置信息发送给客户端
//--------------------------------------------------------------------------------------------
VOID Role::ProcEquipEffectPos(tagEquip *pNewEquip, tagEquip *pOldEquip, EItemConType eConTypeNewDst, EItemConType eConTypeNewSrc)
{
	tagNS_ItemPosChangeEx sendPos;
	sendPos.eConTypeSrc1 = eConTypeNewSrc;
	sendPos.eConTypeSrc2 = eConTypeNewDst;
	sendPos.eConTypeDst1 = eConTypeNewDst;
	sendPos.eConTypeDst2 = eConTypeNewSrc;
	sendPos.n64Serial1 = GT_INVALID;
	sendPos.n64Serial2 = GT_INVALID;
	sendPos.n16Num1 = 1;
	sendPos.n16Num2 = 1;
	sendPos.n16PosDst1 = GT_INVALID;
	sendPos.n16PosDst2 = GT_INVALID;

	if(P_VALID(pNewEquip))
	{
		sendPos.n64Serial1 = pNewEquip->n64Serial;
		sendPos.n16PosDst1 = pNewEquip->n16Index;
	}

	if(P_VALID(pOldEquip))
	{
		sendPos.n64Serial2 = pOldEquip->n64Serial;
		sendPos.n16PosDst2 = pOldEquip->n16Index;
	}

	m_pSession->SendMessage(&sendPos, sendPos.dwSize);
}

//--------------------------------------------------------------------------------------------
// 计算换装影响的玩家属性
//--------------------------------------------------------------------------------------------
VOID Role::ProcEquipEffectAtt(tagEquip *pEquip, bool bEquip, const INT16 n16Index)
{
	ASSERT(P_VALID(pEquip));
	
	INT32 nFactor = 1;
	pFun_RegTriggerEquipSet pRegTriggerEquipSet = &Role::RegisterTriggerEquipSet;
	if(!bEquip)
	{
		nFactor = -1;
		pRegTriggerEquipSet = &Role::UnRegisterTriggerEquipSet;
	}

	const tagEquipProto *pEquipProto = pEquip->pEquipProto;

	// 武器
	if(EIT_Weapon == pEquipProto->eType)
	{
		// 只处理主手武器伤害
		if(n16Index == pEquipProto->eEquipPos)
		{
			// 重新设置武器攻击力(已把镌刻影响计算在内)
			ResetWeaponDmg(*pEquip, bEquip);
		}
	}
	else
	{
		// 镌刻
		ChangeRoleAtt(pEquip->equipSpec.nEngraveAtt, MAX_ROLEATT_ENGRAVE_EFFECT, ERA_EngraveAtt_Start, nFactor);
	}

	// 防具护甲
	ModAttModValue(ERA_Armor, pEquip->equipSpec.n16Armor * nFactor);

	// 与品级无关的属性加成
	ChangeRoleAtt(pEquipProto->BaseEffect, MAX_ROLEATT_BASE_EFFECT, nFactor);
	
	// 铭文
	ChangeRoleAtt(pEquip->equipSpec.PosyEffect, MAX_ROLEATT_POSY_EFFECT, nFactor);

	// 一级属性
	ChangeRoleAtt(pEquip->equipSpec.nRoleAttEffect, X_ERA_ATTA_NUM, ERA_AttA_Start, nFactor);

	// 烙印 -- 伤害减免相关属性
	ChangeRoleAtt(pEquip->equipSpec.nBrandAtt, X_ERA_DERATE_NUM, ERA_Derate_Start, nFactor);

	// 镶嵌
	ChangeRoleAtt(pEquip->equipSpec.dwHoleGemID, pEquip->equipSpec.byHoleNum, nFactor);

	// 龙附 -- 龙魂属性,即五行.灵魂锁链用
	
	// 是否有表,里龙魂能力(GT_INVALID表示没有) -- 由专门管理器在外层处理(同suit)

	// 特殊属性 -- 不会直接影响换装

	// 装备Buff等
	(this->*pRegTriggerEquipSet)(pEquipProto->dwTriggerID0, pEquipProto->dwBuffID0, n16Index);
	(this->*pRegTriggerEquipSet)(pEquipProto->dwTriggerID1, pEquipProto->dwBuffID1, n16Index);
	(this->*pRegTriggerEquipSet)(pEquipProto->dwTriggerID2, pEquipProto->dwBuffID2, n16Index);

	// 时装属性
	if(pEquip->equipSpec.n16Appearance > 0)
	{
		ModAttModValue(ERA_Appearance, pEquip->equipSpec.n16Appearance * nFactor);
	}

	if(pEquip->equipSpec.byRein != 0)
	{
		ModAttModValue(ERA_Rein, pEquip->equipSpec.byRein * nFactor);
	}

	if(pEquip->equipSpec.bySavvy != 0)
	{
		ModAttModValue(ERA_Savvy, pEquip->equipSpec.bySavvy * nFactor);
	}

	if(pEquip->equipSpec.byFortune != 0)
	{
		ModAttModValue(ERA_Fortune, pEquip->equipSpec.byFortune * nFactor);
	}
}

//--------------------------------------------------------------------------------------------
// 计算换装影响的玩家外观, 并发送到客户端
//--------------------------------------------------------------------------------------------
VOID Role::ProcEquipEffectAvatar(tagEquip *pNewEquip, INT16 n16IndexOld)
{
	Map *pMap = GetMap();
	if(!P_VALID(pMap))
	{
		ASSERT(pMap);
		return;
	}

	// 重置装备显示
	INT32 nPos = ResetOneEquipDisplay(pNewEquip, n16IndexOld);
	if(!GT_VALID(nPos))
	{
		return;
	}

	tagNS_AvatarEquipChange send;
	send.dwRoleID		= GetID();
	send.EquipDisplay	= GetAvatarEquip().AvatarEquip[nPos];
	
	pMap->SendBigVisTileMsg(this, &send, send.dwSize);
}

//--------------------------------------------------------------------------------------------
// 重置装备显示，并返回显示部位枚举值。GT_INVALID表示该装备不用显示
//--------------------------------------------------------------------------------------------
INT32 Role::ResetOneEquipDisplay(tagEquip* pEquip, INT16 n16Index)
{
	INT32 nPos = GetEquipDisplayPos(n16Index);
	if(!GT_VALID(nPos))
	{
		return GT_INVALID;
	}

	// 判断是时装，还是装备
	bool bFashion = true;
	if(n16Index >= EEP_Equip_Start && n16Index <= EEP_Equip_End)
	{
		bFashion = FALSE;
	}

	if(!P_VALID(pEquip))
	{		
		SetEquipDisplay(bFashion, nPos, GT_INVALID, X_DEFAULT_FLARE_VAL, EC_Null);
	}
	else
	{
		// 处理挂载效果 -- 龙魂激活特效单独在管理器中处理
		SetEquipDisplay(bFashion, nPos, pEquip->dwTypeID, 
						pEquip->equipSpec.byFlareVal, pEquip->equipSpec.n8ColorID);
	}

	// 判断该外观是否需要显示
	if(GetDisplaySet().bFashionDisplay != bFashion && !MIsWeaponByDisplayPos(nPos))
	{
		nPos = GT_INVALID;
	}

	return nPos;
}

//--------------------------------------------------------------------------------------------
// 判断装备栏位置上装备是否需要显示
//--------------------------------------------------------------------------------------------
INT32 Role::GetEquipDisplayPos(INT16 n16EquipPos)
{
	// 饰品不需要显示
	if(EEP_Finger1 == n16EquipPos || EEP_Finger2 == n16EquipPos
		|| EEP_Waist == n16EquipPos || EEP_Neck == n16EquipPos)
	{
		return GT_INVALID;
	}

	// 转换为显示位置
	switch(n16EquipPos)
	{
	// 武器
	case EEP_RightHand:
		return EAE_RWeapon;
		break;
	case EEP_LeftHand:
		return EAE_LWeapon;
		break;

	// 时装
	case EEP_FashionHead:
		return EAE_Head;
		break;
	case EEP_FashionFace:
		return EAE_Face;
		break;
	case EEP_FashionBody:
		return EAE_Upper;
		break;
	case EEP_FashionLegs:
		return EAE_Lower;
		break;
	case EEP_FashionBack:
		return EAE_Back;
		break;
	case EEP_FashionWrist:
		return EAE_Glove;
		break;
	case EEP_FashionFeet:
		return EAE_Boot;
		break;

	// 防具
	case EEP_Head:
		return EAE_Head;
		break;
	case EEP_Face:
		return EAE_Face;
		break;
	case EEP_Body:
		return EAE_Upper;
		break;
	case EEP_Legs:
		return EAE_Lower;
		break;
	case EEP_Back:
		return EAE_Back;
		break;
	case EEP_Wrist:
		return EAE_Glove;
		break;
	case EEP_Feet:
		return EAE_Boot;
		break;
	}

	return GT_INVALID;
}

//--------------------------------------------------------------------------------------------
// 计算磨损对武器伤害影响.返回值为对武器伤害影响的百分比值.
//--------------------------------------------------------------------------------------------
FLOAT Role::CalAbrasionEffect(const tagEquip &Equip)
{
	INT32 nCurNewness = Equip.pEquipProto->n16Newness 
							- Equip.nUseTimes / ABRASION2USETIMES;
	nCurNewness = max(nCurNewness, 0);

	/*	A为武器当前崭新度，B为武器原始伤害
		A=200-999	武器伤害（武魂）加成 = 8% * B
		A=121-199	武器伤害（武魂）加成 =（A-120）* 0.1% * B
		A=80-120	武器伤害（武魂）加成 = 0
		A=50-79		武器伤害（武魂）加成 = -5% * B
		A=20-49		武器伤害（武魂）加成 = -15% * B
		A=5-19		武器伤害（武魂）加成 = -25% * B
		A=0-4		武器伤害（武魂）加成 = -50% * B	*/

	if(nCurNewness >= 200)
	{
		return 0.08f;
	}

	if(nCurNewness >= 121)
	{
		return (nCurNewness - 120) * 0.001f;
	}

	if(nCurNewness >= 80)
	{
		return 0.0f;
	}

	if(nCurNewness >= 50)
	{
		return -0.05f;
	}

	if(nCurNewness >= 20)
	{
		return -0.15f;
	}

	if(nCurNewness >= 5)
	{
		return -0.25f;
	}

	return -0.5f;
}

//--------------------------------------------------------------------------------------------
// 设置武器伤害 -- 换装时调用.
//--------------------------------------------------------------------------------------------
VOID Role::ResetWeaponDmg(const tagEquip &Equip, BOOL bEquip)
{
	if(bEquip)	// 穿上
	{
		FLOAT fDmgPct = 1.0f + CalAbrasionEffect(Equip);

		SetBaseAttValue(ERA_WeaponDmgMin, (INT)(Equip.equipSpec.n16MinDmg * fDmgPct));
		SetBaseAttValue(ERA_WeaponDmgMax, (INT)(Equip.equipSpec.n16MaxDmg * fDmgPct));
		SetBaseAttValue(ERA_WeaponSoul, (INT)(Equip.equipSpec.n16WuHun * fDmgPct));

		// 镌刻
		ChangeRoleAtt(Equip.equipSpec.nEngraveAtt, MAX_ROLEATT_ENGRAVE_EFFECT, ERA_EngraveAtt_Start, 1);
	}
	else	// 脱下
	{
		SetBaseAttValue(ERA_WeaponDmgMin, 0);
		SetBaseAttValue(ERA_WeaponDmgMax, 0);
		SetBaseAttValue(ERA_WeaponSoul, 0);

		// 镌刻
		ChangeRoleAtt(Equip.equipSpec.nEngraveAtt, MAX_ROLEATT_ENGRAVE_EFFECT, ERA_EngraveAtt_Start, -1);
	}
}

//--------------------------------------------------------------------------------------------
// 设置武器伤害 -- 崭新度变化时调用.
//--------------------------------------------------------------------------------------------
VOID Role::ResetWeaponDmg(tagEquip &Equip)
{
	// 计算崭新度变化之前对武器伤害影响
	--Equip.nUseTimes;
	FLOAT fDmgPctPre = CalAbrasionEffect(Equip);

	// 当前崭新度对武器伤害影响
	++Equip.nUseTimes;
	FLOAT fDmgPct = CalAbrasionEffect(Equip);

	// 影响不同，需重新设置
	if(fDmgPctPre != fDmgPct)
	{
		fDmgPct += 1.0f;

		SetBaseAttValue(ERA_WeaponDmgMin, (INT)(Equip.equipSpec.n16MinDmg * fDmgPct));
		SetBaseAttValue(ERA_WeaponDmgMax, (INT)(Equip.equipSpec.n16MaxDmg * fDmgPct));
		SetBaseAttValue(ERA_WeaponSoul, (INT)(Equip.equipSpec.n16WuHun * fDmgPct));
	}
}

//--------------------------------------------------------------------------------------------
// 计算装备属性影响,并修改对应的属性加成值.
//--------------------------------------------------------------------------------------------
VOID Role::ChangeRoleAtt(const tagRoleAttEffect Effect[], INT32 nArraySz, INT32 nFactor)
{
	for(INT32 i=0; i<nArraySz; ++i)
	{
		if(Effect[i].eRoleAtt <= ERA_Null || Effect[i].eRoleAtt >= ERA_End)
		{
			//ASSERT(Effect[i].eRoleAtt == ERA_Null);
			return;
		}

		if(MIsValuePct(Effect[i].nValue))		// 百分数
		{
			ModAttModValuePct(Effect[i].eRoleAtt, MValuePctTrans(Effect[i].nValue) * nFactor);
		}
		else	// 数值
		{
			ModAttModValue(Effect[i].eRoleAtt, Effect[i].nValue * nFactor);
		}
	}
}

//--------------------------------------------------------------------------------------------
// 计算装备属性影响,并修改对应的属性加成值.
//--------------------------------------------------------------------------------------------
VOID Role::ChangeRoleAtt(const INT32 nValue[], INT32 nArraySz, INT32 nAttStart, INT32 nFactor)
{
	ASSERT(nAttStart > ERA_Null && nAttStart + nArraySz < ERA_End);
	
	for(INT32 i=0; i<nArraySz; ++i)
	{
		if(MIsValuePct(nValue[i]))		// 百分数
		{
			ModAttModValuePct(nAttStart + i, MValuePctTrans(nValue[i]) * nFactor);
		}
		else	// 数值
		{
			ModAttModValue(nAttStart + i, nValue[i] * nFactor);
		}
	}
}

//--------------------------------------------------------------------------------------------
// 计算装备属性影响,并修改对应的属性加成值. -- 宝石影响
//--------------------------------------------------------------------------------------------
VOID Role::ChangeRoleAtt(const DWORD dwValue[], INT32 nArraySz, INT32 nFactor)
{
	ASSERT(nArraySz <= MAX_EQUIPHOLE_NUM);

	for(INT32 i=0; i<nArraySz; ++i)
	{
		if(0 == dwValue[i] || ERA_Null == dwValue[i])
		{
			break;
		}

		const tagConsolidateItem *pGemProto = g_attRes.GetConsolidateProto(dwValue[i]);
		if(!P_VALID(pGemProto))
		{
			ASSERT(P_VALID(pGemProto));
			continue;
		}

		ChangeRoleAtt((tagRoleAttEffect*)pGemProto->tagRoleAtt, MAX_CONSOLIDATE_ROLEATT, nFactor);
	}
}

//--------------------------------------------------------------------------------------------
// 装备类buff处理
//--------------------------------------------------------------------------------------------
VOID Role::ProcEquipBuffTrigger(DWORD dwBuffID, BOOL bEquip)
{
	INT nTmp;
	ERoleAttribute	eTmp;
	INT nFactor = (bEquip) ? 1 : -1;
	
	const tagBuffProto *pBuffProto = g_attRes.GetBuffProto(dwBuffID);
	if( !P_VALID(pBuffProto) )
	{
		MAttResCaution(_T("buff proto"), _T("BuffID"), dwBuffID);
		return;
	}

	if( pBuffProto->mapRoleAttMod.Size() > 0 )
	{
		TMap<ERoleAttribute, INT>::TMapIterator iter = pBuffProto->mapRoleAttMod.Begin();
		while( pBuffProto->mapRoleAttMod.PeekNext(iter, eTmp, nTmp) )
		{
			ModAttModValue(eTmp, nTmp * nFactor);
		}
	}

	if( pBuffProto->mapRoleAttModPct.Size() > 0 )
	{
		TMap<ERoleAttribute, INT>::TMapIterator iter = pBuffProto->mapRoleAttModPct.Begin();
		while( pBuffProto->mapRoleAttModPct.PeekNext(iter, eTmp, nTmp) )
		{
			ModAttModValuePct(eTmp, nTmp * nFactor);
		}
	}
}

// 角色采集技能加成
INT Role::CalGatherRate( Creature* pCreature )
{
	Skill* pGatherSkill = NULL;
	if ( pCreature->IsNatuRes() )
		pGatherSkill = GetSkill(GATHER_SKILL_MINING);
	else if ( pCreature->IsManRes() )
		pGatherSkill = GetSkill(GATHER_SKILL_HARVEST);
	if ( !P_VALID(pGatherSkill) )
		return 0;

	INT nSkillLvl = pGatherSkill->GetLevel();

	if (nSkillLvl <= 0)
		return 0;

	// 得到收获角色福缘 	
	FLOAT fFortune = (FLOAT)GetAttValue(ERA_Fortune);

	// 计算产出率加成	产出概率加成=5%×（收获技能等级-1）/10+角色福缘/1000
	// Jason 2009-11-28
	// 产出概率加成=20%×（开采技能等级-1）/10+角色福缘/1000
	return 0.2 * (nSkillLvl - 1) * 0.1 + fFortune / 1000;
	//INT nAddRatPct = INT((5.0f * FLOAT(nSkillLvl - 1) + fFortune) * 10.0f);

	//return nAddRatPct;
};

// 经验实际值（经任务打怪经验加成属性计算）
INT Role::CalRealGainExp( INT nSrcExp )
{
	// 防沉迷收益率
	FLOAT fEarnRate		= GetEarnRate() / 10000.0f;

	// gm收益率
	FLOAT fGMRate		= GetMap()->GetExpRate();

	// role本身属性
	FLOAT fRoleRate		= 1 + GetAttValue(ERA_Exp_Add_Rate)/10000.0f + GetVNBExpRate();

	// 原经验/金钱/掉率 * (1+加成/10000)
	FLOAT fExpRate	= max(fGMRate, fRoleRate);

	return INT(nSrcExp * fEarnRate * fExpRate);
	
}

// 打开随身仓库
DWORD Role::OpenSideWare(INT64 n64ItemSerial)
{
	tagItem *pItem = GetItemMgr().GetBagItem(n64ItemSerial);
	if(!P_VALID(pItem))
	{
		return E_UseItem_ItemNotExist;
	}

	if(pItem->pProtoType->eSpecFunc != EISF_PortableWare)
	{
		return E_UseItem_SpecFunError;
	}

	// 消耗物品
	//DWORD dwErrorCode = GetItemMgr().ItemUsedFromBag(n64ItemSerial, 1, ELCLD_Item_Use);
	//if(dwErrorCode != E_Success)
	//{
	//	return E_UseSkill_CostLimit;
	//}

	return E_Success;
}
