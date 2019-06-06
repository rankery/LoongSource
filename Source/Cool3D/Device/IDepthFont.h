#pragma once

namespace Cool3D
{

	/**	\class IDepthFont
		\brief 带有z值的字体
	*/
	class IDepthFont
	{
	public:
		IDepthFont(void);
		virtual ~IDepthFont(void);

		virtual void Create(UINT size) =0;
		virtual void Create(UINT width,UINT height,UINT weight,BOOL isItalic=FALSE,const TCHAR*szFaceName=_T("宋体"))=0;
		virtual void Begin() =0;
		virtual void DrawText(const TCHAR* szString,RECT *pRect,DWORD format,DWORD color,float z) =0;
		virtual void DrawText(const TCHAR* szString,RECT *pRect,DWORD format,DWORD color,float z,float scale) =0;
		virtual void End() =0;
		virtual void MeasureText(const TCHAR* szString,SIZE& out) =0;
	};
}//namespace Cool3D