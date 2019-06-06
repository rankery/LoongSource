//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: loong_common.cpp
// author: Sxg
// actor:
// data: 2008-06-04
// last:
// brief: 处理从游戏服务器接收到的消息 -- account_common表相关
//-------------------------------------------------------------------------------------------------------
#include "StdAfx.h"

#include "Loong.h"
#include "LoongDB.h"
#include "../ServerDefine/msg_common.h"
//-------------------------------------------------------------------------------------------------------
// 设置安全码
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::SetSafeCode(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_SafeCodeSet);

	if(!m_pDB->UpdateSafeCode(p->dwAccountID, p->dwSafeCodeCrc))
	{
		return GT_INVALID;
	}

	return E_Success;
}

//-------------------------------------------------------------------------------------------------------
// 重置安全码
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::ResetSafeCode(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_SafeCodeReset);

	if(!m_pDB->UpdateSafeCodeResetTime(p->dwAccountID, p->dwTimeReset))
	{
		return GT_INVALID;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 取消安全码重置
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::CancelSafeCodeReset(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_SafeCodeResetCancel);

	if(!m_pDB->UpdateSafeCodeResetTime(p->dwAccountID, GT_INVALID))
	{
		return GT_INVALID;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 修改帐号行囊密码
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::ChangeBagPsd(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_ChangeBagPsd);

	if(!m_pDB->UpdateBagPsd(p->dwAccountID, p->dwNewBagPsd))
	{
		return GT_INVALID;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 更新仓库大小
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::UpdateWareSize(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_WareSizeUpdate);

	if(!m_pDB->UpdateWareSize(p->dwAccountID, &p->n16WareSize))
	{
		return GT_INVALID;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 更新仓库金钱
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::UpdateWareMoney(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_WareMoneyUpdate);

	if(!m_pDB->UpdateWareMoney(p->dwAccountID, p->n64WareSilver))
	{
		return GT_INVALID;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 更新百宝袋元宝
//-------------------------------------------------------------------------------------------------------
DWORD CLoong::UpdateBaiBaoYuanBao(DWORD pDBMsg, DWORD dwDummy)
{
	MGET_MSG(p, pDBMsg, NDBC_BaiBaoYBUpdate);

	if(!m_pDB->UpdateBaiBaoYuanBao(p->dwAccountID, &p->nBaiBaoYuanBao))
	{
		return GT_INVALID;
	}

	return 0;
}