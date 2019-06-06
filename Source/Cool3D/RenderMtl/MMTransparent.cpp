#include "StdAfx.h"
#include ".\MMTransparent.h"
#include ".\MGeneral.h"
#include ".\MTwoLayers.h"
#include "..\RenderSys\MtlRenderCache.h"
#include ".\MMAlphaMask.h"
#include "..\Kernel.h"
#include ".\MDyeTex.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(MMTransparent,IMtlModifier);

	MMTransparent::MMTransparent(void)
	{
	}

	MMTransparent::~MMTransparent(void)
	{
	}

	void MMTransparent::Update(float deltaTime,IMaterial *pOwner)
	{
	}

}//namespace Cool3D