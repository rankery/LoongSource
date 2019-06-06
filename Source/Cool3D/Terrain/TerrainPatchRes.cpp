#include "StdAfx.h"
#include ".\terrainpatchres.h"
#include "..\Util\Exception.h"
#include "Heightmap.h"
#include "..\VFS\FileUtil.h"
#include "..\ResSys\ResMgr.h"
#include "..\EditorSupport\TerrainEditor.h"
#include ".\TerrainMtlTwoLayers.h"
#include ".\TerrainMtlPrecombine.h"
#include "..\Device\IVertBuffer.h"
#include "..\Device\IDevice.h"
#include "..\Kernel.h"
#include "..\RenderMtl\MGeneral.h"

namespace Cool3D
{
	IMP_RTTI(TerrainPatchRes,ResRenderable);

	TerrainPatchRes::TerrainPatchRes(const TCHAR* szName):
	ResRenderable(szName),
	m_pPosNormalVB(NULL),
	m_riverID(-1),
	m_underWaterFogStart(0.0f),
	m_underWaterFogEnd(0.0f),
	m_underWaterFogColor( 0.0f, 0.0f, 0.0f )
	{
	}

	TerrainPatchRes::~TerrainPatchRes(void)
	{
		SAFE_DELETE(m_pPosNormalVB);
	}

	UINT TerrainPatchRes::CreateContent(DWORD param)
	{
		IFS *pFS=GetSafeFS();
		const TCHAR* szName=GetName();
		DWORD hFile=pFS->Open(szName);
		THROW_NULLEX(hFile,_T("Terrain patch load failed"),szName);
		
		//--read header
		tagHeader header;
		pFS->Read(&header,sizeof(tagHeader),hFile);
		m_szPrecombineImage=header.szPrecombineImage;
		m_box=header.box;
		m_patchX=header.patchX;
		m_patchZ=header.patchZ;
		m_riverID=header.riverID;
		m_underWaterFogStart=header.underWaterFogStart;
		m_underWaterFogEnd=header.underWaterFogEnd;
		m_underWaterFogColor=header.underWaterFogColor;


		//--load vertstream
		m_posNormalVS.Create(EVType_PN,header.numVerts);
		pFS->Seek(hFile,header.vertsOffset,FILE_BEGIN);
		pFS->Read(m_posNormalVS.GetRawBuffer(),m_posNormalVS.GetBufferSize(),hFile);		

		//--create vertexbuffer
		m_pPosNormalVB=Device()->NewVB();
		m_pPosNormalVB->Create(m_posNormalVS.GetBufferSize(),EB_StaticWriteOnly,m_posNormalVS.GetStride());
		m_pPosNormalVB->CopyFromVertStream(&m_posNormalVS);

		//--close file
		pFS->Close(hFile);

		return ResRenderable::CreateContent(param);
	}

	UINT TerrainPatchRes::GetNumRenderNode() const
	{
		return 0;
	}

	RenderNode *TerrainPatchRes::GetRenderNode(UINT i) const
	{
		return NULL;
	}

	const AABBox *TerrainPatchRes::GetAABBox() const
	{
		return &m_box;
	}

	bool TerrainPatchRes::EditorSave(const TCHAR* szPath,RECT myRect,
		const TCHAR* szPrecombineImage,TerrainEditor *pEditor)
	{
		FILE *fp=_tfopen(szPath,_T("wb"));
		if(fp==NULL)
			return false;

		Heightmap *pHMap=(Heightmap*)pEditor->GetHeightmap();

		tagHeader header;
		memset(&header,0,sizeof(tagHeader));
		_tcscpy((TCHAR*)header.magic,_T("PATCH"));

		//预混合贴图路径
		_tcsncpy(header.szPrecombineImage,szPrecombineImage,sizeof(header.szPrecombineImage)/sizeof(TCHAR));

		//AABBox
		pHMap->BuildAABBox(&header.box,&myRect);

		//地块坐标
		header.patchX=myRect.left/TerrainEditor::EPatchSize;
		header.patchZ=myRect.top/TerrainEditor::EPatchSize;
		
		//默认没有河
		header.riverID = -1;

		fwrite(&header,sizeof(header),1,fp);

		//保存顶点
		VertStream vertStream;
		vertStream.Create(EVType_PN,TerrainEditor::EPatchVerts);
		pHMap->FillVertStream_PN(&vertStream,TerrainEditor::EPatchSideVerts,myRect);
		header.numVerts=vertStream.GetNumVert();
		header.vertsOffset=ftell(fp);
		fwrite(vertStream.GetRawBuffer(),vertStream.GetBufferSize(),1,fp);

		//回写Header
		fseek(fp,0,SEEK_SET);
		fwrite(&header,sizeof(header),1,fp);
		
		fclose(fp);
		return true;
	}

	IMaterial* TerrainPatchRes::CreateTwoLayersMtl()
	{
		TerrainMtlTwoLayers* pMtl=new TerrainMtlTwoLayers;

		//combine tex
		MBitmap* pCombineTex=new MBitmap;
		pCombineTex=new MBitmap;
		pCombineTex->SetTexture(0,m_szPrecombineImage.c_str());
		pCombineTex->m_magFilter=ETFilter_LINEAR;
		pCombineTex->m_minFilter=ETFilter_LINEAR;
		pCombineTex->m_mipFilter=ETFilter_LINEAR;
		pCombineTex->m_addressU
			=pCombineTex->m_addressV
			=pCombineTex->m_addressW
			=ETAddress_Clamp;
		pMtl->m_pCombineTex=pCombineTex;

		//detail tex
		MBitmap* pDetailTex=new MBitmap;
		pDetailTex->SetTexture(0,_T("data\\system\\trntex\\detailtex0.tga"));
		pDetailTex->m_magFilter=ETFilter_LINEAR;
		pDetailTex->m_minFilter=ETFilter_LINEAR;
		pDetailTex->m_mipFilter=ETFilter_LINEAR;
		pDetailTex->m_addressU
			=pDetailTex->m_addressV
			=pDetailTex->m_addressW
			=ETAddress_Wrap;
		pMtl->m_pDetailTex=pDetailTex;

		return pMtl;
	}

	IMaterial* TerrainPatchRes::CreatePrecombineMtl()
	{
		TerrainMtlPrecombine* pMtl=new TerrainMtlPrecombine;

		MBitmap* pDiff=new MBitmap;
		pDiff->SetTexture(0,m_szPrecombineImage.c_str());
		pDiff->m_magFilter=pDiff->m_minFilter=ETFilter_LINEAR;
		pDiff->m_mipFilter=ETFilter_POINT;
		pDiff->m_addressU=pDiff->m_addressV=pDiff->m_addressW=ETAddress_Clamp;
		pMtl->m_pPrecombineTex=pDiff;

		pMtl->LoadShader();

		return pMtl;
	}

	IMaterial* TerrainPatchRes::CreateEditorMtl(const Color4f& diffuse,const Color4f& ambient,
		const Color4f& specular,float power)
	{
		MGeneral* pMtl=new MGeneral;

		//diffuse tex
		MBitmap* pDiff=new MBitmap;
		pDiff->SetTexture(0,m_szPrecombineImage.c_str());
		pDiff->m_magFilter=pDiff->m_minFilter=ETFilter_LINEAR;
		pDiff->m_mipFilter=ETFilter_POINT;
		pDiff->m_addressU=pDiff->m_addressV=pDiff->m_addressW=ETAddress_Clamp;
		pMtl->m_pDiffuse=pDiff;

		//color
		MConstColor* pColor=new MConstColor;
		pColor->m_bEnableLight=true;
		pColor->m_bTwoSide=false;
		pColor->m_color.diffuse=diffuse;
		pColor->m_color.ambient=ambient;
		pColor->m_color.specular=specular;
		pColor->m_color.power=power;
		pMtl->m_pColor=pColor;

		//uv index
		pMtl->m_uvIndexDiffuse=1;
		pMtl->m_uvIndexLightMap=1;

		return pMtl;
	}

}//namespace Cool3D