#include "stdafx.h"
#include "NavBintreeNode.h"
#include "NavBintreeCollider.h"

namespace WorldBase
{
	NavBintreeNode::NavBintreeNode( void )
		: m_pContents(NULL)
	{
		memset(m_pChildren,0,sizeof(m_pChildren));
	}

	NavBintreeNode::~NavBintreeNode( void )
	{
		for(int i=0;i<EChildNum;i++)
			SAFE_DELETE(m_pChildren[i]);
	}

	void NavBintreeNode::ReadFile( IFS *pFS,DWORD hFile )
	{
		ASSERT(pFS!=NULL);

		FReadValue(pFS,hFile,m_box.max);
		FReadValue(pFS,hFile,m_box.min);
		FReadValVector(pFS,hFile,m_content);

		if(m_content.size()>0)
			m_pContents=&m_content[0];
		else
			m_pContents=NULL;

		bool bChild=false;
		for(int i=0;i<EChildNum;i++)
		{
			FReadValue(pFS,hFile,bChild);
			if(bChild)
			{
				NavBintreeNode *pNode=new NavBintreeNode;
				pNode->ReadFile(pFS,hFile);
				m_pChildren[i]=pNode;
			}
		}
	}

	bool NavBintreeNode::AABBoxCollideBox( const AABBox& box,NavBintreeCollider* pCollider )
	{
		//--与自已的盒子列表碰
		if(m_content.size()>0)
		{
			for(UINT i=0;i<m_content.size();i++)
			{	
				const AABBox& collideBox=pCollider->GetCollideBox(m_pContents[i]);
				if(collideBox.Intersects(box))
					return true;
			}
		}

		//--递归子结点
		for(INT i=0;i<EChildNum;i++)
		{
			if(m_pChildren[i]!=NULL
				&&m_pChildren[i]->GetBox().Intersects(box))
			{
				if(m_pChildren[i]->AABBoxCollideBox(box,pCollider))
					return true;
			}
		}

		return false;
	}

	bool NavBintreeNode::AABBoxCollideBoxTopHighest( const AABBox& box,float& intersectY,NavBintreeCollider* pCollider )
	{
		bool bCollide=false;

		//--与自已的盒子列表碰
		if(m_content.size()>0)
		{
			for(UINT i=0;i<m_content.size();i++)
			{	
				const AABBox& collideBox=pCollider->GetCollideBox(m_pContents[i]);
				if(collideBox.AABBoxCollideBoxTopHighest(box,intersectY))
				{
					bCollide=true;
				}
			}
		}

		//--递归子结点
		for(INT i=0;i<EChildNum;i++)
		{
			if(m_pChildren[i]!=NULL
				&&m_pChildren[i]->GetBox().Intersects(box))
			{
				if(m_pChildren[i]->AABBoxCollideBoxTopHighest(box,intersectY,pCollider))
					bCollide=true;
			}
		}

		return bCollide;
	}

	Cool3D::TResult NavBintreeNode::RayCollideBox( const Ray& ray,NavBintreeCollider* pCollider )
	{
		TResult ret(false,FLOAT_MAX);

		//--先与自已的大盒子碰
		TResult tr=ray.Intersect(m_box);
		if(!tr.first)
			return ret;

		//--与自已的盒子列表碰
		for(UINT i=0;i<m_content.size();i++)
		{	
			const AABBox& box=pCollider->GetCollideBox(m_pContents[i]);
			tr=ray.Intersect(box);
			if(tr.first)
			{
				ret.first=true;
				if(tr.second<ret.second)
					ret.second=tr.second;
			}
		}

		//--递归子结点
		for(INT i=0;i<EChildNum;i++)
		{
			if(m_pChildren[i]!=NULL)
			{
				tr=m_pChildren[i]->RayCollideBox(ray,pCollider);
				if(tr.first)
				{
					ret.first=true;
					if(tr.second<ret.second)
						ret.second=tr.second;
				}			
			}
		}

		return ret;
	}

	bool NavBintreeNode::RayCollideBoxOnce( const Ray& ray,NavBintreeCollider* pCollider )
	{
		//--先与自已的大盒子碰
		TResult tr=ray.Intersect(m_box);
		if(!tr.first)
			return false;

		//--与自已的盒子列表碰
		for(UINT i=0;i<m_content.size();i++)
		{	
			const AABBox& box=pCollider->GetCollideBox(m_pContents[i]);
			tr=ray.Intersect(box);
			if(tr.first)
				return true;
		}

		//--递归子结点
		for(INT i=0;i<EChildNum;i++)
		{
			if(m_pChildren[i]!=NULL
				&&m_pChildren[i]->RayCollideBoxOnce(ray,pCollider))
				return true;
		}

		return false;
	}
}//namespace WorldBase