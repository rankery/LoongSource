#include "StdAfx.h"
#include ".\rendertexchain.h"
#include ".\IRenderTexture.h"
#include "..\Kernel.h"
#include "..\Util\DebugMisc.h"

namespace Cool3D
{
	RenderTexChain::RenderTexChain(void)
	{
		m_renderTex[0]=NULL;
		m_renderTex[1]=NULL;
		m_index=0;
	}

	RenderTexChain::~RenderTexChain(void)
	{
		Destroy();
	}

	void RenderTexChain::Create(UINT w,UINT h,EPixelFormat fmt,bool bDepthStencilBuffer)
	{
		Destroy();

		for(int i=0;i<2;i++)
		{
			m_renderTex[i]=Device()->NewRenderTexture();
			m_renderTex[i]->Create(w,h,fmt,bDepthStencilBuffer,false,1);
		}

		m_index=0;
	}

	void RenderTexChain::Destroy()
	{
		SAFE_DELETE(m_renderTex[0]);
		SAFE_DELETE(m_renderTex[1]);
	}

	IRenderTexture* RenderTexChain::Src()
	{
		return m_renderTex[m_index];
	}

	IRenderTexture* RenderTexChain::Dest()
	{
		int destIndex=(m_index+1)%2;
		return m_renderTex[destIndex];
	}

	void RenderTexChain::Flip()
	{
		m_index=(m_index+1)%2;
	}

}//namespace Cool3D