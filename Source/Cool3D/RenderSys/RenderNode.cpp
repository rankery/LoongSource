#include "StdAfx.h"
#include ".\rendernode.h"
#include "..\Device\IDevice.h"
#include "..\Kernel.h"
#include "..\RenderMtl\IMaterial.h"
#include "RenderSys.h"
#include "MtlRenderCache.h"
#include "..\SceneGraph\SceneNode.h"

namespace Cool3D
{
	IMP_RTTI(RenderNode,RTTIObj);

	IMaterial		*RenderNode::m_pSpecialMtl=NULL;
	IVertShader		*RenderNode::m_pSpecialVS=NULL;
	IPixelShader	*RenderNode::m_pSpecialPS=NULL;

	RenderNode::RenderNode(void)
	{
		m_pCurMtl=NULL;
		m_renderPri=EPri_Normal;
	}

	RenderNode::~RenderNode(void)
	{
		m_pCurMtl=NULL;
	}

	void RenderNode::Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod,RenderItem* pItem,SceneGraph* pSG)	//Ö§³ÖLodµÄ
	{}
	bool RenderNode::SupportLod()
	{
		return false;
	}

	int RenderNode::BeginMtl(RenderSys *pSys,IMaterial *pMtl)
	{
		if(pMtl==NULL
			&& m_pSpecialMtl==NULL)
			return 0;
		if(m_pSpecialMtl!=NULL)
			m_pCurMtl=m_pSpecialMtl;
		else
			m_pCurMtl=pMtl;
		int numPass=pSys->PrepMtl(m_pCurMtl);
		MtlRenderCache *pCache=m_pCurMtl->GetRenderCache();
		if(pCache)
			pCache->Begin(m_pSpecialVS,m_pSpecialPS);
		return numPass;
	}

	void RenderNode::BeginMtlPass(int passIndex)
	{
		ASSERT(m_pCurMtl!=NULL);
		m_pCurMtl->GetRenderCache()->BeginPass(passIndex);
	}

	void RenderNode::EndMtlPass(bool clearTex)
	{
		ASSERT(m_pCurMtl!=NULL);
		m_pCurMtl->GetRenderCache()->EndPass(clearTex);
	}

	void RenderNode::EndMtl()
	{
		if(m_pCurMtl!=NULL)
		{
			MtlRenderCache *pCache=m_pCurMtl->GetRenderCache();
			if(pCache)
				pCache->End();
			m_pCurMtl=NULL;
		}
	}

	EMtlTrait RenderNode::GetMtlTrait(int mtlID,SceneNode* pSGNode) const
	{
		if(pSGNode==NULL)
			return EMT_Unknow;
		IMaterial *pMtl=pSGNode->GetMtl(mtlID);
		if(pMtl==NULL)
			return EMT_Unknow;
		Kernel::Inst()->GetRenderSys()->PrepMtl(pMtl);
		MtlRenderCache *pCache=pMtl->GetRenderCache();
		if(pCache==NULL)
			return EMT_Unknow;
		return pCache->GetMtlTrait();
	}

	DWORD RenderNode::GetMtlTrait1(int mtlID,SceneNode *pSGNode) const
	{
		if(pSGNode==NULL)
			return 0xFFFFFFFF;
		IMaterial *pMtl=pSGNode->GetMtl(mtlID);
		if(pMtl==NULL)
			return 0xFFFFFFFF;
		return pMtl->GetRenderTrait1();
	}

	DWORD RenderNode::GetMtlTrait2(int mtlID,SceneNode *pSGNode) const
	{
		if(pSGNode==NULL)
			return 0xFFFFFFFF;
		IMaterial *pMtl=pSGNode->GetMtl(mtlID);
		if(pMtl==NULL)
			return 0xFFFFFFFF;
		return pMtl->GetRenderTrait2();
	}

}//namespace Cool3D