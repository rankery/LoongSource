#include "StdAfx.h"
#include "SGAttachableEffectNode.h"
#include "..\SceneGraph\EntityNode.h"
#include "..\Math\Transform.h"
#include "..\Animation\AnimationCtrl.h"
#include "..\SceneGraph\AvatarNode.h"
#include "..\SceneGraph\SGAvatarAttachment.h"
#include "..\ResSys\ResSkeleton.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(SGAttachableEffectNode,SGEffectNode);

	SGAttachableEffectNode::SGAttachableEffectNode(void)
		: m_tagIndex(-1)
	{
	}

	SGAttachableEffectNode::~SGAttachableEffectNode(void)
	{
	}

	void SGAttachableEffectNode::BindToTagNode(const TCHAR* szTagName)
	{
		ASSERT(szTagName!=NULL);
		m_szTagName=szTagName;
		m_tagIndex=-1;
	}

	bool SGAttachableEffectNode::UnbindToTagNode(void)
	{
		m_szTagName.clear();
		if(m_szTagName.empty())
		{
			return true;
		}
		else
		{
			return false;
		}
		return false;
	}

	void SGAttachableEffectNode::Update(float deltaTime,SceneGraph *pSG)
	{
		UpdateWorldMatrix();
		SGEffectNode::Update(deltaTime,pSG);
	}

	void SGAttachableEffectNode::UpdateWorldMatrix()
	{
		if(m_szTagName.empty())
			return;

		const SceneNode *pFather=SGEffectNode::GetParent();
		if(pFather==NULL)
			return;

		m_finalToWorld = pFather->GetWorldMat();

		if( pFather->IS_KIND_OF(AvatarNode) )//特效绑在骨骼上，需要m_tagIndex
		{
			AvatarNode *pParent=(AvatarNode*)pFather;

			if( m_tagIndex==-1 )
			{
				ResSkeleton *pSkel=const_cast<ResSkeleton *>(pParent->GetResource());
				if(pSkel!=NULL
					&& pSkel->IsCreated())
				{
					m_tagIndex=pSkel->GetBoneID(m_szTagName.c_str());
					ASSERT(m_tagIndex!=-1 && "bone not find");
				}
			}

			if( m_tagIndex!=-1 )
			{
				m_finalToWorld=(*(pParent->GetSkeletonAniCtrl()->GetBoneSpace(m_tagIndex)));

				m_finalToWorld*=pParent->GetWorldMat();//变换到父节点的空间
			}
		}
		else if( pFather->IS_KIND_OF(SGAvatarAttachment) )//特效绑在装备上，只需要m_szTagName
		{
			SGAvatarAttachment *pParent=(SGAvatarAttachment*)pFather;
			if( pParent->GetModelSpace(m_szTagName.c_str(), m_finalToWorld) )
			{
				m_finalToWorld=m_finalToWorld*pParent->GetWorldMat();
			}
		}
		else if( pFather->IS_KIND_OF(EntityNode) )//特效绑在骨骼上，需要m_tagIndex
		{
			EntityNode *pParent=(EntityNode*)pFather;
			ResRenderable* pRes = pParent->GetResObj();
			if( NULL != pRes && pRes->IS_STATIC_CLASS(ResSkeleton) && pRes->IsCreated() )
			{
				if( m_tagIndex==-1 )
				{
					ResSkeleton *pSkel=(ResSkeleton*)pRes;
					m_tagIndex=pSkel->GetBoneID(m_szTagName.c_str());
					ASSERT(m_tagIndex!=-1 && "bone not find");
				}
				if( m_tagIndex!=-1 )
				{
					AnimationCtrl* pAni = pParent->GetAniCtrl();
					if( NULL != pAni && pAni->IS_STATIC_CLASS(NodeAnimationCtrl) )
					{
						NodeAnimationCtrl* pNodeAni = (NodeAnimationCtrl*)pAni;
						m_finalToWorld=(*(pNodeAni->GetBoneSpace(m_tagIndex)));

						m_finalToWorld*=pParent->GetWorldMat();//变换到父节点的空间
					}
				}
			}
		}


		SGEffectNode::SetWorldMat(&m_finalToWorld);
	}
}//end of namespace Cool3D
