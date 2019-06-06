#pragma once
#include "..\irendertexture.h"

namespace Cool3D
{
	namespace Detail
	{
		/**	\class DX9RenderTex 
		\brief DX9 render target for texture
		*/
		class DX9RenderTex : public IRenderTexture
		{
		public:
			DX9RenderTex(void);
			virtual ~DX9RenderTex(void);

			virtual bool Create(UINT w,UINT h,EPixelFormat fmt=EPF_A8R8G8B8,bool bDepthStencilBuffer=true,bool bLockable=false,UINT mipLevels=1,bool bThrowExcept=true,bool bUseBackBufferMutiSampleType=false);
			virtual bool Begin();
			virtual void End();
			virtual bool HaveDepthBuffer();
			virtual bool HaveStencilBuffer();
			virtual bool WriteToFile(const TCHAR* szPath);

			virtual EPixelFormat Format() { return m_createFmt;}
			virtual UINT Width() { return m_createW;}
			virtual UINT Height(){ return m_createH;}

			virtual void LockRect( tagLockedRect *pLock, CONST RECT *pRect );
			virtual void UnlockRect();
			virtual void StrechRect(IDeviceTex* pSrc,CONST RECT* pDestRect,CONST RECT* pSrcRect,ETextrueFilter filter);
			virtual void StrechRect(IRenderTexture* pSrc,CONST RECT* pDestRect,CONST RECT* pSrcRect,ETextrueFilter filter);

			IDirect3DTexture9* GetDX9Texture()			{ return m_pTex;}
			IDirect3DSurface9* GetDX9Surface();
			IDirect3DSurface9* GetDX9DepthBuffer()		{ return m_pDepthBuffer; }

			void PreDeviceReset();
			void PostDeviceReset();

		private:
			IDirect3DTexture9		*m_pTex;
			IDirect3DSurface9		*m_pTargetSurface;
			IDirect3DSurface9		*m_pMultiSampleTargetSurface;
			IDirect3DSurface9		*m_pDepthBuffer;
			IDirect3DSurface9		*m_pLockSurface;
			D3DSURFACE_DESC			m_topSurfaceDesc;

			IDirect3DSurface9		*m_pLastTarget;
			IDirect3DSurface9		*m_pLastDepthBuffer;

			bool					m_bRendering;

			int						m_createW,
									m_createH;
			EPixelFormat			m_createFmt;
			D3DFORMAT				m_depthFmt;
			bool					m_bDepthStencilBuffer;
			bool					m_bLockable;
			bool					m_bUseBackBufferMutiSampleType;
			UINT					m_size;

			static int				s_numInst;
		};
	}//namespace Detail
}//namespace Cool3D