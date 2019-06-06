#include "StdAfx.h"
#include ".\staticnode.h"
#include "..\Util\Exception.h"
#include "..\Kernel.h"
#include "..\Device\IDevice.h"
#include "..\Device\IIndexBuffer.h"
#include "..\Device\IStreamDefine.h"
#include "RenderSys.h"
#include "..\SceneGraph\SceneNode.h"
#include "..\RenderMtl\IMaterial.h"

#define VFS_READ
#include "\work\public\SubMesh.h"

namespace F3D
{
	IMP_RTTI_DYNC(StaticNode,RenderNode);

	StaticNode::StaticNode(void)
	{
	}

	StaticNode::~StaticNode(void)
	{
	}

	void StaticNode::Draw(RenderSys *pSys,SceneNode *pSGNode,int nLod)
	{
		IDevice *pDev=Device();
		pDev->SetVB(0,m_pVB.GetMem());

		IMaterial *pMtl=pSGNode->GetMtl(m_subMesh.GetMtlID());
		m_subMesh.Draw(nLod,pSys,this,pMtl);
	}

	void StaticNode::Create(SharedVB pVB,EVertType vt,const LodSubMesh* pSubMesh)
	{
		ASSERT(pVB.GetMem()!=NULL);
		ASSERT(pSubMesh!=NULL);

		m_pVB=pVB;
		m_vertType=vt;

		m_subMesh.CreateFromLodSubMesh(pSubMesh);
	}

	DWORD StaticNode::GetMtlTrait(SceneNode *pSGNode) const
	{
		return DWORD(pSGNode->GetMtl(m_subMesh.GetMtlID()));
	}
	ETransparentType StaticNode::GetTransparentType(const SceneNode *pSGNode) const
	{
		const IMaterial *pMtl=(const_cast<SceneNode*>(pSGNode))->GetMtl(m_subMesh.GetMtlID());
		if(pMtl)
		{
			if(pMtl->IsTransparent())
				return ETrans_Blend;
		}
		return ETrans_None;
	}
	const AABBox& StaticNode::GetAABBox() const
	{
		return m_subMesh.GetAABBox();
	}
}//namespace F3D