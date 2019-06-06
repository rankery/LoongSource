#include "StdAfx.h"
#include ".\MPreCombineDiffuse.h"
#include "..\Image\ImageTGA.h"
#include "..\ResSys\ResTexture.h"
#include "..\Device\IDevice.h"
#include "..\Device\IDeviceTex.h"
#include "..\Kernel.h"

namespace F3D
{
	IMP_RTTI_DYNC(MPreCombineDiffuse,IRenderMtl);

	MPreCombineDiffuse::MPreCombineDiffuse(void)
	{
		m_pBaseTex=new MBitmap;
	}

	MPreCombineDiffuse::~MPreCombineDiffuse(void)
	{
		SAFE_DELETE(m_pBaseTex);
	}

	bool MPreCombineDiffuse::IsResourceReady() const
	{
		return m_pBaseTex!=NULL 
			&& m_pBaseTex->IsResourceReady();
	}
}//namespace F3D