#include "StdAfx.h"
#include ".\Carrier.h"
#include "MapView.h"
#include "ClientApp.h"

Carrier::Carrier(void)
{
	m_pSGNode=NULL;
	m_pNavNode=NULL;
}

Carrier::~Carrier(void)
{
	m_pSGNode=NULL;//自己不delete,而交给SceneGraph取delete
}

void Carrier::Update(MapView *pMap,NavMap *pNav)
{
	if(m_pNavNode==NULL
		||m_pSGNode==NULL)
		return;

	Vector3 nowPos;
	m_pNavNode->GetPos(Kernel::Inst()->GetAccumTime(),nowPos);
	m_space.SetPos(nowPos);
	
	if(m_pSGNode)
		m_pSGNode->SetWorldMat(m_space.GetMatrix());
}

bool Carrier::Init( const tagMapCarrier& carrier,const vector<tagMapWayPoint>& wayPoints,NavMap *pNav )
{
	m_pNavNode=pNav->GetCollider()->FindCarrier(carrier.dwObjID);
	if(m_pNavNode==NULL)
		return false;

	//设置旋转
	m_space.SetYaw(carrier.fYaw);

	//加载SceneNode
	m_pSGNode=new EntityNode;
	m_pSGNode->LoadRes(carrier.szModelPath);

	//添加导航点
	for(size_t i=0;i<wayPoints.size();++i)
	{
		const tagMapWayPoint& wayPoint=wayPoints[i];

		if(_tcscmp(carrier.szWayPoint,wayPoint.szName)==0)
		{
			m_pNavNode->AddWayPoint(wayPoint.vPos,2.0f);
		}
	}

	//设置速度
	m_pNavNode->SetSpeed(500.0f);

	//开始运行
	m_pNavNode->Run(Kernel::Inst()->GetAccumTime(),0.0f);

	return true;
}

void Carrier::AttachToSceneGraph( SceneGraph *pSG )
{
	pSG->AttachDynamicNode(m_pSGNode);
}