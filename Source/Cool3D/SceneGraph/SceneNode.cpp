#include "StdAfx.h"
#include ".\scenenode.h"

namespace Cool3D
{
	IMP_RTTI(SceneNode,RTTIObj);

	SceneNode::SceneNode(void)
		:m_parent(0), m_staticNodeID(0)
	{
	}

	SceneNode::~SceneNode(void)
	{
		vector<SceneNode*>::iterator iter;
		for(iter=m_children.begin();iter!=m_children.end();iter++)
		{
			SceneNode *pNode=(*iter);
			delete pNode;
		}
		m_children.clear();
		m_parent=NULL;
	}

	void SceneNode::Serialize(FArchive& ar)
	{
		ar<<m_staticNodeID;
		ar<<m_szLightMap;
		ar<<m_szUserData;
		//--处理所有children
		SerialObjPtrVector(ar,m_children);
	}

	void SceneNode::Deserialize(FArchive& ar)
	{
		ar>>m_staticNodeID;
		ar>>m_szLightMap;
		ar>>m_szUserData;
		ASSERT(m_children.size()==0);
		vector<SceneNode*> tmp;
		DeserialObjPtrVector(ar,tmp);
		for(vector<SceneNode*>::iterator iter=tmp.begin();
			iter!=tmp.end();++iter)
			AttachChild(*iter);

	}

	void SceneNode::DetachChild(const SceneNode *pNode)
	{
		vector<SceneNode*>::iterator iter;
		for(iter=m_children.begin();iter!=m_children.end();iter++)
		{
			SceneNode *pN=(*iter);
			if(pN==pNode)
			{
				m_children.erase(iter);
				pN->m_parent=NULL;
				break;
			}
		}
	}

	void SceneNode::DetachChild(const TCHAR* szClassName)
	{
		vector<SceneNode*>::iterator iter;
		for(iter=m_children.begin();iter!=m_children.end();)
		{
			SceneNode *pN=(*iter);
			if(pN->IsStaticClass(szClassName))
			{
				iter=m_children.erase(iter);
				pN->m_parent=NULL;
			}
			else
				iter++;
		}
	}

	void SceneNode::Update(float deltaTime,SceneGraph *pSG)
	{
		//--调用所有children的update函数
		vector<SceneNode*>::iterator iter;
		for(iter=m_children.begin();iter!=m_children.end();iter++)
		{
			SceneNode *pNode=(*iter);
			pNode->Update(deltaTime,pSG);
		}
	}

	void SceneNode::ForceUpdate(float deltaTime,SceneGraph *pSG)
	{
		//--调用所有children的ForceUpdate函数
		vector<SceneNode*>::iterator iter;
		for(iter=m_children.begin();iter!=m_children.end();iter++)
		{
			SceneNode *pNode=(*iter);
			pNode->ForceUpdate(deltaTime,pSG);
		}
	}

	UINT SceneNode::GetNumOccluder()
	{
		return m_occluders.size();
	}
	Occluder* SceneNode::GetOccluder(UINT i)
	{
		ASSERT(i<GetNumOccluder());
		return m_occluders[i];
	}
	
	TResult SceneNode::RayCollide(const Ray& ray)
	{ 
		return TResult(false,0); 
	}

}//namespace Cool3D