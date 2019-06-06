#include "StdAfx.h"
#include ".\builtmapview.h"
#include "..\WorldBase\WorldBase.h"
using namespace WorldBase;
#include "ClientApp.h"
#include "..\Cool3D\VFS\FileUtil.h"
#include "..\Cool3D\SceneGraph\SGSkyNode.h"
#include "..\Cool3D\SceneGraph\SGSkyBox.h"
#include "..\Cool3D\SceneGraph\HugeQuadTreeSG.h"
#include "..\Cool3D\SceneGraph\SGSunNode.h"
#include <time.h>
#include "StrTable.h"

float BuiltMapView::ViewDistSN=256*50.0f;
float BuiltMapView::ViewDistMN=512*50.0f;
float BuiltMapView::ViewDistBN=800*50.0f;
float BuiltMapView::ViewDistDN=256*50.0f;
float BuiltMapView::ViewDistST=2000*50.0f;
bool BuiltMapView::NoMonster=false;

BuiltMapView::BuiltMapView(void)
{
	m_pSG=NULL;
	m_numNPC=0;
	m_curMapAreaIndex=-1;
}

BuiltMapView::~BuiltMapView(void)
{
	SAFE_DELETE(m_pSG);
	for(size_t i=0;i<m_carriers.size();i++)
		SAFE_DELETE(m_carriers[i]);
	m_carriers.clear();
}

int BuiltMapView::LoadMap(const TCHAR* szMapName,CameraBase *pCamera)
{
	ResMgr::Inst()->EnableIOAsync(false);

	ASSERT(m_pSG==NULL);
	m_pSG=new HugeQuadTreeSG;
	m_pSG->BindCamera(pCamera);
	m_pSG->LoadFromFile(Kernel::Inst()->DefaultFS(),szMapName);

	m_pSG->SetViewZoneSize(HugeQuadTreeSG::EVZT_LitNode,
		Vector3(BuiltMapView::ViewDistSN,BuiltMapView::ViewDistSN,BuiltMapView::ViewDistSN));
	m_pSG->SetViewZoneSize(HugeQuadTreeSG::EVZT_MidNode,
		Vector3(BuiltMapView::ViewDistMN,BuiltMapView::ViewDistMN,BuiltMapView::ViewDistMN));
	m_pSG->SetViewZoneSize(HugeQuadTreeSG::EVZT_BigNode,
		Vector3(BuiltMapView::ViewDistBN,BuiltMapView::ViewDistBN,BuiltMapView::ViewDistBN));
	m_pSG->SetViewZoneSize(HugeQuadTreeSG::EVZT_DynamicNode,
		Vector3(BuiltMapView::ViewDistDN,BuiltMapView::ViewDistDN,BuiltMapView::ViewDistDN));
	m_pSG->SetViewZoneSize(HugeQuadTreeSG::EVZT_SimpleTerrain,
		Vector3(BuiltMapView::ViewDistST,BuiltMapView::ViewDistST,BuiltMapView::ViewDistST));

	NavResMgr::Inst()->EnableIOAsync(false);
	m_nav.LoadFromFile(szMapName,Kernel::Inst()->DefaultFS(),Kernel::Inst()->DefaultFS(),NavMap::ELO_Collider|NavMap::ELO_GroundType);
	//m_nav.SetViewZoneSize(Vector3(ViewDistSN,ViewDistSN,ViewDistSN));
	m_nav.SetViewZoneSize(Vector3(10000,10000,10000));
	m_nav.InvalidViewZone(MathConst::Zero3);
	NavResMgr::Inst()->EnableIOAsync(true);

	m_role.LoadDefaultAvatar();
	m_role.AttachToSceneGraph(m_pSG);
	SetRolePos(0,0);

	ResMgr::Inst()->EnableIOAsync(true);

	//--设置太阳光照 
	TCHAR szPath[512];
	_stprintf_s(szPath,sizeof(szPath)/sizeof(TCHAR),_T(".\\data\\map\\%s\\%s.mb"),szMapName,szMapName);

	m_gameMap.LoadFromFile(Kernel::Inst()->DefaultFS(),szPath);
	//--使用动态平行光的颜色
	tagLight sunLight = m_gameMap.GetSunLight();
	sunLight.diffuse = m_gameMap.GetDynamicDiffCol();
	sunLight.diffuse.R *= m_gameMap.GetSunModulus();
	sunLight.diffuse.G *= m_gameMap.GetSunModulus();
	sunLight.diffuse.B *= m_gameMap.GetSunModulus();
	sunLight.ambient = m_gameMap.GetDynamicAmbCol();
	sunLight.specular = m_gameMap.GetDynamicSpecCol();
	m_pSG->SetSunLight(sunLight);

	//--设置天空盒
	SGSkyBox *pSkyBox=new SGSkyBox;
	pSkyBox->SetRotate(_DegToRad(m_gameMap.GetSkyYaw()),0.0f,0.0f);
	Vector3 sz;
	TCHAR *szTopFile=m_gameMap.GetSkyBoxTop();
	if (szTopFile!=NULL
		&& _tcslen(szTopFile)>0)
	{
		m_gameMap.GetSkyBoxSize(sz);
		pSkyBox->Create(m_gameMap.GetSkyBoxTop(),m_gameMap.GetSkyBoxFrontRight(),
		m_gameMap.GetSkyBoxBackLeft(),sz);
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
			pSunNode->EnableFlare( true );
			m_pSG->SetSunNode( pSunNode );
		}
		else
			SAFE_DELETE( pSunNode );
	}

	//--设置雾
	tagDistFog fog;
	m_gameMap.GetFog(fog);
	fog.start*=BuiltMapView::ViewDistBN;
	fog.end*=BuiltMapView::ViewDistBN;
	m_pSG->SetSceneFog(fog);

	//--释放地图区域
	for(int i=0;i<(int)m_mapAreaRgn.size();i++)
		::DeleteObject(m_mapAreaRgn[i]);
	m_mapAreaRgn.clear();

	//--创建地图区域
	int i;
	const vector<tagMapArea>& mapAreas=m_gameMap.GetMapRect();
	for(i=0;i<(int)mapAreas.size();i++)
	{
		const tagMapArea& mapArea=mapAreas[i];
		ASSERT(mapArea.region.size()>0);

		vector<POINT> points;
		for(int j=0;j<(int)mapArea.region.size();j++)
		{
			POINT pt=World2Tile(mapArea.region[j]);
			points.push_back(pt);
		}

		HRGN hRgn=::CreatePolygonRgn(&points[0],(int)points.size(),ALTERNATE);
		m_mapAreaRgn.push_back(hRgn);
	}

	//--创建载具
	const vector<tagMapCarrier>& carriers=m_gameMap.GetMapCarrier();
	for(i=0;i<(int)carriers.size();++i)
	{
		const tagMapCarrier& carrier=carriers[i];

		Carrier* pCarrier=new Carrier;
		if(!pCarrier->Init(carrier,m_gameMap.GetWayPoints(),&m_nav))
		{
			delete pCarrier;
			continue;
		}
		pCarrier->AttachToSceneGraph(m_pSG);
		m_carriers.push_back(pCarrier);
	}



	return 1;
}

void BuiltMapView::Update()
{
	if(m_pSG)
	{
		m_pSG->InvalidViewZone();
		m_role.Update(this,&m_nav);
		m_nav.InvalidViewZone(m_role.GetPos());
		UpdateMapArea();
		
		int i;
		for(i=0;i<(int)m_carriers.size();i++)
			m_carriers[i]->Update(this,&m_nav);
	}
 }

bool BuiltMapView::RayCollide(const Ray& ray,Vector3& pos)
{
	return m_nav.GetCollider()->RayCollideBoxAndTrn(ray,pos);
}

POINT BuiltMapView::World2Tile(const Vector3& wpt)
{
	POINT pt;
	pt.x=int(wpt.x/50.0f);
	pt.y=int(wpt.z/50.0f);
	return pt;
}

Vector3 BuiltMapView::Tile2World( int tileX,int tileZ )
{
	Vector3 wpt;
	wpt.x=float(tileX)*50.0f;
	wpt.z=float(tileZ)*50.0f;
	return wpt;
}

void BuiltMapView::SetRolePos(int tileX,int tileZ)
{
	Vector3 wpt=Tile2World(tileX,tileZ);

	AABBox box;
	box.min.x=wpt.x-25.0f;
	box.max.x=wpt.x+25.0f;
	box.min.z=wpt.z-25.0f;
	box.max.z=wpt.z+25.0f;
	box.min.y=-100000.0f;
	box.max.y=100000.0f;
	m_nav.GetCollider()->AABBoxCollideBoxTopAndTrn(box,0.0f,wpt.y);

	m_role.SetPos(wpt);
}

void BuiltMapView::SetRolePos( const Vector3& pos )
{
	m_role.SetPos(pos);
}

Vector3 BuiltMapView::GetRolePos()
{
	return m_role.GetPos();
}

void BuiltMapView::OpenDoor( DWORD dwObjID )
{
	m_nav.GetCollider()->EnableDoorCollide(dwObjID,false);
}

void BuiltMapView::CloseDoor( DWORD dwObjID )
{
	m_nav.GetCollider()->EnableDoorCollide(dwObjID,true);
}

void BuiltMapView::UpdateMapArea()
{
	POINT roleTile=World2Tile(m_role.GetPos());

	int inAreaIndex=-1;
	const vector<tagMapArea>& mapAreas=m_gameMap.GetMapRect();
	for(int i=0;i<(int)mapAreas.size();i++)
	{
		const tagMapArea& mapArea=mapAreas[i];
		if( TRUE==::PtInRegion(m_mapAreaRgn[i],roleTile.x,roleTile.y) )
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
				if( NULL != inAreaEx
					&& NULL != mapAreaEx
					&& inAreaEx->byPriority < mapAreaEx->byPriority )
				{
					inAreaIndex=i;
				}
			}
		}
	}

	if( m_curMapAreaIndex>=0 && inAreaIndex==-1 )	//LeaveMapArea
	{
		OnLeaveMapArea( m_curMapAreaIndex );
		m_curMapAreaIndex = -1;
		return;
	}

	if( inAreaIndex>=0 && m_curMapAreaIndex!=inAreaIndex )	//EnterMapArea
	{
		m_curMapAreaIndex=inAreaIndex;
		OnEnterMapArea( m_curMapAreaIndex );
		return;
	}
}

void BuiltMapView::OnLeaveMapArea( const int nIndex )
{
	tagDistFog fog;
	m_gameMap.GetFog(fog);
	fog.start*=BuiltMapView::ViewDistBN;
	fog.end*=BuiltMapView::ViewDistBN;
	m_pSG->SwitchSceneFog(fog);
}

void BuiltMapView::OnEnterMapArea( const int nIndex )
{
	tagDistFog fog;
	if( m_gameMap.GetMapAreaFog( nIndex, fog ) )
	{
		fog.start*=BuiltMapView::ViewDistBN;
		fog.end*=BuiltMapView::ViewDistBN;
		m_pSG->SwitchSceneFog(fog);
	}
}
