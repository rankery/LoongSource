#include "StdAfx.h"
#include ".\MWater.h"
#include ".\MBitmap.h"
#include ".\MBitmapFrames.h"
#include ".\MMFrameBlend.h"
#include "..\ResSys\ResMgr.h"
#include "..\ResSys\ResPixelShader.h"
#include "..\Cool3DOption.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(MWater,IMaterial);

	MWater::MWater(void)
	{
		m_pFrames=NULL;
		m_pAlphaMap=NULL;
		m_color.m_color.specular=Color4f(1.0f,1.0f,1.0f,1.0f);
		m_color.m_color.power=32.0f;
	}

	MWater::~MWater(void)
	{
		SAFE_DELETE(m_pAlphaMap);
		SAFE_DELETE(m_pFrames);
	}

	bool MWater::IsResourceReady() const
	{
		if(m_pFrames==NULL
			||!m_pFrames->IsResourceReady())
			return false;

		if(m_pAlphaMap==NULL
			||!m_pAlphaMap->IsResourceReady())
			return false;

		return true;
	}

	void MWater::UpdateModifiers(float deltaTime)
	{
		ResTexture* pBitmap=m_pFrames->m_pBitmap;
		m_pFrames->UpdateModifiers(deltaTime);
		if(pBitmap!=m_pFrames->m_pBitmap)
			InvalidateRenderCache();
	}

	void MWater::SetFrames(const TCHAR* szBaseName,int waterFrm,float waterFps)
	{
		SAFE_DELETE(m_pFrames);

		MBitmapFrames* pFrames=new MBitmapFrames;		//多个贴图文件组成的贴图动画材质
		pFrames->LoadFrames(szBaseName,true,waterFrm,0);
		pFrames->SetParam(waterFps);	//循环播放
		pFrames->SetMipFilter(ETFilter_LINEAR);
		pFrames->m_addressU=pFrames->m_addressV=pFrames->m_addressW=ETAddress_Wrap;

		pFrames->m_bTwoSide=true;

		m_pFrames=pFrames;
	}

	void MWater::SetAlphaMap( const TCHAR* szName )
	{
		SAFE_DELETE(m_pAlphaMap);

		m_pAlphaMap=new MBitmap;
		m_pAlphaMap->SetTexture(0,szName,0);
		m_pAlphaMap->SetMipFilter(ETFilter_POINT);
		m_pAlphaMap->m_addressU=m_pAlphaMap->m_addressV=m_pAlphaMap->m_addressW=ETAddress_Clamp;
	}
}//namespace Cool3D