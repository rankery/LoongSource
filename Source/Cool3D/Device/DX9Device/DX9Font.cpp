#include "stdafx.h"
#include "DX9Font.h"
#include "DX9Device.h"
#include "..\..\Util\Exception.h"
#include "..\DrawX.h"
#include "..\StateBlock.h"

namespace Cool3D
{
	namespace Detail
	{
		DX9Font::DX9Font()
		{
			m_pD3DFont=NULL;
			m_pD3DSprite = NULL;
			m_hDC=NULL;
			ZeroMemory(&m_asciiSize[0],sizeof(m_asciiSize));
		}

		DX9Font::~DX9Font()
		{
			MyDX9Dev()->OnFontFree(this);
			SAFE_RELEASE(m_pD3DFont);
			SAFE_RELEASE(m_pD3DSprite);
		}

		void DX9Font::Create(UINT width,UINT height,UINT weight,const TCHAR*szFaceName,BOOL isItalic, BOOL bAntiAliase)
		{
			ASSERT(m_pD3DFont == NULL);//不能重复创建
			D3DXFONT_DESC font;
			memset(&font,0,sizeof(font));
			font.Height = height;
			font.Width = width;
			font.Weight = weight;
			font.MipLevels = 1;
			font.Italic = isItalic;
			font.CharSet = DEFAULT_CHARSET;
			font.OutputPrecision = OUT_DEFAULT_PRECIS;
			font.Quality = bAntiAliase ? ANTIALIASED_QUALITY : NONANTIALIASED_QUALITY;
			font.PitchAndFamily = DEFAULT_PITCH;
			_tcscpy( font.FaceName, szFaceName);

			HRESULT hr = D3DXCreateFontIndirect(DX9Dev(), &font, &m_pD3DFont);
			THROW_FAILED(hr, NULL);
			m_hDC = m_pD3DFont->GetDC();
			ASSERT(m_hDC != NULL);
			CreateAsciiSizeTab();

			hr = D3DXCreateSprite(DX9Dev(),&m_pD3DSprite);
			THROW_FAILED(hr,NULL);
		}

		void DX9Font::DrawText(const TCHAR* szString,RECT *pRect,DWORD format,DWORD color, BOOL bWriteAlpha)
		{
			m_pD3DSprite->Begin(D3DXSPRITE_ALPHABLEND);
			size_t len=_tcslen(szString);
			
			D3DXMATRIX matrix;
			D3DXMatrixIdentity(&matrix);
			D3DXMatrixTranslation(&matrix, 0.0f, 0.0f, 0.0f);
			m_pD3DSprite->SetTransform(&matrix);

			IDevice* pIDev=Device();
			StateBlock block;

			if( !bWriteAlpha )
			{
				block.SaveState(ERS_ColorWriteEnable);	
				pIDev->SetRenderState(ERS_ColorWriteEnable,COLORWRITEENABLE_BLUE|COLORWRITEENABLE_GREEN|COLORWRITEENABLE_RED);
			}

			m_pD3DFont->DrawText(m_pD3DSprite,szString,len,pRect,format,color);
			m_pD3DSprite->End();

			block.Apply();
		}

		void DX9Font::DrawText(const TCHAR *szString,UINT x,UINT y,DWORD color)
		{
			RECT rc;
			rc.left=x;
			rc.top=y;
			rc.right=Device()->GetClientWidth();
			rc.bottom=Device()->GetClientHeight();
			size_t len=_tcslen(szString);

			m_pD3DFont->DrawText(NULL,szString,len,&rc,DT_TOP|DT_LEFT,color);
		}

		void DX9Font::MeasureText(const TCHAR* szString,SIZE& out)
		{
			if(szString==NULL)
			{
				out.cx=out.cy=0;
				return;
			}
			ASSERT(m_hDC!=NULL);

			//--对获得单个ascii字符尺寸进行优化
			if( szString[1]==_T('\0') && szString[0]>=0 && szString[0]<127 )
			{
				out=m_asciiSize[szString[0]];
				return;
			}
			GetTextExtentPoint32(m_hDC,szString,_tcslen(szString),&out);
		}

		void DX9Font::PreDeviceReset()
		{
			m_pD3DFont->OnLostDevice();
			m_pD3DSprite->OnLostDevice();
		}
		void DX9Font::PostDeviceReset()
		{
			m_pD3DFont->OnResetDevice();
			m_pD3DSprite->OnResetDevice();
		}

		void DX9Font::CreateAsciiSizeTab()
		{
			TCHAR szChar[2]={0};
			for(int i=0;i<127;i++)
			{
				szChar[0]=i;
				GetTextExtentPoint32(m_hDC,szChar,1,&m_asciiSize[i]);
			}
		}
	}
}//namespace Cool3D