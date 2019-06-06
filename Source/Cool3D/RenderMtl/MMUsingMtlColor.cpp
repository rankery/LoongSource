#include "StdAfx.h"
#include ".\MMUsingMtlColor.h"
#include "..\Device\IDevice.h"
#include ".\MGeneral.h"
#include ".\MMColor.h"
#include ".\MConstColor.h"
#include "..\RenderSys\MtlRenderCache.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(MMUsingMtlColor,IMtlModifier);

	MMUsingMtlColor::MMUsingMtlColor(void)
	{
		m_diffuse=Color4f(1,1,1,1);
	}

	MMUsingMtlColor::~MMUsingMtlColor(void)
	{
	}

	void MMUsingMtlColor::Update(float deltaTime,IMaterial *pOwner)
	{
		MConstColor* pColor=NULL;
		if(pOwner->IS_STATIC_CLASS(MGeneral))
			pColor=((MGeneral*)pOwner)->m_pColor;

		if(pColor!=NULL)
		{
			m_diffuse=pColor->m_color.diffuse;

			MtlRenderCache* pCache=pOwner->GetRenderCache();
			if(pCache)
				pCache->UpdateMtlMod(this);
		}

	}

}//namespace Cool3D