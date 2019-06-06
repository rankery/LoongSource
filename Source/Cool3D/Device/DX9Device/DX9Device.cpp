#include "StdAfx.h"
#include ".\dx9device.h"

#include <d3d9types.h>
#include "..\..\Util\DebugMisc.h"
#include "..\..\Util\Exception.h"
#include "DX9Init.h"
#include "DX9BaseDef.h"
#include "DX9IB.h"
#include "DX9VB.h"
#include "DX9Tex.h"
#include "DX9RenderTex.h"
#include "DX9VS.h"
#include "DX9PS.h"
#include "DX9Font.h"
#include "DX9DepthFont.h"
#include "DX9VertDecl.h"
#include "DX9RenderTex.h"
#include "DX9RenderCubeTex.h"
#include "DX9RenderToEnvMap.h"
#include "DX9VizQuery.h"

#include "..\..\Util\FMemory.h"

#pragma comment(lib,"d3d9.lib")

namespace Cool3D
{
	namespace Detail
	{
		// Helper function to stuff a FLOAT into a DWORD argument
		inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

		DX9Device::DX9Device(void)
		{
			m_pD3D=NULL;
			m_pDevice=NULL;
			memset(m_matStack,0,sizeof(m_matStack));
			m_pRenderStatus=NULL;
			memset(&m_caps,0,sizeof(m_caps));
			m_pBackSurface=NULL;
			m_pAddSurface=NULL;

			ClearCurrentState();

			m_bLost = false;
			m_bDeviceResetFlag = false;
		}

		DX9Device::~DX9Device(void)
		{
			SAFE_DELETE(m_pRenderStatus);
			Destroy();
		}

		bool DX9Device::Create(tagDeviceCfg *pCfg)
		{
			m_pD3D=::Direct3DCreate9(D3D_SDK_VERSION);
			THROW_NULL(m_pD3D,_T("D3D9 object create failed."));

			m_pDevice=DX9Init::CreateDevice(m_pD3D,pCfg,&m_D3DPP);
			THROW_NULL(m_pDevice,_T("D3D9 device create failed."));
			m_pDevice->GetDeviceCaps(&m_caps);

			HRESULT hr=m_pDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&m_pBackSurface);

			for(int i=0;i<EMS_Num;i++)
			{
				HRESULT hr=D3DXCreateMatrixStack(0,&m_matStack[i]);
				THROW_FAILED(hr,NULL);
			}

			LOG(_T("VertexShader=%x,PixelShader=%x\n"),m_caps.VertexShaderVersion,m_caps.PixelShaderVersion); 
			LOG(_T("Device Create ok.TexMem=%d K\n"),m_pDevice->GetAvailableTextureMem()/1024); 
			LOG(_T("Device max point size=%.2f\n"),m_caps.MaxPointSize);
			m_deviceCfg=*pCfg;

			m_pRenderStatus=new RenderStatus;

			//--默认状态
			SetDefaultState();

			//--获得当前材质
			ASSERT(sizeof(D3DMATERIAL9)==sizeof(tagRenderColor));
			m_pDevice->GetMaterial((D3DMATERIAL9*)&m_curRenderColor);
			
			return true;
		}

		void DX9Device::ClearCurrentState()
		{
			memset(m_pCurVB,NULL,sizeof(m_pCurVB));
			memset(m_dwCurState,0,sizeof(m_dwCurState));
			m_pCurIB=NULL;
			memset(m_pCurTex,NULL,sizeof(m_pCurTex));
			memset(m_curTexAddU,ETAddress_Invalid,sizeof(m_curTexAddU));
			memset(m_curTexAddV,ETAddress_Invalid,sizeof(m_curTexAddV));
			memset(m_curTexAddW,ETAddress_Invalid,sizeof(m_curTexAddW));
			memset(m_curTexMagFilter,ETFilter_Invalid,sizeof(m_curTexMagFilter));
			memset(m_curTexMinFilter,ETFilter_Invalid,sizeof(m_curTexMinFilter));
			memset(m_curTexMipFilter,ETFilter_Invalid,sizeof(m_curTexMipFilter));
			memset(m_curTexColorOp,ETOP_Invalid,sizeof(m_curTexColorOp));
			memset(m_curTexAlphaOp,ETOP_Invalid,sizeof(m_curTexAlphaOp));
			memset(m_curTexColorArg1,ETA_Invalid,sizeof(m_curTexColorArg1));
			memset(m_curTexColorArg2,ETA_Invalid,sizeof(m_curTexColorArg2));
			memset(m_curTexAlphaArg1,ETA_Invalid,sizeof(m_curTexAlphaArg1));
			memset(m_curTexAlphaArg2,ETA_Invalid,sizeof(m_curTexAlphaArg2));
			memset(m_curTexUVIndex,-1,sizeof(m_curTexUVIndex));
			memset(m_curTexUVIndexFlags,EUVI_Invalid,sizeof(m_curTexUVIndexFlags));
			memset(m_curTexUVTrans,EUVT_Invalid,sizeof(m_curTexUVTrans));

			m_pCurVS=NULL;
			m_pCurPS=NULL;
			m_pCurStreamDef=NULL;
		}

		void DX9Device::SetDefaultState()
		{
			m_dwCurState[ERS_DepthFunc]=ECMP_LessEqual;
			m_dwCurState[ERS_DepthTestEnable]=true;
			m_dwCurState[ERS_DepthWriteEnable]=true;
			m_dwCurState[ERS_CullMode]=ECull_CW;
			m_dwCurState[ERS_FillMode]=EFill_Solid;

			m_dwCurState[ERS_ShadeMode]=EShade_Gouraud;
			m_dwCurState[ERS_AlphaTestEnable]=false;
			m_dwCurState[ERS_AlphaTestFunc]=ECMP_LessEqual;
			m_dwCurState[ERS_AlphaRef]=255;
			m_dwCurState[ERS_AlphaBlendEnable]=false;
			m_dwCurState[ERS_SrcBlend]=EBlend_SrcAlpha;
			m_dwCurState[ERS_DstBlend]=EBlend_InvSrcAlpha;
			m_dwCurState[ERS_StencilEnable]=false;
			m_dwCurState[ERS_StencilFunc]=ECMP_Always;
			m_dwCurState[ERS_StencilOpFail]=ESOP_Keep;
			m_dwCurState[ERS_StencilOpZFail]=ESOP_Keep;
			m_dwCurState[ERS_StencilOpPass]=ESOP_Keep;
			m_dwCurState[ERS_AntialiasedLine]=false;
			m_dwCurState[ERS_FLightEnable]=false;
			m_dwCurState[ERS_FSpecularEnable]=false;
			m_dwCurState[ERS_FFogEnable]=false;
			m_dwCurState[ERS_FFogColor]=true;
			m_dwCurState[ERS_ColorWriteEnable]=0x0000000F;
			m_dwCurState[ERS_UseVertexDiffuse]=true;
			m_dwCurState[ERS_TextureFactor]=0xFFFFFFFF;
			m_dwCurState[ERS_DepthBias]=0;
			m_dwCurState[ERS_DepthBiasScale]=0;
			m_dwCurState[ERS_NormalizeNormals]=0;

			for(int i=0;i<ERS_Num;i++)
				SetRenderState(ERStateType(i),m_dwCurState[i],true);
		}

		void DX9Device::Destroy()
		{
			//--如果是全屏模式,则把显示模式设回去
			if(!m_deviceCfg.bWindowed)
				ChangeDisplaySettings(NULL,0);
			//--
			for(int i=0;i<EMS_Num;i++)
			{
				SAFE_RELEASE(m_matStack[i]);
			}

			SAFE_RELEASE(m_pDevice);
			SAFE_RELEASE(m_pD3D);
		}

		D3DPRIMITIVETYPE DrawType2DX9(EDrawType type)
		{
			switch(type)
			{
			case EDraw_PointList:		 return D3DPT_POINTLIST;
			case EDraw_LineList:		 return D3DPT_LINELIST;
			case EDraw_LineStrip:		 return D3DPT_LINESTRIP;
			case EDraw_TriangleList:	 return D3DPT_TRIANGLELIST;
			case EDraw_TriangleStrip:	 return D3DPT_TRIANGLESTRIP;
			case EDraw_TriangleFan:		 return D3DPT_TRIANGLEFAN;
			}
			return D3DPT_FORCE_DWORD;
		}

		bool DX9Device::DrawPointSprites(float pointSize,int numPoints)
		{
			HRESULT hr=m_pDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );    // Turn on point sprites
			
			//如果纹理大于1像素，最好打开以计算纹理坐标
			hr=m_pDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );    // Allow sprites to be scaled with distance
			hr=m_pDevice->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(0.0f) );    // Default 1.0
			hr=m_pDevice->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.0f) );    // Default 0.0
			hr=m_pDevice->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(1.0f) );  
			hr=m_pDevice->SetRenderState( D3DRS_POINTSIZE_MAX, FtoDW(4096.0f));  //Default 64pixel
			
			hr=m_pDevice->SetRenderState( D3DRS_POINTSIZE,     FtoDW(pointSize) );  // Float value that specifies the size to use for point size computation in cases where point size is not specified for each vertex.
			hr=m_pDevice->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(0.0f) ); // Float value that specifies the minimum size of point primitives. Point primitives are clamped to this size during rendering. 

			bool ret=Draw(EDraw_PointList,numPoints);

			hr=m_pDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );    // Turn off point sprites

			return ret;
		}

		void DX9Device::ColorFill(const RECT* pRect,DWORD color)
		{
			IDirect3DSurface9* pSurface=NULL;
			HRESULT hr=m_pDevice->GetRenderTarget(0,&pSurface);
			THROW_FAILED(hr,_T("ColorFill"));
			m_pDevice->ColorFill(pSurface,pRect,color);			
			SAFE_RELEASE(pSurface);
		}

		bool DX9Device::Draw(EDrawType type,UINT numPrimitive)
		{
			HRESULT hr=m_pDevice->DrawPrimitive(DrawType2DX9(type),0,numPrimitive);
#ifdef RENDER_STATUS
			m_pRenderStatus->numDrawCall++;
			m_pRenderStatus->numPrim+=numPrimitive;
#endif//RENDER_STATUS
			return SUCCEEDED(hr);
		}

		bool DX9Device::DrawMem(EDrawType type,UINT numPrimitive,void *pVert,UINT nStride)
		{
			m_pCurVB[0]=NULL;
			HRESULT hr=m_pDevice->DrawPrimitiveUP(DrawType2DX9(type),numPrimitive,pVert,nStride);

#ifdef RENDER_STATUS
			m_pRenderStatus->numDrawCall++;
			m_pRenderStatus->numPrim+=numPrimitive;
#endif//RENDER_STATUS
			return SUCCEEDED(hr);
		}
		
		bool DX9Device::DrawIndexed(EDrawType type,UINT baseIndex,UINT numVert,UINT numPrimitive,UINT startIndex)
		{
			HRESULT hr=m_pDevice->DrawIndexedPrimitive(DrawType2DX9(type),
				baseIndex,//BaseVertexIndex
				0,//MinIndex
				numVert,
				startIndex,//StartIndex
				numPrimitive);
#ifdef RENDER_STATUS
			m_pRenderStatus->numDrawCall++;
			m_pRenderStatus->numPrim+=numPrimitive;
#endif//RENDER_STATUS
			return SUCCEEDED(hr);
		}

		void DX9Device::SetClearParam(bool target,bool depth, bool stencil,DWORD color,float z,DWORD stencilVal)
		{
			memset(&m_clearParam,0,sizeof(m_clearParam));
			if(target)
				m_clearParam.flags|=D3DCLEAR_TARGET;
			if(depth)
				m_clearParam.flags|=D3DCLEAR_ZBUFFER;
			if(stencil)
				m_clearParam.flags|=D3DCLEAR_STENCIL;

			m_clearParam.color=color;
			m_clearParam.z=z;
			m_clearParam.stencil=stencilVal;
		}

		void DX9Device::Clear()
		{
			m_pDevice->Clear(0,NULL,
				m_clearParam.flags,m_clearParam.color,
				m_clearParam.z,m_clearParam.stencil);
		}

		void DX9Device::Clear(const RECT* pRect)
		{
			if(pRect==NULL)
			{
				m_pDevice->Clear(0,NULL,
					m_clearParam.flags,m_clearParam.color,
					m_clearParam.z,m_clearParam.stencil);
			}
			else
			{
				D3DRECT d3dRect;
				d3dRect.x1=pRect->left;
				d3dRect.y1=pRect->top;
				d3dRect.x2=pRect->right;
				d3dRect.y2=pRect->bottom;

				m_pDevice->Clear(1,&d3dRect,
					m_clearParam.flags,m_clearParam.color,
					m_clearParam.z,m_clearParam.stencil);
			}
		}

		void DX9Device::Clear(bool target,bool depth, bool stencil)
		{
			DWORD flags=0;
			if(target)
				flags|=D3DCLEAR_TARGET;
			if(depth)
				flags|=D3DCLEAR_ZBUFFER;
			if(stencil)
				flags|=D3DCLEAR_STENCIL;
			m_pDevice->Clear(0,NULL,flags,
				m_clearParam.color,m_clearParam.z,m_clearParam.stencil);
		}

		void DX9Device::Clear(bool target,bool depth, bool stencil,DWORD color,float z,DWORD stencilVal,const RECT* pRect)
		{
			DWORD flags=0;
			if(target)
				flags|=D3DCLEAR_TARGET;
			if(depth)
				flags|=D3DCLEAR_ZBUFFER;
			if(stencil)
				flags|=D3DCLEAR_STENCIL;
			if(pRect!=NULL)
			{
				D3DRECT d3dRect;
				d3dRect.x1=pRect->left;
				d3dRect.y1=pRect->top;
				d3dRect.x2=pRect->right;
				d3dRect.y2=pRect->bottom;
				m_pDevice->Clear(1,&d3dRect,flags,color,z,stencilVal);
			}
			else
				m_pDevice->Clear(0,NULL,flags,color,z,stencilVal);
		}

		bool DX9Device::Begin()
		{
			m_pRenderStatus->Clear();//不能放在Clear中,因为每次变更Render Target都需要调用Clear()

			if(m_bLost)
				return false;
			else
				return SUCCEEDED(m_pDevice->BeginScene());
		}

		void DX9Device::End()
		{
			if(m_bLost)
				return;

#ifdef RENDER_STATUS
			m_pRenderStatus->Draw();
#endif//RENDER_STATUS
			m_pDevice->EndScene();
		}

		void DX9Device::Present(HWND hDestWnd)
		{
			m_pRenderStatus->CalFPS();
			if(m_bLost)
			{
				Sleep( 100 );
				HandleDeviceLost();
			}
			else
			{
				HRESULT hr=m_pDevice->Present(NULL,NULL,hDestWnd,NULL);
				if(hr == D3DERR_DEVICELOST)//device lost
				{
					m_bLost = true;
				}
			}
		}

		void DX9Device::SetDepthFunc(ECmpFunc zFun)
		{
			SetRenderState(ERS_DepthFunc,zFun);
		}

		void DX9Device::EnableDepthTest(bool bEnable)
		{
			SetRenderState(ERS_DepthTestEnable,bEnable);
		}
		void DX9Device::EnableDepthWrite(bool bEnable)
		{
			SetRenderState(ERS_DepthWriteEnable,bEnable);
		}

		void DX9Device::SetCullMode(ECullMode cullMode)
		{
			SetRenderState(ERS_CullMode,cullMode);
		}

		void DX9Device::SetFillMode(EFillMode fillMode)
		{
			SetRenderState(ERS_FillMode,fillMode);
		}
		void DX9Device::SetShadeMode(EShadeMode shadeMode)
		{
			SetRenderState(ERS_ShadeMode,shadeMode);
		}

		void DX9Device::EnableAlphaTest(bool bEnable)
		{
			SetRenderState(ERS_AlphaTestEnable,bEnable);
		}
		void DX9Device::SetAlphaTestFunc(ECmpFunc func)
		{
			SetRenderState(ERS_AlphaTestFunc,func);
		}
		void DX9Device::SetAlphaRef(BYTE ref)
		{
			SetRenderState(ERS_AlphaRef,ref);
		}

		void DX9Device::EnableAlphaBlend(bool bEnable)
		{
			SetRenderState(ERS_AlphaBlendEnable,bEnable);
		}
		void DX9Device::SetAlphaBlend(EBlend src,EBlend dst)
		{
			SetRenderState(ERS_SrcBlend,src);
			SetRenderState(ERS_DstBlend,dst);
		}

		void DX9Device::EnableStenciling(bool bEnable)
		{
			SetRenderState(ERS_StencilEnable,bEnable);
		}
		void DX9Device::SetStencilFunc(ECmpFunc func, int refVal, DWORD mask)
		{
			m_pDevice->SetRenderState(D3DRS_STENCILREF,refVal);
			m_pDevice->SetRenderState(D3DRS_STENCILMASK,mask);

			SetRenderState(ERS_StencilFunc,func);
			//todo
		}
		void DX9Device::SetStencilOp(EStencilOP fail,EStencilOP zFail,EStencilOP pass)
		{
			SetRenderState(ERS_StencilOpFail,fail);
			SetRenderState(ERS_StencilOpZFail,zFail);
			SetRenderState(ERS_StencilOpPass,pass);
		}

		void DX9Device::SetSimplerAddress(DWORD sampler,ETextureAddress addU,ETextureAddress addV,ETextureAddress addW)
		{
			ASSERT(sampler<MAX_Sampler);
			if(m_curTexAddU[sampler]!=addU)
			{
				m_curTexAddU[sampler]=addU;
				m_pDevice->SetSamplerState(sampler,D3DSAMP_ADDRESSU,TAddress2DX9(addU));
			}
			if(m_curTexAddV[sampler]!=addV)
			{
				m_curTexAddV[sampler]=addV;
				m_pDevice->SetSamplerState(sampler,D3DSAMP_ADDRESSV,TAddress2DX9(addV));
			}
			if(m_curTexAddW[sampler]!=addW)
			{
				m_curTexAddW[sampler]=addW;
				m_pDevice->SetSamplerState(sampler,D3DSAMP_ADDRESSW,TAddress2DX9(addW));
			}
		}
		void DX9Device::SetSimplerFilter(DWORD sampler,ETextrueFilter mag,ETextrueFilter min)
		{
			ASSERT(sampler<MAX_Sampler);
			if(m_curTexMagFilter[sampler]!=mag)
			{
				m_curTexMagFilter[sampler]=mag;
				m_pDevice->SetSamplerState(sampler,D3DSAMP_MAGFILTER,TFilter2DX9(mag));
			}
			if(m_curTexMinFilter[sampler]!=min)
			{
				m_curTexMinFilter[sampler]=min;
				m_pDevice->SetSamplerState(sampler,D3DSAMP_MINFILTER,TFilter2DX9(min));
			}

			if(mag==ETFilter_ANISOTROPIC||min==ETFilter_ANISOTROPIC)
			{
				//当使用各向异性过滤时，我们必须设置D3DSAMP_MAXANISOTROPY等级，来确定anisotropic的质量
				m_pDevice->SetSamplerState(0,D3DSAMP_MAXANISOTROPY,m_caps.MaxAnisotropy);
			}
		}

		void DX9Device::SetSimplerMipFilter(DWORD sampler,ETextrueFilter mip)
		{
			ASSERT(sampler<MAX_Sampler);
			if(m_curTexMipFilter[sampler]!=mip)
			{
				m_curTexMipFilter[sampler]=mip;
				m_pDevice->SetSamplerState(sampler,D3DSAMP_MIPFILTER,TFilter2DX9(mip));
				float g_fMipMapLodBias=-0.8f;
				m_pDevice->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD)(&g_fMipMapLodBias)));
			}
		}

		void DX9Device::SetRenderState(ERStateType state, DWORD val, bool bForce)
		{
			if(bForce==false
				&& m_dwCurState[state]==val)
				return;

			HRESULT hr;

			switch(state)
			{
			case ERS_DepthFunc:
				m_pDevice->SetRenderState(D3DRS_ZFUNC,CmpFunc2DX9(ECmpFunc(val)));
				break;
			case ERS_DepthTestEnable:
				m_pDevice->SetRenderState(D3DRS_ZENABLE,val);
				break;
			case ERS_DepthWriteEnable:
				m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE,val);
				break;
			case ERS_CullMode:
				m_pDevice->SetRenderState(D3DRS_CULLMODE,CullMode2DX9(ECullMode(val)));
				break;
			case ERS_FillMode:
				m_pDevice->SetRenderState(D3DRS_FILLMODE ,FillMode2DX9(EFillMode(val)));
				break;
			case ERS_ShadeMode:
				m_pDevice->SetRenderState(D3DRS_SHADEMODE ,ShadeMode2DX9(EShadeMode(val)));
				break;
			case ERS_AlphaTestEnable:
				m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,val);
				break;
			case ERS_AlphaTestFunc:
				m_pDevice->SetRenderState(D3DRS_ALPHAFUNC, CmpFunc2DX9(ECmpFunc(val)));
				break;
			case ERS_AlphaRef:
				m_pDevice->SetRenderState(D3DRS_ALPHAREF,val);
				break;
			case ERS_AlphaBlendEnable:
				m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,val);
				break;
			case ERS_SrcBlend:
				m_pDevice->SetRenderState(D3DRS_SRCBLEND, Blend2DX9(EBlend(val)));
				break;
			case ERS_DstBlend:
				m_pDevice->SetRenderState(D3DRS_DESTBLEND, Blend2DX9(EBlend(val)));
				break;
			case ERS_StencilEnable:
				m_pDevice->SetRenderState(D3DRS_STENCILENABLE, val);
				break;
			case ERS_StencilFunc:
				m_pDevice->SetRenderState(D3DRS_STENCILFUNC, CmpFunc2DX9(ECmpFunc(val)));
				break;
			case ERS_StencilOpFail:
				m_pDevice->SetRenderState(D3DRS_STENCILFAIL, StencilOP2DX9(EStencilOP(val)));
				break;
			case ERS_StencilOpZFail:
				m_pDevice->SetRenderState(D3DRS_STENCILZFAIL, StencilOP2DX9(EStencilOP(val)));
				break;
			case ERS_StencilOpPass:
				m_pDevice->SetRenderState(D3DRS_STENCILPASS, StencilOP2DX9(EStencilOP(val)));
				break;
			case ERS_AntialiasedLine:
				m_pDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, val);
				break;
			case ERS_FLightEnable:
				m_pDevice->SetRenderState(D3DRS_LIGHTING,val);
				break;
			case ERS_FSpecularEnable:
				m_pDevice->SetRenderState(D3DRS_SPECULARENABLE,val);
				break;
			case ERS_FFogEnable:
				m_pDevice->SetRenderState(D3DRS_FOGENABLE,val);
				break;
			case ERS_FFogColor:
				m_pDevice->SetRenderState(D3DRS_FOGCOLOR,val);
				break;
			case ERS_ColorWriteEnable:
				m_pDevice->SetRenderState(D3DRS_COLORWRITEENABLE,val);
				break;
			case ERS_UseVertexDiffuse:
				if(val==1)
					m_pDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_COLOR1);
				else
					m_pDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_MATERIAL);
				break;
			case ERS_TextureFactor:
				m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,val);
				break;
			case ERS_DepthBias:
				hr=m_pDevice->SetRenderState(D3DRS_DEPTHBIAS,val);
				break;
			case ERS_DepthBiasScale:
				hr=m_pDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS,val);
				break;
			case ERS_NormalizeNormals:
				hr=m_pDevice->SetRenderState(D3DRS_NORMALIZENORMALS,val);
				break;
			default:
				ASSERT(0);
			}//endof switch

			m_dwCurState[state]=val;
		}

		DWORD DX9Device::GetRenderState(ERStateType state)
		{
			return m_dwCurState[state];
		}

		IVertBuffer *DX9Device::NewVB()
		{
			DX9VB *pNewVB = new DX9VB;
			m_vbList.push_back(pNewVB);

			return pNewVB;
		}
		IIndexBuffer *DX9Device::NewIB()
		{
			DX9IB *pNewIB = new DX9IB;
			m_ibList.push_back(pNewIB);

			return pNewIB;
		}

		IDeviceTex* DX9Device::NewTexture()
		{
			return new DX9Tex;
		}

		IRenderTexture* DX9Device::NewRenderTexture()
		{
			DX9RenderTex *pNewRenderTex = new DX9RenderTex;
			m_renderTexList.PushBack(pNewRenderTex);

			return pNewRenderTex;
		}

		IRenderCubeTexture* DX9Device::NewRenderCubeTexture()
		{
			DX9RenderCubeTex *pNewRenderCubeTex = new DX9RenderCubeTex;
			m_renderCubeTexList.push_back(pNewRenderCubeTex);

			return pNewRenderCubeTex;
		}

		IVertShader* DX9Device::NewVS()
		{
			return new DX9VS;
		}

		IPixelShader* DX9Device::NewPS()
		{
			return new DX9PS;
		}

		IFont* DX9Device::NewFont()
		{
			DX9Font *pNewFont = new DX9Font;
			
			m_fontList.push_back(pNewFont);
			return pNewFont;
		}
		IDepthFont* DX9Device::NewDepthFont()
		{
			DX9DepthFont *pNewFont = new DX9DepthFont;
			m_dfontList.push_back(pNewFont);

			return pNewFont;
		}
		IStreamDefine* DX9Device::NewStreamDefine()
		{
			return new DX9VertDecl;
		}

		IRenderToEnvMap* DX9Device::NewRenderToEnvMap()
		{
			return new DX9RenderToEnvMap;
		}

		IDeviceVizQuery* DX9Device::NewVizQuery()
		{
			DX9VizQuery* pVizQuery = new DX9VizQuery;
			m_vizQueryList.push_back(pVizQuery);
			return pVizQuery;
		}

		void DX9Device::SetStreamDefine(IStreamDefine *pDef)
		{
			if(m_pCurStreamDef==pDef)
				return;
			else
				m_pCurStreamDef=pDef;

			DX9VertDecl *pDecl=static_cast<DX9VertDecl *>(pDef);
			HRESULT hr=m_pDevice->SetVertexDeclaration(pDecl->GetD3D());
		}
		void DX9Device::SetVB(DWORD stream,IVertBuffer *pVB)
		{
			ASSERT(stream < MAX_VertStream);
			/**	如果是Dynamic vb,外部更新后需要Set到device才能更新
			*/
			if(pVB==m_pCurVB[stream])
			{
				if(pVB->IsDynamic())
					m_pCurVB[stream]=pVB;
				else
					return;
			}
			else
				m_pCurVB[stream]=pVB;
				
			//--
			DX9VB *dVB=static_cast<DX9VB*>(pVB);
			ASSERT(dVB->GetDX9() != NULL);
			HRESULT hr=m_pDevice->SetStreamSource(stream,dVB->GetDX9(),0,pVB->GetStride());

#ifdef RENDER_STATUS
			m_pRenderStatus->numVBSwitch++;
#endif//RENDER_STATUS
		}

		void DX9Device::SetIB(IIndexBuffer *pIB)
		{
			/**	如果是Dynamic ib,外部更新后需要Set到device才能更新
			*/
			if(pIB==m_pCurIB)
			{
				if(pIB->IsDynamic())
					m_pCurIB=pIB;
				else
					return;
			}
			else
				m_pCurIB=pIB;

			DX9IB *dIB=static_cast<DX9IB*>(pIB);
			ASSERT(dIB->GetDX9() != NULL);
			HRESULT hr=m_pDevice->SetIndices(dIB->GetDX9());

		}

		void DX9Device::SetTexture(DWORD sample,IDeviceTex *pTex)
		{
			ASSERT(sample<MAX_Sampler);

			IDirect3DTexture9* pD3DTex=NULL;
			if(pTex!=NULL)
			{
				DX9Tex *pDX9Tex=static_cast<DX9Tex *>(pTex);
				pD3DTex=pDX9Tex->GetDX9();
			}

			if(pD3DTex==m_pCurTex[sample])
				return;
			else
				m_pCurTex[sample]=pD3DTex;

			m_curTexAddU[sample]=ETAddress_Invalid;
			m_curTexAddV[sample]=ETAddress_Invalid;
			m_curTexAddW[sample]=ETAddress_Invalid;
			m_curTexMagFilter[sample]=ETFilter_Invalid;
			m_curTexMinFilter[sample]=ETFilter_Invalid;
			m_curTexMipFilter[sample]=ETFilter_Invalid;

			HRESULT hr=m_pDevice->SetTexture(sample,pD3DTex);

#ifdef RENDER_STATUS
			m_pRenderStatus->numTexSwitch++;
#endif//RENDER_STATUS
		}

		void DX9Device::SetTexture(DWORD sample,IRenderTexture *pTex)
		{
			IDirect3DTexture9* pD3DTex=NULL;
			if(pTex!=NULL)
			{
				DX9RenderTex *pDX9Tex=static_cast<DX9RenderTex*>(pTex);
				pD3DTex=pDX9Tex->GetDX9Texture();
			}

			/**	如果是render tex,外部更新后需要Set到device才能更新??
			*/
			m_pCurTex[sample]=pD3DTex;

			m_curTexAddU[sample]=ETAddress_Invalid;
			m_curTexAddV[sample]=ETAddress_Invalid;
			m_curTexAddW[sample]=ETAddress_Invalid;
			m_curTexMagFilter[sample]=ETFilter_Invalid;
			m_curTexMinFilter[sample]=ETFilter_Invalid;
			m_curTexMipFilter[sample]=ETFilter_Invalid;


            HRESULT hr=m_pDevice->SetTexture(sample,pD3DTex);
#ifdef RENDER_STATUS
			m_pRenderStatus->numTexSwitch++;
#endif//RENDER_STATUS
		}

		void DX9Device::SetTexture( DWORD sample,IRenderCubeTexture *pTex )
		{
			IDirect3DCubeTexture9* pD3DTex=NULL;
			if(pTex!=NULL)
			{
				DX9RenderCubeTex *pDX9Tex=static_cast<DX9RenderCubeTex*>(pTex);
				pD3DTex=pDX9Tex->GetDX9Texture();
			}

			/**	如果是render tex,外部更新后需要Set到device才能更新??
			*/
			m_pCurTex[sample]=pD3DTex;

			m_curTexAddU[sample]=ETAddress_Invalid;
			m_curTexAddV[sample]=ETAddress_Invalid;
			m_curTexAddW[sample]=ETAddress_Invalid;
			m_curTexMagFilter[sample]=ETFilter_Invalid;
			m_curTexMinFilter[sample]=ETFilter_Invalid;
			m_curTexMipFilter[sample]=ETFilter_Invalid;


			HRESULT hr=m_pDevice->SetTexture(sample,pD3DTex);
#ifdef RENDER_STATUS
			m_pRenderStatus->numTexSwitch++;
#endif//RENDER_STATUS
		}

		void DX9Device::SetVertShader(IVertShader *pVS)
		{
			if(m_pCurVS==pVS)
				return;
			else
				m_pCurVS=pVS;

			if(pVS)
			{
				DX9VS *pDX9VS=static_cast<DX9VS *>(pVS);
				HRESULT hr=m_pDevice->SetVertexShader(pDX9VS->GetDX9());
			}
			else
				m_pDevice->SetVertexShader(NULL);
		}

		void DX9Device::SetPixelShader(IPixelShader *pPS)
		{
			if(m_pCurPS==pPS)
				return;
			else
				m_pCurPS=pPS;

			if(pPS)
			{
				DX9PS *pDX9PS=static_cast<DX9PS *>(pPS);
				HRESULT hr=m_pDevice->SetPixelShader(pDX9PS->GetDX9());
				THROW_FAILED(hr,0);
			}
			else
				m_pDevice->SetPixelShader(NULL);
		}
		
		Matrix4* DX9Device::GetMatrix(EMatrixStackType type)
		{
			ASSERT(type>=EMS_Prj&&type<EMS_Num);
			return m_matStack[type]->GetTop();
		}
		void DX9Device::MatPush(EMatrixStackType type)
		{
			ASSERT(type>=EMS_Prj&&type<EMS_Num);
			HRESULT hr=m_matStack[type]->Push();
		}
		void DX9Device::MatPop(EMatrixStackType type)
		{
			ASSERT(type>=EMS_Prj&&type<EMS_Num);
			HRESULT hr=m_matStack[type]->Pop();
		}
		void DX9Device::MatLoad(EMatrixStackType type, const Matrix4 *pMat)
		{
			ASSERT(type>=EMS_Prj&&type<EMS_Num);
			HRESULT hr=m_matStack[type]->LoadMatrix(pMat);
		}

		void DX9Device::SetScissorRect(const RECT *pRect)
		{
			HRESULT hr;
			if(pRect==NULL)
			{
				m_pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
			}
			else
			{
				hr=m_pDevice->SetScissorRect(pRect);
				hr=m_pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
			}
		}

		void DX9Device::SetClipPlane(Plane *pPlane)
		{
			if(pPlane!=NULL)
			{
				m_pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);

				float a[4];
				a[0]=pPlane->normal.x;
				a[1]=pPlane->normal.y;
				a[2]=pPlane->normal.z;
				a[3]=pPlane->dist;
				m_pDevice->SetClipPlane(0,a);
			}
			else
			{
				m_pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, 0);
			}
		}

		bool DX9Device::GetClipPlane(Plane *pPlane)
		{
			ASSERT(pPlane!=NULL);
			ASSERT(sizeof(Plane)==4*sizeof(float));

			DWORD clipPlane;
			m_pDevice->GetRenderState(D3DRS_CLIPPLANEENABLE, &clipPlane);
			if(clipPlane==0)
				return false;

			float a[4];
			m_pDevice->GetClipPlane(0,a);
			pPlane->normal.x=a[0];
			pPlane->normal.y=a[1];
			pPlane->normal.z=a[2];
			pPlane->dist=a[3];

			return true;
		}

		void DX9Device::GetViewport(tagViewport& viewport)
		{
			ASSERT(sizeof(D3DVIEWPORT9)==sizeof(tagViewport));
			m_pDevice->GetViewport((D3DVIEWPORT9*)&viewport);
		}
		
		void DX9Device::SetViewport(const tagViewport* pViewport)
		{
			ASSERT(pViewport!=NULL);
			ASSERT(sizeof(D3DVIEWPORT9)==sizeof(tagViewport));
			m_pDevice->SetViewport((CONST D3DVIEWPORT9*)pViewport);
		}

		D3DTEXTUREOP TextureOp2DX9(ETextureOp op)
		{
			switch(op)
			{
			case ETOP_Disable:		return D3DTOP_DISABLE;
			case ETOP_SelectArg1:	return D3DTOP_SELECTARG1;
			case ETOP_SelectArg2:	return D3DTOP_SELECTARG2;
			case ETOP_Mul:			return D3DTOP_MODULATE;
			case ETOP_Mul2x:		return D3DTOP_MODULATE2X;
			case ETOP_Mul4x:		return D3DTOP_MODULATE4X;
			case ETOP_Add:			return D3DTOP_ADD;
			case ETOP_Sub:			return D3DTOP_SUBTRACT;
			case ETOP_BlendCurrentAlpha:	return D3DTOP_BLENDCURRENTALPHA;
			case ETOP_BlendTextureAlpha:	return D3DTOP_BLENDTEXTUREALPHA;
			case ETOP_BlendDiffuseAlpha:	return D3DTOP_BLENDDIFFUSEALPHA;
			case ETOP_BlendFactorAlpha:		return D3DTOP_BLENDFACTORALPHA;
			case ETOP_BumpEnvMap:			return D3DTOP_BUMPENVMAP;
			}
			return D3DTOP_FORCE_DWORD;
		}

		DWORD TextureArg2DX9(ETextureArg arg)
		{
			switch(arg)
			{
			case ETA_Current:	return D3DTA_CURRENT;
			case ETA_Diffuse:	return D3DTA_DIFFUSE;
			case ETA_Texture:	return D3DTA_TEXTURE;
			case ETA_Temp:		return D3DTA_TEMP;
			case ETA_TFactor:	return D3DTA_TFACTOR;
			case ETA_Specular:	return D3DTA_SPECULAR;
			case ETA_TextureAlphaReplicate: return D3DTA_TEXTURE|D3DTA_ALPHAREPLICATE;
			}
			return 0;
		}

		UINT DX9Device::FGetMaxTextureStage()
		{
			return m_caps.MaxSimultaneousTextures;
		}

		void DX9Device::FSetTextureColorOp(DWORD stage,ETextureOp op,ETextureArg arg1,ETextureArg arg2)
		{
			ASSERT(stage<MAX_Sampler);
			if(m_curTexColorOp[stage]!=op)
			{
				m_curTexColorOp[stage]=op;
				m_pDevice->SetTextureStageState(stage,D3DTSS_COLOROP,TextureOp2DX9(op));
			}
			if(m_curTexColorArg1[stage]!=arg1)
			{
				m_curTexColorArg1[stage]=arg1;
				m_pDevice->SetTextureStageState(stage,D3DTSS_COLORARG1,TextureArg2DX9(arg1));
			}
			if(m_curTexColorArg2[stage]!=arg2)
			{
				m_curTexColorArg2[stage]=arg2;
				m_pDevice->SetTextureStageState(stage,D3DTSS_COLORARG2,TextureArg2DX9(arg2));
			}
		}
		void DX9Device::FSetTextureAlphaOp(DWORD stage,ETextureOp op,ETextureArg arg1,ETextureArg arg2)
		{
			ASSERT(stage<MAX_Sampler);
			if(m_curTexAlphaOp[stage]!=op)
			{
				m_curTexAlphaOp[stage]=op;
				m_pDevice->SetTextureStageState(stage,D3DTSS_ALPHAOP,TextureOp2DX9(op));
			}
			if(m_curTexAlphaArg1[stage]!=arg1)
			{
				m_curTexAlphaArg1[stage]=arg1;
				m_pDevice->SetTextureStageState(stage,D3DTSS_ALPHAARG1,TextureArg2DX9(arg1));
			}
			if(m_curTexAlphaArg2[stage]!=arg2)
			{
				m_curTexAlphaArg2[stage]=arg2;
				m_pDevice->SetTextureStageState(stage,D3DTSS_ALPHAARG2,TextureArg2DX9(arg2));
			}
		}

		void DX9Device::FSetTextureColorOp(DWORD stage,ETextureOp op)
		{
			ASSERT(stage<MAX_Sampler);
			if(m_curTexColorOp[stage]!=op)
			{
				m_curTexColorOp[stage]=op;
				m_pDevice->SetTextureStageState(stage,D3DTSS_COLOROP,TextureOp2DX9(op));
			}
		}
		void DX9Device::FSetTextureAlphaOp(DWORD stage,ETextureOp op)
		{
			ASSERT(stage<MAX_Sampler);
			if(m_curTexAlphaOp[stage]!=op)
			{
				m_curTexAlphaOp[stage]=op;
				m_pDevice->SetTextureStageState(stage,D3DTSS_ALPHAOP,TextureOp2DX9(op));
			}
		}

		DWORD TexCoordSrc2DX9(UINT uvIndex,DWORD flags)
		{
			DWORD ret=uvIndex;
			if(flags!=0)
			{
				switch (flags)
				{
				case EUVI_CameraSpaceNormal:
					ret=D3DTSS_TCI_CAMERASPACENORMAL;
					break;
				case EUVI_CameraSpacePos:
					ret=D3DTSS_TCI_CAMERASPACEPOSITION;
					break;
				case EUVI_CameraReflectionVec:
					ret=D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR;
					break;
				case EUVI_SphereMap:
					ret=D3DTSS_TCI_SPHEREMAP;
					break;
				}
			}
			return ret;
		}

		void DX9Device::FSetTexCoordSrc(DWORD stage,UINT uvIndex,DWORD flags)
		{
			if(m_curTexUVIndex[stage]!=uvIndex
				||m_curTexUVIndexFlags[stage]!=flags)
			{
				m_curTexUVIndex[stage]=uvIndex;
				m_curTexUVIndexFlags[stage]=flags;
				m_pDevice->SetTextureStageState(stage,D3DTSS_TEXCOORDINDEX,TexCoordSrc2DX9(uvIndex,flags));
			}
		}

		void DX9Device::FSetTexBumpMat(DWORD stage,const Matrix2& mat)
		{
			m_pDevice->SetTextureStageState(stage,D3DTSS_BUMPENVMAT00,FtoDW(mat._11));
			m_pDevice->SetTextureStageState(stage,D3DTSS_BUMPENVMAT01,FtoDW(mat._12));
			m_pDevice->SetTextureStageState(stage,D3DTSS_BUMPENVMAT10,FtoDW(mat._21));
			m_pDevice->SetTextureStageState(stage,D3DTSS_BUMPENVMAT11,FtoDW(mat._22));
		}

		void DX9Device::FFlushMatrix(bool world,bool view,bool prj,bool tex,DWORD texIndex)
		{
			HRESULT hr;
			if(prj)
				hr=m_pDevice->SetTransform(D3DTS_PROJECTION,GetMatrix(EMS_Prj));
			if(view)
				hr=m_pDevice->SetTransform(D3DTS_VIEW,GetMatrix(EMS_View));
			if(world)
				hr=m_pDevice->SetTransform(D3DTS_WORLD,GetMatrix(EMS_World));
			if(tex)
			{
				if(texIndex&ETM_Tex0)
					hr=m_pDevice->SetTransform(D3DTS_TEXTURE0,GetMatrix(EMS_Tex0));
				if(texIndex&ETM_Tex1)
					hr=m_pDevice->SetTransform(D3DTS_TEXTURE1,GetMatrix(EMS_Tex1));
			}
		}

		DWORD UVTrans2DX9(EUVTrans trans)
		{
			switch(trans)
			{
			case EUVT_Disable:	return D3DTTFF_DISABLE;
			case EUVT_Count2:	return D3DTTFF_COUNT2;
			case EUVT_Count3:	return D3DTTFF_COUNT3;
			case EUVT_Project3:	return D3DTTFF_COUNT3|D3DTTFF_PROJECTED;
			case EUVT_Project4:	return D3DTTFF_COUNT4|D3DTTFF_PROJECTED;
			}
			return D3DTTFF_FORCE_DWORD;
		}

		void DX9Device::FSetUVTrans(DWORD stage,EUVTrans trans)
		{
			if(m_curTexUVTrans[stage]!=trans)
			{
				m_curTexUVTrans[stage]=trans;
				HRESULT hr=m_pDevice->SetTextureStageState(stage,D3DTSS_TEXTURETRANSFORMFLAGS,UVTrans2DX9(trans));
			}
		}

		void DX9Device::FEnableFog(bool bEnable)
		{
			SetRenderState(ERS_FFogEnable,bEnable);

			HRESULT hr;
			hr=m_pDevice->SetRenderState(D3DRS_FOGTABLEMODE,D3DFOG_LINEAR);
		}

		void DX9Device::FSetFog(DWORD color,float start,float end,float density)
		{
			SetRenderState(ERS_FFogColor,color);

			HRESULT hr;
			hr=m_pDevice->SetRenderState(D3DRS_FOGSTART,*((DWORD*)(&start)));
			hr=m_pDevice->SetRenderState(D3DRS_FOGEND,*((DWORD*)(&end)));
			hr=m_pDevice->SetRenderState(D3DRS_FOGDENSITY,*((DWORD*)(&density)));
		}

		void DX9Device::FSetFogFormula(EFogFormula fm)
		{
			D3DFOGMODE mode=D3DFOG_NONE;
			switch(fm)
			{
			case EFFM_None:
				mode=D3DFOG_NONE;
				break;
			case EFFM_Linear:
				mode=D3DFOG_LINEAR;
				break;
			case EFFM_Exp:
				mode=D3DFOG_EXP;
				break;
			case EFFM_Exp2:
				mode=D3DFOG_EXP2;
				break;
			}
			HRESULT hr=m_pDevice->SetRenderState(D3DRS_FOGVERTEXMODE,mode);
		}

		void DX9Device::FEnableLight(bool bEnable)
		{
			SetRenderState(ERS_FLightEnable,bEnable);
		}

		void DX9Device::FEnableSpecular(bool bEnable)
		{
			SetRenderState(ERS_FSpecularEnable,bEnable);
		}

		void DX9Device::FSetRenderColor(const tagRenderColor* pColor)
		{
			if(m_curRenderColor==*pColor)
				return;
			else
			{
				ASSERT(sizeof(D3DMATERIAL9) == sizeof(tagRenderColor));
				m_pDevice->SetMaterial( (D3DMATERIAL9*)pColor );
				m_curRenderColor=*pColor;
			}
		}
		
		static void CopyColor(D3DCOLORVALUE& dx9C,const Color4f& fc)
		{
			dx9C.a=fc.A;
			dx9C.r=fc.R;
			dx9C.g=fc.G;
			dx9C.b=fc.B;
		}
		void DX9Device::FSetLight(DWORD index,const tagLight *pLight)
		{
			HRESULT hr;
			hr=m_pDevice->LightEnable(index,pLight!=NULL);
			if(pLight!=NULL)
			{
				D3DLIGHT9 light;
				memset(&light,0,sizeof(D3DLIGHT9));
				light.Type=(D3DLIGHTTYPE)pLight->type;//定义是一样的
				CopyColor(light.Diffuse,pLight->diffuse);
				CopyColor(light.Specular,pLight->specular);
				CopyColor(light.Ambient,pLight->ambient);
				light.Position=pLight->pos;
				light.Direction=pLight->dir;
				light.Range=pLight->range;
				light.Falloff=pLight->spot.z;
				light.Attenuation0=pLight->attenuation.x;
				light.Attenuation1=pLight->attenuation.y;
				light.Attenuation2=pLight->attenuation.z;
				light.Theta=pLight->spot.x;
				light.Phi=pLight->spot.y;

				hr=m_pDevice->SetLight(index,&light);
			}
		}

		UINT DX9Device::GetClientWidth()
		{
			return m_deviceCfg.nWidth;
		}

		UINT DX9Device::GetClientHeight()
		{
			return m_deviceCfg.nHeight;
		}

		D3DFORMAT DX9Device::GetDepthBufferFmt() const
		{
			return ZF2DX9(m_deviceCfg.depthFormat);
		}

		bool DX9Device::ScreenShot(const TCHAR* szPathName, BOOL bJPG)
		{
			if(m_pDevice==NULL)
				return false;

			//--得到rendertarget
			D3DSURFACE_DESC desc;
			m_pBackSurface->GetDesc(&desc);

			IRenderTexture* pRenderTarget = NewRenderTexture();
			if( false == pRenderTarget->Create(desc.Width,desc.Height,DX9FMT2Cool3D(desc.Format),false,false,1) )
			{
				SAFE_DELETE(pRenderTarget);
				return false;
			}

			IDirect3DSurface9* pVideoSurface=((DX9RenderTex*)pRenderTarget)->GetDX9Surface();
			if( !pVideoSurface )
			{
				SAFE_DELETE(pRenderTarget);
				return false;
			}

			HRESULT hr = m_pDevice->StretchRect(m_pBackSurface,NULL,pVideoSurface,NULL,D3DTEXF_POINT);
			if(FAILED(hr))
			{
				SAFE_DELETE(pRenderTarget);
				return false;
			}

			//--创建一个与target一样的surface
			IDirect3DSurface9* pSurface=NULL;
			hr=m_pDevice->CreateOffscreenPlainSurface(desc.Width,desc.Height,desc.Format,D3DPOOL_SYSTEMMEM,&pSurface,NULL);
			if(FAILED(hr))
			{
				SAFE_DELETE(pRenderTarget);
				return false;
			}

			//--copy data
			hr=m_pDevice->GetRenderTargetData(pVideoSurface, pSurface);
			if(FAILED(hr))
			{
				SAFE_DELETE(pRenderTarget);
				pSurface->Release();
				return false;
			}

			//--save to file
			if( bJPG )
				hr = D3DXSaveSurfaceToFile(szPathName,D3DXIFF_JPG,pSurface,NULL,NULL);
			else
				hr = D3DXSaveSurfaceToFile(szPathName,D3DXIFF_BMP,pSurface,NULL,NULL);

			//--release all
			pSurface->Release();
			SAFE_DELETE(pRenderTarget);
			return SUCCEEDED(hr);
		}

		void DX9Device::BitBltBackbuffer(HDC src,int x,int y,int w,int h)
		{
			if(m_pAddSurface==NULL)
				return;

			//m_pDevice->ColorFill(m_pAddSurface,NULL,0);
			HDC hdc=NULL;
			HRESULT hr=	m_pAddSurface->GetDC(&hdc);
			if(SUCCEEDED(hr))
			{
				BitBlt(hdc,x,y,w,h,src,0,0,SRCCOPY);
				m_pAddSurface->ReleaseDC(hdc);

				hr=m_pDevice->StretchRect(m_pAddSurface,NULL,
					m_pBackSurface,NULL,D3DTEXF_NONE);
			}
			//--
			/*D3DLOCKED_RECT br,ar;
			m_pBackSurface->LockRect(&br,NULL,0);
			m_pAddSurface->LockRect(&ar,NULL,0);

			FMemcpy(br.pBits,ar.pBits,ar.Pitch*m_deviceCfg.nHeight);

			m_pAddSurface->UnlockRect();
			m_pBackSurface->UnlockRect();
			*/
		}

		void DX9Device::CopyFromBackbuffer(IRenderTexture* pTex,const RECT* src,const RECT* dest)
		{
			ASSERT(pTex!=NULL);
			ASSERT(m_pBackSurface!=NULL);
			
			IDirect3DSurface9* pTargetSurrface=((DX9RenderTex*)pTex)->GetDX9Surface();
			ASSERT(pTargetSurrface!=NULL);

			HRESULT hr=DX9Dev()->StretchRect(m_pBackSurface,src,pTargetSurrface,dest,D3DTEXF_LINEAR);
			THROW_FAILED(hr,NULL);
		}

		void DX9Device::CreateBltBuffer()
		{
			D3DSURFACE_DESC bdsc;
			HRESULT hr=m_pBackSurface->GetDesc(&bdsc);
			hr=m_pDevice->CreateOffscreenPlainSurface(bdsc.Width,bdsc.Height,
				bdsc.Format,D3DPOOL_DEFAULT,&m_pAddSurface,NULL);
		}

		void DX9Device::ReleaseBltBuffer()
		{
			SAFE_RELEASE(m_pAddSurface);
		}
		
		void DX9Device::HandleDeviceLost()
		{
			HRESULT hr;
			if( FAILED( hr = m_pDevice->TestCooperativeLevel() ) )
			{
				//if( hr == D3DERR_DEVICELOST )
				//	return;
				if( hr == D3DERR_DEVICENOTRESET )
				{
					ThreadLock lock(&m_lock);

					PreDeviceReset();

					D3DPRESENT_PARAMETERS d3dpp=m_D3DPP;
					hr = m_pDevice->Reset( &d3dpp );
					THROW_FAILED(hr,_T("D3D Device Reset failed!"));

					PostDeviceReset();

					m_bDeviceResetFlag = true;
					m_bLost = false;

				}//endof if(can reset)

			}//endof if(failed)
		}

		void DX9Device::PreDeviceReset()
		{
			//--释放自身使用的两个surface
			SAFE_RELEASE(m_pBackSurface);
			SAFE_RELEASE(m_pAddSurface);

			//--处理所有Vertex buffer
			bool bNext = m_vbList.begin_iterator();
			while(bNext)
			{
				DX9VB *pVB = NULL;
				m_vbList.get_current(pVB);
				bNext = m_vbList.goto_next();

				pVB->PreDeviceReset();
			}
			m_vbList.end_iterator();

			//--所有index buffer
			bNext = m_ibList.begin_iterator();
			while(bNext)
			{
				DX9IB *pIB = NULL;
				m_ibList.get_current(pIB);
				bNext = m_ibList.goto_next();
				
				pIB->PreDeviceReset();
			}
			m_ibList.end_iterator();

			//--所有font
			for (list<DX9Font*>::iterator iter = m_fontList.begin();
				iter!=m_fontList.end();++iter)
			{
				DX9Font *pFont = (*iter);
				pFont->PreDeviceReset();
			}

			for (list<DX9DepthFont*>::iterator iter = m_dfontList.begin();
				iter!=m_dfontList.end();++iter)
			{
				DX9DepthFont *pFont = (*iter);
				pFont->PreDeviceReset();
			}

			//--所有的render texture
			list<DX9RenderTex*> listTemp;
			m_renderTexList.Export(listTemp);
			for (list<DX9RenderTex*>::iterator iter = listTemp.begin();
				iter!=listTemp.end();++iter)
			{
				DX9RenderTex *pRT = (*iter);
				pRT->PreDeviceReset();
			}

			for (list<DX9RenderCubeTex*>::iterator iter = m_renderCubeTexList.begin();
				iter!=m_renderCubeTexList.end();++iter)
			{
				DX9RenderCubeTex *pRT = (*iter);
				pRT->PreDeviceReset();
			}

			//--所有的VizQuery
			for (list<DX9VizQuery*>::iterator iter = m_vizQueryList.begin();
				iter!=m_vizQueryList.end();++iter)
			{
				DX9VizQuery *pVizQuery = (*iter);
				pVizQuery->PreDeviceReset();
			}
		}

		void DX9Device::PostDeviceReset()
		{
			//--重建两个surface
			HRESULT hr=m_pDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&m_pBackSurface);

			if(m_pAddSurface != NULL)//如果刚才是有的
				CreateBltBuffer();

			//--处理所有Vertex buffer
			bool bNext = m_vbList.begin_iterator();
			while(bNext)
			{
				DX9VB *pVB = NULL;
				m_vbList.get_current(pVB);
				bNext = m_vbList.goto_next();

				pVB->PostDeviceReset();
			}
			m_vbList.end_iterator();

			//--所有index buffer
			bNext = m_ibList.begin_iterator();
			while(bNext)
			{
				DX9IB *pIB = NULL;
				m_ibList.get_current(pIB);
				bNext = m_ibList.goto_next();

				pIB->PostDeviceReset();
			}
			m_ibList.end_iterator();

			//--所有font
			for (list<DX9Font*>::iterator iter = m_fontList.begin();
				iter!=m_fontList.end();++iter)
			{
				DX9Font *pFont = (*iter);
				pFont->PostDeviceReset();
			}

			for (list<DX9DepthFont*>::iterator iter = m_dfontList.begin();
				iter!=m_dfontList.end();++iter)
			{
				DX9DepthFont *pFont = (*iter);
				pFont->PostDeviceReset();
			}

			//--所有的render texture
			list<DX9RenderTex*> listTemp;
			m_renderTexList.Export(listTemp);
			for (list<DX9RenderTex*>::iterator iter = listTemp.begin();
				iter!=listTemp.end();++iter)
			{
				DX9RenderTex *pRT = (*iter);
				pRT->PostDeviceReset();
			}

			for (list<DX9RenderCubeTex*>::iterator iter = m_renderCubeTexList.begin();
				iter!=m_renderCubeTexList.end();++iter)
			{
				DX9RenderCubeTex *pRT = (*iter);
				pRT->PostDeviceReset();
			}

			//--所有的VizQuery
			for (list<DX9VizQuery*>::iterator iter = m_vizQueryList.begin();
				iter!=m_vizQueryList.end();++iter)
			{
				DX9VizQuery *pVizQuery = (*iter);
				pVizQuery->PostDeviceReset();
			}


			//--重新设置一下渲染状态
			for(int i=0;i<ERS_Num;i++)
				SetRenderState(ERStateType(i),m_dwCurState[i],true);

			//--清空其它状态
			memset(m_pCurVB,NULL,sizeof(m_pCurVB));
			m_pCurIB=NULL;
			memset(m_pCurTex,NULL,sizeof(m_pCurTex));
			memset(m_curTexAddU,ETAddress_Invalid,sizeof(m_curTexAddU));
			memset(m_curTexAddV,ETAddress_Invalid,sizeof(m_curTexAddV));
			memset(m_curTexAddW,ETAddress_Invalid,sizeof(m_curTexAddW));
			memset(m_curTexMagFilter,ETFilter_Invalid,sizeof(m_curTexMagFilter));
			memset(m_curTexMinFilter,ETFilter_Invalid,sizeof(m_curTexMinFilter));
			memset(m_curTexMipFilter,ETFilter_Invalid,sizeof(m_curTexMipFilter));
			memset(m_curTexColorOp,ETOP_Invalid,sizeof(m_curTexColorOp));
			memset(m_curTexAlphaOp,ETOP_Invalid,sizeof(m_curTexAlphaOp));
			memset(m_curTexColorArg1,ETA_Invalid,sizeof(m_curTexColorArg1));
			memset(m_curTexColorArg2,ETA_Invalid,sizeof(m_curTexColorArg2));
			memset(m_curTexAlphaArg1,ETA_Invalid,sizeof(m_curTexAlphaArg1));
			memset(m_curTexAlphaArg2,ETA_Invalid,sizeof(m_curTexAlphaArg2));
			memset(m_curTexUVIndex,-1,sizeof(m_curTexUVIndex));
			memset(m_curTexUVIndexFlags,EUVI_Invalid,sizeof(m_curTexUVIndexFlags));
			memset(m_curTexUVTrans,EUVT_Invalid,sizeof(m_curTexUVTrans));

			m_pCurVS=NULL;
			m_pCurPS=NULL;
			m_pCurStreamDef=NULL;

			//--重新获得当前材质
			ASSERT(sizeof(D3DMATERIAL9)==sizeof(tagRenderColor));
			m_pDevice->GetMaterial((D3DMATERIAL9*)&m_curRenderColor);
		}

		void DX9Device::OnVBFree(DX9VB *pVB)
		{
			m_vbList.remove(pVB);
		}

		void DX9Device::OnIBFree(DX9IB *pIB)
		{
			m_ibList.remove(pIB);
		}

		void DX9Device::OnFontFree(DX9Font *pFont)
		{
			m_fontList.remove(pFont);
		}

		void DX9Device::OnDepthFontFree(DX9DepthFont *pFont)
		{
			m_dfontList.remove(pFont);
		}

		void DX9Device::OnRenderTexFree(DX9RenderTex *pRenderTex)
		{
			m_renderTexList.Erase(pRenderTex);
		}

		void DX9Device::OnRenderCubeTexFree(DX9RenderCubeTex *pRenderTex)
		{
			m_renderCubeTexList.remove(pRenderTex);
		}

		void DX9Device::OnVizQueryFree(DX9VizQuery *pVizQuery)
		{
			m_vizQueryList.remove(pVizQuery);
		}

		void DX9Device::SetDepthBias(float bias)
		{
			SetRenderState(ERS_DepthBias,FtoDW(bias));
		}

		void DX9Device::SetDepthBiasScale(float biasScale)
		{
			SetRenderState(ERS_DepthBiasScale,FtoDW(biasScale));
		}

		bool DX9Device::EnumDisplayModes( std::list<tagDisplayMode>& ModeList, const UINT nMinW, const UINT nMinH )
		{
			if( NULL == m_pD3D )
				return false;

			ModeList.clear();

			const D3DFORMAT szFormat[] = 
			{
				D3DFMT_R5G6B5,
				D3DFMT_A8R8G8B8,
				D3DFMT_X8R8G8B8
			};

			const UINT nFormatCount  = sizeof(szFormat) / sizeof(szFormat[0]);
			for( UINT nFormat = 0; nFormat < nFormatCount; nFormat++ )
			{
				UINT nModeCount = m_pD3D->GetAdapterModeCount( D3DADAPTER_DEFAULT, szFormat[nFormat] );
				for( UINT nMode = 0; nMode < nModeCount; nMode++ )
				{
					D3DDISPLAYMODE D3DMode;
					m_pD3D->EnumAdapterModes( D3DADAPTER_DEFAULT, szFormat[nFormat], nMode, &D3DMode );
					if( D3DMode.Width  < nMinW  ||
						D3DMode.Height < nMinH )
						continue;

					tagDisplayMode Mode;
					Mode.nWidth = D3DMode.Width;
					Mode.nHeight = D3DMode.Height;
					Mode.nRefreshRate = D3DMode.RefreshRate;
					Mode.eFormat = DX9FMT2Cool3D( D3DMode.Format );
					ModeList.push_back( Mode );
				}
			}
			return true;
		}

		bool DX9Device::CapsVizQuery()
		{
			ASSERT(m_pDevice);
			HRESULT hr = m_pDevice->CreateQuery(D3DQUERYTYPE_OCCLUSION,NULL);
			return SUCCEEDED( hr );
		}

		UINT DX9Device::GetAdapterCount()
		{
			ASSERT(m_pD3D);
			return m_pD3D->GetAdapterCount();
		}

		bool DX9Device::GetAdapterIdentifier( UINT nAdapter, D3DADAPTER_IDENTIFIER9* pIdentifier )
		{
			if( NULL == m_pD3D )
				return false;

			if( nAdapter >= m_pD3D->GetAdapterCount() )
				return false;

			HRESULT hr = m_pD3D->GetAdapterIdentifier( nAdapter,0, pIdentifier );
			return SUCCEEDED( hr );
		}

		UINT DX9Device::GetAvailableTextureMem()
		{
			if( NULL == m_pDevice )
				return 0;
			return m_pDevice->GetAvailableTextureMem();
		}
	}
}//endof namespace