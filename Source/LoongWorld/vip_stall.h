//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: vip_stall.h
// author: sunnee
// actor:
// data: 2009-06-15
// last:
// brief: VIP摊位类
//-------------------------------------------------------------------------------------------------------
#pragma once
#include "event_mgr.h"
#include "../WorldDefine/stall_define.h"

struct tagVIPStall;
struct tagVipStall2DB;

class VIPStall : public EventMgr<VIPStall>
{
public:
	VIPStall();
	~VIPStall();

	BOOL	Init();
	VOID	Update();
	VOID	Destroy();

	// 事件处理函数
	VOID	OnSetStallTitleFunc(DWORD dwSenderID, VOID* pEventMessage);
	VOID	OnSetStallStatusFunc(DWORD dwSenderID, VOID* pEventMessage);
	VOID	OnChangeStallGoodsFunc(DWORD dwSenderID, VOID* pEventMessage);

	// 地图上层消息处理
	DWORD	ApplyVIPStall(Role* pRole, BYTE byIndex, INT32 nRent);
	DWORD	GetAllVIPStallInfo(OUT tagVIPStall* pStallInfo, DWORD& dwTime);
	DWORD	GetUpdatedStallInfo(OUT tagVIPStall* pStallInfo, INT32& nNum, DWORD& dwTime);
	DWORD	GetVIPStallGoods(OUT LPVOID pData, OUT BYTE &byGoodsNum, OUT INT &nGoodsSz, DWORD& dwRequestTime, BYTE byIndex);

public:
	// 摊位信息相关
	tagVIPStall*	GetVIPStallInfo(BYTE index);
	tagVIPStall*	GetVIPStallInfo(DWORD dwRoleID);
	Role*			GetVIPStallRole(BYTE index);
	BOOL			IsInStatus(BYTE index, EVIPStallStatus eStatus);
	DWORD			GetSpecVIPStallGoodsTime(BYTE index);

	// 设置摊位状态
	DWORD			SetVIPStallStatus(BYTE index, EVIPStallStatus eStatus);

	// 角色删除时的清理操作
	VOID			RemoveRoleVIPStall(DWORD dwRoleID);

	// 读取所有VIP摊位信息
	DWORD	LoadAllVIPStallInfo(tagVipStall2DB* pVIPStallInfo);

private:
	static	VOID RegisterStallEventFunc();

private:
	vector<tagVIPStall>		m_vecVIPStall;		// VIP摊位信息
	TMap<DWORD, BYTE>		m_mapVIPStall;		// 角色ID与VIP摊位序号对照表
	BOOL					m_bInit;

	DWORD					m_dwVIPStallChange;	// VIP摊位变更时刻
	tagDWORDTime			m_dwSecTimer;		// 更新计时
	INT						m_nMinuteTimer;		// 分钟计时
};

extern VIPStall g_VIPStall;