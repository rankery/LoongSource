#include "StdAfx.h"
#include "wx/wx.h"
#include "Resource.h"
#include "Loong_ServerMerge.h"
#include "Loong_LoginDBCtrl.h"
#include "Loong_LoongDBCtrl.h"
#include "Loong_OutputCtrl.h"
#include "Loong_TMethod.h"


CLoongServerMerge::CLoongServerMerge(void)
{

}

CLoongServerMerge::~CLoongServerMerge(void)
{	
	std::vector<tagThreadPara*>::iterator itLoop = vecParaList.begin();
	for(;itLoop != vecParaList.end(); ++itLoop)
	{
		SAFE_DEL(*itLoop);
	}

	//关闭句柄
	std::list<HANDLE>::iterator itH = m_listSimpleThreadPool.begin();
	for(;itH != m_listSimpleThreadPool.end(); ++itH)
	{
		if(*itH != NULL)
		{
			TerminateThread(*itH,0);
			CloseHandle(*itH);
			*itH = 0;
		}
	}

	SAFE_CLOSE_HANDLE(m_hPoolMergeEvent);

	//销毁数据库
	sLoginBeton.Destroy();
	sLoongBeton.Destroy();
}
/*
/	初始化DB
/
*/
BOOL CLoongServerMerge::Init(tagConfigDetail stConfigOne[EM_DB_TYPE_END],
							 tagConfigDetail stConfigTwo[EM_DB_TYPE_END],EDBMergeType eMergeType,
							 tstring strSrc2WorldName,
							 const std::multimap<ERoleCompensationType, tagCompensation*> mmCompensation)
{
	BOOL bRet = FALSE;

	//如果是跨服合并
	if(eMergeType == EM_MERGE_CROSS_SECTION)
	{
		//初始化Login
		bRet = sLoginBeton.Init(stConfigOne);
		if(bRet == FALSE)
		{
			return FALSE;
		}
	}
	//初始化Loong
	bRet = sLoongBeton.Init(stConfigTwo);
	if(bRet == FALSE)
	{
		return FALSE;
	}
	//合并类型
	m_eMergeType = eMergeType;
	m_strSrc2WorldName = strSrc2WorldName;

	//线程池合并同步
	m_hPoolMergeEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	//关闭句柄
	std::list<HANDLE>::iterator itH = m_listSimpleThreadPool.begin();
	for(;itH != m_listSimpleThreadPool.end(); ++itH)
	{
		if(*itH != NULL)
		{
			TerminateThread(*itH,0);
			CloseHandle(*itH);
			*itH = 0;
		}
	}

	for(INT32 nLoop = 0; nLoop<THREAD_POOL_NUM; nLoop++)
	{
		tagThreadPara *par = new tagThreadPara(nLoop,this);
		vecParaList.push_back(par);

		HANDLE thread_work_handle_ = (HANDLE)_beginthreadex(NULL, 0, (THREADPROC)&MyThreadPool, (LPVOID)par, 0, NULL);
		m_listSimpleThreadPool.push_back(thread_work_handle_);
	}

	//玩家补偿信息
	m_mmCompensation.insert(mmCompensation.begin(),mmCompensation.end());

	return TRUE;
}
/*
/	初始化合并逻辑
*/
BOOL CLoongServerMerge::LoongMergeLogicCtrlInit(CLoongMergeLogicCtrl &clsMergeLogicCtrl)
{	
	//合并类型
	clsMergeLogicCtrl.SetMergeType(m_eMergeType);
	//源2的游戏世界名
	clsMergeLogicCtrl.SetSrc2WorldName(m_strSrc2WorldName);

	//如果是跨大区合并
	if(m_eMergeType == EM_MERGE_CROSS_SECTION)
	{
		//源1的account表最大num
		DWORD dwSrc1LoginMaxNum = sLoginBeton.GetAccountMaxNum(EM_DB_SRC_ONE);
		if(dwSrc1LoginMaxNum == GT_INVALID)
		{
			return FALSE;
		}
		clsMergeLogicCtrl.SetSrc1LoginMaxNum(dwSrc1LoginMaxNum);
	}

	//源1roledate表最大num
	DWORD dwSrc1RoleMaxNum = sLoongBeton.GetRoleDataMaxNum(EM_DB_SRC_ONE);
	if(dwSrc1RoleMaxNum == GT_INVALID)
	{
		return FALSE;
	}
	clsMergeLogicCtrl.SetSrc1RoleMaxNum(dwSrc1RoleMaxNum);

	//源1petdata表最大num
	DWORD dwSrc1PetMaxNum = sLoongBeton.GetPetDataMaxNum(EM_DB_SRC_ONE);
	if(dwSrc1PetMaxNum == GT_INVALID)
	{
		return FALSE;
	}
	clsMergeLogicCtrl.SetSrc1PetMaxNum(dwSrc1PetMaxNum);

	//源1划拨物品最小序号
	INT64 n64ItemMinSerialBill = sLoongBeton.GetItemMinSerialBill(EM_DB_SRC_ONE);
	if(n64ItemMinSerialBill == GT_INVALID)
	{
		return FALSE;
	}
	clsMergeLogicCtrl.SetSrc1ItemMinSerialBill(n64ItemMinSerialBill);

	//源1内部物品的最大序号
	INT64 n64ItemMaxSerialInternal = sLoongBeton.GetMaxSerialInternal(EM_DB_SRC_ONE);
	if(n64ItemMaxSerialInternal == GT_INVALID)
	{
		return FALSE;
	}
	clsMergeLogicCtrl.SetSrc1ItemMaxSerialInternal(n64ItemMaxSerialInternal);

	//源1外部物品的最小序号
	INT64 n64ItemMinSerialOutter = sLoongBeton.GetMinSerialOutter(EM_DB_SRC_ONE);
	if(n64ItemMinSerialOutter == GT_INVALID)
	{
		return FALSE;
	}
	clsMergeLogicCtrl.SetSrc1ItemMinSerialOutter(n64ItemMinSerialOutter);

	return TRUE;
}

/*
/	换名字的补偿
*/
BOOL CLoongServerMerge::LoongRoleCompensationChangeRoleName(const tagRoleCompensation &st)
{
	//道具补偿
	std::multimap<ERoleCompensationType, tagCompensation*>::iterator itLoop =  m_mmCompensation.find(st.Type);

	if(itLoop != m_mmCompensation.end())
	{
		//寻找对应的补偿类型
		INT32 nCount = m_mmCompensation.count(st.Type);
		for(INT32 nLoop = 0; nLoop != nCount; ++nLoop,++itLoop)
		{
			//物品补偿
			INT64 n64SerialNum = m_clsMergeLogicCtrl.GetNewItemSerialOutter();
			if(typeid(*(itLoop->second)) == typeid(tagItemCompensation))
			{
				INT64 n64SerialNum = m_clsMergeLogicCtrl.GetNewItemSerialOutter();
				BOOL bRet = sLoongBeton.CreateItem(n64SerialNum,
													((tagItemCompensation*)(itLoop->second))->dwItemTypeID,
													st.dwAccountID,
													st.dwRoleID);
				if(bRet == FALSE)
				{
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}

/*
/	玩家补偿
*/
BOOL CLoongServerMerge::LoongRoleCompensation()
{
	const std::vector<tagRoleCompensation> &vecRoleCompensation = m_clsMergeLogicCtrl.GetRoleCompensation();
	std::vector<tagRoleCompensation>::const_iterator itLoop =  vecRoleCompensation.begin();
	for(;itLoop != vecRoleCompensation.end(); ++itLoop)
	{
		switch((*itLoop).Type)
		{
		case ERC_CHANGE_ROLE_NAME:
			{
				BOOL bRet = LoongRoleCompensationChangeRoleName((*itLoop));
				if(bRet != TRUE)
				{
					return FALSE;
				}
			}
			break;
		default:
			break;
		}
	}

	return TRUE;
}

/*
/	简单线程池
*/
UINT MyThreadPool(LPVOID pParam)
{
	//异常保护
	EXCEPTION_PROTECT;

	tagThreadPara		*pPara;
	pPara	= (tagThreadPara*)pParam;


	WaitForSingleObject(pPara->_p->m_hPoolMergeEvent, INFINITE);

	if(pPara->_n == 0)
	{
		//////////////////////////////////////////////////////////////////////////
		//itemdel
		g_clsOutput.Output(_T("开始合并--itemdel!\r\n"));
		tagTWrap_Loong_ItemDel *TypeItemDel = NULL;
		BOOL bRet = TMethod_Loong_PagingServerMerge(TypeItemDel,pPara->_p->m_clsMergeLogicCtrl);
		if(bRet == FALSE)
		{	
			g_clsOutput.Output(_T("合并失败--itemdel!\r\n"));
			return FALSE;
		}
		g_clsOutput.Output(_T("合并成功--itemdel!\r\n"));
	}
	else if(pPara->_n == 1)
	{
		//////////////////////////////////////////////////////////////////////////
		//item
		g_clsOutput.Output(_T("开始合并--item!\r\n"));
		tagTWrap_Loong_Item *TypeItem = NULL;
		BOOL bRet = TMethod_Loong_AccountServerMerge(TypeItem,pPara->_p->m_clsMergeLogicCtrl);
		if(bRet == FALSE)
		{	
			g_clsOutput.Output(_T("合并失败--item!\r\n"));
			return FALSE;
		}
		g_clsOutput.Output(_T("合并成功--item!\r\n"));

		//////////////////////////////////////////////////////////////////////////
		//itembaibao
		g_clsOutput.Output(_T("开始合并--itembaibao!\r\n"));
		tagTWrap_Loong_ItemBaiBao *TypeItemBaiBao = NULL;
		bRet = TMethod_Loong_ServerMerge(TypeItemBaiBao,pPara->_p->m_clsMergeLogicCtrl);
		if(bRet == FALSE)
		{	
			g_clsOutput.Output(_T("合并itembaibao失败!\r\n"));
			return FALSE;
		}
		g_clsOutput.Output(_T("合并成功--itembaibao!\r\n"));
	}
	else if(pPara->_n == 2)
	{
		//equip
		g_clsOutput.Output(_T("开始合并--equip!\r\n"));
		tagTWrap_Loong_Equip *TypeEquip = NULL;
		BOOL bRet = TMethod_Loong_PagingServerMerge(TypeEquip,pPara->_p->m_clsMergeLogicCtrl);
		if(bRet == FALSE)
		{	
			g_clsOutput.Output(_T("合并失败--equip!\r\n"));
			return FALSE;
		}
		g_clsOutput.Output(_T("合并成功--equip!\r\n"));
	}
	else if(pPara->_n == 3)
	{
		//equipdel
		g_clsOutput.Output(_T("开始合并--equipdel!\r\n"));
		tagTWrap_Loong_EquipDel *TypeEquipDel = NULL;
		BOOL bRet = TMethod_Loong_PagingServerMerge(TypeEquipDel,pPara->_p->m_clsMergeLogicCtrl);
		if(bRet == FALSE)
		{
			g_clsOutput.Output(_T("合并失败--equipdel!\r\n"));
			return FALSE;
		}
		g_clsOutput.Output(_T("合并成功--equipdel!\r\n"));

		//equipbaibao
		g_clsOutput.Output(_T("开始合并--Equipbaibao!\r\n"));
		tagTWrap_Loong_EquipBaiBao *TypeEquipBaiBao = NULL;
		bRet = TMethod_Loong_SeparateServerMerge(TypeEquipBaiBao,pPara->_p->m_clsMergeLogicCtrl);
		if(bRet == FALSE)
		{
			g_clsOutput.Output(_T("合并失败--Equipbaibao!\r\n"));
			return FALSE;
		}
		g_clsOutput.Output(_T("合并成功--Equipbaibao!\r\n"));
	}
	else if(pPara->_n == 4)
	{
		//task
		g_clsOutput.Output(_T("开始合并--Task!\r\n"));
		tagTWrap_Loong_Task *TypeTask = NULL;
		BOOL bRet = TMethod_Loong_PagingServerMerge(TypeTask,pPara->_p->m_clsMergeLogicCtrl);
		if(bRet == FALSE)
		{
			g_clsOutput.Output(_T("合并失败--Task!\r\n"));
			return FALSE;
		}
		g_clsOutput.Output(_T("合并成功--Task!\r\n"));
	
		//taskdone
		g_clsOutput.Output(_T("开始合并--TaskDone!\r\n"));
		tagTWrap_Loong_TaskDone *TypeTaskDone = NULL;
		bRet = TMethod_Loong_PagingServerMerge(TypeTaskDone,pPara->_p->m_clsMergeLogicCtrl);
		if(bRet == FALSE)
		{	
			g_clsOutput.Output(_T("合并失败--TaskDone!\r\n"));
			return FALSE;
		}
		g_clsOutput.Output(_T("合并成功--TaskDone!\r\n"));
	}


	return TRUE;

}

/*
/	开始合并
*/
BOOL CLoongServerMerge::ServerMerge()
{	
	//初始化合并逻辑控制

	BOOL bRet = LoongMergeLogicCtrlInit(m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并逻辑控制初始化失败!\r\n"));
		return FALSE;
	}

//////////////////////////////////////////////////////////////////////////
//以下方法需要按顺序在合并时最先调用 
//////////////////////////////////////////////////////////////////////////
	g_clsOutput.Output(_T("开始合并--AccountCommon!\r\n"));
	tagTWrap_Loong_AccountCommon *TypeAccountCommon = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeAccountCommon,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--AccountCommon!\r\n"));
		return FALSE;
	}	
	g_clsOutput.Output(_T("合并成功--AccountCommon!\r\n"));
//////////////////////////////////////////////////////////////////////////
	//帮派
	g_clsOutput.Output(_T("开始合并--Guild!\r\n"));
	tagTWrap_Loong_Guild *TypeGuild = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeGuild,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{	
		g_clsOutput.Output(_T("合并失败--Guild!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--Guild!\r\n"));
//////////////////////////////////////////////////////////////////////////
	//操作role data
	g_clsOutput.Output(_T("开始合并--RoleData!\r\n"));
	tagTWrap_Loong_RoleData *TypeRoleData = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeRoleData,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--RoleData!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--RoleData!\r\n"));
//////////////////////////////////////////////////////////////////////////
//以上方法需要按顺序在合并时最先调用 
//////////////////////////////////////////////////////////////////////////

//大区内合并可以跳过login合并
if(m_eMergeType == EM_MERGE_INSIDE_SECTION)
	goto INSIDE_SECTION;
//////////////////////////////////////////////////////////////////////////
//以下方法为login库的表
//////////////////////////////////////////////////////////////////////////
	g_clsOutput.Output(_T("开始合并--LoginAccount!\r\n"));
	tagTWrap_Longin_Account *TypeAccount = NULL;
	bRet = TMethod_Login_ServerMerge(TypeAccount,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--LoginAccount!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--LoginAccount!\r\n"));

//////////////////////////////////////////////////////////////////////////
	g_clsOutput.Output(_T("开始合并--LoginWorldForbid!\r\n"));
	tagTWrap_Longin_WorldForbid *TypeWorldForbid = NULL;
	bRet = TMethod_Login_ServerMerge(TypeWorldForbid,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--LoginWorldForbid!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--LoginWorldForbid!\r\n"));
//////////////////////////////////////////////////////////////////////////
	g_clsOutput.Output(_T("开始合并--LoginBlackList!\r\n"));
	tagTWrap_Longin_BlackList *TypeLoginBlackList = NULL;
	bRet = TMethod_Login_ServerMerge(TypeLoginBlackList,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--LoginBlackList!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--LoginBlackList!\r\n"));
//////////////////////////////////////////////////////////////////////////
	g_clsOutput.Output(_T("开始合并--LoginFatigueTime!\r\n"));
	tagTWrap_Longin_FatigueTime *TypeLoginFatigueTime = NULL;
	bRet = TMethod_Login_ServerMerge(TypeLoginFatigueTime,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--LoginFatigueTime!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--LoginFatigueTime!\r\n"));

//////////////////////////////////////////////////////////////////////////
//以上方法为login库的表
//////////////////////////////////////////////////////////////////////////

//大区内合并可以跳过login合并
INSIDE_SECTION:
//////////////////////////////////////////////////////////////////////////
	//activity
	g_clsOutput.Output(_T("开始合并--activity!\r\n"));
	tagTWrap_Loong_ActivityData *TypeActivityData = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeActivityData,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--activity!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--activity!\r\n"));	
//////////////////////////////////////////////////////////////////////////
	//ActClanTreasure
	g_clsOutput.Output(_T("开始合并--ActClanTreasure!\r\n"));
	tagTWrap_Loong_ActClanTreasure *TypeActClanTreasure = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeActClanTreasure,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--ActClanTreasure!\r\n"));
		return FALSE;
	}	
	g_clsOutput.Output(_T("合并成功--ActClanTreasure!\r\n"));
//////////////////////////////////////////////////////////////////////////
	g_clsOutput.Output(_T("开始合并--RepRstTime!\r\n"));
	tagTWrap_Loong_RepRstTime *TypeRepRstTime = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeRepRstTime,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--RepRstTime!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--RepRstTime!\r\n"));
//////////////////////////////////////////////////////////////////////////
	g_clsOutput.Output(_T("开始合并--FameHallEnterSnap!\r\n"));
	tagTWrap_Loong_FameHallEnterSnap *TypeFameHallEnterSnap = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeFameHallEnterSnap,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--FameHallEnterSnap!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--FameHallEnterSnap!\r\n"));
//////////////////////////////////////////////////////////////////////////
	//yuanbaoorder
	g_clsOutput.Output(_T("开始合并--yuanbaoorder!\r\n"));
	tagTWrap_Loong_YuanBaoOrder *TypeYuanBaoOrder = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeYuanBaoOrder,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--yuanbaoorder!\r\n"));
		return FALSE;
	}	
	g_clsOutput.Output(_T("合并成功--yuanbaoorder!\r\n"));
//////////////////////////////////////////////////////////////////////////
	//vip_stall
	g_clsOutput.Output(_T("开始合并--VipStall!\r\n"));
	tagTWrap_Loong_VipStall *TypeVipStall = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeVipStall,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--VipStall!\r\n"));
		return FALSE;
	}	
	g_clsOutput.Output(_T("合并成功--VipStall!\r\n"));
//////////////////////////////////////////////////////////////////////////
	//pet_skill
	g_clsOutput.Output(_T("开始合并--PetSkill!\r\n"));
	tagTWrap_Loong_PetSkill *TypePetSkill = NULL;
	bRet = TMethod_Loong_ServerMerge(TypePetSkill,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--PetSkill!\r\n"));
		return FALSE;
	}	
	g_clsOutput.Output(_T("合并成功--PetSkill!\r\n"));
//////////////////////////////////////////////////////////////////////////
	//pet_data
	g_clsOutput.Output(_T("开始合并--PetData!\r\n"));
	tagTWrap_Loong_PetData *TypePetData = NULL;
	bRet = TMethod_Loong_ServerMerge(TypePetData,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--PetData!\r\n"));
		return FALSE;
	}	
	g_clsOutput.Output(_T("合并成功--PetData!\r\n"));
//////////////////////////////////////////////////////////////////////////
	//BlackList
	g_clsOutput.Output(_T("开始合并--BlackList!\r\n"));
	tagTWrap_Loong_BlackList *TypeBlackList = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeBlackList,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{	
		g_clsOutput.Output(_T("合并失败--BlackList!\r\n"));
		return FALSE;
	}	
	g_clsOutput.Output(_T("合并成功--BlackList!\r\n"));
//////////////////////////////////////////////////////////////////////////
	//left msg
	g_clsOutput.Output(_T("开始合并--left_msg!\r\n"));
	tagTWrap_Loong_LeftMsg *TypeLeftMsg = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeLeftMsg,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--left_msg!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--left_msg!\r\n"));
//////////////////////////////////////////////////////////////////////////
	//buff
	g_clsOutput.Output(_T("开始合并--buff!\r\n"));
	tagTWrap_Loong_Buff *TypeBuff = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeBuff,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--buff!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--buff!\r\n"));	

//////////////////////////////////////////////////////////////////////////
	//clan_data
	g_clsOutput.Output(_T("开始合并--clan_data!\r\n"));
	tagTWrap_Loong_ClanData *TypeClanDate = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeClanDate,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{	
		g_clsOutput.Output(_T("合并失败--clan_data!\r\n"));
		return FALSE;
	}	
	g_clsOutput.Output(_T("合并成功--clan_data!\r\n"));	

//////////////////////////////////////////////////////////////////////////
	//enemy
	g_clsOutput.Output(_T("开始合并--enemy!\r\n"));
	tagTWrap_Loong_Enemy *TypeEnemy = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeEnemy,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{	
		g_clsOutput.Output(_T("合并失败--enemy!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--enemy!\r\n"));
//////////////////////////////////////////////////////////////////////////
	//friend
	g_clsOutput.Output(_T("开始合并--Friend!\r\n"));
	tagTWrap_Loong_Friend *TypeFriend = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeFriend,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{	
		g_clsOutput.Output(_T("合并失败--Friend!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--Friend!\r\n"));

//////////////////////////////////////////////////////////////////////////
	//friendship
	g_clsOutput.Output(_T("开始合并--FriendShip!\r\n"));
	tagTWrap_Loong_FriendshipSave *TypeFriendShip = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeFriendShip,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{	
		g_clsOutput.Output(_T("合并FriendShip失败!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--FriendShip!\r\n"));

//////////////////////////////////////////////////////////////////////////
	//GroupPurchase
	g_clsOutput.Output(_T("开始合并--GroupPuichase!\r\n"));
	tagTWrap_Loong_GroupPurchase *TypeGroupPurchase = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeGroupPurchase,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并GroupPuichase失败!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--GroupPuichase!\r\n"));

//////////////////////////////////////////////////////////////////////////
	//GuildMem
	g_clsOutput.Output(_T("开始合并--GuildMem!\r\n"));
	tagTWrap_Loong_GuildMem *TypeGuildMem = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeGuildMem,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--GuildMem!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--GuildMem!\r\n"));

//////////////////////////////////////////////////////////////////////////
	//item_cdtime
	g_clsOutput.Output(_T("开始合并--ItemCDTime!\r\n"));
	tagTWrap_Loong_ItemCDTime *TypeItemCDTime = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeItemCDTime,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{	
		g_clsOutput.Output(_T("合并失败--ItemCDTime!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--ItemCDTime!\r\n"));

//////////////////////////////////////////////////////////////////////////
	//item_needlog
	g_clsOutput.Output(_T("开始合并--ItemNeedLog!\r\n"));
	tagTWrap_Loong_ItemNeedLog *TypeItemNeedLog = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeItemNeedLog,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--ItemNeedLog!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--ItemNeedLog!\r\n"));

//////////////////////////////////////////////////////////////////////////
	//log_baibao
	g_clsOutput.Output(_T("开始合并--log_baibao!\r\n"));
	tagTWrap_Loong_LogBaiBao *TypeLogBaiBao = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeLogBaiBao,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{	
		g_clsOutput.Output(_T("合并失败--log_baibao!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--log_baibao!\r\n"));

//////////////////////////////////////////////////////////////////////////
	//role_del
	g_clsOutput.Output(_T("开始合并--RoleDel!\r\n"));
	tagTWrap_Loong_RoleDel *TypeRoleDel = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeRoleDel,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{	
		g_clsOutput.Output(_T("合并失败--RoleDel!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--RoleDel!\r\n"));

//////////////////////////////////////////////////////////////////////////
	//skill
	g_clsOutput.Output(_T("开始合并--Skill!\r\n"));
	tagTWrap_Loong_Skill *TypeSkill = NULL;
	bRet = TMethod_Loong_SeparateServerMerge(TypeSkill,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--Skill!\r\n"));
		return FALSE;
	}		
	g_clsOutput.Output(_T("合并成功--Skill!\r\n"));

//////////////////////////////////////////////////////////////////////////
	//title
	g_clsOutput.Output(_T("开始合并--Title!\r\n"));
	tagTWrap_Loong_Title *TypeTitle = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeTitle,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--Title!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--Title!\r\n"));

//////////////////////////////////////////////////////////////////////////
	//名片
	g_clsOutput.Output(_T("开始合并--VisitingCard!\r\n"));
	tagTWrap_Loong_VisitingCard *TypeVisitingCard = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeVisitingCard,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--VisitingCard!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--VisitingCard!\r\n"));

//////////////////////////////////////////////////////////////////////////
	//元宝账号
	g_clsOutput.Output(_T("开始合并--FameYBAccount!\r\n"));
	tagTWrap_Loong_YBAccount *TypeYBAccount = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeYBAccount,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--FameYBAccount!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--FameYBAccount!\r\n"));

//////////////////////////////////////////////////////////////////////////
	//guild_commodity
	g_clsOutput.Output(_T("开始合并--guild_commodity!\r\n"));
	tagTWrap_Loong_GuildCommodity *TypeGuildCommodity = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeGuildCommodity,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--guild_commodity!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--guild_commodity!\r\n"));

//////////////////////////////////////////////////////////////////////////
	//guild_skill
	g_clsOutput.Output(_T("开始合并--guild_skill!\r\n"));
	tagTWrap_Loong_GuildSkill *TypeGuildSkill = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeGuildSkill,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--guild_skill!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--guild_skill!\r\n"));

//////////////////////////////////////////////////////////////////////////
	//vip_netbar
	g_clsOutput.Output(_T("开始合并--vip_netbar!\r\n"));
	tagTWrap_Loong_VipNetBar *TypeVipNetBar = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeVipNetBar,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--vip_netbar!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--vip_netbar!\r\n"));

//////////////////////////////////////////////////////////////////////////
	//guild_upgrade
	g_clsOutput.Output(_T("开始合并--guild_upgrade!\r\n"));
	tagTWrap_Loong_GuildFacilities *TypeGuildFacilities = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeGuildFacilities,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--guild_upgrade!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--guild_upgrade!\r\n"));	

//////////////////////////////////////////////////////////////////////////
	//commerce_rank
	g_clsOutput.Output(_T("开始合并--commerce_rank!\r\n"));
	tagTWrap_Loong_CommerceRank *TypeCommerceRank = NULL;
	bRet = TMethod_Loong_ServerMerge(TypeCommerceRank,m_clsMergeLogicCtrl);
	if(bRet == FALSE)
	{
		g_clsOutput.Output(_T("合并失败--commerce_rank!\r\n"));
		return FALSE;
	}
	g_clsOutput.Output(_T("合并成功--commerce_rank!\r\n"));	


//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//以下是数据量很大的几个表 (item itembaibao 顺序不能变) 采用线程池合并
//////////////////////////////////////////////////////////////////////////
	//开启线程池合并
	for(INT32 nLoop = 0; nLoop<THREAD_POOL_NUM; nLoop++)
		SetEvent(m_hPoolMergeEvent);

	//等待线程池中所有线程
	EThreadPoolState e;
	while(TRUE)
	{
		e = GetThreadPoolState();
		if(e == ETPS_STILL_HAVE_ACTIVE)
		{
			Sleep(10000); //10秒
			continue;
		}
		else if(e == ETPS_FAILURE)
		{
			break;
		}
		else if(e == ETPS_SUCCESS)
		{
			break;
		}
	}

	//如果合服成功 给予玩家相应的补偿
	if(e == ETPS_SUCCESS)
	{
		g_clsOutput.Output(_T("开始玩家补偿!\r\n"));
		BOOL bRet = LoongRoleCompensation();
		if(bRet  == FALSE)
		{
			g_clsOutput.Output(_T("玩家补偿失败!\r\n"));
			e = ETPS_FAILURE;
		}	
		g_clsOutput.Output(_T("玩家补偿成功!\r\n"));
	}

	//清空逻辑
	m_clsMergeLogicCtrl.ClearMemory();

	if(e == ETPS_SUCCESS)
		g_clsOutput.Output(_T("成功!\r\n"));
	else if(e == ETPS_FAILURE)
	{
		//关闭句柄
		std::list<HANDLE>::iterator itH = m_listSimpleThreadPool.begin();
		for(;itH != m_listSimpleThreadPool.end(); ++itH)
		{
			if(*itH != NULL)
			{
				TerminateThread(*itH,0);
				CloseHandle(*itH);
				*itH = 0;
			}
		}
		g_clsOutput.Output(_T("失败 请重启工具 重新合并!\r\n"));
	}

	return TRUE;
}
/*
/	线程池内是否还有线程活着
*/
EThreadPoolState CLoongServerMerge::GetThreadPoolState()
{
	std::list<HANDLE>::iterator itLoop = m_listSimpleThreadPool.begin();

	for(;itLoop != m_listSimpleThreadPool.end(); ++itLoop)
	{
		HANDLE hHandle = *itLoop;
		DWORD dwResult = 0;
		if( hHandle )
		{
			DWORD dwConnectExitCode = STILL_ACTIVE;
			GetExitCodeThread(hHandle, &dwConnectExitCode);
			if( STILL_ACTIVE == dwConnectExitCode )
				return ETPS_STILL_HAVE_ACTIVE;
			else if(FALSE == dwConnectExitCode)
				return ETPS_FAILURE;
		}
	}

	return ETPS_SUCCESS;
}