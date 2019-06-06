#pragma once

struct tagGroundItem;

/** \class  GroundItemTipsFrame
	\brief  µÙ¬‰ŒÔ∆∑∆¡œ‘
*/
class GroundItemTipsFrame : public GameFrame
{
public:
	GroundItemTipsFrame(void);
	virtual ~GroundItemTipsFrame(void);

	// GameFrame
	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual VOID Update();
	virtual VOID Render3D();

private:
	VOID DrawItemTip( const tagGroundItem* pItem );
	VOID DrawTip( const Vector3& vPos, const TCHAR* szText, const DWORD dwColor );
private:
	IDepthFont*						m_pFont;
	TObjRef<InputDX8>				m_pInput;
};
