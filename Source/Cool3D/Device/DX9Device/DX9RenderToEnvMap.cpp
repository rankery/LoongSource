#include "StdAfx.h"
#include ".\DX9RenderToEnvMap.h"
#include "..\..\Util\DebugMisc.h"
#include "..\..\Util\Exception.h"
#include ".\DX9RenderTex.h"
#include "DX9Device.h"
#include "DX9BaseDef.h"

namespace Cool3D
{
	namespace Detail
	{
		DX9RenderToEnvMap::DX9RenderToEnvMap(void)
		{
			m_pRenderToEnvMap=NULL;
		}

		DX9RenderToEnvMap::~DX9RenderToEnvMap(void)
		{
			SAFE_RELEASE(m_pRenderToEnvMap);
		}

		void DX9RenderToEnvMap::Create(UINT size,EPixelFormat fmt,bool bDepthStencilBuffer)
		{
			SAFE_RELEASE(m_pRenderToEnvMap);

			DX9Device *pDev=static_cast<DX9Device*>(Device());
			D3DFORMAT zFmt=pDev->GetDepthBufferFmt();

			BOOL bDepth=bDepthStencilBuffer?TRUE:FALSE;

			HRESULT hr=D3DXCreateRenderToEnvMap(DX9Dev(),size,1,PF2DX9(fmt),
				bDepth,zFmt,&m_pRenderToEnvMap);
			THROW_FAILED(hr,_T("D3DXCreateRenderToEnvMap"));
		}

		void DX9RenderToEnvMap::BeginHemisphere(IRenderTexture* pRenderTex)
		{
			IDirect3DTexture9* pDX9Tex=((DX9RenderTex*)pRenderTex)->GetDX9Texture();
			HRESULT hr=m_pRenderToEnvMap->BeginHemisphere(NULL,pDX9Tex);
			THROW_FAILED(hr,_T("BeginHemisphere"));
		}

		void DX9RenderToEnvMap::Face(int face)
		{
			HRESULT hr=m_pRenderToEnvMap->Face(CubeMapFace2DX9(face),D3DX_DEFAULT);
			THROW_FAILED(hr,_T("Face"));
		}

		void DX9RenderToEnvMap::End()
		{
			HRESULT hr=m_pRenderToEnvMap->End(D3DX_FILTER_NONE);
			THROW_FAILED(hr,_T("End"));
		}

	}
}//namespace Cool3D