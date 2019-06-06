#include "StdAfx.h"
#include ".\mbitmap.h"
#include "..\ResSys\ResMgr.h"
#include "..\ResSys\ResTexture.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(MBitmap,IRenderMtl);

	MBitmap::MBitmap(void)
	{
		m_pBitmap=NULL;
		m_magFilter=ETFilter_LINEAR;
		m_minFilter=ETFilter_LINEAR;
		m_mipFilter=ETFilter_POINT;
		m_addressU=m_addressV=m_addressW=ETAddress_Wrap;
		m_numLevels=0;
		m_bIsCube=false;
	}

	MBitmap::~MBitmap(void)
	{
		ResMgr::Inst()->ReleaseTexture(m_pBitmap);
	}

	void MBitmap::SetTexture(UINT index,const TCHAR* szPath,UINT numLevels)
	{
		ResMgr::Inst()->ReleaseTexture(m_pBitmap);
		IMaterial::InvalidateRenderCache();

		m_pBitmap=ResMgr::Inst()->NewTexture(szPath,numLevels);
		m_numLevels=numLevels;
	}

	void MBitmap::Serialize(FArchive& ar)
	{
		ar<<m_magFilter;
		ar<<m_minFilter;
		ar<<m_mipFilter;
		ar<<m_addressU;
		ar<<m_addressV;
		ar<<m_addressW;
		ar<<m_pBitmap->GetName();
		ar<<m_numLevels;

		IRenderMtl::Serialize(ar);
	}

	void MBitmap::Deserialize(FArchive& ar)
	{
		int tmp=0;
		ar>>tmp;		m_magFilter=(ETextrueFilter)tmp;
		ar>>tmp;		m_minFilter=(ETextrueFilter)tmp;
		ar>>tmp;		m_mipFilter=(ETextrueFilter)tmp;
		ar>>tmp;		m_addressU=(ETextureAddress)tmp;
		ar>>tmp;		m_addressV=(ETextureAddress)tmp;
		ar>>tmp;		m_addressW=(ETextureAddress)tmp;

		//--texture
		tstring szTexName;
		ar>>szTexName;
		ar>>m_numLevels;
		if(!szTexName.empty())
			SetTexture(0,szTexName.c_str(),m_numLevels);

		IRenderMtl::Deserialize(ar);
	}
}//namespace Cool3D