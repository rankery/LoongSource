//-----------------------------------------------------------------------------
//!\file d3d_interface.h
//!\author Lyp
//!
//!\date 2002-03-08
//! last 2005-11-18
//!
//!\brief d3d interface
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {
#ifdef _USE_D3D7_
//-------------------------------------------------------------------------
// 匹配纹理搜索结构
//-------------------------------------------------------------------------
struct TEXTURESEARCHINFO
{
	DWORD dwDesiredBPP;   // Input for texture format search
	DWORD dwAlphaBitMask;
	DDPIXELFORMAT* pddpf; // Output of texture format search
};

#define LPSURFACE LPDIRECTDRAWSURFACE7
//-----------------------------------------------------------------------------
//!\brief d3d interface
//!	Surface :	一般指离屏表面
//!	Buffer	: 	指专用表面缓冲区
//-----------------------------------------------------------------------------
class D3DRender : public IRender
{
public:
	virtual BOOL	Init(INT nWidth, INT nHeight);
	virtual VOID	Destroy();

	virtual DWORD	CreateSurface(INT nWidth, INT nHeight, BOOL bAlpha, BOOL bTarget);
	virtual VOID	ReleaseSurface(DWORD dwHandle);

	//!\param 如果!P_VALID(dwSurfaceHandle)代表要取得后台缓冲的HDC
	virtual DWORD	GetDC(DWORD dwHandle);
	virtual VOID	ReleaseDC(DWORD dwHandle, DWORD dwDC);

	virtual VOID	Clear(DWORD dwHandle, const tagRect* prc, DWORD dwColor);
	virtual VOID	Draw(DWORD dwDestHandle, DWORD dwSrcHandle, const tagRect* prcDest, 
		const tagRect* prcSrc, DWORD dwColor);

	virtual BOOL	Render();


	// 将窗口client坐标转换为窗口view坐标(view坐标为逻辑坐标)
	virtual VOID	ClientToView(tagPoint& pt);
	virtual VOID	UpdateWindowPos();



    D3DRender();
	~D3DRender();

private:

	GUID				m_GUIDDevice;			// 设备的GUID

    LPDIRECTDRAW7		m_pDD;					// 主显示对象
 	LPDIRECT3D7			m_pD3D;					// 三维显示对象
    LPDIRECT3DDEVICE7	m_pd3dDevice;			// 三维显示设备

	LPSURFACE			m_pddsFrontBuffer;		// 前台缓冲区指针
    LPSURFACE			m_pddsBackBuffer;		// 后台缓冲区指针

	DDPIXELFORMAT		m_PixelFormat;			// 普通纹理格式
	DDPIXELFORMAT		m_PixelFormat32;		// 32位纹理格式
   
	LPSURFACE			m_pLastTexture;		// 优化用
	LPSURFACE			m_pLastTarget;
	D3DTLVERTEX			m_Mesh4[4];
    HWND				m_hWnd;
   

	HRESULT	CreateWindowedDisplay(INT nWidth, INT nHeight);

	// 调整纹理大小
	VOID	SetD3DState();	// 设置三维Rander/Texture状态
  
	HRESULT	GetCaps(bool bRunTest);	// 得到硬件设备能力
	// 纹理枚举回调
	static HRESULT CALLBACK TextureSearchCallback(DDPIXELFORMAT* pddpf,VOID* param);

};



__forceinline void SetTLVertex(D3DTLVERTEX *pTLVertex, D3DVALUE x, D3DVALUE y, 
							   D3DVALUE tu, D3DVALUE tv, D3DCOLOR color)
{																
	pTLVertex->sx = x;
    pTLVertex->sy = y;
    pTLVertex->tu = tu;
	pTLVertex->tv = tv;
	pTLVertex->dcColor = color;
}


#endif
} // namespace vEngine {
