#include "StdAfx.h"
#include ".\sgattachableeffectnode.h"

namespace F3D
{
	SGAttachableEffectNode::SGAttachableEffectNode(void)
	{
		D3DXMatrixIdentity(&m_finalToWorld);
		m_tagIndex = -1;
	}

	SGAttachableEffectNode::~SGAttachableEffectNode(void)
	{
	}

	void SGAttachableEffectNode::BindToTagNode(const char* szTagName)
	{
		ASSERT(szTagName!=NULL);
		m_szTagName=szTagName;
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
		if(m_szTagName.empty())
			return;

		const SceneNode *pFather = SGEffectNode::GetParent();
		if(pFather==NULL)
			return;

		EntityNode * pEntity = (EntityNode *)pFather;
		AniCtrl *pAni = NULL;
		pAni=pEntity->GetAniCtrl();

		const vector<tagTagNodeSpace>& tagNodes=pAni->GetTagNodes();
		if(tagNodes.size()==0)
		{
			return;
		}

		//DWORD tagCode=_MakeFourCC(m_p->m_szTagName);
		if(m_tagIndex==-1)
		{
			for(int i=0;i<tagNodes.size();i++)
			{
				const tagTagNodeSpace& tn=tagNodes[i];
				//if(tn.name==tagCode)
				if(_FourCC2Str(tn.name)==m_szTagName)
				{
					m_tagIndex=i;
					break;
				}
			}
		}
		
		int  tagNodesCounter = tagNodes.size();

		if(m_tagIndex>=0 && m_tagIndex<tagNodesCounter)
		{
			const tagTagNodeSpace& tn=tagNodes[m_tagIndex];

			m_finalToWorld=*(tn.trans.GetMatrix());
			m_finalToWorld=m_finalToWorld*pEntity->GetWorldMat();//变换到父节点的空间
			SGEffectNode::SetWorldMat(&m_finalToWorld);//update world matrix and aabbox
		}

		 SGEffectNode::Update(deltaTime,pSG);
	}//end  of  Update
}//end of namespace F3D
