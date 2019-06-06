#include "StdAfx.h"
#include ".\teditorpatchproxy.h"
#include "..\Util\FMemory.h"
#include "..\Util\Exception.h"
#include "TerrainEditor.h"
#include "..\Image\Image.h"
#include "..\Image\ImageTGA.h"
#include "TMapLayerExt.h"
#include "TEditorPatch.h"
#include "..\Math\Frustum.h"
#include ".\IUICallBack.h"
#include "..\Terrain\Heightmap.h"
#include "..\Terrain\TPatchTriangulationDynamic.h"
#include "..\Terrain\ResGrass.h"

namespace Cool3D
{
	tstring TEditorPatchProxy::s_cachePath=_T("");

	void TEditorPatchProxy::SetCachePath(const TCHAR* szPath)
	{
		s_cachePath=szPath;
	}

	TEditorPatchProxy::TEditorPatchProxy(void)
	{
		FZeroMem(&m_mapRect,sizeof(m_mapRect));
		m_pRealPatch=NULL;
		m_id=0;
	}

	TEditorPatchProxy::~TEditorPatchProxy(void)
	{
		SAFE_DELETE(m_pRealPatch);
	}

	void TEditorPatchProxy::Create(RECT rc,int id)
	{
		m_mapRect=rc;
		m_center.x=(rc.left+rc.right)/2;
		m_center.y=(rc.top+rc.bottom)/2;
		m_id=id;
	}

	void TEditorPatchProxy::Update(TerrainEditor *pEditor,Heightmap *pHMap,
		const RECT& drawRect,const RECT& editRect)
	{
		BOOL inView=::PtInRect(&drawRect, m_center);
		BOOL inEdit=::PtInRect(&editRect, m_center);

		//--
		if(inView)
		{
			if(m_pRealPatch==NULL)
			{
				//创建内部数据
				GetRealPatch(pEditor,pHMap);
			}

			if(inEdit && !m_pRealPatch->IsCacheLoaded())
			{
				TCHAR szCachePath[512];
				_stprintf(szCachePath,_T("%s\\TPatch_%03d"),s_cachePath.c_str(),m_id);

				m_pRealPatch->LoadCache(pEditor,szCachePath);
			}

			if(!inEdit && m_pRealPatch->IsCacheLoaded())
			{
				m_pRealPatch->SaveToCacheFile();
				m_pRealPatch->ClearCache();
			}
		}//endof if
		else
		{
			SaveCache();
			SAFE_DELETE(m_pRealPatch);
		}
	}

	void TEditorPatchProxy::SaveCache()
	{
		if(m_pRealPatch==NULL)
			return;
		m_pRealPatch->SaveToCacheFile();
	}

	bool TEditorPatchProxy::BuildRunTimeData(TerrainEditor *pEditor,Heightmap *pHMap,const TCHAR* szBasepath,IUICallBack* pUI)
	{
		bool bRet=false;

		if(m_pRealPatch!=NULL)
		{
			if(m_pRealPatch->IsCacheLoaded())
			{
				bRet=m_pRealPatch->BuildRunTimeData(pEditor,pHMap,szBasepath,m_id,pUI);
			}
			else
			{
				TCHAR szCachePath[512];
				_stprintf(szCachePath,_T("%s\\TPatch_%03d"),s_cachePath.c_str(),m_id);

				m_pRealPatch->LoadCache(pEditor,szCachePath);
				bRet=m_pRealPatch->BuildRunTimeData(pEditor,pHMap,szBasepath,m_id,pUI);
				m_pRealPatch->ClearCache();
			}
		}
		else
		{
			GetRealPatch(pEditor,pHMap);

			TCHAR szCachePath[512];
			_stprintf(szCachePath,_T("%s\\TPatch_%03d"),s_cachePath.c_str(),m_id);
			m_pRealPatch->LoadCache(pEditor,szCachePath);

			bRet=m_pRealPatch->BuildRunTimeData(pEditor,pHMap,szBasepath,m_id,pUI);
			SAFE_DELETE(m_pRealPatch);
		}

		return bRet;
	}

	void TEditorPatchProxy::GetRealPatch(TerrainEditor *pEditor,Heightmap *pHMap)
	{
		if(m_pRealPatch!=NULL)
			return;

		m_pRealPatch=new TEditorPatch;
		m_pRealPatch->CreateRenderBuffer(pEditor,pHMap,m_mapRect);
		m_pRealPatch->AddLayer(pEditor->GetMapLayer(0),0,255,NULL,
			pEditor->GetMtlDiffuse(),pEditor->GetMtlAmbient(),pEditor->GetMtlSpecular());
	}

	void TEditorPatchProxy::AddLayerMtl(TerrainEditor *pEditor,int layerIndex,BYTE initAlpha)
	{
		if(m_pRealPatch)
		{
			if(layerIndex==0)
				m_pRealPatch->AddLayer(pEditor->GetMapLayer(layerIndex),0,255,NULL,
					pEditor->GetMtlDiffuse(),pEditor->GetMtlAmbient(),pEditor->GetMtlSpecular());
			else
			{
				TCHAR szCachePath[512];
				_stprintf(szCachePath,_T("%s\\TPatch_%03d"),s_cachePath.c_str(),m_id);
				m_pRealPatch->AddLayer(pEditor->GetMapLayer(layerIndex),layerIndex,initAlpha,szCachePath,
					pEditor->GetMtlDiffuse(),pEditor->GetMtlAmbient(),pEditor->GetMtlSpecular());
			}
		}
	}

	void TEditorPatchProxy::ClearLayers()
	{
		if(m_pRealPatch==NULL)
			return;
		else
			m_pRealPatch->ClearLayers();
	}

	void TEditorPatchProxy::RemoveLayer(const TCHAR* szLayerName)
	{
		if(m_pRealPatch==NULL)
			return;
		else
			m_pRealPatch->RemoveLayer(szLayerName);
	}

	void TEditorPatchProxy::MoveLayer(const TCHAR* szLayerName,int offset)
	{
		if(m_pRealPatch==NULL)
			return;
		else
			m_pRealPatch->MoveLayer(szLayerName,offset);
	}
	
	void TEditorPatchProxy::Draw(const Frustum& frustum)
	{
		if(m_pRealPatch!=NULL && frustum.BoxInFrustum(m_pRealPatch->GetWorldBox()))
			m_pRealPatch->Draw();
	}

	void TEditorPatchProxy::BrushLayerMtl(const TCHAR* szLayerName,const tagTEditBrush& brush)
	{
		if(m_pRealPatch==NULL)
			return;
		else
			m_pRealPatch->BrushLayerMtl(szLayerName,brush);
	}

	void TEditorPatchProxy::SmoothLayerMtl(const TCHAR* szLayerName,const tagTEditBrush& brush)
	{
		if(m_pRealPatch==NULL)
			return;
		else
			m_pRealPatch->SmoothLayerMtl(szLayerName,brush);
	}

	void TEditorPatchProxy::NoiseLayerMtl(const TCHAR* szLayerName,const tagTEditBrush& brush,float noiseFreq)
	{
		if(m_pRealPatch==NULL)
			return;
		else
			m_pRealPatch->NoiseLayerMtl(szLayerName,brush,noiseFreq);
	}

	void TEditorPatchProxy::DrawPatchEdge(TerrainEditor *pEditor)
	{
		if(m_pRealPatch==NULL
			||!m_pRealPatch->IsCacheLoaded())
			return;
		pEditor->DrawGroundQuad(m_mapRect.left,m_mapRect.right,m_mapRect.top,m_mapRect.bottom,0xFF00FF00);
	}

	void TEditorPatchProxy::DrawSubPatchEdge(TerrainEditor *pEditor)
	{
		if(m_pRealPatch==NULL
			||!m_pRealPatch->IsCacheLoaded())
			return;
		int gridSize=TerrainEditor::EPatchSize/TerrainEditor::EPatchSideGridsLod0;
		for(int y=0;y<TerrainEditor::EPatchSideGridsLod0;y++)
		{
			for(int x=0;x<TerrainEditor::EPatchSideGridsLod0;x++)
			{
				RECT subRect;
				subRect.left=m_mapRect.left+x*gridSize;
				subRect.top=m_mapRect.top+y*gridSize;
				subRect.right=min(subRect.left+gridSize,m_mapRect.right);
				subRect.bottom=min(subRect.top+gridSize,m_mapRect.bottom);
				pEditor->DrawGroundQuad(subRect.left,subRect.right,subRect.top,subRect.bottom,0xFF00FF00);
			}
		}
	}

	void TEditorPatchProxy::GetLayers(POINT ptMap,TerrainEditor *pEditor,Heightmap *pHMap,vector<int>& layers)
	{
		if(m_pRealPatch!=NULL)
			m_pRealPatch->GetLayers(ptMap,layers);
	}

	void TEditorPatchProxy::UpdateVB(TerrainEditor* pEditor,Heightmap* pHMap,const RECT* pRc)
	{
		if(m_pRealPatch==NULL)
			return;
		RECT tmp;
		if(pRc==NULL || ::IntersectRect(&tmp,pRc,&m_mapRect)==TRUE)
			m_pRealPatch->UpdateVB(pEditor,pHMap);
	}

	bool TEditorPatchProxy::ApplyLightMapToCombineImage(const TCHAR* szLightMapPath,const TCHAR* szTrnPath)
	{
		TCHAR szLightMap[512];
		_sntprintf(szLightMap,512,_T("%s\\trn_%03d.tga"),szLightMapPath,m_id);
		TCHAR szCombineImg[512];
		_sntprintf(szCombineImg,512,_T("%s\\trn_%03d_combine.tga"),szTrnPath,m_id);

		Image lightMap;
		if(!lightMap.LoadFromFile(szLightMap))
			return false;
		lightMap.VFlip();

		Image combineImg;
		if(!combineImg.LoadFromFile(szCombineImg))
			return false;

		if(lightMap.Width()!=combineImg.Width()
			||lightMap.Height()!=combineImg.Height())
			lightMap.Scale(combineImg.Width(),combineImg.Height(),EIS_BestQuality);
		if(!combineImg.Mul(&lightMap))
			return false;

		if(!combineImg.WriteToFile(szCombineImg))
			return false;

		return true;
	}

	bool TEditorPatchProxy::ApplyLightMapToDecoLayer( const TCHAR* szLightMapPath,const TCHAR* szTrnPath,TerrainEditor* pEditor)
	{
		//--加载lightmap
		TCHAR szLightMap[512];
		_sntprintf(szLightMap,512,_T("%s\\trn_%03d.tga"),szLightMapPath,m_id);

		Image lightMap;
		if(!lightMap.LoadFromFile(szLightMap))
			return false;
		lightMap.VFlip();

		//--搜索装饰层文件
		TCHAR szSearch[512];
		_sntprintf(szSearch,512,_T("%s\\grass_%03d_*.deco"),szTrnPath,m_id);

		TCHAR szDecoFile[512];

		WIN32_FIND_DATA findFileData;
		HANDLE hFind= FindFirstFile(szSearch, &findFileData);
		if (hFind != INVALID_HANDLE_VALUE) 
		{
			do
			{
				_sntprintf(szDecoFile,512,_T("%s\\%s"),szTrnPath,findFileData.cFileName);
				if(!ResGrass::ApplyLightMap(&lightMap,szDecoFile,m_mapRect,pEditor))
				{
					FindClose(hFind);
					return false;
				}
			}
			while (FindNextFile(hFind, &findFileData) != 0);

			FindClose(hFind);
		}

		return true;
	}

	void TEditorPatchProxy::GetMapRectNoOverlap( RECT& rect )
	{
		rect=m_mapRect;
		rect.right=min(rect.left+TerrainEditor::EPatchSize,m_mapRect.right);
		rect.bottom=min(rect.top+TerrainEditor::EPatchSize,m_mapRect.bottom);
	}

	void TEditorPatchProxy::BuildNavHeightmap( Heightmap* pHMap, Heightmap& nav )
	{
		VertStream vertStream;
		vertStream.Create(EVType_PN,TerrainEditor::EPatchVerts);
		pHMap->FillVertStream_PN(&vertStream,TerrainEditor::EPatchSideVerts,m_mapRect);

		RECT rc;
		GetMapRectNoOverlap(rc);

		for(int z=rc.top;z<rc.bottom;z++)
		{
			for(int x=rc.left;x<rc.right;x++)
			{
				Vector3 pos;
				pos.x=x*pHMap->GetXScale();
				pos.z=z*pHMap->GetZScale();
				if(GetMeshHeight(vertStream,pos))
					nav.SetValue(x,z,(int)pos.y);
				else
					nav.SetValue(x,z,(int)pHMap->GetHeight(x,z));
			}
		}
	}

	bool TEditorPatchProxy::BuildLayerTypeMap( TerrainEditor *pEditor,Heightmap *pHMap,Heightmap* pLayerType,IUICallBack* pUI )
	{
		bool bRet=false;

		if(m_pRealPatch!=NULL)
		{
			if(m_pRealPatch->IsCacheLoaded())
			{
				bRet=m_pRealPatch->BuildLayerTypeMap(pLayerType,pUI);
			}
			else
			{
				TCHAR szCachePath[512];
				_stprintf(szCachePath,_T("%s\\TPatch_%03d"),s_cachePath.c_str(),m_id);

				m_pRealPatch->LoadCache(pEditor,szCachePath);
				bRet=m_pRealPatch->BuildLayerTypeMap(pLayerType,pUI);
				m_pRealPatch->ClearCache();
			}
		}
		else
		{
			GetRealPatch(pEditor,pHMap);

			TCHAR szCachePath[512];
			_stprintf(szCachePath,_T("%s\\TPatch_%03d"),s_cachePath.c_str(),m_id);
			m_pRealPatch->LoadCache(pEditor,szCachePath);

			bRet=m_pRealPatch->BuildLayerTypeMap(pLayerType,pUI);
			SAFE_DELETE(m_pRealPatch);
		}

		return bRet;
	}

	bool TEditorPatchProxy::GetMeshHeight( VertStream& vertStream, Vector3& pos )
	{
		const vector<WORD> indices=TPatchTriangulationDynamic::Inst()->GetIB(0,0,0,0,0);
		ASSERT(indices.size()%3==0);

		Ray ray;
		ray.origin=pos;
		ray.origin.y=1000000.0f;
		ray.dir=Vector3(0,-1,0);

		Vert_PN* pVert=(Vert_PN*)vertStream.GetRawBuffer();
		for(size_t i=0;i<indices.size();i+=3)
		{
			int i1=indices[i];
			int i2=indices[i+1];
			int i3=indices[i+2];
			Vector3 p1=pVert[i1].pos;
			Vector3 p2=pVert[i2].pos;
			Vector3 p3=pVert[i3].pos;
			TResult tr=ray.IntersectTriangle(p1,p2,p3);
			if(tr.first)
			{
				pos.y=ray.origin.y-tr.second;
				return true;
			}
		}

		return false;
	}

	void TEditorPatchProxy::SetMtlDiffuse(const Color4f& color)
	{
		if(m_pRealPatch!=NULL)
			m_pRealPatch->SetMtlDiffuse(color);
	}

	void TEditorPatchProxy::SetMtlAmbient( const Color4f& color )
	{
		if(m_pRealPatch!=NULL)
			m_pRealPatch->SetMtlAmbient(color);
	}

	void TEditorPatchProxy::SetMtlSpecular( const Color4f& color )
	{
		if(m_pRealPatch!=NULL)
			m_pRealPatch->SetMtlSpecular(color);
	}

	void TEditorPatchProxy::SetMtlPower( float power )
	{
		if(m_pRealPatch!=NULL)
			m_pRealPatch->SetMtlPower(power);
	}

}//namespace Cool3D