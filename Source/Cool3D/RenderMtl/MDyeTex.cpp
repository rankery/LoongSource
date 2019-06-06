#include "StdAfx.h"
#include ".\MDyeTex.h"
#include ".\MConstColor.h"
#include ".\MBitmap.h"
#include ".\MMAlphaMask.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(MDyeTex,IMaterial);

	MDyeTex::MDyeTex(void)
	{
		m_pAlphaMap=NULL;
		m_pDiffuseMap=NULL;
		m_dyeColor=Color4f(1,1,1,1);
		m_pColor=NULL;
	}

	MDyeTex::~MDyeTex(void)
	{
		SAFE_DELETE(m_pAlphaMap);
		SAFE_DELETE(m_pDiffuseMap);
		SAFE_DELETE(m_pColor);
	}

	bool MDyeTex::IsResourceReady() const
	{
		if(m_pAlphaMap==NULL 
			||!m_pAlphaMap->IsResourceReady())
			return false;
		if(m_pDiffuseMap==NULL
			||!m_pDiffuseMap->IsResourceReady())
			return false;
		return true;
	}

	DWORD MDyeTex::GetRenderTrait1() const
	{
		return IMaterial::GetRenderTrait1();
	}

	DWORD MDyeTex::GetRenderTrait2() const
	{
		if(m_pDiffuseMap)
			return m_pDiffuseMap->GetRenderTrait2();
		else
			return IMaterial::GetRenderTrait2();
	}

	void MDyeTex::SetAlphaMap(const TCHAR* szPath)
	{
		SAFE_DELETE(m_pAlphaMap);

		m_pAlphaMap=new MBitmap;
		m_pAlphaMap->SetTexture(0,szPath);
		m_pAlphaMap->SetMipFilter(ETFilter_LINEAR);
	}


	MMAlphaMask* MDyeTex::GetAlphaMask()
	{
		MMAlphaMask* pMM=NULL;

		if(m_pDiffuseMap)
		{
			pMM=(MMAlphaMask*)m_pDiffuseMap->GetModifier(_T("MMAlphaMask"));
		}

		return pMM;
	}

}//namespace Cool3D