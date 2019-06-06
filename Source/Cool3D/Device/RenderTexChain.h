#pragma once
#include ".\IDevice.h"

namespace Cool3D
{
	class IRenderTexture;
	/**	\class RenderTexChain
		\brief 两个RenderTex的交换链
	*/
	class RenderTexChain
	{
	public:
		RenderTexChain(void);
		virtual ~RenderTexChain(void);
		
		void Create(UINT w,UINT h,EPixelFormat fmt=EPF_A8R8G8B8,bool bDepthStencilBuffer=true);
		void Destroy();

		IRenderTexture* Src();
		IRenderTexture* Dest();
		void Flip();

	private:
		IRenderTexture* m_renderTex[2];
		int m_index;
	};
}//namespace Cool3D