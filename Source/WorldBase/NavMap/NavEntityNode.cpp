#include "stdafx.h"
#include "NavEntityNode.h"
#include "NavResSceneMesh.h"
#include "NavSceneGraph.h"
#include "NavResMgr.h"

namespace WorldBase
{
	IMP_RTTI_DYNC(NavEntityNode,NavSceneNode);


	NavEntityNode::NavEntityNode()
	{
		m_pRes=NULL;
		m_bIsCollide=true;
		m_scale=Vector3(1,1,1);
		m_scaleInv=Vector3(1,1,1);
		m_surfaceType=EST_None;
	}

	NavEntityNode::~NavEntityNode()
	{
		NAV_RES_MGR_RELEASE_RES(m_pRes);
	}

	void NavEntityNode::Serialize( FArchive& ar )
	{
		ar<<m_pRes->GetName();
		ar<<m_toWorld;
		ar<<m_worldBox.max;
		ar<<m_worldBox.min;
		ar<<m_bIsCollide;
		ar<<m_scale;
		ar<<m_scaleInv;
		int tmp=m_surfaceType;
		ar<<tmp;

		NavSceneNode::Serialize(ar);
	}

	void NavEntityNode::Deserialize( FArchive& ar )
	{
		ar>>m_szResName;
		ar>>m_toWorld;
		ar>>m_worldBox.max;
		ar>>m_worldBox.min;
		ar>>m_bIsCollide;
		ar>>m_scale;
		ar>>m_scaleInv;
		int tmp;
		ar>>tmp;
		m_surfaceType=(ESurfaceType)tmp;

		D3DXMatrixInverse(&m_toWorldInv,NULL,&m_toWorld);

		LoadRes(m_szResName.c_str());

		NavSceneNode::Deserialize(ar);
	}

	void NavEntityNode::LoadRes( const TCHAR *szName )
	{
		ASSERT(m_pRes==NULL);
		m_pRes=(NavResSceneMesh*)NavResMgr::Inst()->NewRes(szName);
	}

	NavSceneNode* NavEntityNode::Clone()
	{
		NavEntityNode* pNewNode=new NavEntityNode;
		Clone(pNewNode);
		return pNewNode;
	}

	void NavEntityNode::Clone( NavEntityNode* pTarget )
	{
		pTarget->m_pRes		= (NavResSceneMesh*)NavResMgr::Inst()->NewRes(m_szResName.c_str());
		pTarget->m_toWorld		= m_toWorld;
		pTarget->m_toWorldInv	= m_toWorldInv;
		pTarget->m_worldBox	= m_worldBox;
		pTarget->m_szResName	= m_szResName;
		pTarget->m_bIsCollide	= m_bIsCollide;
		pTarget->m_scale		= m_scale;
		pTarget->m_scaleInv	= m_scaleInv;

		pTarget->m_dwFlags			= NavSceneNode::m_dwFlags;
		pTarget->m_bEnableCollide	= NavSceneNode::m_bEnableCollide;
		pTarget->m_dwObjID			= NavSceneNode::m_dwObjID;
	}

	bool NavEntityNode::AABBoxCollideBoxTopHighest( const AABBox& box,float curTime,float& intersectY )
	{
		if(!m_worldBox.Intersects(box))
			return false;

		if(m_pRes!=NULL
			&&m_pRes->IsCreated())
		{
			//--将参数从world空间变换到object空间
			AABBox localBox=box;
			//localBox.TransformFast(m_toWorldInv,m_scaleInv);
			localBox.Transform(m_toWorldInv);

			Vector3 intersectPos(0,intersectY,0);
			Vector3 localPos;
			D3DXVec3TransformCoord(&localPos,&intersectPos,&m_toWorldInv);

			//--
			if(!m_pRes->AABBoxCollideBoxTopHighest(localBox,localPos.y))
				return false;

			//--将碰撞点从object空间变换到world空间
			D3DXVec3TransformCoord(&intersectPos,&localPos,&m_toWorld);
			intersectY=intersectPos.y;

			return true;
		}

		return false;
	}

	bool NavEntityNode::QuerySurfaceType( const AABBox& box,float curTime,float& intersectY,ESurfaceType& out )
	{
		if(!m_worldBox.Intersects(box))
			return false;

		if(m_pRes!=NULL
			&&m_pRes->IsCreated())
		{
			//--将参数从world空间变换到object空间
			AABBox localBox=box;
			localBox.TransformFast(m_toWorldInv,m_scaleInv);

			Vector3 intersectPos(0,intersectY,0);
			Vector3 localPos;
			D3DXVec3TransformCoord(&localPos,&intersectPos,&m_toWorldInv);

			//--
			if(!m_pRes->AABBoxCollideBoxTopHighest(localBox,localPos.y))
				return false;

			//--将碰撞点从object空间变换到world空间
			D3DXVec3TransformCoord(&intersectPos,&localPos,&m_toWorld);
			intersectY=intersectPos.y;

			out=m_surfaceType;
			return true;
		}

		return false;
	}

	bool NavEntityNode::AABBoxCollideBox( const AABBox& box,float curTime )
	{
		if(!m_worldBox.Intersects(box))
			return false;

		if(m_pRes!=NULL
			&&m_pRes->IsCreated())
		{
			//--将参数从world空间变换到object空间
			AABBox localBox=box;
			//localBox.TransformFast(m_toWorldInv,m_scaleInv);
			localBox.Transform(m_toWorldInv);

			//--
			return m_pRes->AABBoxCollideBox(localBox);
		}

		return false;
	}

	void NavEntityNode::GetBox( AABBox& box )
	{
		box=m_worldBox;
	}

	TResult NavEntityNode::RayCollideBox( const Ray& ray )
	{
		TResult tr=ray.Intersect(m_worldBox);
		if(tr.first)
		{
			if(m_pRes!=NULL
				&&m_pRes->IsCreated())
			{
				//--ray从world空间变换到object空间
				Ray localRay=ray;
				D3DXVec3TransformCoord(&localRay.origin,&ray.origin,&m_toWorldInv);
				D3DXVec3TransformNormal(&localRay.dir,&ray.dir,&m_toWorldInv);

				//--
				tr=m_pRes->RayCollideBox(localRay);
				return tr;
			}
		}

		return TResult(false,0);
	}

	bool NavEntityNode::RayCollideBoxOnce(const Ray& ray)
	{
		TResult tr=ray.Intersect(m_worldBox);
		if(tr.first)
		{
			if(m_pRes!=NULL
				&&m_pRes->IsCreated())
			{
				//--ray从world空间变换到object空间
				Ray localRay=ray;
				D3DXVec3TransformCoord(&localRay.origin,&ray.origin,&m_toWorldInv);
				D3DXVec3TransformNormal(&localRay.dir,&ray.dir,&m_toWorldInv);

				//--
				return m_pRes->RayCollideBoxOnce(localRay);
			}
		}

		return false;
	}

	void NavEntityNode::SetWorldMat( const Matrix4& worldMat )
	{
		if(m_pRes!=NULL
			&&m_pRes->IsCreated())
		{
			m_toWorld=worldMat;
			m_worldBox=m_pRes->GetBox();
			m_worldBox.Transform(worldMat);

			D3DXMatrixInverse(&m_toWorldInv,NULL,&m_toWorld);
		}
	}

	const Matrix4& NavEntityNode::GetWorldMat() const
	{
		return m_toWorld;
	}

	const Matrix4& NavEntityNode::GetWorldMatInv() const
	{
		return m_toWorldInv;
	}

	void NavEntityNode::SetScale( float xScale,float yScale,float zScale )
	{
		m_scale.x=xScale;
		m_scale.y=yScale;
		m_scale.z=zScale;
		m_scaleInv.x=1/xScale;
		m_scaleInv.y=1/yScale;
		m_scaleInv.z=1/zScale;
	}

	void NavEntityNode::SetSurfaceType(ESurfaceType type)
	{
		m_surfaceType=type;
	}

	ESurfaceType NavEntityNode::GetSurfaceType()
	{
		return m_surfaceType;
	}

	void NavEntityNode::DrawBox()
	{
		if(m_pRes!=NULL
			&&m_pRes->IsCreated())
		{
			m_pRes->DrawBox();
		}
	}

	const TCHAR* NavEntityNode::GetResName()
	{
		if( m_pRes!=NULL )
		{
			return m_pRes->GetName();
		}
		return m_szResName.c_str();
	}
}//namespace WorldBase