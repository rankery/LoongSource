#include "StdAfx.h"
#include ".\MMColor.h"
#include ".\MGeneral.h"
#include ".\MTwoLayers.h"
#include "..\RenderSys\MtlRenderCache.h"
#include ".\MConstColor.h"
#include ".\MDyeTex.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(MMColor,IMtlModifier);

	MMColor::MMColor(void)
	{
		m_shadowDepth=1.0f;
		m_diffuse=Vector3(1,1,1);
		m_ambient=Vector3(1,1,1);
	}

	MMColor::~MMColor(void)
	{
	}

	void MMColor::Update(IMaterial *pOwner)
	{
		MConstColor* pColor=NULL;
		if(pOwner->IS_STATIC_CLASS(MGeneral))
			pColor=((MGeneral*)pOwner)->m_pColor;
		else if(pOwner->IS_STATIC_CLASS(MTwoLayers))
			pColor=((MTwoLayers*)pOwner)->m_pColor;
		else if(pOwner->IS_STATIC_CLASS(MDyeTex))
			pColor=((MDyeTex*)pOwner)->m_pColor;

		if(pColor!=NULL)
		{
			m_diffuse.x=pColor->m_color.diffuse.R*m_shadowDepth;
			m_diffuse.y=pColor->m_color.diffuse.G*m_shadowDepth;
			m_diffuse.z=pColor->m_color.diffuse.B*m_shadowDepth;

			m_ambient.x=pColor->m_color.ambient.R*m_shadowDepth;
			m_ambient.y=pColor->m_color.ambient.G*m_shadowDepth;
			m_ambient.z=pColor->m_color.ambient.B*m_shadowDepth;

			MtlRenderCache* pCache=pOwner->GetRenderCache();
			if(pCache)
				pCache->UpdateMtlMod(this);
		}
	}

	void MMColor::SetShadowEffect(float shadowDepth,IMaterial* pOwner)
	{
		m_shadowDepth=shadowDepth;
		Update(pOwner);
	}

	void MMColor::RemoveShadowEffect(IMaterial* pOwner)
	{
		m_shadowDepth=1.0f;
		Update(pOwner);
	}

}//namespace Cool3D