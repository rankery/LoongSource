#include "StdAfx.h"
#include ".\dx9depthfont.h"
#include "DX9Device.h"
#include "..\..\Util\Exception.h"

namespace Cool3D
{
	namespace Detail
	{

		DX9DepthFont::DX9DepthFont(void)
		{
			m_pD3DFont=NULL;
			m_pD3DSprite=NULL;
			m_hDC=NULL;
		}

		DX9DepthFont::~DX9DepthFont(void)
		{
			MyDX9Dev()->OnDepthFontFree(this);

			SAFE_RELEASE(m_pD3DFont);
			SAFE_RELEASE(m_pD3DSprite);
		}

		void DX9DepthFont::Create(UINT size)
		{
			ASSERT(m_pD3DFont == NULL);//不能重复创建

			D3DXFONT_DESC font;
			memset(&font,0,sizeof(font));
			font.Height = size;
			font.Width = 0;
			font.Weight=400;
			font.MipLevels = D3DX_DEFAULT;
			font.Italic=FALSE;
			font.CharSet = DEFAULT_CHARSET;
			font.OutputPrecision = OUT_DEFAULT_PRECIS;
			font.Quality = DEFAULT_QUALITY;
			font.PitchAndFamily = DEFAULT_PITCH;
			_tcscpy( font.FaceName, _T("宋体") );

			HRESULT hr=D3DXCreateFontIndirect(DX9Dev(),
				&font,
				&m_pD3DFont);
			THROW_FAILED(hr,NULL);

			m_hDC = m_pD3DFont->GetDC();
			ASSERT(m_hDC != NULL);

			hr = D3DXCreateSprite(DX9Dev(),&m_pD3DSprite);

			THROW_FAILED(hr,NULL);
		}

		void DX9DepthFont::Create(UINT width,UINT height,UINT weight,
			BOOL isItalic/* =FALSE */,const TCHAR*szFaceName/* ="宋体" */)
		{
			ASSERT(m_pD3DFont == NULL);//不能重复创建

			D3DXFONT_DESC font;
			memset(&font,0,sizeof(font));
			font.Height = height;
			font.Width = width;
			font.Weight=weight;
			font.MipLevels = D3DX_DEFAULT;
			font.Italic=isItalic;
			font.CharSet = DEFAULT_CHARSET;
			font.OutputPrecision = OUT_DEFAULT_PRECIS;
			font.Quality = DEFAULT_QUALITY;
			font.PitchAndFamily = DEFAULT_PITCH;
			_tcscpy( font.FaceName, szFaceName);

			HRESULT hr=D3DXCreateFontIndirect(DX9Dev(),
				&font,
				&m_pD3DFont);
			THROW_FAILED(hr,NULL);

			m_hDC = m_pD3DFont->GetDC();
			ASSERT(m_hDC != NULL);

			hr = D3DXCreateSprite(DX9Dev(),&m_pD3DSprite);

			THROW_FAILED(hr,NULL);
		}

		void DX9DepthFont::Begin()
		{
			m_pD3DSprite->Begin(D3DXSPRITE_ALPHABLEND);
		}
		
		void DX9DepthFont::DrawText(const TCHAR* szString,RECT *pRect,DWORD format,DWORD color,float z)
		{
			D3DXMATRIX matrix;
			D3DXMatrixIdentity(&matrix);
			D3DXMatrixTranslation(&matrix, 0.0f, 0.0f, z);
			m_pD3DSprite->SetTransform(&matrix);

			size_t len=_tcslen(szString);
			m_pD3DFont->DrawText(m_pD3DSprite,szString,len,pRect,format,color);
		}

		void DX9DepthFont::DrawText( const TCHAR* szString,RECT *pRect,DWORD format,DWORD color,float z,float scale )
		{
			Vector2 center;
			center.x = (float)(pRect->right+pRect->left)/2.0f;
			center.y = (float)(pRect->bottom+pRect->top)/2.0f;

			D3DXMATRIX matrix;
			D3DXVECTOR2 vecScale(scale,scale);
			D3DXMatrixTransformation2D(&matrix, &center, 0.0f, &vecScale, NULL, 0.0f, NULL);
			matrix._43 = z;
			m_pD3DSprite->SetTransform(&matrix);

			size_t len=_tcslen(szString);
			m_pD3DFont->DrawText(m_pD3DSprite,szString,len,pRect,format,color);
		}

		void DX9DepthFont::End()
		{
			m_pD3DSprite->End();
		}

		void DX9DepthFont::MeasureText(const TCHAR* szString,SIZE& out)
		{
			if(szString==NULL)
			{
				out.cx=out.cy=0;
				return;
			}
			ASSERT(m_hDC!=NULL);
			GetTextExtentPoint32(m_hDC,szString,_tcslen(szString),&out);
		}

		void DX9DepthFont::PreDeviceReset()
		{
			m_pD3DSprite->OnLostDevice();
			m_pD3DFont->OnLostDevice();
		}
		void DX9DepthFont::PostDeviceReset()
		{
			m_pD3DSprite->OnResetDevice();
			m_pD3DFont->OnResetDevice();
		}
	}
}