//-----------------------------------------------------------------------------
//!\file title_mgr.cpp
//!\author xlguo
//!
//!\date 2009-02-02
//! last 2009-02-02
//!
//!\brief 称号消息处理过程
//!
//!	Copyright (c) 2004 TENGWU Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "title_mgr.h"
#include "..\ServerDefine\msg_title.h"
#include "..\WorldDefine\msg_role_title.h"
#include "role.h"

bool Condition::IsCountCond() const 
{	
	return g_attRes.GetTitleProto(m_u16TitleID)->m_CondType == ECT_COUNT; 
}

// 1初始化选项
void TitleMgr::InitOpts(Role* pRole, UINT16 u16ActTitleID, BOOL bVisibility)
{
	ASSERT( P_VALID(pRole) );
	m_pRole = pRole;
	m_u16ActiveID = u16ActTitleID;
	m_bVisibility = bVisibility;
}

// 2初始化称号数据
void TitleMgr::InitTitles(const BYTE* &pData, const INT32 nNum)
{
	m_bitsetObtainedMark.reset();
	m_bitsetDBInserted.reset();
	m_bitsetNeedSaveDB.reset();

	// 初始化条件数组清空
	for( INT nTitleIndex = 0; nTitleIndex < MAX_TITLE_NUM; ++nTitleIndex )
	{
		m_ArrPtrCondition[nTitleIndex] = NULL;
	}

	// 初始化读取的称号数据
	const tagTitleSave* pTitleLoad = reinterpret_cast<const tagTitleSave*>( pData );
	for( INT nLoadIndex = 0; nLoadIndex < nNum; ++nLoadIndex )
	{
		UINT16 u16TitleID	= pTitleLoad[nLoadIndex].u16TitleID;
		DWORD dwStateMark	= pTitleLoad[nLoadIndex].dwStateMark;

		m_bitsetDBInserted.set(u16TitleID);
		if( 0 == dwStateMark )
		{	
			// 已获得称号
			m_bitsetObtainedMark.set(u16TitleID);
		}
		else if( GT_VALID(dwStateMark) )
		{
			// 参数改变称号
			const tagTitleProto* pTitleProto = g_attRes.GetTitleProto( u16TitleID );
			m_ArrPtrCondition[u16TitleID] = pTitleProto->MakeCond( pTitleProto->m_dwPara1, dwStateMark );
			m_bitsetDBInserted.set(u16TitleID);
		}
		else
		{
			ASSERT(0);
		}
	}

	// 初始化条件数组建立未改变的条件
	for( INT nTitleIndex = 0; nTitleIndex < MAX_TITLE_NUM; ++nTitleIndex )
	{
		if( !m_bitsetDBInserted.test(nTitleIndex) )
		{
			if( !P_VALID( m_ArrPtrCondition[nTitleIndex] ) )
			{
				// 没改变
				const tagTitleProto* pTitleProto = g_attRes.GetTitleProto( nTitleIndex );
				m_ArrPtrCondition[nTitleIndex] = pTitleProto->MakeCond( pTitleProto->m_dwPara1, pTitleProto->m_dwPara2 );
			}
			else
			{
				// 已改变
				ASSERT(0);
			}
		}
	}


	// 根据条件初始化事件影射
	for (INT nTitleIndex = 0; nTitleIndex < MAX_TITLE_NUM; nTitleIndex++)
	{
		if( !P_VALID(m_ArrPtrCondition[nTitleIndex]) ) continue;
		
		const tagTitleProto* pProto = g_attRes.GetTitleProto(nTitleIndex);
		for( INT nEventIndex = 0; 
			GT_VALID( pProto->m_Events[nEventIndex] ) && nEventIndex < tagTitleProto::EVENTSNUM; 
			++nEventIndex )
			m_EventMap[pProto->m_Events[nEventIndex]].push_back(pProto->m_u16ID);
	}

	// 重新计算指针位置
	pData = reinterpret_cast<const BYTE *>( pTitleLoad + nNum );

}
// 销毁数据
void TitleMgr::Destroy()
{
	for (INT i = 0; i < MAX_TITLE_NUM; ++i)
	{
		SAFE_DEL(m_ArrPtrCondition[i]);
	}
}

// 发信号
bool TitleMgr::SigEvent(EnumTitleEvent titleEvent, DWORD dwArg1, DWORD dwArg2)
{
	ASSERT( ETE_VALID(titleEvent) );
	if (!ETE_VALID(titleEvent))
		return FALSE;

	BOOL bHasNew = FALSE;
	VecUINT16	vecNewObtTitleIDs;
	vecNewObtTitleIDs.clear();
	ListUINT16& listBindingTitles = m_EventMap[titleEvent];
	ListUINT16::iterator titleItr = listBindingTitles.begin();
	while( titleItr != listBindingTitles.end() )
	{
		UINT16 u16TitleID = *titleItr;
		++titleItr;

		if (!TITLEID_VALID(u16TitleID))
		{
			ASSERT(0);
			return FALSE;
		}
		//如果这个称号已经获取
		if(m_bitsetObtainedMark.test(u16TitleID))
			continue;

		Condition* pCond = m_ArrPtrCondition[u16TitleID];
		if (!P_VALID(pCond))
		{
			ASSERT(0);
			return FALSE;
		}

		switch(pCond->Check(dwArg1, dwArg2))
		{
		case ECR_ACTIVE:
			{
				bHasNew = TRUE;

				// 新获得称号
				m_bitsetObtainedMark.set( u16TitleID );
				vecNewObtTitleIDs.push_back( u16TitleID );

				// 若需要则插入数据库
				InsertTitle2DB( u16TitleID, TRUE );
				m_bitsetDBInserted.set( u16TitleID );

				// 清除条件映射
				//CleanRelation( u16TitleID );
			}
			break;
		case ECR_COUNTDOWN:
			{
				// 条件改变称号
				// 若需要则插入数据库
				if( !m_bitsetDBInserted.test(u16TitleID) )	
				{
					InsertTitle2DB( u16TitleID, FALSE );
					m_bitsetDBInserted.set( u16TitleID );
				}
				else
				{
					m_bitsetNeedSaveDB.set(u16TitleID);
				}
			}
			break;
		case ECR_NOAFECT:
			break;
		default:
			ASSERT(0);
			break;
		}
	}
	if( bHasNew ) NotifyClient( vecNewObtTitleIDs );
	return bHasNew == TRUE;
}

// 存储到数据库
void TitleMgr::SaveTitlesToDB(IN LPVOID pData, OUT LPVOID &pOutPointer, OUT INT32 &nNum)
{
	tagTitleSave* pCondData = static_cast<tagTitleSave *>( pData );
	INT nTitle2SaveNum = 0;
	for (INT nTitleIndex = 0; nTitleIndex < MAX_TITLE_NUM; ++nTitleIndex)
	{
		if (!m_bitsetNeedSaveDB.test(nTitleIndex))
			continue;

		if ( !m_bitsetObtainedMark.test(nTitleIndex) && m_bitsetDBInserted.test(nTitleIndex) )
		{
			// count condition
			ASSERT( P_VALID(m_ArrPtrCondition[nTitleIndex]) );
			if (!P_VALID(m_ArrPtrCondition[nTitleIndex]))
			{
				continue;
			}
			DWORD dwPara2 = m_ArrPtrCondition[nTitleIndex]->GetPara2();
			ASSERT( dwPara2 && GT_VALID(dwPara2));
			if (!dwPara2 || !GT_VALID(dwPara2))
			{
				continue;
			}
			
			pCondData[nTitle2SaveNum].u16TitleID = nTitleIndex;
			pCondData[nTitle2SaveNum].dwStateMark = dwPara2;
			++nTitle2SaveNum;
		}
		else if( m_bitsetObtainedMark.test(nTitleIndex) && m_bitsetDBInserted.test(nTitleIndex) && P_VALID( m_ArrPtrCondition[nTitleIndex] ) )
		{
			// new title this logon
			pCondData[nTitle2SaveNum].u16TitleID = nTitleIndex;
			pCondData[nTitle2SaveNum].dwStateMark = 0;
			++nTitle2SaveNum;
			SAFE_DEL(m_ArrPtrCondition[nTitleIndex]);
		}

		m_bitsetNeedSaveDB.reset(nTitleIndex);
	}
	pOutPointer = static_cast<BYTE *>(pData) + sizeof(tagTitleSave) * nTitle2SaveNum;
	nNum += nTitle2SaveNum;
}

// 通知客户端 增加了称号
void TitleMgr::NotifyClient(VecUINT16& vecNewTitleIDs)
{	
	INT nNewTitleNum = vecNewTitleIDs.size();

	ASSERT( nNewTitleNum > 0);
	if (nNewTitleNum <= 0)
	{
		return;
	}

	DWORD dwSize = sizeof(tagNS_NewTitles) - 1 + sizeof(UINT16) * nNewTitleNum;
	MCREATE_MSG(pSend, dwSize, NS_NewTitles);

	pSend->dwRoleID = m_pRole->GetID();
	pSend->u16TitleNum = nNewTitleNum;

	UINT16* pu16TitleID = reinterpret_cast<UINT16 *>( pSend->byData );
	INT	nTitleSendNum = 0;
	VecUINT16::iterator itrNewTitle = vecNewTitleIDs.begin();
	while( itrNewTitle != vecNewTitleIDs.end() )
	{
		pu16TitleID[nTitleSendNum++] = *itrNewTitle;
		++itrNewTitle;
	}

	ASSERT( nNewTitleNum == nTitleSendNum );
	if (nNewTitleNum != nTitleSendNum)
	{
		MDEL_MSG(pSend);
		return;
	}

	m_pRole->SendMessage(pSend, pSend->dwSize);
	MDEL_MSG(pSend);
	return ;
}
// 通知客户端 删除了称号
void TitleMgr::NotifyClientDelete(VecUINT16& vecDelTitleIDs)
{	
	INT nDelTitleNum = vecDelTitleIDs.size();

	ASSERT( nDelTitleNum > 0);
	if (nDelTitleNum <= 0)
	{
		return;
	}

	DWORD dwSize = sizeof(tagNS_DelTitles) - 1 + sizeof(UINT16) * nDelTitleNum;
	MCREATE_MSG(pSend, dwSize, NS_DelTitles);

	pSend->dwRoleID = m_pRole->GetID();
	pSend->u16TitleNum = nDelTitleNum;

	UINT16* pu16TitleID = reinterpret_cast<UINT16 *>( pSend->byData );
	INT	nTitleSendNum = 0;
	VecUINT16::iterator itrDelTitle = vecDelTitleIDs.begin();
	while( itrDelTitle != vecDelTitleIDs.end() )
	{
		pu16TitleID[nTitleSendNum++] = *itrDelTitle;
		++itrDelTitle;
	}

	ASSERT( nDelTitleNum == nTitleSendNum );
	if (nDelTitleNum != nTitleSendNum)
	{
		MDEL_MSG(pSend);
		return;
	}

	m_pRole->SendMessage(pSend, pSend->dwSize);
	MDEL_MSG(pSend);
	return ;
}

// 激活称号
DWORD TitleMgr::ActiviteTitle(const UINT16 u16TitleID)
{
	if( u16TitleID == m_u16ActiveID )
		return E_Title_Success;


	if( TITLEID_VALID(m_u16ActiveID) )
	{// 重设
		DWORD dwBuffID = g_attRes.GetTitleProto(m_u16ActiveID)->m_dwBuffID;
		m_pRole->RemoveBuff(Buff::GetIDFromTypeID(dwBuffID), TRUE);
		m_u16ActiveID = u16TitleID;
		if( TITLEID_VALID(m_u16ActiveID) )
		{
			DWORD dwBuffID = g_attRes.GetTitleProto(m_u16ActiveID)->m_dwBuffID;
			m_pRole->TryAddBuff(m_pRole, g_attRes.GetBuffProto(dwBuffID), NULL, NULL, NULL);
		}
	}
	else if( TITLEID_VALID(u16TitleID) )
	{// 激活
		ASSERT( !TITLEID_VALID(m_u16ActiveID) );
		m_u16ActiveID = u16TitleID;
		if (!TITLEID_VALID(m_u16ActiveID))
		{
			return E_Title_TitleActivateFailed;
		}
		DWORD dwBuffID = g_attRes.GetTitleProto(m_u16ActiveID)->m_dwBuffID;
		m_pRole->TryAddBuff(m_pRole, g_attRes.GetBuffProto(dwBuffID), NULL, NULL, NULL);
	}

	// 通知客户端
	tagNS_RoleTitleChangeBroadcast send;
	send.dwRoleID	= m_pRole->GetID();
	send.dwTitleID	= m_u16ActiveID;
	m_pRole->GetMap()->SendBigVisTileMsg(m_pRole, &send, send.dwSize);

	return E_Title_Success;

}

// 取得所有已获得称号数据
DWORD TitleMgr::GetObtainedTitleIDs(UINT16* &pData, UINT16 &u16Num)
{
	u16Num = 0;
	for (UINT i = 0; i < MAX_TITLE_NUM; ++i)
	{
		if ( m_bitsetObtainedMark.test(i) )
		{
			pData[u16Num++] = i;
		}
	}

	ASSERT( m_bitsetObtainedMark.count() == u16Num );
	if (m_bitsetObtainedMark.count() != u16Num)
	{
		return E_Title_UnknownError;
	}
	pData += u16Num;
	return E_Title_Success;			
}
//删除称号
BOOL TitleMgr::SigRemoveEvent( EnumTitleEvent titleEvent,  DWORD dwArg1)
{
	ASSERT( ETE_VALID(titleEvent) );
	if (!ETE_VALID(titleEvent))
		return FALSE;
	//被删除的称号id
	VecUINT16	vecDeleteTitleIDs;
	vecDeleteTitleIDs.clear();

	ListUINT16& listBindingTitles = m_EventMap[titleEvent];
	ListUINT16::iterator titleItr = listBindingTitles.begin();

	while( titleItr != listBindingTitles.end() )
	{
		UINT16 u16TitleID = *titleItr;
		++titleItr;

		//查看该称号是否存在
		if(!m_bitsetObtainedMark.test(u16TitleID))
			continue;

		//如果子类型有效
		if(dwArg1 != GT_INVALID)
		{
			Condition* pCond = m_ArrPtrCondition[u16TitleID];
			if (!P_VALID(pCond))
			{
				ASSERT(0);
				return FALSE;
			}
			if(dwArg1 != pCond->GetPara1())
				continue;
		}

		//如果该称号就是当前激活称号
		if(u16TitleID == m_u16ActiveID)
		{
			//去除称号作用的buff
			DWORD dwBuffID = g_attRes.GetTitleProto(m_u16ActiveID)->m_dwBuffID;
			m_pRole->RemoveBuff(Buff::GetIDFromTypeID(dwBuffID), TRUE);
			m_u16ActiveID = 65535;

			//不同步CurTitleID
			tagNS_RoleTitleChangeBroadcast send;
			send.dwRoleID	= m_pRole->GetID();
			send.dwTitleID	= GT_INVALID;
			m_pRole->GetMap()->SendBigVisTileMsg(m_pRole, &send, send.dwSize);
		}

		//往db发消息删除该称号
		if(m_bitsetDBInserted.test(u16TitleID))
			RemoveTitleFromDB(u16TitleID);

		//将该称号内存中的标志 设置一下
		m_bitsetObtainedMark.reset(u16TitleID);
		m_bitsetDBInserted.reset(u16TitleID);
		m_bitsetNeedSaveDB.set(u16TitleID);

		vecDeleteTitleIDs.push_back(u16TitleID);
	}
	//通知客户端
	NotifyClientDelete(vecDeleteTitleIDs);
}
// 称号从数据库删除
VOID TitleMgr::RemoveTitleFromDB( UINT16 u16TitleID)
{
	tagNDBC_TitleDelete send;
	send.dwRoleID = m_pRole->GetID();
	send.titleSave.u16TitleID = u16TitleID;

	g_dbSession.Send( &send, send.dwSize );
}
// 称号插入到数据库
VOID TitleMgr::InsertTitle2DB( UINT16 u16TitleID, BOOL bNew )
{
	tagNDBC_TitleInsert send;
	send.dwRoleID = m_pRole->GetID();
	send.titleSave.u16TitleID = u16TitleID;

	if( bNew )
	{
		// 新称号
		send.titleSave.dwStateMark = 0;				
	}
	else
	{
		ASSERT( !m_bitsetDBInserted.test( u16TitleID ) );
		if (m_bitsetDBInserted.test(u16TitleID))
		{
			return;
		}

		// 条件改变
		Condition* pCond = m_ArrPtrCondition[u16TitleID];
		ASSERT( P_VALID(pCond) );
		if (!P_VALID(pCond))
		{
			return;
		}

		DWORD dwPara2 = pCond->GetPara2();
		ASSERT( dwPara2 != 0 && GT_VALID(dwPara2));
		if (dwPara2 == 0 || !GT_VALID(dwPara2))
		{
			return;
		}
		send.titleSave.dwStateMark = dwPara2;
	}		
	g_dbSession.Send( &send, send.dwSize );
}
// 清除关系
void TitleMgr::CleanRelation(const UINT16 u16TitleID)
{
	ASSERT( m_bitsetObtainedMark.test(u16TitleID) );
	if (!m_bitsetObtainedMark.test(u16TitleID))
	{
		return;
	}

	// 清除事件映射
	for (UINT i = 0; i < ETE_MAX_EVENTNUM; ++i)
	{
		ListUINT16& curEventList = m_EventMap[i];
		ListUINT16::iterator itr = find(curEventList.begin(), curEventList.end(), u16TitleID);
		if ( itr != curEventList.end() ) curEventList.erase( itr );
	}

	// 删除条件
//	SAFE_DEL( m_ArrPtrCondition[u16TitleID] );
}