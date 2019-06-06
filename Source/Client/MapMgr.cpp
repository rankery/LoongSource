#include "StdAfx.h"
#include "MapMgr.h"
#include "..\WorldDefine\msg_map.h"
#include "LocalPlayer.h"
#include "RoleMgr.h"
#include "GameSet.h"
#include "MapEvent.h"
#include "GroundItemMgr.h"
#include "MapLogicData.h"
#include "InstanceMgr.h"

const float TILE_SIZE=50.0f;
const float REFRESHMAPAREA_TIME=2.0f;	//地图区域刷新时间

inline POINT _World2Tile(const Vector3& wpt)
{
	POINT tile;
	tile.x=int(wpt.x/TILE_SIZE);
	tile.y=int(wpt.z/TILE_SIZE);
	return tile;
}

MapMgr::MapMgr(void):m_trunk(this)
{
	m_pSG=NULL;
	m_viewDist=0;
	m_sceneDetailLevel=0;
	m_bEnableSunFlare=false;
	m_bInTrigger=false;
	m_curTriggerIndex=-1;
	m_curMapID=GT_INVALID;
	m_curMapName=_T("");
	m_curMapAreaIndex=-1;
	m_lastUpdateMapAreaTime=Kernel::Inst()->GetAccumTime();
}

MapMgr::~MapMgr(void)
{
	//Destroy();
}


void MapMgr::Init()
{
	m_pGameFrameMgr=TObjRef<GameFrameMgr>();
	m_pZoneSession=TObjRef<NetSession>();

	//--注册游戏事件处理函数
	m_pGameFrameMgr->RegisterEventHandle(_T("tagEnterStandStateEvent"),(FRAMEEVENTPROC)
		m_trunk.sfp1(&MapMgr::OnGameEvent));
}

void MapMgr::Destroy()
{
	//--注销游戏事件处理函数
	m_pGameFrameMgr->UnRegisterEventHandler(_T("tagEnterStandStateEvent"),(FRAMEEVENTPROC)
		m_trunk.sfp1(&MapMgr::OnGameEvent));
	
	CloseMap();
}

void MapMgr::OpenMap(const TCHAR* szMapName,DWORD dwMapID,CameraBase* pBindCamera)
{
	//--先关闭当前的地图
	CloseMap();

	ResMgr::Inst()->EnableIOAsync(true);
	NavResMgr::Inst()->EnableIOAsync(true);
	IFS* pMapFS=(IFS*)(VOID*)TObjRef<VirtualFileSys>("VFS_Map");
	IFS* pSysFS=(IFS*)(VOID*)TObjRef<VirtualFileSys>("VFS_System");

	//--在打开新的包之前, 必须保证异步IO完成了现有包的所有操作
	while(ResMgr::Inst()->GetAsyncQueueSize() > 0 || NavResMgr::Inst()->GetAsyncQueueSize() > 0)
		Sleep(100);
	//--
	TCHAR szCpk[MAX_PATH];
	_stprintf(szCpk,_T("data\\map\\%s.cpk"),szMapName);
	pMapFS->SetWorkingPackage(szCpk);

	//--load game map
	TCHAR szPath[512];
	_stprintf(szPath,_T("data\\map\\%s\\%s.mb"),szMapName,szMapName);
	m_gameMap.LoadFromFile(pMapFS,szPath,GameMap::ELO_Trigger|GameMap::ELO_Npc|GameMap::ELO_Sound|GameMap::ELO_MapRect|GameMap::ELO_MapDoor);

	//--创建触发器区域
	const vector<tagMapTrigger>& triggers=m_gameMap.GetTriggers();
	int i;
	for(i=0;i<(int)triggers.size();i++)
	{
		const tagMapTrigger& trigger=triggers[i];
		ASSERT(trigger.region.size()>0);

		vector<POINT> points;
		for(int j=0;j<(int)trigger.region.size();j++)
		{
			POINT pt=_World2Tile(trigger.region[j]);
			points.push_back(pt);
		}
		
		HRGN hRgn=::CreatePolygonRgn(&points[0],(int)points.size(),ALTERNATE);
		m_triggerRgn.push_back(hRgn);
	}

	//--创建地图区域
	const vector<tagMapArea>& mapAreas=m_gameMap.GetMapRect();
	for(i=0;i<(int)mapAreas.size();i++)
	{
		const tagMapArea& mapArea=mapAreas[i];
		ASSERT(mapArea.region.size()>0);

		vector<POINT> points;
		for(int j=0;j<(int)mapArea.region.size();j++)
		{
			POINT pt=_World2Tile(mapArea.region[j]);
			points.push_back(pt);
		}

		HRGN hRgn=::CreatePolygonRgn(&points[0],(int)points.size(),ALTERNATE);
		m_mapAreaRgn.push_back(hRgn);
	}

	//--load nav map
	m_navMap.LoadFromFile(szMapName,pMapFS,pSysFS,NavMap::ELO_Collider|NavMap::ELO_PlayerNavMap|NavMap::ELO_NPCNavMap|NavMap::ELO_GroundType);

	//--load scene graph
	m_pSG=new HugeQuadTreeSG;
	m_pSG->LoadFromFile(pMapFS,szMapName);
	m_pSG->BindCamera(pBindCamera);

	//--set sky box
	SGSkyBox *pSkyBox=new SGSkyBox;
	pSkyBox->SetRotate(_DegToRad(m_gameMap.GetSkyYaw()),0.0f,0.0f);
	TCHAR *szTopTex=m_gameMap.GetSkyBoxTop();
	if (szTopTex!=NULL
		&& _tcslen(szTopTex)>0)
	{
		Vector3 size;
		m_gameMap.GetSkyBoxSize(size);
		pSkyBox->Create(m_gameMap.GetSkyBoxTop(),m_gameMap.GetSkyBoxFrontRight(),
			m_gameMap.GetSkyBoxBackLeft(),size);

		Vector3 offset;
		m_gameMap.GetSkyBoxOff(offset);
		pSkyBox->SetOffset(offset);
		pSkyBox->EnableRenderShade(m_gameMap.IsRenderSkyShade());
		pSkyBox->SetShadeColor(m_gameMap.GetSkyShadeCol());
		m_pSG->SetSkyNode(pSkyBox);

		SGSunNode* pSunNode = new SGSunNode;
		Filename strSunPath = m_gameMap.GetSkyBoxTop();
		if( pSunNode->Create( strSunPath.GetPath().c_str() ) )
		{
			pSunNode->EnableFlare( m_bEnableSunFlare );
			m_pSG->SetSunNode( pSunNode );
		}
		else
			SAFE_DELETE( pSunNode );
	}

	//--set sunlight
	tagLight sunLight = m_gameMap.GetSunLight();
	sunLight.diffuse = m_gameMap.GetDynamicDiffCol();
	sunLight.diffuse.R *= m_gameMap.GetSunModulus();
	sunLight.diffuse.G *= m_gameMap.GetSunModulus();
	sunLight.diffuse.B *= m_gameMap.GetSunModulus();
	sunLight.ambient = m_gameMap.GetDynamicAmbCol();
	sunLight.specular = m_gameMap.GetDynamicSpecCol();

	m_pSG->SetSunLight(sunLight);

	//--set view zone
	SetViewZoneSize();

	//--
	m_bInTrigger=false;
	m_curTriggerIndex=-1;
	m_curMapID=dwMapID;
	m_curMapName=szMapName;

	// 是否在副本地图内
	const tagMapLogicData *mapLogicData = MapLogicData::Inst()->FindMapLogicData(m_curMapID);
	if (P_VALID(mapLogicData) && mapLogicData->eMapType == EMT_Instance)
	{
		tagGameEvent evt(_T("EnterInstance"), NULL);
		TObjRef<GameFrameMgr>()->SendEvent(&evt);
	}

	// 发送关闭地图音乐事件
	tagGameEvent evtAudio( _T("CloseMapMusic"), NULL );
	TObjRef<GameFrameMgr>()->SendEvent(&evtAudio);

	// 发送打开地图游戏事件
	tagGameEvent evt( _T("OpenMap"), NULL );
	TObjRef<GameFrameMgr>()->SendEvent(&evt);
}

void MapMgr::CloseMap()
{
	// 如果在副本内，发送离开副本事件
	if (InstanceMgr::Inst()->IsInInstance())
	{
		tagGameEvent evt(_T("LeaveInstance"), NULL);
		TObjRef<GameFrameMgr>()->SendEvent(&evt);
	}

	// 发送关闭地图游戏事件
	tagGameEvent evt( _T("CloseMap"), NULL );
	TObjRef<GameFrameMgr>()->SendEvent(&evt);

	// 清空地面物品
	GroundItemMgr::Inst()->ClearAll();

	//--释放场景图
	SAFE_DELETE(m_pSG);

	//--释放导航图
	m_navMap.Destroy();

	//--释放触发器区域
	int i;
	for(i=0;i<(int)m_triggerRgn.size();i++)
		::DeleteObject(m_triggerRgn[i]);
	m_triggerRgn.clear();

	//--释放地图区域
	for(i=0;i<(int)m_mapAreaRgn.size();i++)
		::DeleteObject(m_mapAreaRgn[i]);
	m_mapAreaRgn.clear();

	m_curMapID=GT_INVALID;
	m_curMapAreaIndex=-1;
}

void MapMgr::SetViewDist(int viewDist)
{
	ASSERT(viewDist>=0&&viewDist<=MAX_VIEW_DIST);
	if( viewDist == m_viewDist )
		return;

	m_viewDist=viewDist;
	SetViewZoneSize();
}

void MapMgr::SetSceneDetailLevel(int level)
{
	ASSERT(level>=0&&level<=MAX_VIEW_DIST);
	if( level == m_sceneDetailLevel )
		return;

	m_sceneDetailLevel=level;
	SetViewZoneSize();
}

void MapMgr::SetViewZoneSize()
{
	if(m_pSG==NULL)
		return;

	//--lod
	int nLitNode=min(m_viewDist,m_sceneDetailLevel);
	int nMidNode=m_viewDist;
	int nBigNode=m_viewDist;
	int nSimpleTrn=m_viewDist;

	//--由lod计算viewzone size
	float fLitNode=0;
	switch(nLitNode)
	{
	case 0: fLitNode=128.0f*50.0f; break;
	case 1: fLitNode=160.0f*50.0f; break;
	case 2: fLitNode=192.0f*50.0f; break;
	case 3: fLitNode=224.0f*50.0f; break;
	case 4: fLitNode=256.0f*50.0f; break;
	}
	
	//--midnode
	float fMidNode=0;
	switch(nMidNode)
	{
	case 0: fMidNode=340.0f*50.0f; break;
	case 1: fMidNode=430.0f*50.0f; break;
	case 2: fMidNode=512.0f*50.0f; break;
	case 3: fMidNode=600.0f*50.0f; break;
	case 4: fMidNode=690.0f*50.0f; break;
	}

	//--bignode
	float fBigNode=0;
	switch(nBigNode)
	{
	case 0: fBigNode=512.0f*50.0f; break;
	case 1: fBigNode=640.0f*50.0f; break;
	case 2: fBigNode=768.0f*50.0f; break;
	case 3: fBigNode=896.0f*50.0f; break;
	case 4: fBigNode=1024.0f*50.0f; break;
	}

	//--simpletrn
	float fSimpleTrn=0;
	switch(nSimpleTrn)
	{
	case 0: fSimpleTrn=1800.0f*50.0f; break;
	case 1: fSimpleTrn=2100.0f*50.0f; break;
	case 2: fSimpleTrn=2400.0f*50.0f; break;
	case 3: fSimpleTrn=2700.0f*50.0f; break;
	case 4: fSimpleTrn=3000.0f*50.0f; break;
	}

	m_pSG->SetViewZoneSize(HugeQuadTreeSG::EVZT_LitNode,Vector3(fLitNode,fLitNode,fLitNode));
	m_pSG->SetViewZoneSize(HugeQuadTreeSG::EVZT_MidNode,Vector3(fMidNode,fMidNode,fMidNode));
	m_pSG->SetViewZoneSize(HugeQuadTreeSG::EVZT_BigNode,Vector3(fBigNode,fBigNode,fBigNode));
	m_pSG->SetViewZoneSize(HugeQuadTreeSG::EVZT_SimpleTerrain,Vector3(fSimpleTrn,fSimpleTrn,fSimpleTrn));
	m_pSG->SetViewZoneSize(HugeQuadTreeSG::EVZT_DynamicNode,Vector3(fMidNode,fMidNode,fMidNode));

	//--set fog
	ResetSceneFog( false );

	//--set camera zfar
	CameraBase* pCamera=m_pSG->GetCamera();
	pCamera->SetZFar(fBigNode);

	//--set navmap viewzone
	m_navMap.SetViewZoneSize(Vector3(fMidNode,fMidNode,fMidNode));
}

void MapMgr::EnableSunFlare( const bool bEnable )
{
	if( bEnable == m_bEnableSunFlare )
		return;
	if( NULL != m_pSG && NULL != m_pSG->GetSunNode() )
	{
		m_pSG->GetSunNode()->EnableFlare( bEnable );
	}
	m_bEnableSunFlare = bEnable;
}

void MapMgr::Update()
{
	//--更新SceneGraph可见区域
	if(m_pSG)
	{
		BeginSpeedTest(_T("Client::Mainloop.update.gameframe.worldframe.mapmgr.1"));
		m_pSG->InvalidViewZone();
		EndSpeedTest(_T("Client::Mainloop.update.gameframe.worldframe.mapmgr.1"));

		BeginSpeedTest(_T("Client::Mainloop.update.gameframe.worldframe.mapmgr.2"));
		CameraBase* pCamera=m_pSG->GetCamera();
		m_navMap.InvalidViewZone(pCamera->GetLookat());
		EndSpeedTest(_T("Client::Mainloop.update.gameframe.worldframe.mapmgr.2"));
	}

	float elapseTime=Kernel::Inst()->GetAccumTime();
	if( elapseTime-m_lastUpdateMapAreaTime > REFRESHMAPAREA_TIME )//--每2秒更新检测一次进出地图区域
	{
		BeginSpeedTest(_T("Client::Mainloop.update.gameframe.worldframe.mapmgr.3"));
		UpdateMapArea();
		EndSpeedTest(_T("Client::Mainloop.update.gameframe.worldframe.mapmgr.3"));
		m_lastUpdateMapAreaTime=elapseTime;
	}
}

MapMgr g_mapMgr;
MapMgr* MapMgr::Inst()
{
	return &g_mapMgr;
}

void MapMgr::UpdateTriggers()
{
	//--取得本地玩家包围盒
	AABBox roleBox;
	LocalPlayer* pRole=RoleMgr::Inst()->GetLocalPlayer();
	if(pRole==NULL)
		return;

	SceneNode* pSGNode=pRole->GetSceneNode();
	if(pSGNode==NULL)
		return;

	pSGNode->GetBox(roleBox);
	POINT roleTile=_World2Tile(pRole->GetPos());

	//--
	if(m_bInTrigger)
	{
		const vector<tagMapTrigger>& triggers=m_gameMap.GetTriggers();
		const tagMapTrigger& trigger=triggers[m_curTriggerIndex];
		if(!trigger.box.Intersects(roleBox)
			||FALSE==::PtInRegion(m_triggerRgn[m_curTriggerIndex],roleTile.x,roleTile.y))
		{
			m_bInTrigger=false;
			m_curTriggerIndex=-1;
		}
	}

	if( !m_bInTrigger )
	{
		const vector<tagMapTrigger>& triggers=m_gameMap.GetTriggers();
		for(int i=0;i<(int)triggers.size();i++)
		{
			const tagMapTrigger& trigger=triggers[i];
			if(trigger.box.Intersects(roleBox)
				&&TRUE==::PtInRegion(m_triggerRgn[i],roleTile.x,roleTile.y))
			{
				m_bInTrigger=true;
				m_curTriggerIndex=i;

				// 判断是否是触发了副本的触发器
				if (trigger.eType == EMT_GotoNewMap)
				{
					DWORD dwMapID = TObjRef<Util>()->Crc32(trigger.szMapName);
					const tagMapLogicData *pMapLogicData = NULL;
					pMapLogicData = MapLogicData::Inst()->FindMapLogicData(dwMapID);
					if (P_VALID(pMapLogicData) && pMapLogicData->eMapType == EMT_Instance)
					{
						// 根据副本属性表判断是否触发此触发器
						InstanceMgr::Inst()->TryGotoInstance(trigger.dwObjID, dwMapID);
					}
					if (P_VALID(pMapLogicData) && pMapLogicData->eMapType == EMT_Normal && (InstanceMgr::Inst()->IsInInstance() == TRUE))
					{
						InstanceMgr::Inst()->TryOutInstance(trigger.dwObjID, dwMapID);
					}
					if (P_VALID(pMapLogicData) && pMapLogicData->eMapType == EMT_Normal && (InstanceMgr::Inst()->IsInInstance() == FALSE))
					{
						tagNC_RoleMapTrigger cmd;
						cmd.dwTriggerID = trigger.dwObjID;
						m_pZoneSession->Send(&cmd);
					}
				}
				else
				{
					//--发送网络消息
					tagNC_RoleMapTrigger cmd;
					cmd.dwTriggerID=trigger.dwObjID;
					m_pZoneSession->Send(&cmd);

					//--发送游戏事件
					tagEnterTriggerEvent event( _T("Event_RoleEnterTrigger"), NULL );
					event.triggerID=trigger.dwObjID;
					m_pGameFrameMgr->SendEvent( &event );
				}
				break;	
			}
		}
	}
}

void MapMgr::UpdateMapArea()
{
	//--取得本地玩家
	LocalPlayer* pRole=RoleMgr::Inst()->GetLocalPlayer();
	if(pRole==NULL)
		return;

	int inAreaIndex = GetMapAreaIndex( pRole->GetPos(), pRole->GetRoleSize(), EMA_Display );
	const vector<tagMapArea>& mapAreas=m_gameMap.GetMapRect();
	if( m_curMapAreaIndex>=0 && inAreaIndex==-1 )	//发送LeaveMapArea事件
	{
		const int nLeaveAreaIndex = m_curMapAreaIndex;
		m_curMapAreaIndex = -1;
		tagLeaveMapAreaEvent event(_T("tagLeaveMapAreaEvent"),NULL);
		event.eAreaType=mapAreas[nLeaveAreaIndex].eType;
		event.mapAreaID=mapAreas[nLeaveAreaIndex].dwObjID;
		m_pGameFrameMgr->SendEvent(&event);
		
		ResetSceneFog( true );
		return;
	}

	if( inAreaIndex>=0 && m_curMapAreaIndex!=inAreaIndex )	//发送EnterMapArea事件
	{
		m_curMapAreaIndex=inAreaIndex;

		tagEnterMapAreaEvent event(_T("tagEnterMapAreaEvent"),NULL);
		event.eAreaType=mapAreas[m_curMapAreaIndex].eType;
		event.mapAreaID=mapAreas[m_curMapAreaIndex].dwObjID;
		m_pGameFrameMgr->SendEvent(&event);

		ResetSceneFog( true );
		return;
	}
}

DWORD MapMgr::OnGameEvent( tagGameEvent* pEvent )
{
	if(pEvent->strEventName==_T("tagEnterStandStateEvent"))//角色进入站立状态事件
	{
		//更新触发器
		UpdateTriggers();
	}
	return 0;
}

void MapMgr::SyncInvalidViewZone()
{
	//关闭多线程读盘
	ResMgr::Inst()->EnableIOAsync(false);
	NavResMgr::Inst()->EnableIOAsync(false);

	//更新可见区域
	m_pSG->InvalidViewZone();

	CameraBase* pCamera=m_pSG->GetCamera();
	m_navMap.InvalidViewZone(pCamera->GetPos());

	//执行初始化操作
	m_pSG->SyncInit();

	//重新开启多线程读盘
	ResMgr::Inst()->EnableIOAsync(true);
	NavResMgr::Inst()->EnableIOAsync(true);
}

DWORD MapMgr::GetCurTriggerID() const
{
	if( m_curTriggerIndex == -1 )
		return GT_INVALID;

	const vector<tagMapTrigger>& triggers=m_gameMap.GetTriggers();
	const tagMapTrigger& trigger=triggers[m_curTriggerIndex];

	return trigger.dwObjID;
}

BOOL MapMgr::GetTriggerPos( DWORD questSerial,Vector3& pos,AABBox& box )
{
	const vector<tagMapTrigger>& triggers=m_gameMap.GetTriggers();
	for( size_t i=0; i<triggers.size(); i++ )
	{
		const tagMapTrigger& trigger = triggers[i];
		if( trigger.dwQuestSerial == questSerial )
		{
			Vector3 regionSum(0.0f,0.0f,0.0f);
			for( size_t j=0; j<trigger.region.size(); j++ )
			{
				regionSum += trigger.region[j];
			}
			pos = regionSum / (float)trigger.region.size();
			box = trigger.box;

			return TRUE;
		}
	}

	return FALSE;
}

BOOL MapMgr::GetTriggerPos(DWORD triggerID, Vector3& pos)
{
	const vector<tagMapTrigger>& triggers=m_gameMap.GetTriggers();
	for( size_t i=0; i<triggers.size(); i++ )
	{
		const tagMapTrigger& trigger = triggers[i];
		if( trigger.dwObjID == triggerID )
		{
			Vector3 regionSum(0.0f,0.0f,0.0f);
			for( size_t j=0; j<trigger.region.size(); j++ )
			{
				regionSum += trigger.region[j];
			}
			pos = regionSum / (float)trigger.region.size();

			return TRUE;
		}
	}

	return FALSE;

}
void MapMgr::ResetSceneFog( const bool bFade )
{
	if( NULL == m_pSG )
		return;

	tagDistFog fog;
	if( -1 == m_curMapAreaIndex || !m_gameMap.GetMapAreaFog( m_curMapAreaIndex, fog ) )
	{
		m_gameMap.GetFog(fog);
	}
	Vector3 vViewZoneSizeBigNode = m_pSG->GetViewZoneSize( HugeQuadTreeSG::EVZT_BigNode );
	fog.start*=vViewZoneSizeBigNode.z;
	fog.end*=vViewZoneSizeBigNode.z;
	if( bFade )
		m_pSG->SwitchSceneFog(fog);
	else
		m_pSG->SetSceneFog(fog);
}

int	MapMgr::GetMapAreaIndex( const Vector3& vPos, const Vector3& vSize, EMapArea eAreaType/* = EMA_Null*/ )
{
	AABBox box;
	Vector3 vHalfSize = vSize * 0.5f;
	box.min = vPos - vHalfSize;
	box.max = vPos + vHalfSize;
	box.min.y += vHalfSize.y;
	box.max.y += vHalfSize.y;

	POINT ptTile = _World2Tile( vPos );

	int inAreaIndex=-1;
	const vector<tagMapArea>& mapAreas=m_gameMap.GetMapRect();
	for(int i=0;i<(int)mapAreas.size();i++)
	{
		const tagMapArea& mapArea=mapAreas[i];
		if( EMA_Null != eAreaType && eAreaType != mapArea.eType )
			continue;

		if( mapArea.box.Contain( box ) && TRUE == ::PtInRegion(m_mapAreaRgn[i],ptTile.x,ptTile.y) )
		{
			if( inAreaIndex==-1 )
			{
				inAreaIndex=i;
			}
			else
			{
				const tagMapArea& inArea=mapAreas[inAreaIndex];
				const tagMapAreaEx* inAreaEx=m_gameMap.FindMapAreaEx(inArea.dwObjID);
				const tagMapAreaEx* mapAreaEx=m_gameMap.FindMapAreaEx(mapArea.dwObjID);
				if( P_VALID(inAreaEx)
					&& P_VALID(mapAreaEx)
					&& inAreaEx->byPriority < mapAreaEx->byPriority )
				{
					inAreaIndex=i;
				}
			}
		}
	}
	return inAreaIndex;
}