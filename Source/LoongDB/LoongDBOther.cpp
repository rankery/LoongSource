//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2004 TENGWU Entertainment All rights reserved.
// filename: LoongDBSave.cpp
// author: Sxg
// actor:
// data: 2008-05-12
// last:
// brief: 数据库操作应用层实现 -- 插入，删除，备份等操作
//-------------------------------------------------------------------------------------------------------
#include "StdAfx.h"
#include "../WorldDefine/QuestDef.h"
#include "../WorldDefine/SocialDef.h"
#include "../WorldDefine/ItemDefine.h"
#include "../WorldDefine/RoleDefine.h"
#include "../ServerDefine/role_data.h"
#include "../ServerDefine/msg_quest.h"
#include "../ServerDefine/msg_mall.h"
#include "../ServerDefine/msg_vipstall.h"
#include "../WorldDefine/mall_define.h"
#include "LoongDB.h"

//-------------------------------------------------------------------------------------------------------
// 好友的追加
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::InsertFriend(DWORD dwRoleID, LPVOID pFriendSave)
{
	return Update(dwRoleID, pFriendSave, 1, GT_INVALID, (&CLoongDB::FormatInsertFriend));
}

//-------------------------------------------------------------------------------------------------------
// 好友的更新
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::UpdateFriend(DWORD dwRoleID, LPVOID pFriendSave)
{
	return Update(dwRoleID, pFriendSave, 1, GT_INVALID, (&CLoongDB::FormatUpdateFriend));
}

//-------------------------------------------------------------------------------------------------------
// 好友的删除
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::DeleteFriend(DWORD dwRoleID, DWORD dwFriendID)
{
	return Update(dwRoleID, &dwFriendID, 1, GT_INVALID, (&CLoongDB::FormatDeleteFriend));
}

//-------------------------------------------------------------------------------------------------------
// 删除好友表中和该角色相关的所有记录
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::DeleteAllFriRel(DWORD dwRoleID)
{
	return Update(dwRoleID, NULL, 1, GT_INVALID, (&CLoongDB::FormatDeleteAllFriRel));
}

//-------------------------------------------------------------------------------------------------------
// 友好度的追加
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::InsertFriendship(DWORD dwRoleID, LPVOID pFriendshipSave)
{
	return Update(dwRoleID, pFriendshipSave, 1, GT_INVALID, (&CLoongDB::FormatInsertFriendship));
}

//-------------------------------------------------------------------------------------------------------
// 友好度的更新
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::UpdateFriendship(DWORD dwRoleID, LPVOID pFriendshipSave)
{
	return Update(dwRoleID, pFriendshipSave, 1, GT_INVALID, (&CLoongDB::FormatUpdateFriendship));
}

//-------------------------------------------------------------------------------------------------------
// 友好度的删除
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::DeleteFriendship(DWORD dwRoleID, DWORD dwFriendID)
{
	return Update(dwRoleID, &dwFriendID, 1, GT_INVALID, (&CLoongDB::FormatDeleteFriendship));
}

//-------------------------------------------------------------------------------------------------------
// 删除好友度表中和该角色相关的所有记录
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::DeleteAllFriValRel(DWORD dwRoleID)
{
	return Update(dwRoleID, NULL, 1, GT_INVALID, (&CLoongDB::FormatDeleteAllFriValRel));
}

//-------------------------------------------------------------------------------------------------------
// 黑名单的增加
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::InsertBlack(DWORD dwRoleID, DWORD dwBlackID)
{
	return Update(dwRoleID, &dwBlackID, 1, GT_INVALID, (&CLoongDB::FormatInsertBlack));
}

//-------------------------------------------------------------------------------------------------------
// 黑名单的删除
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::DeleteBlack(DWORD dwRoleID, DWORD dwBlackID)
{
	return Update(dwRoleID, &dwBlackID, 1, GT_INVALID, (&CLoongDB::FormatDeleteBlack));
}

//-------------------------------------------------------------------------------------------------------
// 仇敌的追加
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::InsertEnemy(DWORD dwRoleID, DWORD dwEnemyID)
{
	return Update(dwRoleID, &dwEnemyID, 1, GT_INVALID, (&CLoongDB::FormatInsertEnemy));
}

//-------------------------------------------------------------------------------------------------------
// 仇敌的删除
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::DeleteEnemy(DWORD dwRoleID, DWORD dwEnemyID)
{
	return Update(dwRoleID, &dwEnemyID, 1, GT_INVALID, (&CLoongDB::FormatDeleteEnemy));
}

//-------------------------------------------------------------------------------------------------------
// 称号的追加
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::InsertTitle(DWORD dwRoleID, LPVOID pData)
{
	return Update(dwRoleID, pData, 1, GT_INVALID, (&CLoongDB::FormatInsertTitle));
}
//-------------------------------------------------------------------------------------------------------
// 称号的删除
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::RemoveTitle(DWORD dwRoleID, LPVOID pData)
{
	return Update(dwRoleID, pData, 1, GT_INVALID, (&CLoongDB::FormatRemoveTitle));
}
//-------------------------------------------------------------------------------------------------------
// 删除任务
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::DeleteTask(DWORD dwRoleID, DWORD dwTaskID)
{
	return Update(dwRoleID, &dwTaskID, 1, GT_INVALID, (&CLoongDB::FormatDeleteTask));
}

//-------------------------------------------------------------------------------------------------------
// 插入一个元宝交易账户
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::InsertYBAccount(DWORD dwRoleID)
{
	return Update(dwRoleID, NULL, 1, GT_INVALID, (&CLoongDB::FormatInsertYBAccount));
} 

//-------------------------------------------------------------------------------------------------------
// 更新元宝交易账户的元宝数量
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::UpdateAccountYB(DWORD dwRoleID, INT32 nYuanBao)
{
	return Update(dwRoleID, &nYuanBao, 1, GT_INVALID, (&CLoongDB::FormatAccountYB));
}

//-------------------------------------------------------------------------------------------------------
// 更新元宝交易账户的金钱数量
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::UpdateAccountSilver(DWORD dwRoleID, LPVOID pMoney)
{
	return Update(dwRoleID, pMoney, 1, GT_INVALID, (&CLoongDB::FormatAccountSilver));
}

//-------------------------------------------------------------------------------------------------------
// 更新元宝交易账户出售订单的提交状态
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::UpdateSubmitSell(DWORD dwRoleID, bool bSellOrder)
{
	return Update(dwRoleID, &bSellOrder, 1, GT_INVALID, (&CLoongDB::FormatSubmitSell));
}

//-------------------------------------------------------------------------------------------------------
// 更新元宝交易账户收购订单的提交状态
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::UpdateSubmitBuy(DWORD dwRoleID, bool bBuyOrder)
{
	return Update(dwRoleID, &bBuyOrder, 1, GT_INVALID, (&CLoongDB::FormatSubmitBuy));
}

//-------------------------------------------------------------------------------------------------------
// 插入元宝交易订单
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::InsertOrder(DWORD dwRoleID, LPVOID pYBOrder)
{
	return Update(dwRoleID, pYBOrder, 1, GT_INVALID, (&CLoongDB::FormatInsertOrder));
}

//-------------------------------------------------------------------------------------------------------
// 撤销或关闭元宝交易订单
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::CompleteOrder(DWORD dwRoleID, LPVOID pComOrder)
{
	return Update(dwRoleID, pComOrder, 1, GT_INVALID, (&CLoongDB::FormatCompleteOrder));
}

//-------------------------------------------------------------------------------------------------------
// 插入元宝交易订单
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::RefreshOrder(DWORD dwRoleID, LPVOID pRefreshOrder)
{
	return Update(dwRoleID, pRefreshOrder, 1, GT_INVALID, (&CLoongDB::FormatRefreshOrder));
}

//-------------------------------------------------------------------------------------------------------
// 删除数据库中时间超过七天的所有订单
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::DeleteYBOrder(DWORD dwRoleID, LPVOID pDeleteTime)
{
	return Update(dwRoleID, pDeleteTime, 1, GT_INVALID, (&CLoongDB::FormatDeleteYBOrder));
}

//-------------------------------------------------------------------------------------------------------
// 追加一条团购信息
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::AddGPInfo(tagGPInfo* pGPInfo)
{
	return Update(GT_INVALID, pGPInfo, 1, GT_INVALID, (&CLoongDB::FormatAddGPInfo));
}

//-------------------------------------------------------------------------------------------------------
// 删除一条团购信息
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::RemoveGPInfo(tagGPInfoKey* pGPInfoKey)
{
	return Update(GT_INVALID, pGPInfoKey, 1, GT_INVALID, (&CLoongDB::FormatRemoveGPInfo));
}

//-------------------------------------------------------------------------------------------------------
// 删除指定帮派所有的团购信息
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::RemoveGuildGPInfo(DWORD dwGuildID)
{
	return Update(dwGuildID, NULL, 1, GT_INVALID, (&CLoongDB::FormatRemoveGuildGPInfo));
}

//-------------------------------------------------------------------------------------------------------
// 更新团购的响应者列表
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::UpdateGPInfo(tagGPInfoKey* pGPInfoKey, DWORD dwNewParticipator)
{
	// 获取流和连接
	Beton::MyStream* pStream = m_pDBLoong->GetStream();
	ASSERT(P_VALID(pStream));
	Beton::Connection* pCon = m_pDBLoong->GetFreeConnection();

	// 格式化数据
	pStream->SetSelect("group_purchase", "ParticipatorNum,ParticipatorList");
	pStream->SetWhere();
	pStream->FillString("GuildID=") << pGPInfoKey->dwGuildID;
	pStream->FillString(" and RoleID=") << pGPInfoKey->dwRoleID;
	pStream->FillString(" and MallID=") << pGPInfoKey->dwMallID;

	// 查询数据库
	Beton::QueryResult* pResult = m_pDBLoong->Query(pStream);

#ifdef _DEBUG
	IMSG(_T("%s\r\n"), m_pUtil->AnsiToUnicode(pStream->GetBuf()));
	IMSG(_T("Results Row Count: %d\n"), P_VALID(pResult) ? pResult->GetRowCount() : 0);
#endif

	// 释放流和连接
	m_pDBLoong->ReturnConnection(pCon);
	m_pDBLoong->ReturnStream(pStream);

	++m_dwWriteTimes;

	if(P_VALID(pResult) && pResult->GetRowCount() > 0)
	{
		// 构建新的响应者列表
		INT16 nParticipatorNum = (*pResult)[0].GetShort();

		BYTE *pBuffer = new BYTE[sizeof(tagGPInfo)+nParticipatorNum*sizeof(DWORD)];
		tagGPInfo *pGPInfo = (tagGPInfo*)pBuffer;
		(*pResult)[1].GetBlob(pGPInfo->dwParticipators, (*pResult)[1].GetLen());
		pGPInfo->dwParticipators[nParticipatorNum] = dwNewParticipator;
		pGPInfo->nParticipatorNum = nParticipatorNum + 1;
		pGPInfo->dwGuildID = pGPInfoKey->dwGuildID;
		pGPInfo->dwRoleID = pGPInfoKey->dwRoleID;
		pGPInfo->dwMallID = pGPInfoKey->dwMallID;

		// 更新到数据库
		Update(GT_INVALID, pGPInfo, 1, GT_INVALID, (&CLoongDB::FormatUpdateGPInfo));

		SAFE_DEL_ARRAY(pBuffer);
	}

	return TRUE;
}

//-------------------------------------------------------------------------------------------------------
// 更新团购的响应者列表
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::UpdateGPTime()
{
	// 更新到数据库
	Update(GT_INVALID, NULL, 1, GT_INVALID, (&CLoongDB::FormatUpdateGPTime));

	return TRUE;
}

//-------------------------------------------------------------------------------------------------------
// 申请VIP摊位
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::ApplyVIPStall(tagVipStall2DB* pInfo)
{
	// 更新到数据库
	Update(GT_INVALID, pInfo, 1, GT_INVALID, (&CLoongDB::FormatApplyVIPStall));

	return TRUE;
}

//-------------------------------------------------------------------------------------------------------
// 更新VIP摊位剩余时间
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::UpdateVIPStallTime()
{
	// 更新到数据库
	Update(GT_INVALID, NULL, 1, GT_INVALID, (&CLoongDB::FormatUpdateVIPStallTime));

	return TRUE;
}

//-------------------------------------------------------------------------------------------------------
// 更新角色开启宝箱计数
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::UpdateTreasureSum(DWORD dwRoleID, INT& nSum)
{
	// 更新到数据库
	Update(dwRoleID, &nSum, 1, GT_INVALID, (&CLoongDB::FormatUpdateTreasureSum));

	return TRUE;
}
//-------------------------------------------------------------------------------------------------------
// 结婚
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::RoleGetMarriage(DWORD dwRoleID1, DWORD dwRoleID2)
{
	return Update(dwRoleID1, &dwRoleID2, 1, GT_INVALID, (&CLoongDB::FormatRoleGetMarriage));
}
//-------------------------------------------------------------------------------------------------------
// 离婚
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::RoleBreakMarriage(DWORD dwRoleID1, DWORD dwRoleID2)
{
	return Update(dwRoleID1, &dwRoleID2, 1, GT_INVALID, (&CLoongDB::FormatRoleBreakMarriage));
}
//*********************************************************************************************************

//-------------------------------------------------------------------------------------------------------
// 结婚的sql
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatRoleGetMarriage(Beton::MyStream *pStream, DWORD dwRoleID1, LPVOID pData)
{
	MTRANS_POINTER(p, pData, DWORD);
	pStream->Clear();

	pStream->SetUpdate("roledata");
	pStream->FillString("LoverID=") << *p;
	pStream->FillString(",HaveWedding=0");
	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID1;
}
//-------------------------------------------------------------------------------------------------------
// 离婚的sql
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatRoleBreakMarriage(Beton::MyStream *pStream, DWORD dwRoleID1, LPVOID pData)
{
	MTRANS_POINTER(p, pData, DWORD);
	pStream->Clear();

	pStream->SetUpdate("roledata");
	pStream->FillString("LoverID = 4294967295");
	pStream->FillString(",HaveWedding=0");
	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID1;
}
//-------------------------------------------------------------------------------------------------------
// 好友的追加SQL语句格式化
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatInsertFriend(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pFriendSave)
{
	MTRANS_POINTER(p, pFriendSave, tagFriendSave);
	
	pStream->SetInsert("friend");
	pStream->FillString("RoleID=") << dwRoleID;
	pStream->FillString(",FriendID=") << p->dwFriendID;
	pStream->FillString(",GroupID=") << p->nGroupID;
}

//-------------------------------------------------------------------------------------------------------
// 好友的更新SQL语句格式化
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatUpdateFriend(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pFriendSave)
{
	MTRANS_POINTER(p, pFriendSave, tagFriendSave);
	
	pStream->SetUpdate("friend");
	pStream->FillString("GroupID=") << p->nGroupID;

	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
	pStream->FillString(" and FriendID=") << p->dwFriendID;
}

//-------------------------------------------------------------------------------------------------------
// 好友的删除SQL语句格式化
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatDeleteFriend(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pFriendID)
{
	pStream->SetDelete("friend");
	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
	pStream->FillString(" and FriendID=") << (*(DWORD *)pFriendID);
}

//-------------------------------------------------------------------------------------------------------
// 删除所以和该角色相关的记录SQL语句格式化
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatDeleteAllFriRel(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pDummy)
{
	pStream->SetDelete("friend");
	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
	pStream->FillString(" or FriendID=") << dwRoleID;
}

//-------------------------------------------------------------------------------------------------------
// 友好度的追加SQL语句格式化
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatInsertFriendship(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pFriendshipSave)
{
	MTRANS_POINTER(p, pFriendshipSave, tagFriendshipSave);

	ASSERT(dwRoleID < p->dwFriendID);

	pStream->SetInsert("friendship");
	pStream->FillString("RoleID=") << dwRoleID;
	pStream->FillString(",FriendID=") << p->dwFriendID;
	pStream->FillString(",FriVal=") << p->nFriVal;
}

//-------------------------------------------------------------------------------------------------------
// 友好度的更新SQL语句格式化
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatUpdateFriendship(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pFriendshipSave)
{
	MTRANS_POINTER(p, pFriendshipSave, tagFriendshipSave);

	ASSERT(dwRoleID < p->dwFriendID);

	pStream->SetUpdate("friendship");
	pStream->FillString("FriVal=") << p->nFriVal;

	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
	pStream->FillString(" and FriendID=") << p->dwFriendID;
}

//-------------------------------------------------------------------------------------------------------
// 友好度的删除SQL语句格式化
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatDeleteFriendship(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pFriendID)
{
	ASSERT(dwRoleID < (*(DWORD *)pFriendID));

	pStream->SetDelete("friendship");
	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
	pStream->FillString(" and FriendID=") << (*(DWORD *)pFriendID);
}

//-------------------------------------------------------------------------------------------------------
// 从好友度表中删除和该角色相关的所有记录SQL语句格式化
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatDeleteAllFriValRel(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pDummy)
{
	pStream->SetDelete("friendship");
	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
	pStream->FillString(" or FriendID=") << dwRoleID;
}

//-------------------------------------------------------------------------------------------------------
// 友好度的删除SQL语句格式化
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatInsertBlack(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pBlackID)
{
	pStream->SetInsert("blacklist");
	pStream->FillString("RoleID=") << dwRoleID;
	pStream->FillString(",BlackID=") << (*(DWORD *)pBlackID);
}

//-------------------------------------------------------------------------------------------------------
// 友好度的删除SQL语句格式化
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatDeleteBlack(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pBlackID)
{
	pStream->SetDelete("blacklist");
	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
	pStream->FillString(" and BlackID=") << (*(DWORD *)pBlackID);
}

//-------------------------------------------------------------------------------------------------------
// 仇敌的追加SQL语句格式化
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatInsertEnemy(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pEnemyID)
{
	pStream->SetInsert("enemy");
	pStream->FillString("RoleID=") << dwRoleID;
	pStream->FillString(",EnemyID=") << (*(DWORD *)pEnemyID);
}

//-------------------------------------------------------------------------------------------------------
// 仇敌的删除SQL语句格式化
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatDeleteEnemy(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pEnemyID)
{
	pStream->SetDelete("enemy");
	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
	pStream->FillString(" and EnemyID=") << (*(DWORD *)pEnemyID);
}

//-------------------------------------------------------------------------------------------------------
// 称号的插入SQL语句格式化
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatInsertTitle(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagTitleSave);
	
	pStream->SetReplace("title");
	pStream->FillString("RoleID=") << dwRoleID;
	pStream->FillString(",ID=") << p->u16TitleID;
	pStream->FillString(",Count=") << p->dwStateMark;
}
//-------------------------------------------------------------------------------------------------------
// 删除称号的sql语句
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatRemoveTitle(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagTitleSave);
	
	pStream->SetDelete("title");
	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
	pStream->FillString(" and ID=") << p->u16TitleID;
}
//-------------------------------------------------------------------------------------------------------
// SQL语句格式化
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatInsertTask(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pAcceptQuest)
{
	MTRANS_POINTER(p, pAcceptQuest, tagAcceptQuest);
	
	pStream->SetInsert("task");
	pStream->FillString("RoleID=") << dwRoleID;
	pStream->FillString(",TaskID=") << p->u16QuestID;
	pStream->FillString(",StartTime=") << p->dwAcceptTime;
}

//-------------------------------------------------------------------------------------------------------
// 仇敌的删除SQL语句格式化
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatDeleteTask(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pTaskID)
{
	pStream->SetDelete("task");
	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
	pStream->FillString(" and TaskID=") << (*(DWORD *)pTaskID);
}

//-------------------------------------------------------------------------------------------------------
// 插入元宝账户的SQL语句格式化
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatInsertYBAccount(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	pStream->SetInsert("ybaccount");
	pStream->FillString("RoleID=") << dwRoleID;
}

//-------------------------------------------------------------------------------------------------------
// 更新元宝交易账户的元宝数量
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatAccountYB(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	pStream->SetUpdate("ybaccount");
	pStream->FillString("YuanBao=") <<(*(INT32*)pData);

	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
}

//-------------------------------------------------------------------------------------------------------
// 更新元宝交易账户的金钱数量
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatAccountSilver(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pMoney)
{
	MTRANS_POINTER(p, pMoney, tagMoney);

	pStream->SetUpdate("ybaccount");
	pStream->FillString("Gold=") <<p->nGold;
	pStream->FillString(",Silver=") <<p->nSilver;

	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
}

//-------------------------------------------------------------------------------------------------------
// 更新元宝交易账户出售订单的提交状态
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatSubmitSell(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	pStream->SetUpdate("ybaccount");
	pStream->FillString("bSellOrder=") << (*(BYTE*)pData);

	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
}

//-------------------------------------------------------------------------------------------------------
// 更新元宝交易账户收购订单的提交状态
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatSubmitBuy(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	pStream->SetUpdate("ybaccount");
	pStream->FillString("bBuyOrder=") << (*(BYTE*)pData);

	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
}

//-------------------------------------------------------------------------------------------------------
// 插入元宝交易订单
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatInsertOrder(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagYBOrder);

	pStream->SetInsert("yuanbaoorder");
	pStream->FillString("dwID=") << p->dwID;
	pStream->FillString(",RoleID=") << p->dwRoleID;
	pStream->FillString(",OrderType=") << p->nType;
	pStream->FillString(",Price=") << p->nPrice;
	pStream->FillString(",Number=") << p->nNum;
	pStream->FillString(",OrderMode=") << p->nYBMode;
	pStream->FillString(",StartTime=") << p->dwStartTime;
}

//-------------------------------------------------------------------------------------------------------
// 撤销或关闭元宝交易订单
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatCompleteOrder(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagCompleteOrder);

	pStream->SetUpdate("yuanbaoorder");
	pStream->FillString("OrderMode=") << p->nYBOMode;
	pStream->FillString(",EndTime=") << p->dwEndTime;

	pStream->SetWhere();
	pStream->FillString("dwID=") << p->dwID;
}

//-------------------------------------------------------------------------------------------------------
// 更新元宝交易订单
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatRefreshOrder(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagRefresh);

	pStream->SetUpdate("yuanbaoorder");
	pStream->FillString("AvgPrice=") << p->nAvgPrice;
	pStream->FillString(",DealNum=") << p->nDealNum;

	pStream->SetWhere();
	pStream->FillString("dwID=") << p->dwID;
}

//-------------------------------------------------------------------------------------------------------
// 删除元宝交易订单
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatDeleteYBOrder(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	pStream->SetDelete("yuanbaoorder");
	pStream->SetWhere();
	pStream->FillString("StartTime<") << (*(DWORD*)pData);
}


//-------------------------------------------------------------------------------------------------------
// 格式化插入Log语句
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatInsertBaiBaoLog(Beton::MyStream *pStream, DWORD dwAccountID, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagBaiBaoRecord);

	// 获取连接
	Beton::Connection* pCon = m_pDBLoong->GetFreeConnection();

	pStream->SetInsert("log_baibao");
	pStream->FillString("AccountID=") << dwAccountID;
	pStream->FillString(",RoleID=") << p->dwRoleID;

	// LoongDB设置时间
	if(DwordTime2DataTime(m_szTime, sizeof(m_szTime), p->dwTime))
	{
		pStream->FillString(",LogTime='").FillString(m_szTime);
		pStream->FillString("'");
	}
	else
	{
		IMSG(_T("Error: Func DwordTime2DataTime() run error in CLoongDB::CreateRole()!!!!!\r\n"));
		ASSERT(0);
	}

	pStream->FillString(",n16Type=") << p->n16Type;
	pStream->FillString(",TypeID=") << p->dwTypeID;


	//赠言
	pStream->FillString(",LeaveWords='");
	INT nLen = _tcsnlen(p->szWords, Max_LeaveWord_Length);
	if (nLen > 0)
	{
		pStream->FillBlob(p->szWords, p->n16Size-sizeof(tagBaiBaoRecord)+sizeof(p->szWords), pCon);
	}
	pStream->FillString("'");

	// 释放连接
	m_pDBLoong->ReturnConnection(pCon);
}

//-------------------------------------------------------------------------------------------------------
// 格式化删除Log语句
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatDeleteBaiBaoLog(Beton::MyStream *pStream, DWORD dwAccountID, LPVOID pData)
{
	ASSERT(GT_VALID(dwAccountID));

	MTRANS_POINTER(p, pData, tagBaiBaoRecord);

	pStream->SetDelete("log_baibao");
	pStream->SetWhere();
	pStream->FillString("AccountID=") << dwAccountID;
	pStream->FillString(" and RoleID=") << p->dwRoleID;
	pStream->FillString(" and n16Type=") << p->n16Type;
	pStream->FillString(" and TypeID=") << p->dwTypeID;

	if(DwordTime2DataTime(m_szTime, sizeof(m_szTime), p->dwTime))
	{
		pStream->FillString(" and LogTime='").FillString(m_szTime);
		pStream->FillString("'");
	}
	else
	{
		IMSG(_T("Error: Func DwordTime2DataTime() run error in CLoongDB::CreateRole()!!!!!\r\n"));
		ASSERT(0);
	}
}

//-------------------------------------------------------------------------------------------------------
// 删除多余的百宝袋历史记录
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::ProcResCountBaiBaoLog(OUT LPVOID &pData, OUT INT32 *pNum, Beton::QueryResult *pRes)
{
	INT32 nNum = pRes->GetRowCount();

	if(nNum < MAX_BAIBAO_LOG_NUM) return;

	DWORD dwAccountID = GT_INVALID;
	tagBaiBaoRecord tmpLogInfo = {0};
	DWORD tmpLogTime;
	tmpLogTime = tmpLogInfo.dwTime = GetCurrentDWORDTime();

	for(INT32 i=0; i<nNum; ++i)
	{
		DataTime2DwordTime((*(tagDWORDTime*)&tmpLogTime), (*pRes)[2].GetString(), (*pRes)[2].GetLen());
		if (tmpLogTime <= tmpLogInfo.dwTime)
		{
			dwAccountID = (*pRes)[0].GetDword();
			tmpLogInfo.dwRoleID = (*pRes)[1].GetDword();
			tmpLogInfo.dwTypeID = (*pRes)[4].GetDword();
			tmpLogInfo.n16Type = (*pRes)[3].GetShort();
			DwordTime2DataTime(m_szTime, sizeof(m_szTime), tmpLogTime);
			tmpLogInfo.dwTime = tmpLogTime;
		}
		pRes->NextRow();
	}

	Update(dwAccountID, (LPVOID)&tmpLogInfo, 1, GT_INVALID, &CLoongDB::FormatDeleteBaiBaoLog);
}
//-------------------------------------------------------------------------------------------------------
// 追加一条团购信息
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatAddGPInfo(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagGPInfo);

	// 获取连接
	Beton::Connection* pCon = m_pDBLoong->GetFreeConnection();

	pStream->SetInsert("group_purchase");
	pStream->FillString("GuildID=") << p->dwGuildID;
	pStream->FillString(",RoleID=") << p->dwRoleID;
	pStream->FillString(",MallID=") << p->dwMallID;
	pStream->FillString(",ItemPrice=") << p->nPrice;
	pStream->FillString(",ParticipatorNum=") << p->nParticipatorNum;
	pStream->FillString(",RequireNum=") << p->nRequireNum;
	pStream->FillString(",RemainTime=") << p->nRemainTime;

	//响应者列表，此时只有发起人
	pStream->FillString(",ParticipatorList='");
	pStream->FillBlob(p->dwParticipators, sizeof(p->dwParticipators), pCon);
	pStream->FillString("'");

	// 释放连接
	m_pDBLoong->ReturnConnection(pCon);
}

//-------------------------------------------------------------------------------------------------------
// 删除一条团购信息
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatRemoveGPInfo(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagGPInfoKey);

	pStream->SetDelete("group_purchase");
	pStream->SetWhere();
	pStream->FillString("GuildID=") << p->dwGuildID;
	pStream->FillString(" and RoleID=") << p->dwRoleID;
	pStream->FillString(" and MallID=") << p->dwMallID;
}

//-------------------------------------------------------------------------------------------------------
// 删除指定帮派所有的团购信息
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatRemoveGuildGPInfo(Beton::MyStream *pStream, DWORD dwGuildID, LPVOID pData)
{
	pStream->SetDelete("group_purchase");
	pStream->SetWhere();
	pStream->FillString("GuildID=") << dwGuildID;
}

//-------------------------------------------------------------------------------------------------------
// 更新团购信息响应者列表
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatUpdateGPInfo(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagGPInfo);

	// 获取连接
	Beton::Connection* pCon = m_pDBLoong->GetFreeConnection();

	pStream->SetUpdate("group_purchase");

	pStream->FillString("ParticipatorNum=") << p->nParticipatorNum;

	//响应者列表
	pStream->FillString(",ParticipatorList='");
	pStream->FillBlob(p->dwParticipators, p->nParticipatorNum * sizeof(DWORD), pCon);
	pStream->FillString("'");

	pStream->SetWhere();
	pStream->FillString("GuildID=") << p->dwGuildID;
	pStream->FillString(" and RoleID=") << p->dwRoleID;
	pStream->FillString(" and MallID=") << p->dwMallID;

	// 释放连接
	m_pDBLoong->ReturnConnection(pCon);
}

//-------------------------------------------------------------------------------------------------------
// 更新团购剩余时间
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatUpdateGPTime(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	pStream->SetUpdate("group_purchase");

	pStream->FillString("RemainTime=RemainTime-60");
	pStream->SetWhere();
	pStream->FillString("RemainTime>0");
}

//-------------------------------------------------------------------------------------------------------
// 申请VIP摊位
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatApplyVIPStall(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagVipStall2DB);

	pStream->Clear();
	pStream->SetUpdate("vip_stall");
	pStream->FillString("RoleID=") << p->dwOwnerID;
	pStream->FillString(",RemainTime=") <<p->nRemainTime;
	pStream->SetWhere();
	pStream->FillString("StallID=") << p->byStallIndex;
}

//-------------------------------------------------------------------------------------------------------
// 更新VIP摊位剩余时间
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatUpdateVIPStallTime(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	pStream->Clear();

	pStream->SetUpdate("vip_stall");
	pStream->FillString("RemainTime=RemainTime-60");
	pStream->SetWhere();
	pStream->FillString("RemainTime>0");
}

//-------------------------------------------------------------------------------------------------------
// 插入一条摊位信息
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatInsertVIPStall(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	MTRANS_POINTER(p, pData, tagVipStall2DB);

	pStream->SetInsert("vip_stall");
	pStream->FillString("StallID=") << p->byStallIndex;
	pStream->FillString(",RoleID=") << p->dwOwnerID;
	pStream->FillString(",RemainTime=") << p->nRemainTime;
}

//-------------------------------------------------------------------------------------------------------
// 更新角色开启宝箱计数
//-------------------------------------------------------------------------------------------------------
VOID CLoongDB::FormatUpdateTreasureSum(Beton::MyStream *pStream, DWORD dwRoleID, LPVOID pData)
{
	MTRANS_POINTER(p, pData, DWORD);
	pStream->Clear();

	pStream->SetUpdate("roledata");
	pStream->FillString("TreasureSum=") << *p;
	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;
}

//-------------------------------------------------------------------------------------------------------
// 锁表 -- 锁多个参数为"t1,t2,t3"
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::LockTables(LPCSTR szTableNames)
{
	return Update(GT_INVALID, (LPVOID)szTableNames, 1, 0, (&CLoongDB::FormatLockTables));
}

VOID CLoongDB::FormatLockTables(Beton::MyStream *pStream, DWORD dwDummy, LPVOID pData)
{
	pStream->Clear();

	pStream->FillString("Lock Tables ");
	pStream->FillString((LPCSTR)pData);
	pStream->FillString(" WRITE");
}

//-------------------------------------------------------------------------------------------------------
// 解锁
//-------------------------------------------------------------------------------------------------------
BOOL CLoongDB::UnlockTables()
{
	return Update(GT_INVALID, NULL, 1, 0, (&CLoongDB::FormatUnlockTables));
}

VOID CLoongDB::FormatUnlockTables(Beton::MyStream *pStream, DWORD dwDummy, LPVOID pData)
{
	pStream->Clear();

	pStream->FillString("Unlock Tables");
}
