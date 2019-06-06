#pragma once
#include ".\IDevice.h"

namespace Cool3D
{
	class IRenderTexture;
	/**	\class IRenderToEnvMap
		\brief ª∑æ≥”≥…‰Ã˘Õº‰÷»æ∆˜
	*/
	class IRenderToEnvMap
	{
	public:
		IRenderToEnvMap(void);
		virtual ~IRenderToEnvMap(void);

		virtual void Create(UINT size,EPixelFormat fmt=EPF_A8R8G8B8,bool bDepthStencilBuffer=true) =0;
		virtual void BeginHemisphere(IRenderTexture* pRenderTex) =0;
		virtual void Face(int face) =0;
		virtual void End() =0;

	};
}//namespace Cool3D