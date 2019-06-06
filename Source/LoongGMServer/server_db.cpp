#include "StdAfx.h"
#include "server_db.h"

#include "../WorldDefine/container_define.h"
#include "../WorldDefine/ItemDefine.h"

#include "../ServerDefine/base_define.h"
#include "../ServerDefine/gm_tool_define.h"

BOOL ServerDB::Init( LPCTSTR strHost, LPCTSTR strUser, LPCTSTR strPassword, LPCTSTR strDatabase, INT nPort )
{
	m_pUtil = "Util";

	BOOL bRet = m_DB.Init(strHost, strUser, strPassword, strDatabase, nPort);

	return bRet;
}

VOID ServerDB::Destroy()
{

}

BOOL ServerDB::DBResumeRole( DWORD dwRoleID, LPCTSTR tszNewName )
{
	BOOL bRtv = FALSE;

	if( !P_VALID(tszNewName) ) return FALSE;

	// 获取一个Stream
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 获取一条连接
	Connection* pCon = m_DB.GetFreeConnection();


	// 设置sql语句
	pStream->SetUpdate("roledata");
	pStream->FillString("RemoveFlag=") << 0;
	pStream->FillString(",RemoveTime=") << DWORD(-1);

	if (_tcslen(tszNewName) > 0)
	{
		pStream->FillString(",RoleName='").FillString(tszNewName, pCon).FillString("'");
		DWORD dwNameCrc = m_pUtil->Crc32(tszNewName);
		pStream->FillString(",RoleNameCrc=") << dwNameCrc;
	}
	
	pStream->SetWhere();
	pStream->FillString("RoleID=") << dwRoleID;

	// 归还连接
	m_DB.ReturnConnection(pCon);

	// 进行操作
	bRtv = m_DB.Execute(pStream);

	// 释放Stream
	m_DB.ReturnStream(pStream);

	return bRtv;
}

BOOL ServerDB::DBRoleModify(LPCTSTR tszName, INT nLel, INT nSkill, INT nYuanbao, INT64 n64Exp, INT64 n64Money)
{
	BOOL bRtv = FALSE;

	if( !P_VALID(tszName) ) return FALSE;

	// 获取一个Stream
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 获取一条连接
	Connection* pCon = m_DB.GetFreeConnection();

	// 设置sql语句
 	pStream->SetUpdate("roledata");
	
	BOOL bFirst = TRUE;
	if ((nLel > 0)&&(n64Exp >= 0))
	{
		pStream->FillString("Level=") << nLel;
		pStream->FillString(",ExpCurLevel=") << n64Exp;
		bFirst = FALSE;
	}
	if(nSkill>=0)
	{
		if(!bFirst)
			pStream->FillString(",");
		pStream->FillString("TalentPtAvail=") << nSkill;			
		bFirst = FALSE;
	}
	if(nYuanbao>=0)
	{
		if(!bFirst)
			pStream->FillString(",");
		pStream->FillString("BagYuanBao=") << nYuanbao;
		bFirst = FALSE;
	}
	if(n64Money>=0)
	{
		if(!bFirst)
			pStream->FillString(",");
		pStream->FillString("BagSilver=")	<< n64Money % 10000;
		pStream->FillString(",BagGold=")	<< n64Money / 10000;
	}
	
 	pStream->SetWhere();
	pStream->FillString("RoleName='").FillString(tszName, pCon).FillString("'");

	// 归还连接
	m_DB.ReturnConnection(pCon);

	// 进行操作
	bRtv = m_DB.Execute(pStream);

	// 释放Stream
	m_DB.ReturnStream(pStream);

	return bRtv;
}

DWORD ServerDB::DBQueryItemExist(INT64 n64SerialNum)
{
	// 获取流
	Beton::MyStream* pStream = m_DB.GetStream();

	// 格式化
	pStream->SetSelect("item", "*");
	pStream->SetWhere();
	pStream->FillString("SerialNum=") << n64SerialNum;

	// 查询数据库
	Beton::QueryResult* pResult = m_DB.Query(pStream);
	if(!(P_VALID(pResult))) return E_GMTool_Err;

	if( pResult->GetRowCount() > 0 )
		return TRUE;
	else
		return FALSE;

	// 释放流和结果
	m_DB.ReturnStream(pStream);
	m_DB.FreeQueryResult(pResult);
}

DWORD ServerDB::DBDeleteItem( DWORD dwRoleID, INT64 n64ItemID )
{
	DWORD dwTypeID = GT_INVALID;
	if (!DBQueryTypeIDBySerial(n64ItemID, dwTypeID, "item") &&	!DBQueryTypeIDBySerial(n64ItemID, dwTypeID, "item_baibao") )
		return E_GMTool_DeleteItem_ItemNotExist;

	BOOL bRememberDel = FALSE;
	if (MIsEquipment(dwTypeID))
	{
		bRememberDel = TRUE;
		if ( !(DBDeleteBySerial(n64ItemID, "equip", TRUE) && DBDeleteBySerial(n64ItemID, "equip_baibao", TRUE)) )
			return E_GMTool_Err;
	}

	if ( !(DBDeleteBySerial(n64ItemID, "item", bRememberDel) && DBDeleteBySerial(n64ItemID, "item_baibao", bRememberDel)) )
		return E_GMTool_Err;

	return E_GMTool_Success;
}

DWORD ServerDB::DBResumeItem(INT64 n64SerialNum, DWORD dwTargetID, DWORD dwAccountID)
{
	DWORD dwTypeID = GT_INVALID;
	if ( !(DBQueryTypeIDBySerial(n64SerialNum, dwTypeID, "item_del")) )
	{
		return E_GMTool_ResumeItemNotExit;
	}

	if (!MIsEquipment(dwTypeID))
	{
		return E_GMTool_ResumeItemNotEquip;
	}

	// 将del表中的数据分别恢复到item_baibao和equip_baibao中
	DWORD dwRtv = DBQueryItemExist(n64SerialNum);
	if( dwRtv == FALSE )
	{
		MyStream* pStream = m_DB.GetStream();
		MyStream* pStreamS = m_DB.GetStream();

		pStream->SetCopyItem("item_baibao", "*", "item_del");
		pStream->SetWhere();
		pStream->FillString("SerialNum=")<<n64SerialNum;

		pStreamS->SetCopyItem("equip_baibao", "*", "equip_del");
		pStreamS->SetWhere();
		pStreamS->FillString("SerialNum=")<<n64SerialNum;

		m_DB.Execute(pStream);
		m_DB.Execute(pStreamS);

		m_DB.ReturnStream(pStream);
		m_DB.ReturnStream(pStreamS);
	}
	else if( dwRtv == TRUE )
	{
		return E_GMTool_ResumeItemNotExit;
	}
	else
		return dwRtv;

	// 删除del表中的相应数据
	MyStream* pStreamT = m_DB.GetStream();
	MyStream* pStreamF = m_DB.GetStream();

	pStreamT->SetDelete("item_del");
	pStreamT->SetWhere();
	pStreamT->FillString("SerialNum=")<<n64SerialNum;

	pStreamF->SetDelete("equip_del");
	pStreamF->SetWhere();
	pStreamF->FillString("SerialNum=")<<n64SerialNum;

	m_DB.Execute(pStreamT);
	m_DB.Execute(pStreamF);

	m_DB.ReturnStream(pStreamT);
	m_DB.ReturnStream(pStreamF);

	// 修改装备所有者
	if( dwTargetID != 0 && dwAccountID != 0 )
	{
		MyStream* pStreamR = m_DB.GetStream();

		pStreamR->SetUpdate("item_baibao");
		pStreamR->FillString("OwnerID=")<<dwTargetID;
		pStreamR->FillString(",AccountID=")<<dwAccountID;
		pStreamR->SetWhere();
		pStreamR->FillString("SerialNum=")<<n64SerialNum;

		m_DB.Execute(pStreamR);

		m_DB.ReturnStream(pStreamR);
	}
	
	// 设置在百宝表中的suffix位置
	MyStream* pStreamQ = m_DB.GetStream();

	pStreamQ->SetUpdate("item_baibao");
	pStreamQ->FillString("Suffix=0");
	pStreamQ->SetWhere();
	pStreamQ->FillString("SerialNum=")<<n64SerialNum;

	m_DB.Execute(pStreamQ);
	
	m_DB.ReturnStream(pStreamQ);
	
	return E_GMTool_Success;
}

BOOL ServerDB::DBQueryNameByRoleID( IN DWORD dwRoleID, OUT LPTSTR szName )
{
	BOOL bRtv = FALSE;

	if( !P_VALID(szName) ) return FALSE;

	// 获取一个Stream
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 设置sql语句
	pStream->SetSelect("roledata", "RoleName");
	pStream->SetWhere();
	pStream->FillString("RoleID=")	<< dwRoleID;

	// 查询
	Beton::QueryResult* pResult = m_DB.Query(pStream);
	
	// 处理结果
	if (!P_VALID(pResult))
	{	
		szName[0] = 0;
		bRtv = FALSE;
	}
	else if (1 == pResult->GetRowCount())
	{
		(*pResult)[0].GetTCHAR(szName, X_SHORT_NAME);
		bRtv = TRUE;
	}
	else
	{
		szName[0] = 0;
		bRtv = FALSE;
	}


	// 回收结果
	m_DB.FreeQueryResult(pResult);

	// 释放Stream
	m_DB.ReturnStream(pStream);

	return bRtv;
}

BOOL ServerDB::DBQueryRoleIDByName( IN LPTSTR szName, OUT DWORD &dwRoleID )
{
	BOOL bRtv = FALSE;

	if( !P_VALID(szName) ) return FALSE;

	// 获取一个Stream
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 获取一条连接
	Connection* pCon = m_DB.GetFreeConnection();

	// 设置sql语句
	pStream->SetSelect("roledata", "RoleID");
	pStream->SetWhere();
	pStream->FillString("RoleName='").FillString(szName, pCon).FillString("'");

	// 进行操作
	Beton::QueryResult* pResult = m_DB.Query(pStream);

	// 处理结果
	
	if (!P_VALID(pResult))
	{
		dwRoleID = GT_INVALID;
		bRtv = FALSE;
	}
	else if (1 == pResult->GetRowCount())
	{
		INT nLen = 0;
		dwRoleID = (*pResult)[0].GetDword();
		bRtv = TRUE;
	}
	else
	{
		dwRoleID = GT_INVALID;
		bRtv = FALSE;
	}

	// 回收结果
	m_DB.FreeQueryResult(pResult);

	// 归还连接
	m_DB.ReturnConnection(pCon);

	// 释放Stream
	m_DB.ReturnStream(pStream);

	return bRtv;
}

BOOL ServerDB::DBQueryRoleIDByAccountID( IN DWORD dwAccountID, OUT DWORD &dwRoleID )
{
	ASSERT(0);
	return FALSE;
}

BOOL ServerDB::DBQueryAccountIDByRoleID( IN DWORD dwRoleID, OUT DWORD &dwAccountID )
{
	BOOL bRtv = FALSE;

	// 获取一个Stream
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 获取一条连接
	Connection* pCon = m_DB.GetFreeConnection();

	// 设置sql语句
	pStream->SetSelect("roledata", "AccountID");
	pStream->SetWhere();
	pStream->FillString("RoleID=")	<< dwRoleID;

	// 进行操作
	Beton::QueryResult* pResult = m_DB.Query(pStream);

	// 处理结果
	if (!P_VALID(pResult))
	{
		dwRoleID = GT_INVALID;
		bRtv = FALSE;
	}
	else if (1 == pResult->GetRowCount())
	{
		INT nLen = 0;
		dwAccountID = (*pResult)[0].GetDword();
		bRtv = TRUE;
	}
	else
	{	
		dwAccountID = GT_INVALID;
		bRtv = FALSE;
	}

	// 回收结果
	m_DB.FreeQueryResult(pResult);

	// 归还连接
	m_DB.ReturnConnection(pCon);

	// 释放Stream
	m_DB.ReturnStream(pStream);

	return bRtv;
}

BOOL ServerDB::DBCreateItem(DWORD dwAccountID, DWORD dwRoleID, DWORD dwItemTypeID, INT nItemNum, BYTE byQuality)
{
	// 找到最小的物品序列号
	INT64 n64Serial = GT_INVALID;
	
	BOOL bRtv = TRUE;

	if (!DBFindValidOuterSerial(n64Serial))
	{
		return FALSE;
	}
	// 放入百宝袋item_baibao
	Connection* pCon = m_DB.GetFreeConnection();

	m_DB.BeginTransaction(pCon);

	if (!DBWaitCreateItemInBaiBao(dwAccountID, dwRoleID, n64Serial, dwItemTypeID, nItemNum, pCon))
	{
		m_DB.RollBack(pCon);
		m_DB.ReturnConnection(pCon);
		return FALSE;
	}
	// 放入百宝袋equip_baibao
	else if (MIsEquipment(dwItemTypeID) && !DBWaitCreateEquipSpecInBaiBao(n64Serial, dwItemTypeID, byQuality, pCon))
	{
		m_DB.RollBack(pCon);
		m_DB.ReturnConnection(pCon);
		return FALSE;
	}

	m_DB.EndTransaction(pCon);
	m_DB.ReturnConnection(pCon);

	DBInsertBaiBaoLog(dwAccountID, dwRoleID, EBBRT_System, dwItemTypeID, GetCurrentDWORDTime());
	
	return TRUE;
}

// BOOL ServerDB::DBCreateItem(DWORD dwAccountID, DWORD dwRoleID, DWORD dwItemTypeID, INT nItemNum, BYTE byQuality)
// {
// 	// 找到最小的物品序列号
// 	INT64 n64Serial = GT_INVALID;
// 
// 	BOOL bRtv = TRUE;
// 
// 	if (!DBFindValidOuterSerial(n64Serial))
// 	{
// 		bRtv = FALSE;
// 	}
// 	// 放入百宝袋item_baibao
// 	else if (!DBWaitCreateItemInBaiBao(dwAccountID, dwRoleID, n64Serial, dwItemTypeID, nItemNum))
// 	{
// 		bRtv = FALSE;
// 	}
// 	// 放入百宝袋equip_baibao
// 	else if (MIsEquipment(dwItemTypeID) && !DBWaitCreateEquipSpecInBaiBao(n64Serial, dwItemTypeID, byQuality))
// 	{
// 		DBDeleteBySerial(n64Serial, "item_baibao");
// 		bRtv = FALSE;
// 	}
// 
// 	if (bRtv)
// 	{
// 		DBInsertBaiBaoLog(dwAccountID, dwRoleID, EBBRT_System, dwItemTypeID, GetCurrentDWORDTime());
// 	}
// 
// 	return bRtv;
// }

BOOL ServerDB::DBWaitCreateItemInBaiBao( DWORD dwAccountID, DWORD dwRoleID, INT64 n64Serial, DWORD dwItemTypeID, INT nItemNum, Connection* pCon )
{
	BOOL bRtv = FALSE;

	// 获取一个Stream
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 获取一条连接
// 	Connection* pCon = m_DB.GetFreeConnection();

	// 设置sql语句
	pStream->SetInsert("item_baibao");
	pStream->FillString("SerialNum=")	<< n64Serial;
	pStream->FillString(",TypeID=")		<< dwItemTypeID;
	pStream->FillString(",Num=")		<< nItemNum;
	pStream->FillString(",AccountID=")  << dwAccountID;
	pStream->FillString(",OwnerID=")	<< dwRoleID;

	CHAR szTime[X_DATATIME_LEN+1]= {0};
	DwordTime2DataTime(szTime, X_DATATIME_LEN + 1, GetCurrentDWORDTime());

	pStream->FillString(",CreateTime='").FillString(szTime).FillString("'");

	pStream->FillString(",CreateID=")		<<(DWORD)GT_INVALID;
	pStream->FillString(",CreatorID=")		<<(DWORD)GT_INVALID;

	pStream->FillString(",Bind=")			<<EBS_Unknown;
	pStream->FillString(",CreateMode=")		<<EICM_GMTool;
	pStream->FillString(",ContainerTypeID=")<<EICT_Baibao;
	pStream->FillString(",Suffix=")         <<(BYTE)GT_INVALID;

	// 进行操作
	bRtv = m_DB.WaitExecute(pStream, pCon);

	// 归还连接
//	m_DB.ReturnConnection(pCon);

	// 释放Stream
	m_DB.ReturnStream(pStream);

	return bRtv;
}

BOOL ServerDB::DBFindValidOuterSerial( INT64 &n64SerialNum)
{
	/*select min(ss) from 
	( 
		select min(SerialNum)as ss from  item where  SerialNum>=300000000001 and SerialNum<=400000000000
	union all
		select  min(SerialNum)as ss from item_del where  SerialNum>=300000000001 and SerialNum<=400000000000
	union all
		select min(SerialNum)as ss  from item_baibao where  SerialNum>=300000000001 and SerialNum<=400000000000
	
	) as TT where ss is not null*/

	MyStream* pStream = m_DB.GetStream();
	
	pStream->FillString("select min(ss) from "
						"( select min(SerialNum)as ss from item");
	
	pStream->FillString(" where SerialNum>=")<<MIN_ITEM_SERIAL_OUTTER;
	pStream->FillString(" and SerialNum<=")<<MAX_ITEM_SERIAL_OUTTER;
	pStream->FillString(" union all");
		
	pStream->FillString(" select min(SerialNum)as ss from item_del");

	pStream->FillString(" where SerialNum>=")<<MIN_ITEM_SERIAL_OUTTER;
	pStream->FillString(" and SerialNum<=")<<MAX_ITEM_SERIAL_OUTTER;
	pStream->FillString(" union all");

	pStream->FillString(" select min(SerialNum)as ss from item_baibao");

	pStream->FillString(" where SerialNum>=")<<MIN_ITEM_SERIAL_OUTTER;
	pStream->FillString(" and SerialNum<=")<<MAX_ITEM_SERIAL_OUTTER;

	pStream->FillString(" ) as TT where ss is not null");


	Beton::QueryResult* pResutl = m_DB.Query(pStream);

	m_DB.ReturnStream(pStream);
	if (P_VALID(pResutl) && pResutl->GetRowCount() > 0)
	{
		INT64 n64New = (*pResutl)[0].GetLong() - 1;
		if (-1 == n64New)
		{
			n64SerialNum = MAX_ITEM_SERIAL_OUTTER;
		}
		else
		{
			n64SerialNum = n64New <= MAX_ITEM_SERIAL_OUTTER ? n64New : MAX_ITEM_SERIAL_OUTTER;
		}
				
		return TRUE;
	}
	else
	{
		n64SerialNum = GT_INVALID;
		return FALSE;
	}
	
}

BOOL ServerDB::DBInsertBaiBaoLog(DWORD dwAccountID, DWORD dwRoleID, INT16 n16Type, DWORD dwTypeID, DWORD dwTime )
{
	MyStream* pStream = m_DB.GetStream();

	// 获取连接
	Beton::Connection* pCon = m_DB.GetFreeConnection();

	pStream->SetInsert("log_baibao");
	pStream->FillString("AccountID=") << dwAccountID;
	pStream->FillString(",RoleID=") << dwRoleID;

	// LoongDB设置时间
	CHAR szTime[20];
	if(DwordTime2DataTime(szTime, sizeof(szTime), dwTime))
	{
		pStream->FillString(",LogTime='").FillString(szTime);
		pStream->FillString("'");
	}
	else
	{
		ASSERT(0);
	}

	pStream->FillString(",n16Type=") << n16Type;
	pStream->FillString(",TypeID=") << dwTypeID;
	pStream->FillString(",LeaveWords='Create Item by GMTool'");
	
	// 释放连接
	m_DB.ReturnConnection(pCon);

	m_DB.ReturnStream(pStream);

	return FALSE;
}

BOOL ServerDB::DBWaitCreateEquipSpecInBaiBao( INT64 n64Serial, DWORD dwItemTypeID, BYTE byQuality, Connection* pCon )
{
	BOOL bRtv = FALSE;

	// 获取一个Stream
	MyStream* pStream = m_DB.GetStream();
	if( !P_VALID(pStream) ) return FALSE;

	// 获取一条连接
//	Connection* pCon = m_DB.GetFreeConnection();

	// 设置sql语句
	pStream->SetInsert("equip_baibao")	;

	pStream->FillString(" SerialNum=")<<n64Serial;
	pStream->FillString(",Quality=")<<(BYTE)byQuality;

	pStream->FillString(",WuHun=")	<<(BYTE)GT_INVALID;
	pStream->FillString(",MinDmg=")	<<(BYTE)GT_INVALID;
	pStream->FillString(",MaxDmg=")	<<(BYTE)GT_INVALID;
	pStream->FillString(",Armor=")	<<(BYTE)GT_INVALID;
	pStream->FillString(",PotVal=")	<<(BYTE)GT_INVALID;

	// 进行操作
	bRtv = m_DB.WaitExecute(pStream, pCon);

	// 归还连接
	//m_DB.ReturnConnection(pCon);

	// 释放Stream
	m_DB.ReturnStream(pStream);

	return bRtv;
}

BOOL ServerDB::DBDeleteBySerial( INT64 n64ItemID, LPCSTR szTable, BOOL bRememberDel)
{
	MyStream* pStream = m_DB.GetStream();
	BOOL bRtv = TRUE;

	// 若删除的是装备，则需要向item_del和equip_del表里添加记录
	if(bRememberDel)
	{
		if( strcmp(szTable, "item") == 0 || strcmp(szTable, "item_baibao") == 0 )
		{
			pStream->SetCopyItem("item_del", "*", szTable);
			pStream->SetWhere();
			pStream->FillString("SerialNum=") << n64ItemID;
		}
		else if( strcmp(szTable, "equip") == 0 || strcmp(szTable, "equip_baibao") == 0 )
		{
			pStream->SetCopyItem("equip_del", "*", szTable);
			pStream->SetWhere();
			pStream->FillString("SerialNum=") << n64ItemID;
		}

		bRtv = m_DB.Execute(pStream);
	}

	if(!bRtv) return FALSE;

	pStream->SetDelete(szTable);
	pStream->SetWhere();
	pStream->FillString("SerialNum=")<<n64ItemID;
	bRtv = m_DB.Execute(pStream);

	m_DB.ReturnStream(pStream);

	return bRtv;
}

BOOL ServerDB::DBQueryTypeIDBySerial(IN INT64 n64ItemID, OUT DWORD &dwTypeID, IN LPCSTR pTable)
{
	MyStream* pStream = m_DB.GetStream();

	pStream->SetSelect(pTable, "TypeID");

	pStream->SetWhere();
	pStream->FillString("SerialNum=")<<n64ItemID;

	Beton::QueryResult* pResult = m_DB.Query(pStream);

	BOOL bRtv = TRUE;
	if( !P_VALID(pResult) || pResult->GetRowCount() <= 0)
	{
		bRtv = FALSE;
	}
	else
	{
		dwTypeID = (*pResult)[0].GetDword();
	}

	m_DB.FreeQueryResult(pResult);
	m_DB.ReturnStream(pStream);

	return bRtv;
}