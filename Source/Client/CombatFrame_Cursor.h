#pragma once
#include "gameframe.h"


/** \class CombatFrame_Cursor
	\brief π‚±Íœ‘ æ
*/
class CombatFrame_Cursor :
	public GameFrame
{
public:
	CombatFrame_Cursor(void);
	virtual ~CombatFrame_Cursor(void);

	//--GameFrame
	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual VOID Update();

private:
	DWORD	m_lastRefleshTime;
};
