#include "StdAfx.h"
#include "IL\IL.h"
#include "IL\ILu.h"
#ifdef _UNICODE
#define IL_TEXT(s) L##s
#else
#define IL_TEXT(s) (s)
#endif

#include ".\Image.h"
#include "..\Util\Exception.h"
#include "..\Kernel.h"
#include "..\Util\FMemory.h"


namespace Cool3D
{
	class Image::Member
	{
	public:
		ILuint m_imgID;
	public:
		Member()
		{
			m_imgID=0;
		}
		~Member()
		{
			if(m_imgID != 0)
			{
				ilDeleteImages(1,&m_imgID);
				m_imgID=0;
			}
		}
		//--将Cool3D的pixelformat转换为bit per pixel和IL的格式
		static bool ilConvert2IL(EPixelFormat pixelFmt,BYTE& Bpp,ILenum& ilFmt)
		{
			//IL的BGRA顺序正好与Cool3D的相反,即IL_BGRA等价与Cool3D::EPF_ARGB
			switch(pixelFmt)
			{
			case EPF_R5G6B5:
				Bpp=16;
				ilFmt=IL_BGR;
				return true;
			case EPF_R8G8B8:
				Bpp=24;
				ilFmt=IL_BGR;
				return true;
			case EPF_A1R5G5B5:
				return false;
			case EPF_A8R8G8B8:
				Bpp=32;
				ilFmt=IL_BGRA;
				return true;
			case EPF_X8R8G8B8:
				Bpp=32;
				ilFmt=IL_BGRA;
				return true;
			case EPF_A8:
				Bpp=8;
				ilFmt=IL_LUMINANCE;
				return true;
			case EPF_A32R32G32B32F:
				Bpp=128;
				ilFmt=IL_BGRA;
				return true;
			}
			return false;
		}
		//--将IL的error放入Kernel中
		static void ilDetectError(const TCHAR* szFileName=NULL)
		{
			ILenum errorCode=ilGetError();
			if(errorCode!=IL_NO_ERROR)
			{
				if(szFileName)
					Kernel::Inst()->SetLastError(_T("Image error,%d,File=%s.\n"),errorCode,szFileName);
				else
					Kernel::Inst()->SetLastError(_T("Image error,%d.\n"),errorCode);
			}
		}

	};//end of class member

	UINT Image::GetID() const
	{
		return m_p->m_imgID;
	}

	Image::Image(void)
	{
		m_p=new Member;
	}

	Image::~Image(void)
	{
		delete m_p;
	}

	
	void Image::InitSys()
	{
		// Initialize IL
		ilInit();
		// Initialize ILU
		iluInit();

		//--初始化一些默认状态
		ilEnable(IL_FILE_OVERWRITE);
		ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
		//ilEnable(IL_ORIGIN_SET);
		ilEnable(IL_CONV_PAL);


		Member::ilDetectError();
		TRACE(_T("IL init ok,Vendor=%s,Version=%d\r\n"),ilGetString(IL_VENDOR),ilGetInteger(IL_VERSION_NUM));
	}
	void Image::ShutDownSys()
	{
		ilShutDown();
	}

	bool Image::Create(UINT w,UINT h,EPixelFormat pixelFmt)
	{
		Destroy();
		//--
		ilGenImages(1,&m_p->m_imgID);
		ilBindImage(m_p->m_imgID);
		BYTE bpp=24;
		ILenum ilFmt=IL_RGB;
		if(!Member::ilConvert2IL(pixelFmt,bpp,ilFmt))
		{
			return false;
			Kernel::Inst()->SetLastError(_T("不支持的Image象素格式"));
		}
		
		bool ok=ilTexImage(w,h,1,bpp/8,ilFmt,IL_UNSIGNED_BYTE,NULL)!=0;
		if(ok)
		{
			ilClearColour(255,255,255,255);
			ilClearImage();
		}
		else
			Member::ilDetectError();
		return true;
	}
	
	void Image::SetFS(IFS *pFS)
	{}
	
	bool Image::LoadFromFile(const TCHAR *szPath)
	{
		//--防止外部改变当前路径
		SetCurrentDirectory(Kernel::Inst()->GetWorkPath());
		//--
		ASSERT(m_p->m_imgID == 0);
		ilGenImages(1,&m_p->m_imgID);
		ilBindImage(m_p->m_imgID);
#ifdef _UNICODE
		ILboolean ret=ilLoadImage((const ILstring)WideToChar(szPath).c_str());
#else
		ILboolean ret=ilLoadImage((const ILstring)szPath);
#endif
		Member::ilDetectError(szPath);

		return ret!=0;
	}

	bool Image::LoadFromMem(void *pMem,UINT size)
	{
		ASSERT(m_p->m_imgID == 0);
		ilGenImages(1,&m_p->m_imgID);
		ilBindImage(m_p->m_imgID);
		ILboolean ret=ilLoadL(IL_TGA ,pMem,size);
		Member::ilDetectError(_T(""));

		return ret!=0;
	}

	bool Image::ConvertFmt(EPixelFormat pixelFmt)
	{
		ASSERT(pixelFmt>=0 && pixelFmt<EPF_Num);
		if(GetPixelFormat()==pixelFmt)
			return true;
		BYTE bpp=24;
		ILenum ilFmt=IL_RGB;
		if(!Member::ilConvert2IL(pixelFmt,bpp,ilFmt))
		{
			return false;
			Kernel::Inst()->SetLastError(_T("不支持的Image象素格式"));
		}
		return ilConvertImage(ilFmt,IL_UNSIGNED_BYTE)!=0;
	}
	
	bool Image::WriteToFile(const TCHAR *szPath)
	{
		ASSERT(m_p->m_imgID != 0);

		if(m_p->m_imgID == 0)
			return false;

		ilBindImage(m_p->m_imgID);
#ifdef _UNICODE
		ILboolean ret = ilSaveImage((const ILstring)WideToChar(szPath).c_str());
#else
		ILboolean ret = ilSaveImage((const ILstring)szPath);
#endif
		Member::ilDetectError();
		return ret!=0;
	}
	
	void Image::Destroy()
	{
		if(m_p->m_imgID == 0)
			return;

		ilDeleteImages(1,&m_p->m_imgID);
		m_p->m_imgID=0;
		Member::ilDetectError();
	}

	Image* Image::Clone()
	{
		Image *pNewImg=new Image;
		ilBindImage(m_p->m_imgID);
		pNewImg->m_p->m_imgID=ilCloneCurImage();
		//ilBindImage(pNewImg->m_p->m_imgID);
		//ilCopyImage(m_p->m_imgID);

		return pNewImg;
	}

	UINT Image::Width() const
	{
		ilBindImage(m_p->m_imgID);
		return ilGetInteger(IL_IMAGE_WIDTH);
	}

	UINT Image::Height() const
	{
		ilBindImage(m_p->m_imgID);
		return ilGetInteger(IL_IMAGE_HEIGHT);
	}

	UINT Image::GetBpp() const//bit per pixel
	{
		ilBindImage(m_p->m_imgID);
		return ilGetInteger(IL_IMAGE_BITS_PER_PIXEL );
	}
	EPixelFormat Image::GetPixelFormat() const
	{
		ilBindImage(m_p->m_imgID);
		ILint ilFmt=ilGetInteger(IL_IMAGE_FORMAT);

		EPixelFormat ret=EPF_Num;
		UINT bpp=GetBpp();
		switch(ilFmt)
		{
		case IL_BGR:
			switch(bpp)
			{
			case 16:	ret=EPF_R5G6B5;	break;
			case 24:	ret=EPF_R8G8B8;	break;
			}
			break;
		case IL_BGRA:
			switch(bpp)
			{
			case 32:	ret=EPF_A8R8G8B8; break;
			case 128:	ret=EPF_A32R32G32B32F; break;
			}
			break;
		case IL_LUMINANCE:
			ASSERT(bpp==8);
			ret=EPF_A8;
			break;
		}

		return ret;
	}

	UINT Image::GetSizeInBytes() const
	{
		return Width()*Height()*GetBpp()/8;
	}
	
	void* Image::GetRawBuffer() const
	{
		ilBindImage(m_p->m_imgID);
		return ilGetData();
	}

	void Image::DiffuseRow( UINT row,UINT startCol,UINT endCol,INT offset )
	{
		if( GetPixelFormat()!=EPF_A8R8G8B8 )
			return;

		struct tagPixel
		{
			BYTE B,G,R,A;
		};

		tagPixel *pContent = (tagPixel *)GetRawBuffer();
		int w = Width();

		for( UINT col=startCol; col<=endCol; col++ )
		{
			tagPixel *pDest = &pContent[row*w+col];
			tagPixel *pSrc = &pContent[(row+offset)*w+col];
			if( pDest->A<pSrc->A )
			{
				*pDest = *pSrc;
			}
		}
	}

	void Image::DiffuseCol( UINT col,UINT startRow,UINT endRow,INT offset )
	{
		if( GetPixelFormat()!=EPF_A8R8G8B8 )
			return;

		struct tagPixel
		{
			BYTE B,G,R,A;
		};

		tagPixel *pContent = (tagPixel *)GetRawBuffer();
		int w = Width();

		for( UINT row=startRow; row<=endRow; row++ )
		{
			tagPixel *pDest = &pContent[row*w+col];
			tagPixel *pSrc = &pContent[row*w+col+offset];
			if( pDest->A<pSrc->A )
			{
				*pDest = *pSrc;
			}
		}
	}

	void Image::Blit(Image *pSrc,int destX,int destY,int srcX,int srcY,int w,int h)
	{
		ASSERT(GetBpp()==pSrc->GetBpp());

		BYTE *pD = (BYTE *)GetRawBuffer();
		BYTE *pS = (BYTE *)pSrc->GetRawBuffer();

		int dw = Width();
		int sw = pSrc->Width();
		int dh = Height();
		int sh = pSrc->Height();
		ASSERT( destX<dw && destX+w<=dw && destY<dh && destY+h<=dh );
		ASSERT( srcX<sw && srcX+w<=sw && srcY<sh && srcY+h<=sh );
		int dstLineSize=dw*GetBpp()/8;
		int srcLineSize=sw*GetBpp()/8;
		int lineSize=w*GetBpp()/8;
		int dstXOffset=destX*GetBpp()/8;
		int srcXOffset=srcX*GetBpp()/8;

		for( int dy=destY, sy=srcY; dy<destY+h; dy++, sy++ )
		{
			FMemcpy(&pD[dy*dstLineSize+dstXOffset],&pS[sy*srcLineSize+srcXOffset],lineSize);
		}
	

/*
		ilBindImage(m_p->m_imgID);
		ilBlit(pSrc->GetID(),destX,destY,0,srcX,srcY,1,w,h,0);
#ifdef _DEBUG
		Member::ilDetectError();
#endif
		*/
	}
		
	void Image::Blit(Image *pSrc,int destX,int destY)
	{
		ilBindImage(m_p->m_imgID);
		ilOverlayImage(pSrc->GetID(),destX,destY,0);
#ifdef _DEBUG
		Member::ilDetectError();
#endif
	}

	static ILenum ILU_FILTER_SET[EIS_Num]={ILU_LINEAR,ILU_NEAREST,ILU_SCALE_TRIANGLE};
	bool Image::Scale(UINT w, UINT h, EImageFilter mode)
	{
		ilBindImage(m_p->m_imgID);
		ASSERT(mode>=0 && mode<EIS_Num);
		iluImageParameter(ILU_FILTER,ILU_FILTER_SET[mode]);
		bool ok=iluScale(w,h,1)!=0;
		Member::ilDetectError();
		return ok;
	}
	
	bool Image::Rotate(float rad, EImageFilter mode)
	{
		return NULL;
	}

	bool Image::VFlip()
	{
		ilBindImage(m_p->m_imgID);
		return iluFlipImage()!=0;
	}

	void Image::DoModifiy(EImageMod mod,int param)
	{
		ilBindImage(m_p->m_imgID);
		switch(mod)
		{
		case EMod_AverageBlur:
			iluBlurAvg(param);
			break;
		}
	}

	bool Image::Mul(Image* pOther)
	{
		ASSERT(pOther!=NULL);

		if(GetBpp()!=32
			||pOther->GetBpp()!=32)
			return false;

		if(Width()!=pOther->Width()
			||Height()!=pOther->Height())
			return false;

		tagBGRA *pD = (tagBGRA*)GetRawBuffer();
		tagBGRA *pS = (tagBGRA*)pOther->GetRawBuffer();

		Color4f dcolor;
		Color4f scolor;
		for(UINT i=0;i<Height()*Width();i++)
		{
			dcolor.A=pD->A/255.0f;
			dcolor.R=pD->R/255.0f;
			dcolor.G=pD->G/255.0f;
			dcolor.B=pD->B/255.0f;
			scolor.A=pS->A/255.0f;
			scolor.R=pS->R/255.0f;
			scolor.G=pS->G/255.0f;
			scolor.B=pS->B/255.0f;
			
			dcolor.A*=scolor.A;
			dcolor.R*=scolor.R;
			dcolor.G*=scolor.G;
			dcolor.B*=scolor.B;

			pD->A=BYTE(255.0f*dcolor.A);
			pD->R=BYTE(255.0f*dcolor.R);
			pD->G=BYTE(255.0f*dcolor.G);
			pD->B=BYTE(255.0f*dcolor.B);

			pD++;
			pS++;
		}

		return true;
	}

	BYTE _LumConstrast(BYTE color,float lum,float constrast)
	{
		float fColor=float(color)/255.0f;
		fColor=(fColor-0.5f)*constrast+0.5f+lum;

		if(fColor<0)fColor=0;
		if(fColor>1)fColor=1;

		return BYTE(fColor*255.0f);
	}


	void Image::AdjuctLumContrast(float lum,float constrast)
	{
		ilBindImage(m_p->m_imgID);
		iluGammaCorrect(lum);
		iluContrast(constrast);
	}

	void Image::Blur( UINT blurNum )
	{
		ilBindImage(m_p->m_imgID);
		iluBlurAvg(blurNum);
	}

	void Image::Blit(Image* pSrc,TCHAR channel)
	{
		struct tagPixel
		{
			BYTE B,G,R;
		};

		if(GetBpp()!=24)
			return;

		tagPixel *pD = (tagPixel *)GetRawBuffer();
		BYTE *pS = (BYTE*)pSrc->GetRawBuffer();

		ASSERT(Height()==pSrc->Height()
			&& Width()==pSrc->Width());

		for(UINT i=0;i<Height()*Width();i++)
		{
			if(channel==_T('R'))
				pD->R=*pS;
			else if(channel==_T('G'))
				pD->G=*pS;
			else if(channel==_T('B'))
				pD->B=*pS;
			pD++;
			pS++;
		}
	}

	bool Image::IsReversed()
	{
		ilBindImage(m_p->m_imgID);

		ILinfo info;
		ZeroMemory(&info,sizeof(info));
		iluGetImageInfo(&info);

		return info.Origin==IL_ORIGIN_UPPER_LEFT;
	}
}//namespace Cool3D