#include "stdafx.h"
#include "MallMgr.h"
#include "MallFrame.h"
#include "CombatSysUtil.h"
#include "ItemProtoData.h"
#include "MallGiftFrame.h"
#include "YuanbaoFrame.h"
#include "GroupBuyFrame.h"
#include "..\WorldDefine\container_define.h"
#include "..\WorldDefine\AvatarAtt.h"


MallMgr::MallMgr( void ):m_Trunk(this)
{
	m_pFrameMgr				= NULL;
	m_pUtil					= NULL;
	m_pCmdMgr				= NULL;
	m_pSession				= NULL;
	m_pMallFrame			= NULL;
    m_pYuanbaoFrame			= NULL;
	m_dwFreeGetTime			= GT_INVALID;
	m_pGroupBuyFrame		= NULL;
	m_dwGroupBuyTime		= GT_INVALID;
	m_bOpen					= FALSE;
}

MallMgr::~MallMgr( void )
{

}

MallMgr g_MallMgr;
MallMgr* MallMgr::Inst()
{
	return &g_MallMgr;
}

VOID MallMgr::Init()
{
	m_pUtil = TObjRef<Util>();
	m_pCmdMgr = TObjRef<NetCmdMgr>();
	m_pSession = TObjRef<NetSession>();
	m_pFrameMgr = TObjRef<GameFrameMgr>();

	//--注册游戏事件
	m_pFrameMgr->RegisterEventHandle(_T("Open_Mall"),	  (FRAMEEVENTPROC)m_Trunk.sfp1(&MallMgr::OnOpen_Mall));
	m_pFrameMgr->RegisterEventHandle(_T("Close_Mall"),	  (FRAMEEVENTPROC)m_Trunk.sfp1(&MallMgr::OnClose_Mall));

	//--注册网络消息
	m_pCmdMgr->Register("NS_OpenMall",			(NETMSGPROC)m_Trunk.sfp2(&MallMgr::OnNS_OpenMall),		_T("NS_OpenMall"));
	m_pCmdMgr->Register("NS_MallItem",			(NETMSGPROC)m_Trunk.sfp2(&MallMgr::OnNS_MallItem),		_T("NS_MallItem"));
	m_pCmdMgr->Register("NS_MallFreeItem",		(NETMSGPROC)m_Trunk.sfp2(&MallMgr::OnNS_MallFreeItem),	_T("NS_MallFreeItem"));
	m_pCmdMgr->Register("NS_MallPack",			(NETMSGPROC)m_Trunk.sfp2(&MallMgr::OnNS_MallPack),		_T("NS_MallPack"));
	m_pCmdMgr->Register("NS_MallUpdateItem",	(NETMSGPROC)m_Trunk.sfp2(&MallMgr::OnNS_MallUpdateItem),_T("NS_MallUpdateItem"));
	m_pCmdMgr->Register("NS_MallUpdatePack",	(NETMSGPROC)m_Trunk.sfp2(&MallMgr::OnNS_MallUpdatePack),_T("NS_MallUpdatePack"));
	m_pCmdMgr->Register("NS_MallUpdate",		(NETMSGPROC)m_Trunk.sfp2(&MallMgr::OnNS_MallUpdate),	_T("NS_MallUpdate"));
	m_pCmdMgr->Register("NS_GetSimGPInfo",		(NETMSGPROC)m_Trunk.sfp2(&MallMgr::OnNS_GetSimGPInfo),	_T("NS_GetSimGPInfo"));

	m_pMallFrame = (MallFrame*)m_pFrameMgr->CreateFrame(_T("World"), _T("Mall"), _T("MallFrame"), 0);
}

VOID MallMgr::Update()
{

}

VOID MallMgr::Destroy()
{
	//--注销游戏事件
	m_pFrameMgr->UnRegisterEventHandler(_T("Open_Mall"),	  (FRAMEEVENTPROC)m_Trunk.sfp1(&MallMgr::OnOpen_Mall));
	m_pFrameMgr->UnRegisterEventHandler(_T("Close_Mall"),	  (FRAMEEVENTPROC)m_Trunk.sfp1(&MallMgr::OnClose_Mall));

	//--注销网络消息
	m_pCmdMgr->UnRegister("NS_OpenMall",		(NETMSGPROC)m_Trunk.sfp2(&MallMgr::OnNS_OpenMall));
	m_pCmdMgr->UnRegister("NS_MallItem",		(NETMSGPROC)m_Trunk.sfp2(&MallMgr::OnNS_MallItem));
	m_pCmdMgr->UnRegister("NS_MallFreeItem",	(NETMSGPROC)m_Trunk.sfp2(&MallMgr::OnNS_MallFreeItem));
	m_pCmdMgr->UnRegister("NS_MallPack",		(NETMSGPROC)m_Trunk.sfp2(&MallMgr::OnNS_MallPack));
	m_pCmdMgr->UnRegister("NS_MallUpdateItem",	(NETMSGPROC)m_Trunk.sfp2(&MallMgr::OnNS_MallUpdateItem));
	m_pCmdMgr->UnRegister("NS_MallUpdatePack",	(NETMSGPROC)m_Trunk.sfp2(&MallMgr::OnNS_MallUpdatePack));
	m_pCmdMgr->UnRegister("NS_MallUpdate",		(NETMSGPROC)m_Trunk.sfp2(&MallMgr::OnNS_MallUpdate));
	m_pCmdMgr->UnRegister("NS_GetSimGPInfo",		(NETMSGPROC)m_Trunk.sfp2(&MallMgr::OnNS_GetSimGPInfo));

	m_pMallFrame = NULL;
	ClearMallItem();
	ClearMallPack();
	ClearGroupBuyInfo();
}

void MallMgr::ClearMallItem()
{
	map<DWORD, tagMallItemProto>::iterator it=m_mapMallItemEx.begin();
	while(it!=m_mapMallItemEx.end())
	{
		it = m_mapMallItemEx.erase(it);
	}
	m_mapMallItemEx.clear();
	m_mapMallItem.Clear();
}

void MallMgr::ClearMallPack()
{
	map<DWORD, tagMallPackProto>::iterator it=m_mapPackItemEx.begin();
	while(it!=m_mapPackItemEx.end())
	{
		it = m_mapPackItemEx.erase(it);
	}
	m_mapPackItemEx.clear();
	m_mapPackItem.Clear();
}

void MallMgr::ClearGroupBuyInfo()
{
	map<INT64, tagGroupPurchase>::iterator it = m_mapGgroupPur.begin();
	while( it!=m_mapGgroupPur.end() )
	{
		it = m_mapGgroupPur.erase(it);
	}
	m_mapGgroupPur.clear();
}

// 打开商城界面
DWORD MallMgr::OnOpen_Mall(tagGameEvent* pEvent)
{
	if( P_VALID(m_pMallFrame) )
	{
		if( m_mapMallItemEx.size()!=0
			|| m_mapPackItemEx.size()!=0)
		{
			tagNC_MallUpdate msg;
			msg.dwMallTime = m_dwMallTime;
			m_pSession->Send(&msg);
		}
		else
		{
			tagNC_MallGet msg;
			m_pSession->Send(&msg);
		}
		m_bOpen = TRUE;
	}
	return 0;
}

// 关闭商城界面
DWORD MallMgr::OnClose_Mall(tagGameEvent* pEvent)
{
	if( P_VALID(m_pMallFrame) )
	{
		m_pMallFrame->ShowWnd(FALSE);
		m_bOpen	 = FALSE;
	}
	return 0;
}

tagMallItemProto* MallMgr::GetMallItemByID( DWORD dwID )
{
	return m_mapMallItem.Peek(dwID);
}

tagMallPackProto* MallMgr::GetMallPackByID(DWORD dwID)
{
	return m_mapPackItem.Peek(dwID);
}

DWORD MallMgr::OnNS_OpenMall( tagNS_OpenMall* pMsg, DWORD dwParam )
{
	if(pMsg->dwErrorCode == E_Success)
	{
		if( !P_VALID(m_pMallFrame) )
		{
			CombatSysUtil::Inst()->ShowScreenCenterMsg(_T("mall create fail!\n"));
			return 0;
		}

		m_dwMallTime = pMsg->dwMallTime;
		m_pMallFrame->ClassifiedMallItem();
		m_pMallFrame->UpdateAvatar();
		m_pMallFrame->ShowWnd(m_bOpen);

	}
	else
	{
		CombatSysUtil::Inst()->ShowScreenCenterMsg(_T("mall open fail!\n"));
	}
	return 0;
}

DWORD MallMgr::OnNS_MallItem( tagNS_MallItem* pMsg, DWORD dwParam )
{
	ClearMallItem();
	tagMallItemProto* pItem = (tagMallItemProto*)pMsg->byData;
	for(int i=0; i<pMsg->nItemNum; i++)
	{
		const tagItemProto* pItemProto = ItemProtoData::Inst()->FindItemProto(pItem[i].dwTypeID);
		if( P_VALID(pItemProto) )
		{
			m_mapMallItemEx.insert(make_pair(pItem[i].dwID, pItem[i]));
		}
	}
	map<DWORD, tagMallItemProto>::iterator it = m_mapMallItemEx.begin();
	for( ; it!=m_mapMallItemEx.end(); ++it )
	{
		m_mapMallItem.Add(it->first, &(it->second));
		if( it->second.byRank!=0 && it->second.byRank<MAX_PromoteItem+1 )
		{
			m_pProItem[it->second.byRank-1] = &(it->second);
		}
	}
	return 0;
}

DWORD MallMgr::OnNS_MallFreeItem( tagNS_MallFreeItem* pMsg, DWORD dwParam )
{
	m_MallFreeItem = pMsg->freeItem;
	return 0;
}

DWORD MallMgr::OnNS_MallPack( tagNS_MallPack* pMsg, DWORD dwParam )
{
	ClearMallPack();
	tagMallPackProto* pPack = (tagMallPackProto*)pMsg->byData;
	for(int i=0; i<pMsg->nPackNum; i++)
	{
		m_mapPackItemEx.insert(make_pair(pPack[i].dwID, pPack[i]));
	}
	map<DWORD, tagMallPackProto>::iterator it = m_mapPackItemEx.begin();
	for( ; it!=m_mapPackItemEx.end(); ++it )
	{
		m_mapPackItem.Add(it->first, &(it->second));
	}
	return 0;
}

DWORD MallMgr::OnNS_MallUpdate( tagNS_MallUpdate* pMsg, DWORD dwParam )
{
	m_dwMallTime = pMsg->dwMallTime;
	return 0;
}

DWORD MallMgr::OnNS_MallUpdateItem( tagNS_MallUpdateItem* pMsg, DWORD dwParam )
{
	tagMallUpdate* pUdateItem = (tagMallUpdate*)pMsg->byData;
	for(int i=0; i<pMsg->nItemNum; i++)
	{
		map<DWORD, tagMallItemProto>::iterator it = m_mapMallItemEx.find(pUdateItem[i].dwID);
		if( it!=m_mapMallItemEx.end() )
		{
			it->second.byNum = pUdateItem[i].byRemainNum;
		}
	}
	return 0;
}

DWORD MallMgr::OnNS_MallUpdatePack( tagNS_MallUpdatePack* pMsg, DWORD dwParam )
{
	tagMallUpdate* pUdatePack = (tagMallUpdate*)pMsg->byData;
	for(int i=0; i<pMsg->nItemNum; i++)
	{
		map<DWORD, tagMallPackProto>::iterator it = m_mapPackItemEx.find(pUdatePack[i].dwID);
		if( it!=m_mapPackItemEx.end() )
		{
			it->second.byNum = pUdatePack[i].byRemainNum;
		}
	}
	return 0;
}

tagMallItemProto* MallMgr::GetMallPromoteItem(int nIndex)
{
	if( nIndex>=0 && nIndex<MAX_PromoteItem )
		return m_pProItem[nIndex];
	else
		return NULL;
}

void MallMgr::CreateMallGiftFrame()
{
	MallGiftFrame *pFrame = (MallGiftFrame*)m_pFrameMgr->GetFrame(_T("MallGift"));
	if( !P_VALID(pFrame) )
	{
		pFrame = (MallGiftFrame*)m_pFrameMgr->CreateFrame( _T("World"), _T("MallGift"), _T("MallGiftFrame"), 0);
		if( P_VALID(pFrame) )
			pFrame->SetBuyNum();
	}
	else 
	{
		pFrame->ShowGiftFrame();
	}
}

void MallMgr::SetMallGiftFrameMaxBuyNum(INT nMaxBuyNum)
{
	MallGiftFrame *pFrame = (MallGiftFrame*)m_pFrameMgr->GetFrame(_T("MallGift"));
	if( P_VALID(pFrame) )
	{
		pFrame->SetMaxBuyNum(nMaxBuyNum);
	}
}

INT16 MallMgr::GetEquipDisplayPos(INT16 n16EquipPos)
{
	// 饰品不需要显示
	if(EEP_Finger1 == n16EquipPos || EEP_Finger2 == n16EquipPos
		|| EEP_Waist == n16EquipPos || EEP_Neck == n16EquipPos)
	{
		return GT_INVALID;
	}

	// 转换为显示位置
	switch(n16EquipPos)
	{
		// 武器
	case EEP_RightHand:
		return EAE_RWeapon;
		break;
	case EEP_LeftHand:
		return EAE_LWeapon;
		break;

		// 时装
	case EEP_FashionHead:
		return EAE_Head;
		break;
	case EEP_FashionFace:
		return EAE_Face;
		break;
	case EEP_FashionBody:
		return EAE_Upper;
		break;
	case EEP_FashionLegs:
		return EAE_Lower;
		break;
	case EEP_FashionBack:
		return EAE_Back;
		break;
	case EEP_FashionWrist:
		return EAE_Glove;
		break;
	case EEP_FashionFeet:
		return EAE_Boot;
		break;

		// 防具
	case EEP_Head:
		return EAE_Head;
		break;
	case EEP_Face:
		return EAE_Face;
		break;
	case EEP_Body:
		return EAE_Upper;
		break;
	case EEP_Legs:
		return EAE_Lower;
		break;
	case EEP_Back:
		return EAE_Back;
		break;
	case EEP_Wrist:
		return EAE_Glove;
		break;
	case EEP_Feet:
		return EAE_Boot;
		break;
	}
	return GT_INVALID;
}

void MallMgr::CreateYuanBaoFrame(LPCTSTR szFatherName)
{
    YuanbaoFrame* pFrame = (YuanbaoFrame*)m_pFrameMgr->GetFrame(_T("YuanbaoFrame"));
    if (!P_VALID(pFrame))
    {
        pFrame = (YuanbaoFrame*)m_pFrameMgr->CreateFrame(szFatherName, _T("YuanbaoFrame"), _T("YuanbaoFrame"), 0);
        if (P_VALID(pFrame))
            m_pYuanbaoFrame = pFrame;
        else
            IMSG(_T("Create YuanbaoFrame failed"));
    }
    else
    {
        m_pYuanbaoFrame = pFrame;
    }
}

void MallMgr::CreateGroupBuyFrame( LPCTSTR szFatherName )
{
	GroupBuyFrame* pFrame = (GroupBuyFrame*)m_pFrameMgr->GetFrame(_T("GroupBuy"));
	if (!P_VALID(pFrame))
	{
		pFrame = (GroupBuyFrame*)m_pFrameMgr->CreateFrame(szFatherName, _T("GroupBuy"), _T("GroupBuyFrame"), 0);
		if (P_VALID(pFrame))
			m_pGroupBuyFrame = pFrame;
		else
			IMSG(_T("Create GroupBuyFrame failed"));
	}
	else
	{
		m_pGroupBuyFrame = pFrame;
	}
}

void MallMgr::SendGetGroupBuyInfo()
{
	tagNC_GetSimGPInfo msg;
	m_pSession->Send(&msg);
}

DWORD MallMgr::OnNS_GetSimGPInfo( tagNS_GetSimGPInfo* pMsg, DWORD dwParam )
{
	switch( pMsg->dwErrorCode )
	{
	case E_Success:
		{
			m_nGroupPurExp = pMsg->nGroupPurchase;
			INT nNum = pMsg->nGPInfoNum;
			for( int i=0; i<nNum; ++i )
			{	
				tagSimGPInfo* pInfo = &pMsg->simData[i];
				if( (INT)(pInfo->dwRemainTime)<=0 || pInfo->dwRemainTime==GT_INVALID )
					continue;
				tagGroupPurchase gp;
				gp.dwGuildID			= pInfo->dwGuildID;	
				gp.dwRoleID				= pInfo->dwRoleID;
				gp.dwMallID				= pInfo->dwMallID;
				gp.nPrice				= pInfo->nPrice;
				gp.nParticipatorNum		= pInfo->nParticipatorNum;
				gp.nRequireNum			= pInfo->nRequireNum;
				gp.dwRemainTime			= pInfo->dwRemainTime;
				INT64 n64temp = gp.dwRoleID;
				INT64 n64Key = (n64temp << 32) | gp.dwMallID;
				m_mapGgroupPur.insert(make_pair(n64Key, gp));				
			}
			if( P_VALID(m_pGroupBuyFrame) )
			{
				m_pGroupBuyFrame->RefreshGroupBuyInfo();
			}
		}
		break;
	}
	return 0;
}

tagGroupPurchase* MallMgr::GetGroupBuyInfoByID( INT64 dwID )
{
	map<INT64, tagGroupPurchase>::iterator it =m_mapGgroupPur.find(dwID);
	
	if( it!=m_mapGgroupPur.end() )
	{
		return &(it->second);
	}

	return NULL;
}

void MallMgr::DeleGroupBuyInfoByID(INT64 dwID)
{
	map<INT64, tagGroupPurchase>::iterator it =m_mapGgroupPur.find(dwID);

	if( it!=m_mapGgroupPur.end() )
	{
		m_mapGgroupPur.erase(it);
	}
}

void MallMgr::RefreshGroupBuyInfoByID( INT64 dwID, INT nNum )
{
	map<INT64, tagGroupPurchase>::iterator it =m_mapGgroupPur.find(dwID);

	if( it!=m_mapGgroupPur.end() )
	{
		it->second.nParticipatorNum = nNum;
	}
}