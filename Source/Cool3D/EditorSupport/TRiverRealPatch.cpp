#include "stdafx.h"
#include ".\TRiverRealPatch.h"
#include ".\TRiverEditorMtl.h"
#include "..\Util\FMemory.h"
#include ".\TRiverEditor.h"
#include "..\ResSys\VertStream.h"
#include "..\Device\IDevice.h"
#include "..\RenderMtl\MConstColor.h"
#include "..\Terrain\TRiverPatchTriangulationDynamic.h"
#include "..\Terrain\Heightmap.h"
#include "..\RenderSys\RenderSys.h"
#include "..\RenderSys\MtlRenderCache.h"
#include "..\Terrain\ResRiverPatch.h"
#include ".\ResDynaAlphaMap.h"
#include "..\Math\math.h"
#include "..\Image\Image.h"
#include "..\Terrain\BitMap.h"

namespace Cool3D
{
	//--使得Rect落在map的尺寸允许的范围内,并且不改变Rect的尺寸
	void ClipMapRect(Heightmap *pMap,RECT &rc)
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
		if(rc.right>(int)pMap->Width())
		{
			int offset=rc.right-pMap->Width();
			rc.right-=offset;
			rc.left-=offset;
		}
		if(rc.bottom>(int)pMap->Height())
		{
			int offset=rc.bottom-pMap->Height();
			rc.bottom-=offset;
			rc.top-=offset;
		}
	}

	class TRiverRealPatch::Member
	{
	public:
		RECT				m_rect;
		VertStream			*m_pVS;				//从map指定的rect创建的vert stream
		IVertBuffer			*m_pVB;				//渲染用的Vert buffer
		TRiverEditorMtl		*m_pMtl;
		ELoadState			m_loadState;
		AABBox				m_box;
		int					m_height;			//河流的高度
		UINT				m_riverID;			//河流索引

		MConstColor			m_color;

	public:
		Member(UINT riverID,int height)
			:m_riverID(riverID)
			,m_height(height)
		{
			m_pVS=NULL;
			m_pVB=NULL;
			m_pMtl=NULL;
			m_loadState=LS_NOTLOAD;
			m_color.m_color=RenderSys::WhiteRenderColor;
			m_color.m_bUsingVertColor=false;
		}

		~Member()
		{
			SAFE_DELETE(m_pMtl);
			FreeBuffer();
		}

		void FreeBuffer()
		{
			SAFE_DELETE(m_pVS);
			SAFE_DELETE(m_pVB);
		}
	};

	TRiverRealPatch::TRiverRealPatch( UINT riverID,int height )
	{
		m_p=new Member(riverID,height);
	}

	TRiverRealPatch::~TRiverRealPatch()
	{
		SAFE_DELETE(m_p);
	}

	bool TRiverRealPatch::CreateRenderBuffer( TRiverEditor *pEditor,Heightmap *pHMap,const RECT *pRect )
	{
		ASSERT(pEditor!=NULL);
		ASSERT(pHMap!=NULL);
		ASSERT(pRect!=NULL);

		//--释放已存在的buffer
		m_p->FreeBuffer();

		m_p->m_rect=*pRect;
		ClipMapRect(pHMap,m_p->m_rect);

		m_p->m_pVS=new VertStream;
		m_p->m_pVS->Create(EVType_PNT,TRiverEditor::EPatchVerts);
		m_p->m_pVB=Device()->NewVB();
		m_p->m_pVB->Create(m_p->m_pVS->GetBufferSize(),EB_DynamicWriteOnly,GetVertSize(EVType_PNT));
		UpdateVB(pHMap);

		return true;
	}

	bool TRiverRealPatch::CreateMtl( const TCHAR *szTexFile,const TCHAR *szMapFile,BYTE initVal,float alpha )
	{
		if(szTexFile==NULL || szTexFile[0]==0)
			return false;
		if(szMapFile==NULL || szMapFile[0]==0)
			return false;

		if(m_p->m_pMtl!=NULL)
			SAFE_DELETE(m_p->m_pMtl);

		m_p->m_pMtl=new TRiverEditorMtl();
		return m_p->m_pMtl->Create(szTexFile,szMapFile,initVal,alpha);
	}

	void TRiverRealPatch::UpdateVB( Heightmap *pHMap )
	{
		FillVertStream(pHMap);
		m_p->m_pVB->CopyFromVertStream(m_p->m_pVS);
		m_p->m_pVS->BuildAABBox(m_p->m_box);
	}

	void TRiverRealPatch::FillVertStream( Heightmap *pHMap )
	{
		RECT rect=m_p->m_rect;

		int stepX=(rect.right-rect.left)/(TRiverEditor::EPatchSideVerts-1);
		int stepZ=(rect.bottom-rect.top)/(TRiverEditor::EPatchSideVerts-1);

		Vert_PNT *pVert=(Vert_PNT *)m_p->m_pVS->GetRawBuffer();

		float rectW=float(rect.right-rect.left-1);
		float rectH=float(rect.bottom-rect.top-1);
		int sideVerts=TRiverEditor::EPatchSideVerts;

		for(int z=0;z<sideVerts;z++)
		{
			for(int x=0;x<sideVerts;x++)
			{
				int hx,hz;
				if(x==sideVerts-1)
					hx=rect.right-1;
				else
					hx=rect.left+x*stepX;	
				if(z==sideVerts-1)
					hz=rect.bottom-1;
				else
					hz=rect.top+z*stepZ;

				pHMap->GetVertPos(hx,hz,pVert->pos);
				pVert->normal=Vector3(0.0f,1.0f,0.0f);

				pVert->uv.x=float(hx-rect.left)/rectW;
				pVert->uv.y=float(hz-rect.top)/rectH;

				//--
				pVert++;
			}
		}
	}

	void TRiverRealPatch::Draw()
	{
		IDevice *pDev=Device();

		int numVerts,numFace;
		IIndexBuffer* pIB=TRiverPatchTriangulationDynamic::Inst()->GetIB_HW(0,numVerts,numFace);

		pDev->SetStreamDefine(Kernel::Inst()->GetRenderSys()->GetStreamDefine(EVType_PNT));
		pDev->SetIB(pIB);
		pDev->SetVB(0,m_p->m_pVB);

		if(pDev->GetRenderState(ERS_FillMode)==EFill_Wireframe)
			DrawPass(false,numFace);
		else
		{
			if(m_p->m_loadState==LS_LOADED)
			{
				if(m_p->m_pMtl!=NULL)
					DrawPass(true,numFace);
				else
					DrawPass(false,numFace);
			}
			else
				DrawPass(true,numFace);
		}
	}

	void TRiverRealPatch::DrawPass( bool useMtl,int numFace )
	{
		IDevice *pDev=Device();

		IMaterial *pMtl=NULL;
		if(useMtl 
			&& m_p->m_pMtl!=NULL)
			pMtl=m_p->m_pMtl;
		else
			pMtl=&m_p->m_color;

		int numPass=Kernel::Inst()->GetRenderSys()->PrepMtl(pMtl);
		MtlRenderCache *pMtlCache=pMtl->GetRenderCache();
		if(pMtlCache)
		{
			pMtlCache->Begin();
			for(int i=0;i<numPass;i++)
			{
				pMtlCache->BeginPass(i);
				pDev->DrawIndexed(EDraw_TriangleList,0,TRiverEditor::EPatchVerts,numFace);
				pMtlCache->EndPass();
			}
			pMtlCache->End();
		}
	}

	bool TRiverRealPatch::SaveToFile() const
	{
		if(m_p->m_loadState!=LS_LOADED)
			return false;

		m_p->m_pMtl->SaveMapToFile();

		return true;
	}

	void TRiverRealPatch::LoadFromFile( TRiverEditor *pEditor,const TCHAR *szFileName )
	{
		if(m_p->m_loadState==LS_NOTLOAD)
		{
			tagRiverMtlDesc *pMtlDesc=pEditor->GetRiverMtlDescByID(m_p->m_riverID);
			if( CreateMtl(pMtlDesc->szWaterTex,szFileName,0,pMtlDesc->alpha) )
				m_p->m_loadState=LS_LOADING;
		}

		if (m_p->m_loadState==LS_LOADING)
		{
			if(m_p->m_pMtl!=NULL
				&& !m_p->m_pMtl->IsResourceReady())
				return;
			m_p->m_loadState=LS_LOADED;
		}

		ApplyMtl(pEditor);
	}

	bool TRiverRealPatch::CacheIsLoaded() const
	{
		return m_p->m_loadState==LS_LOADED;
	}

	void TRiverRealPatch::ClearCache()
	{
		if(m_p->m_loadState==LS_NOTLOAD)
			return;

		SAFE_DELETE(m_p->m_pMtl);
		m_p->m_loadState=LS_NOTLOAD;
	}

	void TRiverRealPatch::DeleteCacheFile()
	{
		if(m_p->m_loadState==LS_NOTLOAD)
			return;

		m_p->m_pMtl->DeleteCacheFile();
	}

	const AABBox& TRiverRealPatch::GetWorldBox() const
	{
		return m_p->m_box;
	}

	void TRiverRealPatch::BrushMtl( const tagTEditBrush& brush )
	{
		if(m_p->m_loadState!=LS_LOADED)
			return;

		if(m_p->m_pMtl!=NULL)
		{
			tagTEditBrush localBrush=brush;
			if(ConvLocalMtlBrush(localBrush))
			{
				Heightmap *pAlphaMap=m_p->m_pMtl->m_pMap->GetHeightmap();
				m_p->m_pMtl->BrushMap(localBrush);
			}
		}
	}

	void TRiverRealPatch::SmoothMtl( const tagTEditBrush& brush )
	{
		if(m_p->m_loadState!=LS_LOADED)
			return;

		if(m_p->m_pMtl!=NULL)
		{
			tagTEditBrush localBrush=brush;
			if(ConvLocalMtlBrush(localBrush))
				m_p->m_pMtl->SmoothMap(localBrush);
		}
	}

	void TRiverRealPatch::NoiseMtl( const tagTEditBrush& brush,float noiseFreq )
	{
		if(m_p->m_loadState!=LS_LOADED)
			return;

		if(m_p->m_pMtl!=NULL)
		{
			tagTEditBrush localBrush=brush;
			if(ConvLocalMtlBrush(localBrush))
				m_p->m_pMtl->NoiseMap(localBrush,noiseFreq);
		}
	}

	bool TRiverRealPatch::ConvLocalMtlBrush( tagTEditBrush& brush )
	{
		int texlPerTile=TRiverEditor::EPatchAlphaMapSize/TRiverEditor::EPatchSize;

		//--中心点坐标移动到本地格子空间
		POINT ltpt={brush.centerX-m_p->m_rect.left,brush.centerZ-m_p->m_rect.top};
		//--中心点坐标缩放到贴图空间
		brush.centerX=ltpt.x*texlPerTile;
		brush.centerZ=ltpt.y*texlPerTile;

		//--内径外径大小缩放到贴图空间
		brush.inner=brush.inner*texlPerTile;
		brush.outer=brush.outer*texlPerTile;

		//--测试是否影响本patch的贴图
		RECT texRc;
		texRc.left=texRc.top=0;
		texRc.right=texRc.bottom=TRiverEditor::EPatchAlphaMapSize;
		RECT brushRc;
		brushRc.left=brush.centerX-brush.outer;
		brushRc.right=brush.centerX+brush.outer;
		brushRc.top=brush.centerZ-brush.outer;
		brushRc.bottom=brush.centerZ+brush.outer;
		RECT tmp;
		return ::IntersectRect(&tmp,&texRc,&brushRc)!=0;
	}

	bool TRiverRealPatch::Build( TRiverEditor *pEditor,const TCHAR* szSavePath,int id,Heightmap* pTerrainHmap,IUICallBack* pUI )
	{
		if(m_p->m_loadState!=LS_LOADED)
		{
			pUI->AddMsg(_T("河流cache未load完成."));
			return false;
		}

		TCHAR szPath[512];

		if( m_p->m_pMtl->IsEmptyAlpha() )
			return true;

		//--写河流patch资源文件
		_stprintf(szPath,_T("%sRiver_%02d_%03d.rpatch"),szSavePath,m_p->m_riverID,id);
		TCHAR szAlphaMap[512];
		_stprintf(szAlphaMap,_T("%sRiver_%02d_%03d.tga"),szSavePath,m_p->m_riverID,id);
		ResRiverPatch::EditorSave(pEditor,m_p->m_riverID,szPath,szAlphaMap,&m_p->m_rect);

		//--生成alphamap
		Heightmap *pAlphaMap=m_p->m_pMtl->m_pMap->GetHeightmap();
		Heightmap tmp;
		pAlphaMap->CopyTo(tmp);
		tagRiverMtlDesc *pMtlDesc=pEditor->GetRiverMtlDescByID(m_p->m_riverID);
		const int nRiverHeight = pEditor->GetRiverHeightByID( m_p->m_riverID );
		for(UINT j=0;j<tmp.Height();j++)
		{
			for(UINT i=0;i<tmp.Width();i++)
			{
				if(tmp.GetSafeVal(i,j)==0)
					continue;

				if( !pMtlDesc->bCloseEdgeFade )
				{
					LONG nX = m_p->m_rect.left + i;
					LONG nZ = m_p->m_rect.top + j;
					int nRiverDeep = nRiverHeight - pTerrainHmap->GetSafeVal(nX,nZ);
					int nEdgeDeep = 255;//nRiverDeep / fMaxDeep * 255;
					if( nRiverDeep <= 50 )
					{
						if( nRiverDeep >= 0 )
						{
							nEdgeDeep = int( nRiverDeep / 50.0f * 255 );
							if( nEdgeDeep < 30 ) // 为了防止折射效果边缘漏，边缘过度Alpha值为30~255
								nEdgeDeep = 30;
						}
						else
							nEdgeDeep = 0;
					}
					int val=ftol_ambient(nEdgeDeep*pMtlDesc->alpha);
					tmp.SafeSetVal(i,j,val);
				}
				else
				{
					int val=ftol_ambient(tmp.GetSafeVal(i,j)*pMtlDesc->alpha);
					tmp.SafeSetVal(i,j,val);
				}
			}
		}
		Image image;
		image.Create(tmp.Width(),tmp.Height(),EPF_A8);
		FMemcpy(image.GetRawBuffer(),tmp.GetRawBuffer(),tmp.Width()*tmp.Height());
		image.WriteToFile(szAlphaMap);

		return true;
	}

	void TRiverRealPatch::ApplyMtl(TRiverEditor *pEditor)
	{
		if(m_p->m_loadState!=LS_LOADED)
			return;

		tagRiverMtlDesc *pDesc=pEditor->GetRiverMtlDescByID(m_p->m_riverID);
		m_p->m_pMtl->m_alpha=pDesc->alpha;
		SAFE_DELETE(m_p->m_pMtl->m_pTex);
		m_p->m_pMtl->LoadBaseTex(pDesc->szWaterTex);
		m_p->m_pMtl->m_color.m_color.diffuse=pDesc->diffuse;
		m_p->m_pMtl->m_color.m_color.ambient=pDesc->ambient;
		m_p->m_pMtl->m_color.m_color.specular=pDesc->specular;
		m_p->m_pMtl->InvalidateRenderCache();
	}

	bool TRiverRealPatch::HaveWater()
	{
		if( m_p->m_loadState!=LS_LOADED )
			return false;

		if( NULL == m_p->m_pMtl )
			return false;

		return !m_p->m_pMtl->IsEmptyAlpha();
	}

	bool TRiverRealPatch::BuildWaterBitmap( BitMap& bitmap,RECT rc,IUICallBack* pUI )
	{
		if(m_p->m_loadState!=LS_LOADED)
		{
			pUI->AddMsg(_T("河流cache未load完成."));
			return false;
		}

		if( NULL==m_p->m_pMtl )
			return false;

		Image alphaMap;
		m_p->m_pMtl->CopyAlphaMap2Img(&alphaMap,rc.right-rc.left,rc.bottom-rc.top);

		int x,z;
		BYTE* pPixel=(BYTE*)alphaMap.GetRawBuffer();
		for(z=rc.top;z<rc.bottom;z++)
		{
			for(x=rc.left;x<rc.right;x++)
			{
				if((*pPixel)!=0)
				{
					bitmap.SafeSetValue(x,z,true);
				}
				pPixel++;
			}
		}

		return true;
	}
}//namespace Cool3D