#include "StdAfx.h"
#include ".\mgeneral.h"
#include "..\Util\Exception.h"
#include "MConstColor.h"
#include "MBitmap.h"
#include "MMUVTrans.h"
#include "IMtlModifier.h"
#include "MMAlphaMask.h"
#include "MMFrameBlend.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(MGeneral,IMaterial);

	MGeneral::MGeneral(void) : m_color( 1.0f, 1.0f, 1.0f, 1.0f )
	{
		m_pColor=NULL;
		m_pDiffuse=NULL;
		m_pLightMap=NULL;
		m_pBump=NULL;
		m_bEnableSpecular=false;
		m_bEnableReflection=false;
		m_bTransparent=false;

		m_uvIndexDiffuse=0;
		m_uvIndexLightMap=1;
	}

	MGeneral::~MGeneral(void)
	{
		Destroy();
	}

	void MGeneral::Destroy()
	{
		SAFE_DELETE(m_pColor);
		SAFE_DELETE(m_pDiffuse);
		SAFE_DELETE(m_pLightMap);
		SAFE_DELETE(m_pBump);
	}

	bool MGeneral::IsResourceReady() const
	{
		if(m_pDiffuse!=NULL
			&& !m_pDiffuse->IsResourceReady())
			return false;
		if(m_pLightMap!=NULL
			&& !m_pLightMap->IsResourceReady())
			return false;
		if(m_pBump!=NULL
			&& !m_pBump->IsResourceReady())
			return false;
		return true;
	}

	void MGeneral::CreateFromMaxExport(const tagMtlDesc& desc,UINT mipLevels,const TCHAR* szLightMap,float mtlDiffuseFactor)
	{
		Destroy();
		IMaterial::InvalidateRenderCache();

		//--
		m_pColor=new MConstColor;
		m_pColor->m_color.ambient=Color4f(desc.m_ambient);
		m_pColor->m_color.diffuse.R=desc.m_diffuse.x*mtlDiffuseFactor;
		m_pColor->m_color.diffuse.G=desc.m_diffuse.y*mtlDiffuseFactor;
		m_pColor->m_color.diffuse.B=desc.m_diffuse.z*mtlDiffuseFactor;
		m_pColor->m_color.specular=Color4f(desc.m_specular);
		m_pColor->m_color.emissive=Color4f(desc.m_emissive);
		m_pColor->m_color.power=desc.m_power;
		
		if(desc.m_power > 1)
			m_bEnableSpecular=true;

		if (_tcslen(desc.m_map[EMap_Sphere]) > 0)
			m_bEnableReflection=true;

		if(_tcslen(desc.m_map[EMap_Diffuse]) == 0)
			return;//diffuse 贴图没有就不处理任何贴图
		m_pDiffuse=new MBitmap;
		m_pDiffuse->SetTexture(0,desc.m_map[EMap_Diffuse]);
		m_pDiffuse->m_bTwoSide=desc.m_bTwoSide;
		if(1!=mipLevels)
			m_pDiffuse->SetMipFilter(ETFilter_LINEAR);//max输出的物体都使用mip map??人物??

		if(szLightMap!=NULL && _tcslen(szLightMap)>0)
		{
			m_pLightMap=new MBitmap;
			m_pLightMap->SetTexture(0,szLightMap);
		}

		if(_tcslen(desc.m_map[EMap_Bump]) > 0)
		{
			m_pBump=new MBitmap;
			m_pBump->SetTexture(0,desc.m_map[EMap_Bump]);
		}
		
		if(desc.m_bOpcityMap)
		{
			if(desc.m_opcityVal<255)//使用alpha mask代替透明处理
			{
				MMAlphaMask *pAlphaMask=new MMAlphaMask;
				pAlphaMask->m_alphaRef=desc.m_opcityVal;
				pAlphaMask->m_alphaTestFunc=ECMP_Greater;
				m_pDiffuse->AttachModifier(pAlphaMask);
			}
			else
			{
				MMFrameBlend *pAlphaBlend=new MMFrameBlend;
				pAlphaBlend->m_blendEnable=true;
				pAlphaBlend->m_bMultiPass=desc.m_bMultiPass;
				pAlphaBlend->m_src=EBlend_SrcAlpha;
				pAlphaBlend->m_dst=EBlend_InvSrcAlpha;
				m_pDiffuse->AttachModifier(pAlphaBlend);
			}
		}

		//保存漫反射颜色，运行时需要修改和恢复
		m_color=m_pColor->m_color.diffuse;
	}

	void MGeneral::Serialize(FArchive& ar)
	{
		bool bHave=m_pColor!=NULL;
		ar<<bHave;
		if(bHave)
			m_pColor->Serialize(ar);
		bHave=m_pDiffuse!=NULL;
		ar<<bHave;
		if(bHave)
			m_pDiffuse->Serialize(ar);
		bHave=m_pLightMap!=NULL;
		ar<<bHave;
		if(bHave)
			m_pLightMap->Serialize(ar);
		bHave=m_pBump!=NULL;
		ar<<bHave;
		if(bHave)
			m_pBump->Serialize(ar);
		ar<<m_bEnableSpecular;

		IMaterial::Serialize(ar);
	}

	void MGeneral::Deserialize(FArchive& ar)
	{
		//--必须是没有创建的
		ASSERT(m_pColor==NULL);
		ASSERT(m_pDiffuse==NULL);
		ASSERT(m_pLightMap==NULL);
		ASSERT(m_pBump==NULL);

		//--
		bool bHave=false;
		ar>>bHave;
		if(bHave)
		{
			m_pColor=new MConstColor;
			m_pColor->Deserialize(ar);
		}
		ar>>bHave;
		if(bHave)
		{
			m_pDiffuse=new MBitmap;
			m_pDiffuse->Deserialize(ar);
		}
		ar>>bHave;
		if(bHave)
		{
			m_pLightMap=new MBitmap;
			m_pLightMap->Deserialize(ar);
		}
		ar>>bHave;
		if(bHave)
		{
			m_pBump=new MBitmap;
			m_pBump->Deserialize(ar);
		}
		ar>>m_bEnableSpecular;

		IMaterial::Deserialize(ar);
	}

	void MGeneral::BeginTransparent()
	{
		if(NULL==m_pDiffuse)
			return;
		m_pDiffuse->m_bTwoSide=false;

		IMtlModifier* pModBlend=m_pDiffuse->GetModifier(_T("MMFrameBlend"));
		if(NULL==pModBlend)
		{
			MMFrameBlend* pBlend=new MMFrameBlend;
			pBlend->m_blendEnable=true;
			pBlend->m_src=EBlend_SrcAlpha;
			pBlend->m_dst=EBlend_InvSrcAlpha;
			pBlend->SetName(_T("transparent"));
			AttachModifier(pBlend);
		}
		MMAlphaMask* pMask=(MMAlphaMask*)m_pDiffuse->GetModifier(_T("MMAlphaMask"));
		if(NULL!=pMask)
			pMask->m_alphaRef=0;
		
		InvalidateRenderCache();
	}

	void MGeneral::SetTransparent(float alpha,const tagMtlDesc& desc)
	{
		if(alpha>=1.0f)  //不透明
		{
			if(m_bTransparent)
			{
				EndTransparent(desc);
				m_bTransparent=false;
			}
		}
		else //透明
		{
			if(alpha<0.0f)
				alpha=0.0f;

			if(!m_bTransparent)
			{
				BeginTransparent();
				m_bTransparent=true;
			}
			if(NULL!=m_pColor)
			{
				m_pColor->m_color.diffuse.A=alpha;
				UpdateRenderCache();
			}
		}
	}

	void MGeneral::EndTransparent(const tagMtlDesc& desc)
	{
		if(NULL==m_pDiffuse)
			return;

		RemoveModifier(_T("transparent"));

		m_pDiffuse->m_bTwoSide=desc.m_bTwoSide;

		if(NULL!=m_pColor)
			m_pColor->m_color.diffuse.A=1.0f;

		MMAlphaMask* pMask=(MMAlphaMask*)m_pDiffuse->GetModifier(_T("MMAlphaMask"));
		if(NULL!=pMask)
			pMask->m_alphaRef=desc.m_opcityVal;

		InvalidateRenderCache();
	}

	void MGeneral::SetMulColor(const float r, const float g, const float b)
	{
		if(NULL==m_pColor)
			return;

		m_pColor->m_color.ambient.R = m_color.R * r;
		m_pColor->m_color.ambient.G = m_color.G * g;
		m_pColor->m_color.ambient.B = m_color.B * b;
		UpdateRenderCache();
	}
}//namespace Cool3D