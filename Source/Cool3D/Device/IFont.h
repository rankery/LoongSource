
#pragma once

namespace Cool3D
{
	/**	\class IFont
		\brief Font½Ó¿Ú
	*/
	class IFont  
	{
	public:
		IFont();
		virtual ~IFont();
		virtual void Create(UINT width,UINT height,UINT weight,const TCHAR*szFaceName,BOOL isItalic=FALSE, BOOL bAntiAliase=TRUE) =0;
		/**	»­×Ö
			\param format 
		*/
		virtual void DrawText(const TCHAR* szString,RECT *pRect,DWORD format,DWORD color, BOOL bWriteAlpha=FALSE) =0;
		virtual void DrawText(const TCHAR* szString,UINT x,UINT y,DWORD color=0xFFFFFFFF) =0;
		virtual void MeasureText(const TCHAR* szString,SIZE& out) =0;
	};
}//namespace Cool3D