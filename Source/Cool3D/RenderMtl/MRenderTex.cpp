#include "StdAfx.h"
#include ".\mrendertex.h"
#include "..\Device\IRenderTexture.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(MRenderTex,IRenderMtl);

	MRenderTex::MRenderTex(void)
	{
		m_magFilter=ETFilter_LINEAR;
		m_minFilter=ETFilter_LINEAR;
		m_mipFilter=ETFilter_NONE;
		m_addressU=m_addressV=m_addressW=ETAddress_Clamp;

		m_pRTex=NULL;
	}

	MRenderTex::~MRenderTex(void)
	{
		SAFE_DELETE(m_pRTex);
	}
}//namespace Cool3D