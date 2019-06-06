#include "StdAfx.h"
#include ".\scenegraph.h"
#include "..\Util\Exception.h"
#include "SGSkyNode.h"
#include "SGSunNode.h"
#include "..\SpecialEffect\SGEffectNode.h"
#include "..\Kernel.h"

namespace Cool3D
{
	IMP_RTTI(SceneGraph,RTTIObj);

	SceneGraph::SceneGraph(void)
	{
		m_pCamera=NULL;
	
		m_sunLight.type=Light_Direct;
		float am=1.0f;
		m_sunLight.ambient=Color4f(1.0f,am,am,am);
		float bright=1.0f;
		m_sunLight.diffuse=Color4f(1.0f,bright,bright,bright);
		m_sunLight.dir=Vector3(1,-1,1);
		D3DXVec3Normalize(&m_sunLight.dir,&m_sunLight.dir);
		m_sunLight.specular=Color4f(1.0f,1.0f,1.0f,1.0f);
		m_sceneFog.color=Color4ub(250,60,100,180);
		m_sceneFog.start=16*50;
		m_sceneFog.end=32*50;
		m_sceneFog.density=1.0f;//todo

		//--计算lod distance
		float r[MAX_LOD]={0.125f,0.25f,0.55f,0.75f,1.0f,1.25f,1.5f,2.0f,3.0f,4.0f};
		float refDist=512*50.0f;//LOD计算参照距离(512个格子)
		for(int i=0;i<MAX_LOD;i++)
		{
			m_lodDistSq[i]=powf(refDist*r[i],2);
		}

		m_pSkyNode=NULL;
		m_pSunNode=NULL;

		m_bSyncInit=false;

		m_bFogFading=false;
		m_fFogSwitchTime=0.0f;
		m_fFogFadeTime=5.0f;
		m_lastFog=m_sceneFog;
		m_nextFog=m_sceneFog;
	}

	SceneGraph::~SceneGraph(void)
	{
		SAFE_DELETE(m_pSkyNode);
		SAFE_DELETE(m_pSunNode);
	}

	SceneGraph* SceneGraph::NewSceneGraph(const TCHAR* szClassName)
	{
		return (SceneGraph*)RTTFactory::Inst()->CreateObj(szClassName);
	}

	void SceneGraph::BindCamera(CameraBase *pCamera,const bool bSetDefaultFogDist/*=true*/)
	{
		m_pCamera=pCamera; 

		ASSERT(m_pCamera!=NULL);

		if( bSetDefaultFogDist )
		{
			//--设置默认fog参数
			float rang=pCamera->GetZFar()-pCamera->GetZNear();
			m_sceneFog.start=pCamera->GetZFar()*0.1f;
			m_sceneFog.end=pCamera->GetZFar()*0.75f;
		}
	}

	int SceneGraph::CalcDetailLevel(const AABBox& box,const Matrix4* pWorldMat)
	{
		ASSERT(m_pCamera!=NULL);

		//--
		AABBox tmpBox=box;
		if(pWorldMat)
			tmpBox.Transform(*pWorldMat);
		float distSq=tmpBox.DistToPointSq(m_pCamera->GetLookat());
	
		//-- lod
		for(int i=0;i<MAX_LOD;i++)
		{
			if(distSq<m_lodDistSq[i])
				return i;
		}
		return MAX_LOD;
	}

	void SceneGraph::SetSkyNode(SGSkyNode *pNode)
	{
		SAFE_DELETE(m_pSkyNode);
		m_pSkyNode=pNode;
 	}

	SGSkyNode* SceneGraph::GetSkyNode() const
	{
		return m_pSkyNode;
	}

	void SceneGraph::Render(float deltaTime)
	{
		if(m_pSkyNode!=NULL)
			m_pSkyNode->Update(deltaTime,this);
		if(m_pSunNode!=NULL)
			m_pSunNode->Update(deltaTime,this);

		if(m_bFogFading)
			UpdataSceneFogFade();
	}

	void SceneGraph::SetSunNode(SGSunNode *pNode)
	{
		SAFE_DELETE(m_pSunNode);
		m_pSunNode=pNode;
	}

	SGSunNode* SceneGraph::GetSunNode() const
	{
		return m_pSunNode;
	}

	void SceneGraph::SwitchSceneFog( const tagDistFog& fog )
	{
		m_lastFog = m_sceneFog;
		m_nextFog = fog;
		m_fFogSwitchTime = Kernel::Inst()->GetAccumTime();
		m_bFogFading = true;
	}

	void SceneGraph::UpdataSceneFogFade()
	{
		float passTime = Kernel::Inst()->GetAccumTime() - m_fFogSwitchTime;
		if( passTime > m_fFogFadeTime )
		{
			SetSceneFog( m_nextFog );
			m_bFogFading = false;
		}
		else
		{
			float r = passTime / m_fFogFadeTime;
			SetSceneFog( DistFogLerp( m_lastFog, m_nextFog, r ) );
		}
	}
}//namespace Cool3D