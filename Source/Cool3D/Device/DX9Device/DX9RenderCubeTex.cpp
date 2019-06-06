#include "StdAfx.h"
#include ".\DX9RenderCubeTex.h"
#include "..\..\Util\Exception.h"
#include "DX9Device.h"
#include "DX9BaseDef.h"
#include "DX9Tex.h"
#include "..\..\Kernel.h"

namespace Cool3D
{
	namespace
	{
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
	}

	namespace Detail
	{
		int	DX9RenderCubeTex::s_numInst=0;

		DX9RenderCubeTex::DX9RenderCubeTex(void)
		{
			m_pTex=NULL;
			m_pDepthBuffer=NULL;
			m_pCurrentFace=NULL;
			m_pLastTarget=NULL;
			m_pLastDepthBuffer=NULL;

			m_size = 1;
			m_createFmt = EPF_A8R8G8B8;
			m_depthFmt = D3DFMT_D16;
			m_bDepthStencilBuffer = false;
			m_bRendering = false;

			s_numInst++;
		}

		DX9RenderCubeTex::~DX9RenderCubeTex(void)
		{
			MyDX9Dev()->OnRenderCubeTexFree(this);

			SAFE_RELEASE(m_pTex);
			SAFE_RELEASE(m_pDepthBuffer);
			SAFE_RELEASE(m_pCurrentFace);

#ifdef RENDER_STATUS
			Device()->GetRenderStatus()->renderTexSize-=m_bufferSize;
#endif

			s_numInst--;
		}

		bool DX9RenderCubeTex::Create(UINT size,EPixelFormat fmt,bool bDepthStencilBuffer,UINT mipLevels,bool bThrowExcept)
		{
			m_size = size;
			m_createFmt = fmt;
			m_bDepthStencilBuffer = bDepthStencilBuffer;

			HRESULT hr=D3DXCreateCubeTexture(DX9Dev(),size,
				1,//num mip lvs
				D3DUSAGE_RENDERTARGET,//usage
				PF2DX9(fmt),//format
				D3DPOOL_DEFAULT,&m_pTex);

			if(bThrowExcept)
				THROW_FAILED(hr,NULL);

			if(FAILED(hr))
				return false;

			DX9Device *pDev=static_cast<DX9Device*>(Device());
			D3DFORMAT zFmt=pDev->GetDepthBufferFmt();
			if(bDepthStencilBuffer)
			{
				hr=DX9Dev()->CreateDepthStencilSurface(size,size,zFmt,D3DMULTISAMPLE_NONE,
					0,FALSE,&m_pDepthBuffer,NULL);

				if(bThrowExcept)
					THROW_FAILED(hr,NULL);

				if(FAILED(hr))
					return false;
			}


#ifdef RENDER_STATUS
			m_bufferSize=GetPixelFormatSize(fmt)*m_size*m_size;
			Device()->GetRenderStatus()->renderTexSize+=m_bufferSize;
#endif

			return true;
		}

		bool DX9RenderCubeTex::Begin()
		{
			if(m_pTex==NULL)
				return false;
			//--save old
			HRESULT hr=DX9Dev()->GetRenderTarget(0,&m_pLastTarget);
			if(FAILED(hr))
				return false;
			hr=DX9Dev()->SetRenderTarget( 0, m_pCurrentFace );
			if(FAILED(hr))
				return false;

			if(m_pDepthBuffer)
			{
				hr=DX9Dev()->GetDepthStencilSurface(&m_pLastDepthBuffer);
				if(FAILED(hr))
					return false;
				hr=DX9Dev()->SetDepthStencilSurface( m_pDepthBuffer );
				if(FAILED(hr))
					return false;
			}

			m_bRendering = true;
			return true;
		}

		bool DX9RenderCubeTex::SetPass( int pass )
		{
			if ( pass<0 || pass>5 )
				return false;

			SAFE_RELEASE( m_pCurrentFace );

			if ( !m_pTex || !m_pDepthBuffer )
				return false;

			m_pTex->GetCubeMapSurface( (D3DCUBEMAP_FACES)pass, 0, &m_pCurrentFace );

			return true;
		}

		void DX9RenderCubeTex::End()
		{
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
		}

		bool DX9RenderCubeTex::HaveDepthBuffer()
		{
			return m_pDepthBuffer!=NULL;
		}

		bool DX9RenderCubeTex::HaveStencilBuffer()
		{
			switch(m_depthFmt)
			{
			case D3DFMT_D16:	return false;
			case D3DFMT_D24S8:	return true;
			}
			return false;
		}

		bool DX9RenderCubeTex::WriteToFile(const TCHAR* szPath)
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

		void DX9RenderCubeTex::PreDeviceReset()
		{
			int ref = m_pCurrentFace->Release();
			ref = m_pTex->Release();
			if(m_pDepthBuffer != NULL)
				ref = m_pDepthBuffer->Release();

#ifdef RENDER_STATUS
			Device()->GetRenderStatus()->renderTexSize-=m_bufferSize;
#endif
		}

		void DX9RenderCubeTex::PostDeviceReset()
		{
			Create(m_size,m_createFmt,m_bDepthStencilBuffer);
		}
	}//namespace Detail
}//namespace Cool3D