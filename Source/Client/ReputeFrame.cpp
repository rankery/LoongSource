#include "StdAfx.h"
#include "ReputeFrame.h"
#include "CharacterFrame.h"
#include "ProgressEx.h"
#include <iomanip>

/**
	*struct ReputeGUIs
	*brief 八个氏话声望的结构体，包括，进度条，声望级别，贡献值显示框，名人显示框
	*author hyu
	*remark
*/
struct ReputeFrame::tagReputeGUICollect
{
	tagReputeGUICollect() : pPrgrssReputeValue(NULL), 
		pStcReputeLevel(NULL),
		pStcContrib(NULL),
		pStcHero(NULL) {}

	ProgressEx*			pPrgrssReputeValue;
	GUIStatic*			pStcReputeLevel;
	GUIStatic*			pStcContrib;
	GUIStatic*			pStcHero;
};

ReputeFrame::ReputeFrame(void) : m_Trunk(this),
	m_pWnd(NULL),
	m_pWndFather(NULL),
	m_pReputeGUICollect(NULL),
	m_bisFamousOpen(false),
    m_bneedRefreshRepute(false),
    m_timeFirst(0)
{
}

ReputeFrame::~ReputeFrame(void)
{
	SAFE_DELETEA(m_pReputeGUICollect);
}

BOOL ReputeFrame::Init( GameFrameMgr* pMgr, GameFrame* pFather, DWORD dwParam )
{
	m_pReputeGUICollect = new tagReputeGUICollect[ECLT_NUM];

	BOOL bRet = GameFrame::Init(pMgr, pFather, dwParam);
	
	/*! 注册游戏事件 */
	m_pFrameMgr->RegisterEventHandle(_T("FamousUIOpen"), (FRAMEEVENTPROC)m_Trunk.sfp1(&ReputeFrame::OnFamousUIOpen));
	m_pFrameMgr->RegisterEventHandle(_T("FamousUIClose"), (FRAMEEVENTPROC)m_Trunk.sfp1(&ReputeFrame::OnFamousUIClose));
    m_pFrameMgr->RegisterEventHandle(_T("ReputeChange"), (FRAMEEVENTPROC)m_Trunk.sfp1(&ReputeFrame::OnReputeChange));
    m_pFrameMgr->RegisterEventHandle(_T("tagBecomeEvent"), (FRAMEEVENTPROC)m_Trunk.sfp1(&ReputeFrame::OnBecomeEvent));
    m_pFrameMgr->RegisterEventHandle(_T("ClanContributeChange"), (FRAMEEVENTPROC)m_Trunk.sfp1(&ReputeFrame::OnRoleClanContributeChange));

	return bRet;
}

BOOL ReputeFrame::Destroy()
{
	SAFE_DELETEA(m_pReputeGUICollect);

	BOOL bRet = GameFrame::Destroy();
	
	/*! 注销游戏事件 */
	m_pFrameMgr->UnRegisterEventHandler(_T("FamousUIOpen"), (FRAMEEVENTPROC)m_Trunk.sfp1(&ReputeFrame::OnFamousUIOpen));
	m_pFrameMgr->UnRegisterEventHandler(_T("FamousUIClose"), (FRAMEEVENTPROC)m_Trunk.sfp1(&ReputeFrame::OnFamousUIClose));
    m_pFrameMgr->UnRegisterEventHandler(_T("ReputeChange"), (FRAMEEVENTPROC)m_Trunk.sfp1(&ReputeFrame::OnReputeChange));
    m_pFrameMgr->UnRegisterEventHandler(_T("tagBecomeEvent"), (FRAMEEVENTPROC)m_Trunk.sfp1(&ReputeFrame::OnBecomeEvent));
    m_pFrameMgr->UnRegisterEventHandler(_T("ClanContributeChange"), (FRAMEEVENTPROC)m_Trunk.sfp1(&ReputeFrame::OnRoleClanContributeChange));

	return bRet;
}

BOOL ReputeFrame::ReloadUI()
{
	// 创建
	m_pWndFather = m_pGUI->GetWnd(((CharacterFrame*)m_pFather)->GetGUIFullName().c_str());
	m_pWnd = m_pWndFather->GetChild(_T("repute"));
	m_pBtEnterFameHall = (GUIButton*)m_pWnd->GetChild(_T("herolist"));

	for (int i = 0; i < ECLT_NUM; ++i)
	{
		TCHAR cbuff1[128];
		TCHAR cbuff2[128];
		int num = i + 1;
		_sntprintf(cbuff1, sizeof(cbuff1)/sizeof(TCHAR), _T("%s%d"), _T("propic"), num);
		_sntprintf(cbuff2, sizeof(cbuff2)/sizeof(TCHAR), _T("%s%d"), _T("pro"), num);
		m_pReputeGUICollect[i].pPrgrssReputeValue = (ProgressEx*)m_pWnd->GetChild(_T("reputebackpic"))->GetChild(cbuff1)->GetChild(cbuff2);
		m_pReputeGUICollect[i].pPrgrssReputeValue->SetMaxValue(100);
		m_pReputeGUICollect[i].pPrgrssReputeValue->SetSpeed(100000000);

		_sntprintf(cbuff2, sizeof(cbuff2)/sizeof(TCHAR), _T("%s%d\\%s%d"), _T("pro"), num, _T("extent"), num);
		m_pReputeGUICollect[i].pStcReputeLevel = (GUIStatic*)m_pWnd->GetChild(_T("reputebackpic"))->GetChild(cbuff1)->GetChild(cbuff2);

		_sntprintf(cbuff1, sizeof(cbuff1)/sizeof(TCHAR), _T("%s%d"), _T("num"), num);
		m_pReputeGUICollect[i].pStcContrib = (GUIStatic*)m_pWnd->GetChild(_T("reputebackpic"))->GetChild(cbuff1);

		_sntprintf(cbuff1, sizeof(cbuff1)/sizeof(TCHAR), _T("%s%d"), _T("hero"), num);
		m_pReputeGUICollect[i].pStcHero = (GUIStatic*)m_pWnd->GetChild(_T("reputebackpic"))->GetChild(cbuff1);
	}

	// 注册事件处理函数
	m_pGUI->RegisterEventHandler(m_pWnd->GetFullName().c_str(), m_Trunk.sfp1(&ReputeFrame::EventHandler));

	/* 创建名人堂Frame */
	m_pFameHallFrame = (FameHallFrame*)m_pFrameMgr->CreateFrame(m_strName.c_str(), _T("FameHall"), _T("FameHallFrame"), 0);
	if (!P_VALID(m_pFameHallFrame))
		IMSG(_T("Create FameHallFrame failed"));
	
	return TRUE;
}

VOID ReputeFrame::Update()
{
    if (m_pWnd->IsInvisible())
        return;

    if (m_bneedRefreshRepute && Kernel::Inst()->GetAccumTimeDW() - m_timeFirst > 50)
    {
        m_pWnd->SetRefresh(TRUE);
        m_bneedRefreshRepute = false;
    }
}

DWORD ReputeFrame::EventHandler( tagGUIEvent* pEvent )
{
	GUIWnd* pWnd = m_pGUI->GetWnd(pEvent->dwWndID);

	switch (pEvent->eEvent)
	{
	case EGUIE_Click:
		if (pWnd == m_pBtEnterFameHall)
		{

			/*! 打开/关闭名人堂窗体 */
			if (m_bisFamousOpen)
				m_pFrameMgr->SendEvent(&tagGameEvent(_T("SetFamousClose"), this));
			else
				m_pFrameMgr->SendEvent(&tagGameEvent(_T("SetFamousOpen"), this));

		}
		break;
	}
	return 0;
}

VOID ReputeFrame::UpdateRepute()
{
	
	for (INT i = ECLT_BEGIN; i < ECLT_END; ++i)
	{
		/*! 读取声望级别 */
		const tagReputeLevel* pReputeLevel = ReputeMgr::Inst()->GetReputeLevel((ECLanType)i);
		/*! 读取声望值 */
		INT nRepute = ReputeMgr::Inst()->GetReputeValue((ECLanType)i);
		/*! 设置颜色 */
		ReputeMgr::Inst()->UlitySetPrgrssColor(m_pReputeGUICollect[i].pPrgrssReputeValue, pReputeLevel->eColor);
		/*! 设置状态条数据 */
		m_pReputeGUICollect[i].pPrgrssReputeValue->SetMaxValue((FLOAT)pReputeLevel->nDiffValue);
		m_pReputeGUICollect[i].pPrgrssReputeValue->SetValue((FLOAT)nRepute - pReputeLevel->nFrom);
		/*! 设置贡献值 */
		TCHAR szTemp[128];
		_itot(CurrencyMgr::Inst()->GetClanContribute((ECLanType)i), szTemp, 10);
		m_pReputeGUICollect[i].pStcContrib->SetText(szTemp);
        /*! 设置名人标记 */
        LPCTSTR szFame = ReputeMgr::Inst()->IsFame((ECLanType)i) ? g_StrTable[_T("ClanFame")] : _T("");
        m_pReputeGUICollect[i].pStcHero->SetText(szFame);
        /*! 设置级别文字 */
        tstringstream str;
        str << pReputeLevel->szLevel << _T(" ") << setw(6) << ReputeMgr::Inst()->GetReputeValue(static_cast<ECLanType>(i));
        m_pReputeGUICollect[i].pStcReputeLevel->SetText(str.str().c_str());
	}
	
    m_bneedRefreshRepute = true;
    m_timeFirst = Kernel::Inst()->GetAccumTimeDW();
}

DWORD ReputeFrame::OnFamousUIClose( tagGameEvent* pMsg )
{
	m_bisFamousOpen = false;
	return 0;
}

DWORD ReputeFrame::OnFamousUIOpen( tagGameEvent* pMsg )
{
	m_bisFamousOpen = true;
	return 0;
}

DWORD ReputeFrame::OnReputeChange( tagReputeChange* pMsg )
{
    UpdateRepute();
	return 0;
}

DWORD ReputeFrame::OnBecomeEvent( tagBecomeEvent* pMsg )
{
    UpdateRepute();

	return 0;
}

DWORD ReputeFrame::OnRoleClanContributeChange( tagRoleClanContributeChange* pMsg )
{
    UpdateRepute();

	return 0;
}