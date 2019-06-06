#include "StdAfx.h"
#include ".\NavSceneNode.h"
#include ".\NavQuadTreeData.h"
#include ".\NavSGQuadTreeNode.h"
#include ".\NavColliderCache.h"
#include ".\NavQuadTreeSG.h"
#include ".\NavCarrierNode.h"

namespace WorldBase
{
	IMP_RTTI(NavSGQuadTreeNode,RTTIObj);

	NavSGQuadTreeNode::NavSGQuadTreeNode(NavQuadTreeSG* pSG):m_pSG(pSG)
	{
		m_id=-1;
		m_lv=0;
		memset(m_pChildren,0,sizeof(m_pChildren));
		for(int i=0;i<4;i++)
			m_childrenID[i]=-1;
	}

	NavSGQuadTreeNode::~NavSGQuadTreeNode(void)
	{
		//--从SceneGraph注销动态地物
		UnRegisterDynamicNodesFromSG();

		FreePtrVector<NavSceneNode> freeSceneNodes;
		freeSceneNodes(m_pContent);

		for(int i=0;i<4;i++)
			SAFE_DELETE(m_pChildren[i]);
	}

	void NavSGQuadTreeNode::InvalidViewData(const AABBox& viewZone,NavQuadTreeData *pDataSrc)
	{
		for(int i=0;i<4;i++)
		{
			if(m_childrenID[i]!=-1)
			{
				if(viewZone.Intersects(m_childrenBox[i]))
				{
					if(m_pChildren[i]==NULL)
						m_pChildren[i]=pDataSrc->LoadNode(m_childrenID[i],m_pSG);
				}
				else
				{
					if(m_pChildren[i]!=NULL)
					{
						delete m_pChildren[i];
						m_pChildren[i]=NULL;
					}
				}
				//--递归调用所有可能可见的子节点
				if(m_pChildren[i]!=NULL)
					m_pChildren[i]->InvalidViewData(viewZone,pDataSrc);
			}//endof if
		}//endof for
	}

	void NavSGQuadTreeNode::Serialize(FArchive& ar)
	{
		ar<<m_id;
		ar<<m_box.max;
		ar<<m_box.min;

		int i;
		for(i=0;i<4;i++)
			ar<<m_childrenID[i];
		for(i=0;i<4;i++)
		{
			ar<<m_childrenBox[i].max;
			ar<<m_childrenBox[i].min;
		}
		
		SerialObjPtrVector(ar,m_pContent);

		//--不保存QuadTreeNode的子节点
	}

	void NavSGQuadTreeNode::Deserialize(FArchive& ar)
	{
		ar>>m_id;
		ar>>m_box.max;
		ar>>m_box.min;

		int i;
		for(i=0;i<4;i++)
			ar>>m_childrenID[i];
		for(i=0;i<4;i++)
		{
			ar>>m_childrenBox[i].max;
			ar>>m_childrenBox[i].min;
		}
		
		DeserialObjPtrVector(ar,m_pContent);

		//--将动态地物注册到SceneGraph
		RegisterDynamicNodesToSG();
	}

	void NavSGQuadTreeNode::Clone(NavSGQuadTreeNode* pTarget)
	{
		pTarget->m_id=m_id;
		pTarget->m_lv=m_lv;
		pTarget->m_box=m_box;

		memcpy(pTarget->m_childrenID,m_childrenID,sizeof(m_childrenID));
		memcpy(pTarget->m_childrenBox,m_childrenBox,sizeof(m_childrenBox));

		//--clone scene node
		int i;
		ASSERT(pTarget->m_pContent.size()==0);
		for(i=0;i<(int)m_pContent.size();i++)
		{
			NavSceneNode* pNewNode=m_pContent[i]->Clone();
			pTarget->m_pContent.push_back(pNewNode);
		}

		//--clone children node
		for(i=0;i<4;i++)
		{
			ASSERT(pTarget->m_pChildren[i]==NULL);
			if(m_pChildren[i]!=NULL)
			{
				NavSGQuadTreeNode* pNewNode=new NavSGQuadTreeNode(m_pSG);
				m_pChildren[i]->Clone(pNewNode);
				pTarget->m_pChildren[i]=pNewNode;
			}
		}
	}

	bool NavSGQuadTreeNode::AABBoxCollideBoxTopHighest( const AABBox& box,float curTime,float& intersectY,DWORD dwFlags,NavSceneNode** ppSceneNode)
	{
		bool bRet=false;

		//--
		for(vector<NavSceneNode*>::iterator iter=m_pContent.begin();
			iter!=m_pContent.end();++iter)
		{
			NavSceneNode* pSceneNode=*iter;

			if((dwFlags&pSceneNode->GetFlags())!=dwFlags)
				continue;

			if(!pSceneNode->IsEnableCollide())
				continue;

			if(pSceneNode->AABBoxCollideBoxTopHighest(box,curTime,intersectY))
			{
				*ppSceneNode=pSceneNode;
				bRet=true;
			}
		}

		//--递归调用所有子节点
		for(int i=0;i<4;i++)
		{
			if(m_pChildren[i]
				&&m_pChildren[i]->GetBox().Intersects(box))
			{
				if(m_pChildren[i]->AABBoxCollideBoxTopHighest(box,curTime,intersectY,dwFlags,ppSceneNode))
					bRet=true;

				//if(m_pChildren[i]->GetBox().Contain(box))
				//	break;
			}
		}

		return bRet;
	}

	bool NavSGQuadTreeNode::QuerySurfaceType( const AABBox& box,float curTime,float& intersectY,ESurfaceType& out )
	{
		bool bRet=false;

		//--
		for(vector<NavSceneNode*>::iterator iter=m_pContent.begin();
			iter!=m_pContent.end();++iter)
		{
			NavSceneNode* pSceneNode=*iter;

			if(!pSceneNode->IsEnableCollide())
				continue;

			if(pSceneNode->QuerySurfaceType(box,curTime,intersectY,out))
			{
				bRet=true;
			}
		}

		//--递归调用所有子节点
		for(int i=0;i<4;i++)
		{
			if(m_pChildren[i]
				&&m_pChildren[i]->GetBox().Intersects(box))
			{
				if(m_pChildren[i]->QuerySurfaceType(box,curTime,intersectY,out))
					bRet=true;

				//if(m_pChildren[i]->GetBox().Contain(box))
				//	break;
			}
		}

		return bRet;
	}

	bool NavSGQuadTreeNode::AABBoxCollideBox( const AABBox& box,float curTime,DWORD dwFlags,NavSceneNode** ppSceneNode )
	{
		//--
		for(vector<NavSceneNode*>::iterator iter=m_pContent.begin();
			iter!=m_pContent.end();++iter)
		{
			NavSceneNode* pSceneNode=*iter;

			if(!pSceneNode->IsEnableCollide())
				continue;

			if((dwFlags&pSceneNode->GetFlags())!=dwFlags)
				continue;

			if(pSceneNode->AABBoxCollideBox(box,curTime))
			{
				*ppSceneNode=pSceneNode;
				return true;
			}
		}

		//--递归调用所有子节点
		for(int i=0;i<4;i++)
		{
			if(m_pChildren[i]
				&&m_pChildren[i]->GetBox().Intersects(box))
			{
				if(m_pChildren[i]->AABBoxCollideBox(box,curTime,dwFlags,ppSceneNode))
					return true;
				//if(m_pChildren[i]->GetBox().Contain(box))
				//	break;
			}
		}

		return false;
	}

	bool NavSGQuadTreeNode::RayCollideBox( const Ray& ray,Vector3& intersectPos )
	{
		TResult tr(false,FLOAT_MAX);
		RayCollideR(ray,tr);
		if(tr.first)
		{
			Vector3 dir=ray.dir;
			Vec3Normalize(dir);
			intersectPos=ray.origin+dir*tr.second;
		}
		return tr.first;
	}

	bool NavSGQuadTreeNode::RayCollideBox( const Ray& ray,DWORD dwFlags )
	{
		//先判断与四叉树box的相交
		TResult tr=ray.Intersect(m_box);
		if(!tr.first)
			return false;

		//判断每个scene node
		for(vector<NavSceneNode*>::iterator iter=m_pContent.begin();
			iter!=m_pContent.end();++iter)
		{
			NavSceneNode *pNode=(*iter);

			if(!pNode->IsEnableCollide())
				continue;

			if((dwFlags&pNode->GetFlags())!=dwFlags)
				continue;

			if(pNode->RayCollideBoxOnce(ray))
				return true;
		}//endof for

		//--递归调用所有子节点
		for(int i=0;i<4;i++)
		{
			if(m_pChildren[i]
				&&m_pChildren[i]->RayCollideBox(ray,dwFlags))
				return true;
		}

		return false;
	}

	void NavSGQuadTreeNode::RayCollideR( const Ray& ray,TResult& lastTR )
	{
		//先判断与四叉树box的相交
		TResult tr=ray.Intersect(m_box);
		if(!tr.first)
			return;

		//判断每个scene node
		for(vector<NavSceneNode*>::iterator iter=m_pContent.begin();
			iter!=m_pContent.end();++iter)
		{
			NavSceneNode *pNode=(*iter);

			if(!pNode->IsEnableCollide())
				continue;

			tr=pNode->RayCollideBox(ray);
			if(tr.first
				&& tr.second<lastTR.second)
			{
				lastTR=tr;
			}
		}//endof for

		//--递归调用所有子节点
		for(int i=0;i<4;i++)
		{
			if(m_pChildren[i])
				m_pChildren[i]->RayCollideR(ray,lastTR);
		}
	}

	void NavSGQuadTreeNode::AddSceneNodeToCache( NavColliderCache* pCache )
	{
	}

	void NavSGQuadTreeNode::RegisterDynamicNodesToSG()
	{
		if(m_pSG==NULL)
			return;

		for(vector<NavSceneNode*>::iterator iter=m_pContent.begin();
			iter!=m_pContent.end();++iter)
		{
			NavSceneNode *pNode=(*iter);

			//door
			if(pNode->GetFlags()&NavSceneNode::EFlag_Door)
				m_pSG->RegisterDoor(pNode->GetObjID(),pNode);

			//carrier
			if(pNode->IS_KIND_OF(NavCarrierNode))
				m_pSG->RegisterCarrier(pNode->GetObjID(),(NavCarrierNode*)pNode);
		}
	}

	void NavSGQuadTreeNode::UnRegisterDynamicNodesFromSG()
	{
		if(m_pSG==NULL)
			return;

		for(vector<NavSceneNode*>::iterator iter=m_pContent.begin();
			iter!=m_pContent.end();++iter)
		{
			NavSceneNode *pNode=(*iter);

			//door
			if(pNode->GetFlags()&NavSceneNode::EFlag_Door)
				m_pSG->UnregisterDoor(pNode->GetObjID());

			//carrier
			if(pNode->IS_KIND_OF(NavCarrierNode))
				m_pSG->UnregisterCarrier(pNode->GetObjID());
		}
	}
}//namespace WorldBase
