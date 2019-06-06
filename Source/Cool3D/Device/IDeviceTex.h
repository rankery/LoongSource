#pragma once
#include "..\Cool3D.h"
#include "..\Image\Image.h"

class IFS;
namespace Cool3D
{
	enum ETexDetail
	{
		ETexDetail_High =0,
		ETexDetail_Mid = 1,
		ETexDetail_Low = 2,
	};

	class Image;
	class ImageTGA;
	/**	\class IDeviceTex
		\brief Device上的Texture对象
		\todo 现在是硬件生成mipmap,硬件不支持的话还没做
	*/
	class Cool3D_API IDeviceTex
	{
	public:
		static EImageFilter s_MipMapQuat;//!mip map产生的质量,影响mipmap生成的速度
		IDeviceTex(void);
		virtual ~IDeviceTex(void);

		/**	得到贴图对象占用的内存的大小
			\remarks 如果使用了mipmap,则为所有level的总和
		*/
		virtual UINT GetSizeInBytes() =0;
		/**	从文件读取贴图内容
			\remarks 使用hFile而不是szPath,可以忽略是从那个FileSystem读取的问题
		*/
		virtual void LoadFromFile(IFS *pFS,const TCHAR* szPath,UINT numLevels=0,bool bManagedPool=true,bool bUseDDS=true,ETexDetail texDetail=ETexDetail_High) =0;
		virtual bool WriteToFile(const TCHAR* szPath) =0;

		/**	从一个Image对象创建贴图
			\param pImg,Image对象
			\param numLevels,mipmap的等级数,如果为0,则产生所有的等级(直到1*1大小)
		*/
		virtual void CreateFromImage(Image *pImg,UINT numLevels) =0;
		/**	将Image拷贝到贴图上
		*/
		virtual void UpdateFromImage(Image *pImg,bool bUpdateMipMap) =0;

		virtual void CreateFromData(int w,int h,EPixelFormat fmt,BYTE *pImageData,int numLevels) =0;

		virtual void CreateFromJPGLump(void *pLump, int lumpSize) = 0;

		virtual UINT Width() =0;
		virtual UINT Height() =0;
		virtual EPixelFormat Format() =0;
	};
}//namespace Cool3D