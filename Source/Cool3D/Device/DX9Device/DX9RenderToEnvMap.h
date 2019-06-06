#pragma once
#include "..\irendertoenvmap.h"
#include <d3dx9core.h>

namespace Cool3D
{
	namespace Detail
	{
		class DX9RenderToEnvMap : public IRenderToEnvMap
		{
		public:
			DX9RenderToEnvMap(void);
			virtual ~DX9RenderToEnvMap(void);

			//--IRenderToEnvMap
			virtual void Create(UINT size,EPixelFormat fmt=EPF_A8R8G8B8,bool bDepthStencilBuffer=true);
			virtual void BeginHemisphere(IRenderTexture* pRenderTex);
			virtual void Face(int face);
			virtual void End();

		private:
			ID3DXRenderToEnvMap* m_pRenderToEnvMap;
		};
	}
}//namespace Cool3D