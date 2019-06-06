#include "StdAfx.h"
#include "..\Image\Image.h"
#include "..\Util\FMemory.h"
#include "..\Util\Exception.h"
#include "..\Terrain\Heightmap.h"
#include "..\Terrain\TerrainPatchRes.h"
#include "..\Terrain\ResGrass.h"
#include "TQuadTreeTriangulation.h"
#include "..\Kernel.h"
#include "..\ResSys\VertStream.h"
#include "..\Device\IDevice.h"
#include "..\Device\IIndexBuffer.h"
#include "..\Device\IVertBuffer.h"
#include "..\Device\IRenderTexture.h"
#include "..\Device\IPixelShader.h"
#include "..\RenderSys\RenderSys.h"
#include "..\RenderSys\MtlRenderCache.h"
#include "TMtlEditor.h"
#include ".\teditorpatch.h"
#include "..\RenderMtl\MBitmap.h"
#include "..\VFS\IFS.h"
#include "TerrainEditor.h"
#include "..\Image\ImageTGA.h"
#include "..\Terrain\Bitmap.h"
#include "..\Terrain\TPatchTriangulationStatic.h"
#include "..\Terrain\TPatchTriangulationDynamic.h"
#include ".\IUICallBack.h"

namespace Cool3D
{
	//--使得Rect落在height map的尺寸允许的范围内,并且不改变Rect的尺寸
	void ClipHeightMapRect(Heightmap *pHMap,RECT &rc)
	{
		if(rc.left<0)
		{
			rc.right-=rc.left;
			rc.left=0;
		}
		if(rc.top<=0)
		{
			rc.bottom-=rc.top;
			rc.top=0;
		}
		if(rc.right>(int)pHMap->Width())
		{
			int offset=rc.right-pHMap->Width();
			rc.right-=offset;
			rc.left-=offset;
		}
		if(rc.bottom>(int)pHMap->Height())
		{
			int offset=rc.bottom-pHMap->Height();
			rc.bottom-=offset;
			rc.top-=offset;
		}
	}

	MConstColor	TEditorPatch::s_color;
	TEditorPatch::TEditorPatch(void)
	{
		m_pVertStream=NULL;
		m_pVB=NULL;
		FZeroMem(m_mtls,sizeof(m_mtls));
		s_color.m_color=RenderSys::WhiteRenderColor;
		s_color.m_bUsingVertColor=false;
		m_cacheState=0;
	}

	TEditorPatch::~TEditorPatch(void)
	{
		FreeRenderBuffer();
		FreeMtls();
	}

	void TEditorPatch::FreeMtls()
	{
		for(int i=0;i<TerrainEditor::MAX_MAP_LAYER;i++)
		{
			SAFE_DELETE(m_mtls[i]);
		}
	}

	void TEditorPatch::FreeRenderBuffer()
	{
		SAFE_DELETE(m_pVertStream);
		SAFE_DELETE(m_pVB);
	}

	void TEditorPatch::CreateRenderBuffer(TerrainEditor *pEditor,Heightmap *pHMap,RECT rc)
	{
		ASSERT(pHMap!=NULL);

		//--释放现有
		FreeRenderBuffer();

		//--
		m_rect=rc;
		ClipHeightMapRect(pHMap,m_rect);

		m_pVertStream=new VertStream;
		m_pVertStream->Create(EVType_PNT,TerrainEditor::EPatchVerts);
		m_pVB=Device()->NewVB();
		m_pVB->Create(m_pVertStream->GetBufferSize(),EB_DynamicWriteOnly,GetVertSize(EVType_Trn4));
		UpdateVB(pEditor,pHMap);

		//保存layerUVScale
		for (int i=0;i<TRN_MAX_UV_SET;i++)
			m_layerUVScale[i]=pEditor->GetLayerUVScale(i);
	}

	void TEditorPatch::ClearLayers()
	{
		for(int i=0;i<TerrainEditor::MAX_MAP_LAYER;i++)
		{
			if(m_mtls[i]==NULL)
				continue;

			delete m_mtls[i];
			m_mtls[i]=NULL;
		}//endof for
	}

	void TEditorPatch::RemoveLayer(const TCHAR* szLayerName)
	{
		if(m_cacheState==0)
			return;

		for(int i=0;i<TerrainEditor::MAX_MAP_LAYER;i++)
		{
			if(m_mtls[i]==NULL)
				continue;
			if(_tcsicmp(m_mtls[i]->GetName(),szLayerName)==0)
			{
				m_mtls[i]->DeleteCacheFile();
				SAFE_DELETE(m_mtls[i]);
				ResMgr::Inst()->DoGC();//清空资源池中的cache
				break;
			}
		}//endof for
	}

	void TEditorPatch::AddLayer(const tagTMapLayer *pLayer,int layerIndex,BYTE initAlpha,const TCHAR* szCachePath,
		const Color4f& diffuse,const Color4f& ambient,const Color4f& specular)
	{
		if(m_cacheState==0 && layerIndex!=0)
			return;

		if(layerIndex<0 || layerIndex>=TerrainEditor::MAX_MAP_LAYER)
			return;
		if(m_mtls[layerIndex]!=NULL)
		{
			m_mtls[layerIndex]->Destroy();
			delete m_mtls[layerIndex];//删除原有的
			m_mtls[layerIndex]=NULL;
		}
		if(pLayer->szName[0]==0
			|| pLayer->szTexFile[0]==0)//没有数据??
			return;

		ASSERT(m_mtls[layerIndex]==NULL);
		m_mtls[layerIndex]=new TMtlEditor;
		if(layerIndex==0)
			m_mtls[layerIndex]->Create(pLayer->szTexFile,NULL,initAlpha,pLayer->szName,pLayer->uvSetIndex,
				diffuse,ambient,specular);
		else
		{
			tstring szAlphaMap=GetMtlCacheName(pLayer->szName,szCachePath);
			m_mtls[layerIndex]->Create(pLayer->szTexFile,szAlphaMap.c_str(),initAlpha,pLayer->szName,
				pLayer->uvSetIndex,diffuse,ambient,specular);
		}
	}

	void TEditorPatch::UpdateVB(TerrainEditor *pEditor,Heightmap *pHMap)
	{
		pHMap->FillVertStream_PNT(m_pVertStream,TerrainEditor::EPatchSideVerts,m_rect,true);
		m_pVB->CopyFromVertStream(m_pVertStream);
		m_pVertStream->BuildAABBox(m_worldBox);
	}

	void TEditorPatch::Draw()
	{
		IDevice *pDev=Device();

		int numVerts,numFace;
		IIndexBuffer* pIB=TPatchTriangulationDynamic::Inst()->GetIB_HW(0,0,0,0,0,numVerts,numFace);

		pDev->SetStreamDefine(Kernel::Inst()->GetRenderSys()->GetStreamDefine(EVType_Trn4));
		pDev->SetIB(pIB);
		pDev->SetVB(0,m_pVB);

		if(pDev->GetRenderState(ERS_FillMode)==EFill_Wireframe)
			DrawLayerPass(0,false,numFace);
		else
		{
			if(m_cacheState==2)
			{
				int drawCount=0;
				for(int i=0;i<TerrainEditor::MAX_MAP_LAYER;i++)
				{
					if(m_mtls[i]!=NULL)
					{
						DrawLayerPass(i,true,numFace);
						drawCount++;
					}
				}
				if(drawCount==0)//一个材质都没有,就画一层颜色吧
					DrawLayerPass(0,false,numFace);
			}
			else
				DrawLayerPass(0,true,numFace);
		}//endof else
	}

	void TEditorPatch::DrawLayerPass(int nLayer,bool useMtl,int numFace)
	{
		IDevice *pDev=Device();

		IMaterial *pMtl=NULL;
		if(useMtl && m_mtls[nLayer])
		{
			pMtl=m_mtls[nLayer]->GetMtl();
		}
		else
		{
			pMtl=&s_color;
		}
		
		int numPass=Kernel::Inst()->GetRenderSys()->PrepMtl(pMtl);
		MtlRenderCache *pMtlCache=pMtl->GetRenderCache();
		if(pMtlCache)
		{
			pMtlCache->Begin();
			for(int i=0;i<numPass;i++)
			{
				pMtlCache->BeginPass(i);
				pDev->DrawIndexed(EDraw_TriangleList,0,TerrainEditor::EPatchVerts,numFace);
				pMtlCache->EndPass();
			}
			pMtlCache->End();
		}
	}

	tstring TEditorPatch::GetMtlCacheName(TMtlEditor *pMtl,const TCHAR* szBasePath)
	{
		ASSERT(pMtl!=NULL);
		TCHAR szPath[512]={0};
		_stprintf(szPath,_T("%s_%s.layercache"),szBasePath,pMtl->GetName());
		return tstring(szPath);
	}

	tstring TEditorPatch::GetMtlCacheName(const TCHAR* szMtlName,const TCHAR* szBasePath)
	{		
		TCHAR szPath[512]={0};
		_stprintf(szPath,_T("%s_%s.layercache"),szBasePath,szMtlName);
		return tstring(szPath);
	}

	void TEditorPatch::LoadCache(TerrainEditor* pEditor,const TCHAR* szPath)
	{
		if(m_cacheState==0)
		{
			m_cacheState=1;
			for(UINT i=1;i<TerrainEditor::MAX_MAP_LAYER;i++)
			{
				tagTMapLayer *pLayer=(tagTMapLayer *)pEditor->GetMapLayer(i);
				AddLayer(pLayer,i,i==0?255:0,szPath,
					pEditor->GetMtlDiffuse(),pEditor->GetMtlAmbient(),pEditor->GetMtlSpecular());
			}
		}
		
		if(m_cacheState==1)
		{
			for(UINT i=1;i<TerrainEditor::MAX_MAP_LAYER;i++)
			{
				if(m_mtls[i]!=NULL
					&&!m_mtls[i]->GetMtl()->IsResourceReady())
					return;
			}
			m_cacheState=2;
		}
	}

	void TEditorPatch::ClearCache()
	{
		if(m_cacheState==0)
			return;

		for(int i=1;i<TerrainEditor::MAX_MAP_LAYER;i++)
		{
			SAFE_DELETE(m_mtls[i]);
		}

		m_cacheState=0;
	}

	void TEditorPatch::SaveToCacheFile()
	{
		if(m_cacheState!=2)
			return;

		//--cache主要是存每个图层的alpha map,每个alpha map存一个文件
		for(int i=0;i<TerrainEditor::MAX_MAP_LAYER;i++)
		{
			if(m_mtls[i]==NULL)
				continue;

			m_mtls[i]->SaveAlphaMapToFile();
		}
	}

	void TEditorPatch::BuildRenderGroups(const TCHAR* szSavepath,int id,TerrainEditor* pEditor)
	{
		vector<RENDER_GROUP> groupArray;

		//--生成渲染组
		map<string,RENDER_GROUP> groupMap;
		_BuildRenderGroups(groupMap);

		//--生成每个渲染组的alphamap
		_BuildRenderGroupsAlphaMap(groupMap,szSavepath,id);

		//--将所有渲染组按层数分组
		list<RENDER_GROUP> groupList[4];
		_OptimizeShaderSwitch(groupMap,groupList);

		//--将渲染组按纹理排序
		for(int i=0;i<4;i++)
			_OptimizeTextureSwitch(groupList[i],groupArray);

		//--写地形块渲染分组资源文件
		TCHAR szPath[512];
		_stprintf(szPath,_T("%sTrn_%03d.rendergroups"),szSavepath,id);
		TerrainPatchRenderGroupsRes::EditorSave(szPath,m_rect,groupArray,pEditor);

		//--debug
#if 0
		LOG(_T("groups:\n"));
		TCHAR szTmp[20];
		for(size_t i=0;i<groupArray.size();i++)
		{
			TerrainPatchRenderGroupsRes::tagRenderGroup& group=groupArray[i];
			tstring str;
			for(size_t j=0;j<group.layers.size();j++)
			{
				str+=_itot(group.layers[j],szTmp,10);
				str+=_T(",");
			}
			LOG(_T("%s\n"),str.c_str());
		}
#endif
	}

	void TEditorPatch::_BuildRenderGroups(map<string,RENDER_GROUP>& renderGroups)
	{
		const int GridSize=TerrainEditor::EPatchAlphaMapSize/TerrainEditor::EPatchSideGridsLod0;
		int gridIndex=0;
		char szTmp[20];
		for(int z=0;z<TerrainEditor::EPatchSideGridsLod0;z++)
		{
			for(int x=0;x<TerrainEditor::EPatchSideGridsLod0;x++)
			{
				string szKey;
				vector<int> layers;

				RECT rc;
				rc.left=x*GridSize;
				rc.right=rc.left+GridSize;
				rc.top=z*GridSize;
				rc.bottom=rc.top+GridSize;

				for(int i=0;i<TerrainEditor::MAX_MAP_LAYER;i++)
				{
					if(_IsLayerGridVisible(i,rc))
					{
						layers.push_back(i);
						szKey+=itoa(i,szTmp,10);
						szKey+=",";
						if(layers.size()>=4)
							break;
					}
				}

				map<string,RENDER_GROUP>::iterator iter=renderGroups.find(szKey);
				if(iter==renderGroups.end())
				{
					RENDER_GROUP newGroup;
					newGroup.grids.push_back(gridIndex);
					newGroup.layers=layers;
					renderGroups.insert(make_pair(szKey,newGroup));
				}
				else
				{
					iter->second.grids.push_back(gridIndex);
				}
				gridIndex++;
			}
		}
	}

	void TEditorPatch::_BuildRenderGroupsAlphaMap(map<string,RENDER_GROUP>& renderGroups,const TCHAR* szSavepath,int id)
	{
		int groupIndex=0;
		for(map<string,RENDER_GROUP>::iterator iter=renderGroups.begin();
			iter!=renderGroups.end();++iter)
		{
			RENDER_GROUP& renderGroup=iter->second;
			if(renderGroup.layers.size()>1)
			{
				Image alphaMap;
				alphaMap.Create(128,128,EPF_R8G8B8);

				for(int j=1;j<(int)renderGroup.layers.size();j++)
				{
					if(j>3)
						break;

					Image img;
					int layerIndex=renderGroup.layers[j];
					m_mtls[layerIndex]->CopyAlphaMap2Img(&img,alphaMap.Width(),alphaMap.Height());

					if(j==1)
						alphaMap.Blit(&img,'R');
					else if(j==2)
						alphaMap.Blit(&img,'G');
					else if(j==3)
						alphaMap.Blit(&img,'B');
				}

				TCHAR szAlphaMap[512];
				_sntprintf(szAlphaMap,512,_T("%sTrn_%03d_%d.tga"),szSavepath,id,groupIndex);
				alphaMap.VFlip();
				alphaMap.WriteToFile(szAlphaMap);

				renderGroup.szAlphaMap=szAlphaMap;
				groupIndex++;
			}
		}
	}

	void TEditorPatch::_OptimizeShaderSwitch(map<string,RENDER_GROUP>& src,list<RENDER_GROUP> dst[4])
	{
		for(map<string,RENDER_GROUP>::iterator iter=src.begin();
			iter!=src.end();iter++)
		{
			int numLayers=(int)iter->second.layers.size();
			if(numLayers>0)
			{
				ASSERT(numLayers<=4);
				dst[numLayers-1].push_back(iter->second);
			}
		}
	}

	void TEditorPatch::_OptimizeTextureSwitch(list<RENDER_GROUP>& src,vector<RENDER_GROUP>& dst)
	{
		if(src.size()==0)
			return;

		dst.push_back(src.front());
		src.pop_front();

		while(src.size()>0)
		{
			int maxSameTex=-1;
			list<RENDER_GROUP>::iterator iterSame=src.end();

			for(list<RENDER_GROUP>::iterator iter=src.begin();
				iter!=src.end();iter++)
			{
				int sameTex=_CompareRenderGroupByTex(dst.back(),*iter);
				if(sameTex>maxSameTex)
				{
					maxSameTex=sameTex;
					iterSame=iter;
				}
			}

			if(iterSame!=src.end())
			{
				dst.push_back(*iterSame);
				src.erase(iterSame);
			}		
		}
	}

	int TEditorPatch::_CompareRenderGroupByTex(const RENDER_GROUP& group1,const RENDER_GROUP& group2)
	{
		int sameLayers=0;
		int minSize=(int)min(group1.layers.size(),group2.layers.size());
		for(int i=0;i<minSize;i++)
		{
			if(group1.layers[i]==group2.layers[i])
				sameLayers++;
		}
		return sameLayers;
	}

	bool TEditorPatch::_IsLayerGridVisible(int layerIndex,RECT& rc)
	{
		for(int z=rc.top;z<rc.bottom;z++)
			for(int x=rc.left;x<rc.right;x++)
				if(_IsLayerPixelVisible(layerIndex,x,z))
					return true;
		return false;
	}

	bool TEditorPatch::_IsLayerPixelVisible(int layerIndex,int x,int z)
	{
		if(m_mtls[layerIndex]==NULL)
			return false;

		float finalAlpha=1.0f;
		if(layerIndex>0)
		{
			Heightmap* pHeightMap=m_mtls[layerIndex]->GetAlphaMap()->GetHeightmap();
			finalAlpha=pHeightMap->GetValue(x,z)/255.0f;
		}

		for(int i=layerIndex+1;i<TerrainEditor::MAX_MAP_LAYER;i++)
		{
			if(m_mtls[i]==NULL)
				continue;
			Heightmap* pHeightMap=m_mtls[i]->GetAlphaMap()->GetHeightmap();
			float alpha=pHeightMap->GetValue(x,z)/255.0f;
			finalAlpha*=(1-alpha);
		}

		return finalAlpha>0.1f;
	}

	bool TEditorPatch::BuildRunTimeData(TerrainEditor *pEditor,Heightmap *pHMap,
		const TCHAR* szSavePath,int id,IUICallBack* pUI)
	{
		if(m_cacheState!=2)
		{
			pUI->AddMsg(_T("地形cache未load完成."));
			return false;
		}

		TCHAR szCombineFile[512];
		_stprintf(szCombineFile,_T("%sTrn_%03d_combine.tga"),szSavePath,id);

		//--写地形资源文件
		TCHAR szPath[512];
		_stprintf(szPath,_T("%sterrain.trn"),szSavePath,id);
		TerrainRes::EditorSave(szPath,m_rect,pEditor);

		//--写地形块资源文件
		_stprintf(szPath,_T("%sTrn_%03d.patch"),szSavePath,id);
		TerrainPatchRes::EditorSave(szPath,m_rect,szCombineFile,pEditor);

		//--生成地形块渲染分组
		BuildRenderGroups(szSavePath,id,pEditor);

		//--写装饰层资源文件
		_stprintf(szPath,_T("%sGrass_%03d"),szSavePath,id);
		ResGrass::EditorSave(szPath,m_rect,pEditor);
		
		//--生成预混合贴图
		BuildPrecombine(szCombineFile);

		return true;
	}

	bool TEditorPatch::BuildLayerTypeMap( Heightmap* pLayerType,IUICallBack* pUI )
	{
		if(m_cacheState!=2)
		{
			pUI->AddMsg(_T("地形cache未load完成."));
			return false;
		}

		//--根据图层的浓度判断地表类型，相同浓度的以上层为准
		CalSurfaceType(pLayerType);

		return true;
	}

	void TEditorPatch::BuildPrecombine(const TCHAR* szFilename)
	{
		const UINT tarW=256;
		const UINT tarH=256;

		StateBlock sb;

		IDevice* pDev=Device();
		sb.SaveState(ERS_CullMode);			pDev->SetCullMode(ECull_None);
		sb.SaveState(ERS_DepthTestEnable);	pDev->SetRenderState(ERS_DepthTestEnable,false);
		sb.SaveState(ERS_DepthWriteEnable);	pDev->SetRenderState(ERS_DepthWriteEnable,false);
		sb.SaveState(ERS_FLightEnable);		pDev->FEnableLight(false);
		IRenderTexture* pRenderTex=pDev->NewRenderTexture();
		pRenderTex->Create(tarW,tarH,EPF_A8R8G8B8,false,false,1);

		IPixelShader* pPS1=pDev->NewPS();
		pPS1->CreateFromFile(_T("data\\system\\terrain\\shader\\precombine_1_gen.ps"),"ps_2_0","main");

		IPixelShader* pPS2=pDev->NewPS();
		pPS2->CreateFromFile(_T("data\\system\\terrain\\shader\\precombine_n_gen.ps"),"ps_2_0","main");

		RenderSys* pSys=Kernel::Inst()->GetRenderSys();
		pDev->SetStreamDefine(pSys->GetStreamDefine(EVType_PtT4));

		Vert_PtT4 vert[4];
		vert[0].pos=Vector4(-0.5f,		-0.5f,		1.0f,	1.0f);
		vert[1].pos=Vector4(tarW-0.5f,	-0.5f,		1.0f,	1.0f);
		vert[2].pos=Vector4(tarW-0.5f,	tarH-0.5f,	1.0f,	1.0f);
		vert[3].pos=Vector4(-0.5f,		tarH-0.5f,	1.0f,	1.0f);
		float uSize=TerrainEditor::EPatchSize/m_layerUVScale[0].x;
		float vSize=TerrainEditor::EPatchSize/m_layerUVScale[0].y;
		vert[0].uv=Vector2(0.0f,	0.0f);
		vert[1].uv=Vector2(uSize,	0.0f);
		vert[2].uv=Vector2(uSize,	vSize);
		vert[3].uv=Vector2(0.0f,	vSize);
		uSize=TerrainEditor::EPatchSize/m_layerUVScale[1].x;
		vSize=TerrainEditor::EPatchSize/m_layerUVScale[1].y;
		vert[0].uv2=Vector2(0.0f,	0.0f);
		vert[1].uv2=Vector2(uSize,	0.0f);
		vert[2].uv2=Vector2(uSize,	vSize);
		vert[3].uv2=Vector2(0.0f,	vSize);

		uSize=TerrainEditor::EPatchSize/m_layerUVScale[2].x;
		vSize=TerrainEditor::EPatchSize/m_layerUVScale[2].y;
		vert[0].uv3=Vector2(0.0f,	0.0f);
		vert[1].uv3=Vector2(uSize,	0.0f);
		vert[2].uv3=Vector2(uSize,	vSize);
		vert[3].uv3=Vector2(0.0f,	vSize);

		uSize=TerrainEditor::EPatchSize/m_layerUVScale[3].x;
		vSize=TerrainEditor::EPatchSize/m_layerUVScale[3].y;
		vert[0].uv4=Vector2(0.0f,	0.0f);
		vert[1].uv4=Vector2(uSize,	0.0f);
		vert[2].uv4=Vector2(uSize,	vSize);
		vert[3].uv4=Vector2(0.0f,	vSize);

		//--set render target as texture 0
		pDev->SetTexture(0,pRenderTex);
		pDev->FSetTexCoordSrc(0,TRN_MAX_UV_SET-1);
		pDev->SetSimplerFilter(0,ETFilter_POINT,ETFilter_POINT);
		pDev->SetSimplerMipFilter(0,ETFilter_NONE);
		pDev->SetSimplerAddress(0,ETAddress_Clamp,ETAddress_Clamp,ETAddress_Clamp);

		pRenderTex->Begin();
		pDev->Clear(true,false,false,0,1,0);
		for(int i=0;i<TerrainEditor::MAX_MAP_LAYER;i++)
		{
			if(m_mtls[i]!=NULL)
			{
				//--set pixel shader
				if(i==0)
					pDev->SetPixelShader(pPS1);
				else
					pDev->SetPixelShader(pPS2);

				TEditorMtl* pMtl=m_mtls[i]->GetMtl();
				ASSERT(pMtl->IsResourceReady());

				//--set tile texture as texture 1
				pDev->SetTexture(1,pMtl->m_pTileTex->m_pBitmap->GetDeviceTex());
				pDev->FSetTexCoordSrc(1,pMtl->m_uvSetIndex);
				pDev->SetSimplerFilter(1,ETFilter_LINEAR,ETFilter_LINEAR);
				pDev->SetSimplerMipFilter(1,ETFilter_LINEAR);
				pDev->SetSimplerAddress(1,ETAddress_Wrap,ETAddress_Wrap,ETAddress_Wrap);

				//--set alpha map as texture 2
				if(i!=0)
				{
					pDev->SetTexture(2,pMtl->m_pAlphaMap->GetDeviceTex());
					pDev->FSetTexCoordSrc(2,TRN_MAX_UV_SET-1);
					pDev->SetSimplerFilter(2,ETFilter_LINEAR,ETFilter_LINEAR);
					pDev->SetSimplerMipFilter(2,ETFilter_NONE);
					pDev->SetSimplerAddress(2,ETAddress_Clamp,ETAddress_Clamp,ETAddress_Clamp);
				}

				//--draw
				pDev->DrawMem(EDraw_TriangleFan,2,vert,sizeof(Vert_PtT4));
			}
		}
		pRenderTex->End();
		sb.Apply();
		pDev->SetPixelShader(NULL);

		pRenderTex->WriteToFile(szFilename);
		SAFE_DELETE(pRenderTex);

		SAFE_DELETE(pPS1);
		SAFE_DELETE(pPS2);
	}

	TMtlEditor *TEditorPatch::FindMtlByName(const TCHAR* szName)
	{
		if(szName==NULL || szName[0]==0)
			return NULL;
		for(int i=0;i<TerrainEditor::MAX_MAP_LAYER;i++)
		{
			if(m_mtls[i]==NULL)
				continue;
			if(_tcsicmp(szName,m_mtls[i]->GetName())==0)
			{
				return m_mtls[i];
			}
		}//endof for
		return NULL;
	}

	bool TEditorPatch::ConvLoaclMtlBrush(tagTEditBrush& brush)
	{
		int texlPerTile=TerrainEditor::EPatchAlphaMapSize/TerrainEditor::EPatchSize;
		
		//中心点坐标移动到本地格子空间
		POINT ltpt={brush.centerX-m_rect.left,brush.centerZ-m_rect.top};
		//中心点坐标缩放到贴图空间
		brush.centerX=ltpt.x*texlPerTile;
		brush.centerZ=ltpt.y*texlPerTile;
		
		//--内径外径大小缩放到贴图空间
		brush.inner=brush.inner*texlPerTile;
		brush.outer=brush.outer*texlPerTile;

		//--测试是否影响本patch的贴图
		RECT texRc;
		texRc.left=texRc.top=0;
		texRc.right=texRc.bottom=TerrainEditor::EPatchAlphaMapSize;
		RECT brushRc;
		brushRc.left=brush.centerX-brush.outer;
		brushRc.right=brush.centerX+brush.outer;
		brushRc.top=brush.centerZ-brush.outer;
		brushRc.bottom=brush.centerZ+brush.outer;
		RECT tmp;
		return ::IntersectRect(&tmp,&texRc,&brushRc)!=0;//win32,GDI函数
	}

	void TEditorPatch::BrushLayerMtl(const TCHAR* szLayerName,const tagTEditBrush& brush)
	{
		if(m_cacheState!=2)
			return;

		TMtlEditor *pMtl=FindMtlByName(szLayerName);
		
		if(pMtl!=NULL)
		{
			tagTEditBrush localBrush=brush;
			if(ConvLoaclMtlBrush(localBrush))
				if(CheckLayers(szLayerName,localBrush))
					pMtl->BrushAlpha(localBrush);
		}
	}

	void TEditorPatch::SmoothLayerMtl(const TCHAR* szLayerName,const tagTEditBrush& brush)
	{
		if(m_cacheState!=2)
			return;

		TMtlEditor *pMtl=FindMtlByName(szLayerName);
		
		if(pMtl!=NULL)
		{
			tagTEditBrush localBrush=brush;
			if(ConvLoaclMtlBrush(localBrush))
				pMtl->SmoothAlpha(localBrush);
		}
	}

	void TEditorPatch::NoiseLayerMtl(const TCHAR* szLayerName,const tagTEditBrush& brush,float noiseFreq)
	{
		if(m_cacheState!=2)
			return;

		TMtlEditor *pMtl=FindMtlByName(szLayerName);
		
		if(pMtl!=NULL)
		{
			tagTEditBrush localBrush=brush;
			if(ConvLoaclMtlBrush(localBrush))
				pMtl->NoiseAlpha(localBrush,noiseFreq);
		}
	}

	void TEditorPatch::MoveLayer(const TCHAR* szLayerName,int offset)
	{
		if(m_cacheState==0)
			return;

		//--找到需要移动的图层的index
		int find=-1;
		for(int i=0;i<TerrainEditor::MAX_MAP_LAYER;i++)
		{
			if(m_mtls[i]==NULL)
				continue;
			if(_tcsicmp(szLayerName,m_mtls[i]->GetName())==0)
			{
				find=i;
				break;
			}
		}//endof for
		if(find==-1)//没有找到
			return;
		if(find==0)//base不能移动
			return;

		//--交换一下指针
		int other=find+offset;
		if(other<=0)//等于0也不行,因为base layer不允许移动
			return;
		if(other>=TerrainEditor::MAX_MAP_LAYER)
			return;
		TMtlEditor *pMtl=m_mtls[find];
		m_mtls[find]=m_mtls[other];
		m_mtls[other]=pMtl;
	}

	void TEditorPatch::GetMapRectNoOverlap( RECT& rect )
	{
		rect=m_rect;
		rect.right=rect.left+TerrainEditor::EPatchSize;
		rect.bottom=rect.top+TerrainEditor::EPatchSize;
	}

	void TEditorPatch::GetLayers(POINT ptMap,vector<int>& layers)
	{
		if(m_cacheState!=2)
			return;

		RECT rcNoOverlap;
		GetMapRectNoOverlap(rcNoOverlap);
		if(!PtInRect(&rcNoOverlap,ptMap))
			return;

		//--转换成本patch局部空间
		int lx=ptMap.x-rcNoOverlap.left;
		int ly=ptMap.y-rcNoOverlap.top;

		//--计算所在grid的rect
		RECT rc;
		const int GridSize=TerrainEditor::EPatchSize/TerrainEditor::EPatchSideGridsLod0;
		rc.left=lx/GridSize*GridSize;
		rc.right=rc.left+GridSize;
		rc.top=ly/GridSize*GridSize;
		rc.bottom=rc.top+GridSize;

		//--rect转换到贴图空间
		const int TexelPerTile=TerrainEditor::EPatchAlphaMapSize/TerrainEditor::EPatchSize;
		rc.left*=TexelPerTile;
		rc.right*=TexelPerTile;
		rc.top*=TexelPerTile;
		rc.bottom*=TexelPerTile;

		//--
		layers.clear();
		layers.push_back(0);//底层

		//--统计层数
		float minH,maxH;
		for(int i=1;i<TerrainEditor::MAX_MAP_LAYER;i++)
		{
			if(m_mtls[i]==NULL)
				continue;

			Heightmap* pHMap=m_mtls[i]->GetAlphaMap()->GetHeightmap();
			pHMap->GetHeightRange(minH,maxH,rc);

			if(minH>=255.0f)//全不透明的层，算作底层
			{
				layers.clear();
			}
			if(maxH>0)
			{
				layers.push_back(i);
			}
		}	
	}

	bool TEditorPatch::CheckLayers(const TCHAR* szLayerName,const tagTEditBrush& localBrush)
	{
		if(m_cacheState!=2)
			return false;

		if(localBrush.strength<0)//擦除操作不用检查层数
			return true;

		//--贴图空间的rect
		RECT rc;
		rc.left=localBrush.centerX-localBrush.outer;
		rc.right=localBrush.centerX+localBrush.outer;
		rc.top=localBrush.centerZ-localBrush.outer;
		rc.bottom=localBrush.centerZ+localBrush.outer;

		//--clamp
		if(rc.left<0) 
			rc.left=0;
		if(rc.right>=TerrainEditor::EPatchAlphaMapSize) 
			rc.right=TerrainEditor::EPatchAlphaMapSize-1;
		if(rc.top<0)
			rc.top=0;
		if(rc.bottom>=TerrainEditor::EPatchAlphaMapSize) 
			rc.bottom=TerrainEditor::EPatchAlphaMapSize-1;

		//--与grid对齐
		const int TexelPerGrid=TerrainEditor::EPatchSize/TerrainEditor::EPatchSideGridsLod0;
		rc.left=rc.left/TexelPerGrid*TexelPerGrid;
		rc.right=rc.right/TexelPerGrid*TexelPerGrid+TexelPerGrid;
		rc.top=rc.top/TexelPerGrid*TexelPerGrid;
		rc.bottom=rc.bottom/TexelPerGrid*TexelPerGrid+TexelPerGrid;

		//--预测图层数量
		int numLayers=1;
		float minH,maxH;
		bool bExist=false;
		for(int i=1;i<TerrainEditor::MAX_MAP_LAYER;i++)
		{
			if(m_mtls[i]==NULL)
				continue;

			 Heightmap* pHMap=m_mtls[i]->GetAlphaMap()->GetHeightmap();
			 pHMap->GetHeightRange(minH,maxH,rc);

			 if(maxH>0
				 &&_tcsicmp(szLayerName,m_mtls[i]->GetName())==0)
				bExist=true;

			 if(minH>=255.0f)//全不透明的层，算作底层
				 numLayers=1;
			 else if(maxH>0)
				 numLayers++;
		}	

		if(!bExist)
			numLayers++;

		return numLayers<=4;
	}

	void TEditorPatch::CalSurfaceType( Heightmap *pTypeMap )
	{
		ASSERT(pTypeMap!=NULL);

		//--判断每个小格子地表类型
		for(UINT y=0;y<pTypeMap->Height();y++)
		{
			for(UINT x=0;x<pTypeMap->Width();x++)
			{
				int maxH=0;
				int mainLayer=0;
				for(int i=1;i<TerrainEditor::MAX_MAP_LAYER;i++)
				{
					if(m_mtls[i]==NULL)
						continue;

					Heightmap* pHMap=m_mtls[i]->GetAlphaMap()->GetHeightmap();
					int xMul=pHMap->Width()/(pTypeMap->Width()-1);
					int yMul=pHMap->Height()/(pTypeMap->Height()-1);

					int val=pHMap->GetSafeVal(x*xMul,y*yMul);

					if(val>=maxH)
					{
						mainLayer=i;
						maxH=val;
					}
				}
				if( maxH<128 )
					mainLayer=0;
				pTypeMap->SafeSetVal(x,y,mainLayer);
			}
		}
	}

	void TEditorPatch::SetMtlDiffuse( const Color4f& color )
	{
		for(int i=0;i<TerrainEditor::MAX_MAP_LAYER;i++)
		{
			if(m_mtls[i]!=NULL)
			{
				TEditorMtl* pMtl=m_mtls[i]->GetMtl();
				if(pMtl!=NULL)
				{
					pMtl->m_color.m_color.diffuse=color;
					pMtl->UpdateRenderCache();
				}
			}
		}
	}

	void TEditorPatch::SetMtlAmbient( const Color4f& color )
	{
		for(int i=0;i<TerrainEditor::MAX_MAP_LAYER;i++)
		{
			if(m_mtls[i]!=NULL)
			{
				TEditorMtl* pMtl=m_mtls[i]->GetMtl();
				if(pMtl!=NULL)
				{
					pMtl->m_color.m_color.ambient=color;
					pMtl->UpdateRenderCache();
				}
			}
		}
	}

	void TEditorPatch::SetMtlSpecular( const Color4f& color )
	{
		for(int i=0;i<TerrainEditor::MAX_MAP_LAYER;i++)
		{
			if(m_mtls[i]!=NULL)
			{
				TEditorMtl* pMtl=m_mtls[i]->GetMtl();
				if(pMtl!=NULL)
				{
					pMtl->m_color.m_color.specular=color;
					pMtl->UpdateRenderCache();
				}
			}
		}
	}

	void TEditorPatch::SetMtlPower( float power )
	{
		for(int i=0;i<TerrainEditor::MAX_MAP_LAYER;i++)
		{
			if(m_mtls[i]!=NULL)
			{
				TEditorMtl* pMtl=m_mtls[i]->GetMtl();
				if(pMtl!=NULL)
				{
					pMtl->m_color.m_color.power=power;
					pMtl->UpdateRenderCache();
				}
			}
		}
	}
}//namespace Cool3D