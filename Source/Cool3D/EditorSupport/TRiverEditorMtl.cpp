#include "stdafx.h"
#include ".\BrushOp.h"
#include ".\TRiverEditorMtl.h"
#include ".\ResDynaAlphaMap.h"
#include ".\TRiverEditor.h"
#include "..\ResSys\ResMgr.h"
#include "..\vfs\FileUtil.h"
#include "..\Terrain\Heightmap.h"
#include "..\Image\Image.h"
#include "..\Util\FMemory.h"

namespace Cool3D
{
	IMP_RTTI_DYNC(TRiverEditorMtl,IMaterial);

	using namespace Detail;

	TRiverEditorMtl::TRiverEditorMtl()
	{
		m_pTex=NULL;
		m_pMap=NULL;
		m_name=_T("");
		m_bNeedSave=false;
		m_color.m_color.diffuse=Color4f(1.0f,1.0f,1.0f,1.0f);
		m_color.m_color.ambient=Color4f(1.0f,1.0f,1.0f,1.0f);
		m_alpha=1.0f;
	}

	TRiverEditorMtl::~TRiverEditorMtl()
	{
		SAFE_DELETE(m_pTex);
		RES_MGR_RELEASE_ALPHAMAP(m_pMap);
	}

	bool TRiverEditorMtl::Create( const TCHAR *szTexFile,const TCHAR *szMapFile,BYTE initVal,float alpha )
	{
		SAFE_DELETE(m_pTex);
		RES_MGR_RELEASE_ALPHAMAP(m_pMap);

		m_name=szMapFile;
		m_bNeedSave=false;
		m_alpha=alpha;

		LoadBaseTex(szTexFile);
		if(szMapFile!=NULL)
			LoadMap(szMapFile,TRiverEditor::EPatchAlphaMapSize,initVal);

		return true;
	}

	void TRiverEditorMtl::Serialize( FArchive& ar )
	{
		ASSERT(0 && _T("not implement yet"));
		IMaterial::Serialize(ar);
	}

	void TRiverEditorMtl::Deserialize( FArchive& ar )
	{
		ASSERT(0 && _T("not implement yet"));
		IMaterial::Deserialize(ar);
	}

	bool TRiverEditorMtl::IsResourceReady() const
	{
		if(m_pTex==NULL
			|| !m_pTex->IsResourceReady())
			return false;
		if(m_pMap!=NULL
			&& !m_pMap->IsCreated())
			return false;

		return true;
	}

	void TRiverEditorMtl::LoadBaseTex( const TCHAR* szBaseTex )
	{
		SAFE_DELETE(m_pTex);
		m_pTex=new MBitmap;
		m_pTex->SetTexture(0,szBaseTex);
		m_pTex->m_mipFilter=ETFilter_LINEAR;
		m_pTex->m_magFilter=m_pTex->m_minFilter=ETFilter_LINEAR;
		m_pTex->m_addressU=m_pTex->m_addressV=m_pTex->m_addressW=ETAddress_Wrap;
	}

	void TRiverEditorMtl::LoadMap( const TCHAR* szPath,int size,int initVal )
	{
		RES_MGR_RELEASE_ALPHAMAP(m_pMap);
		m_pMap=ResMgr::Inst()->NewDyncAlphaMap(szPath,initVal,size);
	}

	void TRiverEditorMtl::DeleteCacheFile()
	{
		if (m_pMap!=NULL)
		{
			const TCHAR *szFileName=m_pMap->GetName();
			if (IsFileExist(szFileName))
				DeleteFile(szFileName);
			RES_MGR_RELEASE_ALPHAMAP(m_pMap);
			ResMgr::Inst()->DoGC();//清空资源池中的cache
		}
	}

	void TRiverEditorMtl::PostEditChange( const tagTEditBrush& brush )
	{
		if(!IsResourceReady()
			|| m_pMap==NULL)
			return;

		RECT brushRc;
		brushRc.left=brush.centerX-brush.outer;
		brushRc.right=brush.centerX+brush.outer;
		brushRc.top=brush.centerZ-brush.outer;
		brushRc.bottom=brush.centerZ+brush.outer;
		m_pMap->UpdateTexture(&brushRc);

		m_bNeedSave=true;
	}

	void TRiverEditorMtl::SaveMapToFile()
	{
		if(!m_bNeedSave)
			return;

		if(m_pMap==NULL
			|| !m_pMap->IsCreated())
			return;

		if(IsEmptyAlpha())
			::DeleteFile(m_pMap->GetName());
		else
			m_pMap->SaveToFile();

		m_bNeedSave=false;
	}

	void TRiverEditorMtl::SaveMapToImage( const TCHAR *szImagePath )
	{
		if (m_pMap==NULL
			|| !m_pMap->IsCreated())
			return;

		m_pMap->SaveToImage(szImagePath,m_pMap->GetHeightmap()->Width());
	}

	bool TRiverEditorMtl::IsEmptyAlpha()
	{
		ASSERT(IsResourceReady());
		if(m_pMap==NULL)
			return false;

		Heightmap* pMap=m_pMap->GetHeightmap();

		RECT rc;
		rc.left=rc.top=0;
		rc.right=pMap->Width();
		rc.bottom=pMap->Height();
		float min,max;
		pMap->GetHeightRange(min,max,rc);
		return max<1 && max-min<1;
	}

	void TRiverEditorMtl::BrushMap( const tagTEditBrush& brush )
	{
		if(!IsResourceReady()
			|| m_pMap==NULL)
			return;

		Heightmap* pMap=m_pMap->GetHeightmap();

		//--do brush
		BOP_AddBrush burshOp(NULL);
		DoBrush(pMap,brush,burshOp);

		//--update texture
		PostEditChange(brush);
	}

	void TRiverEditorMtl::SmoothMap( const tagTEditBrush& brush )
	{
		if(!IsResourceReady()
			|| m_pMap==NULL)
			return;

		Heightmap* pMap=m_pMap->GetHeightmap();

		BOP_SmoothFilter flt(3);
		DoBrush(pMap,brush,flt);
		flt.Apply(pMap);
		//--update texture
		PostEditChange(brush);
	}

	void TRiverEditorMtl::NoiseMap( const tagTEditBrush& brush,float noiseFreq )
	{
		if(!IsResourceReady()
			|| m_pMap==NULL)
			return;

		Heightmap* pMap=m_pMap->GetHeightmap();

		//--do brush
		POINT tPt={brush.centerX,brush.centerZ};
		tPt.x-=brush.outer;
		tPt.y-=brush.outer;

		BOP_ApplyNoise ns(brush.outer*2,noiseFreq,tPt.x,tPt.y);
		DoBrush(pMap,brush,ns);

		//--update texture
		PostEditChange(brush);
	}

	void TRiverEditorMtl::CopyAlphaMap2Img( Image* pImage,int w,int h )
	{
		ASSERT(IsResourceReady());
		if(m_pMap==NULL)
			return;

		Heightmap *pMap=m_pMap->GetHeightmap();
		pImage->Create(pMap->Width(),pMap->Height(),EPF_A8);
		FMemcpy(pImage->GetRawBuffer(),pMap->GetRawBuffer(),pMap->Width()*pMap->Height());
		pImage->Scale(w,h,EIS_Fastest);
	}
}//namespace Cool3D