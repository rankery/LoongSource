#pragma once
#include "..\IRenderCubeTexture.h"

namespace Cool3D
{
	namespace Detail
	{
		/**	\class DX9RenderCubeTex 
			\brief DX9 render target for texture
		*/
		class DX9RenderCubeTex : public IRenderCubeTexture
		{
		public:
			DX9RenderCubeTex(void);
			virtual ~DX9RenderCubeTex(void);

			virtual bool Create(UINT size,EPixelFormat fmt=EPF_A8R8G8B8,bool bDepthStencilBuffer=true,UINT mipLevels=1,bool bThrowExcept=true);
			virtual bool Begin();
			virtual bool SetPass(int pass);
			virtual void End();
			virtual bool HaveDepthBuffer();
			virtual bool HaveStencilBuffer();
			virtual bool WriteToFile(const TCHAR* szPath);

			virtual EPixelFormat Format() { return m_createFmt;}
			virtual UINT Size() { return m_size;}

			IDirect3DCubeTexture9*	GetDX9Texture()			{ return m_pTex;}
			IDirect3DSurface9*		GetDX9DepthBuffer()		{ return m_pDepthBuffer; }

			void PreDeviceReset();
			void PostDeviceReset();

		private:
			IDirect3DCubeTexture9	*m_pTex;
			IDirect3DSurface9		*m_pDepthBuffer;
			IDirect3DSurface9		*m_pCurrentFace;

			IDirect3DSurface9		*m_pLastTarget;
			IDirect3DSurface9		*m_pLastDepthBuffer;

			bool					m_bRendering;

			int						m_size;
			EPixelFormat			m_createFmt;
			D3DFORMAT				m_depthFmt;
			bool					m_bDepthStencilBuffer;

			UINT					m_bufferSize;

			static int				s_numInst;
		};
	}//namespace Detail
}//namespace Cool3D