//-----------------------------------------------------------------------------
// File: Display
// Desc: 2d render interface
// Auth: Lyp
// Date: 2003/11/13
// Last: 2207/12/19
//-----------------------------------------------------------------------------
#pragma once
namespace vEngine {

enum EPixelFormat
{
	EPF_565,
	EPF_555,
	EPF_888,
	EPF_End
};

// 各种色彩转换宏
#define GET_RGB_R(rgb)	((rgb & 0xff0000) >> 16)
#define GET_RGB_G(rgb)	((rgb & 0xff00) >> 8)
#define GET_RGB_B(rgb)	((rgb & 0xff))
#define _RGB565(r, g, b)	((WORD)((r) >> 3 << 11) | ((g) >> 2 << 5) | ((b) >> 3))
#define _RGB555(r, g, b)	((WORD)((r) >> 3 << 10) | ((g) >> 3 << 5) | ((b) >> 3))
#define RGB565(rgb)	_RGB565(GET_RGB_R(rgb), GET_RGB_G(rgb), GET_RGB_B(rgb))
#define RGB555(rgb)	_RGB555(GET_RGB_R(rgb), GET_RGB_G(rgb), GET_RGB_B(rgb))

//-----------------------------------------------------------------------------
// display interface
//-----------------------------------------------------------------------------
class IRender
{
public:
	virtual BOOL	Init(INT nWidth, INT nHeight)=0;
	virtual VOID	Destroy()=0;

	virtual DWORD	CreateSurface(INT nWidth, INT nHeight, BOOL bAlpha, BOOL bTarget)=0;
	virtual VOID	ReleaseSurface(DWORD dwHandle)=0;

	//!\param 如果!P_VALID(dwHandle)代表要取得后台缓冲的HDC
	virtual DWORD	GetDC(DWORD dwHandle)=0;
	virtual VOID	ReleaseDC(DWORD dwHandle, DWORD dwDC)=0;

	virtual VOID	Clear(DWORD dwHandle, const tagRect* prc, DWORD dwColor)=0;
	virtual VOID	Draw(DWORD dwDestHandle, DWORD dwSrcHandle, const tagRect* prcDest, 
		const tagRect* prcSrc, DWORD dwColor)=0;

	virtual BOOL	Render()=0;


	// 将窗口client坐标转换为窗口view坐标(view坐标为逻辑坐标)
	virtual VOID	ClientToView(tagPoint& pt)=0;
	virtual VOID	UpdateWindowPos()=0;

	// 颜色格式的转化 GDI : 0xaarrggbb -> 0x00bbggrr
	__forceinline DWORD	ARGB2GDI(DWORD dwColor32)
	{return (dwColor32 & 0x00ff00)|((dwColor32&0xff0000)>>16)|((dwColor32 & 0xff) << 16);}


	virtual VOID SwitchFullScreenMenu();
	virtual VOID SetShowFPS(BOOL bShow);
	
	virtual INT		GetWidth() { return m_nWidth; }
	virtual INT		GetHeight() { return m_nHeight; }

	IRender();
	virtual ~IRender();

protected:
	TObjRef<VarContainer>	m_pGlobeVar;

	INT						m_nWidth;
	INT						m_nHeight;
	tagRect					m_rcWindow;

	BOOL					m_bShowFullScreenMenu;
	BOOL					m_bShowFPS;
};


} // namespace vEngine {
