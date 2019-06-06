#include "StdAfx.h"
#include ".\imagegdi.h"
#include "IL\ILut.h"

namespace Cool3D
{
	ImageGDI::ImageGDI(void)
	{
		m_hBmp=NULL;
	}

	ImageGDI::~ImageGDI(void)
	{
		Destroy();
	}

	void ImageGDI::InvalidateHBmp()
	{
		if(m_hBmp)
		{
			DeleteObject(m_hBmp);
			m_hBmp=NULL;
		}
	}

	void ImageGDI::Destroy()
	{
		InvalidateHBmp();
		Image::Destroy();
	}

	void ImageGDI::InitSys()
	{
		ilutRenderer(ILUT_WIN32);
	}

	HBITMAP ImageGDI::GetHBmp()
	{
		if(m_hBmp==NULL)//Éú³ÉHBITMAP
		{
			ilBindImage(GetID());
			m_hBmp=ilutConvertToHBitmap(::GetDC(NULL));
		}
		return m_hBmp;
	}

	void ImageGDI::DrawDC(HDC hDestDc,int destX,int destY,int srcX,int srcY,int w,int h,DWORD op)
	{
		if(GetID()==0)
			return;
		if(hDestDc==NULL)
			return;
		
		if(m_hBmp==NULL)
		{
			return;
		}

		//--
		HDC myDC=CreateCompatibleDC(hDestDc);
		if(myDC==NULL)
			return;
		HGDIOBJ oldBmp=SelectObject(myDC,m_hBmp);
		BitBlt(hDestDc,destX,destY,w,h,myDC,srcX,srcY,op);
		SelectObject(myDC,oldBmp);
		DeleteDC(myDC);
	}
}//namespace Cool3D