#pragma once
#include "..\idevice.h"
#include "..\..\Kernel.h"
#include "..\RenderStatus.h"

#include "..\..\Thread\TSList.h"
#include "..\..\Thread\FastLock.h"
#include "..\..\EditorSupport\safe_list.h"
#define RENDER_STATUS

namespace Cool3D
{
	namespace Detail
	{
		const UINT MAX_VertStream=8;
		const UINT MAX_Sampler=16;
		struct DX9ClearParam
		{
			DWORD flags;
			DWORD color;
			float z;
			DWORD stencil;
		};

		class DX9VB;
		class DX9IB;
		class DX9Font;
		class DX9DepthFont;
		class DX9RenderTex;
		class DX9RenderCubeTex;
		class DX9VizQuery;

		class DX9Device : public IDevice
		{
		public:
			DX9Device(void);
			virtual ~DX9Device(void);

			virtual bool Create(tagDeviceCfg *pCfg);
			virtual void Destroy();

			virtual bool Draw(EDrawType type,UINT numPrimitive);
			virtual bool DrawMem(EDrawType type,UINT numPrimitive,void *pVert,UINT nStride);
			virtual bool DrawIndexed(EDrawType type,UINT baseIndex,UINT numVert,UINT numPrimitive,UINT startIndex=0);
			virtual bool DrawPointSprites(float pointSize,int numPoints);
			virtual void ColorFill(const RECT* pRect,DWORD color);
			
			virtual void SetClearParam(bool target,bool depth, bool stencil,DWORD color,float z,DWORD stencilVal);
			virtual void Clear();
			virtual void Clear(const RECT* pRect);
			virtual void Clear(bool target,bool depth, bool stencil);
			virtual void Clear(bool target,bool depth, bool stencil,DWORD color,float z,DWORD stencilVal,const RECT* pRect=NULL);
			virtual bool Begin();
			virtual void End();
			virtual void Present(HWND hDestWnd=NULL);

			virtual void SetDepthFunc(ECmpFunc zFun);
			virtual void EnableDepthTest(bool bEnable);
			virtual void EnableDepthWrite(bool bEnable);

			virtual void SetCullMode(ECullMode cullMode);

			virtual void SetFillMode(EFillMode fillMode);
			virtual void SetShadeMode(EShadeMode shadeMode);

			virtual void EnableAlphaTest(bool bEnable);
			virtual void SetAlphaTestFunc(ECmpFunc func);
			virtual void SetAlphaRef(BYTE ref);

			virtual void EnableAlphaBlend(bool bEnable);
			virtual void SetAlphaBlend(EBlend src,EBlend dst);

			virtual void EnableStenciling(bool bEnable);
			virtual void SetStencilFunc(ECmpFunc func, int refVal, DWORD mask);
			virtual void SetStencilOp(EStencilOP fail,EStencilOP zFail,EStencilOP pass);

			virtual void SetRenderState(ERStateType state, DWORD val, bool bForce=false);
			virtual DWORD GetRenderState(ERStateType state);

			virtual void SetSimplerAddress(DWORD sampler,ETextureAddress addU,ETextureAddress addV,ETextureAddress addW);
			virtual void SetSimplerFilter(DWORD sampler,ETextrueFilter mag,ETextrueFilter min);
			virtual void SetSimplerMipFilter(DWORD sampler,ETextrueFilter mip);

			virtual IVertBuffer *NewVB();
			virtual IIndexBuffer *NewIB();
			virtual IDeviceTex* NewTexture();
			virtual IRenderTexture* NewRenderTexture();
			virtual IRenderCubeTexture* NewRenderCubeTexture();
			virtual IVertShader* NewVS();
			virtual IPixelShader* NewPS();
			virtual IFont* NewFont();
			virtual IDepthFont *NewDepthFont();
			virtual IStreamDefine* NewStreamDefine();
			virtual IRenderToEnvMap* NewRenderToEnvMap();
			virtual IDeviceVizQuery* NewVizQuery();
			virtual void SetStreamDefine(IStreamDefine *pDef);
			virtual void SetVB(DWORD stream,IVertBuffer *pVB);
			virtual void SetIB(IIndexBuffer *pIB);
			virtual void SetTexture(DWORD sample,IDeviceTex *pTex);
			virtual void SetTexture(DWORD sample,IRenderTexture *pTex);
			virtual void SetTexture(DWORD sample,IRenderCubeTexture *pTex);
			virtual void SetVertShader(IVertShader *pVS);
			virtual void SetPixelShader(IPixelShader *pPS);

			virtual Matrix4* GetMatrix(EMatrixStackType type);
			virtual void MatPush(EMatrixStackType type);
			virtual void MatPop(EMatrixStackType type);
			virtual void MatLoad(EMatrixStackType type, const Matrix4 *pMat);

			virtual void SetScissorRect(const RECT *pRect);
			virtual bool GetClipPlane(Plane *pPlane);
			virtual void SetClipPlane(Plane *pPlane);
			virtual void GetViewport(tagViewport& viewport);
			virtual void SetViewport(const tagViewport* pViewport);
			virtual void SetDepthBias(float bias);
			virtual void SetDepthBiasScale(float biasScale);

			virtual void FFlushMatrix(bool world=true,bool view=true,bool prj=true,bool tex=true,DWORD texIndex=ETM_All);
			virtual UINT FGetMaxTextureStage();
			virtual void FSetTextureColorOp(DWORD stage,ETextureOp op,ETextureArg arg1,ETextureArg arg2);
			virtual void FSetTextureAlphaOp(DWORD stage,ETextureOp op,ETextureArg arg1,ETextureArg arg2);
			virtual void FSetTextureColorOp(DWORD stage,ETextureOp op);
			virtual void FSetTextureAlphaOp(DWORD stage,ETextureOp op);
			virtual void FSetTexCoordSrc(DWORD stage,UINT uvIndex,DWORD flags=0);
			virtual void FSetTexBumpMat(DWORD stage,const Matrix2& mat);
			virtual void FEnableFog(bool bEnable);
			virtual void FSetFog(DWORD color,float start,float end,float density);
			virtual void FSetFogFormula(EFogFormula fm);
			
			virtual void FEnableLight(bool bEnable);
			virtual void FEnableSpecular(bool bEnable);
			virtual void FSetLight(DWORD index,const tagLight *pLight);
			virtual void FSetRenderColor(const tagRenderColor* pColor);
			virtual void FSetUVTrans(DWORD stage,EUVTrans trans);
			
			virtual UINT GetClientWidth();
			virtual UINT GetClientHeight();
			virtual bool ScreenShot(const TCHAR* szPathName, BOOL bJPG=FALSE);
			
			virtual void BitBltBackbuffer(HDC src,int x,int y,int w,int h);
			virtual void CreateBltBuffer();
			virtual void ReleaseBltBuffer();

			virtual void CopyFromBackbuffer(IRenderTexture* pTex,const RECT* src,const RECT* dest);

			IDirect3DDevice9 *GetD3DDevice()
			{ return m_pDevice; }
			virtual RenderStatus *GetRenderStatus()		{	return m_pRenderStatus;}

			virtual int FCapsMaxSimultaneousTextures()	{	return m_caps.MaxSimultaneousTextures;}
			virtual int CapsMaxTextureStages()			{	return m_caps.MaxTextureBlendStages;}
			virtual DWORD CapsPixelShaderVersion()		{   return m_caps.PixelShaderVersion;}
			virtual DWORD CapsVertexShaderVersion()		{	return m_caps.VertexShaderVersion;}
			virtual bool CapsBumpEnvMap()				{   return (m_caps.TextureOpCaps&D3DTEXOPCAPS_BUMPENVMAP)!=0;}
			virtual bool CapsTexProjected()				{   return (m_caps.TextureCaps&D3DPTEXTURECAPS_PROJECTED)!=0;}
			virtual bool CapsBorderTexWrapMode()		{	return (m_caps.TextureAddressCaps&D3DPTADDRESSCAPS_BORDER)!=0;}
			virtual bool CapsColorWriteEnable()         {   return (m_caps.PrimitiveMiscCaps&D3DPMISCCAPS_COLORWRITEENABLE)!=0;}
			virtual bool CapsVizQuery();

			D3DFORMAT GetDepthBufferFmt() const;
			D3DMULTISAMPLE_TYPE GetMutiSampleType() const { return m_D3DPP.MultiSampleType; }
			DWORD GetMultisampleQuality() const { return m_D3DPP.MultiSampleQuality; }

			void OnVBFree(DX9VB *pVB);
			void OnIBFree(DX9IB *pIB);
			void OnFontFree(DX9Font *pFont);
			void OnDepthFontFree(DX9DepthFont *pFont);
			void OnRenderTexFree(DX9RenderTex *pRenderTex);
			void OnRenderCubeTexFree(DX9RenderCubeTex *pRenderTex);
			void OnVizQueryFree(DX9VizQuery *pVizQuery);

			virtual bool EnumDisplayModes( std::list<tagDisplayMode>& ModeList, const UINT nMinW, const UINT nMinH );

			virtual bool GetDeviceResetFlag(){ return m_bDeviceResetFlag;}
			virtual void ClearDeviceResetFlag(){ m_bDeviceResetFlag=false;}

			virtual LockableObject* GetResetLock(){ return &m_lock;}

			virtual UINT GetAdapterCount();
			virtual bool GetAdapterIdentifier( UINT nAdapter, D3DADAPTER_IDENTIFIER9* pIdentifier );
			virtual UINT GetAvailableTextureMem();
		private:
			void HandleDeviceLost();
			void PreDeviceReset();
			void PostDeviceReset();
			void ClearCurrentState();
			void SetDefaultState();
		private:
			RenderStatus		*m_pRenderStatus;
			tagDeviceCfg		m_deviceCfg;

			DX9ClearParam		m_clearParam;
			IDirect3DDevice9	*m_pDevice;
			IDirect3D9			*m_pD3D;// Direct3D9 Object
			D3DCAPS9			m_caps;	//创建之后得到此结构体
			D3DPRESENT_PARAMETERS	m_D3DPP;
			
			IDirect3DSurface9	*m_pBackSurface;
			IDirect3DSurface9	*m_pAddSurface;


			ID3DXMatrixStack	*m_matStack[EMS_Num];

			//--为了减少DX API调用
			DWORD				m_dwCurState[ERS_Num];
			IVertBuffer			*m_pCurVB[MAX_VertStream];
			IIndexBuffer		*m_pCurIB;
			IDirect3DBaseTexture9	*m_pCurTex[MAX_Sampler];
			ETextureAddress		m_curTexAddU[MAX_Sampler];
			ETextureAddress		m_curTexAddV[MAX_Sampler];
			ETextureAddress		m_curTexAddW[MAX_Sampler];
			ETextrueFilter		m_curTexMagFilter[MAX_Sampler];
			ETextrueFilter		m_curTexMinFilter[MAX_Sampler];
			ETextrueFilter		m_curTexMipFilter[MAX_Sampler];
			ETextureOp			m_curTexColorOp[MAX_Sampler];
			ETextureOp			m_curTexAlphaOp[MAX_Sampler];
			ETextureArg			m_curTexColorArg1[MAX_Sampler];
			ETextureArg			m_curTexColorArg2[MAX_Sampler];
			ETextureArg			m_curTexAlphaArg1[MAX_Sampler];
			ETextureArg			m_curTexAlphaArg2[MAX_Sampler];
			UINT				m_curTexUVIndex[MAX_Sampler];
			DWORD				m_curTexUVIndexFlags[MAX_Sampler];
			EUVTrans			m_curTexUVTrans[MAX_Sampler];
			IVertShader			*m_pCurVS;
			IPixelShader		*m_pCurPS;
			IStreamDefine		*m_pCurStreamDef;
			tagRenderColor		m_curRenderColor;

			//--为了处理device lost
			bool				m_bLost;//是否Device Lost状态
			FastLock			m_lock;//Device Reset锁

			TSList<DX9VB*>				m_vbList;
			TSList<DX9IB*>				m_ibList;

			list<DX9Font*>				m_fontList;
			list<DX9DepthFont*>			m_dfontList;
			TSafeList<DX9RenderTex*>	m_renderTexList;
			list<DX9RenderCubeTex*>		m_renderCubeTexList;
			list<DX9VizQuery*>			m_vizQueryList;

			bool				m_bDeviceResetFlag;
		};

		inline DX9Device* MyDX9Dev()
		{
			return static_cast<DX9Device*>(Device());
		}

		inline IDirect3DDevice9* DX9Dev()
		{
			DX9Device *pDev=static_cast<DX9Device*>(Device());
			return pDev->GetD3DDevice();
		}
	}
}//endof namespace