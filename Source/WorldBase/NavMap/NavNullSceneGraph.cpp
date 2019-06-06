#include "StdAfx.h"
#include "NavNullSceneGraph.h"
#include "NavEntityNode.h"

namespace WorldBase
{

	NavNullSceneGraph::NavNullSceneGraph(void)
	{
	}

	NavNullSceneGraph::~NavNullSceneGraph(void)
	{
		for(list<NavEntityNode*>::iterator iter=m_sceneNodes.begin();
			iter!=m_sceneNodes.end();iter++)
		{
			NavEntityNode* pEntity=*iter;
			SAFE_DELETE(pEntity);
		}
		m_sceneNodes.clear();
		m_pCurNode = NULL;
	}

	void NavNullSceneGraph::AttachNode( NavEntityNode* pNode )
	{
		ASSERT(pNode);
		m_sceneNodes.push_back(pNode);
	}

	void NavNullSceneGraph::DetachNode( NavEntityNode* pNode )
	{
		if(pNode==NULL)
			return;
		m_sceneNodes.remove(pNode);
		m_pCurNode = NULL;
	}

	bool NavNullSceneGraph::AABBoxCollideBox( const AABBox& box, float curTime, DWORD dwFlags, DWORD* pdwCarrierObjID )
	{
		for(list<NavEntityNode*>::iterator iter=m_sceneNodes.begin();
			iter!=m_sceneNodes.end();iter++)
		{
			NavEntityNode *pNode=(*iter);
			if(pNode->AABBoxCollideBox(box,curTime))
				return true;
		}

		return false;
	}

	bool NavNullSceneGraph::AABBoxCollideBox( const AABBox& box,const Vector3& maxIgnoreMdlSize )
	{
		for(list<NavEntityNode*>::iterator iter=m_sceneNodes.begin();
			iter!=m_sceneNodes.end();iter++)
		{
			NavEntityNode *pNode=(*iter);

			AABBox mdlBox;
			pNode->GetBox(mdlBox);

			if(mdlBox.max.x-mdlBox.min.x<=maxIgnoreMdlSize.x
				&&mdlBox.max.y-mdlBox.min.y<=maxIgnoreMdlSize.y
				&&mdlBox.max.z-mdlBox.min.z<=maxIgnoreMdlSize.z)
				continue;

			if(pNode->AABBoxCollideBox(box,0.0f))
				return true;
		}

		return false;
	}

	bool NavNullSceneGraph::RayCollideBox( const Ray& ray,Vector3& intersectPos )
	{
		TResult lastTR(false,FLOAT_MAX);

		for(list<NavEntityNode*>::iterator iter=m_sceneNodes.begin();
			iter!=m_sceneNodes.end();iter++)
		{
			NavEntityNode *pNode=(*iter);
			if(m_pCurNode == pNode)
				continue;
			TResult tr=pNode->RayCollideBox(ray);
			if(tr.first
				&& tr.second<lastTR.second)
			{
				lastTR=tr;
			}
		}

		if(lastTR.first)
		{
			Vector3 dir=ray.dir;
			Vec3Normalize(dir);
			intersectPos=ray.origin+dir*lastTR.second;
		}

		return lastTR.first;
	}

	bool NavNullSceneGraph::AABBoxCollideBoxTop( const AABBox& box,float curTime,float& intersectY,DWORD dwFlags,DWORD* pdwCarrierObjID )
	{
		bool bCollide=false;

		intersectY=box.min.y;
		for(list<NavEntityNode*>::iterator iter=m_sceneNodes.begin();
			iter!=m_sceneNodes.end();iter++)
		{
			NavEntityNode *pNode=(*iter);
			if(pNode->AABBoxCollideBoxTopHighest(box,curTime,intersectY))
				bCollide=true;
		}

		return bCollide;
	}
}//namespace WorldBase