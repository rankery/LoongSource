#include "stdafx.h"
#include ".\TRiverPatchProxy.h"
#include ".\TRiverRealPatch.h"
#include "..\Util\FMemory.h"
#include "..\Math\Frustum.h"
#include ".\TRiverEditor.h"

namespace Cool3D
{
	tstring TRiverPatchProxy::s_cachePath=_T("");

	class TRiverPatchProxy::Member
	{
	public:
		RECT				m_rect;			//patch在整个map中占据的rect大小
		UINT				m_riverID;		//河流ID
		UINT				m_patchID;		//patch的索引
		POINT				m_center;		//patch的中心点
		TRiverRealPatch		*m_realPatch;	//实际的数据
		int					m_height;		//河流高度
		
	public:
		Member(int height)
			:m_height(height)
		{
			FZeroMem(&m_rect,sizeof(m_rect));
			m_realPatch=NULL;
			m_riverID=0;
			m_patchID=0;
		}

		~Member()
		{
			SAFE_DELETE(m_realPatch);
		}
	};

	TRiverPatchProxy::TRiverPatchProxy( int height )
	{
		m_p=new Member(height);
	}

	TRiverPatchProxy::~TRiverPatchProxy()
	{
		SAFE_DELETE(m_p);
	}

	void TRiverPatchProxy::Create( RECT rc,UINT riverID,UINT patchID )
	{
		m_p->m_rect=rc;
		m_p->m_center.x=(rc.left+rc.right)/2;
		m_p->m_center.y=(rc.top+rc.bottom)/2;
		m_p->m_riverID=riverID;
		m_p->m_patchID=patchID;
	}

	void TRiverPatchProxy::Update( TRiverEditor *pEditor,Heightmap *pHMap,const RECT* pDrawRect )
	{
		ASSERT(pHMap!=NULL);
		ASSERT(pDrawRect!=NULL);

		BOOL inView=::PtInRect(pDrawRect, m_p->m_center);

		//--
		if(inView)
		{
			if(m_p->m_realPatch==NULL)
			{
				//创建内部数据
				this->GetRealPatch(pEditor,pHMap);
			}

			if(!m_p->m_realPatch->CacheIsLoaded())
			{
				TCHAR szCachePath[512];
				_stprintf(szCachePath,_T("%s\\river_%02d_patch_%03d.cache"),
					s_cachePath.c_str(),m_p->m_riverID,m_p->m_patchID);

				m_p->m_realPatch->LoadFromFile(pEditor,szCachePath);
			}
		}
		else
		{
			SaveCache();
			SAFE_DELETE(m_p->m_realPatch);
		}
	}

	void TRiverPatchProxy::SetCachePath( const TCHAR *szPath )
	{
		ASSERT(szPath!=NULL);

		s_cachePath=szPath;
	}

	void TRiverPatchProxy::GetRealPatch( TRiverEditor *pEditor,Heightmap *pHMap )
	{
		if(m_p->m_realPatch!=NULL)
			return;

		m_p->m_realPatch=new TRiverRealPatch(m_p->m_riverID,m_p->m_height);
		m_p->m_realPatch->CreateRenderBuffer(pEditor,pHMap,&m_p->m_rect);
	}

	TRiverRealPatch* TRiverPatchProxy::GetRealPatch( void )
	{
		return m_p->m_realPatch;
	}

	void TRiverPatchProxy::LoadCache(TRiverEditor *pEditor,Heightmap *pHMap)
	{
		if( NULL == pEditor || NULL == pHMap ) 
			return;

		if(m_p->m_realPatch==NULL)
		{
			GetRealPatch(pEditor,pHMap);

			TCHAR szCachePath[512];
			_stprintf(szCachePath,_T("%s\\river_%02d_patch_%03d.cache"),
				s_cachePath.c_str(),m_p->m_riverID,m_p->m_patchID);

			m_p->m_realPatch->LoadFromFile(pEditor,szCachePath);
		}
		else
		{
			if(!m_p->m_realPatch->CacheIsLoaded())
			{
				TCHAR szCachePath[512];
				_stprintf(szCachePath,_T("%s\\river_%02d_patch_%03d.cache"),
					s_cachePath.c_str(),m_p->m_riverID,m_p->m_patchID);

				m_p->m_realPatch->LoadFromFile(pEditor,szCachePath);
			}
		}
	}

	void TRiverPatchProxy::SaveCache()
	{
		if(m_p->m_realPatch!=NULL)
			m_p->m_realPatch->SaveToFile();
	}

	void TRiverPatchProxy::ClearCache()
	{
		SAFE_DELETE(m_p->m_realPatch);
	}

	void TRiverPatchProxy::DeleteCache()
	{
		TCHAR szCachePath[512];
		_stprintf(szCachePath,_T("%s\\river_%02d_patch_%03d.cache"),
			s_cachePath.c_str(),m_p->m_riverID,m_p->m_patchID);

		if(_taccess(szCachePath,4)!=-1)
			::DeleteFile(szCachePath);

		if(m_p->m_realPatch!=NULL)
			m_p->m_realPatch->DeleteCacheFile();
	}

	bool TRiverPatchProxy::Build( TRiverEditor *pEditor,Heightmap *pHMap,const TCHAR* szBasePath,Heightmap* pTerrainHmap,IUICallBack* pUI )
	{
		bool bRet=false;

		if(m_p->m_realPatch==NULL)
		{
			GetRealPatch(pEditor,pHMap);

			TCHAR szCachePath[512];
			_stprintf(szCachePath,_T("%s\\river_%02d_patch_%03d.cache"),
				s_cachePath.c_str(),m_p->m_riverID,m_p->m_patchID);

			m_p->m_realPatch->LoadFromFile(pEditor,szCachePath);

			bRet=m_p->m_realPatch->Build(pEditor,szBasePath,m_p->m_patchID,pTerrainHmap,pUI);
			SAFE_DELETE(m_p->m_realPatch);
		}
		else
		{
			if(m_p->m_realPatch->CacheIsLoaded())
			{
				bRet=m_p->m_realPatch->Build(pEditor,szBasePath,m_p->m_patchID,pTerrainHmap,pUI);
			}
			else
			{
				TCHAR szCachePath[512];
				_stprintf(szCachePath,_T("%s\\river_%02d_patch_%03d.cache"),
					s_cachePath.c_str(),m_p->m_riverID,m_p->m_patchID);

				m_p->m_realPatch->LoadFromFile(pEditor,szCachePath);
				bRet=m_p->m_realPatch->Build(pEditor,szBasePath,m_p->m_patchID,pTerrainHmap,pUI);
				m_p->m_realPatch->ClearCache();
			}
		}

		return bRet;
	}

	void TRiverPatchProxy::ApplyMtl( TRiverEditor *pEditor )
	{
		if(m_p->m_realPatch!=NULL)
			m_p->m_realPatch->ApplyMtl(pEditor);
	}

	void TRiverPatchProxy::Draw(const Frustum& frustum)
	{
		if(m_p->m_realPatch!=NULL 
			&& frustum.BoxInFrustum(m_p->m_realPatch->GetWorldBox()))
			m_p->m_realPatch->Draw();
	}

	void TRiverPatchProxy::BrushMtl( const tagTEditBrush& brush )
	{
		if(m_p->m_realPatch!=NULL)
			m_p->m_realPatch->BrushMtl(brush);
	}

	void TRiverPatchProxy::SmoothMtl( const tagTEditBrush& brush )
	{
		if(m_p->m_realPatch!=NULL)
			m_p->m_realPatch->SmoothMtl(brush);
	}

	void TRiverPatchProxy::NoiseMtl( const tagTEditBrush& brush,float noiseFreq )
	{
		if(m_p->m_realPatch!=NULL)
			m_p->m_realPatch->NoiseMtl(brush,noiseFreq);
	}

	const RECT& TRiverPatchProxy::GetRect() const
	{
		return m_p->m_rect;
	}

	bool TRiverPatchProxy::HaveWater(TRiverEditor *pEditor,Heightmap *pHMap,bool bLoad)
	{
		if( NULL == pEditor || NULL == pHMap ) 
			return false;

		bool bRet = false;
		if(m_p->m_realPatch==NULL)
		{
			if( bLoad )
			{
				GetRealPatch(pEditor,pHMap);

				TCHAR szCachePath[512];
				_stprintf(szCachePath,_T("%s\\river_%02d_patch_%03d.cache"),
					s_cachePath.c_str(),m_p->m_riverID,m_p->m_patchID);

				m_p->m_realPatch->LoadFromFile(pEditor,szCachePath);
				bRet=m_p->m_realPatch->HaveWater();
				SAFE_DELETE(m_p->m_realPatch);
			}
		}
		else
		{
			if(m_p->m_realPatch->CacheIsLoaded())
			{
				bRet=m_p->m_realPatch->HaveWater();
			}
			else if( bLoad )
			{
				TCHAR szCachePath[512];
				_stprintf(szCachePath,_T("%s\\river_%02d_patch_%03d.cache"),
					s_cachePath.c_str(),m_p->m_riverID,m_p->m_patchID);

				m_p->m_realPatch->LoadFromFile(pEditor,szCachePath);
				bRet=m_p->m_realPatch->HaveWater();
				m_p->m_realPatch->ClearCache();
			}
		}
		return bRet;
	}

	bool TRiverPatchProxy::BuildWaterBitmap( BitMap& bitmap,TRiverEditor *pEditor,Heightmap *pRvrHMap,IUICallBack* pUI )
	{
		bool bRet=true;

		if(m_p->m_realPatch==NULL)
		{
			GetRealPatch(pEditor,pRvrHMap);

			TCHAR szCachePath[512];
			_stprintf(szCachePath,_T("%s\\river_%02d_patch_%03d.cache"),
				s_cachePath.c_str(),m_p->m_riverID,m_p->m_patchID);

			m_p->m_realPatch->LoadFromFile(pEditor,szCachePath);
			bRet=m_p->m_realPatch->BuildWaterBitmap(bitmap,m_p->m_rect,pUI);
			SAFE_DELETE(m_p->m_realPatch);
		}
		else
		{
			if(m_p->m_realPatch->CacheIsLoaded())
			{
				bRet=m_p->m_realPatch->BuildWaterBitmap(bitmap,m_p->m_rect,pUI);
			}
			else
			{
				TCHAR szCachePath[512];
				_stprintf(szCachePath,_T("%s\\river_%02d_patch_%03d.cache"),
					s_cachePath.c_str(),m_p->m_riverID,m_p->m_patchID);

				m_p->m_realPatch->LoadFromFile(pEditor,szCachePath);
				bRet=m_p->m_realPatch->BuildWaterBitmap(bitmap,m_p->m_rect,pUI);
				m_p->m_realPatch->ClearCache();
			}
		}		

		return bRet;
	}

}//namespace Cool3D
