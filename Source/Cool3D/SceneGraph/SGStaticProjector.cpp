#include "StdAfx.h"
#include ".\sgstaticprojector.h"
#include "..\Math\Frustum.h"
#include "..\RenderMtl\MBitmap.h"
#include "..\RenderSys\RenderSys.h"
#include "..\RenderSys\RenderNode.h"
#include "..\RenderSys\RStaticMesh.h"
#include "..\Device\IDevice.h"
#include "..\Device\IVertBuffer.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(SGStaticProjector,SceneProjector);

	class SGStaticProjector::Member
	{
	public:
		MBitmap		m_mtl;
		AABBox		m_box;
		RStaticMesh	m_renderNode;
		VertStream	*m_pVertStream;//除了存盘/读盘没其他价值
		
	public:
		Member()	
		{
			m_pVertStream=NULL;
		}
		~Member()	
		{
			SAFE_DELETE(m_pVertStream);
		}

		void CreateRenderNode()
		{
			m_renderNode.Create(0,EDraw_TriangleList,m_pVertStream);
			m_renderNode.SetPriority(RenderNode::EPri_Lowest);
		}

	};

	SGStaticProjector::SGStaticProjector(void)
	{
		m_p=new Member;
	}

	SGStaticProjector::~SGStaticProjector(void)
	{
		delete m_p;
	}

	//--序列化接口
	void SGStaticProjector::Serialize(FArchive& ar)
	{
		ar<<m_p->m_box.max;
		ar<<m_p->m_box.min;

		ar<<m_p->m_pVertStream->GetNumVert();
		ar<<m_p->m_pVertStream->GetType();
		ar.BufferWrite(m_p->m_pVertStream->GetRawBuffer(),m_p->m_pVertStream->GetBufferSize());

		m_p->m_mtl.Serialize(ar);

		SceneProjector::Serialize(ar);
	}

	void SGStaticProjector::Deserialize(FArchive& ar)
	{
		ar>>m_p->m_box.max;
		ar>>m_p->m_box.min;

		UINT numVert;
		int vertType;
		ar>>numVert;
		ar>>vertType;

		m_p->m_pVertStream=new VertStream;
		m_p->m_pVertStream->Create((EVertType)vertType,numVert);
		ar.BufferRead(m_p->m_pVertStream->GetRawBuffer(),m_p->m_pVertStream->GetBufferSize());

		m_p->CreateRenderNode();

		m_p->m_mtl.Deserialize(ar);

		SceneProjector::Deserialize(ar);
	}

	bool SGStaticProjector::IsInFrustum(const Frustum *pFrumstum) const
	{
		return true;
		//return pFrumstum->BoxInFrustum(m_p->m_box);
	}

	void SGStaticProjector::LoadRes(const TCHAR* szName)
	{
		ASSERT(0 && _T("projector have not resource"));
	}

	UINT SGStaticProjector::GetNumRenderNode() const
	{
		return 1;
	}

	RenderNode *SGStaticProjector::GetRenderNode(UINT i) const
	{
		return &m_p->m_renderNode;
	}

	const Matrix4& SGStaticProjector::GetWorldMat() const
	{
		return MathConst::IdentityMat;
	}

	void SGStaticProjector::SetWorldMat(const Matrix4 *pMat)
	{
		ASSERT(0 && _T("projector doesn't support world matrix"));
	}

	IMaterial*	SGStaticProjector::GetMtl(int mtlID)
	{
		return &m_p->m_mtl;
	}

	void SGStaticProjector::Create(VertStream *pVertStream,const TCHAR *szTexPath)
	{
		ASSERT(m_p->m_pVertStream==NULL);
		m_p->m_pVertStream=pVertStream->Clone(true);
		m_p->m_pVertStream->BuildAABBox(m_p->m_box);

		m_p->m_mtl.SetTexture(0,szTexPath);
		m_p->m_mtl.m_addressU=m_p->m_mtl.m_addressV=m_p->m_mtl.m_addressW=ETAddress_Clamp;
		AddDefaultMtlMod(&m_p->m_mtl);

		m_p->CreateRenderNode();
	}

	void SGStaticProjector::GetBox(AABBox& out) const
	{
		out = m_p->m_box;
	}

	void SGStaticProjector::ForceUpdate(float deltaTime,SceneGraph *pSG)
	{}
}//namespace Cool3D