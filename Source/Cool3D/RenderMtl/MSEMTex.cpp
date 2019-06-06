#include "StdAfx.h"
#include ".\MSEMTex.h"
#include ".\MBitmap.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(MSEMTex,IMaterial);

	MSEMTex::MSEMTex(void)
	{
		m_pEnvMap=NULL;
		m_pBaseMap=NULL;
	}

	MSEMTex::~MSEMTex(void)
	{
		SAFE_DELETE(m_pEnvMap);
		SAFE_DELETE(m_pBaseMap);
	}

	bool MSEMTex::IsResourceReady() const
	{
		if(m_pEnvMap==NULL || !m_pEnvMap->IsResourceReady())
			return false;
		if(m_pBaseMap==NULL || !m_pBaseMap->IsResourceReady())
			return false;

		return true;
	}

	DWORD MSEMTex::GetRenderTrait1() const
	{
		if(m_pBaseMap)
			return m_pBaseMap->GetRenderTrait1();
		else
			return IMaterial::GetRenderTrait1();
	}

	DWORD MSEMTex::GetRenderTrait2() const
	{
		if(m_pBaseMap)
			return m_pBaseMap->GetRenderTrait2();
		else
			return IMaterial::GetRenderTrait2();
	}

	void MSEMTex::SetEnvMap(const TCHAR* szPath)
	{
		SAFE_DELETE(m_pEnvMap);

		m_pEnvMap=new MBitmap;
		m_pEnvMap->SetTexture(0,szPath);
		m_pEnvMap->m_magFilter=ETFilter_LINEAR;
		m_pEnvMap->m_minFilter=ETFilter_LINEAR;
	}

}//namespace Cool3D