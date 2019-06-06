
#pragma once
#include "..\IFont.h"
#include <D3DX9.h>

namespace Cool3D
{
	namespace Detail
	{
		/**	\class DX9Font
		\brief DX9 Font
		*/
		class DX9Font : public IFont  
		{
		public:
			virtual void Create(UINT width,UINT height,UINT weight,const TCHAR*szFaceName,BOOL isItalic=FALSE, BOOL bAntiAliase=TRUE);
			virtual void DrawText(const TCHAR* szString,RECT *pRect,DWORD format,DWORD color, BOOL bWriteAlpha=FALSE);
			virtual void DrawText(const TCHAR* szString,UINT x,UINT y,DWORD color=0xFFFFFFFF);
			virtual void MeasureText(const TCHAR* szString,SIZE& out);
			DX9Font();
			virtual ~DX9Font();

			void PreDeviceReset();
			void PostDeviceReset();
		private:
			void CreateAsciiSizeTab();

			ID3DXFont	*m_pD3DFont;
			ID3DXSprite	*m_pD3DSprite;
			HDC			m_hDC;

			SIZE        m_asciiSize[127];
		};
	}
}