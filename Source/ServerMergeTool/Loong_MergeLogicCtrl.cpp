#include "StdAfx.h"
#include "wx/wx.h"
#include "Loong_MergeLogicCtrl.h"
#include "servermergetoolapp.h"
#include "Loong_OutputCtrl.h"
#include "../WorldDefine/time.h"
#include "../WorldDefine/base_define.h"

//降序
BOOL Compare_Loong_GuildLess(tagTWrap_Loong_Guild* pFir,tagTWrap_Loong_Guild* pSec)
{
	if( pFir->stOthers.nReputation<pSec->stOthers.nReputation)
		return false;
	else if(pFir->stOthers.nReputation>pSec->stOthers.nReputation)
		return true;

	return false;
}
//排序
BOOL Compare_Loong_RoleDataAcs(tagTWrap_Loong_RoleData* pFir,tagTWrap_Loong_RoleData* pSec)
{
	//升序
	if( pFir->dwAccountID<pSec->dwAccountID)
		return true;
	else if(pFir->dwAccountID>pSec->dwAccountID)
		return false;

	//降序
	if(pFir->stOthers.RoleDataSave.nLevel>pSec->stOthers.RoleDataSave.nLevel)
		return true;
	else if(pFir->stOthers.RoleDataSave.nLevel<pSec->stOthers.RoleDataSave.nLevel)
		return false;

	//降序
	if(pFir->stOthers.RoleDataSave.nCurExp>pSec->stOthers.RoleDataSave.nCurExp)
		return true;
	else if(pFir->stOthers.RoleDataSave.nCurExp<pSec->stOthers.RoleDataSave.nCurExp)
		return false;

	return false;
}
//排序
BOOL Compare_Loong_ItemAcs(tagTWrap_Loong_Item* pFir,tagTWrap_Loong_Item* pSec)
{
	//升序
	if( pFir->stOthers.dwAccountID<pSec->stOthers.dwAccountID)
		return true;
	else if(pFir->stOthers.dwAccountID>pSec->stOthers.dwAccountID)
		return false;

	//升序 为了确定保存源1的道具
	if( pFir->stOthers.dwOwnerID<pSec->stOthers.dwOwnerID)
		return true;
	else if(pFir->stOthers.dwOwnerID>pSec->stOthers.dwOwnerID)
		return false;

	//按容器类型分一下 升序
	if(pFir->stOthers.eConType<pSec->stOthers.eConType)
		return true;
	else if(pFir->stOthers.eConType>pSec->stOthers.eConType)
		return false;

	//按位置信息排一下
	if( pFir->stOthers.n16Index<pSec->stOthers.n16Index)
		return true;
	else if(pFir->stOthers.n16Index>pSec->stOthers.n16Index)
		return false;


	return false;
}

CLoongMergeLogicCtrl::CLoongMergeLogicCtrl()
:m_eMergeType(EM_MERGE_NULL),m_strSrc2WorldName(_T("")),m_n64Src1ItemMaxSerialInternal(MIN_ITEM_SERIAL_INTERNAL+1)
,m_n64Src1ItemMinSerialOutter(MIN_ITEM_SERIAL_OUTTER),m_n64Src1ItemMinSerialBill(MAX_ITEM_SERIAL_BILL),m_dwSrc1VipStallMaxNum(0)
,m_dwSrc1RoleMaxNum(0),m_dwSrc1LoginMaxNum(0),m_dwSrc1WorldNameCrc(0),m_dwSrc1PetMaxNum(0)
{
	//考虑一些划拨物品和百宝袋物品
	m_vecOldSrc1AccountID.push_back(0xffffffff);
	m_mpOldSrc1VsSrc2.insert(make_pair(0xffffffff,0xffffffff));

	m_n64ItemCurSerialInternal = MIN_ITEM_SERIAL_INTERNAL;
	m_n64ItemCurSerialOutter = MAX_ITEM_SERIAL_OUTTER;
	m_n64ItemCurSerialBill = MAX_ITEM_SERIAL_BILL;

}

CLoongMergeLogicCtrl::~CLoongMergeLogicCtrl(void)
{
}
//干掉所有中间数据
VOID CLoongMergeLogicCtrl::ClearMemory()
{
	m_n64Src1ItemMaxSerialInternal = MIN_ITEM_SERIAL_INTERNAL+1;
	m_n64Src1ItemMinSerialOutter = MIN_ITEM_SERIAL_OUTTER;
	m_n64Src1ItemMinSerialBill = MAX_ITEM_SERIAL_BILL;

	m_n64ItemCurSerialInternal = MIN_ITEM_SERIAL_INTERNAL;
	m_n64ItemCurSerialOutter = MAX_ITEM_SERIAL_OUTTER;
	m_n64ItemCurSerialBill = MAX_ITEM_SERIAL_BILL;

	m_dwSrc1LoginMaxNum = 0;
	m_dwSrc1RoleMaxNum = 0;
	m_dwSrc1PetMaxNum = 0;
	m_dwSrc1VipStallMaxNum = 0;

	m_dwSrc1WorldNameCrc = 0;
	//m_mpOldItemNumvsNewItemNum.clear();
	m_mpOldGuildIDvsNewGuildID.clear();
	m_mpAccountIDvsWareSize.clear();
	m_mpRoleIDvsRemoveType.clear();
	m_vecOverItem.clear();
	m_setOldGuildID.clear();
	m_mpAccountIDvsRoleIDforCB.clear();
	m_vecOldSrc1AccountID.clear();
	m_mpOldSrc1VsSrc2.clear();
	m_vecOldSrc2OnlyAccountID.clear();
	m_vecRoleCompensation.clear();
	m_vecOverItem.clear();

}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//更新roleid 供源2使用
VOID CLoongMergeLogicCtrl::Update_Loong_RoleID(DWORD &dwRoleID)
{
	if(IS_PLAYER(dwRoleID))
		dwRoleID += m_dwSrc1RoleMaxNum;
	else if(IS_PET(dwRoleID))
		dwRoleID += (m_dwSrc1PetMaxNum - MIN_PET_ID);

}
//转换成小写后的crc32
DWORD CLoongMergeLogicCtrl::LowerCrc32(LPCTSTR str,DWORD strLen)
{
	TCHAR *szTemp = new TCHAR[strLen];

	_tcsncpy(szTemp,str,strLen);

	_tcslwr(szTemp);

	DWORD dwCrc = IUTIL->Crc32(szTemp);

	SAFE_DEL_ARRAY(szTemp);

	return dwCrc;
}

//更新item 64位编号
VOID CLoongMergeLogicCtrl::Update_Loong_Item64SerialNum(INT64 &n64Serial)
{
	if(n64Serial != GT_INVALID)
	{
		if(n64Serial>MIN_ITEM_SERIAL_INTERNAL)
		{	
			n64Serial += (m_n64Src1ItemMaxSerialInternal - MIN_ITEM_SERIAL_INTERNAL);

			if(n64Serial > m_n64ItemCurSerialInternal)
			{
				m_n64ItemCurSerialInternal = n64Serial;
			}
		}
		else if(n64Serial>=MIN_ITEM_SERIAL_OUTTER && n64Serial<=MAX_ITEM_SERIAL_OUTTER)
		{
			n64Serial -= (MAX_ITEM_SERIAL_OUTTER - m_n64Src1ItemMinSerialOutter + 1);

			if(n64Serial < m_n64ItemCurSerialOutter)
			{
				m_n64ItemCurSerialOutter = n64Serial;
			}
		}
		else if(n64Serial<=MAX_ITEM_SERIAL_BILL)
		{
			n64Serial -= (MAX_ITEM_SERIAL_BILL - m_n64Src1ItemMinSerialBill);

			if(n64Serial < m_n64ItemCurSerialBill)
			{
				m_n64ItemCurSerialBill = n64Serial;
			}
		}
		else
		{
			ASSERT(0);
		}
	}

}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//login account表
BOOL CLoongMergeLogicCtrl::Loong_LoginMerge(std::vector <tagTWrap_Longin_Account*> &vecSrc1,
							  std::vector <tagTWrap_Longin_Account*> &vecSrc2,
							  std::vector <tagTWrap_Longin_Account*> &vecTar)
{
	//跨大区 src1的accountid不变 src2的从新排 重名的不追加
	std::vector <tagTWrap_Longin_Account*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Longin_Account*>::iterator itSrc2 = vecSrc2.begin();
	std::map<tagAccountIDKey,DWORD>::iterator itSrcFind;
	std::map<DWORD ,DWORD>::iterator itAccountFind;
	std::set<string>::iterator	itAccountName;
	std::set<string> setAccountName; //处理重名	

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());

	//源1
	for(;itSrc1!= vecSrc1.end();++itSrc1)
	{
		setAccountName.insert((*itSrc1)->szName);
		vecTar.push_back(*itSrc1);
	}

	//如果不是是跨大区
	if(m_eMergeType != EM_MERGE_CROSS_SECTION)
	{
		return TRUE;
	}
	//找一个源1的世界名crc 
	//用
	for(int iLoop = 0;iLoop<vecTar.size();iLoop++)
	{
		//利用源1的背包表 看该account是否属于源1世界
		itAccountFind = m_mpAccountIDvsWareSize.find(vecTar[iLoop]->stOthers.dwAccountID);
		if(itAccountFind != m_mpAccountIDvsWareSize.end())
		{
			//如果找到了 把crc赋上
			m_dwSrc1WorldNameCrc = vecTar[iLoop]->stOthers.dwWorldNameCrc;
			break;
		}
	}
	//源2
	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		//在源2里找
		itSrcFind = m_mpOldAccountVsNewAccount.find(tagAccountIDKey((*itSrc2)->stOthers.dwAccountID,EM_DB_SRC_TWO));
		//如果找不到
		if(itSrcFind == m_mpOldAccountVsNewAccount.end())
		{
			continue;
		}
		//如果能找到 看其id是否在src1范围内 
		if( (itSrcFind->second) <=  m_dwSrc1LoginMaxNum )
		{
			continue;
		}
		//如果能找到重名的
		itAccountName = setAccountName.find((*itSrc2)->szName);
		if(itAccountName != setAccountName.end())
		{
			continue;
		}
		(*itSrc2)->stOthers.dwWorldNameCrc = m_dwSrc1WorldNameCrc;
		(*itSrc2)->stOthers.dwAccountID = itSrcFind->second;
		vecTar.push_back(*itSrc2);
	}
	return TRUE;
}
//login worldforbid表
BOOL CLoongMergeLogicCtrl::Loong_LoginMerge(std::vector <tagTWrap_Longin_WorldForbid*> &vecSrc1,
											std::vector <tagTWrap_Longin_WorldForbid*> &vecSrc2,
											std::vector <tagTWrap_Longin_WorldForbid*> &vecTar)
{
	std::vector <tagTWrap_Longin_WorldForbid*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Longin_WorldForbid*>::iterator itSrc2 = vecSrc2.begin();
	std::map<tagAccountIDKey,DWORD>::iterator itSrcFind;
	//源1的不变 源2的根据源2世界名 算出crc 
	DWORD dwSrc2WorldCrc = IUTIL->Crc32(m_strSrc2WorldName.c_str());

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());

	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());

	if(m_eMergeType != EM_MERGE_CROSS_SECTION)
	{
		return TRUE;
	}

	//如果跨大区
	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		itSrcFind = m_mpOldAccountVsNewAccount.find(tagAccountIDKey((*itSrc2)->dwAccountID,EM_DB_SRC_TWO));
		//如果找到
		if(itSrcFind != m_mpOldAccountVsNewAccount.end())
		{
			//accountid
			(*itSrc2)->dwAccountID = (*itSrcFind).second;				
		}
		
		//如果找不到
		if(itSrcFind == m_mpOldAccountVsNewAccount.end())
		{
			continue;
		}	
		//如果能找到 看其id是否在src1范围内 
		if( (itSrcFind->second) <=  m_dwSrc1LoginMaxNum )
		{
			continue;
		}		
		//在确定其为源2游戏世界时 将crc改成源1的世界名crc
		if((*itSrc2)->dwWorldNameCrc == dwSrc2WorldCrc)
		{
			(*itSrc2)->dwWorldNameCrc = m_dwSrc1WorldNameCrc;
			vecTar.push_back(*itSrc2);	
		}

	}
	return true;
}
//login 黑名单
BOOL CLoongMergeLogicCtrl::Loong_LoginMerge(std::vector <tagTWrap_Longin_BlackList*> &vecSrc1,
											std::vector <tagTWrap_Longin_BlackList*> &vecSrc2,
											std::vector <tagTWrap_Longin_BlackList*> &vecTar)
{
	std::vector <tagTWrap_Longin_BlackList*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Longin_BlackList*>::iterator itSrc2 = vecSrc2.begin();
	std::set<string>::iterator	itIP;
	std::set<string> setIP; //处理重复ip	
	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());

	//源1
	for(;itSrc1!= vecSrc1.end();++itSrc1)
	{
		setIP.insert((*itSrc1)->szIP);
		vecTar.push_back(*itSrc1);
	}

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		//在源2里找
		itIP = setIP.find((*itSrc2)->szIP);
		//如果找到
		if(itIP != setIP.end())
		{
			continue;
		}
		vecTar.push_back(*itSrc2);
	}
	return true;
}
//login 反沉迷
BOOL CLoongMergeLogicCtrl::Loong_LoginMerge(std::vector <tagTWrap_Longin_FatigueTime*> &vecSrc1,
											std::vector <tagTWrap_Longin_FatigueTime*> &vecSrc2,
											std::vector <tagTWrap_Longin_FatigueTime*> &vecTar)
{
	std::vector <tagTWrap_Longin_FatigueTime*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Longin_FatigueTime*>::iterator itSrc2 = vecSrc2.begin();

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());
	std::set<DWORD> setAccountNameCrc;
	std::set<DWORD>::iterator itFind;
	//源1
	for(;itSrc1!= vecSrc1.end();++itSrc1)
	{
		setAccountNameCrc.insert((*itSrc1)->dwAccountNameCrc);
		vecTar.push_back(*itSrc1);
	}

	if(m_eMergeType != EM_MERGE_CROSS_SECTION)
	{
		return TRUE;
	}

	//如果跨大区
	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		itFind = setAccountNameCrc.find((*itSrc2)->dwAccountNameCrc);
		if(itFind != setAccountNameCrc.end())
		{
			continue;
		}

		vecTar.push_back(*itSrc2);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//对账号名进行纠错
string Loong_CheckAccountName(CHAR szAccountName[X_SHORT_NAME])
{
	ASSERT(szAccountName);

	string strTemp = szAccountName;

	//返回结果
	string strReturn = "";

	for(string::size_type iLoop = 0; iLoop != strTemp.size(); ++iLoop)
	{
		//数字
		if(isdigit(strTemp[iLoop]) != 0)
		{
			strReturn += strTemp[iLoop];
			continue;
		}
		//字母
		if(isalpha(strTemp[iLoop]) != 0)
		{
			strReturn += tolower(strTemp[iLoop]);
			continue;
		}
		if(strTemp[iLoop] == '-')
		{
			strReturn += strTemp[iLoop];
			continue;
		}
		if(strTemp[iLoop] == '+')
		{
			strReturn += strTemp[iLoop];
			continue;
		}
		if(strTemp[iLoop] == '_')
		{
			strReturn += strTemp[iLoop];
			continue;
		}
	}


	return strReturn;
}
//loong 需要合并仓库内金钱
BOOL CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_AccountCommon*> &vecSrc1,
									std::vector <tagTWrap_Loong_AccountCommon*> &vecSrc2,
									std::vector <tagTWrap_Loong_AccountCommon*> &vecTar)
{

	std::vector <tagTWrap_Loong_AccountCommon*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_AccountCommon*>::iterator itSrc2 = vecSrc2.begin();

	std::map<string,tagTWrap_Loong_AccountCommon*>::iterator itSrcFind;
	std::map<string,tagTWrap_Loong_AccountCommon*> mpAccountNameVsAccountCom;//用于处理相同的account

	//src1 login account表 id最大数
	DWORD dwSrc1LoginMaxNum = m_dwSrc1LoginMaxNum + 1;
	//存储纠错后的账号名
	string strAccountName = "";

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());
	for(;itSrc1!= vecSrc1.end();++itSrc1)
	{
		m_mpAccountIDvsWareSize[(*itSrc1)->dwAccountID] = (*itSrc1)->stOthers.n16WareSize;
		strAccountName = Loong_CheckAccountName((*itSrc1)->szAccountName);
		mpAccountNameVsAccountCom[ strAccountName ] = (*itSrc1);

		m_vecOldSrc1AccountID.push_back((*itSrc1)->dwAccountID);
		//推进去
		vecTar.push_back(*itSrc1);	
	}

	//往m_mpOldAccountVsNewAccount里存
	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		strAccountName = Loong_CheckAccountName((*itSrc2)->szAccountName);
		//用源2的name在 源1里找
		itSrcFind = mpAccountNameVsAccountCom.find(strAccountName);
		//如果找到 重名的 把仓库合并
		if(itSrcFind != mpAccountNameVsAccountCom.end())
		{
			//加一下源1内仓库的 金 银 元宝 size？bag psd 以及重置时间 
			(itSrcFind->second)->stOthers.nBaiBaoYuanBao += (*itSrc2)->stOthers.nBaiBaoYuanBao;
			(itSrcFind->second)->stOthers.n64WareSilver += (*itSrc2)->stOthers.n64WareSilver;
	
			//size取大的
			if((itSrcFind->second)->stOthers.n16WareSize < (*itSrc2)->stOthers.n16WareSize)
			{
				//修改目标数据
				(itSrcFind->second)->stOthers.n16WareSize = (*itSrc2)->stOthers.n16WareSize;
				//修改对照表
				m_mpAccountIDvsWareSize[(itSrcFind->second)->dwAccountID] = (*itSrc2)->stOthers.n16WareSize;
			}

			//有重名的就不添加了 直接continue
			m_mpOldAccountVsNewAccount[tagAccountIDKey((*itSrc2)->dwAccountID,EM_DB_SRC_TWO)] \
				= (itSrcFind->second)->dwAccountID; //设置为与src1同名的那个account id

			m_mpOldSrc1VsSrc2[(itSrcFind->second)->dwAccountID] = (*itSrc2)->dwAccountID;

			continue;
		}

		//如果没找到重名的
		//如果跨大区
		if(m_eMergeType == EM_MERGE_CROSS_SECTION)
		{
			//重新排号
			m_mpOldAccountVsNewAccount[tagAccountIDKey((*itSrc2)->dwAccountID,EM_DB_SRC_TWO)] \
				= dwSrc1LoginMaxNum;
			m_vecOldSrc2OnlyAccountID.push_back((*itSrc2)->dwAccountID);

			(*itSrc2)->dwAccountID = dwSrc1LoginMaxNum;
			//accountid与背包的对照表
			m_mpAccountIDvsWareSize[dwSrc1LoginMaxNum] = (*itSrc2)->stOthers.n16WareSize;
			++dwSrc1LoginMaxNum;
		}
		//同一个大区下的添加
		else
		{
			//m_mpOldAccountVsNewAccount[tagAccountIDKey((*itSrc2)->dwAccountID,EM_DB_SRC_TWO)] \
			//	= (*itSrc2)->dwAccountID;

			m_vecOldSrc2OnlyAccountID.push_back((*itSrc2)->dwAccountID);

			//容错
			std::map<DWORD ,DWORD>::iterator itCheck;
			itCheck = m_mpAccountIDvsWareSize.find((*itSrc2)->dwAccountID);
			if(itCheck != m_mpAccountIDvsWareSize.end())
			{
				continue;
			}
			//accountid与背包的对照表
			m_mpAccountIDvsWareSize[(*itSrc2)->dwAccountID] = (*itSrc2)->stOthers.n16WareSize;
		}

		vecTar.push_back(*itSrc2);	
	}
	
	return TRUE;
	
}
//role data
BOOL CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_RoleData*> &vecSrc1,
											std::vector <tagTWrap_Loong_RoleData*> &vecSrc2,
											std::vector <tagTWrap_Loong_RoleData*> &vecTar)
{
	if(vecSrc1.size() == 0 && vecSrc2.size() == 0)
		return TRUE;

	//重名的role name加  _世界名
	//一个Account用户多余5个role时，取等级最高的5个role~其他的role置removeflag为1
	//guildid更新
	//其他不变
	std::vector <tagTWrap_Loong_RoleData*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_RoleData*>::iterator itSrc2 = vecSrc2.begin();

	std::map<tagAccountIDKey,DWORD>::iterator itSrcFind;

	//由于目前数据库存在错误，两个不同的RoleName可能会对应同一个RoleNameCrc
	//为解决此问题，在处理重名时，同时要比对名字和crc
	std::set<tstring>::iterator itNameFind;
	std::set <tstring> setRoleName;
	std::set<DWORD> setRoleNameCrc;

	//先把role进行重名处理 都插到vecTar里

	TCHAR	szRoleName[X_SHORT_NAME] = {0};
	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());

	for(;itSrc1!= vecSrc1.end();++itSrc1)
	{
		//name检查
		_tcsncpy(szRoleName, (*itSrc1)->stOthers.RoleDataConst.szRoleName, X_SHORT_NAME);
		szRoleName[X_SHORT_NAME-1] = _T('\0');
		_tcslwr(szRoleName);
		setRoleName.insert(szRoleName);
		
		//crc检查
		setRoleNameCrc.insert((*itSrc1)->stOthers.RoleDataConst.dwRoleNameCrc);

		vecTar.push_back(*itSrc1);
	}

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		//解决名字重复问题
		_tcsncpy(szRoleName, (*itSrc2)->stOthers.RoleDataConst.szRoleName, X_SHORT_NAME);
		szRoleName[X_SHORT_NAME-1] = _T('\0');
		_tcslwr(szRoleName);

		//是否改名
		BOOL bChangeName = FALSE;

		itNameFind = setRoleName.find(szRoleName);
		//如果找到重名的 在名字后面加世界名 现在不考虑重复合服后玩家名字带多个AddName的情况
		if(itNameFind != setRoleName.end())
		{
			tstring AddName;
			AddName += _T("_");
			AddName += m_strSrc2WorldName;

			//role名字超长？
			StrCat((*itSrc2)->stOthers.RoleDataConst.szRoleName,AddName.c_str());

			//重新算crc
			(*itSrc2)->stOthers.RoleDataConst.dwRoleNameCrc = \
				LowerCrc32((*itSrc2)->stOthers.RoleDataConst.szRoleName,X_SHORT_NAME);
		
			bChangeName = TRUE;
		}
		
		//解决crc重复问题
		while(setRoleNameCrc.end() !=  setRoleNameCrc.find((*itSrc2)->stOthers.RoleDataConst.dwRoleNameCrc))
		{
			//名字后面加个下划线重新计算
			tstring AddName;
			AddName += _T("_");
			StrCat((*itSrc2)->stOthers.RoleDataConst.szRoleName,AddName.c_str());

			//重新算crc
			DWORD dwSrc2 = LowerCrc32((*itSrc2)->stOthers.RoleDataConst.szRoleName,X_SHORT_NAME);
			(*itSrc2)->stOthers.RoleDataConst.dwRoleNameCrc = dwSrc2;
			
			bChangeName = TRUE;
		}
		setRoleNameCrc.insert((*itSrc2)->stOthers.RoleDataConst.dwRoleNameCrc);

		//直接加上源1的玩家数 不删除role
		Update_Loong_RoleID((*itSrc2)->dwRoleID);

		//更新帮派id
		if((*itSrc2)->stOthers.RoleDataSave.dwGuildID != GT_INVALID)
		{
			(*itSrc2)->stOthers.RoleDataSave.dwGuildID = \
				m_mpOldGuildIDvsNewGuildID[tagGuildIDKey((*itSrc2)->stOthers.RoleDataSave.dwGuildID,EM_DB_SRC_TWO)];
		}
		//如果是跨大区的
		if(m_eMergeType == EM_MERGE_CROSS_SECTION)
		{
			itSrcFind = m_mpOldAccountVsNewAccount.find(tagAccountIDKey((*itSrc2)->dwAccountID,EM_DB_SRC_TWO));
			//如果找到
			if(itSrcFind != m_mpOldAccountVsNewAccount.end())
			{
				//accountid
				(*itSrc2)->dwAccountID = (*itSrcFind).second;				
			}
		}
		vecTar.push_back(*itSrc2);

		//记下该玩家
		if(bChangeName == TRUE)
		{
			m_vecRoleCompensation.push_back(tagRoleCompensation(ERC_CHANGE_ROLE_NAME,(*itSrc2)->dwAccountID,(*itSrc2)->dwRoleID));
		}
	}
	
	//按account id升序  level降序
	std::sort(vecTar.begin(),vecTar.end(),Compare_Loong_RoleDataAcs);
	//处理一下超过5个role的情况
	std::map<DWORD ,DWORD>::iterator itAccountFinder;
	std::vector <tagTWrap_Loong_RoleData*>::iterator itTar = vecTar.begin();
	WORD wRoleNum = 0;
	DWORD dwAccountIDPre = (*itTar)->dwAccountID; //初始化为第一个account
	ETBRoleRemoveType eRemoveType = EM_R_NULL;

	for(;itTar != vecTar.end(); ++itTar)
	{	
		//源数据自删除
		if((*itTar)->bRemoveFlag == TRUE)
		{
			eRemoveType = EM_R_ONESELF;
		}
		else
		{
			//清空
			eRemoveType = EM_R_NULL;
		}

		if((*itTar)->dwAccountID == dwAccountIDPre)
		{
			if(wRoleNum >= ROLE_MAX_LIMIT)
			{
				//表示该role为合并时自动删除
				if(eRemoveType != EM_R_ONESELF)
					eRemoveType = EM_R_MERGE;

				(*itTar)->bRemoveFlag = TRUE;
			}
			//累计账号人数
			if((*itTar)->bRemoveFlag == FALSE)
				++wRoleNum;
	
			//保存一个账号中“尚未被删除的”“等级最高的“roleid用于改绑定
			if((*itTar)->bRemoveFlag == FALSE)
			{
				itAccountFinder = m_mpAccountIDvsRoleIDforCB.find((*itTar)->dwAccountID);
				if(itAccountFinder == m_mpAccountIDvsRoleIDforCB.end())
				{
					m_mpAccountIDvsRoleIDforCB.insert(make_pair((*itTar)->dwAccountID,(*itTar)->dwRoleID));
				}
			}

		}
		else
		{	
			//当切换account时，如果第一个角色没有被删除 rolenum计数为1 并保存该role到m_mpAccountIDvsRoleIDforCB表
			if((*itTar)->bRemoveFlag == FALSE)
			{
				wRoleNum = 1;
				m_mpAccountIDvsRoleIDforCB.insert(make_pair((*itTar)->dwAccountID,(*itTar)->dwRoleID));
			}
			else
			{
				wRoleNum = 0;
			}

			dwAccountIDPre = (*itTar)->dwAccountID;
		}

		//新的roleid和其removetype的对照表		
		m_mpRoleIDvsRemoveType[(*itTar)->dwRoleID] = eRemoveType;
	}
	return TRUE;
}
//itembaibao
BOOL CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_Item*> &vecSrc1,
											std::vector <tagTWrap_Loong_Item*> &vecSrc2,
											std::vector <tagTWrap_Loong_Item*> &vecTar)
{
	return Loong_LoongMerge((std::vector <tagTWrap_Loong_Item*> &)vecSrc1,
							(std::vector <tagTWrap_Loong_Item*> &)vecSrc2,
							(std::vector <tagTWrap_Loong_Item*> &)vecTar,
							EM_TB_ITEM);
}
//itembaibao
BOOL CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_ItemBaiBao*> &vecSrc1,
											std::vector <tagTWrap_Loong_ItemBaiBao*> &vecSrc2,
											std::vector <tagTWrap_Loong_ItemBaiBao*> &vecTar)
{
	return Loong_LoongMerge((std::vector <tagTWrap_Loong_Item*> &)vecSrc1,
							(std::vector <tagTWrap_Loong_Item*> &)vecSrc2,
							(std::vector <tagTWrap_Loong_Item*> &)vecTar,
							EM_TB_ITEM_BAIBAO);
}
//itemDel
BOOL CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_ItemDel*> &vecSrc1,
											std::vector <tagTWrap_Loong_ItemDel*> &vecSrc2,
											std::vector <tagTWrap_Loong_ItemDel*> &vecTar)
{
	return Loong_LoongMerge((std::vector <tagTWrap_Loong_Item*> &)vecSrc1,
							(std::vector <tagTWrap_Loong_Item*> &)vecSrc2,
							(std::vector <tagTWrap_Loong_Item*> &)vecTar,
							EM_TB_ITEM_DEL);
}

//item
BOOL CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_Item*> &vecSrc1,
											std::vector <tagTWrap_Loong_Item*> &vecSrc2,
											std::vector <tagTWrap_Loong_Item*> &vecTar,
											ETBItemType emTBType)
{
	//item的serial num重排
	//如果跨大区合并 需要更新OwnerID(RoleID绑定物品) AccountID

	if(vecSrc1.size() == 0 && vecSrc2.size() == 0)
		return TRUE;

	//ContainerTypeID不变
	std::vector <tagTWrap_Loong_Item*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_Item*>::iterator itSrc2 = vecSrc2.begin();

	std::map<tagAccountIDKey,DWORD>::iterator itSrcFind;
	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());

	//全部重新排 建对照表
	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());

	//源2 更新ownerid和accountid
	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		Update_Loong_Item64SerialNum((*itSrc2)->stOthers.n64Serial);

		//如果是gm命令 或 玩家制作
		if((*itSrc2)->stOthers.eCreateMode == EICM_GM
			|| (*itSrc2)->stOthers.eCreateMode == EICM_Product)
			Update_Loong_RoleID((*itSrc2)->stOthers.dwCreateID);

		//
		if((*itSrc2)->stOthers.eCreateMode == EICM_Product)
			Update_Loong_RoleID((*itSrc2)->stOthers.dwCreatorID);

		//ownerid
		Update_Loong_RoleID((*itSrc2)->stOthers.dwOwnerID);

		//如果是帮派仓库物品
		if((*itSrc2)->stOthers.eConType == EICT_GuildWare)
		{
			//更新帮派id
			(*itSrc2)->stOthers.dwAccountID = \
				m_mpOldGuildIDvsNewGuildID[tagGuildIDKey((*itSrc2)->stOthers.dwAccountID,EM_DB_SRC_TWO)];
		}

		if(m_eMergeType == EM_MERGE_CROSS_SECTION)
		{

			if((*itSrc2)->stOthers.eConType != EICT_GuildWare)
			{
				itSrcFind = m_mpOldAccountVsNewAccount.find(tagAccountIDKey((*itSrc2)->stOthers.dwAccountID,EM_DB_SRC_TWO));
				//如果找到
				if(itSrcFind != m_mpOldAccountVsNewAccount.end())
				{
					//accountid
					(*itSrc2)->stOthers.dwAccountID = (*itSrcFind).second;				
				}
			}	
		}
		vecTar.push_back(*itSrc2);
	}
	//如果是item_baibao需把m_vecOverItem里的东西放到vectar中 一起写入db
	if(emTBType == EM_TB_ITEM_BAIBAO)
	{
		std::vector <tagTWrap_Loong_Item>::iterator itOver = m_vecOverItem.begin();
		for(;itOver != m_vecOverItem.end();++itOver)
			vecTar.push_back(&(*itOver));
	}
	//////////////////////////////////////////////////////////////////////////
	//处理EICT_RoleWare EICT_Baibao  仓库 百宝物品超多的情况

	//如果不是item表 无需处理物品超多问题
	if(emTBType != EM_TB_ITEM)
		return TRUE;

	//vecTar 按AccountID RoleID排序
	std::sort(vecTar.begin(),vecTar.end(),Compare_Loong_ItemAcs);
	//检查 EICT_RoleWare EICT_Baibao 类型的item 如果超过24往 item_baibao里放
	std::vector <tagTWrap_Loong_Item*>::iterator itTar = vecTar.begin();
	std::map <DWORD, ETBRoleRemoveType>::iterator itRoleIDvsRemoveType = m_mpRoleIDvsRemoveType.begin();	
	std::map <DWORD, DWORD>::iterator itAccountIDvsWareSize;

	DWORD dwWareItem = 0;
	DWORD dwBaibaoItem = 0;

	DWORD dwWarePos = 0; //位置
	DWORD dwBaibaoPos = 0;

	DWORD dwAccountPre = (*itTar)->stOthers.dwAccountID; //初始化为第一个account
	for(;itTar != vecTar.end();)
	{
		if((*itTar)->stOthers.dwAccountID == dwAccountPre)
		{
			if((*itTar)->stOthers.eConType != EICT_RoleWare
				&&(*itTar)->stOthers.eConType != EICT_Baibao)
			{
				++itTar;
				continue;
			}
			itRoleIDvsRemoveType = m_mpRoleIDvsRemoveType.find((*itTar)->stOthers.dwOwnerID);
			if(itRoleIDvsRemoveType != m_mpRoleIDvsRemoveType.end())
			{
				//如果该role是合并时被干掉
				if((*itRoleIDvsRemoveType).second == EM_R_MERGE)
				{
					//需要处理物品 目前绑定物品全部做删除处理　
					//非绑定的　如果仓库够位就放仓库　否则扔到item_baibao里
					//绑定的	尝试进行改绑定
					if((*itTar)->stOthers.byBind == EBS_Bind)
					{
						std::map <DWORD,DWORD>::iterator itFinder = m_mpAccountIDvsRoleIDforCB.find((*itTar)->stOthers.dwAccountID);
						if(itFinder != m_mpAccountIDvsRoleIDforCB.end())
						{
							(*itTar)->stOthers.dwOwnerID = itFinder->second;
						}
						//根本找不到该account中存在未删除账号
						else
						{
							//itTar = vecTar.erase(itTar);
							//continue;
						}
					}
				}
				//如果就是自删除role 还能剩下点儿未绑定的物品 也正常处理
				//绑定道具的话就直接删除
				else if((*itRoleIDvsRemoveType).second == EM_R_ONESELF)
				{
					if((*itTar)->stOthers.byBind == EBS_Bind)
					{
						//itTar = vecTar.erase(itTar);					
						//continue;
					}
				}
				//
				if((*itTar)->stOthers.eConType == EICT_RoleWare)
				{
					itAccountIDvsWareSize = m_mpAccountIDvsWareSize.find( (*itTar)->stOthers.dwAccountID);
					if(itAccountIDvsWareSize == m_mpAccountIDvsWareSize.end())
					{
						tstringstream tszErr;
						tszErr << _T("accountid:") << (*itTar)->stOthers.dwAccountID << _T("拥有仓库物品但在account_common表中找不到对应记录！");
						g_clsOutput.Output(tszErr.str().c_str());
						itTar = vecTar.erase(itTar);
						continue;
					}
					//如果已经达到最大量
					if( dwWareItem >= (*itAccountIDvsWareSize).second )
					{
						//把自己扔到m_vecOverItem里
						m_vecOverItem.push_back(*(*itTar));
						//从tar表中删除该item 由于存的是指针 速度影响不大
						itTar = vecTar.erase(itTar);					
						continue;
					}
					else
					{
						//给这个item一个位置
						(*itTar)->stOthers.n16Index = dwWarePos;
						++dwWarePos;
						++dwWareItem;
					}
				}
				if((*itTar)->stOthers.eConType == EICT_Baibao)
				{
					//如果已经达到最大量
					if(dwBaibaoItem >= SPACE_BAIBAO_BAG)
					{
						//把自己扔到m_vecOverItem里
						m_vecOverItem.push_back(*(*itTar));
						//从tar表中删除该item
						itTar = vecTar.erase(itTar);					
						continue;
					}
					//给这个item一个位置
					else
					{
						//给这个item一个位置
						(*itTar)->stOthers.n16Index = dwBaibaoPos;
						++dwBaibaoItem;
						++dwBaibaoPos;
					}
				}
			}
			//该item没有拥有者
			else
			{}
		}
		//切换了account
		else
		{	
			dwWarePos = 0; //位置
			dwBaibaoPos = 0;
			dwWareItem = 0;	
			dwBaibaoItem = 0;
			dwAccountPre = (*itTar)->stOthers.dwAccountID;

			//相关计数变量处理
			if((*itTar)->stOthers.eConType == EICT_RoleWare)
			{	
				(*itTar)->stOthers.n16Index = dwWarePos;
				dwWareItem = 1;
				++dwWarePos;
			}	
			if((*itTar)->stOthers.eConType == EICT_Baibao)
			{
				(*itTar)->stOthers.n16Index = dwBaibaoPos;
				dwBaibaoItem = 1;
				++dwBaibaoPos;
			}
		}
		//下一个物品
		++itTar;
	}
	return TRUE;
}
//buff
BOOL CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_Buff*> &vecSrc1,
											std::vector <tagTWrap_Loong_Buff*> &vecSrc2,
											std::vector <tagTWrap_Loong_Buff*> &vecTar)
{
	//roleid srcunitid ItemSerialID
	std::vector <tagTWrap_Loong_Buff*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_Buff*>::iterator itSrc2 = vecSrc2.begin();

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());
	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		Update_Loong_Item64SerialNum((*itSrc2)->stOthers.n64Serial);
		Update_Loong_RoleID((*itSrc2)->dwRoleID);
		Update_Loong_RoleID((*itSrc2)->stOthers.dwSrcUnitID);

		vecTar.push_back(*itSrc2);
	}
	return true;
}
//帮派
BOOL CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_Guild*> &vecSrc1,
											std::vector <tagTWrap_Loong_Guild*> &vecSrc2,
											std::vector <tagTWrap_Loong_Guild*> &vecTar)
{
	//重排guild id 帮派名称小写的32位crc
	//如果帮派重名 加世界名当后缀处理
	//更新创始人nameid 现在就是roleid
	//其他不变
	std::vector <tagTWrap_Loong_Guild*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_Guild*>::iterator itSrc2 = vecSrc2.begin();

	//
	std::set <tstring> setGuildName;
	std::set <tstring>::iterator itFind;

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());

	for(;itSrc1!= vecSrc1.end();++itSrc1)
	{
		//插进去
		setGuildName.insert((*itSrc1)->stOthers.szName);
		m_setOldGuildID.insert((*itSrc1)->stOthers.dwID);

		vecTar.push_back(*itSrc1);
	}
	DWORD dwNewGuildID;
	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		itFind = setGuildName.find((*itSrc2)->stOthers.szName);

		m_setOldGuildID.insert((*itSrc2)->stOthers.dwID);
		//原来的id
		dwNewGuildID = (*itSrc2)->stOthers.dwID;
		//如果找到重名的 在名字后面加世界名
		if(itFind != setGuildName.end())
		{
			tstring AddName;
			AddName += _T("_");
			AddName += m_strSrc2WorldName;

			//role名字超长？
			StrCat((*itSrc2)->stOthers.szName,AddName.c_str());

			//重新算crc
			TCHAR szName[MAX_GUILD_NAME_LEN] = {'\0'};
			_tcsncpy(szName, (*itSrc2)->stOthers.szName, MAX_GUILD_NAME_LEN);
			szName[MAX_GUILD_NAME_LEN-1] = _T('\0');
			_tcslwr(szName);
			dwNewGuildID = IUTIL->Crc32(szName);

		}
		//重新排源2id 目前不删除role
		Update_Loong_RoleID((*itSrc2)->stOthers.dwFounderNameID);
		Update_Loong_RoleID((*itSrc2)->stOthers.dwLeaderRoleID);
		//新旧id对照表
		m_mpOldGuildIDvsNewGuildID[tagGuildIDKey((*itSrc2)->stOthers.dwID,EM_DB_SRC_TWO)] = dwNewGuildID;
		(*itSrc2)->stOthers.dwID = dwNewGuildID;


		vecTar.push_back(*itSrc2);
	}
	//帮派排名
	std::sort(vecTar.begin(),vecTar.end(),Compare_Loong_GuildLess);

	std::vector<tagTWrap_Loong_Guild*>::iterator itTar = vecTar.begin();

	DWORD dwRank = 1;
	for(;itTar != vecTar.end();++itTar)
	{
		//把之前已经排名的 全部按威望重排
		if((*itTar)->stOthers.n16Rank != 0)
		{
			(*itTar)->stOthers.n16Rank = dwRank;
			++dwRank;
		}
	}


	return TRUE;
}
//BlackList
BOOL CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_BlackList*> &vecSrc1,
											std::vector <tagTWrap_Loong_BlackList*> &vecSrc2,
											std::vector <tagTWrap_Loong_BlackList*> &vecTar)
{
	//roleid blackid
	std::vector <tagTWrap_Loong_BlackList*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_BlackList*>::iterator itSrc2 = vecSrc2.begin();

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());
	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());
	
	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		Update_Loong_RoleID((*itSrc2)->dwRoleID);
		Update_Loong_RoleID((*itSrc2)->dwBlackID);

		vecTar.push_back(*itSrc2);
	}
	return true;
}
//enemy
BOOL CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_Enemy*> &vecSrc1,
											std::vector <tagTWrap_Loong_Enemy*> &vecSrc2,
											std::vector <tagTWrap_Loong_Enemy*> &vecTar)
{
	//roleid blackid
	std::vector <tagTWrap_Loong_Enemy*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_Enemy*>::iterator itSrc2 = vecSrc2.begin();

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());
	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());
	//for(;itSrc1!= vecSrc1.end();++itSrc1)
	//{
	//	vecTar.push_back(*itSrc1);
	//}
	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		Update_Loong_RoleID((*itSrc2)->dwRoleID);
		Update_Loong_RoleID((*itSrc2)->dwEnemyID);

		vecTar.push_back(*itSrc2);
	}
	return true;
}
//equip
BOOL CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_Equip*> &vecSrc1,
											std::vector <tagTWrap_Loong_Equip*> &vecSrc2,
											std::vector <tagTWrap_Loong_Equip*> &vecTar)
{
	//更新序号
	std::vector <tagTWrap_Loong_Equip*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_Equip*>::iterator itSrc2 = vecSrc2.begin();

	//std::map<tagItemNumKey ,INT64>::iterator itFind;

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());
	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		Update_Loong_Item64SerialNum((*itSrc2)->n64SerialNum);

		vecTar.push_back(*itSrc2);
	}
	return true;
}
//EquipDel
BOOL CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_EquipDel*> &vecSrc1,
											std::vector <tagTWrap_Loong_EquipDel*> &vecSrc2,
											std::vector <tagTWrap_Loong_EquipDel*> &vecTar)
{
	return Loong_LoongMerge((std::vector <tagTWrap_Loong_Equip*> &)vecSrc1,
							(std::vector <tagTWrap_Loong_Equip*> &)vecSrc2,
							(std::vector <tagTWrap_Loong_Equip*> &)vecTar);
}
//EquipBaiBao
BOOL CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_EquipBaiBao*> &vecSrc1,
											std::vector <tagTWrap_Loong_EquipBaiBao*> &vecSrc2,
											std::vector <tagTWrap_Loong_EquipBaiBao*> &vecTar)
{
	return Loong_LoongMerge((std::vector <tagTWrap_Loong_Equip*> &)vecSrc1,
							(std::vector <tagTWrap_Loong_Equip*> &)vecSrc2,
							(std::vector <tagTWrap_Loong_Equip*> &)vecTar);
}
//氏族
BOOL CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_ClanData*> &vecSrc1,
											std::vector <tagTWrap_Loong_ClanData*> &vecSrc2,
											std::vector <tagTWrap_Loong_ClanData*> &vecTar)
{
	//更新序号
	std::vector <tagTWrap_Loong_ClanData*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_ClanData*>::iterator itSrc2 = vecSrc2.begin();

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());
	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		Update_Loong_RoleID((*itSrc2)->dwRoleID);
		vecTar.push_back(*itSrc2);
	}
	return true;
}

//珍宝激活
BOOL CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_ActClanTreasure*> &vecSrc1,
											std::vector <tagTWrap_Loong_ActClanTreasure*> &vecSrc2,
											std::vector <tagTWrap_Loong_ActClanTreasure*> &vecTar)
{

	//更新激活人id
	std::vector <tagTWrap_Loong_ActClanTreasure*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_ActClanTreasure*>::iterator itSrc2 = vecSrc2.begin();

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());

	std::set<tagActClanTreasureKey> setPrimaryKey;
	std::set<tagActClanTreasureKey>::iterator itFind;

	for(;itSrc1!= vecSrc1.end();++itSrc1)
	{
		//联合主键
		setPrimaryKey.insert(tagActClanTreasureKey((*itSrc1)->byClanType,(*itSrc1)->u16TreasureID));

		vecTar.push_back(*itSrc1);
	}

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		//要保证这个珍宝在源1中没有被激活过
		itFind = setPrimaryKey.find(tagActClanTreasureKey((*itSrc1)->byClanType,(*itSrc1)->u16TreasureID));
		if(itFind != setPrimaryKey.end())
		{
			Update_Loong_RoleID((*itSrc2)->dwRoleID);
			Update_Loong_RoleID((*itSrc2)->dwActNameID);
			vecTar.push_back(*itSrc2);
		}
	}
	return true;
}
BOOL CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_FameHallEnterSnap*> &vecSrc1,
											 std::vector <tagTWrap_Loong_FameHallEnterSnap*> &vecSrc2,
											 std::vector <tagTWrap_Loong_FameHallEnterSnap*> &vecTar)
{
	//更新人id
	std::vector <tagTWrap_Loong_FameHallEnterSnap*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_FameHallEnterSnap*>::iterator itSrc2 = vecSrc2.begin();

	std::set<DWORD> setTime;
	std::set<DWORD>::iterator itFind;

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());

	for(;itSrc1!= vecSrc1.end();++itSrc1)
	{
		setTime.insert((*itSrc1)->dwEnterTime);
		vecTar.push_back(*itSrc1);
	}
	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		Update_Loong_RoleID((*itSrc2)->dwRoleID);
		Update_Loong_RoleID((*itSrc2)->dwEnterNameID);

		//处理相同时间的冲突
		while (setTime.find((*itSrc2)->dwEnterTime) != setTime.end())
		{
			(*itSrc2)->dwEnterTime = IncreaseTime((*itSrc2)->dwEnterTime,1);
		}
		setTime.insert((*itSrc2)->dwEnterTime);

		vecTar.push_back(*itSrc2);
	}
	return true;
}
BOOL CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_RepRstTime*> &vecSrc1,
											 std::vector <tagTWrap_Loong_RepRstTime*> &vecSrc2,
											 std::vector <tagTWrap_Loong_RepRstTime*> &vecTar)
{
	//取离当前时间最近的数据
	std::vector <tagTWrap_Loong_RepRstTime*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_RepRstTime*>::iterator itSrc2 = vecSrc2.begin();

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());

	//比时间
	for(;itSrc1!= vecSrc1.end();++itSrc1)
	{
		for(;itSrc2!= vecSrc2.end();++itSrc2)
		{
			
			DWORD dwCur = GetCurrentDWORDTime();
			if((*itSrc2)->byClanType == (*itSrc1)->byClanType)
			{
				if(   CalcTimeDiff(dwCur,(*itSrc1)->dwRepRstTimeStamp) 
					> CalcTimeDiff(dwCur,(*itSrc2)->dwRepRstTimeStamp) )
				{
						vecTar.push_back(*itSrc2);
				}
				else
				{
						vecTar.push_back(*itSrc1);
				}
			}
			

		}
	}

	return true;
}
//friend
BOOL  CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_Friend*> &vecSrc1,
											std::vector <tagTWrap_Loong_Friend*> &vecSrc2,
											std::vector <tagTWrap_Loong_Friend*> &vecTar)
{

	//更新序号
	std::vector <tagTWrap_Loong_Friend*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_Friend*>::iterator itSrc2 = vecSrc2.begin();

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());
	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		Update_Loong_RoleID((*itSrc2)->dwRoleID);
		Update_Loong_RoleID((*itSrc2)->stOthers.dwFriendID);
		vecTar.push_back(*itSrc2);
	}
	return true;
}
//friendship
BOOL  CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_FriendshipSave*> &vecSrc1,
											std::vector <tagTWrap_Loong_FriendshipSave*> &vecSrc2,
											std::vector <tagTWrap_Loong_FriendshipSave*> &vecTar)
{

	//更新序号
	std::vector <tagTWrap_Loong_FriendshipSave*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_FriendshipSave*>::iterator itSrc2 = vecSrc2.begin();

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());
	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		Update_Loong_RoleID((*itSrc2)->dwRoleID);
		Update_Loong_RoleID((*itSrc2)->stOthers.dwFriendID);
		vecTar.push_back(*itSrc2);
	}
	return true;
}
//GroupPurchase
BOOL  CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_GroupPurchase*> &vecSrc1,
											std::vector <tagTWrap_Loong_GroupPurchase*> &vecSrc2,
											std::vector <tagTWrap_Loong_GroupPurchase*> &vecTar)
{

	//更新序号
	std::vector <tagTWrap_Loong_GroupPurchase*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_GroupPurchase*>::iterator itSrc2 = vecSrc2.begin();

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());
	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		(*itSrc2)->stOthers.dwGuildID \
			= m_mpOldGuildIDvsNewGuildID[tagGuildIDKey((*itSrc2)->stOthers.dwGuildID,EM_DB_SRC_TWO)];
		Update_Loong_RoleID((*itSrc2)->stOthers.dwRoleID);
		//更新列表
		if((*itSrc2)->stOthers.nParticipatorNum > 0)
		{
			DWORD dwTemp;
			for(int iLoop = 0; iLoop<(*itSrc2)->stOthers.nParticipatorNum;iLoop++)
			{
				dwTemp = (DWORD)((*itSrc2)->pcBlob + sizeof(DWORD)*iLoop);
				Update_Loong_RoleID(dwTemp);
				memcpy((*itSrc2)->pcBlob + sizeof(DWORD)*iLoop,&dwTemp,sizeof(DWORD));
			}
		}	
		vecTar.push_back(*itSrc2);
	}
	return true;
}
//GuildMem
BOOL  CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_GuildMem*> &vecSrc1,
											 std::vector <tagTWrap_Loong_GuildMem*> &vecSrc2,
											 std::vector <tagTWrap_Loong_GuildMem*> &vecTar)
{

	//更新roleid guildid
	std::vector <tagTWrap_Loong_GuildMem*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_GuildMem*>::iterator itSrc2 = vecSrc2.begin();

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());
	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{

		Update_Loong_RoleID((*itSrc2)->stOthers.sGuildMember.dwRoleID);
		(*itSrc2)->stOthers.dwGuildID = \
				m_mpOldGuildIDvsNewGuildID[tagGuildIDKey((*itSrc2)->stOthers.dwGuildID,EM_DB_SRC_TWO)];

		vecTar.push_back(*itSrc2);
	}
	return true;
}
//item_cdtime
BOOL  CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_ItemCDTime*> &vecSrc1,
											 std::vector <tagTWrap_Loong_ItemCDTime*> &vecSrc2,
											 std::vector <tagTWrap_Loong_ItemCDTime*> &vecTar)
{
	std::vector <tagTWrap_Loong_ItemCDTime*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_ItemCDTime*>::iterator itSrc2 = vecSrc2.begin();

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());
	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		Update_Loong_RoleID((*itSrc2)->dwRoleID);
		vecTar.push_back(*itSrc2);
	}
	return true;
}
//item_needlog
BOOL  CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_ItemNeedLog*> &vecSrc1,
											 std::vector <tagTWrap_Loong_ItemNeedLog*> &vecSrc2,
											 std::vector <tagTWrap_Loong_ItemNeedLog*> &vecTar)
{

	//需要去重复
	std::vector <tagTWrap_Loong_ItemNeedLog*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_ItemNeedLog*>::iterator itSrc2 = vecSrc2.begin();

	std::set<DWORD> setTypeID;
	std::set<DWORD>::iterator itFind;

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());

	for(;itSrc1!= vecSrc1.end();++itSrc1)
	{
		setTypeID.insert((*itSrc1)->dwTypeID);
		vecTar.push_back(*itSrc1);
	}
	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		itFind = setTypeID.find((*itSrc2)->dwTypeID);
		//如果没找到
		if(itFind == setTypeID.end())
		{
			vecTar.push_back(*itSrc2);
		}
	}
	return true;
}
//log_baibao
BOOL  CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_LogBaiBao*> &vecSrc1,
											 std::vector <tagTWrap_Loong_LogBaiBao*> &vecSrc2,
											 std::vector <tagTWrap_Loong_LogBaiBao*> &vecTar)
{
	//需要去重复
	std::vector <tagTWrap_Loong_LogBaiBao*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_LogBaiBao*>::iterator itSrc2 = vecSrc2.begin();
	std::map<tagAccountIDKey,DWORD>::iterator itSrcFind;

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());
	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		Update_Loong_RoleID((*itSrc2)->stOthers.dwRoleID);
		//如果跨大区
		if(m_eMergeType == EM_MERGE_CROSS_SECTION)
		{
			itSrcFind = m_mpOldAccountVsNewAccount.find(tagAccountIDKey((*itSrc2)->dwAccountID,EM_DB_SRC_TWO));
			//如果找到
			if(itSrcFind != m_mpOldAccountVsNewAccount.end())
			{
				//accountid
				(*itSrc2)->dwAccountID = (*itSrcFind).second;				
			}
		}

		vecTar.push_back(*itSrc2);	
	}
	return true;
}
//RoleDel
BOOL  CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_RoleDel*> &vecSrc1,
											 std::vector <tagTWrap_Loong_RoleDel*> &vecSrc2,
											 std::vector <tagTWrap_Loong_RoleDel*> &vecTar)
{
	//需要去重复
	std::vector <tagTWrap_Loong_RoleDel*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_RoleDel*>::iterator itSrc2 = vecSrc2.begin();
	std::map<tagAccountIDKey,DWORD>::iterator itSrcFind;

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());
	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		Update_Loong_RoleID((*itSrc2)->dwRoleID);
		//如果跨大区
		if(m_eMergeType == EM_MERGE_CROSS_SECTION)
		{
			itSrcFind = m_mpOldAccountVsNewAccount.find(tagAccountIDKey((*itSrc2)->dwAccountID,EM_DB_SRC_TWO));
			//如果找到
			if(itSrcFind != m_mpOldAccountVsNewAccount.end())
			{
				//accountid
				(*itSrc2)->dwAccountID = (*itSrcFind).second;				
			}
		}

		vecTar.push_back(*itSrc2);	
	}
	return true;
}
//pet_data
BOOL  CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_PetData*> &vecSrc1,
											 std::vector <tagTWrap_Loong_PetData*> &vecSrc2,
											 std::vector <tagTWrap_Loong_PetData*> &vecTar)
{
	//需要去重复
	std::vector <tagTWrap_Loong_PetData*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_PetData*>::iterator itSrc2 = vecSrc2.begin();

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());

	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		(*itSrc2)->dwPetID += (m_dwSrc1PetMaxNum - MIN_PET_ID);
		Update_Loong_RoleID((*itSrc2)->dwMasterID);
		vecTar.push_back(*itSrc2);	
	}
	return true;
}
//pet skill
BOOL  CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_PetSkill*> &vecSrc1,
											 std::vector <tagTWrap_Loong_PetSkill*> &vecSrc2,
											 std::vector <tagTWrap_Loong_PetSkill*> &vecTar)
{
	std::vector <tagTWrap_Loong_PetSkill*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_PetSkill*>::iterator itSrc2 = vecSrc2.begin();

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());
	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());
	
	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		(*itSrc2)->dwPetID += (m_dwSrc1PetMaxNum - MIN_PET_ID);
		vecTar.push_back(*itSrc2);	
	}
	return true;
}
//skill
BOOL  CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_Skill*> &vecSrc1,
											 std::vector <tagTWrap_Loong_Skill*> &vecSrc2,
											 std::vector <tagTWrap_Loong_Skill*> &vecTar)
{
	//需要去重复
	std::vector <tagTWrap_Loong_Skill*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_Skill*>::iterator itSrc2 = vecSrc2.begin();

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());
	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		Update_Loong_RoleID((*itSrc2)->dwRoleID);
		vecTar.push_back(*itSrc2);	
	}
	return true;
}
//task
BOOL  CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_Task*> &vecSrc1,
											 std::vector <tagTWrap_Loong_Task*> &vecSrc2,
											 std::vector <tagTWrap_Loong_Task*> &vecTar)
{
	//需要去重复
	std::vector <tagTWrap_Loong_Task*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_Task*>::iterator itSrc2 = vecSrc2.begin();

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());
	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		Update_Loong_RoleID((*itSrc2)->dwRoleID);
		vecTar.push_back(*itSrc2);	
	}
	return true;
}
//taskdone
BOOL  CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_TaskDone*> &vecSrc1,
											 std::vector <tagTWrap_Loong_TaskDone*> &vecSrc2,
											 std::vector <tagTWrap_Loong_TaskDone*> &vecTar)
{
	//需要去重复
	std::vector <tagTWrap_Loong_TaskDone*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_TaskDone*>::iterator itSrc2 = vecSrc2.begin();

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());

	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		Update_Loong_RoleID((*itSrc2)->dwRoleID);
		vecTar.push_back(*itSrc2);	
	}
	return true;
}
//title
BOOL  CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_Title*> &vecSrc1,
											 std::vector <tagTWrap_Loong_Title*> &vecSrc2,
											 std::vector <tagTWrap_Loong_Title*> &vecTar)
{
	//需要去重复
	std::vector <tagTWrap_Loong_Title*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_Title*>::iterator itSrc2 = vecSrc2.begin();

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());
	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		Update_Loong_RoleID((*itSrc2)->dwRoleID);
		vecTar.push_back(*itSrc2);	
	}
	return true;
}
//vip_stall
BOOL  CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_VipStall*> &vecSrc1,
											 std::vector <tagTWrap_Loong_VipStall*> &vecSrc2,
											 std::vector <tagTWrap_Loong_VipStall*> &vecTar)
{
	//需要去重复
	std::vector <tagTWrap_Loong_VipStall*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_VipStall*>::iterator itSrc2 = vecSrc2.begin();

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());
	
	//byStallIndex 从0开始
	for(;itSrc1!= vecSrc1.end();++itSrc1)
	{

		vecTar.push_back(*itSrc1);
		if((*itSrc1)->byStallIndex >= m_dwSrc1VipStallMaxNum)
		{
			//这个号是从0开始编的
			m_dwSrc1VipStallMaxNum = (*itSrc1)->byStallIndex;
			++m_dwSrc1VipStallMaxNum;
		}
	}
	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		Update_Loong_RoleID((*itSrc2)->dwOwnerID);

		if((*itSrc2)->byStallIndex != GT_INVALID)
			(*itSrc2)->byStallIndex += m_dwSrc1VipStallMaxNum;

		vecTar.push_back(*itSrc2);	
	}
	return true;
}
//VisitingCard
BOOL  CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_VisitingCard*> &vecSrc1,
											 std::vector <tagTWrap_Loong_VisitingCard*> &vecSrc2,
											 std::vector <tagTWrap_Loong_VisitingCard*> &vecTar)
{
	std::vector <tagTWrap_Loong_VisitingCard*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_VisitingCard*>::iterator itSrc2 = vecSrc2.begin();
	
	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());
	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());
	//for(;itSrc1!= vecSrc1.end();++itSrc1)
	//{
	//	vecTar.push_back(*itSrc1);
	//}

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		Update_Loong_RoleID((*itSrc2)->stOthers.dwRoleID);
		//配偶
		if((*itSrc2)->stOthers.dwMateRoleID != GT_INVALID)
			Update_Loong_RoleID((*itSrc2)->stOthers.dwMateRoleID);
		//帮派
		if((*itSrc2)->stOthers.dwFactionID != GT_INVALID)
			(*itSrc2)->stOthers.dwFactionID = \
				m_mpOldGuildIDvsNewGuildID[tagGuildIDKey((*itSrc2)->stOthers.dwFactionID,EM_DB_SRC_TWO)];		

		vecTar.push_back(*itSrc2);	
	}
	return true;
}
//ybaccount
BOOL  CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_YBAccount*> &vecSrc1,
											 std::vector <tagTWrap_Loong_YBAccount*> &vecSrc2,
											 std::vector <tagTWrap_Loong_YBAccount*> &vecTar)
{
	std::vector <tagTWrap_Loong_YBAccount*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_YBAccount*>::iterator itSrc2 = vecSrc2.begin();

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());
	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());
	
	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		Update_Loong_RoleID((*itSrc2)->dwRoleID);
		vecTar.push_back(*itSrc2);	
	}
	return true;
}
//yuanbaoorder
BOOL  CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_YuanBaoOrder*> &vecSrc1,
											 std::vector <tagTWrap_Loong_YuanBaoOrder*> &vecSrc2,
											 std::vector <tagTWrap_Loong_YuanBaoOrder*> &vecTar)
{
	
	std::vector <tagTWrap_Loong_YuanBaoOrder*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_YuanBaoOrder*>::iterator itSrc2 = vecSrc2.begin();
	
	DWORD dwMaxID = 0;


	for(;itSrc1!= vecSrc1.end();++itSrc1)
	{
		vecTar.push_back(*itSrc1);

		if((*itSrc1)->dwID > dwMaxID)
		{
			dwMaxID = (*itSrc1)->dwID;
		}
	}

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		++dwMaxID;
		//索引
		(*itSrc2)->dwID = dwMaxID;
		//roleid
		Update_Loong_RoleID((*itSrc2)->dwRoleID);
		vecTar.push_back(*itSrc2);	
	}
	
	return true;
}
//bill item
BOOL  CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_BillItem*> &vecSrc1,
											 std::vector <tagTWrap_Loong_BillItem*> &vecSrc2,
											 std::vector <tagTWrap_Loong_BillItem*> &vecTar)
{
	
	std::vector <tagTWrap_Loong_BillItem*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_BillItem*>::iterator itSrc2 = vecSrc2.begin();
	std::map<tagAccountIDKey,DWORD>::iterator itSrcFind;

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());

	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());
	//for(;itSrc1!= vecSrc1.end();++itSrc1)
	//{
	//	vecTar.push_back(*itSrc1);
	//}

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		//如果跨大区
		if(m_eMergeType == EM_MERGE_CROSS_SECTION)
		{
			itSrcFind = m_mpOldAccountVsNewAccount.find(tagAccountIDKey((*itSrc2)->dwAccountID,EM_DB_SRC_TWO));
			//如果找到
			if(itSrcFind != m_mpOldAccountVsNewAccount.end())
			{
				//accountid
				(*itSrc2)->dwAccountID = (*itSrcFind).second;				
			}
		}

		vecTar.push_back(*itSrc2);	
	}
	
	return true;
}
//bill yuanbao log
BOOL  CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_BillYuanBaoLog*> &vecSrc1,
											 std::vector <tagTWrap_Loong_BillYuanBaoLog*> &vecSrc2,
											 std::vector <tagTWrap_Loong_BillYuanBaoLog*> &vecTar)
{

	std::vector <tagTWrap_Loong_BillYuanBaoLog*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_BillYuanBaoLog*>::iterator itSrc2 = vecSrc2.begin();
	std::map<tagAccountIDKey,DWORD>::iterator itSrcFind;

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());

	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());
	//for(;itSrc1!= vecSrc1.end();++itSrc1)
	//{
	//	vecTar.push_back(*itSrc1);
	//}
	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		//如果跨大区
		if(m_eMergeType == EM_MERGE_CROSS_SECTION)
		{
			itSrcFind = m_mpOldAccountVsNewAccount.find(tagAccountIDKey((*itSrc2)->dwAccountID,EM_DB_SRC_TWO));
			//如果找到
			if(itSrcFind != m_mpOldAccountVsNewAccount.end())
			{
				//accountid
				(*itSrc2)->dwAccountID = (*itSrcFind).second;				
			}
		}

		vecTar.push_back(*itSrc2);	
	}

	return true;
}
//bill yuanbao
BOOL  CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_BillYuanBao*> &vecSrc1,
											 std::vector <tagTWrap_Loong_BillYuanBao*> &vecSrc2,
											 std::vector <tagTWrap_Loong_BillYuanBao*> &vecTar)
{
	
	std::vector <tagTWrap_Loong_BillYuanBao*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_BillYuanBao*>::iterator itSrc2 = vecSrc2.begin();
	std::map<tagAccountIDKey,DWORD>::iterator itSrcFind;

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());

	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());
	//for(;itSrc1!= vecSrc1.end();++itSrc1)
	//{
	//	vecTar.push_back(*itSrc1);
	//}
	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		//如果跨大区
		if(m_eMergeType == EM_MERGE_CROSS_SECTION)
		{
			itSrcFind = m_mpOldAccountVsNewAccount.find(tagAccountIDKey((*itSrc2)->dwAccountID,EM_DB_SRC_TWO));
			//如果找到
			if(itSrcFind != m_mpOldAccountVsNewAccount.end())
			{
				//accountid
				(*itSrc2)->dwAccountID = (*itSrcFind).second;				
			}
		}

		vecTar.push_back(*itSrc2);	
	}
	
	return true;
}
//guild_commodity
BOOL CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_GuildCommodity*> &vecSrc1,
					  std::vector <tagTWrap_Loong_GuildCommodity*> &vecSrc2,
					  std::vector <tagTWrap_Loong_GuildCommodity*> &vecTar)
{
	std::vector <tagTWrap_Loong_GuildCommodity*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_GuildCommodity*>::iterator itSrc2 = vecSrc2.begin();
	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());

	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		Update_Loong_RoleID((*itSrc2)->stOthers.dwRoleID);
		(*itSrc2)->dwGuildID = m_mpOldGuildIDvsNewGuildID[tagGuildIDKey((*itSrc2)->dwGuildID,EM_DB_SRC_TWO)];

		vecTar.push_back(*itSrc2);	
	}

	return true;
}
//guild_skill
BOOL CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_GuildSkill*> &vecSrc1,
					  std::vector <tagTWrap_Loong_GuildSkill*> &vecSrc2,
					  std::vector <tagTWrap_Loong_GuildSkill*> &vecTar)
{
	std::vector <tagTWrap_Loong_GuildSkill*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_GuildSkill*>::iterator itSrc2 = vecSrc2.begin();
	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());

	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		(*itSrc2)->dwGuildID = m_mpOldGuildIDvsNewGuildID[tagGuildIDKey((*itSrc2)->dwGuildID,EM_DB_SRC_TWO)];

		vecTar.push_back(*itSrc2);	
	}

	return true;

}
//vip_netbar
BOOL CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_VipNetBar*> &vecSrc1,
					  std::vector <tagTWrap_Loong_VipNetBar*> &vecSrc2,
					  std::vector <tagTWrap_Loong_VipNetBar*> &vecTar)
{
	std::vector <tagTWrap_Loong_VipNetBar*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_VipNetBar*>::iterator itSrc2 = vecSrc2.begin();
	std::map<tagAccountIDKey,DWORD>::iterator itSrcFind;

	std::set<DWORD> setAccountID;
	std::set<DWORD>::iterator itAccountFind;

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());

	for(;itSrc1!= vecSrc1.end();++itSrc1)
	{
		setAccountID.insert((*itSrc1)->dwAccountID);
		vecTar.push_back(*itSrc1);	
	}
	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		//如果是跨大区的
		if(m_eMergeType == EM_MERGE_CROSS_SECTION)
		{
			itSrcFind = m_mpOldAccountVsNewAccount.find(tagAccountIDKey((*itSrc2)->dwAccountID,EM_DB_SRC_TWO));
			//如果找到
			if(itSrcFind != m_mpOldAccountVsNewAccount.end())
			{		
				//更新一下id
				(*itSrc2)->dwAccountID = itSrcFind->second;			
			}
		}

		//如果找到重复的号
		itAccountFind = setAccountID.find((*itSrc2)->dwAccountID);
		if(itAccountFind != setAccountID.end())
		{
			continue;
		}


		vecTar.push_back(*itSrc2);	
	}

	return true;
}
//guild_upgrade
BOOL CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_GuildFacilities*> &vecSrc1,
					  std::vector <tagTWrap_Loong_GuildFacilities*> &vecSrc2,
					  std::vector <tagTWrap_Loong_GuildFacilities*> &vecTar)
{
	std::vector <tagTWrap_Loong_GuildFacilities*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_GuildFacilities*>::iterator itSrc2 = vecSrc2.begin();

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());

	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		(*itSrc2)->dwGuildID = m_mpOldGuildIDvsNewGuildID[tagGuildIDKey((*itSrc2)->dwGuildID,EM_DB_SRC_TWO)];
		vecTar.push_back(*itSrc2);	
	}

	return true;
}
//commerce_rank
BOOL CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_CommerceRank*> &vecSrc1,
					  std::vector <tagTWrap_Loong_CommerceRank*> &vecSrc2,
					  std::vector <tagTWrap_Loong_CommerceRank*> &vecTar)
{
	std::vector <tagTWrap_Loong_CommerceRank*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_CommerceRank*>::iterator itSrc2 = vecSrc2.begin();
	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());

	vecTar.insert(vecTar.end(),vecSrc1.begin(),vecSrc1.end());

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		Update_Loong_RoleID((*itSrc2)->stOthers.dwRoleID);
		(*itSrc2)->dwGuildID = m_mpOldGuildIDvsNewGuildID[tagGuildIDKey((*itSrc2)->dwGuildID,EM_DB_SRC_TWO)];
		vecTar.push_back(*itSrc2);	
	}

	return true;
}
//activity
BOOL CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_ActivityData*> &vecSrc1,
					  std::vector <tagTWrap_Loong_ActivityData*> &vecSrc2,
					  std::vector <tagTWrap_Loong_ActivityData*> &vecTar)
{
	std::vector <tagTWrap_Loong_ActivityData*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_ActivityData*>::iterator itSrc2 = vecSrc2.begin();
	std::set <DWORD> setScriptID;
	std::set <DWORD>::iterator itFinder;
	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());

	//根据脚本id去重复


	for(;itSrc1!= vecSrc1.end();++itSrc1)
	{
		setScriptID.insert((*itSrc1)->dwID);
		vecTar.push_back(*itSrc1);	
	}

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		itFinder = setScriptID.find((*itSrc2)->dwID);
		if(itFinder != setScriptID.end())
		{
			continue;
		}
		vecTar.push_back(*itSrc2);	
	}

	return true;
}

//left msg
BOOL CLoongMergeLogicCtrl::Loong_LoongMerge(std::vector <tagTWrap_Loong_LeftMsg*> &vecSrc1,
											std::vector <tagTWrap_Loong_LeftMsg*> &vecSrc2,
											std::vector <tagTWrap_Loong_LeftMsg*> &vecTar)
{
	std::vector <tagTWrap_Loong_LeftMsg*>::iterator itSrc1 = vecSrc1.begin();
	std::vector <tagTWrap_Loong_LeftMsg*>::iterator itSrc2 = vecSrc2.begin();

	std::set<DWORD> setTime;
	std::set<DWORD>::iterator itFind;

	//预留空间
	vecTar.reserve(vecSrc1.size() + vecSrc2.size());

	for(;itSrc1!= vecSrc1.end();++itSrc1)
	{
		setTime.insert((*itSrc1)->dwMsgID);
		vecTar.push_back(*itSrc1);
	}

	for(;itSrc2!= vecSrc2.end();++itSrc2)
	{
		//处理相同时间的冲突
		while (setTime.find((*itSrc2)->dwMsgID) != setTime.end())
		{
			(*itSrc2)->dwMsgID = IncreaseTime((*itSrc2)->dwMsgID,1);
		}
		setTime.insert((*itSrc2)->dwMsgID);

		Update_Loong_RoleID((*itSrc2)->dwRoleID);
		vecTar.push_back(*itSrc2);	
	}

	return true;
}