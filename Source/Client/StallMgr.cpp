#include "StdAfx.h"
#include "StallMgr.h"
#include "MapMgr.h"
#include "ItemMgr.h"
#include "Container.h"
#include "RoleMgr.h"
#include "LocalPlayer.h"
#include "WorldPickerFrame.h"
#include "ItemFrame.h"
#include "CombatSysUtil.h"
#include "..\WorldDefine\msg_stall.h"
#include "..\WorldDefine\exchange_define.h"
#include "StallShelfFrame.h"
#include "PlayerNameTab.h"
#include "StallSignboardTipsFrame.h"

const tstring STAR_ICON		= _T("<pic=data\\ui\\bag\\L_baitan-lv1.bmp,0,0,16,16>");
const tstring MOON_ICON		= _T("<pic=data\\ui\\bag\\L_baitan-lv2.bmp,0,0,16,16>");
const tstring SUN_ICON		= _T("<pic=data\\ui\\bag\\L_baitan-lv3.bmp,0,0,16,16>");
const tstring HEART_ICON	= _T("<pic=data\\ui\\bag\\L_baitan-lv4.bmp,0,0,16,16>");
const tstring DIAMOND_ICON	= _T("<pic=data\\ui\\bag\\L_baitan-lv5.bmp,0,0,16,16>");
const tstring CROWN_ICON	= _T("<pic=data\\ui\\bag\\L_baitan-lv6.bmp,0,0,16,16>");

StallMgr::StallMgr(void) : m_Trunk(this)
{
	m_bSetAdFlag = FALSE;
	m_eOperateStallType = EOperateStallType_NULL;
}

StallMgr::~StallMgr(void)
{
}
//-----------------------------------------------------------------------------
// Init
//-----------------------------------------------------------------------------
VOID StallMgr::Init()
{
	m_pUtil		= TObjRef<Util>();
	m_pCmdMgr	= TObjRef<NetCmdMgr>();
	m_pSession	= TObjRef<NetSession>();
	m_pFrameMgr	= TObjRef<GameFrameMgr>();
	m_dwStallRoleID	= GT_INVALID;
	m_dwVipTime	 = GT_INVALID;
	
	m_pContainerStall = new TemporaryContainer(STALL_MAX_DISPLAY);
	
	m_pLp = RoleMgr::Inst()->GetLocalPlayer();
	ASSERT(P_VALID(m_pLp));

	m_pCmdMgr->Register("NS_StallStart",		(NETMSGPROC)m_Trunk.sfp2(&StallMgr::OnNetStallStart),		_T("NS_StallStart"));
	m_pCmdMgr->Register("NS_StallClose",		(NETMSGPROC)m_Trunk.sfp2(&StallMgr::OnNetStallClose),		_T("NS_StallClose"));
	m_pCmdMgr->Register("NS_StallGetTitle",		(NETMSGPROC)m_Trunk.sfp2(&StallMgr::OnNetStallGetTitle),	_T("NS_StallGetTitle"));
	m_pCmdMgr->Register("NS_StallBuyRefresh",	(NETMSGPROC)m_Trunk.sfp2(&StallMgr::OnNetStallBuyRefresh),	_T("NS_StallBuyRefresh"));
	m_pCmdMgr->Register("NS_GetAllVIPStallInfo",(NETMSGPROC)m_Trunk.sfp2(&StallMgr::OnNetGetAllVIPStallInfo),_T("NS_GetAllVIPStallInfo"));
	m_pCmdMgr->Register("NS_UpdateVIPStallInfo",(NETMSGPROC)m_Trunk.sfp2(&StallMgr::OnNetUpdateVIPStallInfo),_T("NS_UpdateVIPStallInfo"));
	m_pCmdMgr->Register("NS_StallSetFinish",	(NETMSGPROC)m_Trunk.sfp2(&StallMgr::OnNetStallSetFinish),	_T("NS_StallSetFinish"));

	m_pFrameMgr->RegisterEventHandle(_T("StallEvent_StallAppear"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&StallMgr::OnEventStallAppear));
	m_pFrameMgr->RegisterEventHandle(_T("StallEvent_StallDisappear"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&StallMgr::OnEventStallDisappear));
	m_pFrameMgr->RegisterEventHandle(_T("WorldEvent_StallPick"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&StallMgr::OnEventPickStall));
	m_pFrameMgr->RegisterEventHandle(_T("StallEvent_OpenWindow"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&StallMgr::OnEventOpenStall));
	m_pFrameMgr->RegisterEventHandle(_T("StallEvent_CloseWindow"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&StallMgr::OnEventCloseStall));
	m_pFrameMgr->RegisterEventHandle(_T("StallEvent_CloseVend"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&StallMgr::OnEventCloseVend));
	m_pFrameMgr->RegisterEventHandle(_T("OpenVipStall"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&StallMgr::OnEventOpenVipStall));
	m_pFrameMgr->RegisterEventHandle(_T("tagVipStallEvent"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&StallMgr::OnCheckVipStallEvent));
    m_pFrameMgr->RegisterEventHandle(_T("tagRoleGetNameEvent"),         (FRAMEEVENTPROC)m_Trunk.sfp1(&StallMgr::OnGetPlayerName));

	tstring strConfigPath = _T("data\\system\\attdata\\BT.xml");
	LoadStallModelConfig(strConfigPath);

}
//-----------------------------------------------------------------------------
// Destroy
//-----------------------------------------------------------------------------
VOID StallMgr::Destroy()
{
	m_pCmdMgr->UnRegister("NS_StallStart",		(NETMSGPROC)m_Trunk.sfp2(&StallMgr::OnNetStallStart));
	m_pCmdMgr->UnRegister("NS_StallClose",		(NETMSGPROC)m_Trunk.sfp2(&StallMgr::OnNetStallClose));
	m_pCmdMgr->UnRegister("NS_StallGetTitle",	(NETMSGPROC)m_Trunk.sfp2(&StallMgr::OnNetStallGetTitle));
	m_pCmdMgr->UnRegister("NS_StallBuyRefresh",	(NETMSGPROC)m_Trunk.sfp2(&StallMgr::OnNetStallBuyRefresh));
	m_pCmdMgr->UnRegister("NS_GetAllVIPStallInfo",(NETMSGPROC)m_Trunk.sfp2(&StallMgr::OnNetGetAllVIPStallInfo));
	m_pCmdMgr->UnRegister("NS_UpdateVIPStallInfo",(NETMSGPROC)m_Trunk.sfp2(&StallMgr::OnNetUpdateVIPStallInfo));
	m_pCmdMgr->UnRegister("NS_StallSetFinish",	(NETMSGPROC)m_Trunk.sfp2(&StallMgr::OnNetStallSetFinish));

	m_pFrameMgr->UnRegisterEventHandler(_T("StallEvent_StallAppear"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&StallMgr::OnEventStallAppear));
	m_pFrameMgr->UnRegisterEventHandler(_T("StallEvent_StallDisappear"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&StallMgr::OnEventStallDisappear));
	m_pFrameMgr->UnRegisterEventHandler(_T("WorldEvent_StallPick"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&StallMgr::OnEventPickStall));
	m_pFrameMgr->UnRegisterEventHandler(_T("StallEvent_OpenWindow"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&StallMgr::OnEventOpenStall));
	m_pFrameMgr->UnRegisterEventHandler(_T("StallEvent_CloseWindow"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&StallMgr::OnEventCloseStall));
	m_pFrameMgr->UnRegisterEventHandler(_T("StallEvent_CloseVend"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&StallMgr::OnEventCloseVend));
	m_pFrameMgr->UnRegisterEventHandler(_T("OpenVipStall"),					(FRAMEEVENTPROC)m_Trunk.sfp1(&StallMgr::OnEventOpenVipStall));
	m_pFrameMgr->UnRegisterEventHandler(_T("tagVipStallEvent"),				(FRAMEEVENTPROC)m_Trunk.sfp1(&StallMgr::OnCheckVipStallEvent));
    m_pFrameMgr->UnRegisterEventHandler(_T("tagRoleGetNameEvent"),          (FRAMEEVENTPROC)m_Trunk.sfp1(&StallMgr::OnGetPlayerName));

	if( P_VALID(GetObj("OpenStallShelfError_MsgBox")) )
		KillObj("OpenStallShelfError_MsgBox");

	// 清除数据
	m_mapStall.ResetIterator();
	tagStall* pStall = NULL;
	while(m_mapStall.PeekNext(pStall))
		SAFE_DEL(pStall);
	m_mapStall.Clear();

	m_mapVipStall.clear();

	if (P_VALID(m_pContainerStall))
	{
		m_pContainerStall->Destroy();
		SAFE_DEL(m_pContainerStall);
	}

	m_strLPStallName = _T("");

}

StallMgr g_stallmgr;
StallMgr* StallMgr::Inst()
{
	return &g_stallmgr;
}

//-----------------------------------------------------------------------------
// 处理摊位显示事件
//-----------------------------------------------------------------------------
DWORD StallMgr::OnEventStallAppear( tagStallAppearEvent* pGameEvent )
{
	tagStall* pStall = m_mapStall.Peek(pGameEvent->dwRoleID);
	if( !P_VALID(pStall) )
	{
		pStall = new tagStall;
		if( P_VALID(pStall) )
			m_mapStall.Add(pGameEvent->dwRoleID, pStall);
	}
	
	tagStallStarEvent StallEvent(_T("StallStar"), NULL);
	StallEvent.nLevel = pGameEvent->byStallLevel;
	m_pFrameMgr->SendEvent(&StallEvent);

	if( P_VALID(pStall) )
	{
		pStall->dwRoleID = pGameEvent->dwRoleID;
		Vector3 vDir(0, 0, 0); 
		Yaw2Dir(pGameEvent->fYaw, vDir);
		pStall->space.SetPos(pGameEvent->vPos + vDir * 2 * TILE_SCALE);
		pStall->space.SetYaw(pGameEvent->fYaw);
		pStall->byLevel = pGameEvent->byStallLevel;
		
		// 显示出来
		if( FALSE == this->ShowStall(pStall) )
		{
			SAFE_DEL(pStall);
		}
		else 
		{
			if(pStall->dwRoleID != m_pLp->GetID())
			{
				SendToGetStallTitle(pStall->dwRoleID);
			}
			else
			{
				_tcsncpy(pStall->szName, m_strLPStallName.c_str(), sizeof(pStall->szName)/sizeof(TCHAR));
			}
		}
	}
	return 0;
}

//-----------------------------------------------------------------------------
// 处理摊位消失事件
//-----------------------------------------------------------------------------
DWORD StallMgr::OnEventStallDisappear( tagStallAppearEvent* pGameEvent )
{
	tagStall* pStall = m_mapStall.Peek(pGameEvent->dwRoleID);
	if( P_VALID(pStall) )
	{
		// 隐藏
		this->HideStall(pStall);
		StallSignboardTipsFrame *pFrame = (StallSignboardTipsFrame*)m_pFrameMgr->GetFrame(_T("StallSignboard"));
		if (P_VALID(pFrame))
			pFrame->ReleaseTexture(pStall);
		
		// 删除列表
		m_mapStall.Erase(pGameEvent->dwRoleID);
		SAFE_DEL(pStall);
	}
	return 0;
}

//-----------------------------------------------------------------------------
// 显示摊位
//-----------------------------------------------------------------------------
BOOL StallMgr::ShowStall( tagStall* pStall )
{
	SceneGraph* pScene = MapMgr::Inst()->GetSceneGraph();
	if( !P_VALID(pStall->pNode) )
		pStall->pNode = new EntityNode;

	if( !P_VALID(pStall->pNode) )
	{
		ASSERT(0);
		return FALSE;
	}

	tstring strMod = GetStallModelPath(pStall->byLevel);

	pStall->pNode->LoadRes(strMod.c_str());
	pStall->pNode->SetWorldMat(pStall->space.GetMatrix());
	pScene->AttachDynamicNode(pStall->pNode);
	return TRUE;
}
//-----------------------------------------------------------------------------
// 隐藏摊位
//-----------------------------------------------------------------------------
VOID StallMgr::HideStall( tagStall* pStall )
{
	if( !P_VALID(pStall->pNode) )
	{
		ASSERT(0);
		return;
	}

	SceneGraph* pScene = MapMgr::Inst()->GetSceneGraph();
	if (P_VALID(pScene))
	{
		pScene->DetachDynamicNode(pStall->pNode);
		SAFE_DEL(pStall->pNode);
	}
}

//-----------------------------------------------------------------------------
// 射线检测
//-----------------------------------------------------------------------------
tagStall* StallMgr::PickStall(const Ray& rayMouse)
{
	tagStall* pStall = NULL;
	tagStall* pNearestStall = NULL;
	float fMinDistSq = FLOAT_MAX;

	m_mapStall.ResetIterator();
	while(m_mapStall.PeekNext(pStall))
	{
		if( !P_VALID(pStall->pNode) )
			continue;

		AABBox box;
		pStall->pNode->GetBox(box);
		TResult tr = rayMouse.Intersect(box);
		if(tr.first)
		{
			float fDistSq = Vec3DistSq( rayMouse.origin, pStall->space.GetPos() );
			if( fDistSq < fMinDistSq )
			{
				fMinDistSq = fDistSq;
				pNearestStall = pStall;
			}
		}
	}

	return pNearestStall;
}


VOID StallMgr::InitStallContainer()
{
	m_pContainerStall->Destroy();
	
	INT64 n64ItemId = 0;
	INT64 n64Price = 0;
	INT16 n16Pos = 0;

	m_mapStallItem.ResetIterator();
	while (m_mapStallItem.PeekNext(n64ItemId, n64Price))
	{
		Item* pItem = ItemMgr::Inst()->GetPocketItemByID(n64ItemId);
		BOOL bRet = AddItem(pItem, n16Pos, n64Price);
		
		if (bRet)
			++n16Pos;
	}
	m_mapStallItem.Clear();
}


VOID StallMgr::RecordStallList()
{
	m_mapStallItem.Clear();
	
	TMap<INT64, tagTemporaryItem*> mapContainer;
	m_pContainerStall->Clear(mapContainer);

	INT64 n64ItemId = 0;
	tagTemporaryItem* pTemp = NULL;

	mapContainer.ResetIterator();
	while (mapContainer.PeekNext(n64ItemId, pTemp))
	{
		if (P_VALID(pTemp))
		{
			pTemp->pItem->SetOperable(true);
			m_mapStallItem.Add(n64ItemId, pTemp->n64Price);
			SAFE_DEL(pTemp);
		}
	}
}

BOOL StallMgr::AddItem( Item* pItem, INT16 &nIndex, INT64 n64Price )
{
	BOOL bRet = FALSE;
	if (P_VALID(pItem) && pItem->IsItem())
	{
		// 如果未定义位置，则找空位
		if (!GT_VALID(nIndex))
			nIndex = m_pContainerStall->GetBlank();
		// 如果没找到空位，返回
		if (!GT_VALID(nIndex))
			return FALSE;

		tagTemporaryItem* pTemp = new tagTemporaryItem(pItem, nIndex);
		if (!m_pContainerStall->IsExist(nIndex))
		{
			pTemp->n64Price = n64Price;
			bRet = m_pContainerStall->Add(pTemp);
			pItem->SetOperable(false);
		}
	}
	return bRet;
}

BOOL StallMgr::DelItem( INT16 nIndex, BOOL bUnlock )
{
	BOOL bRet = FALSE;
	tagTemporaryItem* pTemp = m_pContainerStall->Remove(nIndex);
	if (P_VALID(pTemp))
	{
		//如果需要解锁
		if(bUnlock)
		{
			if (pTemp->bShelf)
			{
				m_pContainerStall->Add(pTemp);
				return bRet;
			}
			
			pTemp->pItem->SetOperable(true);
		}
		bRet = TRUE;
		SAFE_DEL(pTemp);
	}
	return bRet;
}

DWORD StallMgr::OnNetStallStart( tagNS_StallStart* pNetCmd, DWORD )
{
	if (pNetCmd->dwErrorCode == E_Success)
	{
		InitStallContainer();
		
		OpenStallShelf();
	}
	else
	{
		ShowStallErrorMsg(pNetCmd->dwErrorCode);
	}
	return 0;
}

DWORD StallMgr::OnNetStallClose( tagNS_StallClose* pNetCmd, DWORD )
{
	if (pNetCmd->dwErrorCode == E_Success)
	{
		if (pNetCmd->dwStallRoleID == m_pLp->GetID())
		{
			CloseStallShelf();
			
			RecordStallList();

			m_strLPStallName = _T("");
			m_bSetAdFlag	 = FALSE;

			if( P_VALID(m_pLp) )
				RefreshVipStallState(m_pLp->GetID());
		}
		else if (pNetCmd->dwStallRoleID == m_dwStallRoleID)
		{
			CloseStallVend();

			RefreshVipStallState(m_dwStallRoleID);

			m_dwStallRoleID = GT_INVALID;
		}
		Player *pPlayer = (Player*)RoleMgr::Inst()->FindRole(pNetCmd->dwStallRoleID);
		if (P_VALID(pPlayer))
		{
			pPlayer->SendEventStallAppear(false);
		}

	}
	else
	{
		ShowStallErrorMsg(pNetCmd->dwErrorCode);
	}
	return 0;
}

VOID StallMgr::ShowStallErrorMsg( DWORD dwErrorCode )
{
	if(GT_VALID(dwErrorCode))
	{
		TCHAR szTmp[X_LONG_NAME];
		_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), _T("StallErrorMsg_%d"), dwErrorCode);
		CombatSysUtil::Inst()->ShowScreenCenterMsg(g_StrTable[szTmp]);
	}
}

tagTemporaryItem* StallMgr::GetItem( INT16 nIndex )
{
	if (P_VALID(m_pContainerStall))
	{
		return m_pContainerStall->GetValue(nIndex); 
	}
	return NULL;
}

DWORD StallMgr::OnEventPickStall( tagRolePickEvent* pGameEvent )
{
	if (P_VALID(m_pLp) && pGameEvent->dwRoleID == m_pLp->GetID())
	{
		// 创建摊位设置界面
		OnEventOpenStall(&tagGameEvent(_T("StallEvent_OpenWindow"), NULL));
		tagStall* pStall = m_mapStall.Peek(pGameEvent->dwRoleID);
		if (P_VALID(pStall))
		{
			tagStallStarEvent StallEvent(_T("StallStar"), NULL);
			StallEvent.nLevel = pStall->byLevel;
			m_pFrameMgr->SendEvent(&StallEvent);
		}
	}
	else
	{
		// 记录当前摊位设置
		m_dwStallRoleID = pGameEvent->dwRoleID;
		m_eOperateStallType = EOperateStallType_Normal;
		if (IsOutValidDist())
		{
			m_dwStallRoleID = GT_INVALID;
			return 0;
		}

		OpenStallVend();
		tagStall* pStall = m_mapStall.Peek(pGameEvent->dwRoleID);
		if (P_VALID(pStall))
		{
			tagStallStarEvent StallEvent(_T("StallStar"), NULL);
			StallEvent.nLevel = pStall->byLevel;
			m_pFrameMgr->SendEvent(&StallEvent);
		}

		tagNC_StallGet m;
		m.dwStallRoleID = pGameEvent->dwRoleID;
		m_pSession->Send(&m);
		
	}
	return 0;
}

BOOL StallMgr::IsStallState()
{
	if (P_VALID(m_pLp))
	{
		return m_pLp->GetRoleState(ERS_Stall);
	}
	return FALSE;
}

DWORD StallMgr::OnEventOpenStall( tagGameEvent* pGameEvent )
{
	if (m_pLp->GetRoleState(ERS_Stall))
	{
		OpenStallShelf();
	}
	else
	{
		// 如果没有名字，则设置为默认摊位名
		if (m_strLPStallName==_T(""))
		{
			TCHAR szTmp[X_SHORT_NAME] = {0};
			_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), g_StrTable[_T("StallMsg_DefaultName")], PlayerNameTab::Inst()->FindNameByID(m_pLp->GetID()));
			m_strLPStallName = szTmp;
		}

		tagNC_StallStart m;
		m_pSession->Send(&m);
	}
	return 0;
}

DWORD StallMgr::OnEventCloseStall( tagGameEvent* pGameEvent )
{
	if (m_pLp->GetRoleState(ERS_Stall))
	{
		CloseStallShelf();
	}
	else
	{
		tagNC_StallClose m;
		m_pSession->Send(&m);
	}
	return 0;
}


DWORD StallMgr::OnEventCloseVend( tagGameEvent* pGameEvent )
{
	CloseStallVend();
	m_dwStallRoleID = GT_INVALID;

	return 0;
}

VOID StallMgr::OpenStallShelf()
{
	GameFrame* pFrame = m_pFrameMgr->GetFrame(_T("StallShelf"));
	if (!P_VALID(pFrame))
	{
		m_pFrameMgr->CreateFrame(_T("Trade"), _T("StallShelf"), _T("StallShelfFrame"), 0);
		SetStrategy(EIST_ItemStall);
	}
}

VOID StallMgr::CloseStallShelf()
{
	GameFrame* pFrame = m_pFrameMgr->GetFrame(_T("StallShelf"));
	if (P_VALID(pFrame))
	{
		m_pFrameMgr->AddToDestroyList(pFrame);
	}
	SetStrategy(EIST_ItemPocket, EIST_ItemStall);
}

VOID StallMgr::OpenStallVend()
{
	// 创建摊位购买界面
	GameFrame* pFrame = m_pFrameMgr->GetFrame(_T("StallVend"));
	if (P_VALID(pFrame))
	{
		m_pFrameMgr->AddToDestroyList(pFrame);
	}
	else
	{
		pFrame = m_pFrameMgr->CreateFrame(_T("Trade"), _T("StallVend"), _T("StallVendFrame"), 0);
	}
}

VOID StallMgr::CloseStallVend()
{
	GameFrame* pFrame = m_pFrameMgr->GetFrame(_T("StallVend"));
	if (P_VALID(pFrame))
	{
		m_pFrameMgr->AddToDestroyList(pFrame);
	}
}

BOOL StallMgr::IsOutValidDist()
{
	// vip摊位打开方式不作距离检查
	if( m_eOperateStallType==EOperateStallType_Vip )
		return FALSE;

	tagStall* pStall = m_mapStall.Peek(m_dwStallRoleID);
	if (P_VALID(pStall) && P_VALID(m_pLp))
	{
		if( Vec3DistSq(pStall->space.GetPos(), m_pLp->GetPos()) > MAX_EXCHANGE_SPACE*MAX_EXCHANGE_SPACE )
			return TRUE;
		else
			return FALSE;
	}
	return TRUE;
}

BOOL StallMgr::IsAABBCollideStall( const AABBox &box )
{
	m_mapStall.ResetIterator();
	tagStall* pStall = NULL; 
	while (m_mapStall.PeekNext(pStall))
	{
		if(!P_VALID(pStall) || !P_VALID(pStall->pNode))
			continue;

		AABBox other;
		pStall->pNode->GetBox(other);
		if(box.Intersects(other))
			return TRUE;
	}
	return FALSE;
}



VOID StallMgr::GetStallAABBox( AABBox& out, Vector3& wpt, BYTE byLevel )
{
	if (P_VALID(m_pLp))
	{
		Vector3 vDir(0, 0, 0);
		wpt = m_pLp->GetPos() + vDir * 2 * TILE_SCALE;  
		Yaw2Dir(m_pLp->GetYaw(), vDir);
		 
		out.max.x = wpt.x + 4 * TILE_SCALE;
		out.max.y = wpt.y + 4 * TILE_SCALE;
		out.max.z = wpt.z + 4 * TILE_SCALE;
		out.min.x = wpt.x - 4 * TILE_SCALE;
		out.min.y = wpt.y - 4 * TILE_SCALE;
		out.min.z = wpt.z - 4 * TILE_SCALE;
	}
	
}

BOOL StallMgr::IsAABBCollideGround( const AABBox &box, Vector3 &wpt )
{
	NavCollider* pCollider = MapMgr::Inst()->GetNavMap()->GetCollider();
	if (P_VALID(m_pLp) && P_VALID(pCollider))
	{
		if(pCollider->AABBoxCollideBoxTopAndTrn(box,Kernel::Inst()->GetAccumTime(),wpt.y,NULL,NavSceneNode::EFlag_NotCarrier))
			return TRUE;
	}
	return FALSE;
}

BOOL StallMgr::CanStall(BYTE byLevel)
{
	if (!P_VALID(m_pLp))
		return FALSE;
	
	AABBox stallbox;
	Vector3 wpt;
	GetStallAABBox(stallbox, wpt, byLevel);

	if (!IsAABBCollideGround(stallbox, wpt))
		return FALSE;

	if (IsAABBCollideStall(stallbox))
		return FALSE;

	if (RoleMgr::Inst()->IsAABBoxCollideRole(stallbox))
		return FALSE;
	
	return TRUE;	
}

LPCTSTR StallMgr::GetStallName( DWORD dwRoleID )
{
	if (dwRoleID == m_pLp->GetID())
	{
		return m_strLPStallName.c_str();
	}

	tagStall* pStall = m_mapStall.Peek(dwRoleID);
	if (P_VALID(pStall))
	{
		return pStall->szName;
	}

	return _T("");
}

VOID StallMgr::SetStallName( DWORD dwRoleID, LPCTSTR szName, bool bCustom )
{
	tagStall* pStall = m_mapStall.Peek(dwRoleID);
	if (P_VALID(pStall))
	{
		_tcsncpy(pStall->szName, szName, sizeof(pStall->szName)/sizeof(TCHAR));
        pStall->bCustomName = bCustom;
	}
	// 本地玩家需要缓存起来
	if (m_pLp->GetID() == dwRoleID)
	{
		m_strLPStallName = szName;
	}
}

DWORD StallMgr::OnNetStallGetTitle( tagNS_StallGetTitle* pNetCmd, DWORD )
{
    bool bIsCustom; // 是否自定义摊位名
	TCHAR szTmp[X_SHORT_NAME] = {0};
	if (E_Success == pNetCmd->dwErrorCode)
	{
		if (pNetCmd->szTitle[0] == 0)
        {
			_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), g_StrTable[_T("StallMsg_DefaultName")], PlayerNameTab::Inst()->FindNameByID(pNetCmd->dwStallRoleID));
			bIsCustom = false;
        }
		else
        {
			_tcsncpy(szTmp, pNetCmd->szTitle, sizeof(szTmp)/sizeof(TCHAR));
            bIsCustom = true;
        }
	}
	else
	{
		_sntprintf(szTmp, sizeof(szTmp)/sizeof(TCHAR), g_StrTable[_T("StallMsg_DefaultName")], PlayerNameTab::Inst()->FindNameByID(pNetCmd->dwStallRoleID));
		/*ShowStallErrorMsg(pNetCmd->dwErrorCode);*/
	}

	if(pNetCmd->dwStallRoleID == m_pLp->GetID())
	{
		m_strLPStallName = szTmp;
	}

	SetStallName(pNetCmd->dwStallRoleID, szTmp, bIsCustom);
	
	return 0;
}

DWORD StallMgr::OnNetStallBuyRefresh( tagNS_StallBuyRefresh* pNetCmd, DWORD )
{

	// 如果是本地玩家则刷新摊位货架
	if (m_pLp->GetID() == pNetCmd->dwStallRoleID)
	{
		if(pNetCmd->n16Num > 0)
		{
			m_pContainerStall->GetValue(pNetCmd->byIndex);	
			
		}
		else
		{
			DelItem(pNetCmd->byIndex, FALSE);
		}
		
		StallShelfFrame* pStall = (StallShelfFrame*)m_pFrameMgr->GetFrame(_T("StallShelf"));
		if (P_VALID(pStall))
		{
			pStall->ReflashShelf(pNetCmd->byIndex);
		}
			
		
	}
	return 0;
}

tstring StallMgr::GetStallStars(INT64 nTotalTax)
{
	if(nTotalTax < MGold2Silver(2))			
	{
		return _T("");
	}											
	else if(nTotalTax < MGold2Silver(5))
	{				
		return STAR_ICON;
	}
	else if(nTotalTax < MGold2Silver(9))
	{				
		return STAR_ICON + STAR_ICON;
	}
	else if(nTotalTax < MGold2Silver(14))
	{				
		return STAR_ICON + STAR_ICON + STAR_ICON;
	}
	else if(nTotalTax < MGold2Silver(20))
	{				
		return STAR_ICON + STAR_ICON + STAR_ICON + STAR_ICON ;
	}
	else if(nTotalTax < MGold2Silver(30))
	{				
		return STAR_ICON + STAR_ICON + STAR_ICON + STAR_ICON + STAR_ICON;
	}
	else if(nTotalTax < MGold2Silver(50))
	{				
		return MOON_ICON;
	}
	else if(nTotalTax < MGold2Silver(80))
	{				
		return MOON_ICON + MOON_ICON;
	}
	else if(nTotalTax < MGold2Silver(120))
	{				
		return MOON_ICON + MOON_ICON + MOON_ICON;
	}
	else if(nTotalTax < MGold2Silver(170))
	{				
		return MOON_ICON + MOON_ICON + MOON_ICON + MOON_ICON ;
	}
	else if(nTotalTax < MGold2Silver(210))
	{				
		return MOON_ICON + MOON_ICON + MOON_ICON + MOON_ICON + MOON_ICON;
	}
	else if(nTotalTax < MGold2Silver(270))
	{				
		return SUN_ICON;
	}
	else if(nTotalTax < MGold2Silver(350))
	{				
		return SUN_ICON + SUN_ICON;
	}
	else if(nTotalTax < MGold2Silver(450))
	{				
		return SUN_ICON + SUN_ICON + SUN_ICON;
	}
	else if(nTotalTax < MGold2Silver(600))
	{				
		return SUN_ICON + SUN_ICON + SUN_ICON + SUN_ICON ;
	}
	else if(nTotalTax < MGold2Silver(700))
	{				
		return SUN_ICON + SUN_ICON + SUN_ICON + SUN_ICON + SUN_ICON;
	}
	else if(nTotalTax < MGold2Silver(800))
	{				
		return HEART_ICON;
	}
	else if(nTotalTax < MGold2Silver(1000))
	{				
		return HEART_ICON + HEART_ICON;
	}
	else if(nTotalTax < MGold2Silver(1500))
	{				
		return HEART_ICON + HEART_ICON + HEART_ICON;
	}
	else if(nTotalTax < MGold2Silver(2000))
	{				
		return HEART_ICON + HEART_ICON + HEART_ICON + HEART_ICON ;
	}
	else if(nTotalTax < MGold2Silver(2200))
	{				
		return HEART_ICON + HEART_ICON + HEART_ICON + HEART_ICON + HEART_ICON;
	}
	else if(nTotalTax < MGold2Silver(2400))
	{				
		return DIAMOND_ICON;
	}
	else if(nTotalTax < MGold2Silver(3000))
	{				
		return DIAMOND_ICON + DIAMOND_ICON;
	}
	else if(nTotalTax < MGold2Silver(4000))
	{				
		return DIAMOND_ICON + DIAMOND_ICON + DIAMOND_ICON;
	}
	else if(nTotalTax < MGold2Silver(5000))
	{				
		return DIAMOND_ICON + DIAMOND_ICON + DIAMOND_ICON + DIAMOND_ICON ;
	}
	else if(nTotalTax < MGold2Silver(8000))
	{				
		return DIAMOND_ICON + DIAMOND_ICON + DIAMOND_ICON + DIAMOND_ICON + DIAMOND_ICON;
	}
	else
	{
		return CROWN_ICON;
	}

	return _T("");
}

VOID StallMgr::SendToGetStallTitle( DWORD dwRoleID )
{
	tagNC_StallGetTitle m;
	m.dwStallRoleID = dwRoleID;
	m_pSession->Send(&m);
}

//---------------------------------------------------------------------------------------------------------------
// VIP摊位
//---------------------------------------------------------------------------------------------------------------

DWORD StallMgr::OnEventOpenVipStall( tagGameEvent* pGameEvent )
{
	// 创建VIP摊位界面
	GameFrame* pFrame = m_pFrameMgr->GetFrame(_T("StallVip"));
	if (P_VALID(pFrame))
	{
		m_pFrameMgr->AddToDestroyList(pFrame);
	}
	else
	{
		pFrame = m_pFrameMgr->CreateFrame(_T("World"), _T("StallVip"), _T("StallVipFrame"), 0);
		if( !P_VALID(pFrame) )
			pFrame = m_pFrameMgr->CreateFrame(_T("World"), _T("StallVip"), _T("StallVipFrame"), 0);

		//本地无数据发获取消息
		if( m_mapVipStall.empty() )
		{
			tagNC_GetAllVIPStallInfo msg;
			m_pSession->Send(&msg);
		}
		//本地有数据发更新消息
		else 
		{
			tagNC_UpdateVIPStallInfo msg;
			msg.dwRequestTime = m_dwVipTime;
			m_pSession->Send(&msg);
		}
	}
	return 0;
}

BOOL StallMgr::IsCanCheckVipStall( BYTE byIndex )
{

	map<BYTE, tagVIPStall>::iterator it = m_mapVipStall.find(byIndex);
	if( it!=m_mapVipStall.end() )
	{
		if( it->second.eStallStatus == EVSS_Open )
			return TRUE;
	}

	return FALSE;
}

BOOL StallMgr::IsCanRentVipStall( BYTE byIndex )
{
	map<BYTE, tagVIPStall>::iterator it = m_mapVipStall.find(byIndex);
	if( it!=m_mapVipStall.end() )
	{
		if( it->second.eStallStatus == EVSS_ForHire )
			return TRUE;
	}

	return FALSE;
}

BOOL StallMgr::IsSelfVipStall()
{
	if( !P_VALID(m_pLp) )
		return TRUE;

	DWORD dwRoleID = m_pLp->GetID();
	map<BYTE, tagVIPStall>::iterator it = m_mapVipStall.begin();
	for( ; it!=m_mapVipStall.end(); ++it )
	{
		if( dwRoleID == it->second.dwOwnerID )
			return TRUE;
	}
	return FALSE;
}

DWORD StallMgr::OnCheckVipStallEvent( tagVipStallEvent* pGameEvent )
{
	if (P_VALID(m_pLp) && pGameEvent->dwRoleID == m_pLp->GetID())
	{
		if( P_VALID(GetObj("OpenStallShelfError_MsgBox")) )
			KillObj("OpenStallShelfError_MsgBox");
		CreateObj("OpenStallShelfError_MsgBox", "MsgBoxEx");

		TObjRef<MsgBoxEx>("OpenStallShelfError_MsgBox")->Init(_T(""), g_StrTable[_T("VipStall_Error0")], 
			_T("OpenStallShelfError_Event"), MsgBoxFlag(MBF_OK), TRUE);
		return 0;
	}
	else
	{
		// 记录当前摊位设置
		m_dwStallRoleID = pGameEvent->dwRoleID;
		m_eOperateStallType = EOperateStallType_Vip;
		
		OpenStallVend();

		tagNC_SpecVIPStallGet m;
		m.byIndex = pGameEvent->byIndex;
		m_pSession->Send(&m);

	}
	return 0;
}

DWORD StallMgr::OnNetGetAllVIPStallInfo( tagNS_GetAllVIPStallInfo* pNetCmd, DWORD )
{
	if( pNetCmd->dwErrorCode == E_Success )
	{
		m_dwVipTime = pNetCmd->dwRequestTime;
		for( int i=0; i<VIP_STALL_MAX_NUM; ++i )
		{
			m_mapVipStall.insert( make_pair(pNetCmd->VIPStallInfo[i].byStallIndex, pNetCmd->VIPStallInfo[i]) );
		}

		tagGameEvent evt(_T("AllVipStallInfoEvent"), NULL);
		m_pFrameMgr->SendEvent(&evt);
	}
	else 
	{
		// 创建摊位设置界面
		if( P_VALID(GetObj("OpenStallShelfError_MsgBox")) )
			KillObj("OpenStallShelfError_MsgBox");
		CreateObj("OpenStallShelfError_MsgBox", "MsgBoxEx");

		TObjRef<MsgBoxEx>("OpenStallShelfError_MsgBox")->Init(_T(""), g_StrTable[_T("VipStall_Error4")], 
			_T("OpenStallShelfError_Event"), MsgBoxFlag(MBF_OK), TRUE);

	}

	return 0;
}

tagVIPStall* StallMgr::FinVipStallInfo( BYTE byKey )
{
	map<BYTE, tagVIPStall>::iterator it = m_mapVipStall.find(byKey);
	if( it!=m_mapVipStall.end() )
		return &(it->second);
	return NULL;
}

DWORD StallMgr::OnNetUpdateVIPStallInfo( tagNS_UpdateVIPStallInfo* pNetCmd, DWORD )
{
	if( pNetCmd->dwErrorCode == E_Success )
	{
		map<BYTE, tagVIPStall>::iterator it;
		m_dwVipTime = pNetCmd->dwRequestTime;
		INT nInfoNum = pNetCmd->nInfoNum;
		for( INT i=0; i<nInfoNum; ++i )
		{
			it = m_mapVipStall.find( pNetCmd->VIPStallInfo[i].byStallIndex );
			if( it!=m_mapVipStall.end() )
			{
				it->second = pNetCmd->VIPStallInfo[i];
			}
		}

		tagGameEvent evt(_T("AllVipStallInfoEvent"), NULL);
		m_pFrameMgr->SendEvent(&evt);
	}

	return 0;
}

void StallMgr::RefreshVipStallState(DWORD dwRoleID)
{
	if( !IS_PLAYER(dwRoleID) )
		return; 

	map<BYTE, tagVIPStall>::iterator it = m_mapVipStall.begin();
	for( ; it!=m_mapVipStall.end(); ++it )
	{
		if( dwRoleID == it->second.dwOwnerID )
		{
			it->second.eStallStatus = EVSS_Close;
			tagGameEvent evt(_T("AllVipStallInfoEvent"), NULL);
			m_pFrameMgr->SendEvent(&evt);
			break;
		}
	}
	
}

DWORD StallMgr::OnGetPlayerName( tagRoleGetNameEvent* pGameEvent )
{
    tagStall* pStall = m_mapStall.Peek(pGameEvent->dwRoleID);
    if (P_VALID(pStall) && !pStall->bCustomName && pGameEvent->bResult)
    {
        _sntprintf(pStall->szName, sizeof(pStall->szName)/sizeof(TCHAR), g_StrTable[_T("StallMsg_DefaultName")], pGameEvent->strRoleName.c_str());
		// 本地玩家需要缓存起来
		if (m_pLp->GetID() == pGameEvent->dwRoleID)
		{
			m_strLPStallName = pStall->szName;
		}
    }

    return 0;
}

BOOL StallMgr::LoadStallModelConfig( tstring strConfigPath )
{

	m_mapStallModelConfigs.clear();
	list<tstring> cfList;
	TObjRef<VarContainer> con = CreateObj("ModelConfig", "VarContainer");
	if (!con->Load("VFS_System", strConfigPath.c_str(), "id", &cfList))
	{
		IMSG(_T("Load file bt.xml failed\r\n"));
		return FALSE;
	}

	list<tstring>::iterator end = cfList.end();
	for (list<tstring>::iterator iter = cfList.begin(); iter != end; ++iter)
	{
		tagStallModelConfig cf;
		cf.nStallLevel = con->GetInt(_T("level"), iter->c_str(), 1);
		cf.nModel = con->GetInt(_T("model"), iter->c_str(), 1);
		cf.nSignBoard = con->GetInt(_T("board"), iter->c_str(), 1);
		m_mapStallModelConfigs.insert(std::make_pair(cf.nStallLevel, cf));
	}
	KillObj("ModelConfig");
	return TRUE;
}

tstring StallMgr::GetStallModelPath( int nStallLevel )
{
	ASSERT(nStallLevel>=1 && nStallLevel <=10);
	StallModelConfigMap::iterator iter = m_mapStallModelConfigs.find(nStallLevel);
	if (iter != m_mapStallModelConfigs.end())
	{
		int nModel = iter->second.nModel;
		tstring strMod = _T("data\\model\\BT\\booth\\booth_%d.FSM");
		TCHAR szBuffer[256];
		_sntprintf(szBuffer, 256, strMod.c_str(), nModel);
		return szBuffer;
	}
	return _T("");
}

tstring StallMgr::GetStallSignBoardPath( int nStallLevel )
{
	ASSERT(nStallLevel>=1 && nStallLevel <=10);
	StallModelConfigMap::iterator iter = m_mapStallModelConfigs.find(nStallLevel);
	if (iter != m_mapStallModelConfigs.end())
	{
		int nBoard = iter->second.nSignBoard;
		tstring strMod = _T("data\\model\\BT\\ZP\\zp%d.dds");
		TCHAR szBuffer[256];
		_sntprintf(szBuffer, 256, strMod.c_str(), nBoard);
		return szBuffer;
	}
	return _T("");
}

DWORD StallMgr::OnNetStallSetFinish( tagNS_StallSetFinish* pNetCmd, DWORD )
{
	if (pNetCmd->dwErrorCode == E_Success)
	{
		Player *pPlayer = (Player*)RoleMgr::Inst()->FindRole(pNetCmd->dwRoleID);
		if (P_VALID(pPlayer))
		{
			pPlayer->SendEventStallAppear(true, pNetCmd->byStallLevel);
		}

	}
	return 0;
}