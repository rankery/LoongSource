#pragma once
#include "idraw2d.h"
#include "..\ResSys\ResPool.h"
#include "..\Device\StateBlock.h"

namespace Cool3D
{

	class IRenderTexture;
	class Draw2DImp : public IDraw2D
	{
	public:
		Draw2DImp(void);
		virtual ~Draw2DImp(void);

		//--IDraw2D
		virtual void Create(UINT texPoolSize=1024*1024*4);
		virtual void Destroy();

		virtual ResTexture *NewTexture(const TCHAR *szPath,IFS *pFS=NULL);
		virtual void ReleaseTexture(ResTexture *pTex);
		virtual void TextureGC();
		virtual UINT GetTexPoolSize();

		virtual void BeginDraw(EBlendMode blendMode,bool bZEnable=false);
		virtual void EndDraw();

		/** 将贴图绘制到屏幕指定区域
		*/
		virtual void Draw(const RECT* pDestRect,const RECT* pSrcRect,ResTexture* pTex,DWORD mulColor=0xFFFFFFFF,ETextrueFilter filter=ETFilter_POINT,float fAngle=0,EAlphaWriteType eAlphaWriteType=EAWT_Write, float z=1.0f);
		virtual void Draw(const RECT* pDestRect,const RECT* pSrcRect,IRenderTexture* pTex,DWORD mulColor=0xFFFFFFFF,ETextrueFilter filter=ETFilter_POINT,float fAngle=0,float z=1.0f);



		// 特殊Draw方法 ------------------------------------------------------------
		// 将贴图绘制到屏幕指定区域，alpha通道从alphaMap获取 (minimap)
		virtual void Draw(const RECT* pDestRect,const RECT* pSrcRectBaseTex,const RECT* pSrcRectAlphaMap,ResTexture* pBaseTex,ResTexture* pAlphaMap,ETextrueFilter filter=ETFilter_POINT,DWORD mulColor=0xFFFFFFFF);
		virtual void Draw(const RECT* pDestRect,const RECT* pSrcRectBaseTex,const RECT* pSrcRectAlphaMap,IRenderTexture* pBaseTex,ResTexture* pAlphaMap,ETextrueFilter filter=ETFilter_POINT,DWORD mulColor=0xFFFFFFFF);

		// 将贴图绘制到屏幕指定区域，可指定Tex2的AlphaOp (minimap)
		virtual void Draw(const RECT* pDestRect,ResTexture* pTex1,IRenderTexture* pTex2,const RECT* pSrcRect1,const RECT* pSrcRect2,ETextureOp eAlphaOp);

		// 画图元 (CoolDown Button)
		virtual void Draw(EDrawType eType, LPPOINT lpPoints, int nVerCount, DWORD color, EAlphaWriteType eAlphaWriteType);
		// -------------------------------------------------------------------------

		virtual void DrawLine(int sx,int sy,int ex,int ey,DWORD color);

		virtual void Report();
		virtual void DrawInfo(int x,int y);

	protected:
		void MakeQuadVert(Vert_PtDT* pQuadVert,const RECT* pDestRect,const RECT* pSrcRect,UINT nW, UINT nH, DWORD mulColor,float fAngle=0, float z=1.0f);
		void MakeQuadVert(Vert_PtDT2* pQuadVert,const RECT* pDestRect,const RECT* pSrcRect1,const RECT* pSrcRect2,UINT nTexW1,UINT nTexH1,UINT nTexW2,UINT nTexH2,DWORD mulColor);
		
		void RotateQuad( Vert_PtDT* pQuadVert, const float fAngle );
	private:
		ResPool<ResTexture>		*m_texPool;
		StateBlock				m_stateBlock;
		bool					m_bBeginDraw;
	};
}//namespace Cool3D