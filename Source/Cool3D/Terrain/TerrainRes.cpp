#include "StdAfx.h"
#include ".\TerrainRes.h"
#include "..\Util\Exception.h"
#include "..\VFS\FileUtil.h"
#include "..\EditorSupport\TerrainEditor.h"
#include ".\TerrainMtlMultiLayers.h"
#include ".\TerrainPatchRenderGroupsRes.h"
#include "..\Device\IVertBuffer.h"
#include "..\Device\IDevice.h"
#include "..\Kernel.h"
#include ".\Heightmap.h"

namespace Cool3D
{
	IMP_RTTI(TerrainRes,ResRenderable);

	TerrainRes::TerrainRes(const TCHAR* szName):ResRenderable(szName)
	{
		m_pTexCoord2VB=NULL;
		m_pTexCoord4VB=NULL;
		m_mtlDiffuse.R=m_mtlDiffuse.G=m_mtlDiffuse.B=m_mtlDiffuse.A=1.0f;
		m_mtlAmbient.R=m_mtlAmbient.G=m_mtlAmbient.B=m_mtlAmbient.A=1.0f;
		m_mtlSpecular.R=m_mtlSpecular.G=m_mtlSpecular.B=m_mtlSpecular.A=1.0f;
		m_mtlPower=8.0f;
	}

	TerrainRes::~TerrainRes(void)
	{
		SAFE_DELETE(m_pTexCoord2VB);
		SAFE_DELETE(m_pTexCoord4VB);
	}

	UINT TerrainRes::CreateContent(DWORD param)
	{
		IFS *pFS=GetSafeFS();
		const TCHAR* szName=GetName();
		DWORD hFile=pFS->Open(szName);
		THROW_NULLEX(hFile,_T("Terrain patch load faied"),szName);
		
		//--read header
		tagHeader header;
		pFS->Read(&header,sizeof(tagHeader),hFile);
		m_mtlDiffuse=header.diffuse;
		m_mtlAmbient=header.ambient;
		m_mtlSpecular=header.specular;
		m_mtlPower=header.power;

		//--load mtl
		pFS->Seek(hFile,header.mtlOffset,FILE_BEGIN);
		m_layerDescs.resize(header.numLayers);

		UINT i;
		for(i=0;i<header.numLayers;i++)
		{
			tagLayerDesc& desc=m_layerDescs[i];
			FReadString(pFS,hFile,desc.szTileTex);
			FReadValue(pFS,hFile,desc.uvSetIndex);
		}

		//--load uv2 vertstream
		m_texCoord2VS.Create(EVType_T2,header.numVerts);
		pFS->Seek(hFile,header.uv2Offset,FILE_BEGIN);
		pFS->Read(m_texCoord2VS.GetRawBuffer(),m_texCoord2VS.GetBufferSize(),hFile);		

		//--create uv2 vertexbuffer
		m_pTexCoord2VB=Device()->NewVB();
		m_pTexCoord2VB->Create(m_texCoord2VS.GetBufferSize(),EB_StaticWriteOnly,m_texCoord2VS.GetStride());
		m_pTexCoord2VB->CopyFromVertStream(&m_texCoord2VS);

		//--load uv4 vertstream
		m_texCoord4VS.Create(EVType_T4,header.numVerts);
		pFS->Seek(hFile,header.uv4Offset,FILE_BEGIN);
		pFS->Read(m_texCoord4VS.GetRawBuffer(),m_texCoord4VS.GetBufferSize(),hFile);		

		//--create uv4 vertexbuffer
		m_pTexCoord4VB=Device()->NewVB();
		m_pTexCoord4VB->Create(m_texCoord4VS.GetBufferSize(),EB_StaticWriteOnly,m_texCoord4VS.GetStride());
		m_pTexCoord4VB->CopyFromVertStream(&m_texCoord4VS);

		//--close file
		pFS->Close(hFile);

		m_box.min=header.min;
		m_box.max=header.max;

		return ResRenderable::CreateContent(param);
	}

	UINT TerrainRes::GetNumRenderNode() const
	{
		return 0;
	}

	RenderNode *TerrainRes::GetRenderNode(UINT i) const
	{
		return NULL;
	}

	const AABBox* TerrainRes::GetAABBox() const
	{
		return &m_box;
	}

	bool TerrainRes::EditorSave(const TCHAR* szPath,RECT myRect,TerrainEditor *pEditor)
	{
		FILE *fp=_tfopen(szPath,_T("wb"));
		if(fp==NULL)
			return false;

		Heightmap *pHMap=(Heightmap*)pEditor->GetHeightmap();

		tagHeader header;
		memset(&header,0,sizeof(tagHeader));
		_tcscpy((TCHAR*)header.magic,_T("TERRAIN"));
		header.numVerts=TerrainEditor::EPatchVerts;
		header.numLayers=TerrainEditor::MAX_MAP_LAYER;
		header.diffuse=pEditor->GetMtlDiffuse();
		header.ambient=pEditor->GetMtlAmbient();
		header.specular=pEditor->GetMtlSpecular();
		header.power=pEditor->GetMtlPower();
		
		//计算Box
		AABBox box;
		pHMap->BuildAABBox(&box,&myRect);
		header.min=box.min;
		header.max=box.max;

		fwrite(&header,sizeof(header),1,fp);

		//保存uv2
		VertStream texCoord2VS;
		texCoord2VS.Create(EVType_T2,TerrainEditor::EPatchVerts);
		FillVertStream_T2(texCoord2VS);
		header.uv2Offset=ftell(fp);
		fwrite(texCoord2VS.GetRawBuffer(),texCoord2VS.GetBufferSize(),1,fp);

		//保存uv4
		VertStream texCoord4VS;
		texCoord4VS.Create(EVType_T4,TerrainEditor::EPatchVerts);
		FillVertStream_T4(texCoord4VS,pEditor);
		header.uv4Offset=ftell(fp);
		fwrite(texCoord4VS.GetRawBuffer(),texCoord4VS.GetBufferSize(),1,fp);

		//保存mtl
		header.mtlOffset=ftell(fp);
		int i;
		for(i=0;i<TerrainEditor::MAX_MAP_LAYER;i++)
		{
			const tagTMapLayer* pLayer=pEditor->GetMapLayer(i);
			FWriteString(fp,pLayer->szTexFile);
			FWriteValue(fp,pLayer->uvSetIndex);
		}

		//回写Header
		fseek(fp,0,SEEK_SET);
		fwrite(&header,sizeof(header),1,fp);
		
		fclose(fp);
		return true;
	}

	void TerrainRes::FillVertStream_T4(VertStream& vertStream,TerrainEditor *pEditor)
	{
		ASSERT(vertStream.GetType()==EVType_T4);
		ASSERT(vertStream.GetNumVert()==TerrainEditor::EPatchVerts);

		Vector2 uvStep[TRN_MAX_UV_SET];
		float vertSpace=TerrainEditor::EPatchSize/(TerrainEditor::EPatchSideVerts-1);//两个顶点之间相隔多少个格子
		int i;
		for(i=0;i<TRN_MAX_UV_SET;i++)
		{
			uvStep[i].x=1/pEditor->GetLayerUVScale(i).x*vertSpace;
			uvStep[i].y=1/pEditor->GetLayerUVScale(i).y*vertSpace;
		}

		Vert_T4* pVert=(Vert_T4*)vertStream.GetRawBuffer();
		for(int z=0; z<TerrainEditor::EPatchSideVerts; z++)
		{
			for(int x=0; x<TerrainEditor::EPatchSideVerts; x++)
			{
				for(i=0;i<TRN_MAX_UV_SET;i++)
				{
					pVert->uv[i].x=uvStep[i].x*x;
					pVert->uv[i].y=uvStep[i].y*z;
				}
				pVert++;
			}
		}
	}

	void TerrainRes::FillVertStream_T2(VertStream& vertStream)
	{
		Vector2 uvScales[2];
		uvScales[0]=Vector2(8,8);
		uvScales[1]=Vector2(TerrainEditor::EPatchSize,TerrainEditor::EPatchSize);

		Vector2 uvSteps[2];
		float vertSpace=(float)TerrainEditor::EPatchSize/(TerrainEditor::EPatchSideVerts-1);//两个顶点之间相隔多少个格子
		int i;
		for(i=0;i<2;i++)
		{
			uvSteps[i].x=1/uvScales[i].x*vertSpace;
			uvSteps[i].y=1/uvScales[i].y*vertSpace;
		}

		ASSERT(vertStream.GetStride()==sizeof(Vert_T2));
		ASSERT(vertStream.GetNumVert()==TerrainEditor::EPatchVerts);
		Vert_T2* pVert=(Vert_T2*)vertStream.GetRawBuffer();
		for(int z=0; z<TerrainEditor::EPatchSideVerts; z++)
		{
			for(int x=0; x<TerrainEditor::EPatchSideVerts; x++)
			{
				for(i=0;i<2;i++)
				{
					pVert->uv[i].x=uvSteps[i].x*x;
					pVert->uv[i].y=uvSteps[i].y*z;
				}
				pVert++;
			}
		}
	}

	void TerrainRes::CreateRenderGroupsMtls(TerrainPatchRenderGroupsRes* pRes,const TCHAR* szLightMap,vector<IMaterial*>& mtls)
	{
		for(int i=0;i<pRes->GetNumRenderGroups();i++)
		{
			const TerrainPatchRenderGroupsRes::tagRenderGroup& renderGroup=pRes->GetRenderGroup(i);

			TerrainMtlMultiLayers* pMtl=new TerrainMtlMultiLayers;
			pMtl->m_color.m_color.diffuse=m_mtlDiffuse;
			pMtl->m_color.m_color.ambient=m_mtlAmbient;
			pMtl->m_color.m_color.specular=m_mtlSpecular;
			pMtl->m_color.m_color.power=m_mtlPower;

			int numLayers=(int)renderGroup.layers.size();

			pMtl->BeginLayers(numLayers);
			for(size_t j=0;j<renderGroup.layers.size();j++)
			{
				int layerIndex=renderGroup.layers[j];
				const tagLayerDesc& desc=m_layerDescs[layerIndex];
				pMtl->AddLayer(desc.szTileTex.c_str(),desc.uvSetIndex);
			}
			pMtl->EndLayers();

			if(!renderGroup.szAlphaMap.empty())
				pMtl->LoadAlphaMap(renderGroup.szAlphaMap.c_str());
			if(szLightMap!=NULL&&szLightMap[0]!=_T('\0'))
				pMtl->LoadLightMap(szLightMap);

			pMtl->LoadShaders(numLayers);
			mtls.push_back(pMtl);
		}
	}

	const Color4f& TerrainRes::GetMtlDiffuse() const
	{
		return m_mtlDiffuse;
	}

	const Color4f& TerrainRes::GetMtlAmbient() const
	{
		return m_mtlAmbient;
	}

	const Color4f& TerrainRes::GetMtlSpecular() const
	{
		return m_mtlSpecular;
	}

	float TerrainRes::GetMtlPower() const
	{
		return m_mtlPower;
	}
}//namespace Cool3D