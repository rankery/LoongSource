//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: TreasureChest_mgr.h
// author: twan
// actor:
// data: 2009-07-29
// last:
// brief: 宝箱管理类
//-------------------------------------------------------------------------------------------------------
#pragma once

#include "../WorldDefine/TreasureChest_define.h"
#include "role.h"

class TreasureChestMgr
{
private:
	vector< vector<tagChestItem> >	m_vecItems;						// 存放所有宝箱和物品，一个宝箱有16个物品
	
public:
	BOOL						Init();								// 初始化宝箱内物品
	vector<tagChestItem>&		GetChest(INT nIndex);				// 获得一种宝箱的引用
	BOOL						CanOpenChest(DWORD dwChestID, DWORD dwKeyID);	//判断能否开启宝箱
	VOID						Destroy();							// 销毁
	tagChestItem*				GetRandomItem(DWORD dwChestID, ERateType eRate, FLOAT fRand); //开出宝箱中物品
	ERateType					GetRate(Role* pRole);				// 得到当前宝箱掉率
	// 向客户端发送消息
	BOOL						SendMsg2Client(Role* pRole, DWORD dwChestID, const std::string strMsgName, 
											BOOL bOpened, BOOL bDestroy, DWORD dwItemID, INT nItemNum, DWORD dwErrorCode);		
	
};

extern TreasureChestMgr g_TreasureChestMgr;