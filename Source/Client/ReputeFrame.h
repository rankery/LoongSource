#pragma once

#include "CurrencyMgr.h"
#include "ReputeMgr.h"
#include "ProgressEx.h"
#include "..\WorldDefine\reputation.h"
#include "..\WorldDefine\msg_famehall.h"

class FameHallFrame;

/*!
	\class ReputeFrame
	\brief 声望系统界面
	\author hyu
	\base GameFrame
	\remark
*/
class ReputeFrame : public GameFrame
{
public:
	ReputeFrame(void);
	~ReputeFrame(void);

	virtual BOOL Init(GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam);
	virtual BOOL Destroy();
	virtual BOOL ReloadUI();
	virtual VOID Update();
	
	//刷新声望显示
	VOID UpdateRepute();

private:
	

	DWORD OnGameEvent(tagGameEvent* pGameEvent);

	DWORD OnFamousUIOpen(tagGameEvent* pMsg);
	DWORD OnFamousUIClose(tagGameEvent* pMsg);

    DWORD OnReputeChange(tagReputeChange* pMsg);
    DWORD OnBecomeEvent(tagBecomeEvent* pMsg);
    DWORD OnRoleClanContributeChange(tagRoleClanContributeChange* pMsg);

	DWORD EventHandler(tagGUIEvent* pEvent);

	TSFPTrunk<ReputeFrame>		m_Trunk;
	TObjRef<GUISystem>			m_pGUI;
	TObjRef<Util>				m_pUtil;
	TObjRef<GameFrameMgr>		m_pFrameMgr;
	TObjRef<NetSession>			m_pSession;

	//GUI控件
	GUIWnd*						m_pWnd;
	GUIWnd*						m_pWndFather;

	GUIButton*					m_pBtEnterFameHall;

	struct tagReputeGUICollect;
	tagReputeGUICollect* m_pReputeGUICollect;

	/*! 其它 */
	bool						m_bisFamousOpen;	/*!< 名人堂窗体状态 */
	FameHallFrame*				m_pFameHallFrame;	/*!< 名人堂窗体frame */

    bool                        m_bneedRefreshRepute;
    DWORD                       m_timeFirst;
};
