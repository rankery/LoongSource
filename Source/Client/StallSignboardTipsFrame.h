#pragma once
#include "gameframe.h"

struct tagStall;

class StallSignboardTipsFrame :
	public GameFrame
{
public:
	StallSignboardTipsFrame(void);
	~StallSignboardTipsFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual VOID Render3D();
	void ReleaseTexture(const tagStall* pItem);
private:
	VOID DrawItemTip( const tagStall* pItem );
	VOID DrawTip( const POINT &ptPos, const TCHAR* szText, const DWORD dwColor, const float z, const float scale );
	VOID DrawSignboard(ResTexture *pTex, const POINT &ptPos, const float z, const float scale );
private:
	IDepthFont*						m_pFont;
	std::map<BYTE, ResTexture*> m_mapTexs;

	std::vector<tagStall*> m_vecSortStall;
};
