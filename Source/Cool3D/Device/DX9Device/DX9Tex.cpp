#include "StdAfx.h"
#include ".\dx9tex.h"
#include "..\..\Util\Exception.h"
#include "..\..\Util\FMemory.h"
#include <d3dx9tex.h>
#include "DX9Device.h"
#include "..\..\Image\Image.h"
#include "DX9BaseDef.h"
#include "..\..\Kernel.h"
#include "..\..\Util\Filename.h"
#include "..\..\VFS\IFS.h"
#include <memory>
#include "..\..\Image\ImageTGA.h"
#include "..\..\Cool3DOption.h"

namespace Cool3D
{
	namespace Detail
	{
		DX9Tex::DX9Tex(void)
		{
			m_pTex=NULL;
			memset(&m_topSurfaceDesc,0,sizeof(m_topSurfaceDesc));

			m_fileSize=0;
		}

		DX9Tex::~DX9Tex(void)
		{
			SAFE_RELEASE(m_pTex);
			memset(&m_topSurfaceDesc,0,sizeof(m_topSurfaceDesc));

#ifdef RENDER_STATUS
			Device()->GetRenderStatus()->texSize-=m_fileSize;
#endif
		}

		UINT DX9Tex::GetSizeInBytes()
		{
			return m_fileSize;
		}

		void DX9Tex::CreateFromImage_A8(Image *pImg)
		{
			ASSERT(pImg!=NULL);
			ASSERT(pImg->GetPixelFormat()==EPF_A8);

			int w=pImg->Width();
			int h=pImg->Height();
			HRESULT hr=D3DXCreateTexture(DX9Dev(),w,h,
				1/*num lv*/,0,D3DFMT_A8,D3DPOOL_MANAGED,&m_pTex);
			THROW_FAILED(hr,NULL);
			hr=m_pTex->GetLevelDesc(0,&m_topSurfaceDesc);
			ASSERT(SUCCEEDED(hr));

			CopyImg2Surface_A8(pImg);
		}

		void DX9Tex::CopyImg2Surface_A8(Image *pImg)
		{
			ASSERT(pImg!=NULL);
			ASSERT(m_pTex!=NULL);

			ASSERT(pImg->Width()==m_topSurfaceDesc.Width);
			ASSERT(pImg->Height()==m_topSurfaceDesc.Height);

			int pixelSize=0;
			switch(m_topSurfaceDesc.Format)
			{
			case D3DFMT_A8:
				pixelSize=1;
				break;
			case D3DFMT_A8B8G8R8:
			case D3DFMT_A8R8G8B8:
				pixelSize=4;
				break;
			case D3DFMT_A8P8:
			case D3DFMT_A8R3G3B2:
			case D3DFMT_A8L8:
				pixelSize=2;
				break;
			default://转换到了一个不支持的格式
				THROW_NULL(NULL,_T("not support format!"));
			}

			//--copy image to alpha channel
			D3DLOCKED_RECT lrc;
			HRESULT hr=m_pTex->LockRect(0,&lrc,NULL,0);

			int x,y;
			int w=pImg->Width();
			int h=pImg->Height();
			BYTE *pImgData=(BYTE*)pImg->GetRawBuffer();
			for(y=0;y<h;y++)
			{
				BYTE *pSurfaceLine=(BYTE*)lrc.pBits+y*lrc.Pitch;
				for(x=0;x<w;x++)
				{
					pSurfaceLine[pixelSize-1]=*pImgData;//alpha都在最后一个BYTE
					pSurfaceLine+=pixelSize;
					pImgData++;

				}
			}

			hr=m_pTex->UnlockRect(0);
		}

		void DX9Tex::CreateFromImage(Image *pImg,UINT numLevels)
		{
			ASSERT(m_pTex==NULL);
			ASSERT(pImg!=NULL);

			if(pImg->GetPixelFormat()==EPF_A8)
			{
				CreateFromImage_A8(pImg);
				return;
			}

			//--为了与DX9兼容,需要垂直Flip Image
			pImg->VFlip();

			//--
			UINT w=pImg->Width();
			UINT h=pImg->Height();

			if(numLevels!=1)
			{
				if(!IsPowerOfTwo(w) || !IsPowerOfTwo(h))
				{
					numLevels=1;
					Kernel::Inst()->SetLastError(_T("Image size is not power of 2,so disable mipmap gen."));
				}
			}

			D3DFORMAT fmt=PF2DX9(pImg->GetPixelFormat());
			if(fmt==D3DFMT_FORCE_DWORD)
				THROW_NULL(0,_T("Texture unsupport image format"));
			
			HRESULT hr=D3DXCreateTexture(DX9Dev(),w,h,numLevels,0,fmt,D3DPOOL_MANAGED,&m_pTex);
			THROW_FAILED(hr,NULL);
			hr=m_pTex->GetLevelDesc(0,&m_topSurfaceDesc);
			ASSERT(SUCCEEDED(hr));

			//如果当前硬件不支持Image的format,D3DXCreateTexture函数会自动选用相应的格式,
			//所以需要把Image也转换到对应的格式
			if(m_topSurfaceDesc.Format!=PF2DX9(pImg->GetPixelFormat()))
			{
				if(!pImg->ConvertFmt(DX9FMT2Cool3D(m_topSurfaceDesc.Format)))
				{
					THROW_NULL(0,_T("Image/Texture format match failed"));
				}
			}
			
			UpdateFromImage(pImg,true);
		}

		bool DX9Tex::CopyImg2Surface(Image *pImg,int level,bool autoScaleImg)
		{
			ASSERT(m_pTex!=NULL);
			ASSERT(pImg!=NULL);
			
			D3DSURFACE_DESC	surfaceDesc;
			HRESULT hr=m_pTex->GetLevelDesc(level,&surfaceDesc);
			if(FAILED(hr))
				return false;
			if(autoScaleImg)
			{
				pImg->Scale(surfaceDesc.Width,surfaceDesc.Height,IDeviceTex::s_MipMapQuat);
			}

			ASSERT(pImg->Width() == surfaceDesc.Width);
			ASSERT(pImg->Height() == surfaceDesc.Height);

			if(pImg->Width() != surfaceDesc.Width
				|| pImg->Height() != surfaceDesc.Height)
				return false;

			//--
			UINT w=pImg->Width();
			UINT h=pImg->Height();
			UINT imgPitch=pImg->Width()*pImg->GetBpp()/8;

			D3DLOCKED_RECT lrc;
			hr=m_pTex->LockRect(level,&lrc,NULL,0);
			if(FAILED(hr))
			{
				Kernel::Inst()->SetLastError(_T("texture lock failed,lv=%d."),level);
				return false;
			}

			//--copy
			if(lrc.Pitch==imgPitch)//两个Pitch相等,则可以整个copy
			{
				ASSERT(pImg->GetSizeInBytes() == h*imgPitch);
				FMemcpy(lrc.pBits,pImg->GetRawBuffer(),h*imgPitch);
			}
			else//两个Pitch不相等,只能逐行copy
			{
				BYTE* pDest=(BYTE*)lrc.pBits;
				BYTE* pSrc=(BYTE*)pImg->GetRawBuffer();
				for(int y=0;y<(int)h;y++)
				{
					FMemcpy(pDest,pSrc,imgPitch);
					pDest+=lrc.Pitch;
					pSrc+=imgPitch;
				}
			}//endof else
			hr=m_pTex->UnlockRect(level);
			return true;
		}

		void DX9Tex::UpdateFromImage(Image *pImg,bool bUpdateMipMap)
		{
			if(pImg->GetPixelFormat()==EPF_A8)
			{
				CopyImg2Surface_A8(pImg);
				return;
			}

			//--写入第0层
			CopyImg2Surface(pImg,0,true);//第0层也自动Scale???

			if(bUpdateMipMap)
			{
				DWORD numMipmap=m_pTex->GetLevelCount();
				for(DWORD i=1;i<numMipmap;i++)
				{
					CopyImg2Surface(pImg,i,true);
				}
			}
		}

		void DX9Tex::CreateFromJPGLump(void *pLump, int lumpSize)
		{
			//-- 释放老的 
			SAFE_RELEASE(m_pTex);

			//-- 得到信息
			D3DFORMAT fmt = D3DFMT_UNKNOWN;

			D3DXIMAGE_INFO imgInfo;
			HRESULT hr = D3DXGetImageInfoFromFileInMemory(pLump,lumpSize,&imgInfo);

			hr=D3DXCreateTextureFromFileInMemoryEx(DX9Dev(),
					pLump,lumpSize,
					D3DX_DEFAULT_NONPOW2,D3DX_DEFAULT_NONPOW2,
					1,//numLevels,
					0,//usage
					fmt,
					D3DPOOL_MANAGED,
					D3DX_DEFAULT,
					D3DX_DEFAULT,
					0,//color key
					NULL,NULL,
					&m_pTex);
				THROW_FAILED(hr,_T("CreateFromJPGLump"));
				hr=m_pTex->GetLevelDesc(0,&m_topSurfaceDesc);
				ASSERT(SUCCEEDED(hr));
		}

		void DX9Tex::LoadFromFile(IFS *pFS,const TCHAR* szPath,UINT numLevels,bool bManagedPool,bool bUseDDS,ETexDetail texDetail)
		{
			ASSERT(m_pTex==NULL);

			tstring szLoadPath = szPath;

			//--使用DDS?
			if(bUseDDS)
			{
				Filename szFileName = szPath;
				Filename szDDS = szFileName.NoExtension()+_T(".dds");
				UINT DDSSize=pFS->GetSize(szDDS.c_str());
				if(DDSSize!=0 && DDSSize!=-1)
				{
					szLoadPath = szDDS;
				}
			}

			//--load file to mem lump
			m_fileSize=pFS->GetSize(szLoadPath.c_str());
			if(m_fileSize==0 || m_fileSize==-1)
				THROW_NULLEX(0,_T("Texture file not found"),szLoadPath.c_str());

			DWORD hFile=pFS->Open(szLoadPath.c_str());
			if(hFile==NULL)
				THROW_NULLEX(0,_T("Texture file open failed"),szLoadPath.c_str());

			BYTE *pLump=(BYTE*)malloc(m_fileSize);
			pFS->Read(pLump,m_fileSize,hFile);
			pFS->Close(hFile);


			//--create
			bool bUseDX=true;

			D3DXIMAGE_INFO imgInfo;
			HRESULT hr = D3DXGetImageInfoFromFileInMemory(pLump,m_fileSize,&imgInfo);

			Filename fname(szLoadPath.c_str());
			if(_tcsicmp(fname.GetExtension().c_str(),_T("tga"))==0)
			{
				if(SUCCEEDED(hr))
				{
					if(imgInfo.Format == D3DFMT_L8)
					{
						bUseDX = false;
						CreateFromA8TGA(pLump,m_fileSize,&imgInfo);
					}
				}
			}

			if(bUseDX)
			{
				D3DPOOL pool= bManagedPool? D3DPOOL_MANAGED : D3DPOOL_DEFAULT;

				if(texDetail==ETexDetail_High
					||!CreateLowerLevel(pLump,m_fileSize,numLevels,pool,texDetail,szLoadPath.c_str()))
				{
					hr=D3DXCreateTextureFromFileInMemoryEx(DX9Dev(),
						pLump,m_fileSize,
						D3DX_DEFAULT_NONPOW2,D3DX_DEFAULT_NONPOW2,
						numLevels,
						0,//usage
						D3DFMT_UNKNOWN,
						pool,
						D3DX_DEFAULT,
						D3DX_DEFAULT,
						0,//color key
						NULL,NULL,
						&m_pTex);
					THROW_FAILED(hr,szLoadPath.c_str());
				}

				//--get top surface desc
				hr=m_pTex->GetLevelDesc(0,&m_topSurfaceDesc);
				ASSERT(SUCCEEDED(hr));
			}

			//--update render status
#ifdef RENDER_STATUS
			Device()->GetRenderStatus()->texSize+=m_fileSize;
#endif

			free(pLump);
			pLump=NULL;
		}

		bool DX9Tex::CreateLowerLevel(void *pLump,int lumpSize,UINT numLevels,D3DPOOL pool,ETexDetail texDetail,const TCHAR* szLoadPath)
		{
			//--创建临时纹理
			IDirect3DTexture9* pTempTex=NULL;
			HRESULT hr=D3DXCreateTextureFromFileInMemoryEx(DX9Dev(),
				pLump,m_fileSize,
				D3DX_DEFAULT_NONPOW2,D3DX_DEFAULT_NONPOW2,
				0,
				0,//usage
				D3DFMT_UNKNOWN,
				pool,
				D3DX_DEFAULT,
				D3DX_DEFAULT,
				0,//color key
				NULL,NULL,
				&pTempTex);
			THROW_FAILED(hr,szLoadPath);

			//--计算低精度纹理对应临时纹理的第几级？
			UINT startLevel=0;
			if(texDetail==ETexDetail_Mid)
				startLevel=1;
			else
				startLevel=2;

			//--获得低精度纹理尺寸
			if(startLevel>=pTempTex->GetLevelCount())
			{
				SAFE_RELEASE(pTempTex);
				return false;
			}

			D3DSURFACE_DESC desc;
			hr=pTempTex->GetLevelDesc(startLevel,&desc);
			THROW_FAILED(hr,szLoadPath);

			//--创建低精度纹理
			hr=DX9Dev()->CreateTexture(desc.Width,desc.Height,
				numLevels,
				0,
				desc.Format,
				pool,
				&m_pTex,NULL);
			THROW_FAILED(hr,szLoadPath);

			//--复制表面
			IDirect3DSurface9* pSrcSurface=NULL;
			IDirect3DSurface9* pDestSurface=NULL;
			for(UINT i=0,j=startLevel;i<m_pTex->GetLevelCount();++i,++j)
			{
				hr=pTempTex->GetSurfaceLevel(j,&pSrcSurface);
				if(FAILED(hr))
				{
					SAFE_RELEASE(pTempTex);
					return false;
				}

				hr=m_pTex->GetSurfaceLevel(i,&pDestSurface);
				THROW_FAILED(hr,szLoadPath);

				hr=D3DXLoadSurfaceFromSurface(pDestSurface,NULL,NULL,pSrcSurface,NULL,NULL,D3DX_FILTER_NONE,0);
				THROW_FAILED(hr,szLoadPath);

				SAFE_RELEASE(pSrcSurface);
				SAFE_RELEASE(pDestSurface);
			}

			//--释放临时纹理
			SAFE_RELEASE(pTempTex);
			return true;
		}

		bool DX9Tex::WriteToFile(const TCHAR* szPath)
		{
			if(m_pTex==NULL)
				return false;
			HRESULT hr=D3DXSaveTextureToFile(szPath,D3DXIFF_TGA,m_pTex,NULL);
			return SUCCEEDED(hr);
		}

		UINT DX9Tex::Width()
		{
			return m_topSurfaceDesc.Width;
		}
		UINT DX9Tex::Height()
		{
			return m_topSurfaceDesc.Height;
		}
		EPixelFormat DX9Tex::Format()
		{
			return DX9FMT2Cool3D(m_topSurfaceDesc.Format);
		}

		void DX9Tex::CreateFromData(int w,int h,EPixelFormat fmt,BYTE *pImageData,int numLevels)
		{
			int imgPitch=0;
			switch(fmt)
			{
			case EPF_A8R8G8B8:
				imgPitch=w*4;
				break;
			case EPF_R8G8B8:
				imgPitch=w*3;
				break;
			default:
				ASSERT(0 && _T("unknow pixel fmt"));
				break;
			}

			D3DFORMAT fmtdx=PF2DX9(fmt);
			if(fmtdx==D3DFMT_FORCE_DWORD)
				THROW_NULL(0,_T("Texture unsupport image format"));

			HRESULT hr=D3DXCreateTexture(DX9Dev(),w,h,
				numLevels,0,fmtdx,D3DPOOL_MANAGED,&m_pTex);
			THROW_FAILED(hr,NULL);
			
			hr=m_pTex->GetLevelDesc(0,&m_topSurfaceDesc);
			ASSERT(SUCCEEDED(hr));

			IDirect3DSurface9 *pSurface=NULL;
			hr=m_pTex->GetSurfaceLevel(0,&pSurface);
			THROW_FAILED(hr,NULL);


			RECT rc;
			rc.left=rc.top=0;
			rc.right=w;
			rc.bottom=h;
			hr=D3DXLoadSurfaceFromMemory(pSurface,
				NULL,//PALETTEENTRY
				NULL,//dst rect
				pImageData,
				fmtdx,
				imgPitch,
				NULL,//PALETTEENTRY
				&rc,//src rect
				D3DX_DEFAULT,
				0);
			pSurface->Release();

			//--auto gen mip map
			if(numLevels!=1)
			{
				hr=m_pTex->SetAutoGenFilterType(D3DTEXF_LINEAR);
				m_pTex->GenerateMipSubLevels();
			}
		}

		void DX9Tex::CreateFromA8TGA(BYTE *pLump,int fileSize,D3DXIMAGE_INFO *pImgInfo)
		{
			ASSERT(pImgInfo->Format == D3DFMT_L8);

			int w=pImgInfo->Width;
			int h=pImgInfo->Height;

			HRESULT hr=D3DXCreateTexture(DX9Dev(),w,h,
				1/*num lv*/,0,D3DFMT_A8,D3DPOOL_MANAGED,&m_pTex);
			THROW_FAILED(hr,NULL);
			hr=m_pTex->GetLevelDesc(0,&m_topSurfaceDesc);
			ASSERT(SUCCEEDED(hr));

			int pixelSize=0;
			switch(m_topSurfaceDesc.Format)
			{
			case D3DFMT_A8:
				pixelSize=1;
				break;
			case D3DFMT_A8B8G8R8:
			case D3DFMT_A8R8G8B8:
				pixelSize=4;
				break;
			case D3DFMT_A8P8:
			case D3DFMT_A8R3G3B2:
			case D3DFMT_A8L8:
				pixelSize=2;
				break;
			default://转换到了一个不支持的格式,所以现在使用最普通的格式重建贴图对象
				SAFE_RELEASE(m_pTex);
				hr=D3DXCreateTexture(DX9Dev(),w,h,
					1/*num lv*/,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&m_pTex);
				THROW_FAILED(hr,NULL);
				hr=m_pTex->GetLevelDesc(0,&m_topSurfaceDesc);
				ASSERT(SUCCEEDED(hr));
				ASSERT(m_topSurfaceDesc.Format == D3DFMT_A8R8G8B8);
				pixelSize=4;
				break;
			}

			//--copy image to alpha channel
			RECT rc;
			rc.left=rc.top=0;
			rc.right=w;
			rc.bottom=h;
			D3DLOCKED_RECT lrc;
			hr=m_pTex->LockRect(0,&lrc,&rc,0);
			if(SUCCEEDED(hr))
			{
				int x,y;
				BYTE *pImgData=pLump+12+6;//see class ImageTGA
				for(y=0;y<h;y++)
				{
					BYTE *pSurfaceLine=(BYTE*)lrc.pBits+y*lrc.Pitch;
					for(x=0;x<w;x++)
					{
						pSurfaceLine[pixelSize-1]=*pImgData;//alpha都在最后一个BYTE
						pSurfaceLine+=pixelSize;
						pImgData++;

					}
				}

				hr=m_pTex->UnlockRect(0);
			}//endof if
		}
	}
}//endof namespace