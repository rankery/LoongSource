#include "stdafx.h"
#include "ZoneMapFrame.h"
#include "ZoneMapStatic.h"
#include "RoleMgr.h"
#include "LocalPlayer.h"
#include "MapMgr.h"
#include "MapLogicData.h"
#include "MapEvent.h"
#include "QuestNPCFrame.h"
#include "ZoneMapData.h"
#include "AppliedTree.h"
#include "CombatEvent.h"
#include "QuestMgr.h"
#include "UIEvent.h"
#include "ToolTipFrame.h"

const INT g_nTileSize = 50;

ZoneMapFrame::ZoneMapFrame() :
m_Trunk( this ),
m_pWnd( NULL ),
m_pBn_Close( NULL ),
m_pSt_WoldMap( NULL ),
m_pSt_ZoneMap( NULL ),
m_pEb_ShowName( NULL ),
m_pLb_MapList( NULL ),
m_pBn_Back( NULL ),
m_pBn_Task( NULL ),
m_pTr_Mark( NULL ),
m_nShowAreaIndex( GT_INVALID )
{
}

ZoneMapFrame::~ZoneMapFrame()
{
}

BOOL ZoneMapFrame::Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam)
{
	if( !GameFrame::Init(pMgr, pFather, dwParam) )
		return FALSE;

	m_pMgr->RegisterEventHandle( _T("OpenMap"),					(FRAMEEVENTPROC)m_Trunk.sfp1(&ZoneMapFrame::OnOpenMap));
	m_pMgr->RegisterEventHandle( _T("tagLeaveMapAreaEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&ZoneMapFrame::OnLeaveMapArea));
	m_pMgr->RegisterEventHandle( _T("tagEnterMapAreaEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&ZoneMapFrame::OnEnterMapArea));
	m_pMgr->RegisterEventHandle( _T("tagSetFlagEvent"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&ZoneMapFrame::OnSetFlagEvent));
	m_pMgr->RegisterEventHandle( _T("tagMouseMoveEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&ZoneMapFrame::OnMouseMoveEvent));
	m_pMgr->RegisterEventHandle( _T("tagOpenCloseMapWndEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&ZoneMapFrame::OnOpenCloseMapWndEvent));
	m_pMgr->RegisterEventHandle( _T("tagOpenMapWndEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&ZoneMapFrame::OnOpenMapWndEvent));
	m_pMgr->RegisterEventHandle( _T("tagMouseMoveFinishEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&ZoneMapFrame::OnMouseMoveFinishEvent));

	// 保存区域地图列表
	m_vecZoneMapShow.clear();
	const map<DWORD, tagMapLogicData>& mapMapLogicData = MapLogicData::Inst()->GetDataMap();
	for( map<DWORD, tagMapLogicData>::const_iterator citr = mapMapLogicData.begin(); citr != mapMapLogicData.end(); citr++ )
	{
		// 只显示普通地图
		if( EMT_Normal != citr->second.eMapType )
			continue;

		// 游戏中的地图都是以m开头的（去掉测试场景）
		if( citr->second.szName.empty() || _T('m') != citr->second.szName.c_str()[0] )
			continue;

		tagZoneMapShow zm;
		zm.dwMapID = citr->first;
		zm.strShowName = citr->second.szShowName;
		m_vecZoneMapShow.push_back( zm );
	}
	m_pWnd->SetInvisible( TRUE );

	return TRUE;
}

BOOL ZoneMapFrame::Destroy()
{
	m_pMgr->UnRegisterEventHandler( _T("OpenMap"),					(FRAMEEVENTPROC)m_Trunk.sfp1(&ZoneMapFrame::OnOpenMap));
	m_pMgr->UnRegisterEventHandler( _T("tagLeaveMapAreaEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&ZoneMapFrame::OnLeaveMapArea));
	m_pMgr->UnRegisterEventHandler( _T("tagEnterMapAreaEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&ZoneMapFrame::OnEnterMapArea));
	m_pMgr->UnRegisterEventHandler( _T("tagSetFlagEvent"),			(FRAMEEVENTPROC)m_Trunk.sfp1(&ZoneMapFrame::OnSetFlagEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagMouseMoveEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&ZoneMapFrame::OnMouseMoveEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagOpenCloseMapWndEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&ZoneMapFrame::OnOpenCloseMapWndEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagOpenMapWndEvent"),		(FRAMEEVENTPROC)m_Trunk.sfp1(&ZoneMapFrame::OnOpenMapWndEvent));
	m_pMgr->UnRegisterEventHandler( _T("tagMouseMoveFinishEvent"),	(FRAMEEVENTPROC)m_Trunk.sfp1(&ZoneMapFrame::OnMouseMoveFinishEvent));

	GameFrame::Destroy();

	SafeDestroyUI();
	return TRUE;
}

BOOL ZoneMapFrame::ReloadUI()
{
	SafeDestroyUI();

	// 加载界面
	XmlElement element;
	tstring strPath = g_strLocalPath + _T("\\ui\\maps.xml");
	if( !m_pGUI->LoadXml( &element, "VFS_System", strPath.c_str() ) )
		return FALSE;

	m_pGUI->ChangeXml( &element, "mapswin\\mapbackpic\\maps", "ClassName", "ZoneMapStatic" );
	m_pGUI->ChangeXml( &element, "mapswin\\mapbackpic\\marklist", "ClassName", "AppliedTree" );

	// 区域地图中需要显示的图标
	std::vector<std::string> vecControlName( EMME_Num );
	std::vector<tstring> vecPicFilename( EMME_Num );

	vecControlName[EMME_LocalPlayer]		= "playermark";
	vecControlName[EMME_Teammate]			= "partymark";
	vecControlName[EMME_AcceptQuestNPC]		= "npcdoable";
	vecControlName[EMME_CompleteQuestNPC]   = "npcfinish";
	tstring strDestMarkControlName;

	// 读取贴图路径
	XmlElement* pElement = element.FirstChildElement();
	while( pElement )
	{
		const std::string strName = pElement->Attribute("Name");
		if( strName == "markback" )
		{
			XmlElement* pEleRes = pElement->FirstChildElement();
			while ( pEleRes )
			{
				const std::string strNameRes = pEleRes->Attribute("Name");
				for( UINT i = 0; i < vecControlName.size(); i++ )
				{
					if( strNameRes == vecControlName[i] && !vecControlName[i].empty() )
					{
						const char* szPicFileName = pEleRes->Attribute( "PicFileName" );
						if( NULL != szPicFileName )
							vecPicFilename[i] = m_pUtil->Unicode8ToUnicode( szPicFileName );
						break;
					}
				}
				if( strNameRes == "mark" )
				{
					const char* szPicFileName = pEleRes->Attribute( "PicFileName" );
					if( NULL != szPicFileName )
						strDestMarkControlName = m_pUtil->Unicode8ToUnicode( szPicFileName );
				}

				pEleRes = pEleRes->NextSiblingElement();
			}
		}
		pElement = pElement->NextSiblingElement();
	}


	m_pWnd = m_pGUI->CreateWnd( _T("\\desktop"), &element );
	if( !P_VALID( m_pWnd ) )
		return FALSE;

	// 销毁用来保存数据的控件
	m_pGUI->DestroyWnd( m_pWnd->GetChild( _T("markback") ) );

	m_pBn_Close		= (GUIButton*)m_pWnd->GetChild( _T("mapbackpic\\closebutt") );
	m_pSt_WoldMap	= (GUIStatic*)m_pWnd->GetChild( _T("mapbackpic\\worldmap") );
	m_pSt_ZoneMap	= (ZoneMapStatic*)m_pWnd->GetChild( _T("mapbackpic\\maps") );
	m_pLb_MapList	= (GUIListBox*)m_pWnd->GetChild( _T("mapbackpic\\optmap\\maplist") );
	m_pEb_ShowName	= (GUIEditBox*)m_pWnd->GetChild( _T("mapbackpic\\optmap\\mapname") );
	m_pBn_Back		= (GUIButton*)m_pWnd->GetChild( _T("mapbackpic\\backbutt") );
	m_pBn_Task		= (GUIButton*)m_pWnd->GetChild( _T("mapbackpic\\npclistbutt") );
	m_pTr_Mark      = (AppliedTree*)m_pWnd->GetChild( _T("mapbackpic\\marklist") );

	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&ZoneMapFrame::GUIEventHandler));


	for( UINT i = 0; i< vecControlName.size(); i++ )
	{
		m_pSt_ZoneMap->SetElementImg( i, vecPicFilename[i] );
	}
	m_pSt_ZoneMap->SetDestMarkImg( strDestMarkControlName );

	return TRUE;
}

VOID ZoneMapFrame::SafeDestroyUI()
{
	if( P_VALID(m_pWnd) )
	{
		m_pGUI->DestroyWnd(m_pWnd);
		m_pWnd = NULL;
	}
}

BOOL ZoneMapFrame::GUIEventHandler(tagGUIEvent* pEvent)
{
	const GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);
	if( !P_VALID(pWnd) )
		return FALSE;

	switch( pEvent->eEvent )
	{
	case EGUIE_Active:
		{
			QuestNPCFrame *pFrame = QuestMgr::Inst()->GetNPCFrame(_T("Map_QuestNPC"));
			if( P_VALID(pFrame) )
				pFrame->Show();

			if( pWnd == m_pEb_ShowName )
				m_pGUI->SetActive( m_pLb_MapList );
		}
		break;
	case EGUIE_Click:
		if( pWnd == m_pBn_Close )
		{
			Close();
		}
		else if( pWnd == m_pBn_Back )
		{
			if( m_pSt_WoldMap->IsInvisible() )
			{
				if( GT_INVALID != m_nShowAreaIndex )
				{
					ResetMapData( m_pSt_ZoneMap->GetCurShowMapID(), GT_INVALID );
				}
				else
				{
					SwithToWorldMap();
				}
			}
		}
		else if( pWnd == m_pBn_Task )
		{
			QuestNPCFrame *pFrame = QuestMgr::Inst()->GetNPCFrame(_T("Map_QuestNPC"));
			if( !P_VALID(pFrame) )
			{
				QuestMgr::Inst()->OpenNPCFrame( _T("World"), _T("Map_QuestNPC"), "\\desktop\\mapswin\\mapbackpic", "npclist_map" );
			}
			else
			{
				QuestMgr::Inst()->CloseNPCFrame( _T("Map_QuestNPC") );
			}
		}
		else if( !m_pSt_WoldMap->IsInvisible() )
		{
			OnWorldMapClick( (GUIWnd*)pWnd );
		}
		break;
	case EGUIE_Drag:
		{
			QuestNPCFrame *pFrame = (QuestNPCFrame*)m_pMgr->GetFrame(_T("Map_QuestNPC"));
			if( P_VALID(pFrame) )
			{
				pFrame->Align();
			}
		}
		break;
	case EGUIE_ItemClick:
		if( pWnd == m_pLb_MapList )
		{
			if( m_pSt_WoldMap->IsInvisible() )
				ResetMapData( m_pSt_ZoneMap->GetCurShowMapID(), pEvent->dwParam1 );
			else
			{
				if( pEvent->dwParam1 >= 0 && pEvent->dwParam1 < m_vecZoneMapShow.size() )
					SwithToZoneMap( m_vecZoneMapShow[pEvent->dwParam1].dwMapID );
			}
		}
		else if( pWnd == m_pTr_Mark )
		{
			map<DWORD, tagMarkIndex>::iterator itr = m_mapMarkIndex.find( pEvent->dwParam1 );
			if( itr != m_mapMarkIndex.end() )
			{
				const tagZoneMapData* pData = ZoneMapData::Inst()->FindZoneMapData( m_pSt_ZoneMap->GetCurShowMapID() );
				if( P_VALID( pData ) )
				{
					ASSERT(itr->second.nType<(int)pData->vecMarkTypes.size());
					const tagMapMarkType& markType = pData->vecMarkTypes[itr->second.nType];
					ASSERT(itr->second.nMask<(int)markType.vecMarks.size());
					const tagMapMarkData& markData = markType.vecMarks[itr->second.nMask];
					m_pSt_ZoneMap->ShowDestMark( m_pSt_ZoneMap->GetCurShowMapID(), Vector2( markData.vPos.x, markData.vPos.z ) );
				}
			}
		}
		break;
	case EGUIE_ItemDblClick:
		if( pWnd == m_pTr_Mark )
		{
			// 当前地图
			if( m_pSt_ZoneMap->GetCurShowMapID() == MapMgr::Inst()->GetCurMapID() )
			{
				map<DWORD, tagMarkIndex>::iterator itr = m_mapMarkIndex.find( pEvent->dwParam1 );
				if( itr != m_mapMarkIndex.end() )
				{
					const tagZoneMapData* pData = ZoneMapData::Inst()->FindZoneMapData( m_pSt_ZoneMap->GetCurShowMapID() );
					if( P_VALID( pData ) )
					{
						ASSERT(itr->second.nType<(int)pData->vecMarkTypes.size());
						const tagMapMarkType& markType = pData->vecMarkTypes[itr->second.nType];
						ASSERT(itr->second.nMask<(int)markType.vecMarks.size());
						const tagMapMarkData& markData = markType.vecMarks[itr->second.nMask];
						m_pSt_ZoneMap->LPAutoMoveTo( markData.vPos, 200.0f );
					}
				}
			}
			// 跨地图寻路
			else
			{
				const tstring& strCurMapName = MapMgr::Inst()->GetCurMapName();
				const tagMapLogicData* pDestMapLogicData = MapLogicData::Inst()->FindMapLogicData( m_pSt_ZoneMap->GetCurShowMapID() );
				ASSERT( P_VALID( pDestMapLogicData ) );
				NavMap* pNav = MapMgr::Inst()->GetNavMap();
				PlayerNavMap* pPlayerNav = pNav->GetPlayerNavMap();
				Vector3 vSwitchPos( 0.0f, 0.0f, 0.0f );
				const bool bFind = pPlayerNav->FindPathByMap( strCurMapName.c_str(), pDestMapLogicData->szName.c_str(), vSwitchPos );
				if( bFind )
					m_pSt_ZoneMap->LPAutoMoveTo( vSwitchPos, 0.0f );
			}

		}
		break;
	default:
		break;
	}// switch( pEvent->eEvent )

	return FALSE;
}

DWORD ZoneMapFrame::OnOpenMap(tagGameEvent*)
{
	SetShowMap( MapMgr::Inst()->GetCurMapID() );
	return 0;
}

DWORD ZoneMapFrame::OnLeaveMapArea( tagLeaveMapAreaEvent* pEvent )
{
	if( P_VALID( m_pSt_ZoneMap ) && m_pSt_ZoneMap->GetCurShowMapID() == MapMgr::Inst()->GetCurMapID() )
	{
		const int nIndex = GetCurAreaIndex();
		ResetMapData( MapMgr::Inst()->GetCurMapID(), nIndex );
	}
	return 0;
}

DWORD ZoneMapFrame::OnEnterMapArea( tagEnterMapAreaEvent* pEvent )
{
	if( P_VALID( m_pSt_ZoneMap ) && m_pSt_ZoneMap->GetCurShowMapID() == MapMgr::Inst()->GetCurMapID() )
	{
		const int nIndex = GetCurAreaIndex();
		ResetMapData( MapMgr::Inst()->GetCurMapID(), nIndex );
	}
	return 0;
}

DWORD ZoneMapFrame::OnSetFlagEvent( tagSetFlagEvent* pEvent )
{
	if( !P_VALID(m_pSt_ZoneMap) )
		return 0;

	if( m_pSt_ZoneMap->GetCurShowMapID() == MapMgr::Inst()->GetCurMapID() )
		m_pSt_ZoneMap->ShowDestMark( MapMgr::Inst()->GetCurMapID(), Vector2( pEvent->x, pEvent->z ) );

	return 0;
}

DWORD ZoneMapFrame::OnMouseMoveEvent( tagMouseMoveEvent* pEvent )
{
	if( !P_VALID(m_pSt_ZoneMap) )
		return 0;

	if( m_pSt_ZoneMap->GetCurShowMapID() == MapMgr::Inst()->GetCurMapID() )
		m_pSt_ZoneMap->ShowDestMark( MapMgr::Inst()->GetCurMapID(), Vector2( pEvent->end.x, pEvent->end.z ) );

	return 0;
}

DWORD ZoneMapFrame::OnOpenCloseMapWndEvent( tagOpenCloseMapWndEvent* pEvent )
{
	if( m_pWnd->IsInvisible() )
	{
		Open( pEvent->bWorldMap );
	}
	else
	{
		if( pEvent->bWorldMap )
		{
			if( m_pSt_WoldMap->IsInvisible() )
				SwithToWorldMap();
			else
				Close();
		}
		else
		{
			if( m_pSt_ZoneMap->IsInvisible() )
				SwithToZoneMap( MapMgr::Inst()->GetCurMapID() );
			else
				Close();
		}
	}
	return 0;
}

DWORD ZoneMapFrame::OnOpenMapWndEvent( tagOpenMapWndEvent* pEvent )
{
	if( !m_pWnd->IsInvisible() )
	{
		if( m_pSt_WoldMap->IsInvisible() && pEvent->bWorldMap )
			return 0;
	}
	Open( pEvent->bWorldMap );

	return 0;
}

DWORD ZoneMapFrame::OnMouseMoveFinishEvent( tagMouseMoveFinishEvent* pEvent )
{
	m_pSt_ZoneMap->CloseDestMark();
	return 0;
}

VOID ZoneMapFrame::Update()
{
	// 区域地图是实时刷新的，地图区域列表也要实时刷新
	if( P_VALID(m_pLb_MapList) && !m_pLb_MapList->IsInvisible() && 
		P_VALID(m_pSt_ZoneMap) && !m_pSt_ZoneMap->IsInvisible() )
		m_pLb_MapList->SetRefresh();

	if( P_VALID(m_pLb_MapList) && 0 != m_pLb_MapList->GetCurrentTextRow() )
		m_pLb_MapList->SetCurrentTextRow( 0 );
}

BOOL ZoneMapFrame::EscCancel()
{
	if( P_VALID(m_pWnd) && !m_pWnd->IsInvisible() )
	{
		Close();
		return TRUE;
	}
	return FALSE;
}

VOID ZoneMapFrame::ResetMapData( const DWORD dwMapID, const int nAreaID )
{
	if( !P_VALID( m_pWnd ) )
		return;

	const tagMapLogicData* pLogicData = MapLogicData::Inst()->FindMapLogicData( dwMapID );
	ASSERT( P_VALID( pLogicData ) );

	ZoneMapStatic::tagMapData data;

	if( GT_INVALID == nAreaID )
	{
		data.vWorldSize = pLogicData->vSize * g_nTileSize;
		m_pSt_ZoneMap->ResetMapData( data );

		TCHAR szMapFileName[MAX_PATH];
		_stprintf( szMapFileName, _T("%s\\map\\%s\\%s.tga"), g_strLocalPath.c_str(), pLogicData->szName.c_str(), pLogicData->szName.c_str() );
		m_pSt_ZoneMap->SetColorTex( szMapFileName );

		m_pEb_ShowName->SetText( pLogicData->szShowName.c_str() );
	}
	else
	{
		const tagMapShowAreaData& za = m_vecZoneMapArea[nAreaID];
		data.vWorldPos.x = za.box.min.x;
		data.vWorldPos.y = za.box.min.z;
		data.vWorldSize.x = za.box.max.x - za.box.min.x;
		data.vWorldSize.y = za.box.max.z - za.box.min.z;
		m_pSt_ZoneMap->ResetMapData( data );

		TCHAR szMapFileName[MAX_PATH];
		_stprintf( szMapFileName, _T("%s\\map\\%s\\%s_%x.tga"), g_strLocalPath.c_str(), pLogicData->szName.c_str(), pLogicData->szName.c_str(), za.dwObjID );
		m_pSt_ZoneMap->SetColorTex( tstring( szMapFileName ) );
		m_pEb_ShowName->SetText( za.szTitle.c_str() );
	}
	m_pSt_ZoneMap->SetCurShowMapID( dwMapID );
	m_nShowAreaIndex = nAreaID;
}

VOID ZoneMapFrame::SetShowMap( const DWORD dwMapID )
{
	if( !P_VALID( m_pWnd ) )
		return;

	int nAreaID = GT_INVALID;
	if( dwMapID == MapMgr::Inst()->GetCurMapID() )
		nAreaID = GetCurAreaIndex();

	ReBuidAreaList( dwMapID );
	ResetMapData( dwMapID, nAreaID );
	ReBuidMarksTree( dwMapID );
}

VOID ZoneMapFrame::ReBuidAreaList( DWORD dwMapID )
{
	if( !P_VALID( m_pWnd ) )
		return;

	m_vecZoneMapArea.clear();
	MapLogicData::Inst()->GetMapShowAreaList(dwMapID,m_vecZoneMapArea);

	// 修改列表大小
	tagPoint ptSizeListBox = m_pLb_MapList->GetSize();
	ptSizeListBox.y = m_pLb_MapList->GetRowHeight() * m_vecZoneMapArea.size();

	BOOL bResizable = m_pLb_MapList->IsResizable();
	m_pLb_MapList->SetResizable( TRUE );
	m_pLb_MapList->Resize( ptSizeListBox );
	m_pLb_MapList->SetResizable( bResizable );

	// 修改盒子为正方形
	for( size_t i = 0; i < m_vecZoneMapArea.size(); i++ )
	{
		tagMapShowAreaData& area = m_vecZoneMapArea[i];

		const Vector3 vCenter = area.box.GetCenter();
		const float fHalfSize = max( area.box.max.x - area.box.min.x, area.box.max.z - area.box.min.z ) * 0.5f;
		area.box.min = Vector3( vCenter.x - fHalfSize, vCenter.y - fHalfSize, vCenter.z - fHalfSize );
		area.box.max = Vector3( vCenter.x + fHalfSize, vCenter.y + fHalfSize, vCenter.z + fHalfSize );
		m_pLb_MapList->SetText( i, 0, area.szTitle.c_str() );
	}
}

int ZoneMapFrame::GetCurAreaIndex()
{
	LocalPlayer* pLP = RoleMgr::Inst()->GetLocalPlayer();
	if( NULL == pLP )
		return GT_INVALID;

	const int id = MapMgr::Inst()->GetMapAreaIndex( pLP->GetPos(), pLP->GetRoleSize(), EMA_Display );
	if( -1 == id )
		return GT_INVALID;

	const vector<tagMapArea>& mapArea = MapMgr::Inst()->GetGameMap().GetMapRect();
	const DWORD dwObjID = mapArea[id].dwObjID;
	for( size_t i = 0; i < m_vecZoneMapArea.size(); i++ )
	{
		if( dwObjID == m_vecZoneMapArea[i].dwObjID )
			return i;
	}
	return GT_INVALID;
}

VOID ZoneMapFrame::ReBuidMarksTree( const DWORD dwMapID )
{
	if( !P_VALID(m_pTr_Mark ) )
		return;

	m_pTr_Mark->RemoveItem( m_pUtil->Crc32( _T("\\Root") ) );
	m_mapMarkIndex.clear();
	const DWORD dwRootItemID = m_pTr_Mark->InsterItem( NULL, _T("Root"), _T("rt") );
	ASSERT( P_VALID(dwRootItemID) );

	const tagZoneMapData* pData = ZoneMapData::Inst()->FindZoneMapData( dwMapID );
	if( !P_VALID( pData ) )
		return;

	// 填充树形控件
	const vector<tagMapMarkType>& vecTypes = pData->vecMarkTypes;
	for( size_t nType = 0; nType < vecTypes.size(); nType++ )
	{
		// 第一层：标记类型
		const tagMapMarkType& markType = vecTypes[nType];
		tstringstream streamType;
		streamType<<_T("Type_")<<nType;
		const DWORD dwTypeID = m_pTr_Mark->InsterItem( _T("\\Root"), streamType.str().c_str(), markType.strType.c_str() );
		ASSERT( GT_INVALID != dwTypeID );

		// 第二层：标记
		for( size_t nMark = 0; nMark < markType.vecMarks.size(); nMark++ )
		{
			const tagMapMarkData& markData = markType.vecMarks[nMark];
			tstringstream streamFather;
			streamFather<<_T("\\Root")<<_T("\\")<<streamType.str().c_str();
			tstringstream streamMark;
			streamMark<<_T("Mark_")<<nMark;
			const DWORD dwMarskID = m_pTr_Mark->InsterItem( streamFather.str().c_str(), streamMark.str().c_str(), markData.strName.c_str() );
			ASSERT( GT_INVALID != dwMarskID );
			m_mapMarkIndex[dwMarskID] = tagMarkIndex( nType, nMark );
		}
	}

	// 只展开第一类标记
	tagGUITreeItem* pRootItem = m_pTr_Mark->GetItem( dwRootItemID );
	ASSERT( P_VALID( pRootItem ) );
	for( std::list<tagGUITreeItem*>::iterator itr = pRootItem->listChildren.begin(); itr != pRootItem->listChildren.end(); itr++ )
	{
		if( itr == pRootItem->listChildren.begin() )
			continue;

		tagGUITreeItem* pItem = (*itr);
		pItem->bExpended = false;
	}
	m_pTr_Mark->RefreshScrollBar();
}

VOID ZoneMapFrame::OnWorldMapClick( GUIWnd* pWnd )
{
	const DWORD dwMapID = m_pUtil->Crc32( pWnd->GetName().c_str() );
	const tagMapLogicData* pMapLogicData = MapLogicData::Inst()->FindMapLogicData( dwMapID );
	if( !P_VALID( pMapLogicData ) )
		return;

	SwithToZoneMap( dwMapID );
}

VOID ZoneMapFrame::SwithToWorldMap()
{
	m_nShowAreaIndex = GT_INVALID;

	m_pSt_ZoneMap->SetInvisible( TRUE );
	m_pBn_Back->SetInvisible( TRUE );
	m_pBn_Task->SetInvisible( TRUE );
	m_pTr_Mark->SetInvisible( TRUE );
	m_pSt_WoldMap->SetInvisible( FALSE );
	m_pSt_WoldMap->FlipToTop();

	m_pLb_MapList->SetInvisible( TRUE );
	tagPoint ptSizeListBox = m_pLb_MapList->GetSize();
	ptSizeListBox.y = m_pLb_MapList->GetRowHeight() * m_vecZoneMapShow.size();

	BOOL bResizable = m_pLb_MapList->IsResizable();
	m_pLb_MapList->SetResizable( TRUE );
	m_pLb_MapList->Resize( ptSizeListBox );
	m_pLb_MapList->SetResizable( bResizable );

	for( size_t i = 0; i < m_vecZoneMapShow.size(); i++ )
	{
		m_pLb_MapList->SetText( i, 0, m_vecZoneMapShow[i].strShowName.c_str() );
		if( MapMgr::Inst()->GetCurMapID() == m_vecZoneMapShow[i].dwMapID )
			m_pEb_ShowName->SetText( m_vecZoneMapShow[i].strShowName.c_str() );
	}
}

VOID ZoneMapFrame::SwithToZoneMap( const DWORD dwMapID )
{
	m_pSt_WoldMap->SetInvisible( TRUE );
	m_pSt_ZoneMap->SetInvisible( FALSE );
	m_pBn_Back->SetInvisible( FALSE );
	m_pBn_Task->SetInvisible( FALSE );
	m_pTr_Mark->SetInvisible( FALSE );
	m_pLb_MapList->SetInvisible( TRUE );
	SetShowMap( dwMapID );
}

VOID ZoneMapFrame::Open( BOOL bWorldMap )
{
	if( bWorldMap )
		SwithToWorldMap();
	else
	{
		SwithToZoneMap( MapMgr::Inst()->GetCurMapID() );
	}
	m_pWnd->SetInvisible( FALSE );
}

VOID ZoneMapFrame::Close()
{
	const GUIWnd* pMousPointWnd = m_pGUI->GetMousePoint();
	if( pMousPointWnd == m_pSt_ZoneMap || pMousPointWnd == m_pSt_ZoneMap->GetChild( _T("legal") ) )
	{
		ShowTip( NULL, _T("") );
	}
	QuestMgr::Inst()->CloseNPCFrame( _T("Map_QuestNPC") );
	m_pWnd->SetInvisible( TRUE );
	m_pSt_WoldMap->SetInvisible( TRUE );
	m_pSt_ZoneMap->SetInvisible( TRUE );
}