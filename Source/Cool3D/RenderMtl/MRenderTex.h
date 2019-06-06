#pragma once
#include "irendermtl.h"

namespace Cool3D
{
	class IRenderTexture;
	class Cool3D_API MRenderTex :	public IRenderMtl
	{
	public:
		MRenderTex(void);
		virtual ~MRenderTex(void);

	public:
		IRenderTexture	*m_pRTex;
		ETextrueFilter	m_magFilter;
		ETextrueFilter	m_minFilter;
		ETextrueFilter	m_mipFilter;
		ETextureAddress	m_addressU,
						m_addressV,
						m_addressW;

		DECL_RTTI_DYNC(MRenderTex);
	};
}//namespace Cool3D