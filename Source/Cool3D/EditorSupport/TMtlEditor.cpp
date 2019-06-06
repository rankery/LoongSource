#include "StdAfx.h"
#include ".\tmtleditor.h"
#include "..\Kernel.h"
#include "..\Util\Exception.h"
#include "..\Util\FMemory.h"
#include "..\Device\IDevice.h"
#include "..\Device\IDeviceTex.h"
#include "..\ResSys\ResTexture.h"
#include "..\Math\Noise.h"
#include "..\Terrain\Heightmap.h"
#include "BrushOp.h"
#include "..\Image\ImageTGA.h"
#include "..\Terrain\TerrainPatchRes.h"
#include "..\ResSys\ResMgr.h"
#include "..\RenderMtl\MBitmap.h"


namespace Cool3D
{
	using namespace Detail;

	TMtlEditor::TMtlEditor(void)
	{
		m_pMtl=NULL;
		m_bNeedSave=false;
	}

	TMtlEditor::~TMtlEditor(void)
	{
		Destroy();
	}

	void TMtlEditor::Destroy()
	{
		SAFE_DELETE(m_pMtl);
	}

	bool TMtlEditor::Create(const TCHAR* szTexFile,const TCHAR* szAlphaMap,BYTE alphaInitVal,const TCHAR* szName,
		int uvSetIndex,const Color4f& diffuse,const Color4f& ambient,const Color4f& specular)
	{
		Destroy();

		m_szName=szName;
		m_bNeedSave=false;

		m_pMtl=new TEditorMtl;
		m_pMtl->m_color.m_color.diffuse=diffuse;
		m_pMtl->m_color.m_color.ambient=ambient;
		m_pMtl->m_color.m_color.specular=specular;
		m_pMtl->LoadBaseTex(szTexFile);
		if(szAlphaMap!=NULL)
			m_pMtl->LoadAlphaMap(szAlphaMap,TerrainEditor::EPatchAlphaMapSize,alphaInitVal);
		m_pMtl->m_uvSetIndex=uvSetIndex;

		return true;
	}

	void TMtlEditor::PostEditChange(const tagTEditBrush& brush)
	{
		if(!m_pMtl->IsResourceReady()
			||m_pMtl->m_pAlphaMap==NULL)
			return;

		RECT brushRc;
		brushRc.left=brush.centerX-brush.outer;
		brushRc.right=brush.centerX+brush.outer;
		brushRc.top=brush.centerZ-brush.outer;
		brushRc.bottom=brush.centerZ+brush.outer;
		m_pMtl->m_pAlphaMap->UpdateTexture(&brushRc);

		m_bNeedSave=true;
	}

	void TMtlEditor::SmoothAlpha(const tagTEditBrush& brush)
	{
		if(!m_pMtl->IsResourceReady()
			||m_pMtl->m_pAlphaMap==NULL)
			return;

		Heightmap* pHMap=m_pMtl->m_pAlphaMap->GetHeightmap();

		BOP_SmoothFilter flt(3);
		DoBrush(pHMap,brush,flt);
		flt.Apply(pHMap);
		//--update texture
		PostEditChange(brush);
	}

	void TMtlEditor::BrushAlpha(const tagTEditBrush& brush)
	{
		if(!m_pMtl->IsResourceReady()
			||m_pMtl->m_pAlphaMap==NULL)
			return;

		Heightmap* pHMap=m_pMtl->m_pAlphaMap->GetHeightmap();

		//--do brush
		BOP_AddBrush burshOp(NULL);
		DoBrush(pHMap,brush,burshOp);

		//--update texture
		PostEditChange(brush);
	}

	void TMtlEditor::NoiseAlpha(const tagTEditBrush& brush,float noiseFreq)
	{
		if(!m_pMtl->IsResourceReady()
			||m_pMtl->m_pAlphaMap==NULL)
			return;

		Heightmap* pHMap=m_pMtl->m_pAlphaMap->GetHeightmap();

		//--do brush
		POINT tPt={brush.centerX,brush.centerZ};
		tPt.x-=brush.outer;
		tPt.y-=brush.outer;

		BOP_ApplyNoise ns(brush.outer*2,noiseFreq,tPt.x,tPt.y);
		DoBrush(pHMap,brush,ns);

		//--update texture
		PostEditChange(brush);
	}

	bool TMtlEditor::IsEmptyAlpha()
	{
		ASSERT(m_pMtl->IsResourceReady());
		if(m_pMtl->m_pAlphaMap==NULL)
			return false;

		Heightmap* pHMap=m_pMtl->m_pAlphaMap->GetHeightmap();

		RECT rc;
		rc.left=rc.top=0;
		rc.right=pHMap->Width();
		rc.bottom=pHMap->Height();
		float min,max;
		pHMap->GetHeightRange(min,max,rc);
		return max<1 && max-min < 1;
	}

	void TMtlEditor::CopyAlphaMap2Img(Image* pImage,int w,int h)
	{
		ASSERT(m_pMtl->IsResourceReady());
		if(m_pMtl->m_pAlphaMap==NULL)
			return;
		Heightmap *pHMap=m_pMtl->m_pAlphaMap->GetHeightmap();
		pImage->Create(pHMap->Width(),pHMap->Height(),EPF_A8);
		FMemcpy(pImage->GetRawBuffer(),pHMap->GetRawBuffer(),pHMap->Width()*pHMap->Height());
		pImage->Scale(w,h,EIS_Fastest);
	}

	void TMtlEditor::Blend2FinalImage(Image *pDestImg,bool bForceCopy)
	{
		//todo
	}

	bool TMtlEditor::IsFullOpacity()
	{
		ASSERT(m_pMtl->IsResourceReady());
		if(m_pMtl->m_pAlphaMap==NULL)
			return true;

		Heightmap* pHMap=m_pMtl->m_pAlphaMap->GetHeightmap();

		RECT rc;
		rc.left=rc.top=0;
		rc.right=pHMap->Width();
		rc.bottom=pHMap->Height();
		float min,max;
		pHMap->GetHeightRange(min,max,rc);
		return min==255.0f && max==255.0f;
	}

	void TMtlEditor::DeleteCacheFile()
	{
		m_pMtl->DeleteCacheFile();
	}

	void TMtlEditor::SaveAlphaMapToFile()
	{
		if(m_pMtl->m_pAlphaMap==NULL
			||!m_pMtl->m_pAlphaMap->IsCreated())
			return;

		if(!m_bNeedSave)
			return;

		if(IsEmptyAlpha())
			::DeleteFile(m_pMtl->m_pAlphaMap->GetName());
		else
			m_pMtl->m_pAlphaMap->SaveToFile();

		m_bNeedSave=false;
	}

}//namespace Cool3D