//-----------------------------------------------------------------------------
//!\file gui_render.h
//!\author Lyp
//!
//!\date 2004-12-09
//! last 2007-12-19
//!
//!\brief 界面系统对外渲染接口
//-----------------------------------------------------------------------------
#pragma once

namespace vEngine {

struct tagGUIImage
{
	DWORD		dwID;
	DWORD		dwHandle;	// 句柄
	tagRect		rc;			// 区域
	tagPoint	ptSize;		// 大小
	tagPoint	ptOffset;	// Render偏移
	BOOL		bRefresh;	// 是否需要重绘
	BOOL		bRenderTarget;
	tagGUIImage():dwID(GT_INVALID), dwHandle(GT_INVALID),bRefresh(TRUE),bRenderTarget(FALSE){}
};

struct tagGUIFont
{
	DWORD		dwID;
	DWORD		dwHandle;	// 句柄
	tagGUIFont():dwID(GT_INVALID), dwHandle(GT_INVALID){}
};

enum EGUITextAlign	// 文字对齐方式
{
	EGUITA_LeftTop = 0,		// 左上
	EGUITA_CenterTop,		// 中上
	EGUITA_RightTop,		// 右上
	EGUITA_LeftCenter,		// 左中
	EGUITA_Center,			// 正中
	EGUITA_RightCenter,		// 右中
	EGUITA_LeftBottom,		// 左下
	EGUITA_CenterBottom,	// 下中
	EGUITA_RightBottom,		// 右下
};

enum EGUIAlphaWrite
{
	EGUIAW_NoWrite = 0,		// 不写Alpha通道
	EGUIAW_Write,			// 写Alpha通道
	EGUIAW_Add,				// Alpha相加
};

//-----------------------------------------------------------------------------
//!\brief 界面系统需要的渲染接口
//-----------------------------------------------------------------------------
class VENGINE_API IGUIRender
{
public:
	// 读入图片，失败返回0,成功返回句柄(如果strImage.empty()则返回NULL)
	// 如果无法读取指定图片则返回可以设置为RenderTarget的空纹理
	virtual tagGUIImage* CreateImage(const tstring str, const tagRect& rc)=0;
	// 删除图片,pImage可以为空
	virtual VOID DestroyImage(tagGUIImage* pImage)=0;

	// 建立字体
	virtual tagGUIFont* CreateFont(const tstring str, INT nWidth, INT nHeight, INT nWeight, BOOL bAntiAliase=TRUE)=0;
	// 克隆字体,一般是另一个控件也使用同样字体
	virtual tagGUIFont* CloneFont(tagGUIFont* pFont)=0;
	// 删除字体,pFont可以为空
	virtual VOID DestroyFont(tagGUIFont* pFont)=0;

	// 画图,如果!P_VALID(pDest->dwHandle)则渲染到后台缓冲, pSrc可以为空
	virtual VOID Draw(const tagGUIImage* pDest, const tagGUIImage* pSrc, DWORD dwColor, EGUIAlphaWrite eAlphaWrite, FLOAT z=1.0f)=0;
	// 画字,pFont可以为空
	virtual VOID Text(const tagGUIImage* pDest, const tstring& szText, tagGUIFont* pFont, 
		DWORD dwColor, DWORD dwShadeColor, EGUITextAlign eAlign, BOOL bWriteAlpha)=0;
	// 画线
	virtual VOID Line(const tagGUIImage* pDest, DWORD dwColor, BOOL bBox=FALSE)=0;

	// 得到文本大小,pFont可以为空
	virtual VOID GetTextSize(const LPCTSTR szText, tagGUIFont* pFont, tagPoint& ptSize)=0;

	// 将屏幕坐标转换为窗口view坐标
	virtual VOID ClientToView(tagPoint& pt)=0;

	// 刷新窗口坐标
	virtual VOID UpdateWindowPos()=0;

	// 刷新屏幕(底层只用把后台缓冲拷贝到前台缓冲即可)
	virtual VOID Present()=0;

	// 设置render target
	virtual VOID PushRenderTarget(const tagGUIImage* pDest)=0;
	virtual VOID PopRenderTarget(const tagGUIImage* pDest)=0;
	virtual VOID ClearRenderTarget()=0;
	virtual DWORD CreateRenderTarget(const tagRect& rc)=0;
	virtual VOID ReleaseRenderTarget(DWORD dwHandle)=0;

	// 设置剪裁区域
	virtual VOID SetScissorRect(tagRect* pRect)=0;

	// 播放声音
	virtual VOID PlaySound(LPCTSTR szName, FLOAT fVolume=1.0f)=0;

	virtual ~IGUIRender(){}


};




//-----------------------------------------------------------------------------
//!\brief vEngine内部实现的界面系统渲染接口
//-----------------------------------------------------------------------------
struct tagGUIImageHandle
{
	DWORD		dwHandle;
	tagPoint	ptSize;
	INT			nRefCount;
	BOOL		bRenderTarget;
	tagGUIImageHandle():dwHandle(GT_INVALID), nRefCount(0), bRenderTarget(FALSE){}
};
struct tagGUIFontEx : public tagGUIFont
{
	INT	nRefCount;	// 引用计数
	tagGUIFontEx():nRefCount(0){}
};


class VENGINE_API IGUIRenderNative : public IGUIRender
{
public:
	BOOL Init(IRender* pRender);

	virtual tagGUIImage* CreateImage(const tstring str, const tagRect& rc);
	virtual VOID DestroyImage(tagGUIImage* pImage);
	virtual tagGUIFont* CreateFont(const tstring str, INT nWidth, INT nHeight, INT nWeight, BOOL bAntiAliase=TRUE);
	virtual tagGUIFont* CloneFont(tagGUIFont* pFont);
	virtual VOID DestroyFont(tagGUIFont* pFont);
	virtual VOID Draw(const tagGUIImage* pDest, const tagGUIImage* pSrc, DWORD dwColor, EGUIAlphaWrite eAlphaWrite, FLOAT z=1.0f);
	virtual VOID Text(const tagGUIImage* pDest, const tstring& szText, tagGUIFont* pFont, 
		DWORD dwColor, DWORD dwShadowColor, EGUITextAlign eAlign, BOOL bWriteAlpha);
	virtual VOID Line(const tagGUIImage* pDest, DWORD dwColor, BOOL bBox=FALSE);
	virtual VOID GetTextSize(const LPCTSTR szText, tagGUIFont* pFont, tagPoint& ptSize);
	virtual VOID ClientToView(tagPoint& pt);
	virtual VOID UpdateWindowPos();
	virtual VOID Present();

	// 设置render target
	virtual VOID PushRenderTarget(const tagGUIImage* pDest){ m_pCurrentDest = (tagGUIImage*)pDest; }
	virtual VOID PopRenderTarget(const tagGUIImage* pDest) { m_pCurrentDest = NULL; }
	virtual VOID ClearRenderTarget();
	virtual VOID EnableAlphaWrite(){};	// 只有renderTarget的背景绘制需要修改Alpha通道
	virtual VOID DisableAlphaWrite(){};
	virtual DWORD CreateRenderTarget(const tagRect& rc);
	virtual VOID ReleaseRenderTarget(DWORD dwHandle);

	// 设置剪裁区域
	virtual VOID SetScissorRect(tagRect* pRect){}

	// 播放声音
	virtual VOID PlaySound(LPCTSTR szName, FLOAT fVolume=1.0f){}

private:
	IRender*						m_pRender;
	TObjRef<ImageObj>				m_pImageObj;
	TObjRef<Util>					m_pUtil;

	TMap<DWORD, tagGUIFontEx*>		m_mapFont;
	TMap<DWORD, tagGUIImageHandle*>	m_mapImageHandle;
	tagGUIImage*					m_pCurrentDest;

};

}	// namespace vEngine {
