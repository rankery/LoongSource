#include "StdAfx.h"

#include "wx/wx.h"
#include "servermergetoolapp.h"
#include "Loong_OutputCtrl.h"
#include "Loong_LoongDBCtrl.h"
#include "../WorldDefine/time.h"
CLoongLoongDBCtrl::CLoongLoongDBCtrl(void)
{
}
//-------------------------------------------------------------------------------
// 初始化函数
//-------------------------------------------------------------------------------
BOOL CLoongLoongDBCtrl::Init(tagConfigDetail stConfig[EM_DB_TYPE_END])
{
	//初始化LoongDB
	for(int iLoop=EM_DB_SRC_ONE;iLoop<EM_DB_TYPE_END;iLoop++)
	{
		BOOL bRet = m_LoongDB[iLoop].Init(stConfig[iLoop].strIp.c_str(),
			stConfig[iLoop].strUser.c_str(),
			stConfig[iLoop].strPsd.c_str(),
			stConfig[iLoop].strDBName.c_str(),
			stConfig[iLoop].dwPort);
		if(bRet == FALSE)
		{
			tstring strErr;
			strErr += stConfig[iLoop].strIp;
			strErr += _T("		");
			strErr += stConfig[iLoop].strUser;
			strErr += _T("		");
			strErr += stConfig[iLoop].strPsd;
			strErr += _T("		");
			strErr += stConfig[iLoop].strDBName;
			strErr += _T("		");
			strErr += stConfig[iLoop].dwPort;
			strErr += _T("初始化失败！");

			ILOG->Write(strErr.c_str());
			return FALSE;
		}
	}

	// 设置报警回调函数
	for(int iLoop=EM_DB_SRC_ONE;iLoop<EM_DB_TYPE_END;iLoop++)
	{
		m_LoongDB[iLoop].SetWarningCallBack((WARNINGCALLBACK)LoongDBCallBack);
	}
	return TRUE;
}
//-------------------------------------------------------------------------------
// 销毁函数
//-------------------------------------------------------------------------------
VOID CLoongLoongDBCtrl::Destroy()
{
	for(int iLoop=EM_DB_SRC_ONE;iLoop<EM_DB_TYPE_END;iLoop++)
	{
		m_LoongDB[iLoop].ShutDown();
	}
}
BOOL CLoongLoongDBCtrl::Load(OUT LPVOID pData,EDBType eDBType, 
							 Fun_pFormat pFunFormat, Fun_pProcRes pFunProcRes)
{
	BOOL bRet = TRUE;

	// 获取流
	Beton::MyStream* pStream = m_LoongDB[eDBType].GetStream();
	ASSERT(P_VALID(pStream));

	// 格式化查询的角色数据
	(this->*pFunFormat)(pStream,eDBType);

	// 查询数据库
	Beton::QueryResult* pResult = m_LoongDB[eDBType].Query(pStream);
	if(!P_VALID(pResult))
	{
		bRet = FALSE;
		goto Exit;
	}

	// 检验并保存查询结果
	if (pResult->GetRowCount() > 0)
	{
		(this->*pFunProcRes)(pData,pResult);
	}
Exit:
	// 释放流和结果
	m_LoongDB[eDBType].ReturnStream(pStream);
	m_LoongDB[eDBType].FreeQueryResult(pResult);

	return bRet;
}
//-------------------------------------------------------------------------------------------------------
// 按用户id load数据
//-------------------------------------------------------------------------------------------------------
BOOL CLoongLoongDBCtrl::AccountLoad(OUT LPVOID pData,DWORD dwAccountID,EDBType eDBType,
										Fun_pAccountFormat pFunFormat, Fun_pProcRes pFunProcRes)
{
	BOOL bRet = TRUE;

	// 获取流
	Beton::MyStream* pStream = m_LoongDB[eDBType].GetStream();
	ASSERT(P_VALID(pStream));

	// 格式化查询的角色数据
	(this->*pFunFormat)(pStream,dwAccountID,eDBType);

	// 查询数据库
	Beton::QueryResult* pResult = m_LoongDB[eDBType].Query(pStream);
	if(!P_VALID(pResult))
	{
		bRet = FALSE;
		goto Exit;
	}

	// 检验并保存查询结果
	if (pResult->GetRowCount() > 0)
	{
		(this->*pFunProcRes)(pData,pResult);
	}
Exit:
	// 释放流和结果
	m_LoongDB[eDBType].ReturnStream(pStream);
	m_LoongDB[eDBType].FreeQueryResult(pResult);

	return bRet;
}

//-------------------------------------------------------------------------------------------------------
// 按用户n64Limit 分页load数据
//-------------------------------------------------------------------------------------------------------
EPagingState CLoongLoongDBCtrl::PagingLoad(OUT LPVOID pData,INT64 &n64Limit,EDBType eDBType,
									Fun_pPagingFormat pFunFormat, Fun_pProcRes pFunProcRes)
{
	EPagingState eReturnVal = EPS_DB_NULL;

	// 获取流
	Beton::MyStream* pStream = m_LoongDB[eDBType].GetStream();
	ASSERT(P_VALID(pStream));

	// 格式化查询的角色数据
	(this->*pFunFormat)(pStream,n64Limit,eDBType);

	// 查询数据库
	Beton::QueryResult* pResult = m_LoongDB[eDBType].Query(pStream);
	if(!P_VALID(pResult))
	{
		eReturnVal = EPS_DB_ERROR;
		goto Exit;
	}

	// 检验并保存查询结果
	if (pResult->GetRowCount() > 0)
	{
		(this->*pFunProcRes)(pData,pResult);
	}
	else
	{		
		eReturnVal = EPS_DB_RESULT_NULL;
		goto Exit;
	}
Exit:
	// 释放流和结果
	m_LoongDB[eDBType].ReturnStream(pStream);
	m_LoongDB[eDBType].FreeQueryResult(pResult);

	return eReturnVal;
}


//-------------------------------------------------------------------------------------------------------
// 插入数据库操作模版方法(无结果集,且不需要数据库联接)
//-------------------------------------------------------------------------------------------------------
BOOL CLoongLoongDBCtrl::Insert(LPVOID pData, EDBType eDBType,Fun_pFormat3 pFunFormat)
{
	ASSERT(pData);
	if(pData == NULL)return FALSE;
	if(eDBType>EM_DB_TYPE_END)return FALSE;

	BOOL bRet = TRUE;

	// 获取流
	Beton::MyStream* pStream = m_LoongDB[eDBType].GetStream();
	ASSERT(P_VALID(pStream));
	
	// 格式化数据库操作语句,并执行
	bRet = (this->*pFunFormat)(pStream, pData,eDBType);

	// 释放流
	m_LoongDB[eDBType].ReturnStream(pStream);

	return bRet;
}
BOOL CLoongLoongDBCtrl::Insert(LPVOID pData, EDBType eDBType,LPCSTR pszTable,Fun_pFormat4 pFunFormat)
{
	ASSERT(pData);
	if(pData == NULL)return FALSE;
	if(eDBType>EM_DB_TYPE_END)return FALSE;

	BOOL bRet = TRUE;

	// 获取流
	Beton::MyStream* pStream = m_LoongDB[eDBType].GetStream();
	ASSERT(P_VALID(pStream));

	// 格式化数据库操作语句,并执行
	bRet = (this->*pFunFormat)(pStream, pData,pszTable,eDBType);

	// 释放流
	m_LoongDB[eDBType].ReturnStream(pStream);

	return bRet;
}
//-------------------------------------------------------------------------------
//得到roledata表最大序列号
//-------------------------------------------------------------------------------
DWORD CLoongLoongDBCtrl::GetRoleDataMaxNum(EDBType eDBType)
{
	MyStream* pStream = m_LoongDB[eDBType].GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	pStream->SetSelect("roledata","max(RoleID)");
	// 查询
	QueryResult* pResult = m_LoongDB[eDBType].Query(pStream);

	// 查询结果为空
	if( !P_VALID(pResult) || pResult->GetRowCount() <= 0) return GT_INVALID;

	return (*pResult)[0].GetDword();
}
//-------------------------------------------------------------------------------
//得到petdata表最大序列号
//-------------------------------------------------------------------------------
DWORD CLoongLoongDBCtrl::GetPetDataMaxNum(EDBType eDBType)
{
	MyStream* pStream = m_LoongDB[eDBType].GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	pStream->SetSelect("pet_data","max(pet_id)");
	// 查询
	QueryResult* pResult = m_LoongDB[eDBType].Query(pStream);

	// 查询结果为空
	if( !P_VALID(pResult) || pResult->GetRowCount() <= 0) return GT_INVALID;

	return (*pResult)[0].GetDword();
}
//-------------------------------------------------------------------------------
//得到内部物品最大序号
//-------------------------------------------------------------------------------
INT64 CLoongLoongDBCtrl::GetMaxSerialInternal(EDBType eDBType)
{
	INT64 n64ItemSerial = MIN_ITEM_SERIAL_INTERNAL;

	INT64 nMaxItem			=	MIN_ITEM_SERIAL_INTERNAL;
	INT64 nMaxItemDel		=	MIN_ITEM_SERIAL_INTERNAL;
	INT64 nMaxItemBaibao	=	MIN_ITEM_SERIAL_INTERNAL;

	BOOL bRet = GetMaxItemSerialInTable("item", nMaxItem, eDBType) &&
		GetMaxItemSerialInTable("item_del", nMaxItemDel, eDBType) &&
		GetMaxItemSerialInTable("item_baibao", nMaxItemBaibao, eDBType);

	if( !bRet )
	{
		return n64ItemSerial = GT_INVALID;
	}

	INT64 nMax = max(nMaxItem, nMaxItemDel);
	nMax = max(nMax, nMaxItemBaibao);

	return n64ItemSerial = max(nMax, n64ItemSerial);
}

//-------------------------------------------------------------------------------
//得到外部物品最小序号
//-------------------------------------------------------------------------------
INT64 CLoongLoongDBCtrl::GetMinSerialOutter(EDBType eDBType)
{
	INT64 n64ItemSerial = MAX_ITEM_SERIAL_OUTTER;

	INT64 nMinItem			=	MAX_ITEM_SERIAL_OUTTER;
	INT64 nMinItemDel		=	MAX_ITEM_SERIAL_OUTTER;
	INT64 nMinItemBaibao	=	MAX_ITEM_SERIAL_OUTTER;

	BOOL bRet = GetMinItemSerialInTable("item", nMinItem, eDBType, MIN_ITEM_SERIAL_OUTTER) &&
		GetMinItemSerialInTable("item_del", nMinItemDel, eDBType, MIN_ITEM_SERIAL_OUTTER) &&
		GetMinItemSerialInTable("item_baibao", nMinItemBaibao, eDBType, MIN_ITEM_SERIAL_OUTTER);

	if( !bRet )
	{
		return n64ItemSerial = GT_INVALID;
	}

	INT64 nMin = min(nMinItem, nMinItemDel);
	nMin = min(nMin, nMinItemBaibao);

	return n64ItemSerial = min(nMin, n64ItemSerial);

}


//-------------------------------------------------------------------------------
//得到物品最大序号
//-------------------------------------------------------------------------------
BOOL CLoongLoongDBCtrl::GetMaxItemSerialInTable(LPCSTR szTable, INT64& n64ItemSerial,EDBType eDBType,INT64 n64UpLimit)
{
	MyStream* pStream  = m_LoongDB[eDBType].GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	pStream->SetSelect(szTable, "max(SerialNum)");
	if(n64UpLimit != GT_INVALID)
	{
		pStream->SetWhere();
		pStream->FillString("SerialNum<=") << n64UpLimit;
	}


	QueryResult* pResult = m_LoongDB[eDBType].Query(pStream);

	m_LoongDB[eDBType].ReturnStream(pStream);

	if( !P_VALID(pResult) ) return FALSE;

	if( 0 == pResult->GetRowCount() )
	{
		m_LoongDB[eDBType].FreeQueryResult(pResult);
		return TRUE;
	}

	if( (*pResult)[0].IsNull() )
	{
		m_LoongDB[eDBType].FreeQueryResult(pResult);
		return TRUE;
	}

	n64ItemSerial = (*pResult)[0].GetLong();

	m_LoongDB[eDBType].FreeQueryResult(pResult);

	return TRUE;
}

//-------------------------------------------------------------------------------
//得到划拨物品最小序号
//-------------------------------------------------------------------------------
INT64 CLoongLoongDBCtrl::GetItemMinSerialBill(EDBType eDBType)
{
	INT64 n64ItemSerial = MAX_ITEM_SERIAL_BILL;

	INT64 nMinItem			=	MAX_ITEM_SERIAL_BILL;
	INT64 nMinItemDel		=	MAX_ITEM_SERIAL_BILL;
	INT64 nMinItemBaibao	=	MAX_ITEM_SERIAL_BILL;

	BOOL bRet = GetMinItemSerialInTable("item", nMinItem, eDBType) &&
		GetMinItemSerialInTable("item_del", nMinItemDel, eDBType) &&
		GetMinItemSerialInTable("item_baibao", nMinItemBaibao, eDBType);

	if( !bRet )
	{
		return n64ItemSerial = GT_INVALID;
	}

	INT64 nMin = min(nMinItem, nMinItemDel);
	nMin = min(nMin, nMinItemBaibao);

	return n64ItemSerial = min(nMin, n64ItemSerial);
}
//---------------------------------------------------------------------------------------
// 得到某个表中最小的64位ID
//---------------------------------------------------------------------------------------
BOOL CLoongLoongDBCtrl::GetMinItemSerialInTable(LPCSTR szTable, INT64& n64ItemSerial,EDBType eDBType,INT64 n64LowLimit)
{

	MyStream* pStream  = m_LoongDB[eDBType].GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	pStream->SetSelect(szTable, "min(SerialNum)");
	if(n64LowLimit != GT_INVALID)
	{
		pStream->SetWhere();
		pStream->FillString("SerialNum>=") << n64LowLimit;
	}

	QueryResult* pResult = m_LoongDB[eDBType].Query(pStream);

	m_LoongDB[eDBType].ReturnStream(pStream);

	if( !P_VALID(pResult) ) return FALSE;

	if( 0 == pResult->GetRowCount() )
	{
		m_LoongDB[eDBType].FreeQueryResult(pResult);
		return TRUE;
	}

	if( (*pResult)[0].IsNull() )
	{
		m_LoongDB[eDBType].FreeQueryResult(pResult);
		return TRUE;
	}

	n64ItemSerial = (*pResult)[0].GetLong();

	m_LoongDB[eDBType].FreeQueryResult(pResult);

	return TRUE;
}

// 创建物品
BOOL CLoongLoongDBCtrl::CreateItem(INT64 n64SerialNum, DWORD ItemTypdID, DWORD AccountID, DWORD RoleID)
{
	std::vector<tagTWrap_Loong_ItemBaiBao*> vecData;
	tagTWrap_Loong_ItemBaiBao* pItem = new tagTWrap_Loong_ItemBaiBao;

	pItem->stOthers.n64Serial		= n64SerialNum;
	pItem->stOthers.dwTypeID		= ItemTypdID;
	pItem->stOthers.dwAccountID		= AccountID;
	pItem->stOthers.dwCreateTime	= GetCurrentDWORDTime(); 
	pItem->stOthers.eCreateMode		= EICM_ServerMerge;
	pItem->stOthers.eConType		= EICT_Baibao;
	pItem->stOthers.byBind			= FALSE;
	pItem->stOthers.dw1stGainTime	= GetCurrentDWORDTime();
	pItem->stOthers.n16Num			= 1;
	pItem->stOthers.dwCreateID		= 0xffffffff;
	pItem->stOthers.dwCreatorID		= 0xffffffff;
	pItem->stOthers.dwOwnerID		= RoleID;
	pItem->stOthers.dwNameID		= 0xffffffff;
	pItem->stOthers.nUseTimes		= 0;
	pItem->stOthers.byBind			= 0;
	pItem->stOthers.bLock			= 0;
	pItem->stOthers.n16Index		= 0;

	vecData.push_back(pItem);

	//插入物品
	BOOL bRet = Loong_Insert(vecData,EM_DB_TAR);
	if(bRet == FALSE)
	{
		delete pItem;
		return FALSE;
	}

	delete pItem;
	return TRUE;
}


//	读取AccountCommon表
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_AccountCommon*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryAccountCommon,&CLoongLoongDBCtrl::ProcResQueryAccountCommon);
}
//	读取BlackList表
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_BlackList*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryBlackList,&CLoongLoongDBCtrl::ProcResQueryBlackList);
}
//	读取Buff表
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_Buff*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryBuff,&CLoongLoongDBCtrl::ProcResQueryBuff);
}
//	氏族
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_ClanData*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryClanData,&CLoongLoongDBCtrl::ProcResQueryClanData);
}
//	enemy
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_Enemy*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryEnemy,&CLoongLoongDBCtrl::ProcResQueryEnemy);
}
/*
//	武器
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_Equip*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryEquip,&CLoongLoongDBCtrl::ProcResQueryEquip);
}
//武器删除
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_EquipDel*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryEquipDel,&CLoongLoongDBCtrl::ProcResQueryEquip);
}
*/
//武器baibao
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_EquipBaiBao*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryEquipBaiBao,&CLoongLoongDBCtrl::ProcResQueryEquip);
}

//氏族珍宝激活表
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_ActClanTreasure*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryActClanTreasure,&CLoongLoongDBCtrl::ProcResQueryActClanTreasure);
}
//氏族进入快照
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_FameHallEnterSnap*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryFameHallEnterSnap,&CLoongLoongDBCtrl::ProcResQueryFameHallEnterSnap);
}
//氏族声望重置时间快照
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_RepRstTime*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryRepRstTime,&CLoongLoongDBCtrl::ProcResQueryRepRstTime);
}
//好友列表(单向)
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_Friend*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryFriendSave,&CLoongLoongDBCtrl::ProcResQueryFriendSave);
}
//友好度
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_FriendshipSave*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryFriendshipSave,&CLoongLoongDBCtrl::ProcResQueryFriendshipSave);
}
//团购
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_GroupPurchase*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryGroupPurchase,&CLoongLoongDBCtrl::ProcResQueryGroupPurchase);
}
//帮派
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_Guild*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryGuild,&CLoongLoongDBCtrl::ProcResQueryGuild);
}
//帮派成员
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_GuildMem*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryGuildMem,&CLoongLoongDBCtrl::ProcResQueryGuildMem);
}
//item
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_Item*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryItem,&CLoongLoongDBCtrl::ProcResQueryItem);
}
//item baibao
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_ItemBaiBao*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryItemBaiBao,&CLoongLoongDBCtrl::ProcResQueryItem);
}
//item del
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_ItemDel*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryItemDel,&CLoongLoongDBCtrl::ProcResQueryItem);
}
//item cd time
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_ItemCDTime*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryItemCDTime,&CLoongLoongDBCtrl::ProcResQueryItemCDTime);
}
//item need log
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_ItemNeedLog*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryItemNeedLog,&CLoongLoongDBCtrl::ProcResQueryItemNeedLog);
}
//item log baibao
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_LogBaiBao*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryLogBaiBao,&CLoongLoongDBCtrl::ProcResQueryLogBaiBao);
}
//item role del
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_RoleDel*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryRoleDel,&CLoongLoongDBCtrl::ProcResQueryRoleDel);
}
//item role data
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_RoleData*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryRoleData,&CLoongLoongDBCtrl::ProcResQueryRoleData);
}
//item role data del
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_RoleDataDel*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryRoleDataDel,&CLoongLoongDBCtrl::ProcResQueryRoleData);
}
//skill
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_Skill*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQuerySkill,&CLoongLoongDBCtrl::ProcResQuerySkill);
}
//task
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_Task*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryTask,&CLoongLoongDBCtrl::ProcResQueryTask);
}
//task done
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_TaskDone*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryTaskDone,&CLoongLoongDBCtrl::ProcResQueryTaskDone);
}
//title
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_Title*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryTitle,&CLoongLoongDBCtrl::ProcResQueryTitle);
}
//名帖
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_VisitingCard*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryVisitingCard,&CLoongLoongDBCtrl::ProcResQueryVisitingCard);
}
//元宝
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_YBAccount*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryYBAccount,&CLoongLoongDBCtrl::ProcResQueryYBAccount);
}
//元宝交易订单
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_YuanBaoOrder*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryYuanBaoOrder,&CLoongLoongDBCtrl::ProcResQueryYuanBaoOrder);
}

//bill_item
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_BillItem*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryBillItem,&CLoongLoongDBCtrl::ProcResQueryBillItem);
}
//bill_yuanbao
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_BillYuanBao*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryBillYuanbao,&CLoongLoongDBCtrl::ProcResQueryBillYuanbao);
}
//bill_yuanbao_log
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_BillYuanBaoLog*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryBillYuanbaoLog,&CLoongLoongDBCtrl::ProcResQueryBillYuanbaoLog);
}
//pet_data
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_PetData*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryPetData,&CLoongLoongDBCtrl::ProcResQueryPetData);
}
//pet skill
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_PetSkill*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryPetSkill,&CLoongLoongDBCtrl::ProcResQueryPetSkill);
}
//vip_stall
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_VipStall*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryVipStall,&CLoongLoongDBCtrl::ProcResQueryVipStall);
}
//guild_commodity
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_GuildCommodity*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryGuildCommodity,&CLoongLoongDBCtrl::ProcResQueryGuildCommodity);
}
//guild_skill
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_GuildSkill*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryGuildSkill,&CLoongLoongDBCtrl::ProcResQueryGuildSkill);
}
//vip_netbar
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_VipNetBar*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryVipBar,&CLoongLoongDBCtrl::ProcResQueryVipBar);
}
//guild_upgrade
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_GuildFacilities*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryGuildUpgrade,&CLoongLoongDBCtrl::ProcResQueryGuildUpgrade);
}
//commerce_rank
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_CommerceRank*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryCommerceRank,&CLoongLoongDBCtrl::ProcResQueryCommerceRank);
}
//activity
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_ActivityData*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryActivity,&CLoongLoongDBCtrl::ProcResQueryActivity);
}
//left msg
BOOL CLoongLoongDBCtrl::Loong_Query(std::vector <tagTWrap_Loong_LeftMsg*> &vecData,EDBType eDBType)
{
	return Load(&vecData,eDBType,&CLoongLoongDBCtrl::FormatQueryLeftMsg,&CLoongLoongDBCtrl::ProcResQueryLeftMsg);
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//按用户查询item表
BOOL CLoongLoongDBCtrl::Loong_AccountQuery(std::vector <tagTWrap_Loong_Item*>&vecData,DWORD dwAccountID,EDBType eDBType)
{
	return AccountLoad(&vecData,dwAccountID,eDBType,&CLoongLoongDBCtrl::FormatAccountQueryItem,&CLoongLoongDBCtrl::ProcResQueryItem);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//分页查询
EPagingState CLoongLoongDBCtrl::Loong_PagingQuery(std::vector <tagTWrap_Loong_Equip*>&vecData,INT64 n64Limit,EDBType eDBType)
{
	return PagingLoad(&vecData,n64Limit,eDBType,&CLoongLoongDBCtrl::FormatPagingQueryEquip,&CLoongLoongDBCtrl::ProcResQueryEquip);
}

EPagingState CLoongLoongDBCtrl::Loong_PagingQuery(std::vector <tagTWrap_Loong_EquipDel*>&vecData,INT64 n64Limit,EDBType eDBType)
{
	return PagingLoad(&vecData,n64Limit,eDBType,&CLoongLoongDBCtrl::FormatPagingQueryEquipDel,&CLoongLoongDBCtrl::ProcResQueryEquip);
}
//分页查询
EPagingState CLoongLoongDBCtrl::Loong_PagingQuery(std::vector <tagTWrap_Loong_TaskDone*>&vecData,INT64 n64Limit,EDBType eDBType)
{
	return PagingLoad(&vecData,n64Limit,eDBType,&CLoongLoongDBCtrl::FormatPagingQueryTaskDone,&CLoongLoongDBCtrl::ProcResQueryTaskDone);
}
//分页查询
EPagingState CLoongLoongDBCtrl::Loong_PagingQuery(std::vector <tagTWrap_Loong_Task*>&vecData,INT64 n64Limit,EDBType eDBType)
{
	return PagingLoad(&vecData,n64Limit,eDBType,&CLoongLoongDBCtrl::FormatPagingQueryTask,&CLoongLoongDBCtrl::ProcResQueryTask);
}

EPagingState CLoongLoongDBCtrl::Loong_PagingQuery(std::vector <tagTWrap_Loong_ItemDel*>&vecData,INT64 n64Limit,EDBType eDBType)
{
	return PagingLoad(&vecData,n64Limit,eDBType,&CLoongLoongDBCtrl::FormatPagingQueryItemDel,&CLoongLoongDBCtrl::ProcResQueryItem);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//AccountCommon表
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_AccountCommon*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertAccountCommon);
}
//BlackList表
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_BlackList*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertBlackList);
}
//Buff表
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_Buff*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertBuff);
}
//	氏族
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_ClanData*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertClanData);
}
//	enemy
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_Enemy*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertEnemy);
}

//	武器
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_Equip*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,"equip",&CLoongLoongDBCtrl::FormatInsertEquip);
}
//武器baibao
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_EquipBaiBao*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,"equip_baibao",&CLoongLoongDBCtrl::FormatInsertEquip);
}
//武器删除
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_EquipDel*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,"equip_del",&CLoongLoongDBCtrl::FormatInsertEquip);
}
//氏族珍宝激活表
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_ActClanTreasure*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertActClanTreasure);
}
//氏族进入快照
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_FameHallEnterSnap*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertFameHallEnterSnap);
}
//氏族声望重置时间快照
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_RepRstTime*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertRepRstTime);
}
//好友列表(单向)
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_Friend*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertFriendSave);
}
//友好度
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_FriendshipSave*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertFriendshipSave);
}
//团购
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_GroupPurchase*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertGroupPurchase);
}
//帮派
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_Guild*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertGuild);
}
//帮派成员
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_GuildMem*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertGuildMem);
}
//item
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_Item*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,"item",&CLoongLoongDBCtrl::FormatInsertItem);
}
//item baibao
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_ItemBaiBao*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,"item_baibao",&CLoongLoongDBCtrl::FormatInsertItem);
}
//item del
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_ItemDel*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,"item_del",&CLoongLoongDBCtrl::FormatInsertItem);
}
//item cd time
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_ItemCDTime*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertItemCDTime);
}
//item need log
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_ItemNeedLog*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertItemNeedLog);
}
//item log baibao
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_LogBaiBao*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertLogBaiBao);
}
//item role del
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_RoleDel*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertRoleDel);
}
//item role data
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_RoleData*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,"roledata",&CLoongLoongDBCtrl::FormatInsertRoleData);
}

//item role data del
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_RoleDataDel*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,"roledata_del",&CLoongLoongDBCtrl::FormatInsertRoleData);
}
//skill
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_Skill*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertSkill);
}
//task
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_Task*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertTask);
}
//task done
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_TaskDone*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertTaskDone);
}
//title
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_Title*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertTitle);
}
//名帖
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_VisitingCard*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertVisitingCard);
}
//元宝
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_YBAccount*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertYBAccount);
}
//元宝交易订单
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_YuanBaoOrder*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertYuanBaoOrder);
}

//bill_item
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_BillItem*> &vecData,EDBType eDBType)
{
return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertBillItem);
}
//bill_yuanbao
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_BillYuanBao*> &vecData,EDBType eDBType)
{
return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertBillYuanbao);
}
//bill_yuanbao_log
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_BillYuanBaoLog*> &vecData,EDBType eDBType)
{
return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertBillYuanbaoLog);
}
//pet_data
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_PetData*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertPetData);
}
//pet skill
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_PetSkill*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertPetSkill);
}
//vip_stall
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_VipStall*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertVipStall);
}
//guild_commodity
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_GuildCommodity*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertGuildCommodity);
}
//guild_skill
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_GuildSkill*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertGuildSkill);
}
//vip_netbar
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_VipNetBar*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertVipBar);
}
//guild_upgrade
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_GuildFacilities*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertGuildUpgrade);
}
//commerce_rank
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_CommerceRank*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertCommerceRank);
}
//activity
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_ActivityData*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertActivity);
}
//left_msg
BOOL CLoongLoongDBCtrl::Loong_Insert(std::vector <tagTWrap_Loong_LeftMsg*> &vecData,EDBType eDBType)
{
	return Insert(&vecData,eDBType,&CLoongLoongDBCtrl::FormatInsertLeftMsg);
}
//------------------------------------------------------------------------------------
// 数据库预警回调函数
//------------------------------------------------------------------------------------
VOID LoongDBCallBack(DataBase* pDB, INT nReason, INT nParam)
{
	TCHAR cTemp[200];

	if (nReason == DataBase::EDBE_System)
	{
		_stprintf(cTemp,_T("Beton Warning CallBack: Reason  %s , nParam = %u\r\n"), _T("EDBE_System"), nParam);
		g_clsOutput.Output(cTemp);
	}
	else if(nReason == DataBase::EDBE_QueueFull)
	{
		_stprintf(cTemp,_T("Beton Warning CallBack: Reason  %s , nParam = %u\r\n"), _T("EDBE_QueueFull"), nParam);
		g_clsOutput.Output(cTemp);
	}
	else if(nReason == DataBase::EDBE_PoolFull)
	{
		_stprintf(cTemp,_T("Beton Warning CallBack: Reason  %s , nParam = %u\r\n"), _T("EDBE_PoolFull"), nParam);
		g_clsOutput.Output(cTemp);
	}
	else
	{	
		_stprintf(cTemp,_T("Beton Warning CallBack: Reason Unknow,nReason = %u, nParam = %u\r\n"), nReason, nParam);
		g_clsOutput.Output(cTemp);
	}
}