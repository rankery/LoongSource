//------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: longhun.h
// author: Sxg
// actor:
// data: 2009-3-17
// last:
// brief: 龙魂能力激活机制处理类 -- 基于装备先脱后穿
//------------------------------------------------------------------------
#pragma once

class Role;

struct tagEquip;
struct tagLongHunProto;
//------------------------------------------------------------------------
class LongHun
{
public:
	LongHun(Role *pRole);
	~LongHun();

	VOID Add(const tagEquip *pEquip, const INT16 nEquipPos, BOOL bSend2Client);
	VOID Remove(const INT16 nEquipPos);
	VOID Swap(const tagEquip *pEquip1, const tagEquip *pEquip2, INT16 n16SrcPos1, INT16 n16DstPos1);

public:
	// 获取已激活的指定部位的龙魂能力
	DWORD GetLongHunID(const INT16 n16EquipPos) const;
	DWORD GetRandomLongHunID();

	INT8 GetPassiveNum() const;
	INT8 GetActiveNum() const;
	VOID InitSendInitState(OUT INT16 n16Data[], INT8 n8Sz) const;

private:
	// 判断该件装备是可被否激活
	BOOL IsNeedCount(const INT16 n16EquipPos, const tagLongHunProto *pLongHunProto, const BOOL bOuterLongHun);

	// 计算龙附属性 -- 即五行属性
	VOID CalLongfuAtt(OUT BYTE byLongfuAtt[], const INT nSz, INT16 n16EquipPos);
	
	VOID Set(const tagEquip *pEquip, const INT16 n16EquipPos, BOOL bSend2Client);
	VOID Unset(const INT16 n16EquipPos, BOOL bSend2Client);

	// 将龙魂能力激活状态变化发送给客户端
	VOID SendAdd2Client(const INT16 n16EquipPos);
	VOID SendRemove2Client(const INT16 n16EquipPos);
	VOID SendEquipEffect2Client(const INT16 n16EquipPos);

private:
	Role			*m_pRole;

	// 已激活的龙魂属性: 被动触发，主动使用
	DWORD			m_dwPassive[EEP_MaxEquip];
	DWORD			m_dwActive[EEP_MaxEquip];
	TList<DWORD>	m_listActive;

	INT8			m_n8NumPassive;
	INT8			m_n8NumActive;
};

//***************************** 内联实现 *********************************

//------------------------------------------------------------------------
// 对换 -- 戒指间，武器间
//------------------------------------------------------------------------
inline VOID LongHun::Swap(const tagEquip *pEquip1, const tagEquip *pEquip2, 
						  INT16 n16SrcPos1, INT16 n16DstPos1)
{
	if(P_VALID(pEquip1))
	{
		Remove(n16SrcPos1);
	}
	
	if(P_VALID(pEquip2))
	{
		Remove(n16DstPos1);
	}
	
	Add(pEquip1, n16DstPos1, TRUE);
	Add(pEquip2, n16SrcPos1, TRUE);
}

//------------------------------------------------------------------------
// 获取被动龙魂能力的个数
//------------------------------------------------------------------------
inline INT8 LongHun::GetPassiveNum() const
{
	return m_n8NumPassive;
}

//------------------------------------------------------------------------
// 获得主动龙魂能力的个数
//------------------------------------------------------------------------
inline INT8 LongHun::GetActiveNum() const
{
	return m_n8NumActive;
}

//------------------------------------------------------------------------
// 获取已激活的指定部位的龙魂能力 -- 一件装备上只可能有一个龙魂能力
//------------------------------------------------------------------------
inline DWORD LongHun::GetLongHunID(const INT16 n16EquipPos) const
{
	if(m_dwActive[n16EquipPos] != GT_INVALID)
	{
		return m_dwActive[n16EquipPos];
	}
		
	if(m_dwPassive[n16EquipPos] != GT_INVALID)
	{
		return m_dwPassive[n16EquipPos];
	}

	return GT_INVALID;
}

//------------------------------------------------------------------------
// 随机取得一个主动激发龙魂ID
//------------------------------------------------------------------------
inline DWORD LongHun::GetRandomLongHunID()
{
	DWORD dwLongHunID = GT_INVALID;
	if (!m_listActive.RandPeek(dwLongHunID))
		return GT_INVALID;

	return dwLongHunID;
}