#pragma once
#include "..\Math\Color.h"

namespace Cool3D
{
	class Image;
	/**	\class Image32Blender
		\将两个32bit的image混合起来
	*/
	class Image32Blender
	{
	public:
		struct tagPixel
		{
			BYTE B,G,R,A;
		};
	public:
		Image32Blender(void);
		~Image32Blender(void);

		/**将src image混合到dst image的指定位置上*/
		static void AlphaBlend(Image *pDst,Image *pSrc,UINT dstx,UINT dsty);
		/**将src image使用指定的alpha 通道混合到dst image的指定位置上*/
		static void AlphaBlend(Image *pDst,Image *pSrc,Image *pSrcAlpha,UINT dstx,UINT dsty);
		/**将指定的Image乘以指定的颜色*/
		static void MulColor(Image *pImg,Color4f color);
		/**将指定的Image加上指定的颜色*/
		static void AddColor(Image *pImg,int a,int r,int g,int b);
		/**与指定的颜色进行alpha混合*/
		static void BlendColor(Image *pImg,Color4f srcColor);
		/** 将指定的图像转换成灰度图像	*/
		static void GrayScale(Image *pImg);
		/** 将指定的图像转换成单色(只有白和指定颜色共两种颜色)图像 */
		static void NoColor(Image *pImg,BYTE color);
	};
}//namespace Cool3D