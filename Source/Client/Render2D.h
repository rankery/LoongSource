//-----------------------------------------------------------------------------
//!\file render2d.h
//!\author Lyp
//!
//!\date 2008-03-27
//! last 2008-04-19
//!
//!\brief 龙客户端2d界面渲染接口
//!
//!	Copyright (c) 2008 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// GUIRender
//-----------------------------------------------------------------------------
class GUIRender : public IGUIRender
{
public:

	VOID Init();
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
	virtual VOID PushRenderTarget(const tagGUIImage* pDest);
	virtual VOID PopRenderTarget(const tagGUIImage* pDest);
	virtual VOID ClearRenderTarget();
	virtual DWORD CreateRenderTarget(const tagRect& rc);
	virtual VOID ReleaseRenderTarget(DWORD dwHandle);

	// 设置剪裁区域
	virtual VOID SetScissorRect(tagRect* pRect);

	// 播放声音
	virtual VOID PlaySound(LPCTSTR szName, FLOAT fVolume=1.0f);

	DWORD	GetRTSize() { return m_dwRTSize; }

	~GUIRender();


private:
	TObjRef<Util>					m_pUtil;
	TObjRef<VirtualFileSys>			m_pVFS;
	TObjRef<VirtualFileSys>			m_pVFSDefault;
	TObjRef<AudioSys>				m_pAudioSys;

	TMap<DWORD, tagGUIFontEx*>		m_mapFont;
	TMap<DWORD, tagGUIImageHandle*>	m_mapImageHandle;

	IFont*							m_pDefaultIFont;
	tagRect							m_rcWindow;

	DWORD							m_dwRTSize;

	TMap<DWORD, tstring>			m_mapDebug;
};