#include "StdAfx.h"
#include ".\image32blender.h"
#include "Image.h"
#include "..\Util\Exception.h"

namespace Cool3D
{
	Image32Blender::Image32Blender(void)
	{
	}

	Image32Blender::~Image32Blender(void)
	{
	}

	void Image32Blender::AlphaBlend(Image *pDst,Image *pSrc,UINT dstx,UINT dsty)
	{
		if(pDst==NULL
			|| pSrc==NULL)
			return;
		ASSERT(pDst->GetBpp()==32);
		ASSERT(pSrc->GetBpp()==32);

		if(dstx>=pDst->Width()
			|| dsty>=pDst->Height())
			return;

		int dw=pDst->Width();
		int dh=pDst->Height();
		int sw=pSrc->Width();
		int sh=pSrc->Width();
		int w=min(dw,sw);
		int h=min(dh,sh);

		tagPixel *pDstData=(tagPixel*)pDst->GetRawBuffer();
		tagPixel *pSrcData=(tagPixel*)pSrc->GetRawBuffer();
		int x,y,t,a,inva;
		tagPixel *pDP;
		tagPixel *pSP;
		for(y=dsty;y<h;y++)
		{
			for(x=dstx;x<w;x++)
			{
				pDP=&pDstData[y*dw+x];
				pSP=&pSrcData[y*sw+x];
				a=pSP->A;
				inva=255-a;
				
				t=(pSP->R*a+pDP->R*inva)/255;
				pDP->R=t;
				t=(pSP->G*a+pDP->G*inva)/255;
				pDP->G=t;
				t=(pSP->B*a+pDP->B*inva)/255;
				pDP->B=t;
			}
		}//endof for(y);
	}

	void Image32Blender::AlphaBlend(Image *pDst,Image *pSrc,Image *pSrcAlpha,UINT dstx,UINT dsty)
	{
		if(pDst==NULL
			|| pSrc==NULL)
			return;
		ASSERT(pDst->GetBpp()==32);
		ASSERT(pSrc->GetBpp()==32);
		ASSERT(pSrcAlpha->GetPixelFormat()==EPF_A8);
		ASSERT(pSrc->Width()==pSrcAlpha->Width() && pSrc->Height()==pSrcAlpha->Height());

		if(dstx>=pDst->Width()
			|| dsty>=pDst->Height())
			return;

		int dw=pDst->Width();
		int dh=pDst->Height();
		int sw=pSrc->Width();
		int sh=pSrc->Width();
		int w=min(dw,sw);
		int h=min(dh,sh);

		tagPixel *pDstData=(tagPixel*)pDst->GetRawBuffer();
		tagPixel *pSrcData=(tagPixel*)pSrc->GetRawBuffer();
		BYTE	 *pAlphaChannel=(BYTE*)pSrcAlpha->GetRawBuffer();
		int x,y,t,a,inva;
		tagPixel *pDP;
		tagPixel *pSP;
		for(y=dsty;y<h;y++)
		{
			for(x=dstx;x<w;x++)
			{
				pDP=&pDstData[y*dw+x];
				pSP=&pSrcData[y*sw+x];
				a=pAlphaChannel[y*sw+x];
				inva=255-a;
				
				t=(pSP->R*a+pDP->R*inva)/255;
				pDP->R=t;
				t=(pSP->G*a+pDP->G*inva)/255;
				pDP->G=t;
				t=(pSP->B*a+pDP->B*inva)/255;
				pDP->B=t;
			}
		}//endof for(y);
	}

	void Image32Blender::BlendColor(Image *pImg,Color4f srcColor)
	{}

	void Image32Blender::MulColor(Image *pImg,Color4f color)
	{
		ASSERT(pImg->GetBpp()==32);
		if(color.A==1.0f
			&& color.R==1.0f
			&& color.G==1.0f
			&& color.B==1.0f)
			return;

		if(pImg==NULL)
			return;
		tagPixel *pData=(tagPixel*)pImg->GetRawBuffer();
		if(pData==NULL)
			return;
		int numPixel=pImg->Width()*pImg->Height();
		for(int i=0;i<numPixel;i++)
		{
			pData->A=BYTE(pData->A*color.A);
			pData->R=BYTE(pData->R*color.R);
			pData->G=BYTE(pData->G*color.G);
			pData->B=BYTE(pData->B*color.B);
			pData++;
		}
	}

	void Image32Blender::AddColor(Image *pImg,int a,int r,int g,int b)
	{
		ASSERT(pImg->GetBpp()==32);
		if(r==0
			&&g==0
			&&b==0)
			return;

		if(pImg==NULL)
			return;
		tagPixel *pData=(tagPixel*)pImg->GetRawBuffer();
		if(pData==NULL)
			return;
		int numPixel=pImg->Width()*pImg->Height();
		int t;
		for(int i=0;i<numPixel;i++)
		{
			t=pData->A+a;
			if(t<0) t=0; if(t>255) t=255;
			pData->A=t;

			t=pData->R+r;
			if(t<0) t=0; if(t>255) t=255;
			pData->R=t;
			
			t=pData->G+g;
			if(t<0) t=0; if(t>255) t=255;
			pData->G=t;

			t=pData->B+b;
			if(t<0) t=0; if(t>255) t=255;
			pData->B=t;
			pData++;
		}//endof for
	}

	void Image32Blender::GrayScale(Image *pImg)
	{
		if(pImg==NULL)
			return;
		ASSERT(pImg->GetBpp()==32);
		tagPixel *pData=(tagPixel*)pImg->GetRawBuffer();
		if(pData==NULL)
			return;
		int numPixel=pImg->Width()*pImg->Height();
		BYTE g;
		for(int i=0;i<numPixel;i++)
		{
			g=BYTE(pData->R*0.3f+pData->G*0.6f+pData->B*0.1f);
			pData->R=pData->G=pData->B=g;
			pData++;
		}//endof for
	}

	void Image32Blender::NoColor(Image *pImg,BYTE color)
	{
		if(pImg==NULL)
			return;
		ASSERT(pImg->GetBpp()==32);
		tagPixel *pData=(tagPixel*)pImg->GetRawBuffer();
		if(pData==NULL)
			return;
		int numPixel=pImg->Width()*pImg->Height();
		BYTE g;
		for(int i=0;i<numPixel;i++)
		{
			if( pData->R!=255 || pData->G!= 255 || pData->B!=255 )
			{
				g = color;
			}
			else
			{
				g = 255;
			}
			pData->R=pData->G=pData->B=g;
			pData++;
		}//endof for
	}

}//namespace Cool3D