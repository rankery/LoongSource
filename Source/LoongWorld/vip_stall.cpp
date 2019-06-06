//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: vip_stall.cpp
// author: sunnee
// actor:
// data: 2009-06-15
// last:
// brief: VIP摊位类
//-------------------------------------------------------------------------------------------------------
#include "StdAfx.h"

#include "../ServerDefine/log_cmdid_define.h"
#include "../ServerDefine/msg_vipstall.h"

#include "../WorldDefine/msg_stall.h"

#include "world.h"
#include "db_session.h"
#include "role_mgr.h"
#include "role.h"
#include "vip_stall.h"

//-------------------------------------------------------------------------------------------------------
// 常量
//-------------------------------------------------------------------------------------------------------
const DWORD MAX_PERIOD_OF_VALIDITY = 60 * 60 * 24;

//-------------------------------------------------------------------------------------------------------
// 构造析构函数
//-------------------------------------------------------------------------------------------------------
VIPStall::VIPStall()
{
	m_bInit = FALSE;
	m_vecVIPStall.clear();
	m_mapVIPStall.Clear();

	m_dwSecTimer	= g_world.GetWorldTime();
	m_nMinuteTimer	= 0;
}

VIPStall::~VIPStall()
{
	Destroy();
}

//-------------------------------------------------------------------------------------------------------
// 初始化，更新，销毁
//-------------------------------------------------------------------------------------------------------
BOOL VIPStall::Init()
{
	m_vecVIPStall.resize(VIP_STALL_MAX_NUM);

	// 载入摊位租金信息
	for (int n=0; n<VIP_STALL_MAX_NUM; n++)
	{
		m_vecVIPStall[n].nRent = g_attRes.GetVIPStallRent(n);
	}

	RegisterStallEventFunc();

	return TRUE;
}

VOID VIPStall::Update()
{
	// 初始化判断
	if (!m_bInit)
		return;

	// 处理事件
	EventMgr<VIPStall>::Update();

	// 摊位到期处理
	if (g_world.GetWorldTime() != m_dwSecTimer)
	{
		m_dwSecTimer = g_world.GetWorldTime();

		for (int n=0; n<VIP_STALL_MAX_NUM; n++)
		{
			if (!GT_VALID(m_vecVIPStall[n].dwOwnerID))
				continue;

			m_vecVIPStall[n].nRemainTime--;
			if (m_vecVIPStall[n].nRemainTime <= 0)
			{
				// 重置摊位信息
				m_mapVIPStall.Erase(m_vecVIPStall[n].dwOwnerID);

				m_vecVIPStall[n].Reset();
				m_vecVIPStall[n].dwLastUpdateTime = m_dwSecTimer;
				m_dwVIPStallChange = m_dwSecTimer;

				// 通知数据库
				tagNDBC_ApplyVIPStall send;
				send.VipStallInfo.byStallIndex	= n;
				send.VipStallInfo.dwOwnerID		= GT_INVALID;
				send.VipStallInfo.nRemainTime	= GT_INVALID;
				g_dbSession.Send(&send, send.dwSize);
			}
		}

		// 每分钟发送时间更新信息
		if (++m_nMinuteTimer > 60)
		{
			tagNDBC_UpdateVIPStall send;
			g_dbSession.Send(&send, send.dwSize);
			m_nMinuteTimer = 0;
		}
	}
}

VOID VIPStall::Destroy()
{
	m_vecVIPStall.clear();
	m_mapVIPStall.Clear();
	m_bInit = FALSE;
}

//-------------------------------------------------------------------------------------------------------
// 事件注册和处理
//-------------------------------------------------------------------------------------------------------
VOID VIPStall::RegisterStallEventFunc()
{
	RegisterEventFunc(EVT_SetStallTitle,			&VIPStall::OnSetStallTitleFunc);
	RegisterEventFunc(EVT_SetStallStatus,			&VIPStall::OnSetStallStatusFunc);
	RegisterEventFunc(EVT_ChangeStallGoods,			&VIPStall::OnChangeStallGoodsFunc);
}

VOID VIPStall::OnSetStallTitleFunc( DWORD dwSenderID, VOID* pEventMessage )
{
	// pEventMessage <==> TCHAR*

	tagVIPStall* pInfo = GetVIPStallInfo(dwSenderID);

	if (!P_VALID(pInfo))
		return;

	// 上层保证了字符串的结束符
	TCHAR* pTitle = (TCHAR*)pEventMessage;

	// 截取最大限度的字符串
	_tcsncpy(pInfo->szStallTitle, pTitle, STALL_MAX_TITLE_NUM-1);

	// 重置结束符
	pInfo->szStallTitle[STALL_MAX_TITLE_NUM-1] = '\0';

	pInfo->dwLastUpdateTime = m_dwSecTimer;
	m_dwVIPStallChange		= m_dwSecTimer;
}

VOID VIPStall::OnSetStallStatusFunc( DWORD dwSenderID, VOID* pEventMessage )
{
	// pEventMessage <==> BYTE*

	tagVIPStall* pInfo = GetVIPStallInfo(dwSenderID);

	if (!P_VALID(pInfo))
		return;

	pInfo->eStallStatus		= (EVIPStallStatus)(*((BYTE*)pEventMessage));
	pInfo->dwLastUpdateTime	= m_dwSecTimer;
	m_dwVIPStallChange		= m_dwSecTimer;
}

VOID VIPStall::OnChangeStallGoodsFunc( DWORD dwSenderID, VOID* pEventMessage )
{
	// pEventMessage <==> NULL

	tagVIPStall* pInfo = GetVIPStallInfo(dwSenderID);

	if (!P_VALID(pInfo))
		return;

	pInfo->dwLastGoodsTime = m_dwSecTimer;
}

//-------------------------------------------------------------------------------------------------------
// 从数据库获取VIP摊位信息
//-------------------------------------------------------------------------------------------------------
DWORD VIPStall::LoadAllVIPStallInfo( tagVipStall2DB* pVIPStallInfo )
{
	ASSERT(pVIPStallInfo);

	if (!P_VALID(pVIPStallInfo))
		return GT_INVALID;

	for (int n=0; n<VIP_STALL_MAX_NUM; n++)
	{
		m_vecVIPStall[n].byStallIndex		= pVIPStallInfo[n].byStallIndex;
		m_vecVIPStall[n].dwOwnerID			= pVIPStallInfo[n].dwOwnerID;
		m_vecVIPStall[n].nRemainTime		= pVIPStallInfo[n].nRemainTime;
		m_vecVIPStall[n].dwLastUpdateTime	= m_dwSecTimer;
		m_dwVIPStallChange					= m_dwSecTimer;

		if (GT_VALID(pVIPStallInfo[n].dwOwnerID))
		{
			m_mapVIPStall.Add(pVIPStallInfo[n].dwOwnerID, n);

			Role* pRole = g_roleMgr.GetRolePtrByID(pVIPStallInfo[n].dwOwnerID);
			if (P_VALID(pRole) && pRole->IsInRoleState(ERS_Stall))
			{
				m_vecVIPStall[n].eStallStatus = EVSS_Open;
				m_vecVIPStall[n].dwLastGoodsTime = m_dwSecTimer;
				pRole->GetStallTitle(m_vecVIPStall[n].szStallTitle);
			}
			else
			{
				m_vecVIPStall[n].eStallStatus = EVSS_Close;
			}
		}
		else
		{
			m_vecVIPStall[n].eStallStatus = EVSS_ForHire;
		}
	}
	m_bInit = TRUE;

	return 0;
}

//-------------------------------------------------------------------------------------------------------
// 取得VIP摊位概要信息
//-------------------------------------------------------------------------------------------------------
tagVIPStall* VIPStall::GetVIPStallInfo( BYTE index )
{
	//ASSERT( index >=0 && index < VIP_STALL_MAX_NUM);

	if (index < 0 || index >= VIP_STALL_MAX_NUM)
		return NULL;

	if (!m_bInit)	return NULL;
	
	return &(m_vecVIPStall[index]);
}

tagVIPStall* VIPStall::GetVIPStallInfo( DWORD dwRoleID )
{
	if (!GT_VALID(dwRoleID))
		return NULL;

	if(!m_bInit)	
		return NULL;

	BYTE byIndex = m_mapVIPStall.Peek(dwRoleID);

	if (!GT_VALID(byIndex))
		return NULL;
	
	return GetVIPStallInfo(byIndex);
}

Role* VIPStall::GetVIPStallRole( BYTE index )
{
	tagVIPStall* pInfo = GetVIPStallInfo(index);

	// 摊位不存在
	if (!P_VALID(pInfo))
		return NULL;

	// 摊位无主
	if (!GT_VALID(pInfo->dwOwnerID))
		return NULL;

	return g_roleMgr.GetRolePtrByID(pInfo->dwOwnerID);
}

BOOL VIPStall::IsInStatus( BYTE index, EVIPStallStatus eStatus )
{
	tagVIPStall* pInfo = GetVIPStallInfo(index);
	if (!P_VALID(pInfo))
		return FALSE;

	if (pInfo->eStallStatus == eStatus)
		return TRUE;

	return FALSE;
}

DWORD VIPStall::GetSpecVIPStallGoodsTime( BYTE index )
{
	tagVIPStall* pInfo = GetVIPStallInfo(index);
	if (!P_VALID(pInfo))
		return GT_INVALID;

	return pInfo->dwLastGoodsTime;
}

DWORD VIPStall::SetVIPStallStatus( BYTE index, EVIPStallStatus eStatus )
{
	tagVIPStall* pInfo = GetVIPStallInfo(index);
	if (!P_VALID(pInfo))
	{
		return GT_INVALID;
	}

	pInfo->eStallStatus = eStatus;

	return E_Success;
}

//-------------------------------------------------------------------------------------------------------
// 地图上层消息处理
//-------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------
// 申请VIP摊位
//-------------------------------------------------------------------------------------------------------
DWORD VIPStall::ApplyVIPStall( Role* pRole, BYTE byIndex, INT32 nRent )
{
	if (!P_VALID(pRole))
		return GT_INVALID;

	if (!m_bInit)
		return E_Stall_VIP_Not_Init;

	// 检测该玩家是否已经租赁过VIP摊位
	if (m_mapVIPStall.IsExist(pRole->GetID()))
	{
		return E_Stall_VIP_Own_Another;
	}

	// 取出摊位信息
	tagVIPStall* pInfo = GetVIPStallInfo(byIndex);
	if (!P_VALID(pInfo))
		return E_Stall_VIP_Info_NotExist;

	// 检测该摊位是否已经被租赁
	if (GT_VALID(pInfo->dwOwnerID))
	{
		return E_Stall_VIP_Already_Rent;
	}

	if (pInfo->nRent != nRent)
		return E_Stall_VIP_Rent_Error;

	// 检查玩家元宝是否足够
	if(nRent > pRole->GetCurMgr().GetBagYuanBao() || nRent <= 0)
	{
		return E_BagYuanBao_NotEnough;
	}

	// 扣除玩家元宝
	pRole->GetCurMgr().DecBagYuanBao(nRent, ELCLD_VIP_Stall_Rent);

	// 申请成功
	pInfo->dwOwnerID		= pRole->GetID();
	pInfo->nRemainTime		= MAX_PERIOD_OF_VALIDITY;
	pInfo->dwLastUpdateTime	= m_dwSecTimer;
	m_dwVIPStallChange		= m_dwSecTimer;

	m_mapVIPStall.Add(pInfo->dwOwnerID, pInfo->byStallIndex);


	if (pRole->IsInRoleState(ERS_Stall))
	{
		pInfo->eStallStatus = EVSS_Open;
		pInfo->dwLastGoodsTime = m_dwSecTimer;
		pRole->GetStallTitle(pInfo->szStallTitle);
	}
	else
	{
		pInfo->eStallStatus = EVSS_Close;
	}

	// 通知数据库
	tagNDBC_ApplyVIPStall send;
	send.VipStallInfo.byStallIndex	= pInfo->byStallIndex;
	send.VipStallInfo.dwOwnerID		= pInfo->dwOwnerID;
	send.VipStallInfo.nRemainTime	= pInfo->nRemainTime;
	g_dbSession.Send(&send, send.dwSize);

	return E_Success;
}

//-------------------------------------------------------------------------------------------------------
// 客户端获取所有VIP摊位概要信息
//-------------------------------------------------------------------------------------------------------
DWORD VIPStall::GetAllVIPStallInfo( OUT tagVIPStall* pStallInfo, OUT DWORD& dwTime )
{
	if (!P_VALID(pStallInfo))
		return GT_INVALID;
	
	if (!m_bInit)
		return E_Stall_VIP_Not_Init;

	memcpy(pStallInfo, &m_vecVIPStall[0], sizeof(tagVIPStall)*VIP_STALL_MAX_NUM);
	dwTime = m_dwVIPStallChange;

	return E_Success;
}

//-------------------------------------------------------------------------------------------------------
// 客户端获取某时间点后更新的所有VIP摊位概要信息
//-------------------------------------------------------------------------------------------------------
DWORD VIPStall::GetUpdatedStallInfo( OUT tagVIPStall* pStallInfo, INT32& nNum, DWORD& dwTime )
{
	if (!P_VALID(pStallInfo))
		return GT_INVALID;

	if (!m_bInit)
		return E_Stall_VIP_Not_Init;

	nNum = 0;

	if (dwTime >= m_dwVIPStallChange)
		return E_Success;

	for (int n=0; n<VIP_STALL_MAX_NUM; n++)
	{
		if (dwTime < m_vecVIPStall[n].dwLastUpdateTime)
		{
			memcpy(&(pStallInfo[nNum++]), &(m_vecVIPStall[n]), sizeof(tagVIPStall));
		}
	}
	dwTime = m_dwVIPStallChange;

	return E_Success;
}

//-------------------------------------------------------------------------------------------------------
// 客户端获取某VIP摊位商品信息
//-------------------------------------------------------------------------------------------------------
DWORD VIPStall::GetVIPStallGoods( OUT LPVOID pData, OUT BYTE &byGoodsNum, OUT INT &nGoodsSz, DWORD& dwRequestTime, BYTE byIndex )
{
	byGoodsNum = 0;
	nGoodsSz = 0;

	// 取得摊位概要信息
	tagVIPStall* pInfo = GetVIPStallInfo(byIndex);
	if (!P_VALID(pInfo))
	{
		return E_Stall_VIP_Info_NotExist;
	}

	// 判断摊位是否在营业中
	switch (pInfo->eStallStatus)
	{
	case EVSS_ForHire:
		return E_Stall_VIP_For_Hire;

	case EVSS_Close:
		return E_Stall_Role_Pull;
	}
	
	// 判断该摊位物品是否更新过
	if (dwRequestTime == pInfo->dwLastGoodsTime)
	{
		return E_Stall_VIP_GetGoods_Needless;
	}

	Role* pStallRole = g_roleMgr.GetRolePtrByID(pInfo->dwOwnerID);
	if (!P_VALID(pStallRole))
	{
		// 上层已经判断过了，这里为了保险
		return GT_INVALID;
	}

	DWORD dwErrorCode = pStallRole->GetStallGoods(pData, byGoodsNum, nGoodsSz);

	if (dwErrorCode == E_Success)
	{
		dwRequestTime = pInfo->dwLastGoodsTime;
	}
	
	return dwErrorCode;
}

//-------------------------------------------------------------------------------------------------------
// 删除角色时的清理动作
//-------------------------------------------------------------------------------------------------------
VOID VIPStall::RemoveRoleVIPStall( DWORD dwRoleID )
{
	tagVIPStall* pInfo = GetVIPStallInfo(dwRoleID);
	if (!P_VALID(pInfo))
	{
		return;
	}

	// 重置摊位信息
	m_mapVIPStall.Erase(pInfo->dwOwnerID);

	pInfo->Reset();
	pInfo->dwLastUpdateTime = m_dwSecTimer;
	m_dwVIPStallChange		= m_dwSecTimer;

	// 通知数据库
	tagNDBC_ApplyVIPStall send;
	send.VipStallInfo.byStallIndex	= pInfo->byStallIndex;
	send.VipStallInfo.dwOwnerID		= GT_INVALID;
	send.VipStallInfo.nRemainTime	= GT_INVALID;
	g_dbSession.Send(&send, send.dwSize);
}

//-------------------------------------------------------------------------------------------------------
// 全局变量
//-------------------------------------------------------------------------------------------------------
VIPStall g_VIPStall;
