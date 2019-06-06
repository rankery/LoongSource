#pragma once
#include "..\idevicetex.h"
#include <d3d9.h>
#include "..\..\Image\ImageTGA.h"

namespace Cool3D
{
	namespace Detail
	{
		class DX9Tex : 	public IDeviceTex
		{
		public:
			DX9Tex(void);
			virtual ~DX9Tex(void);

			IDirect3DTexture9 *GetDX9()
			{ 
				return m_pTex; 
			}

			virtual UINT GetSizeInBytes();
			virtual void LoadFromFile(IFS *pFS,const TCHAR* szPath,UINT numLevels=0,bool bManagedPool=true,bool bUseDDS=true,ETexDetail texDetail=ETexDetail_High);
			virtual bool WriteToFile(const TCHAR* szPath);
			virtual UINT Width();
			virtual UINT Height();
			virtual EPixelFormat Format();
			
			virtual void CreateFromImage(Image *pImg,UINT numLevels);
			virtual void UpdateFromImage(Image *pImg,bool bUpdateMipMap);
			virtual void CreateFromData(int w,int h,EPixelFormat fmt,BYTE *pImageData,int numLevels);
			virtual void CreateFromJPGLump(void *pLump, int lumpSize);

		private:
			void CreateFromImage_A8(Image *pImg);//not support mipmap
			void CreateFromA8TGA(BYTE *pLump,int fileSize,D3DXIMAGE_INFO *pImgInfo);

			bool CopyImg2Surface(Image *pImg,int level,bool autoScaleImg);
			void CopyImg2Surface_A8(Image *pImg);

			bool CreateLowerLevel(void *pLump,int lumpSize,UINT numLevels,D3DPOOL pool,ETexDetail texDetail,const TCHAR* szLoadPath);

		private:
			IDirect3DTexture9	*m_pTex;
			D3DSURFACE_DESC		m_topSurfaceDesc;

			UINT m_fileSize;
		};
	}
}//endof namespace