#include "StdAfx.h"
#include ".\dx9rendertex.h"
#include "..\..\Util\Exception.h"
#include "DX9Device.h"
#include "DX9BaseDef.h"
#include "DX9Tex.h"
#include "..\..\Kernel.h"

namespace Cool3D
{
	namespace Detail
	{
		int	DX9RenderTex::s_numInst=0;

		DX9RenderTex::DX9RenderTex(void)
		{
			m_pTex=NULL;
			m_pDepthBuffer=NULL;
			memset(&m_topSurfaceDesc,0,sizeof(m_topSurfaceDesc));
			m_pTargetSurface = NULL;
			m_pMultiSampleTargetSurface = NULL;
			m_pLockSurface=NULL;
			m_pLastTarget=NULL;
			m_pLastDepthBuffer=NULL;

			m_createW = 1;
			m_createH = 1;
			m_createFmt = EPF_A8R8G8B8;
			m_depthFmt = D3DFMT_D16;
			m_bDepthStencilBuffer = false;
			m_bUseBackBufferMutiSampleType = false;

			m_bRendering = false;

			m_bLockable = false;

			s_numInst++;
		}

		DX9RenderTex::~DX9RenderTex(void)
		{
			MyDX9Dev()->OnRenderTexFree(this);


			SAFE_RELEASE(m_pTargetSurface);
			SAFE_RELEASE(m_pMultiSampleTargetSurface);
			SAFE_RELEASE(m_pTex);
			SAFE_RELEASE(m_pDepthBuffer);
			SAFE_RELEASE(m_pLastTarget);
			SAFE_RELEASE(m_pLastDepthBuffer);
			SAFE_RELEASE(m_pLockSurface);

			memset(&m_topSurfaceDesc,0,sizeof(m_topSurfaceDesc));

#ifdef RENDER_STATUS
			Device()->GetRenderStatus()->renderTexSize-=m_size;
#endif

			s_numInst--;
		}

		UINT GetPixelFormatSize(EPixelFormat fmt)
		{
			switch(fmt)
			{
			case EPF_R5G6B5:
			case EPF_A1R5G5B5:
				return 2;
			case EPF_R8G8B8:
				return 3;
			case EPF_A8R8G8B8:
			case EPF_X8R8G8B8:
				return 4;
			case EPF_A8:
				return 1;
			case EPF_A32R32G32B32F:
				return 16;
			}
			return 0;
		}

		bool DX9RenderTex::Create(UINT w,UINT h,EPixelFormat fmt,bool bDepthStencilBuffer,bool bLockable,UINT mipLevels,bool bThrowExcept,bool bUseBackBufferMutiSampleType)
		{
			m_createW = w;
			m_createH = h;
			m_createFmt = fmt;
			m_bDepthStencilBuffer = bDepthStencilBuffer;
			m_bLockable = bLockable;
			m_bUseBackBufferMutiSampleType = bUseBackBufferMutiSampleType;

			DX9Device *pDev = static_cast<DX9Device*>(Device());
			HRESULT hr = S_OK;

			// 使用后缓冲的抗锯齿级别，需要额外创建一个带有抗锯齿的的Surface
			D3DMULTISAMPLE_TYPE BackBufferMultiSampleType = pDev->GetMutiSampleType();
			DWORD dwBackBufferMultisampleQuality = pDev->GetMultisampleQuality();
			if( D3DMULTISAMPLE_NONE != BackBufferMultiSampleType && bUseBackBufferMutiSampleType )
			{

				hr = DX9Dev()->CreateRenderTarget(	w,h,PF2DX9(fmt),
													BackBufferMultiSampleType,
													dwBackBufferMultisampleQuality,
													false,
													&m_pMultiSampleTargetSurface, NULL );

				if(FAILED(hr))
					return false;
			}

			hr=D3DXCreateTexture(DX9Dev(),w,h,
				1,//num mip lvs
				D3DUSAGE_RENDERTARGET,//usage
				PF2DX9(fmt),//format
				D3DPOOL_DEFAULT,&m_pTex);
			
			if(bThrowExcept)
				THROW_FAILED(hr,NULL);

			if(FAILED(hr))
				return false;

			hr=m_pTex->GetSurfaceLevel(0,&m_pTargetSurface);
			THROW_FAILED(hr,NULL);
			
			D3DFORMAT zFmt=pDev->GetDepthBufferFmt();
			if(bDepthStencilBuffer)
			{
				if( D3DMULTISAMPLE_NONE != BackBufferMultiSampleType && bUseBackBufferMutiSampleType )
				{
					hr=DX9Dev()->CreateDepthStencilSurface(w,h,zFmt,BackBufferMultiSampleType,
						dwBackBufferMultisampleQuality,FALSE,&m_pDepthBuffer,NULL);
				}
				else
				{
					hr=DX9Dev()->CreateDepthStencilSurface(w,h,zFmt,D3DMULTISAMPLE_NONE,
						0,FALSE,&m_pDepthBuffer,NULL);
				}

				if(bThrowExcept)
					THROW_FAILED(hr,NULL);

				if(FAILED(hr))
					return false;
			}

			if(bLockable)
			{
				hr=DX9Dev()->CreateOffscreenPlainSurface(w,h,PF2DX9(fmt),D3DPOOL_SYSTEMMEM,
					&m_pLockSurface,NULL);

				if(bThrowExcept)
					THROW_FAILED(hr,NULL);

				if(FAILED(hr))
					return false;
			}

	
#ifdef RENDER_STATUS
			m_size=GetPixelFormatSize(fmt)*w*h;
			Device()->GetRenderStatus()->renderTexSize+=m_size;
#endif

			return true;
		}

		bool DX9RenderTex::Begin()
		{
			ASSERT(m_bRendering==false);

			if(m_pTex==NULL)
				return false;
			//--save old
			HRESULT hr=DX9Dev()->GetRenderTarget(0,&m_pLastTarget);
			if(FAILED(hr))
				return false;

			//--set new
			if( NULL == m_pMultiSampleTargetSurface )
				hr=DX9Dev()->SetRenderTarget(0,m_pTargetSurface);
			else
				hr=DX9Dev()->SetRenderTarget(0,m_pMultiSampleTargetSurface);

			if(FAILED(hr))
				return false;

			if(m_pDepthBuffer)
			{
				hr=DX9Dev()->GetDepthStencilSurface(&m_pLastDepthBuffer);
				if(FAILED(hr))
					return false;

				hr=DX9Dev()->SetDepthStencilSurface(m_pDepthBuffer);
				if(FAILED(hr))
					return false;
			}

			m_bRendering = true;
			return true;
		}

		void DX9RenderTex::End()
		{
			/*
			TCHAR szFile[256];
			_stprintf(szFile, _T("d:\\test%lu.tga"), this);
			this->WriteToFile(szFile);
			/**/

			m_bRendering = false;

			HRESULT hr;
			if(m_pLastTarget != NULL)
			{
				hr=DX9Dev()->SetRenderTarget(0,m_pLastTarget);
				m_pLastTarget->Release();
				m_pLastTarget = NULL;
			}
			if(m_pLastDepthBuffer != NULL)
			{
				hr=DX9Dev()->SetDepthStencilSurface(m_pLastDepthBuffer);
				m_pLastDepthBuffer->Release();
				m_pLastDepthBuffer =NULL;
			}

			// 把带有抗锯齿的Surface拷贝到texture的Surface
			if( NULL != m_pMultiSampleTargetSurface )
				DX9Dev()->StretchRect( m_pMultiSampleTargetSurface, NULL, m_pTargetSurface, NULL, D3DTEXF_POINT );
		}

		bool DX9RenderTex::HaveDepthBuffer()
		{
			return m_pDepthBuffer!=NULL;
		}

		bool DX9RenderTex::HaveStencilBuffer()
		{
			switch(m_depthFmt)
			{
			case D3DFMT_D16:	return false;
			case D3DFMT_D24S8:	return true;
			}
			return false;
		}

		bool DX9RenderTex::WriteToFile(const TCHAR* szPath)
		{
			if(m_pTex==NULL)
				return false;

			D3DXIMAGE_FILEFORMAT fmt;
			if(m_createFmt==EPF_A32R32G32B32F)
				fmt=D3DXIFF_DDS;
			else
				fmt=D3DXIFF_TGA;

			HRESULT hr=D3DXSaveTextureToFile(szPath,fmt,m_pTex,NULL);
			return SUCCEEDED(hr);
		}

		void DX9RenderTex::PreDeviceReset()
		{
			SAFE_RELEASE(m_pTargetSurface);
			SAFE_RELEASE(m_pMultiSampleTargetSurface);
			SAFE_RELEASE(m_pTex);
			SAFE_RELEASE(m_pDepthBuffer);
			SAFE_RELEASE(m_pLastTarget);
			SAFE_RELEASE(m_pLastDepthBuffer);
			SAFE_RELEASE(m_pLockSurface);

#ifdef RENDER_STATUS
			Device()->GetRenderStatus()->renderTexSize-=m_size;
#endif
		}

		void DX9RenderTex::PostDeviceReset()
		{
			Create(m_createW,m_createH,m_createFmt,m_bDepthStencilBuffer,m_bLockable,1,true,m_bUseBackBufferMutiSampleType);
		}

		void DX9RenderTex::LockRect( tagLockedRect *pLock, CONST RECT *pRect )
		{
			ASSERT(m_pLockSurface!=NULL);
			HRESULT hr=DX9Dev()->GetRenderTargetData(m_pTargetSurface,m_pLockSurface);
			THROW_FAILED(hr,NULL);

			D3DLOCKED_RECT lock;
			hr=m_pLockSurface->LockRect(&lock,pRect,D3DLOCK_READONLY);
			THROW_FAILED(hr,NULL);

			pLock->pitch=lock.Pitch;
			pLock->pBits=lock.pBits;
		}

		void DX9RenderTex::UnlockRect()
		{
			ASSERT(m_pLockSurface!=NULL);
			HRESULT hr=m_pLockSurface->UnlockRect();
			THROW_FAILED(hr,NULL);
		}

		void DX9RenderTex::StrechRect(IDeviceTex* pSrc,CONST RECT* pDestRect,CONST RECT* pSrcRect,ETextrueFilter filter)
		{
			DX9Tex* pDX9SrcTex=(DX9Tex*)pSrc;
			IDirect3DSurface9* pSrcSurface=NULL;
			HRESULT hr=pDX9SrcTex->GetDX9()->GetSurfaceLevel(0,&pSrcSurface);
			THROW_FAILED(hr,NULL);

			hr=DX9Dev()->StretchRect( pSrcSurface, pSrcRect, 
				m_pTargetSurface, pDestRect, TFilter2DX9(filter) );
			THROW_FAILED(hr,NULL);

			SAFE_RELEASE(pSrcSurface);
		}

		void DX9RenderTex::StrechRect(IRenderTexture* pSrc,CONST RECT* pDestRect,CONST RECT* pSrcRect,ETextrueFilter filter)
		{
			DX9RenderTex* pDX9SrcTex=(DX9RenderTex*)pSrc;

			HRESULT hr=DX9Dev()->StretchRect( pDX9SrcTex->GetDX9Surface(), pSrcRect, 
				m_pTargetSurface, pDestRect, TFilter2DX9(filter) );
			THROW_FAILED(hr,NULL);
		}

		IDirect3DSurface9* DX9RenderTex::GetDX9Surface()
		{ 
			if( NULL != m_pMultiSampleTargetSurface )
				return m_pMultiSampleTargetSurface;
			else
				return m_pTargetSurface;
		}
	}//namespace Detail
}//namespace Cool3D