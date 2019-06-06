#include "StdAfx.h"
#include ".\mbmpexplicitalpha.h"
#include "..\ResSys\ResMgr.h"

namespace F3D
{
	IMP_RTTI_DYNC(MBmpExplicitAlpha,MBitmap);

	MBmpExplicitAlpha::MBmpExplicitAlpha(void) : MBitmap()
	{
		m_pAlpha=NULL;
	}

	MBmpExplicitAlpha::~MBmpExplicitAlpha(void)
	{
		ResMgr::Inst()->ReleaseTexture(m_pAlpha);
	}

	void MBmpExplicitAlpha::SetTexture(UINT index,const char* szPath)
	{
		if(index==0)
			MBitmap::SetTexture(index,szPath);
		else
		{
			ResMgr::Inst()->ReleaseTexture(m_pAlpha);
			m_pAlpha=ResMgr::Inst()->NewTexture(szPath);
		}
	}
}//namespace F3D