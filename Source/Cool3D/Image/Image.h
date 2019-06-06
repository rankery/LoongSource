#pragma once
#include "..\Cool3D.h"
#include "..\Util\Noncopyable.h"
#include "..\Device\IDevice.h"//for PixelFormat

class IFS;
namespace Cool3D
{
	enum EImageFilter
	{
		EIS_Normal=0,
		EIS_Fastest,
		EIS_BestQuality,
		EIS_Num
	};
	
	struct tagBGRA
	{
		BYTE B,G,R,A;
	};

	struct tagBGR
	{
		BYTE B,G,R;
	};


	/**	\class Image
		\brief Image接口,一个实例对应一个Image对象
	*/
	class Cool3D_API Image : private Noncopyable
	{
	public:
		enum EImageMod
		{
			EMod_AverageBlur
		};
	public:
		Image(void);
		virtual ~Image(void);

		/**	在内存中创建Image
		*/
		bool Create(UINT w,UINT h,EPixelFormat pixelFmt);
		/**	将当前的图像转换到指定的象素格式
		*/
		bool ConvertFmt(EPixelFormat pixelFmt);
		/**	设置读取的文件系统
		*/
		void SetFS(IFS *pFS);
		/**	从文件读取数据,并按照其内容创建Image
		*/
		bool LoadFromFile(const TCHAR *szPath);
		/*	从内存创建,支持TGA
		*/
		bool LoadFromMem(void *pMem,UINT size);
		/**	把Image的内容写入文件
		*/
		bool WriteToFile(const TCHAR *szPath);
		/**	释放所占用的内存等资源
		*/
		virtual void Destroy();
		/**	creates a copy of the current image
		*/
		Image* Clone();

		UINT Width() const;
		UINT Height() const;
		UINT GetBpp() const;//bit per pixel
		EPixelFormat GetPixelFormat() const;
		UINT GetSizeInBytes() const;
		void* GetRawBuffer() const;

		/**	将指定图像的指定区域画到本图像上来
		*/
		void Blit(Image *pSrc,int destX,int destY,int srcX,int srcY,int w,int h);
		/**	将指定图像的整个图像画到本图像上来
		*/
		void Blit(Image *pSrc,int destX,int destY);
		/**	缩放当前图像
		*/
		bool Scale(UINT w, UINT h, EImageFilter mode);
		/**	旋转当前图像
		*/
		bool Rotate(float rad, EImageFilter mode);
		/**	垂直翻转当前图像
		*/
		bool VFlip();
		/** 图像起点是否是UPPER-LEFT
		*/
		bool IsReversed();
		/**	图像处理	*/
		void DoModifiy(EImageMod mod,int param);
		/** 和另一张相同尺寸的图片相乘
		*/
		bool Mul(Image* pOther);
		/** 调整亮度和对比度
		*/
		void AdjuctLumContrast(float lum,float constrast);

		void Blit(Image* pSrc,TCHAR channel);

		/**	模糊
		*/
		void Blur(UINT blurNum);

		void DiffuseRow(UINT row,UINT startCol,UINT endCol,INT offset);
		void DiffuseCol(UINT col,UINT startRow,UINT endRow,INT offset);

		//--内部方法
		UINT GetID() const;
	private:
		friend class Kernel;//只允许Kernel调用这两个函数
		/**	初始化系统,程序启动时调用一次即可 
		*/
		static void InitSys();
		/**	关闭系统,程序结束时调用一次
		*/
		static void ShutDownSys();
	private:
		class Member;
		Member *m_p;
	};
}//namespace Cool3D