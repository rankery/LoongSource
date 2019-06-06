#include "StdAfx.h"
#include ".\sceneprojector.h"
#include "..\RenderMtl\IMaterial.h"
#include "..\RenderMtl\MMFrameBlend.h"
#include "..\RenderMtl\MMDepthFrame.h"

namespace Cool3D
{
	IMP_RTTI(SceneProjector,SceneNode);

	SceneProjector::SceneProjector(void)
	{
		m_bTerrain=true;
		m_bStaticMesh=false;
	}

	SceneProjector::~SceneProjector(void)
	{
	}

	void SceneProjector::AddDefaultMtlMod(IMaterial *pMtl)
	{
		//--mul color buffer
		MMFrameBlend *pMM=new MMFrameBlend;
		pMM->m_blendEnable=true;
		pMM->m_src=EBlend_Zero;
		pMM->m_dst=EBlend_SrcAlpha;
		
		//--²»Ð´zbuffer
		MMDepthFrame *pDepth=new MMDepthFrame;
		pDepth->m_depthWriteEnable=false;
		pDepth->m_depthTestEnable=true;
		pDepth->m_depthFunc=ECMP_LessEqual;

		//--
		pMtl->AttachModifier(pMM);
		pMtl->AttachModifier(pDepth);
	}
}//namespace Cool3D