//--------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: suit.h
// author: Sxg
// actor:
// data: 2008-12-2
// last:
// brief: 套装类
//--------------------------------------------------------------------------------------------
#pragma once

struct tagEquip;

class Role;
//--------------------------------------------------------------------------------------------

/******************************* 类定义 **********************************/
class Suit
{
public:
	Suit(Role *pRole);
	~Suit();

	VOID Add(const tagEquip *pEquip, INT16 n16EquipPos, BOOL bSend2Client = TRUE);
	VOID Remove(const tagEquip *pEquip, INT16 n16OldIndex);

public:
	INT GetSuitNum();
	INT GetSuitEquipNum(DWORD dwSuitID);
	VOID InitSendInitState(BYTE *pData);

private:
	//--------------------------------------------------------------------------------------------
	// 判断该件套装部件是否需要统计
	//--------------------------------------------------------------------------------------------
	BOOL IsNeedCount(const tagEquip *pEquip, const tagEquipProto *pEquipProto, 
					INT nSuitIndex, INT16 n16IndexOther);

	//--------------------------------------------------------------------------------------------
	// 判断是否是套装部件
	//--------------------------------------------------------------------------------------------
	BOOL IsSuitPart(const tagEquip *pEquip, const tagEquipProto *pEquipProto, INT nSuitIndex, INT16 n16OldIndex);

	//--------------------------------------------------------------------------------------------
	// 如果是武器或者戒指，得到另一个件的位置
	//--------------------------------------------------------------------------------------------
	INT16 GetOtherEquipPos(const tagEquip *pEquip, INT16 n16OldIndex);

private:
	Role				*m_pRole;
	TMap<DWORD, INT>	m_mapSuitNum;	// <dwSuitID, nEquipNum>
};


/******************************* 内联实现 **********************************/

//--------------------------------------------------------------------------------------------
// 套装套数
//--------------------------------------------------------------------------------------------
inline INT Suit::GetSuitNum()
{
	return m_mapSuitNum.Size();
}

//--------------------------------------------------------------------------------------------
// 某套套装已装备件数
//--------------------------------------------------------------------------------------------
inline INT Suit::GetSuitEquipNum(DWORD dwSuitID)
{
	return m_mapSuitNum.Peek(dwSuitID);
}

//--------------------------------------------------------------------------------------------
// 判断是否是套装部件
//--------------------------------------------------------------------------------------------
inline BOOL Suit::IsSuitPart(const tagEquip *pEquip, const tagEquipProto *pEquipProto, 
							 INT nSuitIndex, INT16 n16OldIndex)
{
	// 判断最低品级
	if(pEquipProto->bySuitMinQlty[nSuitIndex] > pEquip->equipSpec.byQuality)
	{
		return FALSE;
	}

	// 如果是武器或者戒指，还需要判断另一个装备位置装备
	INT16 n16IndexOther = GetOtherEquipPos(pEquip, n16OldIndex);
	if(n16IndexOther != GT_INVALID
		&& !IsNeedCount(pEquip, pEquipProto, nSuitIndex, n16IndexOther))
	{
		return FALSE;
	}

	return TRUE;
}

//--------------------------------------------------------------------------------------------
// 如果是武器或者戒指，得到另一个件的位置
//--------------------------------------------------------------------------------------------
inline INT16 Suit::GetOtherEquipPos(const tagEquip *pEquip, INT16 n16OldIndex)
{
	if(MIsRing(pEquip))
	{
		return (EEP_Finger1 == n16OldIndex ? EEP_Finger2 : EEP_Finger1);
	}
	else if(MIsWeapon(pEquip))
	{
		return (EEP_RightHand == n16OldIndex ? EEP_LeftHand : EEP_RightHand);
	}

	return GT_INVALID;
}