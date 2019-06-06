#include "StdAfx.h"
#include ".\avatarnode.h"
#include "..\ResSys\ResMgr.h"
#include "..\ResSys\ResRenderable.h"
#include "..\ResSys\ResSkeleton.h"
#include "..\ResSys\ResSkin.h"
#include "..\Animation\AnimationCtrl.h"
#include "SGSkinNode.h"
#include "..\Math\Frustum.h"
#include "SGAvatarAttachment.h"
#include "..\Util\SpeedAnalyser.h"
#include "..\Kernel.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(AvatarNode,SceneNode);

	AvatarNode::AvatarNode(void)
		: m_pRes(0)
		, m_pAniCtrl(0)
		, m_bHide(false)
	{
		D3DXMatrixIdentity(&m_toWorldMat);
		m_box=MathConst::SmallBox;
	}

	AvatarNode::~AvatarNode(void)
	{
		RES_MGR_RELEASE_RENDERABLE(m_pRes);
		SAFE_DELETE(m_pAniCtrl);
	}

	void AvatarNode::Serialize(FArchive& ar)
	{
		ar<<m_toWorldMat;
		ar<<m_pRes->GetName();
		SceneNode::Serialize(ar);
	}

	void AvatarNode::Deserialize(FArchive& ar)
	{
		ar>>m_toWorldMat;
		tstring szResName;
		ar>>szResName;
		LoadRes(szResName.c_str());
		SceneNode::Deserialize(ar);
	}

	void AvatarNode::GetBox(AABBox& out) const
	{
		out=m_box;
	}

	bool AvatarNode::IsInFrustum(const Frustum *pFrumstum) const
	{
		const_cast<AvatarNode*>(this)->UpdateBoundingBox();
		if(m_bHide)
			return false;
		return pFrumstum->BoxInFrustum(m_box);
	}

	void AvatarNode::LoadRes(const TCHAR* szName)
	{
		//--
		if(m_pRes!=NULL)
		{
			ResMgr::Inst()->ReleaseRenderable(m_pRes);
			m_pRes=NULL;
		}
		SAFE_DELETE(m_pAniCtrl);

		//--
		ResRenderable *pRes=ResMgr::Inst()->NewRenderable(szName);
		ASSERT(pRes!=NULL && pRes->IS_STATIC_CLASS(ResSkeleton));
		m_pRes=(ResSkeleton*)pRes;
		m_pAniCtrl=new NodeAnimationCtrl;
		m_pAniCtrl->BindAnimationRes(m_pRes);//此处只需要bind即可,无需处理res是否创建完成
	}

	UINT AvatarNode::GetNumRenderNode() const
	{
		return 0;//自己没有渲染节点, 渲染节点都在children中
	}

	RenderNode *AvatarNode::GetRenderNode(UINT i) const
	{
		return NULL;
	}

	const Matrix4& AvatarNode::GetWorldMat() const
	{
		return m_toWorldMat;
	}

	void AvatarNode::SetWorldMat(const Matrix4 *pMat)
	{
		m_toWorldMat=*pMat;
	}

	void AvatarNode::Update(float deltaTime,SceneGraph *pSG)
	{
		if(m_pRes==NULL || m_pAniCtrl==NULL)
			return;

		//--update skeleton
		m_pAniCtrl->Update();

		//--update children
		SceneNode::Update(deltaTime,pSG);
	}

	bool AvatarNode::IsCreated()
	{
		for(int i=0;i<GetNumChildren();i++)
		{
			SceneNode* pChild=(SceneNode*)GetChild(i);
			if(pChild->IS_STATIC_CLASS(SGSkinNode))
			{
				SGSkinNode* pSkinNode=(SGSkinNode*)pChild;
				if(!pSkinNode->IsCreated())
					return false;
			}
		}
		return true;
	}

	void AvatarNode::ForceUpdate(float deltaTime,SceneGraph *pSG)
	{
		m_pAniCtrl->ApplyTime(deltaTime);
		//--update children
		SceneNode::ForceUpdate(deltaTime,pSG);
	}

	void AvatarNode::UpdateBoundingBox()
	{
		m_box.max=Vector3(-FLOAT_MAX,-FLOAT_MAX,-FLOAT_MAX);
		m_box.min=Vector3(FLOAT_MAX,FLOAT_MAX,FLOAT_MAX);

		int num=GetNumChildren();
		//--合并所有Skin的object空间box,然后转换到world空间
		int i;
		for(i=0;i<num;i++)
		{
			const SceneNode *pChild=GetChild(i);
			if(pChild->IS_STATIC_CLASS(SGSkinNode))
			{
				const ResSkin *pRes=((const SGSkinNode*)pChild)->GetResSkin();
				m_box.Merge(*(pRes->GetAABBox()));
			}
		}
		m_box.Transform(m_toWorldMat);

		//--合并上所有附件的world空间的box
		//for(i=0;i<num;i++)
		//{
		//	const SceneNode *pChild=GetChild(i);
		//	if(pChild->IS_KIND_OF(EntityNode))
		//	{
		//		EntityNode* pAtt=(EntityNode*)pChild;
		//		m_box.Merge(pAtt->GetAABBox());
		//	}
		//}
	}

	IMaterial*	AvatarNode::GetMtl(int mtlID)
	{
		return NULL;//have no mtl
	}

	TResult AvatarNode::RayCollide(const Ray& ray)
	{
		TResult tr(false,0);

		for(int i=0;i<GetNumChildren();i++)
		{
			const SceneNode *pChild=GetChild(i);
			if(pChild->IS_STATIC_CLASS(SGSkinNode))
			{
				SGSkinNode* pSkinNode = (SGSkinNode*)pChild;
				tr = pSkinNode->RayCollide(ray);
				if( tr.first )
					return tr;
			}
		}

		return tr;
	}

	bool AvatarNode::ReloadMsgTable()
	{
		if( NULL == m_pRes || !m_pRes->IsCreated() )
			return false;

		m_pRes->ReloadMsgTable();
		return true;
	}

	void AvatarNode::SetTransparent( float alpha )
	{
		for(int i=0;i<GetNumChildren();i++)
		{
			const SceneNode *pChild=GetChild(i);
			if(pChild->IS_STATIC_CLASS(SGSkinNode))
				((SGSkinNode*)pChild)->SetTransparent(alpha);
			else if(pChild->IS_KIND_OF(EntityNode))
				((EntityNode*)pChild)->SetTransparent(alpha);
		}
	}

	void AvatarNode::SetMulColor(const float r, const float g, const float b)
	{
		for(int i=0;i<GetNumChildren();i++)
		{
			const SceneNode *pChild=GetChild(i);
			if(pChild->IS_STATIC_CLASS(SGSkinNode))
				((SGSkinNode*)pChild)->SetMulColor(r,g,b);
			else if(pChild->IS_KIND_OF(EntityNode))
				((EntityNode*)pChild)->SetMulColor(r,g,b);
		}
	}

}//namespace Cool3D