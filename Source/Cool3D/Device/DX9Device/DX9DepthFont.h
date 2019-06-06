#pragma once
#include "..\idepthfont.h"
#include <D3DX9.h>

namespace Cool3D
{
	namespace Detail
	{

		class DX9DepthFont : public IDepthFont
		{
		public:
			DX9DepthFont(void);
			virtual ~DX9DepthFont(void);

			virtual void Create(UINT size);
			virtual void Create(UINT width,UINT height,UINT weight,BOOL isItalic=FALSE,const TCHAR*szFaceName=_T("ËÎÌו"));
			virtual void Begin();
			virtual void DrawText(const TCHAR* szString,RECT *pRect,DWORD format,DWORD color,float z);
			virtual void DrawText(const TCHAR* szString,RECT *pRect,DWORD format,DWORD color,float z,float scale);
			virtual void End();
			virtual void MeasureText(const TCHAR* szString,SIZE& out);

			void PreDeviceReset();
			void PostDeviceReset();
		private:
			ID3DXFont	*m_pD3DFont;
			ID3DXSprite	*m_pD3DSprite;
			HDC			m_hDC;
		};
	}
}