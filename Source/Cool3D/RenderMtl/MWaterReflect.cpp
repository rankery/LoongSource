#include "StdAfx.h"
#include ".\MWaterReflect.h"
#include "..\ResSys\ResMgr.h"
#include "..\ResSys\ResVertShader.h"
#include "..\ResSys\ResPixelShader.h"
#include "..\Cool3DOption.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(MWaterReflect,IMaterial);

	MWaterReflect::MWaterReflect(void)
	{
		m_pReflectMap=new MRenderTex;
		m_pReflectMap->m_addressU=m_pReflectMap->m_addressV=m_pReflectMap->m_addressW=ETAddress_Clamp;

		m_pBumpMap=NULL;
		m_pAlphaMap=NULL;
		m_blendColor=Color4f(0.5f,0,0.4f,0.4f);
		m_color.m_color.specular=Color4f(1.0f,1.0f,1.0f,1.0f);
		m_color.m_color.power=32.0f;

		m_bumpScale=0.025f;
		m_bumpUVScroll=Vector2( 1.0f, 1.0f );
		m_specularScale=0.2f;
	}

	MWaterReflect::~MWaterReflect(void)
	{
		SAFE_DELETE(m_pAlphaMap);
		SAFE_DELETE(m_pBumpMap);
		m_pReflectMap->m_pRTex=NULL;
		SAFE_DELETE(m_pReflectMap);
	}

	bool MWaterReflect::IsResourceReady() const
	{
		if(m_pAlphaMap==NULL
			|| !m_pAlphaMap->IsResourceReady())
			return false;

		if(m_pBumpMap!=NULL
			&& !m_pBumpMap->IsResourceReady())
			return false;


		return m_pReflectMap->m_pRTex!=NULL;
	}

	void MWaterReflect::SetAlphaMap( const TCHAR *szName )
	{
		SAFE_DELETE(m_pAlphaMap);

		m_pAlphaMap=new MBitmap;
		m_pAlphaMap->SetTexture(0,szName,0);
		m_pAlphaMap->SetMipFilter(ETFilter_POINT);
		m_pAlphaMap->m_addressU=m_pAlphaMap->m_addressV=m_pAlphaMap->m_addressW=ETAddress_Clamp;
	}
}//namespace Cool3D