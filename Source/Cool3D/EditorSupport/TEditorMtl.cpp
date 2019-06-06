#include "StdAfx.h"
#include ".\teditormtl.h"
#include "..\Device\IDevice.h"
#include "..\Device\IDeviceTex.h"
#include "..\RenderMtl\MBitmap.h"
#include "..\Image\Image.h"
#include "..\Kernel.h"
#include ".\ResDynaAlphaMap.h"
#include "..\ResSys\ResMgr.h"
#include "..\vfs\FileUtil.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(TEditorMtl,IMaterial);

	TEditorMtl::TEditorMtl(void)
	{
		m_pTileTex=NULL;
		m_pAlphaMap=NULL;

		m_color.m_bEnableLight=true;
		m_color.m_bTwoSide=false;
		m_color.m_color.diffuse=Color4f(1.0f,1.0f,1.0f,1.0f);
		m_color.m_color.ambient=Color4f(1.0f,0.7f,0.7f,0.7f);
		m_color.m_color.power=8.0f;
		m_color.m_color.specular=Color4f(1.0f,1.0f,1.0f,1.0f);
	}

	TEditorMtl::~TEditorMtl(void)
	{
		SAFE_DELETE(m_pTileTex);
		RES_MGR_RELEASE_ALPHAMAP(m_pAlphaMap);
	}

	void TEditorMtl::Serialize(FArchive& ar)
	{
		ASSERT(0 && _T("not implement yet"));
		IMaterial::Serialize(ar);
	}

	void TEditorMtl::Deserialize(FArchive& ar)
	{
		
		ASSERT(0 && _T("not implement yet"));
		IMaterial::Deserialize(ar);
	}

	bool TEditorMtl::IsResourceReady() const
	{
		if(m_pTileTex==NULL||!m_pTileTex->IsResourceReady())
			return false;
		if(m_pAlphaMap!=NULL&&!m_pAlphaMap->IsCreated())
			return false;
		return true;
	}

	void TEditorMtl::LoadBaseTex(const TCHAR* szBaseTex)
	{
		SAFE_DELETE(m_pTileTex);
		m_pTileTex=new MBitmap;
		m_pTileTex->SetTexture(0,szBaseTex);
		m_pTileTex->m_mipFilter=ETFilter_LINEAR;
		m_pTileTex->m_magFilter=m_pTileTex->m_minFilter=ETFilter_LINEAR;
		m_pTileTex->m_addressU=m_pTileTex->m_addressV=m_pTileTex->m_addressW=ETAddress_Wrap;
	}

	void TEditorMtl::LoadAlphaMap(const TCHAR* szPath,int size,int initVal)
	{
		RES_MGR_RELEASE_ALPHAMAP(m_pAlphaMap);
		m_pAlphaMap=ResMgr::Inst()->NewDyncAlphaMap(szPath,initVal,size);
	}

	void TEditorMtl::DeleteCacheFile()
	{
		if (m_pAlphaMap!=NULL)
		{
			const TCHAR *szAlphaFile=m_pAlphaMap->GetName();
			if (IsFileExist(szAlphaFile))
				DeleteFile(szAlphaFile);
			RES_MGR_RELEASE_ALPHAMAP(m_pAlphaMap);
		}
	}
}//namespace Cool3D