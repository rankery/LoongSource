#include "StdAfx.h"
#include ".\draw2dImp.h"
#include "..\Util\Exception.h"
#include "..\Device\IDevice.h"
#include "..\Device\IDeviceTex.h"
#include "..\Device\IRenderTexture.h"
#include "..\Device\IStreamDefine.h"
#include "..\Kernel.h"
#include "..\ResSys\ResTexture.h"
#include "..\RenderSys\RenderSys.h"
#include "..\Device\DrawX.h"

namespace Cool3D
{
	//---------------------------------------
	// DX9中的纹理UV计算时附加了0.5的误差计算,
	// 所以在正确计算UV时需附加此误差计算,
	// 以屏幕区域附加时需将区域宽度附加-0.5
	// 以纹理像素附加时需将UV +或- 0.5/纹理宽度
	const float Draw2D_Tex_Offset=0.5f;

	Draw2DImp::Draw2DImp(void)
	{
		m_texPool=NULL;
		m_bBeginDraw=false;
	}

	Draw2DImp::~Draw2DImp(void)
	{
		Destroy();
	}
	
	void Draw2DImp::Create(UINT texPoolSize)
	{
		Destroy();
		m_texPool=new ResPool<ResTexture>(texPoolSize);
	}
	
	void Draw2DImp::Destroy()
	{
		SAFE_DELETE(m_texPool);
	}

	ResTexture *Draw2DImp::NewTexture(const TCHAR *szPath,IFS *pFS)
	{
		DWORD dwParam=MAKELONG(1,ETexDetail_High);
		return m_texPool->NewRes(szPath,pFS,dwParam,false);
	}
	
	void Draw2DImp::ReleaseTexture(ResTexture *pTex)
	{
		m_texPool->DelRes(pTex);
	}

	UINT Draw2DImp::GetTexPoolSize()
	{
		if(m_texPool==NULL)
			return 0;
		return m_texPool->GetCurSize();
	}

	void Draw2DImp::TextureGC()
	{
		if(m_texPool)
			m_texPool->DoGC();
	}

	void Draw2DImp::BeginDraw(EBlendMode blendMode,bool bZEnable)
	{
		ASSERT(!m_bBeginDraw);

		m_stateBlock.Clear();

		IDevice* pDev=Device();
		RenderSys* pSys=Kernel::Inst()->GetRenderSys();

		//--设置渲染状态
		if(blendMode==EBlend_Alpha)
		{
			m_stateBlock.SaveState(ERS_AlphaBlendEnable);	pDev->EnableAlphaBlend(true);
			m_stateBlock.SaveState(ERS_SrcBlend);			
			m_stateBlock.SaveState(ERS_DstBlend);			pDev->SetAlphaBlend(EBlend_SrcAlpha,EBlend_InvSrcAlpha);
		}

		m_stateBlock.SaveState(ERS_DepthTestEnable);	pDev->EnableDepthTest(bZEnable);
		m_stateBlock.SaveState(ERS_DepthWriteEnable);	pDev->EnableDepthWrite(bZEnable);
		m_stateBlock.SaveState(ERS_CullMode);			pDev->SetCullMode(ECull_None);
		m_stateBlock.SaveState(ERS_FLightEnable);		pDev->FEnableLight(false);
		m_stateBlock.SaveState(ERS_FFogEnable);			pDev->FEnableFog(false);

		//--StreamDefine
		pDev->SetStreamDefine(pSys->GetStreamDefine(EVType_PtDT));

		m_bBeginDraw=true;
	}

	void Draw2DImp::EndDraw()
	{
		ASSERT(m_bBeginDraw);

		m_stateBlock.Apply();
		m_bBeginDraw=false;
	}

	void Draw2DImp::Draw(const RECT* pDestRect,const RECT* pSrcRect,ResTexture* pTex,DWORD mulColor, ETextrueFilter filter, float fAngle,EAlphaWriteType eAlphaWriteType, float z)
	{
		IDeviceTex* pTexture = pTex->GetDeviceTex();
		IDevice* pDev=Device();

		pDev->SetTexture(0, pTexture);
		pDev->SetSimplerFilter(0, filter, filter);
		pDev->SetSimplerMipFilter(0, ETFilter_NONE);
		pDev->SetSimplerAddress(0, ETAddress_Wrap, ETAddress_Wrap, ETAddress_Wrap);

		//--set pixel stage
		if( mulColor==0xFFFFFFFF )
		{
			pDev->FSetTextureColorOp(0,ETOP_SelectArg1,ETA_Texture,ETA_Diffuse);
			pDev->FSetTextureAlphaOp(0,ETOP_SelectArg1,ETA_Texture,ETA_Diffuse);
		}
		else
		{
			pDev->FSetTextureColorOp(0,ETOP_Mul,ETA_Texture,ETA_Diffuse);
			pDev->FSetTextureAlphaOp(0,ETOP_Mul,ETA_Texture,ETA_Diffuse);
		}

		pDev->FSetTextureColorOp(1,ETOP_Disable);
		pDev->FSetTextureAlphaOp(1,ETOP_Disable);

		//--make vert
		Vert_PtDT vert[4];
		MakeQuadVert(vert,pDestRect,pSrcRect,pTexture->Width(), pTexture->Height(),mulColor,fAngle, z);

		StateBlock block;
		if(EAWT_Write!=eAlphaWriteType)
		{
			block.SaveState(ERS_ColorWriteEnable);
			pDev->SetRenderState(ERS_ColorWriteEnable, COLORWRITEENABLE_BLUE|COLORWRITEENABLE_GREEN|COLORWRITEENABLE_RED);
			if(EAWT_Add==eAlphaWriteType)
			{
				pDev->DrawMem(EDraw_TriangleFan, 2, &vert, sizeof(Vert_PtDT));
				pDev->SetRenderState(ERS_ColorWriteEnable, COLORWRITEENABLE_ALPHA);
				block.SaveState(ERS_SrcBlend);			
				block.SaveState(ERS_DstBlend);
				pDev->SetAlphaBlend(EBlend_One,EBlend_One);
			}
		}
		pDev->DrawMem(EDraw_TriangleFan, 2, &vert, sizeof(Vert_PtDT));
		block.Apply();
	}

	// renderTarget渲染到后台缓冲
	void Draw2DImp::Draw(const RECT* pDestRect,const RECT* pSrcRect,IRenderTexture* pTex,DWORD mulColor,ETextrueFilter filter,float fAngle,float z)
	{
		IDevice* pDev=Device();

		pDev->SetTexture(0, pTex);
		pDev->SetSimplerFilter(0, filter, filter);
		pDev->SetSimplerMipFilter(0, ETFilter_NONE);
		pDev->SetSimplerAddress(0, ETAddress_Wrap, ETAddress_Wrap, ETAddress_Wrap);

		//--set pixel stage
		if( mulColor==0xFFFFFFFF )
		{
			pDev->FSetTextureColorOp(0,ETOP_SelectArg1,ETA_Texture,ETA_Diffuse);
			pDev->FSetTextureAlphaOp(0,ETOP_SelectArg1,ETA_Texture,ETA_Diffuse);
		}
		else
		{
			pDev->FSetTextureColorOp(0,ETOP_Mul,ETA_Texture,ETA_Diffuse);
			pDev->FSetTextureAlphaOp(0,ETOP_Mul,ETA_Texture,ETA_Diffuse);
		}

		pDev->FSetTextureColorOp(1,ETOP_Disable);
		pDev->FSetTextureAlphaOp(1,ETOP_Disable);

		//--make vert
		Vert_PtDT vert[4];
		MakeQuadVert(vert,pDestRect,pSrcRect,pTex->Width(), pTex->Height(),mulColor,fAngle,z);

		pDev->DrawMem(EDraw_TriangleFan, 2, &vert, sizeof(Vert_PtDT));
	}

	void Draw2DImp::MakeQuadVert(Vert_PtDT* pQuadVert,const RECT* pDestRect,const RECT* pSrcRect,UINT nW, UINT nH, DWORD mulColor,float fAngle, float z)
	{
		/*	0____1
			|    |
			3____2 	*/
		pQuadVert[0].pos.x = pDestRect->left-Draw2D_Tex_Offset;
		pQuadVert[0].pos.y = pDestRect->top-Draw2D_Tex_Offset;
		pQuadVert[1].pos.x = pDestRect->right-Draw2D_Tex_Offset;
		pQuadVert[2].pos.y = pDestRect->bottom-Draw2D_Tex_Offset;
		pQuadVert[3].pos.x = pQuadVert[0].pos.x;
		pQuadVert[1].pos.y = pQuadVert[0].pos.y;
		pQuadVert[2].pos.x = pQuadVert[1].pos.x;
		pQuadVert[3].pos.y = pQuadVert[2].pos.y;

		pQuadVert[0].uv.x = float(pSrcRect->left)/float(nW);
		pQuadVert[0].uv.y = float(pSrcRect->top)/float(nH);
		pQuadVert[1].uv.x = float(pSrcRect->right)/float(nW);
		pQuadVert[2].uv.y = float(pSrcRect->bottom)/float(nH);
		pQuadVert[3].uv.x = pQuadVert[0].uv.x;
		pQuadVert[1].uv.y = pQuadVert[0].uv.y;
		pQuadVert[2].uv.x = pQuadVert[1].uv.x;
		pQuadVert[3].uv.y = pQuadVert[2].uv.y;

		pQuadVert[0].diffuse = mulColor;
		pQuadVert[0].pos.z = z;
		pQuadVert[0].pos.w = 1;
		pQuadVert[1].diffuse = mulColor;
		pQuadVert[1].pos.z = z;
		pQuadVert[1].pos.w = 1;
		pQuadVert[2].diffuse = mulColor;
		pQuadVert[2].pos.z = z;
		pQuadVert[2].pos.w = 1;
		pQuadVert[3].diffuse = mulColor;
		pQuadVert[3].pos.z = z;
		pQuadVert[3].pos.w = 1;

		if( fAngle != 0 )
			RotateQuad( pQuadVert, fAngle );
	}


	void Draw2DImp::Draw( const RECT* pDestRect,const RECT* pSrcRectBaseTex, const RECT* pSrcRectAlphaMap, ResTexture* pBaseTex,ResTexture* pAlphaMap,ETextrueFilter filter,DWORD mulColor)
	{
		ASSERT(m_bBeginDraw);

		if( NULL == pBaseTex || NULL == pAlphaMap )
			return;

		IDevice* pDev=Device();

		//--set base texture
		pDev->SetTexture(0,pBaseTex->GetDeviceTex());
		pDev->SetSimplerFilter(0,filter,filter);
		pDev->SetSimplerMipFilter(0,ETFilter_NONE);
		pDev->SetSimplerAddress(0,ETAddress_Wrap,ETAddress_Wrap,ETAddress_Wrap);

		//--set alpha map
		pDev->SetTexture(1,pAlphaMap->GetDeviceTex());
		pDev->SetSimplerFilter(1,filter,filter);
		pDev->SetSimplerMipFilter(1,ETFilter_NONE);
		pDev->SetSimplerAddress(1,ETAddress_Wrap,ETAddress_Wrap,ETAddress_Wrap);

		//--set pixel stage
		if( mulColor==0xFFFFFFFF )
		{
			pDev->FSetTextureColorOp(0,ETOP_SelectArg1,ETA_Texture,ETA_Diffuse);
			pDev->FSetTextureAlphaOp(0,ETOP_SelectArg1,ETA_Current,ETA_Diffuse);
			pDev->FSetTextureColorOp(1,ETOP_SelectArg1,ETA_Current,ETA_Diffuse);
			pDev->FSetTextureAlphaOp(1,ETOP_SelectArg1,ETA_Texture,ETA_Diffuse);
		}
		else
		{
			pDev->FSetTextureColorOp(0,ETOP_Mul,ETA_Texture,ETA_Diffuse);
			pDev->FSetTextureAlphaOp(0,ETOP_SelectArg1,ETA_Current,ETA_Diffuse);
			pDev->FSetTextureColorOp(1,ETOP_SelectArg1,ETA_Current,ETA_Diffuse);
			pDev->FSetTextureAlphaOp(1,ETOP_Mul,ETA_Texture,ETA_Diffuse);
		}
		pDev->FSetTextureColorOp(2,ETOP_Disable);
		pDev->FSetTextureAlphaOp(2,ETOP_Disable);

		pDev->FSetTexCoordSrc( 0, 0 );
		pDev->FSetTexCoordSrc( 1, 1 );
		pDev->FSetUVTrans( 0, EUVT_Disable );
		pDev->FSetUVTrans( 1, EUVT_Disable );

		//--make vert
		Vert_PtDT2 vert[4];
		
		UINT nBaseTexW = pBaseTex->GetDeviceTex()->Width();
		UINT nBaseTexH = pBaseTex->GetDeviceTex()->Height();
		UINT nAlphaMapW = pAlphaMap->GetDeviceTex()->Width();
		UINT nAlphaMapH = pAlphaMap->GetDeviceTex()->Height();
		MakeQuadVert(vert,pDestRect,pSrcRectBaseTex,pSrcRectAlphaMap,nBaseTexW,nBaseTexH,nAlphaMapW,nAlphaMapH,mulColor);

		pDev->SetStreamDefine(Kernel::Inst()->GetRenderSys()->GetStreamDefine(EVType_PtDT2));

		//--draw
		pDev->DrawMem(EDraw_TriangleFan,2,&vert,sizeof(Vert_PtDT2));

		pDev->SetStreamDefine(Kernel::Inst()->GetRenderSys()->GetStreamDefine(EVType_PtDT));

	}

	void Draw2DImp::Draw( const RECT* pDestRect,const RECT* pSrcRectBaseTex, const RECT* pSrcRectAlphaMap, IRenderTexture* pBaseTex,ResTexture* pAlphaMap,ETextrueFilter filter, DWORD mulColor )
	{
		ASSERT(m_bBeginDraw);

		if( NULL == pBaseTex || NULL == pAlphaMap )
			return;

		IDevice* pDev=Device();

		//--set base texture
		pDev->SetTexture(0,pBaseTex);
		pDev->SetSimplerFilter(0,filter,filter);
		pDev->SetSimplerMipFilter(0,ETFilter_NONE);
		pDev->SetSimplerAddress(0,ETAddress_Wrap,ETAddress_Wrap,ETAddress_Wrap);

		//--set alpha map
		pDev->SetTexture(1,pAlphaMap->GetDeviceTex());
		pDev->SetSimplerFilter(1,filter,filter);
		pDev->SetSimplerMipFilter(1,ETFilter_NONE);
		pDev->SetSimplerAddress(1,ETAddress_Wrap,ETAddress_Wrap,ETAddress_Wrap);

		//--set pixel stage
		if( mulColor==0xFFFFFFFF )
		{
			pDev->FSetTextureColorOp(0,ETOP_SelectArg1,ETA_Texture,ETA_Diffuse);
			pDev->FSetTextureAlphaOp(0,ETOP_SelectArg1,ETA_Current,ETA_Diffuse);
			pDev->FSetTextureColorOp(1,ETOP_SelectArg1,ETA_Current,ETA_Diffuse);
			pDev->FSetTextureAlphaOp(1,ETOP_SelectArg1,ETA_Texture,ETA_Diffuse);
		}
		else
		{
			pDev->FSetTextureColorOp(0,ETOP_Mul,ETA_Texture,ETA_Diffuse);
			pDev->FSetTextureAlphaOp(0,ETOP_SelectArg1,ETA_Current,ETA_Diffuse);
			pDev->FSetTextureColorOp(1,ETOP_SelectArg1,ETA_Current,ETA_Diffuse);
			pDev->FSetTextureAlphaOp(1,ETOP_Mul,ETA_Texture,ETA_Diffuse);
		}
		pDev->FSetTextureColorOp(2,ETOP_Disable);
		pDev->FSetTextureAlphaOp(2,ETOP_Disable);

		pDev->FSetTexCoordSrc( 0, 0 );
		pDev->FSetTexCoordSrc( 1, 1 );
		pDev->FSetUVTrans( 0, EUVT_Disable );
		pDev->FSetUVTrans( 1, EUVT_Disable );

		//--make vert
		Vert_PtDT2 vert[4];

		UINT nBaseTexW = pBaseTex->Width();
		UINT nBaseTexH = pBaseTex->Height();
		UINT nAlphaMapW = pAlphaMap->GetDeviceTex()->Width();
		UINT nAlphaMapH = pAlphaMap->GetDeviceTex()->Height();
		MakeQuadVert(vert,pDestRect,pSrcRectBaseTex,pSrcRectAlphaMap,nBaseTexW,nBaseTexH,nAlphaMapW,nAlphaMapH,mulColor);

		pDev->SetStreamDefine(Kernel::Inst()->GetRenderSys()->GetStreamDefine(EVType_PtDT2));

		//--draw
		pDev->DrawMem(EDraw_TriangleFan,2,&vert,sizeof(Vert_PtDT2));

		pDev->SetStreamDefine(Kernel::Inst()->GetRenderSys()->GetStreamDefine(EVType_PtDT));

	}

	void Draw2DImp::Draw(const RECT* pDestRect,ResTexture* pTex1,IRenderTexture* pTex2,const RECT* pSrcRect1,const RECT* pSrcRect2,ETextureOp eAlphaOp)
	{
		ASSERT(m_bBeginDraw);

		if( NULL == pTex1 || NULL == pTex2 )
			return;

		IDevice* pDev=Device();

		pDev->SetTexture( 0, pTex1->GetDeviceTex() );
		pDev->SetSimplerFilter(0,ETFilter_POINT,ETFilter_POINT);
		pDev->SetSimplerMipFilter(0,ETFilter_NONE);
		pDev->SetSimplerAddress(0,ETAddress_Wrap,ETAddress_Wrap,ETAddress_Wrap);

		pDev->SetTexture(1,pTex2);
		pDev->SetSimplerFilter(1,ETFilter_POINT,ETFilter_POINT);
		pDev->SetSimplerMipFilter(1,ETFilter_NONE);
		pDev->SetSimplerAddress(1,ETAddress_Wrap,ETAddress_Wrap,ETAddress_Wrap);

		pDev->FSetTextureColorOp(0,ETOP_Mul,ETA_Texture,ETA_Diffuse);
		pDev->FSetTextureAlphaOp(0,ETOP_Mul,ETA_Texture,ETA_Diffuse);
		pDev->FSetTextureColorOp(1,ETOP_BlendCurrentAlpha,ETA_Current,ETA_Texture);
		pDev->FSetTextureAlphaOp(1,eAlphaOp,ETA_Texture,ETA_Current);
		pDev->FSetTextureColorOp(2,ETOP_Disable);
		pDev->FSetTextureAlphaOp(2,ETOP_Disable);

		pDev->FSetTexCoordSrc( 0, 0 );
		pDev->FSetTexCoordSrc( 1, 1 );
		pDev->FSetUVTrans( 0, EUVT_Disable );
		pDev->FSetUVTrans( 1, EUVT_Disable );

		//--make vert
		Vert_PtDT2 vert[4];

		UINT nTexW1 = pTex1->GetDeviceTex()->Width();
		UINT nTexH1 = pTex1->GetDeviceTex()->Height();
		UINT nTexW2 = pTex2->Width();
		UINT nTexH2 = pTex2->Height();
		MakeQuadVert(vert,pDestRect,pSrcRect1,pSrcRect2,nTexW1,nTexH1,nTexW2,nTexH2,0xFFFFFFFF);

		pDev->SetStreamDefine(Kernel::Inst()->GetRenderSys()->GetStreamDefine(EVType_PtDT2));

		//--draw
		pDev->DrawMem(EDraw_TriangleFan,2,&vert,sizeof(Vert_PtDT2));

		pDev->SetStreamDefine(Kernel::Inst()->GetRenderSys()->GetStreamDefine(EVType_PtDT));
	}


	void Draw2DImp::Draw( EDrawType eType, LPPOINT lpPoints, int nVerCount, DWORD color, EAlphaWriteType eAlphaWriteType )
	{
		IDevice *pDev=Device();
		CONST INT MAX_VERTEX_COUNT = 64;
		Vert_PtD vetex[MAX_VERTEX_COUNT];

		ASSERT(nVerCount < MAX_VERTEX_COUNT);

		for( int i=0; i<nVerCount; i++ ) 
		{
			vetex[i].diffuse=color;			
			vetex[i].pos.x= (FLOAT)lpPoints[i].x;
			vetex[i].pos.y= (FLOAT)lpPoints[i].y;
			vetex[i].pos.z=1;
			vetex[i].pos.w=1;
		}

		pDev->FSetTextureColorOp(0,ETOP_SelectArg1,ETA_Diffuse,ETA_Diffuse);
		pDev->FSetTextureAlphaOp(0,ETOP_SelectArg1,ETA_Diffuse,ETA_Diffuse);
		pDev->FSetTextureColorOp(1,ETOP_Disable,ETA_Diffuse,ETA_Diffuse);
		pDev->FSetTextureAlphaOp(1,ETOP_Disable,ETA_Diffuse,ETA_Diffuse);


		UINT PrimitiveCount = 0;
		// 根据顶点数，计算图元数量
		switch( eType )
		{
		case	EDraw_PointList:	PrimitiveCount = nVerCount;			break;
		case	EDraw_LineList:		PrimitiveCount = nVerCount/2;		break;
		case	EDraw_LineStrip:	PrimitiveCount = nVerCount-1;		break;
		case	EDraw_TriangleList:	PrimitiveCount = nVerCount/3;		break;
		case	EDraw_TriangleStrip:PrimitiveCount = nVerCount-2;		break;
		case	EDraw_TriangleFan:	PrimitiveCount = nVerCount-2;		break;
		}
		ASSERT( PrimitiveCount > 0 );

		pDev->SetStreamDefine(Kernel::Inst()->GetRenderSys()->GetStreamDefine(EVType_PtD));

		StateBlock block;
		if(EAWT_Write!=eAlphaWriteType)
		{
			block.SaveState(ERS_ColorWriteEnable);
			pDev->SetRenderState(ERS_ColorWriteEnable, COLORWRITEENABLE_BLUE|COLORWRITEENABLE_GREEN|COLORWRITEENABLE_RED);
			if(EAWT_Add==eAlphaWriteType)
			{
				pDev->DrawMem(eType,PrimitiveCount,vetex,sizeof(Vert_PtD));
				pDev->SetRenderState(ERS_ColorWriteEnable, COLORWRITEENABLE_ALPHA);
				block.SaveState(ERS_SrcBlend);			
				block.SaveState(ERS_DstBlend);
				pDev->SetAlphaBlend(EBlend_One,EBlend_One);
			}
		}
		pDev->DrawMem(eType,PrimitiveCount,vetex,sizeof(Vert_PtD));
		block.Apply();

		pDev->SetStreamDefine(Kernel::Inst()->GetRenderSys()->GetStreamDefine(EVType_PtDT));
	}


	void Draw2DImp::MakeQuadVert(Vert_PtDT2* pQuadVert,const RECT* pDestRect,const RECT* pSrcRect1,const RECT* pSrcRect2,UINT nTexW1,UINT nTexH1,UINT nTexW2,UINT nTexH2,DWORD mulColor)
	{
		/*
		0____1
		|    |
		3____2
		*/
		pQuadVert[0].pos.x=pDestRect->left-Draw2D_Tex_Offset;
		pQuadVert[0].pos.y=pDestRect->top-Draw2D_Tex_Offset;
		pQuadVert[1].pos.x=pDestRect->right-Draw2D_Tex_Offset;
		pQuadVert[1].pos.y=pDestRect->top-Draw2D_Tex_Offset;
		pQuadVert[2].pos.x=pDestRect->right-Draw2D_Tex_Offset;
		pQuadVert[2].pos.y=pDestRect->bottom-Draw2D_Tex_Offset;
		pQuadVert[3].pos.x=pDestRect->left-Draw2D_Tex_Offset;
		pQuadVert[3].pos.y=pDestRect->bottom-Draw2D_Tex_Offset;

		ASSERT(nTexW1>0 && nTexH1>0 && nTexW2>0 && nTexH2>0);
		float fInvTexW1 = 1.0f / nTexW1;
		float fInvTexH1 = 1.0f / nTexH1;
		float fInvTexW2 = 1.0f / nTexW2;
		float fInvTexH2 = 1.0f / nTexH2;

		pQuadVert[0].uv.x=float(pSrcRect1->left)   * fInvTexW1;
		pQuadVert[0].uv.y=float(pSrcRect1->top)    * fInvTexH1;
		pQuadVert[1].uv.x=float(pSrcRect1->right)  * fInvTexW1;
		pQuadVert[1].uv.y=float(pSrcRect1->top)    * fInvTexH1;
		pQuadVert[2].uv.x=float(pSrcRect1->right)  * fInvTexW1;
		pQuadVert[2].uv.y=float(pSrcRect1->bottom) * fInvTexH1;
		pQuadVert[3].uv.x=float(pSrcRect1->left)   * fInvTexW1;
		pQuadVert[3].uv.y=float(pSrcRect1->bottom) * fInvTexH1;

		pQuadVert[0].uv2.x=float(pSrcRect2->left)   * fInvTexW2;
		pQuadVert[0].uv2.y=float(pSrcRect2->top)    * fInvTexH2;
		pQuadVert[1].uv2.x=float(pSrcRect2->right)  * fInvTexW2;
		pQuadVert[1].uv2.y=float(pSrcRect2->top)    * fInvTexH2;
		pQuadVert[2].uv2.x=float(pSrcRect2->right)  * fInvTexW2;
		pQuadVert[2].uv2.y=float(pSrcRect2->bottom) * fInvTexH2;
		pQuadVert[3].uv2.x=float(pSrcRect2->left)   * fInvTexW2;
		pQuadVert[3].uv2.y=float(pSrcRect2->bottom) * fInvTexH2;

		for(int i=0;i<4;i++)
		{
			pQuadVert[i].pos.z=1.0f;
			pQuadVert[i].pos.w=1.0f;
			pQuadVert[i].diffuse = mulColor;
		}
	}

	void Draw2DImp::DrawLine(int sx,int sy,int ex,int ey,DWORD color)
	{
		Vert_PtDT vert[2];
		vert[0].diffuse=color;
		vert[0].pos=Vector4((float)sx,(float)sy,0.0f,0.5f);
		vert[0].uv=Vector2(0,0);
		vert[1].diffuse=color;
		vert[1].pos=Vector4((float)ex,(float)ey,0.0f,0.5f);
		vert[1].uv=Vector2(0,0);

		IDevice* pDev=Device();

		pDev->FSetTextureColorOp(0,ETOP_SelectArg2,ETA_Texture,ETA_Diffuse);
		pDev->FSetTextureAlphaOp(0,ETOP_SelectArg2,ETA_Texture,ETA_Diffuse);

		pDev->DrawMem(EDraw_LineList,1,vert,sizeof(Vert_PtDT));
	}

	extern bool CompResInfo(const pair<tstring,int>& info1,const pair<tstring,int>& info2);
	void Draw2DImp::Report()
	{
		vector<pair<tstring,int> > texInfoArray;
		m_texPool->ExportInfo(texInfoArray);

		std::sort(texInfoArray.begin(),texInfoArray.end(),CompResInfo);
		LOG(_T("--Draw2D Texture pool report-----------------------------------------------\r\n"));
		int sum=0;
		for(size_t i=0;i<texInfoArray.size();i++)
		{
			sum+=texInfoArray[i].second;
			LOG(_T("%04d\t%04dKB\t%s\r\n"),i,texInfoArray[i].second/1024,texInfoArray[i].first.c_str());
		}
		LOG(_T("Total=%dKB\r\n"),sum/1024);
	}

	void Draw2DImp::DrawInfo(int x,int y)
	{
		TCHAR szInfo[256];
		_stprintf(szInfo,_T("Draw2D texture pool %dK"),GetTexPoolSize()/1024);
		DrawX::DrawDebugString(szInfo,x,y);
	}

	void Draw2DImp::RotateQuad( Vert_PtDT* pQuadVert, const float fAngle )
	{
		if( fAngle < FLOAT_EPSILON && fAngle > -FLOAT_EPSILON )
			return;

		//--以矩形中心为旋转中心
		float fCenterX = ( pQuadVert[1].pos.x - pQuadVert[0].pos.x ) * 0.5f + pQuadVert[0].pos.x;
		float fCenterY = ( pQuadVert[2].pos.y - pQuadVert[1].pos.y ) * 0.5f + pQuadVert[1].pos.y;
		float fSin = sinf( fAngle );
		float fCos = cosf( fAngle );

		for( int i = 0; i < 4; i++ )
		{
			float fPosX0 = pQuadVert[i].pos.x - fCenterX;
			float fPosY0 = pQuadVert[i].pos.y - fCenterY;

			pQuadVert[i].pos.x = fPosX0 * fCos - fPosY0 * fSin;
			pQuadVert[i].pos.y = fPosX0 * fSin + fPosY0 * fCos;
			pQuadVert[i].pos.x += fCenterX;
			pQuadVert[i].pos.y += fCenterY;
		}
	}

}//namespace Cool3D