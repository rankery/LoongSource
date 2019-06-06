#include "StdAfx.h"
#include ".\navquadtreesg.h"
#include "NavSGQuadTreeNode.h"
#include "NavColliderCache.h"
#include "NavEntityNode.h"

namespace WorldBase
{
	IMP_RTTI_DYNC(NavQuadTreeSG,NavSceneGraph);

	NavQuadTreeSG::NavQuadTreeSG(void)
	{
		m_pRoot=NULL;
	}

	NavQuadTreeSG::~NavQuadTreeSG(void)
	{
		SAFE_DELETE(m_pRoot);
	}

	void NavQuadTreeSG::LoadFromFile(IFS *pFS,const TCHAR* szMapName)
	{
		TCHAR szPath[512];
		_stprintf(szPath,_T("data\\map\\%s\\nav\\navmap.fsg"),szMapName,szMapName);

		m_data.OpenFromFile(pFS,szPath);
		m_pRoot=m_data.LoadNode(0,this);
		ASSERT(m_pRoot!=NULL);
	}

	void NavQuadTreeSG::InvalidViewZone(const Vector3& zoneCenter)
	{
		if(m_pRoot!=NULL)
		{
			AABBox viewZone;
			CalCurrentViewZone(zoneCenter,viewZone);
			m_pRoot->InvalidViewData(viewZone,&m_data);
		}
	}

	bool NavQuadTreeSG::AABBoxCollideBoxTop( const AABBox& box,float curTime,float& intersectY,DWORD dwFlags,DWORD* pdwCarrierObjID)
	{
		if(m_pRoot!=NULL)
		{
			if(!m_pRoot->GetBox().Intersects(box))
				return false;

			NavSceneNode* pSceneNode=NULL;
			if(m_pRoot->AABBoxCollideBoxTopHighest(box,curTime,intersectY,dwFlags,&pSceneNode))
			{
				if(pdwCarrierObjID!=NULL)
				{
					*pdwCarrierObjID=-1;

					if(pSceneNode->GetFlags()&NavSceneNode::EFlag_Carrier)
						*pdwCarrierObjID=pSceneNode->GetObjID();
				}
				return true;
			}
		}
		return false;
	}

	bool NavQuadTreeSG::QuerySurfaceType( const AABBox& box,float curTime,float& intersectY,ESurfaceType& out )
	{
		if(m_pRoot!=NULL)
		{
			if(!m_pRoot->GetBox().Intersects(box))
				return false;
			return m_pRoot->QuerySurfaceType(box,curTime,intersectY,out);
		}
		return false;
	}

	bool NavQuadTreeSG::AABBoxCollideBox( const AABBox& box,float curTime,DWORD dwFlags,DWORD* pdwCarrierObjID )
	{
		if(m_pRoot!=NULL)
		{
			if(!m_pRoot->GetBox().Intersects(box))
				return false;

			NavSceneNode* pSceneNode=NULL;
			if(m_pRoot->AABBoxCollideBox(box,curTime,dwFlags,&pSceneNode))
			{
				if(pdwCarrierObjID!=NULL)
				{
					*pdwCarrierObjID=-1;

					if(pSceneNode->GetFlags()&NavSceneNode::EFlag_Carrier)
						*pdwCarrierObjID=pSceneNode->GetObjID();
				}
				return true;
			}
		}
		return false;
	}

	bool NavQuadTreeSG::RayCollideBox( const Ray& ray,Vector3& intersectPos )
	{
		if(m_pRoot!=NULL)
			return m_pRoot->RayCollideBox(ray,intersectPos);
		return false;
	}

	bool NavQuadTreeSG::RayCollideBox( const Ray& ray,DWORD dwFlags)
	{
		if(m_pRoot!=NULL)
			return m_pRoot->RayCollideBox(ray,dwFlags);
		return false;
	}

	void NavQuadTreeSG::Clone(NavQuadTreeSG* pTarget)
	{
		if(m_pRoot!=NULL)
		{
			ASSERT(pTarget->m_pRoot==NULL);
			pTarget->m_pRoot=new NavSGQuadTreeNode(this);
			m_pRoot->Clone(pTarget->m_pRoot);
		}
	}

	void NavQuadTreeSG::AddSceneNodeToCache( NavColliderCache* pCache )
	{
		if(m_pRoot!=NULL)
		{
			if(!m_pRoot->GetBox().Intersects(pCache->GetBox()))
				return;
			return m_pRoot->AddSceneNodeToCache(pCache);
		}
		return;
	}

	void NavQuadTreeSG::RegisterDoor( DWORD dwObjID,NavSceneNode* pDoor )
	{
		m_doorMap[dwObjID]=pDoor;
	}

	void NavQuadTreeSG::UnregisterDoor( DWORD dwObjID )
	{
		m_doorMap.erase(dwObjID);
	}

	void NavQuadTreeSG::EnableAllDoorsCollide( bool bEnable )
	{
		for(map<DWORD,NavSceneNode*>::iterator iter=m_doorMap.begin();
			iter!=m_doorMap.end();++iter)
		{
			iter->second->EnableCollide(bEnable);
		}
	}

	void NavQuadTreeSG::EnableDoorCollide( DWORD dwObjID,bool bEnable )
	{
		map<DWORD,NavSceneNode*>::iterator iter=m_doorMap.find(dwObjID);
		if(iter!=m_doorMap.end())
		{
			iter->second->EnableCollide(bEnable);
		}
	}

	NavSceneNode* NavQuadTreeSG::FindDoor( DWORD dwObjID )
	{
		map<DWORD,NavSceneNode*>::iterator iter=m_doorMap.find(dwObjID);
		if(iter!=m_doorMap.end())
		{
			return iter->second;
		}
		return NULL;
	}

	void NavQuadTreeSG::RegisterCarrier( DWORD dwObjID,NavCarrierNode* pCarrier )
	{
		m_carrierMap[dwObjID]=pCarrier;
	}

	void NavQuadTreeSG::UnregisterCarrier( DWORD dwObjID )
	{
		m_carrierMap.erase(dwObjID);
	}

	NavCarrierNode* NavQuadTreeSG::FindCarrier( DWORD dwObjID )
	{
		map<DWORD,NavCarrierNode*>::iterator iter=m_carrierMap.find(dwObjID);
		if(iter!=m_carrierMap.end())
		{
			return iter->second;
		}
		return NULL;	
	}
}//namespace WorldBase