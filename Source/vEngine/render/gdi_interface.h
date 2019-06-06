//-----------------------------------------------------------------------------
// File: gdi_interface
// Desc: gdi interface
// Auth: Lyp
// Date: 2007/1/30
// Last: 2007/1/30
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {
typedef BOOL (WINAPI fpAlphaBlend)(HDC,int,int,int,int,HDC,int,int,int,int,BLENDFUNCTION);
//-----------------------------------------------------------------------------
// interface for microsoft gdi
//-----------------------------------------------------------------------------
class GDIRender : public IRender
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


	GDIRender();
	~GDIRender();

private:

	HWND	m_hWnd;
	HDC		m_hdcBackBuff;
	HBITMAP	m_hbmpBackBuff;
	INT		m_nBitsPerPel;

	HMODULE m_hDllModule;
	fpAlphaBlend* m_fpAlphaBlend;

};


}	// namespace vEngine {
