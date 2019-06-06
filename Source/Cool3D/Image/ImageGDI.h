#pragma once
#include "image.h"
#include "..\Cool3D.h"

namespace Cool3D
{
	/**	\class ImageGDI 
		\brief 提供GDI画图接口的Image
	*/
	class Cool3D_API ImageGDI : public Image
	{
	public:
		ImageGDI(void);
		virtual ~ImageGDI(void);

		//--Image
		virtual void Destroy();
		//--
		/**	使得内部保存的bmp handle失效
			\remarks 当外部调用了Image::Scale()等函数后,需要调用此函数,以更新bmp handle
		*/
		void InvalidateHBmp();
		HBITMAP GetHBmp();
		void DrawDC(HDC hDestDc,int destX,int destY,int srcX,int srcY,int w,int h,DWORD op=SRCCOPY);

	private:
		friend class Kernel;//只允许Kernel调用这个函数
		/**	初始化系统,程序启动时调用一次即可 
		*/
		static void InitSys();
	private:
		HBITMAP	m_hBmp;
	};
}//namespace Cool3D