#pragma once
#include "..\Cool3D.h"
#include "..\Public\VertexDefine.h"
#include "..\Device\IDevice.h"

	class IFS;
namespace Cool3D
{
	class ResTexture;
	/**	\class IDraw2D
		\brief 以2D方式向屏幕绘图的功能
	*/
	class Cool3D_API IDraw2D
	{
	public:
		enum EBlendMode
		{
			EBlend_Disable, //关闭alpha混合
			EBlend_Alpha,	//混合:src.rgb*src.a+(1-src.a)*dest.rgb
		};

		enum EAlphaWriteType
		{
			EAWT_Write,     //写Alpha通道
			EAWT_NoWrite,   //不写Alpha通道
			EAWT_Add,       //Alpha通道与渲染目标相加
		};
		
		IDraw2D(void){} 
		virtual ~IDraw2D(void) {}

		static IDraw2D* Inst();

		/**	创建,在系统创建时调用一次,分配本系统需要的所有资源
			\param 所使用的Texture Pool的最大容量
		*/
		virtual void Create(UINT texPoolSize=1024*1024*4) =0;
		/**	销毁,在程序结束时调用一次
		*/
		virtual void Destroy() =0;

		/**	得到一个Texture对象的指针
		*/
		virtual ResTexture *NewTexture(const TCHAR *szPath,IFS *pFS=NULL) =0;
		/**	释放指定的Texture对象
		*/
		virtual void ReleaseTexture(ResTexture *pTex) =0;

		/** Textrue Pool 垃圾收集*/
		virtual void TextureGC() =0;
		/** 得到Texture Pool 大小
		*/
		virtual UINT GetTexPoolSize() =0;

		/** 开始Draw操作，设置渲染状态
			\remark 请尽可能关闭alpha混合和zBuffer，以降低填充率
		*/
		virtual void BeginDraw(EBlendMode blendMode,bool bZEnable=false) =0;
		/** 结束Draw操作，恢复渲染状态
		*/
		virtual void EndDraw() =0;

		/** 将贴图绘制到屏幕指定区域
		*/
		virtual void Draw(const RECT* pDestRect,const RECT* pSrcRect,ResTexture* pTex,DWORD mulColor=0xFFFFFFFF,ETextrueFilter filter=ETFilter_POINT,float fAngle=0,EAlphaWriteType eAlphaWriteType=EAWT_Write, float z=1.0f)=0;
		virtual void Draw(const RECT* pDestRect,const RECT* pSrcRect,IRenderTexture* pTex,DWORD mulColor=0xFFFFFFFF,ETextrueFilter filter=ETFilter_POINT,float fAngle=0,float z=1.0f)=0;



		// 特殊Draw方法 ------------------------------------------------------------
		// 将贴图绘制到屏幕指定区域，alpha通道从alphaMap获取 (minimap)
		virtual void Draw(const RECT* pDestRect,const RECT* pSrcRectBaseTex,const RECT* pSrcRectAlphaMap,ResTexture* pBaseTex,ResTexture* pAlphaMap,ETextrueFilter filter=ETFilter_POINT,DWORD mulColor=0xFFFFFFFF) =0;
		virtual void Draw(const RECT* pDestRect,const RECT* pSrcRectBaseTex,const RECT* pSrcRectAlphaMap,IRenderTexture* pBaseTex,ResTexture* pAlphaMap,ETextrueFilter filter=ETFilter_POINT,DWORD mulColor=0xFFFFFFFF) =0;
		// 将贴图绘制到屏幕指定区域，可指定Tex2的AlphaOp (minimap)
		void virtual Draw(const RECT* pDestRect,ResTexture* pTex1,IRenderTexture* pTex2,const RECT* pSrcRect1,const RECT* pSrcRect2,ETextureOp eAlphaOp) = 0;
		// 画图元 (CoolDown Button)
		virtual void Draw(EDrawType eType, LPPOINT lpPoints, int nVerCount, DWORD color, EAlphaWriteType eAlphaWriteType)=0;
		// -------------------------------------------------------------------------

		/** 画线
		*/
		virtual void DrawLine(int sx,int sy,int ex,int ey,DWORD color) =0;
		
		/** 将TexturePool信息输出到日志文件
		*/
		virtual void Report() =0;
		/** 在屏幕上显示TexturePool大小
		*/
		virtual void DrawInfo(int x,int y) =0;

	};
}//namespace Cool3D